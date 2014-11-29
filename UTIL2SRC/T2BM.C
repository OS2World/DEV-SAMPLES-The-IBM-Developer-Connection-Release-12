/*============================================================================*
 * main() module: t2bm.c - Convert text stream to BookMaster stream.
 *
 * (C)Copyright IBM Corporation, 1990, 1991, 1992.        Brian E. Yoder
 *
 * This program converts an input ASCII text stream to a BookMaster output
 * stream.  Specific characters (see text2bm.c) are translated to their
 * BookMaster symbol equivalents.  For example, a colon ":" is translated
 * into "&colon." so that it won't be interpreted as the start of a GML tag.
 * Other translations are performed for characters that don't map directly
 * into EBCDIC and therefore don't translate correctly uploaded to VM/MVS.
 *
 * It was initially developed to test the text2bm() subroutine in the
 * text2bm.c source module.  However, it is also a useful utility program
 * in its own right.
 *
 * 07/19/90 - Created.
 * 07/19/90 - Initial version of the test program.
 * 01/28/91 - Allow stdin/stdout, as well as specific file names.  Also,
 *            don't enclose output within :gdoc/:xmp. and /:exmp/:egdoc tags.
 * 04/03/91 - Ported from AIX to DOS C/2.
 * 09/11/91 - Explicitly open files in text mode, for DOS, OS/2.
 * 07/24/92 - Changed "rt" to "r" for fopen(). C Set/2 doesn't allow "rt".
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "util.h"

#define LINELEN 1024               /* Maximum input file line length allowed */
static  char line[LINELEN + 1];    /* Input file line buffer */

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
  int numargs;      /* No. of arguments processed (not including -flags) */

  int  rc;          /* Return code store area */
  char cf;          /* Current flag character (if any) */

  char *inname;     /* Pointers to arguments */
  char *outname;

  FILE *infile;
  FILE *outfile;

  int   i;          /* Integer index */
  char *text;       /* Pointer to text string */

  argv++;                 /* Ignore 1st argument (program name) */
  argc--;

  inname = argv[0];       /* Store pointers to arguments */
  outname = argv[1];

  rc = 0;                 /* Initialize return code value */

/*============================================================================*
 * Open input file and create output file
 *============================================================================*/

  switch (argc)
  {
     case 1:                        /* One argument: -i expected: */
        if (strcmp(inname, "-i") == 0)   /* If -i: */
        {
           infile = stdin;                    /* Set input file to stdin, */
           outfile = stdout;                  /* Set output file to stdout */
        }
        else
           syntax();
        break;

     case 2:                        /* Two arguments: Filenames expected: */
        infile = fopen(inname, "r");
        if (infile == NULL)
        {
           fprintf(stderr, "Cannot access input file: '%s'.\n", inname);
           return(1);
        }

        outfile = fopen(outname, "w");
        if (outfile == NULL)
        {
           fprintf(stderr, "Cannot create output file: '%s'.\n", outname);
           return(1);
        }
        break;

     default:                       /* Other no. of args: Display syntax: */
        syntax();
        break;
  }

/*============================================================================*
 * Loop to process the input file line-by-line
 *============================================================================*/

/*fprintf(outfile, ":gdoc.\n:xmp keep=5.\n");  Removed 1/28/91 */

  for (;;)
  {
     text = fgets(line, LINELEN,
                  infile);
     if (text == NULL)
        break;

     rc = text2bm(line, outfile);
     if (rc != 0)
     {
       fprintf(stderr, "--- Error detected by text2bm().\n");
       break;
     }
  }

/*fprintf(outfile, ":exmp.\n:egdoc.\n");       Removed 1/28/91 */

/*============================================================================*
 * Done
 *============================================================================*/

  return(rc);                       /* Done with main(): Return */

} /* end of main() */

/*============================================================================*
 * syntax() - Display command syntax and exit to operating system!
 *============================================================================*/
static void syntax()
{
  fprintf(stderr, "Usage: t2bm TextFile BookMasterFile\n");
  fprintf(stderr, "   or: t2bm -i\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "This program copies the text file to the BookMaster file,\n");
  fprintf(stderr, "converting special characters into BookMaster symbols.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "If -i is specified instead of the two filenames, then this\n");
  fprintf(stderr, "program reads the text from stdin and writes the output file\n");
  fprintf(stderr, "to stdout.\n");
  fprintf(stderr, "\n");
  exit(1);
}
