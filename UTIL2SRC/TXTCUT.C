/* Module @(#)txtcut.c  1.5 last modified 9/16/93 10:26:44 */
/*============================================================================*
 * main() module: txtcut.c - Prepare text file for cut, awk, or Perl.
 *
 * (C)Copyright IBM Corporation, 1990 - 1995.                    Brian E. Yoder
 *
 * This program prepares a text for the AIX 'cut' utility.  Using the
 * configuration file access routines in 'cfaccess.c', this program puts
 * each line's tokens together, separated by a delimiter character.  Then,
 * individual fields (tokens) can be extracted using 'cut'.
 *
 * The configuration file format provides a flexible way to store information
 * in a text form that is both machine-readable and human-readable.  This
 * program reads the configuration file and throws away comments and blank
 * lines.  The rest of the information (in the form of 'tokens') is put into
 * a form that the standard 'cut' utility command can process.
 *
 * The format of the input text is as follows:  Blank lines are ignored.
 * Lines whose first nonblank character is '#' are assumed to be comment
 * lines and are ignored.  Each non-blank non-comment line consists of a
 * series of tokens.  A token may be:
 *
 *              1. A single '=' sign, or:
 *              2. A sequence of characters enclosed by a pair of double
 *                 quotes or a pair of single quotes, or:
 *              3. A contiguous sequence of non-blank characters.
 *
 * A # or a ; (unless enclosed in quotes) and any text that follows is assumed to
 * be a comment and is also ignored.
 *
 * A line's tokens are written to stdout, separated by a field delimiter
 * character.  The default delimiter character is a tab.
 *
 * EXAMPLE:
 *    Assume the delimiter character is specified as a colon.  If the input
 *    text file contains:
 *         # This is a comment line
 *         f1 = a b c    # First line of tokens
 *         x y z         # Second line of tokens
 *         aaa bbbb  "cccc   ddddd"  # Last line of tokens
 *
 *    Then the following is written to stdout:
 *         f1:=:a:b:c
 *         x:y:z
 *         aaa:bbbb:cccc   ddddd
 *
 * 09/13/90 - Created for PS/2 AIX.
 * 09/13/90 - Initial version.
 * 03/18/91 - If EOF found, don't write an extra newline character to stdout.
 * 11/10/92 - Ported to RISC/6000.
 * 05/26/93 - If the input file is missing, lines are read from stdin.
 * 09/01/93 - Add the -n and -l flags to add filename and line number.
 * 09/15/93 - Add the -c flag to add token count.
 * 03/07/94 - Ported to OS/2 2.1.
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "util.h"

/*============================================================================*
 * Define internal functions to allow forward access
 *============================================================================*/

static void syntax();

/*============================================================================*
 * Main Program Entry Point
 *============================================================================*/

main(

int argc     ,      /* arg count */
char *argv[] )      /* arg pointers */

{
  int numargs = 0;        /* No. of arguments processed, not incl. -flags */
  char *pgm;              /* Our program name */

  int  rc;                /* Return code store area */
  char cf;                /* Current flag character (if any) */

  char *cfname = "";      /* Pointer to input file name */
  char  delim  = '\t';    /* Delimiter character */

  int    ShowName = FALSE;  /* Show filename as first token? */
  int    ShowLine = FALSE;  /* Show line number as a token? */
  int    ShowTCnt = FALSE;  /* Show token count as a token? */

  int     tokc;           /* Number of tokens found in a line */
  char  **tokv;           /* Pointer to array of token pointers */

  ulong  lineno;            /* Line number */

/*============================================================================*
 * Process each command line argument:
 *============================================================================*/

  pgm = *argv;            /* Store pointer to program name, then */
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
           case '?':          /* -? : Help */
           case 'h':          /* -h : Help */
                syntax();
                break;

           case 'd':          /* -dchar */
                if (strlen(*argv) != 3)          /* Be sure there are 3 chars */
                {
                   fprintf(stderr, "%s: Invalid -dchar specification.\n", pgm);
                   return(1);
                }
                delim = *((*argv)+2);            /* Get/save delimiter char */
                break;

           case 'n':          /* -n : Show input filename */
                ShowName = TRUE;
                break;

           case 'l':          /* -l : Show line numbers */
                ShowLine = TRUE;
                break;

           case 'c':          /* -c : Show token count */
                ShowTCnt = TRUE;
                break;

           default:
                fprintf(stderr, "%s: Unrecognized flag: '-%c'\n",
                   pgm, cf);
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
           case 0:                   /* Name of text file */
                cfname = *argv;
                numargs++;
                break;

           default:
                fprintf(stderr, "%s: Extra argument: '%s'\n",
                   pgm, *argv);
                return(1);
                break; }
    }   /* end else: argument processing */

    argc--;                     /* Decrement command line argument counter */
    argv++;                     /* Point to next argument */

  }   /* end of command line argument processing loop */

/*============================================================================*
 * Dump the configuration file
 *============================================================================*/

  if (*cfname == '\0')         /* If no filename was specified: */
  {
     cfsetfile(stdin);              /* Process standard input */
     cfname = "(stdin)";            /* Store name used when reading stdin */
  }
  else                         /* Else: a filename was specified: */
  {
     rc = cfopen(cfname);           /* Open it for processing */
     if (rc != 0)
     {
        fprintf(stderr, "%s: Cannot open file: '%s'\n",
           pgm, cfname);
        return(1);
     }
  }

  for (;;)                     /* For each line with one or more tokens: */
  {
     tokc = cfread(&tokv);          /* Read the line */

     if (tokc == 0)                 /* If no tokens: */
        break;                      /*    We're done: break out of loop */

     if (ShowName)                  /* If writing the filename: */
     {
        printf("%s%c",                 /* Print it and a delimiter */
                cfname,
                delim);
     }

     if (ShowLine)                  /* If writing the line number: */
     {
        printf("%lu%c",                /* Print it and a delimiter */
                cfline(),
                delim);
     }

     if (ShowTCnt)                  /* If writing the token count: */
     {
        printf("%u%c",                 /* Print it and a delimiter */
                tokc,
                delim);
     }

     while (*tokv != NULL)          /* Print each token: */
     {
        printf("%s", *tokv);             /* Print the token */
        tokv++;                          /* Point to next token and continue */
        if (*tokv != NULL)               /* If we're not the last token: */
           printf("%c", delim);               /* Print delimiter character */
     }
     printf("\n");                  /* Tack on newline character */
  }

  cfclose();                   /* Close the configuration file */
  return(0);                   /* Done with main(): Return */

} /* end of main() */

/*============================================================================*
 * syntax() - Display command syntax and exit to operating system!
 *============================================================================*/
static void syntax()
{
  fprintf(stderr, "Usage: txtcut [-dchar] [-n] [-l] [textfile]\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "This program prepares a text file for the AIX cut command.\n");
  fprintf(stderr, "For each line of the text file that contains one or more\n");
  fprintf(stderr, "tokens, txtcut writes one line to stdout that contains the\n");
  fprintf(stderr, "tokens separated by a delimiter character. If the textfile\n");
  fprintf(stderr, "is missing, txtcut reads from standard input.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "A token can be an = sign, a series of contiguous nonblank\n");
  fprintf(stderr, "characters, or a sequence of characters enclosed by a pair\n");
  fprintf(stderr, "of double quotes or by a pair of single quotes.  A # and all\n");
  fprintf(stderr, "text that follows is assumed to be a comment and is ignored.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "FLAGS:\n");
  fprintf(stderr, "-dchar  The specified character is used as the delimiter. The\n");
  fprintf(stderr, "        default delimiter is a tab.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "-n      The name of the text file is written as a first token.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "-l      The current line number within the text file is written\n");
  fprintf(stderr, "        as a token.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "-c      The number of tokens in the text file's line is written\n");
  fprintf(stderr, "        as a token.\n");
  fprintf(stderr, "\n");
  exit(1);
}
