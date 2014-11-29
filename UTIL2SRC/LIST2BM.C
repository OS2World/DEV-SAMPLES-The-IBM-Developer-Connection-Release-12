/*============================================================================*
 * main() module: list2bm.c - Convert list of files to a BookMaster document.
 *
 * (C)Copyright IBM Corporation, 1991, 1992.                     Brian E. Yoder
 *
 * This program reads a list file and writes a BookMaster document to stdout.
 * The list file specifies the document title, author, security classification,
 * and a list of other files to include in the output document.  The output
 * document can be transmitted to host system and processed using SCRIPT and
 * BookMaster.
 *
 * The list file is a text file that is interpreted as follows:  Blank lines
 * are ignored.  Text that follows a pound sign (#) is assumed to be a comment
 * and is ignored.  The remaining non-blank, non-comment lines contain one or
 * two space-delimited tokens.
 *
 * If a token contains embedded blanks, you must enclose it in either double
 * quotes or single quotes:  "This a string"  or 'This is a string'.
 *
 * If a token begins with a $, then it is assumed to be the name of an
 * environment variable.  For example, if the date is stored in the 'DATE'
 * variable, then you can specify $DATE as the token.
 *
 * This program expects the first non-blank, non-comment lines to be formatted
 * as follows (to add more of these, update this comment header, and the
 * tpage() and bmtitle() subroutines):
 *
 *    -sec     "Security Classification"
 *    -date    "Date of Document"
 *    -author  "Author of Document"
 *    -title   "Title of Document"
 *
 * The remaining lines may be zero or more of each of the following, in any
 * order you choose:
 *
 *    -section "Title of Section"
 *    filename
 *
 * The -sec, -date, -author, and -title lines must be the first lines in the
 * input file.  The rest of the lines specify section headings and files to
 * be included in the output document.
 *
 * Each -section line adds a major section (:h0.) heading to the output
 * document.
 *
 * Each 'filename' line adds the named file in its own :h1. heading in the
 * output document -- the heading name is set to 'filename'.  The program
 * converts any special characters to BookMaster symbols, so you can include
 * brackets, braces, box characters, and other special characters in the
 * document.
 *
 * 03/25/91 - Created.
 * 03/25/91 - Initial version: Requires the untab and 't2bm' utilities in the
 *            current path.
 * 03/29/91 - Don't put '.gs tag off/on' in output: we are already changing
 *            periods to '&period'.  The '.gs tag off' was left over from the
 *            AIX shell script version of this program.
 *            Also, support the -n option: 'No title page'.  This allows the
 *            output to contain just the -sections and files in the list,
 *            allowing the output file to be included within another document.
 * 04/04/91 - Ported from AIX to DOS and C/2.  We won't execute untab and t2bm
 *            (too slow on DOS), but instead will call text2bm() directly.
 *            If there is any tab expansion to be done, it's left up to
 *            text2bm().  Also, output is directed to file instead of stdout.
 *            We also use static buffers instead of malloc'd buffers to store
 *            the title page parameter strings.
 * 10/08/91 - If a filename contains a drive specification, we won't write the
 *            'd:' to the heading.  This will make BookMaster happier, since
 *            it complains for certain combinations of 'd:fname'.  Also,
 *            the text2bm() subroutine now expands tabs for us: see the
 *            text2bm.c module!
 * 07/24/92 - Changed "rt" to "r" for fopen(). C Set/2 doesn't allow "rt".
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <io.h>              /* Required for access(), on DOS */
#include <sys/types.h>

#include "util.h"

#define MINARGS  2     /* Minimum no. of (non-flag) command line arguments required */

#define NO       0
#define YES      1

#define TBUFFLEN  64         /* Max. size of title page parm buffers */

static char   *ourname;      /* Pointer to our name, for use by all modules */

static char sep[]=           /* Separator line, for BookMaster document: */
".*======================================================================";

static int f_bldtp = YES;    /* Do we put the title page in the output? */

static FILE   *outfile;      /* Output file, used by output subroutines */

/*----------------------------------------------------------------------------*
 * Static buffers used for title page parameters
 *----------------------------------------------------------------------------*/

static  char   b_sec[TBUFFLEN+1];
static  char   b_date[TBUFFLEN+1];
static  char   b_author[TBUFFLEN+1];
static  char   b_title[TBUFFLEN+1];

/*----------------------------------------------------------------------------*
 * Static line buffer
 *----------------------------------------------------------------------------*/

#define LINELEN 1024               /* Maximum input file line length allowed */
static  char line[LINELEN + 1];    /* Input file line buffer */

/*============================================================================*
 * Define internal functions to allow forward access
 *============================================================================*/

static void       syntax();

static int        tpage        ( void );
static int        bmtitle      ( char *, char *, char *, char * );

static int        body         ( void );
static int        wrfile       ( char * );

static char      *getvalue     ( char * );

/*============================================================================*
 * Main Program Entry Point
 *============================================================================*/

main(argc, argv)

int argc;                           /* arg count */
char *argv[];                       /* arg pointers */

{
  int     numargs;                  /* No. of arguments processed, not incl. -flags */

  int     rc;                       /* Return code store area */
  char    cf;                       /* Current flag character (if any) */

  char   *listname;                 /* Pointer to name of input file */
  char   *outname;                  /* Pointer to name of output file */

/*============================================================================*
 * Set initial value of some variables:
 *============================================================================*/

  ourname = "list2bm";    /* Hardcode program name */
  numargs = 0;            /* Initialize number of arguments processed = none */

/*============================================================================*
 * Process each command line argument:
 *============================================================================*/

  argc--;                 /* Ignore 1st argument (program name) */
  argv++;

  while (argc > 0)        /* For each command line argument: */
  {
    /*========================================================================*/
    if ((*argv)[0] == '-')    /*   If argument is a -flag: */
    {
       cf = (*argv)[1];       /* Set cf = flag character: */
       switch (cf)
       {
           case 'n':          /* -n */
                f_bldtp = NO;
                break;

           default:
                fprintf(stderr, "Unrecognized flag: '-%c'\n",
                   cf);
                return(1);
                break; }
    }   /* end if: flag processing */

    /*========================================================================*/

    else {          /* Argument is NOT a flag: */
       /*
        * The value 'numargs' indicates how many (non-flag) arguments have
        * been processed, and hence, what to do with the current (non-flag)
        * argument:
        * When argument[numargs] (0=first, 1=second, etc.) is processed,
        * be sure to increment 'numargs' so that successive arguments will
        * be processed.
       */
       switch (numargs)
       {
           case 0:                   /* Name of input list file */
                listname = *argv;
                numargs++;
                break;

           case 1:                   /* Name of output file */
                outname = *argv;
                numargs++;
                break;

           default:
                fprintf(stderr, "Extra argument: '%s'\n",
                   *argv);
                return(1);
                break; }
    }   /* end else: argument processing */

    argc--;                     /* Decrement command line argument counter */
    argv++;                     /* Point to next argument */

  }   /* end of command line argument processing loop */

  if (numargs < MINARGS) syntax(); /* If not enough arguments: Display syntax */
                                   /* Otherwise, continue */

/*============================================================================*
 * PROCESS THE LIST FILE AND WRITE THE OUTPUT DOCUMENT TO STDOUT
 *============================================================================*/

  rc = cfopen(listname);            /* Open the input list file */
  if (rc != 0)
  {
     fprintf(stderr, "%s: Cannot open file: %s\n",
        ourname, listname);
     return(1);
  }

  outfile = fopen(outname, "w");
  if (outfile == NULL)
  {
     fprintf(stderr, "%s: Cannot create output file: %s\n",
        ourname, outfile);
     return(1);
  }

  rc = tpage();                     /* Build the title page */
  if (rc != 0)
     return(1);

  rc = body();                      /* Build the body of the document */
  if (rc != 0)
     return(1);

  cfclose();                        /* Close the list file */
  return(0);                        /* Done with main(): Return */

} /* end of main() */

/*============================================================================*
 * syntax() - Display command syntax and exit to operating system!
 *============================================================================*/
static void syntax()
{
  fprintf(stderr, "Usage: list2bm [-n] listfile outfile\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "This program reads a list file and writes a BookMaster document\n");
  fprintf(stderr, "to outfile.  The first lines define the title page as follows:\n");
  fprintf(stderr, "   -sec     \"Security Classification\"\n");
  fprintf(stderr, "   -date    \"Date of Document\"\n");
  fprintf(stderr, "   -author  \"Author of Document\"\n");
  fprintf(stderr, "   -title   \"Title of Document\"\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "The remaining lines may be any combination of the following, in any\n");
  fprintf(stderr, "order you choose:\n");
  fprintf(stderr, "   -section \"Title of Section\"\n");
  fprintf(stderr, "   filename\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "For the lines that define the title page, you can specify\n");
  fprintf(stderr, "the name of an environment variable, preceeded by a $.  In this\n");
  fprintf(stderr, "case, the value of the environment variable is used.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "If -n is specified, then the title page lines are ignored and no\n");
  fprintf(stderr, "title page is written to the output.  In this case, the output file\n");
  fprintf(stderr, "has no :userdoc and :euserdoc tags.\n");
  exit(1);
}

/*============================================================================*
 * tpage() - Build title page.
 *
 * PURPOSE:
 *   Reads the first part of the input file and builds the title page for
 *   the output document.
 *
 * RETURNS:
 *   0, if successful.  1, if error.
 *============================================================================*/

static int tpage()

{
  int     rc;                       /* Return code storage */

  int     tokc;                     /* Number of tokens found in a line */
  char  **tokv;                     /* Pointer to array of token pointers */

  char   *val;                      /* Pointer to a string */

  char   *sec;                      /* Title page values */
  char   *date;
  char   *author;
  char   *title;

  short   f_sec;                    /* Flags: 0 == not set yet */
  short   f_date;
  short   f_author;
  short   f_title;

  f_sec     = 0;                    /* Set all flags to 0: Value not set yet */
  f_date    = 0;                    /* If a flag is 1, its corresponding value */
  f_author  = 0;                    /* string has been set. */
  f_title   = 0;

  if (f_bldtp == NO)                /* If no title page: */
     return(0);                     /* then return immediately! */

 /*---------------------------------------------------------------------------*
  * Loop to process the lines that define the title page of the document
  *---------------------------------------------------------------------------*/

  for (;;)                          /* For each line in the file: */
  {
     if ((f_sec    &                     /* See if all values have been set */
          f_date   &
          f_author &
          f_title) == 1)
         break;

     tokc = cfread(&tokv);               /* Read a line */
     if (tokc == 0)                      /* If no more lines: we found the end */
     {                                   /* of the file before we expected it */
        fprintf(stderr, "%s: End of file encountered before title page was defined.\n",
           ourname);
        return(1);
     }

     if (tokc != 2)
     {
        fprintf(stderr, "%s: Line %ld: Two tokens expected.\n",
           ourname, cfline());
        return(1);
     }

    /*------------------------------------------------------------------------*
     * The tokv[1] variable points to the 2nd token on the line.  Store in
     * the 'val' variable either a pointer to the token or a pointer to an
     * environment variable.
     *------------------------------------------------------------------------*/

     val = getvalue(tokv[1]);       /* Get string's value: See getvalue() */
     if (val == NULL)
     {
        fprintf(stderr, "%s: Line %ld: Cannot find environment variable '%s'.\n",
           ourname, cfline(), tokv[1]);
        return(1);
     }

    /*------------------------------------------------------------------------*
     * The tokv[0] variable points to the -sec, -author, etc. token.  The
     * 'val' variable points to its value (string or env. variable contents).
     * Set the correct flag indicating * that the tokv[0] value has been processed.
     * Also, copy string to a static buffer, since the original tokv[] array
     * is overwritten by each call to cfread().
     *------------------------------------------------------------------------*/

     if      (strcmp(tokv[0], "-sec") == 0)
     {
        sec = b_sec;
        strncpy(sec, val, TBUFFLEN);
        f_sec = 1;
     }

     else if (strcmp(tokv[0], "-date") == 0)
     {
        date = b_date;
        strncpy(date, val, TBUFFLEN);
        f_date = 1;
     }

     else if (strcmp(tokv[0], "-author") == 0)
     {
        author = b_author;
        strncpy(author, val, TBUFFLEN);
        f_author = 1;
     }

     else if (strcmp(tokv[0], "-title") == 0)
     {
        title = b_title;
        strncpy(title, val, TBUFFLEN);
        f_title = 1;
     }

     else
     {
        fprintf(stderr, "%s: Line %ld: Bad token while processing title page: '%s'.\n",
           ourname, cfline(), tokv[0]);
        return(1);
     }
    /*------------------------------------------------------------------------*
     * End of loop to process title page lines.
     *------------------------------------------------------------------------*/
  }

 /*---------------------------------------------------------------------------*
  * Title page lines are processed: Build and write the title page itself
  *---------------------------------------------------------------------------*/

  rc = bmtitle(sec,
               date,
               author,
               title);

  return(0);                        /* Return */
}

/*============================================================================*
 * bmtitle() - Build and write the title page.
 *
 * PURPOSE:
 *   Builds the title page and writes it to outfile.  Pass pointers to the
 *   required title page strings:  see the function definition.
 *
 * RETURNS:
 *   0, always.
 *============================================================================*/

static int bmtitle(sec, date, author, title)

char     *sec;                      /* Title page values */
char     *date;
char     *author;
char     *title;

{
  char   *varname;                  /* Pointer to env. var's name */

 /*---------------------------------------------------------------------------*
  * Debugging: Comment out if not needed.
  *---------------------------------------------------------------------------*/
/*
  printf("sec       = %s\n", sec         );
  printf("date      = %s\n", date        );
  printf("author    = %s\n", author      );
  printf("title     = %s\n", title       );
*/

  fprintf(outfile, "%s\n", sep);

  fprintf(outfile, ".* Process this file using BookMaster\n");
  fprintf(outfile, ".*\n");

  fprintf(outfile, ":userdoc sec='%s'\n", sec);
  fprintf(outfile, ":prolog.\n");
  fprintf(outfile, ".*\n");
  fprintf(outfile, ":docprof\n");
  fprintf(outfile, "    justify=yes\n");
  fprintf(outfile, "    ldrdots=yes\n");
  fprintf(outfile, "    layout=1\n");
  fprintf(outfile, "    duplex=no\n");
  fprintf(outfile, "    xrefpage=no\n");

  fprintf(outfile, ".*\n");
  fprintf(outfile, ":title.%s\n", title);
  fprintf(outfile, ":date.%s\n", date);
  fprintf(outfile, ":author.%s\n", author);

  fprintf(outfile, ".*\n");
  fprintf(outfile, ":eprolog.\n");
  fprintf(outfile, ":frontm.\n");
  fprintf(outfile, ":tipage.\n");
  fprintf(outfile, ":toc.\n");
  fprintf(outfile, ".*\n");

  fprintf(outfile, ":body.\n");

  return(0);                        /* Done */
}

/*============================================================================*
 * body() - Build the body.
 *
 * PURPOSE:
 *   Reads the rest of the input file and builds the body for the output
 *   document.
 *
 *   If a filename begins with 'd:' (drive specification), then the heading
 *   will contain only the part that follows: the drive portion will be
 *   removed.
 *
 * RETURNS:
 *   0, if successful.  1, if error.
 *============================================================================*/

static int body()

{
  int     rc;                       /* Return code storage */

  int     tokc;                     /* Number of tokens found in a line */
  char  **tokv;                     /* Pointer to array of token pointers */

  char   *val;                      /* Pointer to a string */
  char   *fname;                    /* Pointer to a file name */
  char   *fnamehd;                  /* Pointer to file name as shown in heading */

 /*---------------------------------------------------------------------------*
  * Loop to process the lines that define the body of the document
  *---------------------------------------------------------------------------*/

  for (;;)                          /* For the rest of the lines in the file */
  {
     tokc = cfread(&tokv);               /* Read a line */
     if (tokc == 0)                      /* If no more lines: we're done */
        break;

    /*------------------------------------------------------------------------*
     * Look for any -tags that define the body of the document.  If we don't
     * find any, then we will assume that the line defines a filename that
     * should be copied to outfile.
     *------------------------------------------------------------------------*/

     if      (strcmp(tokv[0], "-section") == 0)
     {
       /*---------------------------------------------------------------------*
        * Process -section specification:  Must have 2 tokens: -section "title"
        *---------------------------------------------------------------------*/
        if (tokc != 2)
        {
           fprintf(stderr, "%s: Line %ld: Invalid -section specification.\n",
              ourname, cfline());
        }

        fprintf(outfile, "%s\n", sep);
        fprintf(outfile, ":h0.%s\n", tokv[1]);
     }

     else if (*(tokv[0]) == '-')
     {
       /*---------------------------------------------------------------------*
        * Ignore all other lines whose first token starts with '-'
        *---------------------------------------------------------------------*/
     }

     else
     {
       /*---------------------------------------------------------------------*
        * Process filename.  The file needs to have tabs expanded and special
        * characters converted to BookMaster symbols before being copied to
        * outfile.
        *---------------------------------------------------------------------*/
        fname = tokv[0];
        fnamehd = fname;

        if ((strlen(fname) >= 3) && (fname[1] == ':'))
           fnamehd += 2;

        if (access(fname, R_OK) != 0)
        {
           fprintf(stderr, "%s: Line %ld: Cannot read file '%s'.\n",
              ourname, cfline(), fname);
        }
        else
        {
           fprintf(outfile, "%s\n", sep);
           fprintf(outfile, ":h1.%s\n", fnamehd);
       /*  fprintf(outfile, ":xmp keep=5.\n.gs tag off\n");   3/29/91 */
           fprintf(outfile, ":xmp keep=5.\n");
           fflush(outfile);

           rc = wrfile(fname);
           if (rc != 0)
           {
           fprintf(stderr, "%s: Line %ld: Error writing file '%s'.\n",
              ourname, cfline(), fname);
           return(1);
           }

       /*  fprintf(outfile, ".gs tag on\n:exmp.\n");          3/29/91 */
           fprintf(outfile, ":exmp.\n");
        } /* end of if we can't access the file */
     } /* end of else: filename processing */

  } /* end of for (;;) loop to process each line in the file */

 /*---------------------------------------------------------------------------*
  * Add separator line and end-of-document tag (only if there is a title page)
  *---------------------------------------------------------------------------*/

  if (f_bldtp == YES)
  {
     fprintf(outfile, "%s\n", sep);
     fprintf(outfile, ":euserdoc.\n");
  }

  return(0);                        /* Return */
}

/*============================================================================*
 * wrfile() - Write file contents to output file.
 *
 * PURPOSE:
 *   Reads the specified file, converts specical characters using the text2bm()
 *   subroutine, and writes it to the output file specified by the static
 *   'outfile' FILE pointer.
 *
 * RETURNS:
 *   0, if successful.  1, if error.
 *============================================================================*/

static int wrfile(fname)

char     *fname;                    /* Pointer to name of a text file */

{
  int     rc;                       /* Return code storage */
  FILE   *tfile;                    /* Input text file */
  char   *text;                     /* Pointer to text string */

  rc = 0;                           /* Initialize return code to: successful */

 /*---------------------------------------------------------------------------*
  * Open text file
  *---------------------------------------------------------------------------*/

  tfile = fopen(fname, "r");        /* Open the text file for reading */
  if (tfile == NULL)
     return(1);

 /*---------------------------------------------------------------------------*
  * Loop to read text file and write it to output file
  *---------------------------------------------------------------------------*/

  for (;;)
  {
     text = fgets(line, LINELEN,
                  tfile);
     if (text == NULL)
        break;

     rc = text2bm(line, outfile);
     if (rc != 0)
     {
       rc = 1;
       break;
     }
  }

 /*---------------------------------------------------------------------------*
  * Close text file and return
  *---------------------------------------------------------------------------*/

  fclose(tfile);                    /* Close the text file */
  return(rc);                       /* Return */
}

/*============================================================================*
 * getvalue() - Get a string value.
 *
 * PURPOSE:
 *   Gets the value for a given string.
 *
 * RETURNS:
 *   str, if the string 'str' does not begin with $, or:
 *   Pointer to enviroment variable, if 'str' begins with $, or:
 *   NULL, if 'str' begins with $, but the environment variable doesn't exist.
 *
 *   For example, if 'str' is "$VARNAME", then this subroutine returns a pointer
 *   to the value of the environment variable named VARNAME.  If VARNAME doesn't
 *   exist in the environment, then NULL is returned.
 *
 * NOTE:
 *   If not NULL, the pointer returned is either 'str' or a pointer returned
 *   by getenv().
 *============================================================================*/

static char *getvalue(str)

char    *str;                       /* Pointer to string */

{
  char   *varname;                  /* Pointer to env. var's name */

 /*---------------------------------------------------------------------------*
  * Ignore NULL pointer, zero-length string, and string that doesn't begin
  * with a '$':
  *---------------------------------------------------------------------------*/

  if (str == NULL)                  /* If str is NULL pointer: */
     return(NULL);                  /* Just return the NULL pointer */

  if (*str == '\0')                 /* If str points to zero-length string: */
     return(str);                   /* Just return it: it can't be an env var */

  if (*str != '$')                  /* If first char of str is NOT a $: */
     return(str);                   /* Then return str */

 /*---------------------------------------------------------------------------*
  * 'str' begins with $: it names an environment variable:
  *---------------------------------------------------------------------------*/

  varname = str + 1;                /* Point varname past the $ in str */

  if (*varname == '\0')             /* If varname is a zero-length string: */
     return(NULL);                  /* Then it can't name an environment var */

  return(getenv(varname));          /* Return pointer to variable's value */
}
