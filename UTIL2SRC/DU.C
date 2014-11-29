/*============================================================================*
 * main() module: du.c - Directory usage.                    OS/2 2.1 version.
 *
 * This program lists directory usage. Output is displayed in bytes (not
 * blocks). Optionally, subdirectories may be recursed to yield the bytes
 * in an entire tree.
 *
 * The default is to list all files in the current directory. Use -s and -h
 * to include system and hidden files, respectively. Use -R to recurse sub-
 * directories. Enter -? for help.
 *
 * 12/08/92 - Created by Brian Yoder.
 * 12/08/92 - Initial version.
 * 10/01/93 - Ported to OS/2 2.1.
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
//#include <dos.h>

#include "util.h"

static  ulong    fcnt    = 0L;      /* Total no. of files matched */
static  ulong    tsize = 0L;        /* Total no. of bytes in all files */

/*----------------------------------------------------------------------------*
 * Default argc/argv: If no file specs entered, then assume "*"
 *----------------------------------------------------------------------------*/

static  int      dargc = 1;
static  char    *dargv[1] = { "*" };

/*----------------------------------------------------------------------------*
 * Structure for flags
 *----------------------------------------------------------------------------*/

typedef struct {

     int        s_flag;             /* include system files */
     int        h_flag;             /* include hidden files */

} DUFLAGS;

static DUFLAGS flags = {            /* Flags and their initial settings */

     FALSE, FALSE
};

/*============================================================================*
 * Function prototypes for internal subroutines
 *============================================================================*/

static void       syntax       ( void );

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

  SLPATH *speclist;                 /* Pointer to linked list of path specs */

  FINFO  *fdata;                    /* Pointers returned by slmatch() */
  SLPATH *pdata;
  SLNAME *ndata;

  uniq = TRUE;                      /* Init: build SLPATHs only for unique paths */
  recurse = FALSE;                  /* Init: don't recursively search dirs */

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

              case 's':
                 flags.s_flag = TRUE;
                 break;

              case 'h':
                 flags.h_flag = TRUE;
                 break;

              case 'R':
                 recurse = TRUE;
                 break;

              default:
                 fprintf(stderr, "du: Invalid flag '%c'.  For help, enter 'du -?'\n",
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

 /*---------------------------------------------------------------------------*
  * Build the mflags variable: will be passed to slrewind() to restrict
  * the names matched by slmatch().
  *---------------------------------------------------------------------------*/

  mflags = SL_NORMAL;               /* Include normal files. Then: */

  if (flags.s_flag == TRUE)         /* Include system files, too? */
     mflags = mflags | SL_SYSTEM;

  if (flags.h_flag == TRUE)         /* Include hidden files, too? */
     mflags = mflags | SL_HIDDEN;

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
     * Get next matching file. Increment file count and
     * add the file's size to the total size
     *--------------------------------------------------*/

     fdata = slmatch(&pdata,             /* Get next matching DOS file */
                     &ndata);
     if (fdata == NULL)                  /* If none: */
        break;                                /* Done */

     fcnt++;                             /* Increment file count */

     tsize += fdata->Fsize;              /* Add file's size to total */

  } /* end of for loop for all matching files */

 /*---------------------------------------------------------------------------*
  * Display total size and number of files on stdout
  *---------------------------------------------------------------------------*/

  printf("%lu bytes in %lu files\n", tsize, fcnt);

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
  fprintf(stderr, "Usage:  du [-shR] [fspec ...]\n");
  fprintf(stderr, "Flags:\n");

  fprintf(stderr, "  s  Include system files.\n");
  fprintf(stderr, "  h  Include hidden files.\n");

  fprintf(stderr, "  R  Recursively descend subdirectories.\n");

  exit(2);
}
