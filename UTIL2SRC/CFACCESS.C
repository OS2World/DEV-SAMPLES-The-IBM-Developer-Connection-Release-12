/*============================================================================*
 * module: cfaccess.c - Configuration file access subroutines.
 *
 * (C)Copyright IBM Corporation, 1990, 1991, 1992.        Brian E. Yoder
 *
 * This module contains the following externally-available subroutines:
 *
 *      cfopen()        cfsetfile()
 *      cfread()        cfclose()       cfgetbyname()   cfreadfile()
 *      cfstrcmpi()
 *
 * The cfopen() subroutine opens a configuration file.  Only one configuration
 * file at a time may be processed.  If a configuration file is still open
 * from a previous call, cfopen() closes it before it opens the new file.
 *
 * The cfsetfile() subroutine allows you to process a file that is already
 * open (such as stdin).  Again, if a configuration file is already open,
 * cfsetfile() closes it before storing the FILE pointer of the new file.
 *
 * The cfread() subroutine reads the next line from the configuration file
 * that contains tokens. It returns the number of tokens in the line and
 * copies a pointer to an array of pointers to the null-terminated tokens
 * to the calling program.
 *
 * The cfreadfile() subroutine reads the next 'file' line from the configuration
 * file.  It returns a pointer to the name of the file.
 *
 * The cfclose() subroutine closes the configuration file.
 *
 * The cfgetbyname() subroutine searches a configuration file for the first
 * line that begins with a specified token.  It returns the number of tokens
 * in this line and copies two pointers to the calling program: a pointer to
 * the array of token pointers, and one to the first token past the first '='.
 * The cfgetbyname() subroutine calls cfopen() for you, so you cannot use
 * cfsetfile() to specify an already-opened file.
 *
 * The cfline() subroutine returns the current line number within the
 * configuration file.  It can be called after cfread(), cfreadfile(), or
 * cfgetbyname().
 *
 * NOTES:
 *
 * The cfread(), cfreadfile(), and cfgetbyname() subroutines copy
 * pointers to static data that is overwritten with each subsequent call.
 *
 * The cfstrcmpi() subroutine performs a case-insensitive string comparison.
 * It is only needed for AIX -- the C/2 (DOS, OS/2) library has its own.
 *
 * 03/21/90 - Created.
 * 03/26/90 - Initial version, for PS/2 AIX.
 * 05/25/90 - Added cfline() subroutine to return current line number.
 * 07/24/90 - Fixed gettoken() to handle unterminated strings properly.
 * 11/02/90 - Added cfstrcmpi() subroutine.
 * 03/25/91 - Added the cfsetfile() subroutine, to allow processing of stdin.
 *            Also, various comments were updated.
 * 04/03/91 - Ported from AIX to DOS C/2.  Files are opened in text mode.
 * 11/17/91 - Strings may now be enclosed in square brackets as well as quotes.
 *            Comments may begin with a semicolon as well as with a # character.
 *            These changes allow these routines to process clear-text .INI
 *            files that are often used by DOS, OS/2, and MS Windows software.
 * 04/13/92 - Removed a benign nested comment so ccmt won't complain.
 * 07/24/92 - Changed "rt" to "r" for fopen(). C Set/2 doesn't allow "rt".
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>

#include "util.h"

/*============================================================================*
 * Function prototypes for subroutines used only by this module
 *============================================================================*/

static char *strlwr       ( char * );
static void  parseline    ( void );
static char *gettoken     ( void );

/*============================================================================*
 * Static data for this module only
 *============================================================================*/

#define LINE     1024               /* Maximum line length */
#define MAX_TOK   128               /* Maximum pointers in tokv array */

static FILE   *cfile = NULL;        /* Currently open file */

static char   *tnext;               /* Set by parseline(), updated by gettoken() */
static char   *nexttok;             /* Set by parseline(), updated by gettoken() */
static char    tline[LINE+1];       /* Buffer to hold line from file */
static ulong   lineno = 0L;         /* Current line number */

static char    eqsign[] = "=";      /* Equal (assignment) '=' sign */

static int     tokc;                /* No. of tokens found in line */
static char   *tokv[MAX_TOK+1];     /* Array of pointers to tokens */

static int     IsApp = FALSE;       /* Is line an application in an .INI file? */

/*============================================================================*
 * cfopen() - Opens a configuration file.
 *
 * PURPOSE:
 *   This subroutine opens a configuration file for reading.
 *
 * REMARKS:
 *   You should use the subroutines provided in this API set for actually
 *   reading data from the configuration file.
 *
 *   If a configuration file is already open, this subroutine closes it
 *   before opening the new file.
 *
 *   When you are done reading the file, you should call the cfclose()
 *   subroutine.
 *
 * RETURNS:
 *   0, if the file was opened successfully.
 *   1, if the file could not be opened.
 *============================================================================*/
int cfopen(cfname)

char  *cfname;

{
  if (cfile != NULL)                /* If a file is already open: */
     fclose(cfile);                 /*    Close it! */

  tokc = 0;                         /* Initialize token count and array to 'none' */
  *tokv = NULL;

  lineno = 0L;                      /* Set line number to zero */
  IsApp = FALSE;                    /* Reset IsApp */

  cfile = fopen(cfname, "r");       /* Attempt to open the file */

  if (cfile == NULL)                /* Return with appropriate success status */
     return(1);
  else
     return(0);
}

/*============================================================================*
 * cfsetfile() - Sets the FILE * for an open file.
 *
 * PURPOSE:
 *   This subroutine allows the configuration file access subroutines to
 *   process a file that is already open.  You use this subroutine instead
 *   of the cfopen() subroutine.
 *
 *   Pass this subroutine the FILE pointer for a file that has already been
 *   opened for reading.  You can process standard input by passing the
 *   'stdin' FILE pointer.
 *
 * REMARKS:
 *   You should use the subroutines provided in this API set for actually
 *   reading data from the configuration file.
 *
 *   If a configuration file is already open, this subroutine closes it
 *   before setting the new file.
 *
 *   When you are done reading the file, you should call the cfclose()
 *   subroutine.
 *
 *   This subroutine does not check the file pointer (fp) for validity.
 *   You must be sure that you never pass a NULL or invalid pointer to
 *   this subroutine.
 *
 * RETURNS:
 *   0, always.
 *============================================================================*/
int cfsetfile(fp)

FILE  *fp;

{
  if (cfile != NULL)                /* If a file is already open: */
     fclose(cfile);                 /*    Close it! */

  tokc = 0;                         /* Initialize token count and array to 'none' */
  *tokv = NULL;

  lineno = 0L;                      /* Set line number to zero */
  IsApp = FALSE;                    /* Reset IsApp */

  cfile = fp;                       /* Save pointer to open file */
  return(0);                        /* Return */
}

/*============================================================================*
 * cfline() - Gets current line number within the configuration file.
 *
 * PURPOSE:
 *   This subroutine get the number of the line within the configuration
 *   file that the last access took place.
 *
 * REMARKS:
 *   This subroutine returns zero when called after cfopen() or cfsetfile()
 *   but before any calls to cfread(), cfreadfile(), or cfgetbyname().
 *
 * RETURNS:
 *   The number of the current line within the current configuration file.
 *============================================================================*/
ulong cfline()
{
  return(lineno);                   /* Return current line number */
}

/*============================================================================*
 * cfisapp() - Is the current line in the configuration file an application?
 *
 * PURPOSE:
 *   This subroutine checks to see whether or not the current line in the
 *   configuration file is an application in an .INI file.
 *
 *   An .INI file "application" is line whose first token is enclosed within
 *   square brackets, such as:
 *
 *        [Board 0]
 *
 * RETURNS:
 *   TRUE if the current line is an application, or FALSE if it's not.
 *============================================================================*/
int   cfisapp()
{
  return(IsApp);
}

/*============================================================================*
 * cfread() - Reads the next information line from the configuration file.
 *
 * PURPOSE:
 *   This subroutine reads the next line that contains one or more tokens.
 *
 * REMARKS:
 *   The pointer returned by this subroutine points to a static data area.
 *   This data area is overwritten by each call to this subroutine or by
 *   each call to the cfreadfile() subroutine.
 *
 * RETURNS:
 *   The number of tokens found in the line.  If the file is not open,
 *   a read error occurred, or we're at the end of the file, then
 *   zero is returned.  Normally, if a zero is returned, it is safe to
 *   assume that the end of the file has been reached.
 *
 *   On entry, 'tokvp' must point to a location that is to contain a pointer
 *   to an array of pointers to token strings.
 *
 *   The array contains pointers to each token found in the line.  A NULL
 *   pointer is stored in the array position just past the last token's
 *   pointer.
 *
 * EXAMPLE:
 *   The calling program can make the following data declarations on the stack:
 *
 *        int     tokc;
 *        char  **tokv;
 *
 *   The calling program can invoke this subroutine as follows:
 *
 *        tokc = cfread(&tokv);
 *
 *   The calling program can then use 'tokc' and 'tokv' just like the
 *   main() subroutine uses 'argc' and 'argv'.
 *============================================================================*/
int cfread(tokvp)

char  **tokvp[];                    /* Pointer to pointer to array of token pointers */

{
  char  *s;                         /* Returned by fgets() */

  /* Copy pointer to token array to caller */

  *tokvp = tokv;                    /* Copy pointer to array of token pointers */

  /* Be sure file is open */

  if (cfile == NULL)                /* If file is not open: */
  {
     tokc = 0;                            /* Set tokc = 'no tokens' */
     *tokv = NULL;                        /* Store no pointers in array */
     return(tokc);                        /* Return number of tokens found */
  }

  /* Loop to find next line of file that contains tokens */

  for (;;)                          /* Loop reading lines from file: */
  {
     s = fgets(tline, LINE,            /* Get next line from file */
                cfile);
     if (s == NULL)                    /* If at end of file: */
     {
        tokc = 0;                         /* Initialize token count and array to 'none' */
        *tokv = NULL;
        cfclose();                           /* Close file */
        return(tokc);                        /* Return number of tokens found */
     }

     lineno++;                         /* Increment line number */

     parseline();                      /* Parse the line */
     if (tokc != 0)                    /* If one or more tokens were found: */
        break;                              /* Break out of loop! */
  }                                 /* End of loop to look for lines with tokens */

  return(tokc);                     /* Return number of tokens found */
}

/*============================================================================*
 * cfreadfile() - Reads the 'file' line from the configuration file.
 *
 * PURPOSE:
 *   This subroutine reads the next valid line that contains 'file' as its
 *   first token.
 *
 * REMARKS:
 *   The pointer returned by this subroutine points to a static data area.
 *   This data area is overwritten by each call to this subroutine.
 *
 *   Valid lines must be formatted as follows:
 *
 *      file ftag = filename
 *
 *   where: 'file' is a constant, 'ftag' is ignored, and 'filename' is the
 *   name of the file.
 *
 *   Any line not formatted as shown is ignored.
 *
 * RETURNS:
 *   A pointer to the name of the file.  If a NULL pointer is returned,
 *   then it is normally safe to assume that we reached the end of the file.
 *============================================================================*/

#define FileTag        "file"       /* String that defines the 'file' tag */
#define FileTokCnt     4            /* Minimum no. of tokens in 'file' entry */
#define FileEqIndex    2            /* Index of '=' token */
#define FileNameIndex  3            /* Index of file name */

char *cfreadfile()

{
  int    count;                     /* No. of tokens pointers in array */
  char **vector;                    /* Pointer to array of token pointers */

  for (;;)                          /* Loop to find the next 'file' entry: */
  {
     count = cfread(&vector);            /* Get next line with tokens */
     if (count == 0)                     /* If at end of file: */
        return(NULL);                         /* Done: return NULL pointer */

     if (count < FileTokCnt)             /* Ignore line with too few tokens */
        continue;

     if (cfstrcmpi(*vector, FileTag) != 0)  /* Ignore line with wrong tag (1st token) */
        continue;

     if (strcmp(vector[FileEqIndex],     /* Ignore line with no '=' in proper place */
                eqsign) != 0)
        continue;

     return(vector[FileNameIndex]);      /* Ok!  Return pointer to file name */

  } /* end of for loop to find the next 'file' entry */
}

/*============================================================================*
 * cfclose() - Closes the previously opened configuration file.
 *
 * PURPOSE:
 *   This subroutine closes the previously opened configuration file.
 *   If the file is already closed, then no action is taken.
 *
 * REMARKS:
 *   This subroutine should be called whenever you are finished reading
 *   the configuration file.
 *
 * RETURNS:
 *   0, always (for now).
 *============================================================================*/
int cfclose()

{
  if (cfile != NULL)                /* If a file is open: */
     fclose(cfile);                 /*    Close it */

  cfile = NULL;                     /* Reset file pointer to NULL */
  IsApp = FALSE;                    /* Reset IsApp */

  return(0);                        /* For now: always return 0 */
}

/*============================================================================*
 * cfgetbyname() - Gets the tokens associated with a specified tag name.
 *
 * PURPOSE:
 *   This subroutine opens the configuration file and searches for the first
 *   line whose first token matches the specified "tagname".  The subroutine
 *   then closes the configuration file and returns pointers to the token
 *   information for the line.
 *
 * REMARKS:
 *   The pointer returned by this subroutine points to a static data area
 *   that is overwritten by each call to one of these subroutines.
 *
 *   Since this subroutine calls cfopen() for you, you cannot use the
 *   cfsetfile() subroutine that allows you to process an already-opened
 *   file such as stdin.
 *
 * RETURNS:
 *   The number of tokens found in the line.  If the file cannot be opened
 *   or no line begins with the specified "tagname", a zero is returned.
 *
 *   This subroutine also copies two additional pointers to the caller:
 *   A pointer to the array of pointers to tokens in the line, and
 *   a pointer to the first token immediately past the first '=' in the
 *   line.  If there is no '=' or if the '=' is the last token in the line,
 *   a NULL pointer is copied to the calling program.
 *
 * EXAMPLE:
 *   The calling program can make the following data declarations on the stack:
 *
 *        int     tokc;
 *        char  **tokv;
 *        char   *value;
 *
 *   The calling program can invoke this subroutine as follows to retrieve the
 *   information for the tag named "temp" in the specified "cfgfile":
 *
 *        tokc = cfgetbyname("cfgfile", "temp", &tokv, &value);
 *
 *   The calling program can then use 'tokc' and 'tokv' just like the
 *   main() subroutine uses 'argc' and 'argv'.
 *============================================================================*/
int cfgetbyname(fname, tagname, tokvp, value)

char   *fname;                      /* Pointer to name of configuration file */
char   *tagname;                    /* Pointer to (lowercase!) name of tag */
char  **tokvp[];                    /* Pointer to pointer to array of token pointers */
char  **value;                      /* Pointer to pointer to tag's value */

{
  int   rc;                         /* Return code storage */
  char *tagval;                     /* Pointer to tag value */
  int    count;                     /* No. of tokens pointers in array */
  char **vector;                    /* Pointer to array of token pointers */

  *tokvp = tokv;                    /* Store pointer to array of token pointers */

  rc = cfopen(fname);               /* Open the configuration file */
  if (rc != 0)                      /* If we can't open the file: */
  {
     *value = NULL;                      /* Copy NULL pointer to value */
     tokc = 0;                           /* Initialize token count and array to 'none' */
     *tokv = NULL;
     cfclose();                          /* Close the file */
     return(0);                          /* Return 'tag not found' */
  }

  for (;;)                          /* Scan file for 'tagname': */
  {
     count = cfread(&vector);            /* Get next line with tokens */
     if (count == 0)                     /* If at end of file: */
     {
        tokc = 0;                             /* Initialize token count and array to 'none' */
        *tokv = NULL;
        cfclose();                            /* Close the file */
        return(0);                            /* Done: return 'not found' */
     }

     if (cfstrcmpi(*vector, tagname) == 0)  /* If we found the tag: */
        break;                                /* Break out! */
  }

  for (;;)                          /* Look for first token past first '=': */
  {
     vector++;                           /* Point to next token */

     if (*vector == NULL)                /* If at end of array: */
        break;                                /* Break out */

     if (strcmp(*vector, eqsign) == 0)   /* If we found a '=': */
     {
        vector++;                             /* Point vector to next token pointer */
        break;                                /* Break out */
     }
  }

  cfclose();                        /* Close the configuration file */

  *value = *vector;                 /* Copy pointer to tag's value */
  return(count);                    /* Return no. of tokens in line */
}

/*============================================================================*
 * strlwr() - Converts uppercase characters in a string to lowercase.
 *
 * PURPOSE:
 *   Converts any uppercase characters in 'str' to lowercase.  Other
 *   characters are not affected.
 *
 * RETURNS:
 *   Pointer to 'str'.
 *============================================================================*/

static char *strlwr(str)

char *str;                   /* Pointer to string */

{
  while (*str != '\0')       /* Until we reach the end of the string: */
  {
     *str = tolower(*str);        /* Convert character to lowercase */
     str++;                       /* Point 'str' to next character */
  }

  return(str);
}

/*============================================================================*
 * parseline() - Parses a line into individual tokens.
 *
 * PURPOSE:
 *   Parses the line pointed to by 'tline' and breaks it up into tokens.
 *
 * ON ENTRY: The following variables must be set by the caller:
 *   tline = A complete line from the file.
 *
 * ON EXIT: The following variables are set by this subroutine:
 *   tokc  = Number of tokens found in the line (0 = none).
 *   tokv  = Array of pointers to the tokens.  A NULL pointer is stored
 *           after the last token pointer.  The first token is converted
 *           to lowercase.
 *   tline = Modified during the parsing process.
 *   IsApp = Is the current line an .INI file application?  This variable is
 *           initialized to FALSE and may be set to TRUE by gettoken().
 *============================================================================*/

static void parseline()

{
  char **next;                      /* Pointer to next token in array */
  char  *token;                     /* Pointer to a token */

  tokc = 0;                         /* Init: Set tokc = 'no tokens', and */
  *tokv = NULL;                     /* Store no pointers in array */

  IsApp = FALSE;                    /* Initialize: line is not an .INI app */

  tnext = tline;                    /* Point 'tnext' to start of line buffer */
  nexttok = NULL;                   /* NULL this pointer: see gettoken() */

  next = tokv;                      /* Point to first token pointer in array */

  while (tokc < MAX_TOK)            /* For each token found: */
  {
     token = gettoken();                /* Get the token */
     if (token == NULL) break;          /* If no more: done */

     *next = token;                     /* Store pointer to it in array */
     next++;                            /* Point to next entry in array */
     tokc++;                            /* Update token count */
  }

  *next = NULL;                     /* Terminate array with null pointer */

/*if (*tokv != NULL)  11/2/90*/     /* If first token pointer is not NULL: */
/*   strlwr(*tokv);   (bey)  */          /* Convert token to lowercase */

  return;                           /* Return */
}

/*============================================================================*
 * gettoken() - Gets the next token from the input line.
 *
 * PURPOSE:
 *   This subroutine starts searching at 'tnext' for the next token.
 *
 *   A token may consist of:
 *      A single equal '=' sign, or:
 *      One or more contiguous non-whitespace characters, or
 *      A series of characters enclosed by either single or double quotes.
 *      A series of characters enclosed in square brackets.
 *
 * RETURNS:
 *   Pointer to the token.  If no token was found, then a NULL pointer
 *   is returned.
 *
 * DATA USED:
 *   tnext = Pointer to next character in 'tline' to scan.
 *           The caller must initially set tnext = tline.  This subroutine
 *           updates 'tnext' as it scans for tokens.
 *   nexttok = Pointer to the next token, or NULL if there is none.
 *           This subroutine terminates a nonstring token with a null character.
 *           If the terminating character is a '=' instead of whitespace,
 *           then 'nexttok' is set with a pointer to "=", so we don't
 *           forget about it!
 *   tokc  = Number of tokens found so far.
 *   IsApp = If we're processing the first token (tokc==0) and it is delimited
 *           by square brackets, then we set the IsApp flag to TRUE.
 *============================================================================*/

static char *gettoken()

{
  char  *starttok;                  /* Pointer to start of token */
  char   ch;                        /* Character from the line */
  char   strch;                     /* String delimiter character */

  if (nexttok != NULL)              /* If we have a previously stored token: */
  {
     starttok = nexttok;                 /* Store pointer to it */
     nexttok = NULL;                     /* Then null it */
     return(starttok);                   /* Return pointer to token */
  }

  for (;;)                          /* To get the next token (if any), loop */
  {                                 /* getting characters, stop at the end of */
     ch = *tnext;                   /* line or comment, and skip leading white- */
     switch (ch)                    /* space */
     {
                                    /*----------------------------------------*/
        case '\0':                  /* If end of line buffer, or comment */
        case '#':
        case ';':
           return(NULL);                 /* Done: no more tokens */
           break;
                                    /*----------------------------------------*/
        case ' ':                   /* Skip over whitespace characters */
        case '\t':
        case '\n':
        case '\f':
           tnext++;                      /* Point to next character in line */
           break;
                                    /*----------------------------------------*/
        case '=':                   /* Special case: '=' sign found: */
           tnext++;                      /* Bump pointer past the '=' */
           return(eqsign);               /* Return pointer to token string */
           break;
                                    /*----------------------------------------*/
        case '[':                   /* Left bracket found: Change it to a right */
           ch = ']';                     /* bracket and fall thru so it will be */
           if (tokc == 0)                /* stored as the string end delimiter */
              IsApp = TRUE;              /* If 1st token, it's an .INI "application" */
        case '\'':                  /* Single or double quote found: */
        case '"':
           strch = ch;                   /* Store the string end delimiter char */
           tnext++;                      /* Point tnext to 1st char of string */
           starttok = tnext;             /* And store pointer */
           for (;;)                      /* Loop to find end of string: */
           {
              ch = *tnext;                    /* Get next character */

              if ((ch == strch) ||            /* If ch is string delimiter */
                  (ch == '\n'))               /* or the newline char: */
              {
                 *tnext = '\0';                  /* Null the string term char */
                 tnext++;                        /* Set up for next call */
                 return(starttok);               /* Return pointer to token */
              }

              if (ch == '\0')                 /* If at the end of input:  */
                 return(starttok);            /* string isn't terminated: return */

              tnext++;                        /* Point to next char and repeat */
           }                             /* End of loop to get string token */
           break;
                                    /*----------------------------------------*/
        default:                    /* Beginning of nonstring token found */
           starttok = tnext;             /* Store pointer to start of token */
           for (;;)                      /* Loop to find end of token: */
           {
              tnext++;                        /* Point to next character */
              ch = *tnext;                    /* Get the character */
              switch (ch)
              {
                 case ' ':                         /* If whitespace: */
                 case '\t':
                 case '\n':
                 case '\f':
                    *tnext = '\0';                      /* Null the character */
                    tnext++;                            /* Set up for next call */
                    return(starttok);                   /* Return pointer to token */
                    break;

                 case '\0':                        /* If end of line: */
                    return(starttok);                   /* Return pointer to token */
                    break;

                 case '=':                         /* If '=' sign: it's a token too: */
                    *tnext = '\0';                      /* Terminate the token */
                    tnext++;                            /* Set up for next call */
                    nexttok = eqsign;                   /* Let next iteration know about */
                    return(starttok);                   /* the = token */
                    break;
              }
           }                             /* End of loop to get non-string token */
           break;
     } /* end of switch (ch) statement */
  } /* end of for loop to get the next token */
} /* end of gettoken() */

/*============================================================================*
 * cfstrcmpi() - Compares two strings (case-insensitive).
 *
 * PURPOSE:
 *   This subroutine compares two strings without regard for case.  Therefore,
 *   the following strings would compare as equal:
 *          "AbcDef"  "abcdef"
 *
 * RETURNS:
 *   0, if the two strings compare as equal.
 *   1, if they are different.
 *============================================================================*/
int cfstrcmpi(s1, s2)

char  *s1;
char  *s2;

{
   register char c1, c2;            /* Storage for characters */

   for (;;)                         /* Loop to compare strings: */
   {
      c1 = *s1;                          /* Get next character from each string */
      c2 = *s2;

      if ((c1 == 0) && (c2 == 0))        /* If at end of both: */
         return(0);                           /* Strings are the same */

      if (c1 == 0) return(1);            /* If at end of only one string: */
      if (c2 == 0) return(1);                 /* Strings are different */

      c1 = tolower(c1);                  /* Convert characters to lowercase */
      c2 = tolower(c2);

      if (c2 != c1) return(1);           /* Return if strings are different */

      s1++;                              /* Bump pointers to next character */
      s2++;                              /* in each string */
   }
}
