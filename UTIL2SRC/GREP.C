/*============================================================================*
 * main() module: grep.c - Get regular expression and print.      OS/2 2.0
 *
 * (C)Copyright IBM Corporation, 1991, 1992, 1993         Brian E. Yoder
 *
 * The grep (get regular expression and print) command is an adaptation of
 * the AIX grep command.  It searches one or more files for a pattern.
 *
 * See grep.doc for a complete description.
 *
 * 10/28/91 - Created for DOS and OS/2.
 * 11/04/91 - Initial version.
 * 07/24/92 - Changed "rt" to "r" for fopen(). C Set/2 doesn't allow "rt".
 * 09/28/93 - In scanfile(): If fopen fails, print error msg to stderr.
 * 09/30/93 - Use functions in lbuf.c to allow binary files to be scanned.
 *============================================================================*/

static char copr[] = "(c)Copyright IBM Corporation, 1993.  All rights reserved.";

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/types.h>

#include "util.h"

#define MAXLINE 512                 /* Maximum line length supported */

static  char     buff[2048+1];      /* Buffer for output filenames */

#define ELEN     1024               /* Length of compiled pattern buffer */
static  char     cexpr[ELEN+1];     /* Buffer for expanded/compiled pattern */

/*----------------------------------------------------------------------------*
 * Structure for grep run-time control information
 *----------------------------------------------------------------------------*/

typedef struct {

     int        ExactCase;          /* Exact case match? */
     int        StdIn;              /* Reading from stdin? */
     int        MatchType;          /* What to display: RMATCH or NO_RMATCH */

     int        ShowFilesOnly;      /* Show only filenames? */
     int        ShowCountOnly;      /* Show only count of matching lines? */
     int        ShowLines;          /* Show lines? */

     int        PrefixFilename;     /* Prefix output lines with filename? */
     int        PrefixLineNumber;   /* Prefix output lines with line number? */

     ulong      mcount;             /* Match count across all files */
//   char      *iobuff;             /* Pointer to I/O buffer for reading */
     LBUF      *istream;            /* Pointer to buffered input stream */
     uint       iolen;              /* Length of io buffer */

} RUNINFO;

static RUNINFO rinfo = {            /* Initial settings of run-time info */

     TRUE  , FALSE , RMATCH ,
     FALSE , FALSE , TRUE  ,
     TRUE  , FALSE ,

     0L, NULL, 16384                     /* 8192, 16384 : Multiples of 512 */
};

/*============================================================================*
 * Define internal functions to allow forward access
 *============================================================================*/

static void   syntax    ( void );

static int  expandpat   ( char *expr, char *rexpr, char *erexpr, char **endp );
static void scanfile    ( char *fname, LBUF *istream,
                          char *cexpr, RUNINFO *rinfo );
static void showline    ( char *fname, ulong linenum, char *fline,
                          RUNINFO  *rinfo );

/*============================================================================*
 * Main Program Entry Point
 *============================================================================*/

main(argc, argv)

int argc;                           /* arg count */
char **argv;                        /* arg pointers */

{
  int     rc;                       /* Return code storage */
  long    lrc;                      /* Long return code */
  char   *flagstr;                  /* Pointer to string of flags */

  char   *pattern;                  /* Pattern, as entered by user */
  char   *next_eexpr;               /* Returned by rscan(), but not used */
  char   *next_cexpr;

  SLPATH *speclist;                 /* Pointer to linked list of path specs */
  ushort  mflags;                   /* Match flags for slrewind() */
  ushort  recurse;                  /* Recursive directory descent? */

  FINFO  *fdata;                    /* Pointers returned by slmatch() */
  SLPATH *pdata;
  SLNAME *ndata;

  char    eexpr[ELEN+1];            /* Buffer for expanded pattern */

 /*---------------------------------------------------------------------------*
  * Process flags from the command line, if any.  Store information from the
  * flags (except for -s: recurse subdirectories) in the rinfo structure.
  *---------------------------------------------------------------------------*/

  recurse = FALSE;                  /* Initially: don't descend subdirectories */

  argc--;                           /* Ignore 1st argument (program name) */
  argv++;

  while (argc != 0)                 /* For each argument: */
  {
     flagstr = *argv;                  /* Point 'flagstr' to argument */
     if (*flagstr != '-')              /* If it doesn't begin with '-': */
        break;                              /* Then it's the first filespec */
     else                              /* Otherwise: It's a set of flags: */
     {
        flagstr++;                          /* Point past the dash */

        if (*flagstr == '?')                /* If help is requested: */
           syntax();                             /* Show help */

        while (*flagstr != '\0')            /* For each character in flag string: */
        {
           switch (*flagstr)
           {
              case '?':                          /* If we see ? in the list: */
                 syntax();                       /* then user wants help */
                 return(2);
                 break;

              case 's':
              case 'S':
                 recurse = TRUE;
                 break;

              case 'i':
              case 'I':
              case 'y':
              case 'Y':
                 rinfo.ExactCase = FALSE;
                 break;

              case 'c':
              case 'C':
                 rinfo.ShowCountOnly = TRUE;
                 rinfo.ShowFilesOnly = FALSE;
                 rinfo.ShowLines = FALSE;
                 break;

              case 'l':
              case 'L':
                 rinfo.ShowFilesOnly = TRUE;
                 rinfo.ShowCountOnly = FALSE;
                 rinfo.ShowLines = FALSE;
                 break;

              case 'n':
              case 'N':
                 rinfo.PrefixLineNumber = TRUE;
                 break;

              case 'v':
              case 'V':
                 rinfo.MatchType = NO_RMATCH;
                 break;

              default:
                 fprintf(stderr, "grep: Invalid flag '%c'.\n",
                    *flagstr);
                 exit(2);
                 break;
           }
           flagstr++;                            /* Check next character */
        } /* end of while stmt to process all flag characters in current argv */

        argc--;                             /* Done with flags: Discard them */
        argv++;
     } /* end of if stmt to process flag string */
  } /* end of while stmt to process all flag strings */

  if (argc < 1) syntax();            /* If no arguments: Display syntax */

 /*---------------------------------------------------------------------------*
  * Allocate a read buffer to be used by fgets()
  *---------------------------------------------------------------------------*/

  rinfo.istream = newlbuf(rinfo.iolen);
  if (rinfo.istream == NULL)
  {
     fprintf(stderr, "grep: Cannot allocate memory for istream.\n");
     exit(2);
  }

 /*---------------------------------------------------------------------------*
  * Compile pattern and store pointer to it.  If we are not matching exact
  * case, then we need to first convert the pattern to uppercase before we
  * compile it.
  *---------------------------------------------------------------------------*/

  pattern = *argv;                  /* Point to user-entered pattern */
  argc--;                           /* Bump args past pattern */
  argv++;

  if (rinfo.ExactCase == FALSE)     /* If matches are case-insensitive: */
     strupr(pattern);                    /* Convert pattern to uppercase */

  rc = expandpat(pattern, eexpr,    /* Expand the pattern */
                 &eexpr[ELEN+1],
                 &next_eexpr);
  if (rc != 0)
  {
     fprintf(stderr, "grep: Cannot expand pattern into regular expression.\n");
     return(2);
  }

  rc = rcompile(eexpr, cexpr,       /* Compile the regular expression */
                &cexpr[ELEN+1],
                '\0', &next_cexpr);
  if (rc != 0)
  {
     fprintf(stderr, "grep: Cannot compile pattern: %s\n",
        rcmpmsg(rc));
     return(2);
  }

//printf("User-entered pattern: %s\n", pattern);   /* Test */
//printf("Compiled expression:  %s\n", eexpr);
//return(0);

 /*---------------------------------------------------------------------------*
  * If there are no filespecs, then scan standard input
  *---------------------------------------------------------------------------*/

  if (argc == 0)                    /* If no file specifications present: */
  {
     rinfo.StdIn = TRUE;                 /* Set standard input flag */
     rinfo.PrefixFilename = FALSE;       /* There's no filename to prefix! */
     scanfile(NULL, rinfo.istream,       /* Scan stdin for compiled expression */
              cexpr, &rinfo);
     goto ScanDone;                      /* Bypass filespec scanning */
  }

 /*---------------------------------------------------------------------------*
  * Process file specifications and build specification list
  *---------------------------------------------------------------------------*/

  rc = slmake(&speclist, TRUE, TRUE, argc, argv);

  if (rc != 0)                      /* If there was an error: */
  {                                 /* Analyze rc, show msg, and return */
     if (rc == REGX_MEMORY)
        fprintf(stderr, "Out of memory while processing '%s'.\n",
           slerrspec());
     else
        fprintf(stderr, "Invalid filespec: '%s'.\n",
           slerrspec());

     return(2);
  }

 /*---------------------------------------------------------------------------*
  * Attempt to match files to the specification list
  *---------------------------------------------------------------------------*/

  mflags = 0;                       /* Set list of files to match to include: */
  mflags = mflags | SL_NORMAL;           /* Ordinary files */
// no!              SL_HIDDEN |          /* Hidden files */
// no!              SL_SYSTEM;           /* System files */

  slrewind(speclist, mflags, recurse);/* Initialize slmatch() */
  for (;;)                          /* Loop to find all matching files: */
  {
     fdata = slmatch(&pdata,             /* Get next matching file */
                     &ndata);
     if (fdata == NULL)                  /* If none: */
        break;                                /* Done */

     strcpy(buff, pdata->path);          /* Store path portion of filename */
     pathcat(buff, fdata->Fname);        /* Add name to end of path string */

     scanfile(buff, rinfo.istream,       /* Scan file for compiled expression */
              cexpr, &rinfo);

  } /* end of for(;;) loop to find matching files */

 /*---------------------------------------------------------------------------*
  * Display (on stderr) all filespecs that had no matching files
  *
  * For grep, we don't currently care about files that we couldn't match.
  * If I find out that the AIX version does, then I will too.
  *---------------------------------------------------------------------------*/

//lrc = slnotfound(speclist);       /* Display path\names w/no matching files */
//if (lrc == 0L)                    /* If all fspecs were matched: */
//   return(0);                          /* Return successfully */
//else                              /* Otherwise:  One or more not found: */
//   return(2);                          /* Return with error */


 /*---------------------------------------------------------------------------*
  * Done: See if we have a count to display
  *---------------------------------------------------------------------------*/

ScanDone:
  if (rinfo.ShowCountOnly == TRUE)
     printf("%lu\n", rinfo.mcount);

  if (rinfo.mcount == 0L)           /* If no matches were found: return 1 */
     return(1);

  return(0);                        /* Matches were found: return 0 */

} /* end of main() */

/*============================================================================*
 * syntax() - Display command syntax and exit to operating system!
 *============================================================================*/
static void syntax()
{
  fprintf(stderr, "\n");
  fprintf(stderr, "Usage: grep [-flags] \"pattern\" [fspec ...]\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "The grep command searches the specified files for the \"pattern\"\n");
  fprintf(stderr, "and writes lines that contain the pattern to standard output.\n");
  fprintf(stderr, "If no files are listed, it reads lines from standard input.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Flags:\n");
  fprintf(stderr, "   c  Display only a count of matching lines.\n");
  fprintf(stderr, "   i  Ignore case when making comparisons.\n");
  fprintf(stderr, "   l  List just the names of files (once) with matching lines.\n");
  fprintf(stderr, "   n  Preceed each line with the line number, along with the file name.\n");
  fprintf(stderr, "   s  Descend subdirectories, also.\n");
  fprintf(stderr, "   v  Display lines that don't contain the pattern.\n");
  fprintf(stderr, "   y  Ignore case when making comparisons.\n");
  exit(1);
}

/*============================================================================*
 * expandpat() - Expand pattern into a regular expression.
 *
 * REMARKS:
 *   This subroutine creates a regular expression from the user-entered
 *   pattern.  It is more convenient to enter repeat factors within
 *   braces without having to escape the braces.  However, the rcompile()
 *   subroutine in rxpm.c requires braces in {m.n} to be escaped and
 *   otherwise treats braces as-is.
 *
 * RETURNS:
 *   0, if successful.
 *   REGX_FULL, if the expanded expression won't fit in the buffer.
 *
 *   If successful, 'endp' contains a pointer to the location that is one byte
 *   past the null terminating byte of the expanded expression.  If error,
 *   then 'endp' contains a pointer to 'rexpr'.
 *============================================================================*/

#define STORE(ch) if (to >= erexpr) return(REGX_FULL); *to++ = ch;

static int expandpat(

char     *expr,                    /* Pointer to user-entered pattern */
char     *rexpr,                   /* Pointer to expansion buffer */
char     *erexpr,                  /* Pointer past end of expansion buffer */
char    **endp )                   /* Pointer to loc. in which to store pointer */
                                   /* to byte just past expanded expression */

{
  char *from;                      /* Character pointers used to expand the */
  char *to;                        /*    expression */

  from = expr;                     /* Point to start of each expression */
  to = rexpr;

  *endp = rexpr;                   /* Initialize 'endp' to 'rexpr */

  if (rexpr >= erexpr)             /* If end pointer doesn't allow at least */
     return(REGX_FULL);            /* one character: return w/error */

  while (*from != '\0')            /* For each character in 'expr': */
  {
     switch (*from)                    /* Handle special cases: */
     {
        case '{':                            /* Put '\' before */
        case '}':                            /* these characters: */
           STORE('\\');
           STORE(*from);
           break;

        case '\\':                           /* If escape character found: */
           from++;                                /* Point to next character */
           switch (*from)
           {
              case '{':                           /* Store these characters */
              case '}':                           /* without the escape char: */
                 STORE(*from);
                 break;

              default:                            /* Otherwise: store the esc */
                 STORE('\\');                     /* character, too */
                 STORE(*from);
                 break;
           }
           break;

        default:                             /* For all other characters: */
           STORE(*from);                          /* Just copy character */
           break;
     }

     from++;                           /* Bump pointer to next source char */
  }

  STORE('\0');                     /* Store null after the expanded expression */
  *endp = to;                      /* Store pointer past the null byte */
  return(0);
}

/*============================================================================*
 * scanfile() - Scan a file for a regular expression.
 *
 * REMARKS:
 *   This subroutine scans the specified file for the compiled regular
 *   expression.  If the StdIn flag is TRUE, then it scans standard input
 *   and ignores the filename.
 *
 *   This subroutine calls showline() to actually display the results
 *   of the scan.  It also updates rinfo->mcount with the number of
 *   matching/non-matching lines (as appropriate) that were found.
 *
 * RETURNS:
 *   Nothing, for now.
 *
 * NOTE:
 *   This subroutine calls lgets() from lbuf.c to get lines from the file.
 *   The setlbuf() closes any previously opened file so we don't have to
 *   explicitly close the file as we did when we called fgets().
 *============================================================================*/

static void scanfile(

char     *fname,                   /* Pointer to file name */
LBUF     *istream,                  /* Pointer to buffered input stream */
char     *cexpr,                   /* Pointer to compiled regular expression */
RUNINFO  *rinfo )                  /* Pointer to run-time information */

{
  int     rc;                      /* Return code storage */

  int     fd;                       /* File descriptor */

  ulong   linenum;                 /* Current line's line number */
  int     newline;                 /* Did previous line end with \n ? */
  char   *line;                    /* Pointer to line */
  char   *end;                     /* Pointer to end of line */

  int     match;                   /* Result from rscan() */
  char   *mstart;                  /* From rscan(): Pointer to start of match */
  char   *mend;                    /*               Pointer past end of match */
  int     found;                   /* TRUE, if file contains a match */

  char    fline[MAXLINE+1];        /* Original line from file */
  char    uline[MAXLINE+1];        /* Uppercased line, if case-insensitive matches */

 /*---------------------------------------------------------------------------*
  * Set up buffered stream, opened for reading
  *---------------------------------------------------------------------------*/

  if (rinfo->StdIn == TRUE)        /* If reading from standard input: */
  {
     fd = 0;                            /* Store file handle of stdin */
     fname = "";                        /* Set file name to empty string */
  }
  else                             /* Otherwise: */
  {
     fd = open(fname, O_RDONLY | O_BINARY); /* Open the file in binary: */
     if (fd == -1)                      /* lgets() will handle it ok */
     {
        fprintf(stderr, "grep: Cannot open file %s.\n", fname);
        return;
     }
  }

  setlbuf(istream, fd);            /* Attach opened file to buffered stream */

 /*---------------------------------------------------------------------------*
  * Scan the file for the regular expression
  *---------------------------------------------------------------------------*/

  newline = TRUE;                   /* Initialize for loop */
  linenum = 0L;
  found = FALSE;

  for (;;)                          /* Loop to scan the file */
  {
    /*-------------------------------------------------------*
     * Get the next chunk from the file, and update the current
     * line number if the previous chunk ended with \n
     *-------------------------------------------------------*/

     line = lgets(fline, MAXLINE,        /* Get next chunk from file */
                  rinfo->istream);
     if (line == NULL) break;            /* If no more data: quit loop */

     if (newline == TRUE)                /* If previous line ended with \n: */
     {
        linenum++;                            /* We are on the next line */
        newline = FALSE;                      /* Reset the flag */
     }

     end = line + strlen(line) - 1;      /* Point to last character in line */
     if (*end == '\n')                   /* If last char is a new-line: */
     {
        *end = '\0';                          /* Get rid of it, and */
        newline = TRUE;                       /* set flag */
     }

    /*-------------------------------------------------------*
     * If we are performing case-insensitive matches, then we
     * need to uppercase the line.  We copy it to another
     * buffer because if we show the line, we want to show the
     * original unaltered version.
     *-------------------------------------------------------*/

     if (rinfo->ExactCase == FALSE)      /* If case-insensitive matching: */
     {
        strcpy(uline, fline);                 /* Make a copy of the line */
        strupr(uline);                        /* Convert copy to uppercase */
        line = uline;                         /* Point 'line' to the copy */
     }

    /*-------------------------------------------------------*
     * Set 'match' to result of scanning the line for the
     * compiled regular expression.
     *-------------------------------------------------------*/

     match = rscan(cexpr, line, &mstart, &mend);

    /*-------------------------------------------------------*
     * Depending upon 'match' and 'rinfo' flags, show
     * the result of the scan on stdout, and update the
     * 'rinfo->mcount' field.
     *-------------------------------------------------------*/

     if (match == rinfo->MatchType)      /* If match is what we want to show: */
     {
        rinfo->mcount++;                      /* Update match count */
        found = TRUE;                         /* We found what we wanted */
        if (rinfo->ShowLines == TRUE)         /* If we're to show the line: */
           showline(fname, linenum,                /* The show it */
                    fline, rinfo);
     }

  } /* end of for(;;) loop to scan the file */

 /*---------------------------------------------------------------------------*
  * If we're only showing filenames and we found what we were looking for,
  * then write the file name to stdout
  *---------------------------------------------------------------------------*/

  if ((found == TRUE) &&
      (rinfo->ShowFilesOnly == TRUE))
          puts(fname);

 /*---------------------------------------------------------------------------*
  * Done: return
  *---------------------------------------------------------------------------*/

  return;
}

/*============================================================================*
 * showline() - Show the line.
 *
 * REMARKS:
 *   This subroutine shows the specified line.  Depending upon the flags
 *   in rinfo, various decorations are added to the lines.
 *
 *   We assume that, by getting called, the line is to be shown.  It is
 *   a bit faster if the caller checks the rinfo->ShowLines flag and only
 *   call us if it's TRUE rather than calling us for each and every line
 *   and letting us check the flag.
 *
 * RETURNS:
 *   Nothing.
 *============================================================================*/

static void showline(

char     *fname,                   /* Pointer to file name */
ulong     linenum,                 /* Line number */
char     *fline,                   /* Pointer to line string */
RUNINFO  *rinfo )                  /* Pointer to run-time information */

{
  switch (rinfo->PrefixFilename)   /* Determine the prefix information to */
  {                                /* show before the line itself: */
     case TRUE:
        switch (rinfo->PrefixLineNumber)
        {
           case TRUE:
              printf("%s(%lu) : %s\n",         /* Filename + line number */
                 fname, linenum, fline);
              break;

           default:
              printf("%s : %s\n",              /* Filename only */
                 fname, fline);
              break;
        }
        break;

     default:
        switch (rinfo->PrefixLineNumber)
        {
           case TRUE:
              printf("(%lu) : %s\n",           /* Linenumber only */
                 linenum, fline);
              break;

           default:
              printf("%s\n",                   /* No prefix information */
                 fline);
              break;
        }
        break;
  }

  return;
}
