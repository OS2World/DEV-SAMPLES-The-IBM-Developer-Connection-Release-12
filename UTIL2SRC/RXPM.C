/*============================================================================*
 * module: rxpm.c - Regular expression and pattern-matching subroutines
 *
 * (C)Copyright IBM Corporation, 1989, 1990, 1991.        Brian E. Yoder
 *
 * Externally-available functions contained in this module:
 *   rexpand()  - Expand pattern-matching expression into a regular expression.
 *   rcompile() - Compile a regular expression.
 *   rmatch()   - Match a regular expression.
 *   rscan()    - Scan for a regular expression.
 *   rcmpmsg()  - Get error message based on error from rcompile().
 *
 * These subroutines use a modified version of the regexp.h header file
 * that is supplied with PS/2 AIX.
 *
 * The regexp.h header file sets the static variable 'circf' whenever its
 * compile() subroutine is called.  This variable is used by its step()
 * subroutine and should be set to the value it had when the expression
 * was compiled.  Therefore:
 *
 * The rcompile() subroutine in this module stores the value of 'circf'
 * in the first 'sizeof(int)' bytes of the compiled-expression buffer.
 * The rscan() subroutine extracts this value of 'circf' before calling
 * the step() subroutine.
 *
 * 12/04/89 - Created by splitting the subroutines from pmt.c
 * 12/05/89 - Initial version.
 * 12/08/89 - In rmatch(), 'circf' is set to 1 to force a match starting
 *            at the beginning of the string.  This improves performance
 *            in the case of a failed match.
 * 05/25/90 - Include libutil.h instead of rxpm.h.
 * 04/04/91 - Ported from AIX to DOS and C/2. Includes <stdlib.h> and "util.h"
 * 06/16/92 - rexpand() now changes a \! sequence into a !
 * 07/22/92 - Convert function definitions to the new style.
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "util.h"

/*============================================================================*
 * Regular expression data and macros needed by regexp.h
 *============================================================================*/

static char *regexpr;          /* Pointer to regular expression to compile */
static char *after_cexpr;      /* Pointer past compiled expression */

#define INIT        char *sp=regexpr;
#define GETC()      (*sp++)
#define PEEKC()     (*sp)
#define UNGETC(c)   (--sp)
#define RETURN(c)   {after_cexpr = c; return(0);}
#define ERROR(x)    return(x)

#include "regexp.h"            /* Modified version of AIX's header file */

/*============================================================================*
 * rexpand() - Expand pattern-matching expression into a regular expression.
 *
 * Description:
 *   The pattern matching expression pointed to by 'expr' is expanded into
 *   a full regular expression and stored in the buffer pointed to by
 *   'rexpr'.  The 'erexpr' pointer should point just past the buffer in which
 *   the expanded expression is to be stored.  Therefore, the maximum length
 *   allowed for the expanded expression (including the null terminating byte!)
 *   is (erexpr - rexpr) bytes.
 *
 * The following patterns are supported:
 *
 * Pattern      What it matches                    Examples
 * -------      ------------------------------     ----------------------------
 *
 *   *          Matches any string, including      *ab* matches ab, blab,
 *              the null string.                   and babies.
 *
 *   ?          Matches any single character.      ab?c matches any string
 *                                                 that is 4 characters
 *                                                 long, begins with ab,
 *                                                 and ends with c.
 *
 *   [...]      Matches any one of the             [ABC]* matches any string
 *              enclosed characters.               that begins with A, B, or C.
 *
 *   [.-.]      Matches any character              [A-Z]* matches any string
 *              between the enclosed pair,         that begins with an uppercase
 *              inclusive (range).                 letter.
 *
 *                                                 [a-zA-Z]* matches any string
 *                                                 that begins with a letter.
 *
 *   [!...]     Matches any single character       [!XYZ]* matches any string
 *              except one of those enclosed.      that does not begin with
 *                                                 X, Y, or Z.
 *
 *   Enclosed characters can be combined with ranges.  Thus, [abcm-z]* matches
 *   any string that begins with a, b, c, or m through z.
 *
 *   Additionally, any pattern may be followed by:
 *     {m}      Matches exactly m occurrences of the pattern.
 *     {m,}     Matches at least m occurrences of the pattern.
 *     {m,n}    Matches at least m but no more than n occurrences of the pattern.
 *
 *              m and n must be integers from 0 to 255, inclusive.
 *
 *   The expression [a-zA-Z][0-9a-zA-Z]{0,} matches any string that begins
 *   with a letter and is followed only by letters or numbers.
 *
 *   The expression [-+]{0,1}[0-9]{1,} matches any string that optionally
 *   begins with a - or + sign and consists of one or more digits.
 *
 *   The expression "the {1,}cat" matches any string that begins with 'the',
 *   ends with 'cat', and has one or more spaces in the middle.
 *
 *   The expression "[!.]{1,}" matches any string that does not contain a period.
 *
 * Notes:
 *   If the pattern contains braces {} or a !, then 'escape' each brace by
 *   putting a '\' before it.
 *
 * Returns:
 *   0, if successful.
 *   REGX_FULL, if the expanded expression won't fit in the buffer.
 *
 *   If successful, 'endp' contains a pointer to the location that is one byte
 *   past the null terminating byte of the expanded expression.  If error,
 *   then 'endp' contains a pointer to 'rexpr'.
 *============================================================================*/

#define STORE(ch) if (to >= erexpr) return(REGX_FULL); *to++ = ch;

int rexpand(expr, rexpr, erexpr, endp)

char *expr;                    /* Pointer to pattern-matching expression */
char *rexpr;                   /* Pointer to expansion buffer */
char *erexpr;                  /* Pointer past end of expansion buffer */
char **endp;                   /* Pointer to loc. in which to store pointer */
                               /* to byte just past expanded expression */

{
  char *from;                  /* Character pointers used to expand the */
  char *to;                    /*    expression */

  from = expr;                     /* Point to start of each expression */
  to = rexpr;

  *endp = rexpr;                   /* Initialize 'endp' to 'rexpr */

  if (rexpr >= erexpr)             /* If end pointer doesn't allow at least */
     return(REGX_FULL);            /* one character: return w/error */

  while (*from != '\0')            /* For each character in 'expr': */
  {
     switch (*from)                    /* Handle special cases: */
     {
        case '.':                            /* Put '\' before */
        case '$':                            /* these characters: */
        case '{':
        case '}':
           STORE('\\');
           STORE(*from);
           break;

        case '\\':                           /* If escape character found: */
           from++;                                /* Point to next character */
           switch (*from)
           {
              case '{':                           /* Store these characters */
              case '}':                           /* without the escape char: */
              case '$':
              case '!':
                 STORE(*from);
                 break;

              default:                            /* Otherwise: store the esc */
                 STORE('\\');                     /* character, too */
                 STORE(*from);
                 break;
           }
           break;

        case '*':                            /* Put '.' before a '*' */
           STORE('.');
           STORE(*from);
           break;

        case '?':                            /* Put '.' in place of '?' */
           STORE('.');
           break;

        case '!':                            /* Put '^' in place of '!' */
           STORE('^');
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
 * rcompile() - Compile a regular expression.
 *
 * Description:
 *   The regular expression pointed to by 'expr' is compiled and stored in
 *   the buffer pointed to by 'cexpr'.
 *
 *   The 'ecexpr' pointer should point just past the buffer in which
 *   the compiled expression is to be stored.  Therefore, the maximum length
 *   of the compiled expression is (ecexpr - cexpr) bytes.
 *
 *   The expression 'expr' being compiled is terminated by the 'endc' character.
 *   Normally, this is the '\0' character, but it can be any character you choose.
 *
 * Returns:
 *   0, if successful.  Otherwise, an error occurred.  The rcmpmsg() subroutine
 *   returns a pointer to an error message based upon this return code.
 *
 *   If successful, 'endp' contains a pointer to the location one byte
 *   past the compiled expression.  If an error occurred, then 'endp' points
 *   to the beginning of the compiled expression buffer 'cexpr'.
 *
 * Notes:
 *   The value of 'circf' (set by the compile() subroutine in regexp.h) is
 *   stored as an integer in the first 'sizeof(int)' bytes of the compiled-
 *   expression buffer.  The actual compiled expression immediately follows
 *   this value.
 *============================================================================*/
int rcompile(

char *expr   ,                 /* Pointer to pattern-matching expression */
char *cexpr  ,                 /* Where compiled expression is to be stored */
char *ecexpr ,                 /* Pointer past end of compiled expr. buffer */
char  endc   ,                 /* Character used to terminate the expression */
char **endp  )                 /* Indirect pointer to end of compiled expr */

{
  int   rc;                    /* Return code storage */

  regexpr = expr;              /* Point 'regexpr' to the regular expression */
  *endp = cexpr;               /* Initialize 'endp' to start of compile buffer */

  if (cexpr >= ecexpr)         /* Check pointer to end of compile buffer */
     return(REGX_FULL);

  if ((ecexpr - cexpr) < sizeof(int))  /* Be sure we have enough space for the */
     return(REGX_FULL);                /* 'circf' value */

  rc = compile(regexpr,        /* Compile the reg. expression */
          cexpr + sizeof(int),
          ecexpr, endc);

  *(int *)cexpr = circf;       /* Store value of circf */

  if (rc == 0)                 /* If successful:  Store pointer to location */
     *endp = after_cexpr;      /* past the end of the compiled expression */

  return(rc);                  /* Return w/error indication */
}

/*============================================================================*
 * rmatch() - Match a regular expression.
 *
 * Description:
 *   The compiled regular expression pointed to by 'cexpr' is compared with
 *   the string pointed to by 'str'.
 *
 * Returns:
 *   RMATCH, if the compiled regular expression matched the entire string.
 *   NO_RMATCH, otherwise.
 *
 * Note:
 *   The value of 'circf' is set to 1 to force the step() subroutine to
 *   match the beginning of the string.
 *============================================================================*/
int rmatch(

char *cexpr ,                  /* Pointer to compiled regular expression */
char *str   )                  /* Pointer to string to match */

{
  int   rc;                    /* Return code storage */

  circf = 1;                            /* Set value of 'circf' to 1 */

  rc = step(str, cexpr + sizeof(int));  /* Scan for cexpr within 'str' */

  if (rc == 0)                          /* If it's not in the string: */
     return(NO_RMATCH);                       /* Return 'no match' */

  if ((*loc2 == '\0') && (loc1 == str)) /* If we matched entire string: */
     return(RMATCH);                          /* Return 'match' */
  else                                  /* Otherwise: */
     return(NO_RMATCH);                       /* Return 'no match' */
}

/*============================================================================*
 * rscan() - Scan for a regular expression.
 *
 * Description:
 *   The compiled regular expression pointed to by 'cexpr' is compared with
 *   the string pointed to by 'str'.
 *
 * Returns:
 *   RMATCH, if the compiled regular expression is found within the string.
 *   NO_RMATCH, otherwise.
 *
 *   If RMATCH is returned, 'startp' contains a pointer to the first character
 *   matched and 'endp' contains a pointer to the location immediately past
 *   the last character matched.  Therefore, the total number of characters
 *   matched is (*endp - *startp).
 *
 * Note:
 *   The first 'sizeof(int)' bytes of the compiled expression must contain
 *   the value of 'circf' at the time the expression was compiled.
 *============================================================================*/
int rscan(

char  *cexpr  ,                /* Pointer to compiled regular expression */
char  *str    ,                /* Pointer to string to match */
char **startp ,                /* Start of match */
char **endp   )                /* End-of-match + 1 */

{
  int   rc;                    /* Return code storage */

  circf = *(int *)cexpr;                /* Restore value of 'circf' */

  rc = step(str, cexpr + sizeof(int));  /* Scan for expr within 'str' */

  if (rc == 0)                          /* If it's not in the string: */
     return(NO_RMATCH);                       /* Return 'no match' */

  *startp = loc1;                       /* Store pointer to start of match */
  *endp = loc2;                         /* Store pointer past end of match */

  return(RMATCH);                       /* Return 'match' */
}

/*============================================================================*
 * rcmpmsg() - Get error message based on error from rcompile().
 *
 * Description:
 *   This subroutine gets the error message that corresponds to the value
 *   that is returned by the rcompile() subroutine.
 *
 * Returns:
 *   A pointer to the appropriate error message.  See the REGX_ #defined
 *   labels in "util.h".
 *============================================================================*/
char *rcmpmsg(int n)

{
  switch (n)
  {
     case 0:
        return("Successful");

     case 11:
        return("Range endpoint too large");
     case 16:
        return("Bad number");
     case 25:
        return("\\digit out of range");
     case 36:
        return("Illegal or missing delimiter");
     case 41:
        return("No remembered search string");
     case 42:
        return("() imbalance");
     case 43:
        return("Too many (");
     case 44:
        return("More than two numbers given in {}");
     case 45:
        return("} expected after \\");
     case 46:
        return("First number exceeds second in {}");
     case 49:
        return("[] imbalance");
     case 50:
        return("Regular expression overflow");
     case 51:
        return("Out of memory");
     default:
        return("Unknown error from compile()");
  }
}
