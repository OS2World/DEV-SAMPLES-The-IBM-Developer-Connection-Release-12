/*============================================================================*
 * main() module: ls.c - List directory.                         OS/2 version.
 *
 * (C)Copyright IBM Corporation, 1991, 1992, 1993                Brian E. Yoder
 *
 * This program is a loose adaptation of the AIX 'ls' command.  See the LS.DOC
 * file for usage information.
 *
 * This program builds a "specification list" using the subroutines in the
 * speclist.c file.  This is done for performance reasons: we will only make
 * one pass through a directory: See the speclist.c file for more information.
 *
 * 04/04/91 - Created.
 * 04/15/91 - Initial version.
 * 04/16/91 - Version 1.1 - Added copyright and version strings.
 * 04/17/91 - Version 1.2 - Internal changes to speclist.c functions.
 * 04/30/91 - Version 1.3 - Ported from DOS to OS/2.  I made showfile()'s
 *            attrib variable a 'ushort' instead of a 'char'.  Also, we
 *            have to check to see that we have at least one argument
 *            before checking for flags.  Also add the c and u flags.
 * 05/06/91 - Version 1.4 - Support -r flag: recursive descent thru subdirectories.
 * 05/09/91 - Version 1.4b - Support new interface to slmake().
 * 06/10/91 - Version 1.5 - Support m, n, and t flags.  Also, allow multiple
 *            flag specifications (i.e. 'ls -t -n' is the same as 'ls -tn').
 * 06/11/91 - Version 1.6 - Updated multicolumn algorithm.
 * 07/02/91 - Version 1.7 - Add o and 1 flags.  The default is changed to
 *            sort by name, and list in multicolumn format if isatty() is
 *            TRUE for standard output.
 * 04/13/92 - Version 1.8 - Don't force HPFS filenames to lowercase anymore.
 * 04/30/92 - Version 1.9 - The flags are now case-sensitive.  Reverse sorting
 *            and sorting by size have been added.
 * 07/23/92 - Version 1.9 for OS/2 2.0 and C Set/2 (32-bit version).
 * 07/29/93 - Version 1.10 - Support -L flag to lowercase names.
 * 03/07/94 - Version 1.11 - Support -i flag (case-insenstive name sort); and
 *            make -n mean case-sensitive name sort (OS/2 only).
 *============================================================================*/

static char copr[] = "(c)Copyright IBM Corporation, 1994.  All rights reserved.";
static char ver[]  = "Version 1.11";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "util.h"

#define BUFFLEN  2048               /* Buffer length */

#define MAXWIDTH (BUFFLEN-1)        /* Define the same as BUFFLEN */
#define MINPAD   4                  /* Min. padding between names, for -m */

static  char     buff[BUFFLEN+1];   /* Buffer for output filenames */
static  char     fstr[BUFFLEN+1];   /* Buffer for basename portion of filename */

/*----------------------------------------------------------------------------*
 * Default argc/argv: If no file specs entered, then assume "*"
 *----------------------------------------------------------------------------*/

static  int      dargc = 1;
static  char    *dargv[1] = { "*" };

/*----------------------------------------------------------------------------*
 * Definitions for type of output file sorting.  The base values (the first
 * value of each pair) is set by one of the sorting flags.  If a reverse sort
 * is selected, the base value is simply incremented to give the value of the
 * reverse sort.
 *----------------------------------------------------------------------------*/

enum SORT_TYPE {  SortNone0,        /* Don't sort */
                  SortNone,         /* Don't reverse sort either */

                  SortName,         /* Sort by filename */
                  SortRName,        /* Reverse sort by filename */

                  SortNameI,        /* Sort by filename (case-insensitive) */
                  SortRNameI,       /* Reverse sort by filename (case-insensitive)*/

                  SortTime,         /* Sort by date and time */
                  SortRTime,        /* Reverse sort by date and time */

                  SortSize,         /* Sort by file size */
                  SortRSize         /* Reverse sort by file size */
};

/*----------------------------------------------------------------------------*
 * Structure for flags
 *----------------------------------------------------------------------------*/

typedef struct {

     int        f_flag;             /* list ordinary files */
     int        s_flag;             /* list system files */
     int        h_flag;             /* list hidden files */
     int        d_flag;             /* list directories */

     int        a_flag;             /* list all */
     int        l_flag;             /* list in long format */
     int        U_flag;             /* list in uppercase */
     int        L_flag;             /* list in lowercase */
     int        p_flag;             /* Append path separator to directory names */

     int        c_flag;             /* Display time of creation */
     int        u_flag;             /* Display time of last access */

     int        C_flag;             /* Multicolumn mode setting */
     int        r_flag;             /* Reverse sort flag */

} LSFLAGS;

static LSFLAGS flags = {            /* Flags and their initial settings */

     FALSE, FALSE, FALSE, FALSE,
     FALSE, FALSE, FALSE, FALSE,
     FALSE, FALSE,
     FALSE, FALSE
};

/*----------------------------------------------------------------------------*
 * Structures and data used by storefile(), sortfiles(), and mshow()
 *----------------------------------------------------------------------------*/

typedef struct stfile {

     struct stfile *next;           /* Pointer to next stored file's structure */
     char      *fname;              /* Pointer to file's name */
     ulong      fsize;              /* File's size */
     ushort     fmode;              /* File's mode */
     ushort     fdate;              /* File's date word */
     ushort     ftime;              /* File's time word */
     ulong      fdatetime;          /* Time/date doubleword, for sorting */

} STFILE;

static STFILE *stfirst = NULL;      /* Linked list of stored file structures */
static STFILE *stlast  = NULL;

static ulong   stcnt   = 0L;        /* Number of stored file structures */
static uint    stlongest = 0;       /* Length of longest stored name */
static ulong   stmem   = 0L;        /* Total memory usage by stfile structures */
static STFILE **starray = NULL;     /* Array of 'stcnt' ptrs to stored file structures */

static char    MsgStoreMem[] =
"ls: There isn't enough memory to store filenames for multicolumn listing\n\
    and/or sorting.  Try again, but specify the -o1 flags.\n";

/*============================================================================*
 * Function prototypes for internal subroutines
 *============================================================================*/

static void       syntax       ( void );
static void       showfile     ( char *, ulong, ushort, ushort, ushort, LSFLAGS * );
static int        storefile    ( char *, ulong, ushort, ushort, ushort );
static int        sortfiles    ( int );
static void       showarray    ( LSFLAGS * );

static uint       getwidth     ( void );

/*----------------------------------------------------------------------------*
 * The two arguments to each sort comparison function are: STFILE  **
 * However, they need to be desclared as: void *    Therefore, the
 * p1 and p2 macros are used. Assuming v1 and v2 are the sort comparison
 * function arguments, then (*p1) and (*p2) are pointers to the structures.
 *----------------------------------------------------------------------------*/

#define p1 ((STFILE **)v1)
#define p2 ((STFILE **)v2)

static int        qcompName    ( const void *, const void * );
static int        qcompRName   ( const void *, const void * );
static int        qcompNameI   ( const void *, const void * );
static int        qcompRNameI  ( const void *, const void * );
static int        qcompTime    ( const void *, const void * );
static int        qcompRTime   ( const void *, const void * );
static int        qcompSize    ( const void *, const void * );
static int        qcompRSize   ( const void *, const void * );

/*============================================================================*
 * Main Program Entry Point
 *============================================================================*/

main(argc, argv)

int argc;                           /* arg count */
char *argv[];                       /* arg pointers */

{
  int     rc;                       /* Return code store area */
  long    lrc;                      /* Long return code */
  char   *flagstr;                  /* Pointer to string of flags */
  char    cf;                       /* Current flag character (if any) */

  int     uniq;                     /* 'uniq' flag for slmake() */
  ushort  mflags;                   /* Match flags for slrewind() */
  ushort  recurse;                  /* Match flags for slrewind() */

  int     sorttype;                 /* Type of sorting to perform */
  int     store;                    /* Do we need to store filenames first? */

  SLPATH *speclist;                 /* Pointer to linked list of path specs */

  FINFO  *fdata;                    /* Pointers returned by slmatch() */
  SLPATH *pdata;
  SLNAME *ndata;

  ushort f_time;                    /* Time and date values that we will */
  ushort f_date;                    /* display */

  uniq = TRUE;                      /* Init: build SLPATHs only for unique paths */
  recurse = FALSE;                  /* Init: don't recursively search dirs */

  sorttype = SortName;              /* Default is to sort by name */
  store = FALSE;                    /* We'll adjust this later if necessary */

  if (isatty(1))                    /* If standard output is a char device: */
     flags.C_flag = TRUE;                /* Enable multi-column output */
  else                              /* Else: */
     flags.C_flag = FALSE;               /* Set single-column output */

 /*---------------------------------------------------------------------------*
  * Process flags, if any
  *---------------------------------------------------------------------------*/

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

              case 'f':
                 flags.f_flag = TRUE;
                 break;

              case 's':
                 flags.s_flag = TRUE;
                 break;

              case 'h':
                 flags.h_flag = TRUE;
                 break;

              case 'd':
                 flags.d_flag = TRUE;
                 break;

              case 'a':
                 flags.a_flag = TRUE;
                 break;

              case 'c':
                 flags.c_flag = TRUE;
                 break;

              case 'u':
              case 'x':
                 flags.u_flag = TRUE;
                 break;

              case 'U':
                 flags.U_flag = TRUE;
                 flags.L_flag = FALSE;
                 break;

              case 'L':
                 flags.U_flag = FALSE;
                 flags.L_flag = TRUE;
                 break;

              case 'p':
                 flags.p_flag = TRUE;
                 break;

              case 'R':
                 recurse = TRUE;
                 break;

              case 'g':                          /* 'g' means group in order that */
                                                 /* filespecs were listed on cmd line */
                 uniq = FALSE;
                 break;

             /*--------------------------------------*
              * These flags affect multicolumn output
              *--------------------------------------*/

              case 'l':
                 flags.l_flag = TRUE;
                 flags.C_flag = FALSE;
                 break;

              case 'C':                          /* 'C' sets multicolum format */
              case 'm':
                 flags.C_flag = TRUE;
                 flags.l_flag = FALSE;
                 break;

              case '1':                          /* '1' forces single-column format */
                 flags.C_flag = FALSE;
                 break;

             /*--------------------------------------*
              * These flags affect sorting
              *--------------------------------------*/

              case 'n':                          /* 'n' sorts by name */
                 sorttype = SortName;            /* (case-sensitive) */
                 break;

              case 'i':                          /* 'n' sorts by name */
                 sorttype = SortNameI;           /* (case-insensitive) */
                 break;

              case 't':                          /* 't' sorts by date/time */
                 sorttype = SortTime;
                 break;

              case 'z':                          /* 'z' sorts by size */
                 sorttype = SortSize;
                 break;

              case 'o':                          /* Disable sorting */
                 sorttype = SortNone0;
                 break;

              case 'r':                          /* 'r' reverses the sort */
                 flags.r_flag = TRUE;
                 break;

              default:
                 fprintf(stderr, "ls: Invalid flag '%c'.  For help, enter 'ls -?'\n",
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

  if ( (flags.f_flag == FALSE) &&   /* If all flags having to do with file */
       (flags.s_flag == FALSE) &&   /* types are all (false), then set the */
       (flags.h_flag == FALSE) &&   /* f and d flags to true (default) */
       (flags.d_flag == FALSE) )
  {
     flags.f_flag = TRUE;
     flags.d_flag = TRUE;
  }

  if (flags.C_flag ||               /* If we're listing in multi-column format */
     (sorttype > SortNone))         /* or we're sorting the output listing: */
        store = TRUE;                    /* Then we'll need to store all names */

  if (flags.r_flag)                 /* If we're sorting in reverse order: */
     sorttype++;                         /* Bump sort type to 2nd value in pair */

 /*---------------------------------------------------------------------------*
  * Build the mflags variable: will be passed to slrewind() to restrict
  * the names matched by slmatch().
  *---------------------------------------------------------------------------*/

  mflags = 0;                       /* Reset flags, then: */

  if (flags.f_flag == TRUE)
     mflags = mflags | SL_NORMAL;

  if (flags.s_flag == TRUE)
     mflags = mflags | SL_SYSTEM;

  if (flags.h_flag == TRUE)
     mflags = mflags | SL_HIDDEN;

  if (flags.d_flag == TRUE)
     mflags = mflags | SL_DIR;

  if (flags.a_flag == TRUE)
     mflags = mflags | SL_NORMAL |
                       SL_SYSTEM |
                       SL_HIDDEN |
                       SL_DIR    |
                       SL_DOTDIR |
                       SL_VOLID;

 /*---------------------------------------------------------------------------*
  * Process file specifications and build specification list
  *---------------------------------------------------------------------------*/

  if (argc != 0)                    /* If there are command line args: */
     rc = slmake(&speclist, uniq, TRUE, argc, argv);     /* Process them */
  else                              /* If there are NO command line args: */
     rc = slmake(&speclist, uniq, TRUE, dargc, dargv);   /* Then assume "*" */

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

  slrewind(speclist, mflags,        /* Initialize slmatch() */
           recurse);
  for (;;)                          /* Loop to find all matching DOS files: */
  {
    /*--------------------------------------------------*
     * Get next matching file
     *--------------------------------------------------*/

     fdata = slmatch(&pdata,             /* Get next matching DOS file */
                     &ndata);
     if (fdata == NULL)                  /* If none: */
        break;                                /* Done */

    /*--------------------------------------------------*
     * Build d:path\name string and store in buff[]:
     *--------------------------------------------------*/

     strcpy(buff, pdata->path);          /* Store path portion of filename */
     pathcat(buff, fdata->Fname);        /* Add name to end of path string */

     if (!flags.U_flag)                  /* If we don't want uppercase output: */
     {
        if ( (!pdata->hpfs) ||                /* If not an HPFS name, */
             (flags.L_flag) )                 /* or if -L specified: */
           strlwr(buff);                         /* Make it lowercase */
     }
     else                                /* Otherwise: we want uppercase output: */
     {
        strupr(buff);                         /* Convert it to uppercase */
     }

     if (flags.p_flag)                   /* If p flag specified, then: */
        if (isdir(buff))                      /* If name is that of a directory: */
           pathcat(buff, "\\");                    /* Add path separator to it */

    /*--------------------------------------------------*
     * Store the appropriate time/date to display
     *--------------------------------------------------*/

     f_time = f_date = 0;                /* Initialize */

     if ((!flags.c_flag) && (!flags.u_flag))
     {                                   /* If neither c nor u specified: */
        memcpy(&f_time,                  /* Get time of last modification */
               &fdata->ftimeLastWrite,
               sizeof(ushort));
        memcpy(&f_date,
               &fdata->fdateLastWrite,
               sizeof(ushort));
     }
     else if (flags.c_flag)              /* If c specified: */
     {                                   /* Get time of creation */
        memcpy(&f_time,
               &fdata->ftimeCreation,
               sizeof(ushort));
        memcpy(&f_date,
               &fdata->fdateCreation,
               sizeof(ushort));
     }
     else                                /* If u specified: */
     {                                   /* Get time of last access */
        memcpy(&f_time,
               &fdata->ftimeLastAccess,
               sizeof(ushort));
        memcpy(&f_date,
               &fdata->fdateLastAccess,
               sizeof(ushort));
     }

    /*--------------------------------------------------*
     * Either show the file or store it
     *--------------------------------------------------*/

     if (store == FALSE)                 /* If we're not storing the file: */
     {
        showfile(buff, fdata->Fsize,          /* Show it */
                       fdata->Fmode,
                       f_date, f_time,
                       &flags);
     }
     else                                /* Otherwise: */
     {
        rc = storefile(buff, fdata->Fsize,    /* Store it */
                        fdata->Fmode,
                        f_date, f_time);
        if (rc != 0)                          /* If we can't store it: */
        {
           fprintf(stderr, "%s", MsgStoreMem);     /* Display error message */
           return(2);
        }
     }
  } /* end of for loop for all matching files */

 /*---------------------------------------------------------------------------*
  * If we stored the files, then process and list them
  *---------------------------------------------------------------------------*/

  if (store == TRUE)                /* If we have stored files: */
  {
     /*
     printf("Total number of stored files:     %lu\n", stcnt);
     printf("Amount of memory used for files:  %lu\n", stmem);
     */

     rc = sortfiles(sorttype);           /* Sort them if required */
     if (rc != 0)                        /* Check for errors */
     {
        fprintf(stderr, "%s", MsgStoreMem);
        return(2);
     }

     showarray(&flags);                  /* Show the files in the array */

  } /* end of: if (store == TRUE) */

 /*---------------------------------------------------------------------------*
  * Display (on stderr) all filespecs that had no matching files
  *---------------------------------------------------------------------------*/

  lrc = slnotfound(speclist);       /* Display path\names w/no matching files */
  if (lrc == 0L)                    /* If all fspecs were matched: */
     return(0);                          /* Return successfully */
  else                              /* Otherwise:  One or more not found: */
     return(2);                          /* Return with error */

} /* end of main() */

/*============================================================================*
 * syntax() - Display command syntax and exit to operating system!
 *============================================================================*/
static void syntax()
{
  fprintf(stderr, "%s\n", ver);
  fprintf(stderr, "Usage:  ls [-fshdalcuRpC1ntzrogUL] [fspec ...]\n");
  fprintf(stderr, "Flags:\n");

  fprintf(stderr, "  f  Match ordinary files.\n");
  fprintf(stderr, "  s  Match system files.\n");
  fprintf(stderr, "  h  Match hidden files.\n");
  fprintf(stderr, "  d  Match subdirectories.\n");
  fprintf(stderr, "  a  Match all: Equivalent to -fshd.\n");

  fprintf(stderr, "  l  List in long format.\n");
  fprintf(stderr, "  c  Use time of creation (use with -l or -t).\n");
  fprintf(stderr, "  u  Use time of last access (use with -l or -t).\n");
  fprintf(stderr, "  R  Recursively descend subdirectories.\n");
  fprintf(stderr, "  p  Append path separator to names of directories.\n");

  fprintf(stderr, "  C  List files in multiple columns, sorted vertically.\n");
  fprintf(stderr, "  1  List files in one column.\n");
  fprintf(stderr, "  n  Sort listing by filename.\n");
  fprintf(stderr, "  i  Sort listing by filename (case-insensitive).\n");
  fprintf(stderr, "  t  Sort listing by file date and time.\n");
  fprintf(stderr, "  z  Sort listing by file size.\n");
  fprintf(stderr, "  r  Reverse the sorting order.\n");
  fprintf(stderr, "  o  Don't sort: list in the order found.\n");

  fprintf(stderr, "  g  Group output by 'fspec'.\n");
  fprintf(stderr, "  U  List names in uppercase.\n");
  fprintf(stderr, "  L  List names in lowercase.\n");

//fprintf(stderr, "\n");
//fprintf(stderr, "If no file specifications are present, then \"*\" is assumed.\n");
//fprintf(stderr, "If any fspec is a directory, then \"fspec\\*\" is assumed.\n");
//fprintf(stderr, "If none of the f, s, h, d, and a flags are specified, then 'fd'\n");
//fprintf(stderr, "is assumed: match ordinary files and directories.\n");

  exit(2);
}

/*============================================================================*
 * showfile() - Show file information.
 *
 * REMARKS:
 *   This subroutine formats the information on the current file entry and
 *   writes it to stdout.
 *
 * RETURNS: Nothing.
 *============================================================================*/

static void showfile(

char    *fullname ,                 /* Full name to display */
ulong    fsize    ,                 /* File's size */
ushort   attrib   ,                 /* File's attribute byte */
ushort   fdate    ,                 /* Date word */
ushort   ftime    ,                 /* Time word */
LSFLAGS *pflag    )                 /* Pointer to flags structure */

{
  FTM   ftm;                        /* File time structure */

  char  c_dir     = '-';            /* Characters for file mode (long format) */
  char  c_write   = 'w';            /* These are the initial settings: we'll */
  char  c_read    = 'r';            /* be changing them later */
  char  c_system  = '-';
  char  c_hidden  = '-';
  char  c_archive = '-';

 /*---------------------------------------------------------------------------*
  * Show information in either long or short format
  *---------------------------------------------------------------------------*/

  if (!pflag->l_flag)               /* If short format: */
  {
     printf("%s\n", fullname);           /* Just print name */
     return;
  }

 /*---------------------------------------------------------------------------*
  * Show information in long format
  *---------------------------------------------------------------------------*/

  if ((attrib & _A_VOLID) != 0)
     c_dir = 'v';
  if ((attrib & _A_SUBDIR) != 0)
     c_dir = 'd';

  if ((attrib & _A_RDONLY) != 0)
     c_write = '-';

  if ((attrib & _A_SYSTEM) != 0)
     c_system = 's';
  if ((attrib & _A_HIDDEN) != 0)
     c_hidden = 'h';
  if ((attrib & _A_ARCH) != 0)
     c_archive = 'a';

  cvtftime(ftime, fdate, &ftm);     /* "Extract" time/date bit fields */

  printf("%c%c%c%c%c%c %10lu  %2d-%02d-%4d %2d:%02d:%02d %s\n",
    c_dir,
    c_read,
    c_write,
    c_system,
    c_hidden,
    c_archive,

    fsize,                          /* file size */
    ftm.ftm_mon,
    ftm.ftm_day,
    ftm.ftm_year,
    ftm.ftm_hr,
    ftm.ftm_min,
    ftm.ftm_sec,

    fullname);

  return;
}

/*============================================================================*
 * storefile() - Store file's information.
 *
 * REMARKS:
 *   This subroutine stores in information for a file in malloc'd memory.
 *   It allocates a block of memory that is large enough to hold both the
 *   STFILE structure and the file's name, so that we can minimize the
 *   number of calls to malloc.
 *
 *   The static 'stcnt' variable is incremented to reflect the number of
 *   stored file structures.  The static 'stlongest' variable is updated
 *   (if necessary) so that it contains the length of the longest filename
 *   we have stored so far.  The static 'stmem' variable is updated for
 *   debug purposes only: to tell us how much memory we have allocated.
 *
 * RETURNS:
 *   0, if successful.  1, if there isn't enough memory.
 *============================================================================*/

static int storefile(

char    *fullname ,                 /* Full name to display */
ulong    fsize    ,                 /* File's size */
ushort   attrib   ,                 /* File's attribute byte */
ushort   fdate    ,                 /* Date word */
ushort   ftime    )                 /* Time word */

{
  uint    len;                      /* Length */

  int     totallen;                 /* Total length needed for memory block */
  char   *memblock;                 /* Pointer to allocated memory */
  STFILE *p_stfile;                 /* Pointer to STFILE structure */
  char   *p_fname;                  /* Pointers to filename string */

 /*---------------------------------------------------------------------------*
  * Allocate a single block of memory for data structure and all strings
  *---------------------------------------------------------------------------*/

  totallen = sizeof(STFILE) +       /* Calculate total size of memory block */
             strlen(fullname) + 1;  /* needed to hold structure and its strings */

  memblock = malloc(totallen);      /* Allocate the memory */
  if (memblock == NULL)
     return(1);

  p_stfile = (STFILE *)memblock;    /* Store pointer to block and to where */
  p_fname = memblock + sizeof(STFILE);   /* string will be put */
  strcpy(p_fname, fullname);        /* Store name string in the block */

  stmem += totallen;                /* Update total no. of bytes allocated */

  p_stfile->next = NULL;            /* Initialize structure link field */

  p_stfile->fname = p_fname;        /* Store file information in structure */
  p_stfile->fsize = fsize;
  p_stfile->fmode = attrib;
  p_stfile->fdate = fdate;
  p_stfile->ftime = ftime;

 /*---------------------------------------------------------------------------*
  * Store the file's date and time in the 'fdatetime' unsigned long field.
  * This will make it easy sortfiles() to perform a sort based on file
  * date/time if requested.
  *
  * The date is store in the hi-order word of this field, and the time is
  * store in the lo-order word.  We assume that ushort is a word and that
  * ulong is a doubleword.  However, we avoid any dependency on the Intel
  * ordering scheme by not accessing the high and low words directly but
  * by letting the compiler do it for us!
  *---------------------------------------------------------------------------*/

  p_stfile->fdatetime = fdate;      /* Store date in lower word of 'fdatetime' */
  p_stfile->fdatetime <<= 16;       /* Shift date into upper word */
  p_stfile->fdatetime |= ftime;     /* OR time into lower word */
/*
  printf("Date:      %04X\n", fdate);
  printf("Time:      %04X\n", ftime);
  printf("Date/time: %08lX\n", p_stfile->fdatetime);
*/
 /*---------------------------------------------------------------------------*
  * Add this file structure to the linked list
  *---------------------------------------------------------------------------*/

  if (stfirst == NULL)              /* If list is empty: */
     stfirst = p_stfile;                 /* New structure is now first on list */

  if (stlast != NULL)               /* If end-of-list ptr is not NULL: */
     stlast->next = p_stfile;            /* Point old end structure to new one */

  stlast = p_stfile;                /* New structure is now last on list */

 /*---------------------------------------------------------------------------*
  * Update some counts, and we're done
  *---------------------------------------------------------------------------*/

  stcnt++;                          /* Update count of file structures stored */

  len = strlen(fullname);           /* Get length of file name */
  if (stlongest < len)              /* If longest name so far is shorter: */
     stlongest = len;                    /* We have a new longest name! */

  return(0);                        /* Successful: return */
}

/*============================================================================*
 * sortfiles() - Sort the stored files.
 *
 * REMARKS:
 *   This subroutine builds an array of pointers to the stored file structures
 *   and stores a pointer to the array in 'starray'.  The linked list of
 *   'stcnt' structures must already have been set up.
 *
 *   This subroutine then sorts the pointers in this array, depending upon
 *   the value in 'sorttype'.
 *
 * RETURNS:
 *   0, if successful.  1, if there isn't enough memory for the array.
 *============================================================================*/

static int sortfiles(

int       sorttype )                /* Type of sorting: See SORT_TYPE enum */

{
  int     rc;                       /* Return code storage */
  STFILE *p_stfile;                 /* Pointer to STFILE structure */
  ulong   totallen;                 /* Total length needed for memory block */

  STFILE *source;                   /* Pointer to STFILE structure */
  STFILE **target;                  /* Ptr to loc containing ptr to STFILE */
  uint    cnt;                      /* Count value */

 /*---------------------------------------------------------------------------*
  * Allocate a block of memory to hold pointers to all STFILE structures
  *---------------------------------------------------------------------------*/

  if (stcnt == 0L)                  /* If there are no STFILE structures: */
     return(0);                          /* Then there's nothing to do! */

  totallen = sizeof(char *) *       /* Calulate size of block needed to hold */
             stcnt;                 /* the pointers of all structures */

// For 32-bit C Set/2 version: we can malloc a ulong: same as an int!
//if (totallen > 0x0000FFFEL)       /* If the size can't fit into a word: */
//   return(1);                          /* Then return w/error */

  starray = (STFILE **)malloc(totallen);
  if (starray == NULL)              /* Allocate the memory for the array */
     return(1);

 /*---------------------------------------------------------------------------*
  * Fill the block of memory with pointers to STFILE structures
  *---------------------------------------------------------------------------*/

  source = stfirst;                 /* Point source to first file structure */
  target = starray;                 /* Point target to beginning of array */

  cnt = (uint)stcnt;                /* Set loop count: no. of structures */
  while (cnt != 0)                  /* For each STFILE structure: */
  {
     *target = source;                   /* Store structure's ptr in array */
     source = source->next;              /* Point to next structure in list */
     target++;                           /* Point to next location in array */
     cnt--;                              /* Decrement loop counter */
  }

 /*---------------------------------------------------------------------------*
  * Sort, depending upon 'sorttype':
  *---------------------------------------------------------------------------*/

  switch (sorttype)                 /* Only the following types cause a sort: */
  {
     case SortName:                      /* Sort by name */
        qsort((void *)starray,
              (uint)stcnt,
              sizeof(char *),
              qcompName);
        break;

     case SortRName:                     /* Sort by reverse name */
        qsort((void *)starray,
              (uint)stcnt,
              sizeof(char *),
              qcompRName);
        break;

     case SortNameI:                     /* Sort by name (case-insensitive) */
        qsort((void *)starray,
              (uint)stcnt,
              sizeof(char *),
              qcompNameI);
        break;

     case SortRNameI:                    /* Sort by reverse name (case-insensitive) */
        qsort((void *)starray,
              (uint)stcnt,
              sizeof(char *),
              qcompRNameI);
        break;

     case SortTime:                      /* Sort by date/time stamp */
        qsort((void *)starray,
              (uint)stcnt,
              sizeof(char *),
              qcompTime);
        break;

     case SortRTime:                     /* Sort by reverse date/time stamp */
        qsort((void *)starray,
              (uint)stcnt,
              sizeof(char *),
              qcompRTime);
        break;

     case SortSize:                      /* Sort by size */
        qsort((void *)starray,
              (uint)stcnt,
              sizeof(char *),
              qcompSize);
        break;

     case SortRSize:                     /* Sort by reverse size */
        qsort((void *)starray,
              (uint)stcnt,
              sizeof(char *),
              qcompRSize);
        break;
  }

  return(0);                        /* Successful: return */
}

/*============================================================================*
 * showarray() - Show the array of stored files.
 *
 * REMARKS:
 *   This subroutine displays the files whose STFILE structure pointers are
 *   stored in the 'starray' array.  There are 'stcnt' pointers in this
 *   array.
 *
 *   This subroutine shows the files in the order that their pointers appear
 *   in the array.
 *
 * RETURNS:
 *   None.
 *============================================================================*/

static void showarray(flags)

LSFLAGS  *flags;                    /* Pointer to flags structure */

{
  STFILE  *p_stfile;                /* Pointer to STFILE structure */
  STFILE **parray;                  /* Ptr to loc inside the array */
  ulong    cnt;                     /* Count value */

  uint     dwidth;                  /* Width of display, in characters */
  uint     ncols;                   /* No. of columns of filenames */
  uint     nrows;                   /* No. of rows of filenames */
  STFILE **prow;                    /* Ptr to array position that starts a row */
  STFILE **pst;                     /* Ptr to next array position to display */
  STFILE **endarray;                /* Ptr to last location in array */

  uint     coltab;                  /* Tab width: used to put names in buffer */
  char    *pnext;                   /* Pointer inside output buffer */

  uint     rowcnt;                  /* Row counter, used inside loop */
  uint     colcnt;                  /* Column counter, used inside loop */

  if (stcnt == 0L) return;          /* If no files to show: Quit */

 /*---------------------------------------------------------------------------*
  * In case of multi-column format, determine the number of columns of
  * filenames to display.
  *---------------------------------------------------------------------------*/

  dwidth = getwidth();                   /* Find width of display */

  ncols = (dwidth + MINPAD) /            /* Find out how many columns we need */
          (stlongest + MINPAD);
  if (ncols == 0) ncols = 1;             /* Minimum no. of columns is one */

//printf("Display width      = %u\n", dwidth);
//printf("stlongest          = %u\n", stlongest);
//printf("No. of columns     = %u\n", ncols);
//printf("Multi-column mode:   %s\n", flags->C_flag ? "TRUE" : "FALSE");

 /*---------------------------------------------------------------------------*
  * If single-column format or if we only have one column to display anyway:
  * just show each structure in the array:
  *---------------------------------------------------------------------------*/

  if ((!flags->C_flag) ||           /* If we're not in multi-column mode, */
      (ncols == 1))                 /* or we have only one column anyway: */
  {
     cnt = stcnt;                        /* Set cnt = no. of ptrs in array */
     prow = starray;                     /* Point to beginning of array */
     while (cnt > 0)                     /* For each pointer in the array: */
     {
        showfile((*prow)->fname,              /* Show it */
                 (*prow)->fsize,
                 (*prow)->fmode,
                 (*prow)->fdate,
                 (*prow)->ftime,
                 flags);
        cnt--;                                /* Decrement counter */
        prow++;                               /* And point to next array element */
     }

     return;                             /* Done: Return */
  }

 /*---------------------------------------------------------------------------*
  * ===> Display filenames in multi-column mode.  If we get here, we are in
  *      multi-column mode and we have two or more columns to display.
  *
  * We want to show the files in the same order that you read a multi-
  * column newpaper or other publication:  You first descend the leftmost
  * column, then the next column over, etc.  But, we have to write an entire
  * row at a time to the display.
  *---------------------------------------------------------------------------*/

  coltab = stlongest + MINPAD;      /* Set tab increment inside output buffer */

  endarray = starray + stcnt - 1;   /* Point endarray to last array element */

 /*---------------------------------------------------------------------------*
  * Determine number of rows to filenames, including partial rows
  *---------------------------------------------------------------------------*/

  nrows = stcnt / ncols;            /* Determine no. of full rows of filenames */
  if ( (stcnt % ncols) != 0)        /* If last column is only partially filled: */
     nrows++;                       /* then we need to increment the row count */

//printf("No. of rows        = %u\n", nrows);

 /*---------------------------------------------------------------------------*
  * Display filenames, one row at a time.  This algorithm assures that
  * all columns, except possibly the last column, are full.  For a small
  * number of filenames, you may not use the full width of the display.
  * This is the same behaviour as that of PS/2 AIX's ls command.
  *---------------------------------------------------------------------------*/

  prow = starray;                   /* Pointer to start of first row in array */
  rowcnt = nrows;                   /* Set up row counter */
  while (rowcnt != 0)               /* For each row of filenames: */
  {
     memset(buff, ' ', dwidth);          /* Fill output buffer with spaces */
     buff[dwidth] = '\0';                /* and terminate it */

     pst = prow;                         /* Point pst to 1st name in row */
     pnext = buff;                       /* Point pnext to start of output buffer */
     colcnt = ncols;                     /* Set up column counter */
     while (colcnt != 0)                 /* For each column position in the row: */
     {
        if (pst > endarray)                   /* If we're past the end of array: */
           break;                                  /* We're done with this row */

        memcpy(pnext, (*pst)->fname,          /* Copy name to output buffer, but */
               strlen((*pst)->fname));        /* don't copy the null terminator! */

        pst += nrows;                         /* Point pst to next column within row */
        pnext += coltab;                      /* Point to next spot for name in buff */
        colcnt--;                             /* Decrement column count */
     }                                   /* end of loop for each column position */

     puts(buff);                         /* Write the row's names to stdout */

     prow++;                             /* Point prow to start of next row */
     rowcnt--;                           /* Decrement row count and continue */
  }                                 /* End of while loop for each row of filenames */

  return;                           /* Return */
}

/*============================================================================*
 * getwidth() - Gets width of display.
 *
 * REMARKS:
 *   This subroutine gets the width of the current display.  If this width
 *   is wider than MAXWIDTH, it returns MAXWIDTH.
 *============================================================================*/

static uint getwidth()
{
  int  rc;                          /* Return code storage */
  VIOMODEINFO vmi;                  /* Return info from vio */
  uint dwidth;                      /* Width of display */

 /*---------------------------------------------------------------------------*
  * Get width of current display
  *---------------------------------------------------------------------------*/

  rc = VioGetMode(&vmi, 0);         /* Get video mode */

  if (rc != 0)                      /* If error: */
     dwidth = 80;                      /* Assume 80 cols */
  else                              /* Else no error: */
  {                                    /* Extract info from buffer */
     dwidth = vmi.col;
  }

 /*---------------------------------------------------------------------------*
  * Adjust display width and return it
  *---------------------------------------------------------------------------*/

  dwidth--;                         /* Subtract 1, to allow for newline char */

  if (dwidth == 0)                  /* Just to be sure we never return 0 */
     dwidth = 1;

  if (dwidth > MAXWIDTH)            /* If larger than our internal maximum: */
     dwidth = MAXWIDTH;                  /* Then chop it down to size */
  return(dwidth);                   /* Return the width */
}

/*============================================================================*
 * qcompName() - Compare files by name (case-sensitive)
 *
 * REMARKS:
 *   This subroutine is called by qsort() in response to a request by
 *   sortfiles() to sort the 'starray' of STFILE structure pointers.
 *============================================================================*/

static int qcompName(

const void *v1 ,                    /* Pointer to first array element */
const void *v2 )                    /* Pointer to second array element */

{
  return(strcmp ( (*p1)->fname,     /* Return with result from comparison */
                 (*p2)->fname));
}


/*============================================================================*
 * qcompRName() - Compare files by name (case-sensitive)
 *
 * REMARKS:
 *   This subroutine is called by qsort() in response to a request by
 *   sortfiles() to sort the 'starray' of STFILE structure pointers.
 *============================================================================*/

static int qcompRName(

const void *v1 ,                    /* Pointer to first array element */
const void *v2 )                    /* Pointer to second array element */

{
  return(strcmp ( (*p2)->fname,     /* Return with result from comparison */
                 (*p1)->fname));
}

/*============================================================================*
 * qcompNameI() - Compare files by name (case-insensitive)
 *
 * REMARKS:
 *   This subroutine is called by qsort() in response to a request by
 *   sortfiles() to sort the 'starray' of STFILE structure pointers.
 *============================================================================*/

static int qcompNameI(

const void *v1 ,                    /* Pointer to first array element */
const void *v2 )                    /* Pointer to second array element */

{
  return(strcmpi( (*p1)->fname,     /* Return with result from comparison */
                 (*p2)->fname));
}


/*============================================================================*
 * qcompRNameI() - Compare files by name (case-insensitive)
 *
 * REMARKS:
 *   This subroutine is called by qsort() in response to a request by
 *   sortfiles() to sort the 'starray' of STFILE structure pointers.
 *============================================================================*/

static int qcompRNameI(

const void *v1 ,                    /* Pointer to first array element */
const void *v2 )                    /* Pointer to second array element */

{
  return(strcmpi( (*p2)->fname,     /* Return with result from comparison */
                 (*p1)->fname));
}


/*============================================================================*
 * qcompTime() - Compare files by time/date stamp.
 *
 * REMARKS:
 *   This subroutine is called by qsort() in response to a request by
 *   sortfiles() to sort the 'starray' of STFILE structure pointers.
 *============================================================================*/

static int qcompTime(

const void *v1 ,                    /* Pointer to first array element */
const void *v2 )                    /* Pointer to second array element */

{
  if ((*p1)->fdatetime == (*p2)->fdatetime) return(0);
  if ((*p1)->fdatetime >  (*p2)->fdatetime) return(-1);
                                            return(1);
}


/*============================================================================*
 * qcompRTime() - Compare files by time/date stamp.
 *
 * REMARKS:
 *   This subroutine is called by qsort() in response to a request by
 *   sortfiles() to sort the 'starray' of STFILE structure pointers.
 *============================================================================*/

static int qcompRTime(

const void *v1 ,                    /* Pointer to first array element */
const void *v2 )                    /* Pointer to second array element */

{
  if ((*p1)->fdatetime == (*p2)->fdatetime) return(0);
  if ((*p1)->fdatetime <  (*p2)->fdatetime) return(-1);
                                            return(1);
}


/*============================================================================*
 * qcompSize() - Compare files by size.
 *
 * REMARKS:
 *   This subroutine is called by qsort() in response to a request by
 *   sortfiles() to sort the 'starray' of STFILE structure pointers.
 *============================================================================*/

static int qcompSize(

const void *v1 ,                    /* Pointer to first array element */
const void *v2 )                    /* Pointer to second array element */

{
  if ((*p1)->fsize == (*p2)->fsize) return(0);
  if ((*p1)->fsize <  (*p2)->fsize) return(-1);
                                    return(1);
}


/*============================================================================*
 * qcompRSize() - Compare files by size.
 *
 * REMARKS:
 *   This subroutine is called by qsort() in response to a request by
 *   sortfiles() to sort the 'starray' of STFILE structure pointers.
 *============================================================================*/

static int qcompRSize(

const void *v1 ,                    /* Pointer to first array element */
const void *v2 )                    /* Pointer to second array element */

{
  if ((*p1)->fsize == (*p2)->fsize) return(0);
  if ((*p1)->fsize >  (*p2)->fsize) return(-1);
                                    return(1);
}
