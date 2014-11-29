/*============================================================================*
 * main() module: cfdmp.c - Configuration file dump program.
 *
 * (C)Copyright IBM Corporation, 1990, 1991.                     Brian E. Yoder
 *
 * Test program for the configuration file access subroutines in the
 * 'cfaccess.c' source module.
 *
 * The programmer can use the source code in this test program for examples
 * of how to use these subroutines.
 *
 * 03/21/90 - Created.
 * 03/22/90 - Initial version.
 * 05/25/90 - Added test for new cfline() subroutine in cfaccess.c.
 * 03/25/91 - Added test for new cfsetfile() subroutine in cfaccess.c.
 * 04/03/91 - Ported from AIX to DOS C/2.
 * 11/17/91 - Processes "configuration" files, not "customization" files,
 *            in keeping with the enhancement to the cfaccess.c module.
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "util.h"

#define MINARGS  1     /* Minimum no. of (non-flag) command line arguments required */

#define NO       0
#define YES      1

/*============================================================================*
 * Define internal functions to allow forward access
 *============================================================================*/

static void syntax();

/*============================================================================*
 * Main Program Entry Point
 *============================================================================*/

main(argc, argv)

int argc;           /* arg count */
char *argv[];       /* arg pointers */

{
  int numargs;            /* No. of arguments processed, not incl. -flags */

  int  rc;                /* Return code store area */
  char cf;                /* Current flag character (if any) */

  char *cfname;           /* Pointer to file name */
  int  f_flag;            /* -f (only list 'file' entries) flag */

  int  t_flag;            /* -t:tagname flag */
  char *tagname;          /* Pointer to tagname (if -t) */

  int  i_flag;            /* -i flag */

  int     tokc;           /* Number of tokens found in a line */
  char  **tokv;           /* Pointer to array of token pointers */

  char   *name;           /* Pointer to a file name */

/*============================================================================*
 * Set initial value of some variables:
 *============================================================================*/

  numargs = 0;            /* Initialize number of arguments processed = none */
  f_flag = NO;            /* Initialize -f = no */
  t_flag = NO;            /* Initialize -t = no */
  i_flag = NO;            /* Initialize -i = no */

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
           case 'f':          /* -f */
                f_flag = YES;
                break;

           case 'i':          /* -i */
                i_flag = YES;
                break;

           case 't':          /* -t:tagname */
                if ((*argv)[2] != ':')           /* Be sure colon is there */
                {
                   fprintf(stderr, "Missing colon on '-%c:'\n", cf);
                   return(1);
                }
                tagname = (*argv)+3;             /* Point to char after flag */
                t_flag = YES;
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
           case 0:                   /* Name of input file */
                cfname = *argv;
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
 * Get value for specified tag, if -t
 *============================================================================*/

  if (t_flag == YES)
  {
     tokc = cfgetbyname(cfname, tagname, &tokv, &name);

     printf("Return from cfgetbyname() = %d\n", tokc);

     if (name == NULL)
        name = "** none **";

     printf("Tag = '%s':  Value = '%s'\n",
        tagname,
        name);

     return(0);
  }

/*============================================================================*
 * Dump the configuration file
 *============================================================================*/

  if (f_flag == NO)
  {
     /*-----------------------------------------------------------------------*
      * Dump all tag entries
      *-----------------------------------------------------------------------*/

      if (i_flag == NO)            /* If -i is not specified: */
      {
         rc = cfopen(cfname);           /* Open the configuration file */
         if (rc != 0)
         {
            fprintf(stderr, "Cannot open configuration file: %s\n",
               cfname);
            return(1);
         }
      }
      else                         /* Otherwise: */
         cfsetfile(stdin);              /* Process standard input */

      for (;;)                     /* For each entry in the file: */
      {
         tokc = cfread(&tokv);          /* Read the entry */

         printf("Line %lu: Number of tokens = %d%s\n",
            cfline(),
            tokc,
            cfisapp() ? "  (.INI application)" : "" );

         while (*tokv != NULL)          /* Display each token */
         {
            printf("   '%s'\n", *tokv);
            tokv++;
         }

         if (tokc == 0)                 /* If no tokens: */
            break;                      /*    We're done: break out of loop */
      }

      cfclose();                   /* Close the configuration file */
  }
  else
  {
     /*-----------------------------------------------------------------------*
      * Dump only 'file' entries
      *-----------------------------------------------------------------------*/

      if (i_flag == NO)            /* If -i is not specified: */
      {
         rc = cfopen(cfname);           /* Open the configuration file */
         if (rc != 0)
         {
            fprintf(stderr, "Cannot open configuration file: %s\n",
               cfname);
            return(1);
         }
      }
      else                         /* Otherwise: */
         cfsetfile(stdin);              /* Process standard input */

      for (;;)                     /* For each entry in the file: */
      {
         name = cfreadfile();           /* Read a file entry */

         if (name == NULL)              /* If at end of file: */
            break;                      /*    We're done: break out of loop */

         printf("File name: '%s'\n", name);
      }

      cfclose();                   /* Close the configuration file */
  }

  return(0);                    /* Done with main(): Return */

} /* end of main() */

/*============================================================================*
 * syntax() - Display command syntax and exit to operating system!
 *============================================================================*/
static void syntax()
{
  fprintf(stderr, "Usage: cfdmp [-f | -t:tagname] [-i] filename\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "This program dumps the significant lines (non-blank, non-\n");
  fprintf(stderr, "comment) in the specified configuration 'filename'.  If '-i'\n");
  fprintf(stderr, "is specified instead of a filename, then cfdmp processes the\n");
  fprintf(stderr, "lines from stdin, and 'filename' is ignored.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "If -f is specified, then only the \"file\" entries in the\n");
  fprintf(stderr, "configuration file are dumped.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "If -t is specified, then the tokens in the first line of the\n");
  fprintf(stderr, "configuration file whose first token is 'tagname' are dumped.\n");
  fprintf(stderr, "Note: If -t is specified, the -i is ignored.\n");
  fprintf(stderr, "\n");
  exit(1);
}
