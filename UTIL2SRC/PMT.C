/*===========================================================================*
 * == main program ==
 * pmt.c - Shell pattern matching test using modified regexp.h header file.
 *
 * (C)Copyright IBM Corporation, 1989, 1990, 1991.         Brian E. Yoder
 *
 * This program contains an algorithm that attempts to expand (convert)
 * a shell pattern-matching expression into a regular expression.
 *
 * The first argument should be a shell pattern-matching expression enclosed
 * in double quotes.  The arguments that follow are compared with the expanded
 * regular expression.
 *
 * This program uses a modified version of the regexp.h header file that is
 * supplied with AIX.  As a result, this program is about half the size of
 * the rxls program which uses the regcmp() and regex() subroutines from the
 * programmer's workbench library.
 *
 * 12/01/89 - Created from rxls.c, rxh.c, and modified regexp.h files.
 * 12/04/89 - Split out r*() subroutines into 'rxpm.c' module.
 * 12/05/89 - Initial version completed.
 * 12/08/89 - Removed references to 'envp' on main(): 'envp' is not portable
 *            to all environments.
 * 01/10/90 - Moved eexpr[] and cexpr[] buffers to static area, since OS/2's
 *            startup code fails when these buffers are on the stack.
 * 05/25/90 - Include libutil.h instead of rxpm.h.
 * 04/04/91 - Ported from AIX to DOS and C/2.  When -d flag is specified,
 *            then certain pointers are displayed in hexadecimal.  This
 *            program assumes that pointers and integers are 32 bits wide.
 *            For DOS and 16-bit OS/2, ints are 16-bits.  However, this
 *            is left alone for now.
 *===========================================================================*/

char ver[] = "pmt: Pattern-matching test.  (C)IBM Corp. 1989, 1990";
char author[] = "Brian E. Yoder";

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "util.h"

#define MINARGS  1             /* Mimimum command line arguments required */

#define ELEN     1024          /* Length of expression buffers */

#define NO       0             /* Yes/no definitions */
#define YES      1

/*===========================================================================*
 * Function prototypes for subroutines in this module
 *===========================================================================*/

static void syntax();

/*===========================================================================*
 * Data used by the main() program
 *===========================================================================*/

static char *xmatch  = "**not initialized";
static char *match   = "          matched";
static char *nomatch = "--- did not match";

static char  eexpr[ELEN];      /* Full regular (expanded) expression */
static char  cexpr[ELEN];      /* Compiled expression */

/*===========================================================================*
 * Main program entry point
 *===========================================================================*/

main(argc, argv)

int    argc;
char **argv;

{
  int rc;                      /* return code storage */

  int   exact;                 /* Exact match? */
  int   debug;                 /* Print extra debugging information? */

  char *expr;                  /* Pointer to regular expression entered */

  char *next_eexpr;            /* Pointer to next expanded expression */
  char *next_cexpr;            /* Pointer to next compiled expression */

  char *mstart;                /* For rscan(): Pointer to start of match */
  char *mend;                  /*              Pointer past end of match */

  char *mrslt;                 /* Pointer to match result string */

  argv++;                      /* Ignore 1st argument (program name) */
  argc--;

  if (argc <  MINARGS)         /* If not enough arguments: display syntax */
     syntax();

  if (strcmp(*argv, "-d") == 0)
  {                            /* If -d flag: */
     debug = YES;                /* Set 'debug' flag = no */
     argv++;                     /* Bump to next argument */
     argc--;
     if (argc <  MINARGS)      /* If not enough arguments: display syntax */
        syntax();
  }
  else
  {
     debug = NO;               /* Set 'debug' flag = yes */
  }

  if (strcmp(*argv, "-s") == 0)
  {                            /* If -s flag: */
     exact = NO;                  /* Set 'exact' flag = no */
     argv++;                      /* Bump to next argument */
     argc--;
     if (argc <  MINARGS)      /* If not enough arguments: display syntax */
        syntax();
  }
  else
  {
     exact = YES;              /* Set 'exact' flag = yes */
  }

  expr = *argv;                /* Store pointer to expression */

  argv++;                      /* Set argv/argc up to get rest of arguments */
  argc--;

 /*--------------------------------------------------------------------------*
  * Expand 'expr' and store in 'exxpr[]'
  *--------------------------------------------------------------------------*/

  rc = rexpand(expr, eexpr, &eexpr[ELEN+1], &next_eexpr);
  if (rc != 0)
  {
     printf("--- Expanded expression is too large.\n");
     return(0);
  }

 /*--------------------------------------------------------------------------*
  * Display original and expanded regular expressions
  *--------------------------------------------------------------------------*/

  printf("%s\n", ver);
  printf("Pattern:             '%s'\n", expr);
  printf("Expanded expression: '%s'\n", eexpr);

  if (debug == YES)
  {
     printf("Pointer to expanded expression:   0x%08X\n", eexpr);
     printf("Pointer past expanded expression: 0x%08X\n", next_eexpr);
  }

 /*--------------------------------------------------------------------------*
  * Compile 'eexpr' and store in 'cexpr[]'
  *--------------------------------------------------------------------------*/

  rc = rcompile(eexpr, cexpr, &cexpr[ELEN+1], '\0', &next_cexpr);
  if (rc != 0)
  {
     printf("--- Cannot compile expanded expression: %s\n",
        rcmpmsg(rc));
     return(0);
  }

  if (debug == YES)
  {
     printf("Pointer to compiled expression:   0x%08X\n", cexpr);
     printf("Pointer past compiled expression: 0x%08X\n", next_cexpr);
     printf("Value of 'circf' stored in compiled expression: %d\n",
        *(int *)cexpr);
  }

 /*--------------------------------------------------------------------------*
  * Display each successive command line argument and whether or not it
  * matches the expanded regular expression in 'eexpr'
  *--------------------------------------------------------------------------*/

  printf("\n");
  if (exact == YES)
     printf("Arguments checked for exact match with expression:\n");
  else
     printf("Arguments scanned to see if they contain the expression:\n");
  printf("\n");

  while (argc > 0)                 /* For each argument passed by the shell: */
  {
     mrslt = xmatch;                   /* For now: 'mrslt' is not initialized */

     if (exact == YES)                 /* See if expression matches argument */
        rc = rmatch(cexpr, *argv);
     else
        rc = rscan(cexpr, *argv, &mstart, &mend);

     if (rc == RMATCH)
        mrslt = match;
     else
        mrslt = nomatch;

     printf("%s: '%s'",                /* Display the result of the match */
        mrslt,                         /* along with the argument itself */
        *argv);

     if ((exact == NO) && (rc == RMATCH))  /* If not an exact match: */
     {                                     /* Also display the substring matched: */
        *mend = '\0';                      /* Terminate substring w/null byte */
        printf(" : '%s'", mstart);         /* Display it */
     }

     printf("\n");                     /* Add newline */

     argv++;                           /* Point to next argument */
     argc--;
  }

  printf("\n");
  return(0);
}

/*===========================================================================*
 * syntax() - Display command syntax and EXIT TO OS!
 *===========================================================================*/
static void syntax()
{
  printf("\n%s\n", ver);                      /* Display version */
  printf("\n");
  printf("pmt  [-d]  [-s]  \"expr\"  \"str\"  [\"str\" . . .]\n");
  printf("\n");
  printf("The first argument is treated as a pattern-matching expression\n");
  printf("and is expanded into a full regular expression.\n");
  printf("\n");
  printf("The expanded expression is then matched against all arguments\n");
  printf("that follow.  Enclose an argument in quotes if it contains\n");
  printf("embedded blanks or special characters.\n");
  printf("\n");
  printf("if the -d (debug) flag is specified, then additional debugging\n");
  printf("information is displayed.\n");
  printf("\n");
  printf("If the -s (scan) flag is specified, this program scans each argument\n");
  printf("for the expanded regular expression.  Othewise, this program checks\n");
  printf("each argument for an exact match with the regular expression.\n");
  printf("\n");
  exit(1);
}
