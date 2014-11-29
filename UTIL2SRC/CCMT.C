/*===========================================================================*
 * == main program ==
 * ccmt.c - C comment checker.
 *
 * (C)Copyright IBM Corporation, 1990, 1991.               Brian E. Yoder
 *
 * This program checks for mismatched comments in C source code.
 *
 * Each filename listed on the command line is assumed to be a C source file
 * (.c, .h, etc.) and is checked for problems with comments.
 *
 * This program follows the ANSI standard that doesn't allow the nesting of
 * comments.  A compiler that supports nested comments would make this utility
 * unnecessary.
 *
 * The Ingres embedded SQL requires some strings to be entered in single quotes.
 * Therefore, character constants are treated as single-quote strings by ccmt.
 *
 * 02/26/90 - Created.
 * 02/27/90 - Initial version completed.
 * 02/28/90 - Updated to use a state machine.
 * 03/02/90 - Updated to support double-slash comments that stop at end of line.
 * 04/03/91 - Ported from AIX to DOS C/2.
 *===========================================================================*/

char ver[] = "ccmt: C comment checker.  (C)IBM Corp. 1990";
char author[] = "Brian E. Yoder";

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>

#define MINARGS  1             /* Mimimum command line arguments required */

#define BUFFLEN  2048          /* Length of input line buffer */

#define NO       0             /* Yes/no definitions */
#define YES      1

/*===========================================================================*
 * Function prototypes for subroutines in this module
 *===========================================================================*/

static void syntax();
static int  chkfile(FILE *);
static int  get_char();
static int  put_char();
static void errmsg(long, char *);
static int  pstate(int);

/*===========================================================================*
 * Misc data
 *===========================================================================*/

static char *pgm;              /* Pointer to name of program */
static int   debug = NO;       /* 'debug' flag */

/*===========================================================================*
 * Data used to process each source file:
 *===========================================================================*/

static char *fname = NULL;     /* Pointer to file name */
static FILE *sfile = NULL;     /* Input source file */

static long  linenum;          /* Current line number */
static char *cptr;             /* Ptr to a char in line buffer */

static char  lbuff[BUFFLEN];   /* Input line buffer */

/*===========================================================================*
 * List of states for the state machine in the pstate() subroutine.
 *
 * The state machine is entered in the 'Text' state.  This is the normal
 * state.
 *
 * Characters are read from the input file one-by-one.  The get_char() routine
 * not returns a character and sets some global (within this module) variable
 * with information about the character, such as the current line.
 *
 * The pstate() subroutine is called with the value of the character as its
 * argument.  Note that the subroutine is called once for each character
 * that is obtained from the input file.
 *
 * Depending upon the character, the state machine in pstate() performs a
 * specific action (such as storing the current line number or writing an
 * error message).  The state machine then sets the next state as appropriate.
 *
 * The return code from the pstate() subroutine tells the calling program
 * whether or not to continue: 0 = continue, other = stop.
 *===========================================================================*/

enum CCMT_STATES {

        Text,             /* Text mode */

        String,           /* Inside double-quote string */
        StringEsc,        /* Inside escape sequence of a double-quote string */

        CharConst,        /* Inside single-quote string */
        CharConstEsc,     /* Inside escape sequence of a single-quote string */

        PUEndComment,     /* Possible unexpected end of comment */

        PStartComment,    /* Possible start of comment */
        Comment,          /* Inside a comment */
        PEndComment,      /* Possible end of comment */
        PNestComment,     /* Possible start of nested comment */

        EOLComment        /* Inside a comment that stops at end of line (EOL) */
};

/*===========================================================================*
 * Data for the state machine
 *===========================================================================*/

static  int  state;            /* Current state */
static  long start_line;       /* Line on which string, comment, etc. started */

/*===========================================================================*
 * Main program entry point
 *===========================================================================*/

main(argc, argv)

int    argc;
char **argv;

{
  int rc;                      /* return code storage */

  pgm = *argv;                 /* Store pointer to name of program, then: */
  argv++;                      /* Ignore 1st argument (program name) */
  argc--;

  if (argc <  MINARGS)         /* If not enough arguments: display syntax */
     syntax();

  if (strcmp(*argv, "-d") == 0)
  {                            /* If -d flag: */
     debug = YES;                /* Set 'debug' flag = yes */
     argv++;                     /* Bump to next argument */
     argc--;
     if (argc <  MINARGS)      /* If not enough arguments: display syntax */
        syntax();
  }

 /*==========================================================================*
  * Check each file whose name is found on the command line:
  *==========================================================================*/

  while (argc > 0)             /* For each argument passed by the shell: */
  {
     fname = *argv;                 /* Store pointer to name of file */

     sfile = fopen(fname, "r");     /* Open source file */
     if (sfile == NULL)             /* If error: */
     {                                   /* Display message */
        fprintf(stderr, "%s: Cannot open file '%s'.\n",
                pgm, fname);
     }
     else                           /* If no error: */
     {
        chkfile(sfile);                  /* Check file */
        fclose(sfile);                   /* Close file */
     }

     argv++;                        /* Point to next argument */
     argc--;
  }                            /* end of main while loop */

  return(0);                   /* Done: return */
}

/*===========================================================================*
 * syntax() - Display command syntax and EXIT TO OS!
 *===========================================================================*/
static void syntax()
{
  printf("Usage: ccmt sfile [. . . sfile]\n");
  printf("\n");
  printf("  One or more source files are checked to ensure that comments\n");
  printf("  are properly terminated and are not nested.\n");
  printf("\n");
  exit(1);
}



/*===========================================================================*
 * chkfile() - Check a file for correct syntax
 *
 * This subroutine calls pstate() for each character obtained.
 *===========================================================================*/
static int chkfile(sfile)

FILE *sfile;                   /* Input source file */

{
  int   rc;                    /* Return code */
  int   c;                     /* Character obtained from get_char() */

  linenum = 0L;                /* Initialize line number to zero, */
  cptr = lbuff;                /* Point cptr to line buffer, and */
  lbuff[0] = '\0';             /* Store null string, to set up get_char() */

  state = Text;                /* Initialize state to 'text mode' */

  for (;;)                     /* For each character in the input file: */
  {
     c = get_char();                /* Get the character */
     rc = pstate(c);                /* Process state machine for character */
     if (rc != 0)                   /* If quit returned: */
        break;                           /* Break out of loop */
  }

  return(0);
}

/*===========================================================================*
 * get_char() - Get next character from 'sfile'.
 *
 * Returns:
 *   -1 if end of file.
 *   Otherwise, the next character from 'sfile' is returned.
 *===========================================================================*/
static int get_char()
{
  int   rc;                    /* Return code */
  char *s;                     /* Return from fgets() */

  if (*cptr == '\0')           /* If at end of input line string: */
  {
     s = fgets(lbuff, BUFFLEN,      /* Get next line */
                sfile);
     if (s == NULL)                 /* If no more: */
        return(-1);                    /* Return end-of-file */

     cptr = lbuff;                  /* Point cptr to start of line buffer */
     linenum++;                     /* Increment line number */
  }

  rc = *cptr;                  /* Get (next) character from line buffer */
  cptr++;                      /* Bump character pointer */

  return(rc);                  /* Return the character */
}

/*===========================================================================*
 * put_char() - Puts back previous character.
 *
 * This routine is only guarenteed to put back the last character obtained
 * by a previous call to get_char().
 *===========================================================================*/
static int put_char()
{
  int   rc;                    /* Return code */

  if (cptr > lbuff)            /* If cptr is past the start of lbuff: */
     cptr--;                   /* It's safe to back it up! */

  return(0);
}

/*===========================================================================*
 * errmsg() - Write error message to stderr.
 *===========================================================================*/
static void errmsg(line, msg)

long  line;                    /* Line number within file */
char *msg;                     /* Pointer to error message */
{
   fprintf(stderr,
           "%s: file '%s' line %ld: %s\n",
           pgm, fname, line, msg);

  return;                      /* Return */
}

/*===========================================================================*
 * pstate() - Process state machine.
 *
 * This subroutine checks the state in 'state' and the character passed to it.
 * It performs a specific action and advances the 'state' variable to the
 * next state.
 *
 * When some errors are found (including when a -1 character is found), the
 * state machine issues a 'return(1)' to tell the calling program to stop
 * processing this file.  If no return is specified, then the state machine
 * issues a 'return(0)' by default, telling the calling program that it's ok
 * to continue.
 *
 * Returns:
 *   0, if it is ok to continue.
 *   1, if input file processing should stop.
 *===========================================================================*/
static int pstate(ch)

int ch;                        /* Character obtained from the input file */

{
  int   rc;                    /* Return code */

  switch (state)
  {
     /*======================================================================*/
     case Text:                /* In normal text mode */
        switch (ch)
        {
           case -1:                 /* No more characters to read: Done! */
              return(1);
              break;

           case '"':                /* Double quote found */
              start_line = linenum;
              state = String;
              break;

           case '\'':               /* Single quote found */
              start_line = linenum;
              state = CharConst;
              break;

           case '/':                /* Possible start of comment found */
              start_line = linenum;
              state = PStartComment;
              break;

           case '*':                /* Possible unexpected end of comment found */
              state = PUEndComment;
              break;

           default:                 /* Anything else */
              break;
        }
        break;

     /*======================================================================*/
     case String:              /* Inside double-quote string */
        switch (ch)
        {
           case -1:                 /* No more characters to read */
              errmsg(start_line, "String is not terminated before end of file.");
              return(1);
              break;

           case '\n':               /* Newline character found */
              errmsg(start_line, "String is not terminated before end of line.");
              state = Text;
              break;

           case '"':                /* Double quote found */
              state = Text;
              break;

           case '\\':               /* Escape character found */
              state = StringEsc;
              break;

           default:                 /* Anything else */
              break;
        }
        break;

     /*======================================================================*/
     case StringEsc:           /* Inside escape sequence of a double-quote string */
        switch (ch)
        {
           case -1:                 /* No more characters to read */
              errmsg(start_line, "String is not terminated before end of file.");
              return(1);
              break;

           default:                 /* Anything else */
              state = String;
              break;
        }
        break;

     /*======================================================================*/
     case CharConst:           /* Inside single-quote string */
        switch (ch)
        {
           case -1:                 /* No more characters to read */
              errmsg(start_line, "Character constant is not terminated before end of file.");
              return(1);
              break;

           case '\n':               /* Newline character found */
              errmsg(start_line, "Character constant is not terminated before end of line.");
              state = Text;
              break;

           case '\'':               /* Single quote found */
              state = Text;
              break;

           case '\\':               /* Escape character found */
              state = CharConstEsc;
              break;

           default:                 /* Anything else */
              break;
        }
        break;

     /*======================================================================*/
     case CharConstEsc:        /* Inside escape sequence of a single-quote string */
        switch (ch)
        {
           case -1:                 /* No more characters to read */
              errmsg(start_line, "Character constant is not terminated before end of file.");
              return(1);
              break;

           default:                 /* Anything else */
              state = CharConst;
              break;
        }
        break;

     /*======================================================================*/
     case PUEndComment:        /* Possible unexpected end of comment */
        switch (ch)
        {
           case -1:                 /* No more characters to read */
              return(1);
              break;

           case '/':                /* End of comment was found */
              errmsg(linenum, "Unexpected end-of-comment found.");
              state = Text;
              break;

           default:                 /* Anything else */
              put_char();                /* Put character back */
              state = Text;
              break;
        }
        break;

     /*======================================================================*/
     case PStartComment:       /* Possible start of comment */
        switch (ch)
        {
           case -1:                 /* No more characters to read */
              return(1);
              break;

           case '*':                /* Start of comment was found */
              state = Comment;
              break;

           case '/':                /* Start of 'EOL' comment was found */
              state = EOLComment;
              break;

           default:                 /* Anything else */
              put_char();                /* Put character back */
              state = Text;
              break;
        }
        break;

     /*======================================================================*/
     case Comment:             /* Inside a comment */
        switch (ch)
        {
           case -1:                 /* No more characters to read */
              errmsg(start_line, "Comment is not terminated before end of file.");
              return(1);
              break;

           case '*':                /* Possible end of comment was found */
              state = PEndComment;
              break;

           case '/':                /* Possible start of nested comment was found */
              state = PNestComment;
              break;

           default:                 /* Anything else */
              break;
        }
        break;

     /*======================================================================*/
     case PEndComment:         /* Possible end of comment */
        switch (ch)
        {
           case -1:                 /* No more characters to read */
              errmsg(start_line, "Comment is not terminated before end of file.");
              return(1);
              break;

           case '/':                /* End of comment was found */
              state = Text;
              break;

           case '*':                /* Possible end of comment found */
              state = PEndComment;
              break;

           default:                 /* Anything else */
              state = Comment;
              break;
        }
        break;

     /*======================================================================*/
     case PNestComment:        /* Possible start of nested comment */
        switch (ch)
        {
           case -1:                 /* No more characters to read */
              errmsg(start_line, "Comment is not terminated before end of file.");
              return(1);
              break;

           case '*':                /* Start of nested comment was found */
              errmsg(start_line, "Comment started.");
              errmsg(linenum, "Nested comment started.");
              state = Comment;
              break;

           case '/':                /* Possible start of nested comment was found */
              state = PNestComment;
              break;

           default:                 /* Anything else */
              state = Comment;
              break;
        }
        break;

     /*======================================================================*/
     case EOLComment:          /* Inside a comment that stops at end of line */
        switch (ch)
        {
           case -1:                 /* No more characters to read */
              return(1);
              break;

           case '\n':               /* End of line (EOL) was found */
              state = Text;
              break;

           default:                 /* Anything else */
              break;
        }
        break;

     /*======================================================================*/
     default:
        errmsg(linenum, "INTERNAL ERROR: Invalid state encountered.");
        return(1);
        break;
  }

  return(0);                   /* Return and tell caller to keep going */
}
