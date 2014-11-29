/*============================================================================*
 * module: text2bm.c - Text file to BookMaster source conversion.
 *
 * (C)Copyright IBM Corporation, 1990, 1991.              Brian E. Yoder
 *
 * This module contains code to convert ASCII text files to BookMaster
 * source files.  Many characters must be converted to BookMaster
 * symbols (for example: a ':' is converted to "&colon.").  Since the output
 * lines may have many multiple-character symbol names in them, they are
 * broken up (using the BookMaster continuation symbol) if necessary.
 *
 * For a complete list of BookMaster symbols, see IBM's "Publishing Systems
 * BookMaster: User's Guide", Publication number SC34-5009.
 *
 * 07/19/90 - Created.
 * 07/20/90 - Initial version of the text2bm() subroutine.
 * 08/07/90 - Changed " " to "&rbl." for character code 32 (space).
 * 02/05/91 - Changed ";" to "&semi.", since BookMaster sometimes interprets
 *            a semicolon as a line break.  Added the double-line box chars,
 *            left/right arrowheads (16, 17) , the up/down arrowheads (30, 31),
 *            and the arrows (24-27).
 * 02/14/91 - Changed character code 32 back to " " (space), to cut down on
 *            the number of &rbl. strings and make the output file readable.
 *            However, this means that if the last character in a line is a
 *            space, we need to change it into "&rbl.".  Then, if the next
 *            line begins with .ct (continuation), we won't lose those
 *            trailing spaces and misalign the output line.  (This is what
 *            I should have done for the 08/07/90 fix!)
 * 04/03/91 - Ported from AIX to DOS C/2.
 * 08/13/91 - Use BookMaster's &cont. instead of SCRIPT's .ct to handle line
 *            continuation.  Also, change OUT_MAX from 132 to 62, making it
 *            much shorter (a short value supposedly works better with &cont.).
 *            I tried it with OUT_MAX = 72 (not including continuation symbol),
 *            but it doesn't work: 72 is "too long".
 * 10/08/91 - Expand tab characters instead of ignoring them.
 * 10/17/91 - Added more symbols to the TextBM[] translation table.
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>

#include "util.h"

#define EXP_MAX 1024                /* Max. length of tab-expanded input line */

/*============================================================================*
 * Function prototypes for subroutines used only by this module
 *============================================================================*/

static int AddStr(char *, FILE *);

/*============================================================================*
 * The OUT_MAX label defines the maximum number of characters to put into
 * the line buffer.  If adding a string to the output file's line would cause
 * this length to be exceeded, then the line is written to the output file and
 * the next line is initialized with the continuation symbol
 *
 * The cont[] array contains SCRIPT's line continuation control word.
 *
 * The output line(s) are built in the line[] buffer.
 *============================================================================*/

#define OUT_MAX   62                /* Maximum output line length */

static  char cont[] = "&cont.";     /* Continuation control symbol */

#define CONT_LEN (sizeof(cont) - 1) /* Length not including null term. byte */

static  uint linelen;               /* Current output line length */

static  char line[OUT_MAX +         /* Line buffer: text + symbols + */
                  CONT_LEN +        /*              line continuation + */
                  32];              /*              safety buffer! */

/*============================================================================*
 * TextBM[] : ASCII CHARACTER-TO-STRING TRANSLATION TABLE.
 *
 * A character's 8-bit ASCII value is used as an index into this array.  The
 * pointer at an index points to a string that will cause BookMaster to
 * print the symbol.
 *
 * Note: This conversion array would be extremely tedious to edit with vi.
 *============================================================================*/

static char *TextBM[256] = {

/*                    |                           |                           |                           |*/
NULL         /* 0     |*/ , NULL         /* 1     |*/ , NULL         /* 2     |*/ , NULL         /* 3     |*/ ,
NULL         /* 4     |*/ , NULL         /* 5     |*/ , NULL         /* 6     |*/ , NULL         /* 7     |*/ ,
NULL         /* 8     |*/ , " "          /* 9 TAB |*/ , NULL         /* 10 LF |*/ , NULL         /* 11    |*/ ,
".pa"        /* 12 FF |*/ , NULL         /* 13 CR |*/ , NULL         /* 14    |*/ , "&sun."      /* 15   |*/ ,
"&rahead."   /* 16   |*/ , "&lahead."   /* 17   |*/ , NULL         /* 18    |*/ , NULL         /* 19    |*/ ,
NULL         /* 20    |*/ , NULL         /* 21    |*/ , NULL         /* 22    |*/ , NULL         /* 23    |*/ ,
"&uarrow."   /* 24   |*/ , "&darrow."   /* 25   |*/ , "&rarrow."   /* 26    |*/ , "&larrow."   /* 27   |*/ ,
NULL         /* 28    |*/ , NULL         /* 29    |*/ , "&uahead."   /* 30   |*/ , "&dahead."   /* 31   |*/ ,

" "       /* 32 space |*/ , "!"          /* 33  ! |*/ , "\""         /* 34  " |*/ , "#"          /* 35  # |*/ ,
"$"          /* 36  $ |*/ , "&percent."  /* 37  % |*/ , "&amp."      /* 38  & |*/ , "'"          /* 39  ' |*/ ,
"("          /* 40  ( |*/ , ")"          /* 41  ) |*/ , "*"          /* 42  * |*/ , "+"          /* 43  + |*/ ,
","          /* 44  , |*/ , "-"          /* 45  - |*/ , "&period."   /* 46  . |*/ , "/"          /* 47  / |*/ ,
"0"          /* 48  0 |*/ , "1"          /* 49  1 |*/ , "2"          /* 50  2 |*/ , "3"          /* 51  3 |*/ ,
"4"          /* 52  4 |*/ , "5"          /* 53  5 |*/ , "6"          /* 54  6 |*/ , "7"          /* 55  7 |*/ ,
"8"          /* 56  8 |*/ , "9"          /* 57  9 |*/ , "&colon."    /* 58  : |*/ , "&semi."     /* 59  ; |*/ ,
"<"          /* 60  < |*/ , "="          /* 61  = |*/ , ">"          /* 62  > |*/ , "?"          /* 63  ? |*/ ,

"@"          /* 64  @ |*/ , "A"          /* 65  A |*/ , "B"          /* 66  B |*/ , "C"          /* 67  C |*/ ,
"D"          /* 68  D |*/ , "E"          /* 69  E |*/ , "F"          /* 70  F |*/ , "G"          /* 71  G |*/ ,
"H"          /* 72  H |*/ , "I"          /* 73  I |*/ , "J"          /* 74  J |*/ , "K"          /* 75  K |*/ ,
"L"          /* 76  L |*/ , "M"          /* 77  M |*/ , "N"          /* 78  N |*/ , "O"          /* 79  O |*/ ,
"P"          /* 80  P |*/ , "Q"          /* 81  Q |*/ , "R"          /* 83  R |*/ , "S"          /* 83  S |*/ ,
"T"          /* 84  T |*/ , "U"          /* 85  U |*/ , "V"          /* 86  V |*/ , "W"          /* 87  W |*/ ,
"X"          /* 88  X |*/ , "Y"          /* 89  Y |*/ , "Z"          /* 90  Z |*/ , "&lbrk."     /* 91  [ |*/ ,
"&bsl."      /* 92  \ |*/ , "&rbrk."     /* 93  ] |*/ , "&caret."    /* 94  ^ |*/ , "_"          /* 95  _ |*/ ,

"&grave."    /* 96  ` |*/ , "a"          /* 97  a |*/ , "b"          /* 98  b |*/ , "c"          /* 99  c |*/ ,
"d"          /* 100 d |*/ , "e"          /* 101 e |*/ , "f"          /* 102 f |*/ , "g"          /* 103 g |*/ ,
"h"          /* 104 h |*/ , "i"          /* 105 i |*/ , "j"          /* 106 j |*/ , "k"          /* 107 k |*/ ,
"l"          /* 108 l |*/ , "m"          /* 109 m |*/ , "n"          /* 110 n |*/ , "o"          /* 111 o |*/ ,
"p"          /* 112 p |*/ , "q"          /* 111 q |*/ , "r"          /* 114 r |*/ , "s"          /* 115 s |*/ ,
"t"          /* 116 t |*/ , "u"          /* 117 u |*/ , "v"          /* 118 v |*/ , "w"          /* 119 w |*/ ,
"x"          /* 120 x |*/ , "y"          /* 121 y |*/ , "z"          /* 122 z |*/ , "&lbrc."     /* 123 { |*/ ,
"&bxv."      /* 124 | |*/ , "&rbrc."     /* 125 } |*/ , "&tilde."    /* 126 ~ |*/ , "&similar."  /* 127   |*/ ,

"&Cc."       /* 128 € |*/ , "&ue."       /* 129  |*/ , "&ea."       /* 130 ‚ |*/ , "&ac."       /* 131 ƒ |*/ ,
"&ae."       /* 132 „ |*/ , "&ag."       /* 133 … |*/ , "&ao."       /* 134 † |*/ , "&cc."       /* 135 ‡ |*/ ,
"&ec."       /* 136 ˆ |*/ , "&ee."       /* 137 ‰ |*/ , "&eg."       /* 138 Š |*/ , "&ie."       /* 139 ‹ |*/ ,
"&ic."       /* 140 Œ |*/ , "&ig."       /* 141  |*/ , "&Ae."       /* 142 Ž |*/ , "&Ao."       /* 143  |*/ ,
"&Ea."       /* 144  |*/ , "&aelig."    /* 145 ‘ |*/ , "&AElig."    /* 146 ’ |*/ , "&oc."       /* 147 “ |*/ ,
"&oe."       /* 148 ” |*/ , "&og."       /* 149 • |*/ , "&uc."       /* 150 – |*/ , "&ug."       /* 151 — |*/ ,
"&ye."       /* 152 ˜ |*/ , "&Oe."       /* 153 ™ |*/ , "&Ue."       /* 154 š |*/ , "&cent."     /* 155 › |*/ ,
"&Lsterling."/* 156 œ |*/ , "&yen."      /* 157  |*/ , "&peseta."   /* 158 ž |*/ , "&fnof."     /* 159 Ÿ |*/ ,

"&aa."       /* 160   |*/ , "&ia."       /* 161 ¡ |*/ , "&oa."       /* 162 ¢ |*/ , "&ua."       /* 163 £ |*/ ,
"&nt."       /* 164 ¤ |*/ , "&Nt."       /* 165 ¥ |*/ , "&aus."      /* 166 ¦ |*/ , "&ous."      /* 167 § |*/ ,
"&invq."     /* 168 ¨ |*/ , "&lnotrev."  /* 169 © |*/ , "&lnot."     /* 170 ª |*/ , "&frac12."   /* 171 « |*/ ,
"&frac14."   /* 172 ¬ |*/ , "&inve."     /* 173 ­ |*/ , "&odqf."     /* 174 ® |*/ , "&cdqf."     /* 175 ¯ |*/ ,
"&box14."    /* 176 ° |*/ , "&box12."    /* 177 ± |*/ , "&box34."    /* 178 ² |*/ , "&bxv."      /* 179 ³ |*/ ,
"&bxri."     /* 180 ´ |*/ , "&bx1012."   /* 181   |*/ , "&bx2021."   /* 182   |*/ , "&bx0021."   /* 183   |*/ ,
"&bx0012."   /* 184   |*/ , "&bx2022."   /* 185   |*/ , "&bx2020."   /* 186   |*/ , "&bx0022."   /* 187   |*/ ,
"&bx2002."   /* 188   |*/ , "&bx2001."   /* 189   |*/ , "&bx1002."   /* 190   |*/ , "&bxur."     /* 191 ¿ |*/ ,

"&bxll."     /* 192 À |*/ , "&bxas."     /* 193 Á |*/ , "&bxde."     /* 194 Â |*/ , "&bxle."     /* 195 Ã |*/ ,
"&bxh."      /* 196 Ä |*/ , "&bxcr."     /* 197 Å |*/ , "&bx1210."   /* 198   |*/ , "&bx2120."   /* 199   |*/ ,
"&bx2200."   /* 200   |*/ , "&bx0220."   /* 201   |*/ , "&bx2202."   /* 202   |*/ , "&bx0222."   /* 203   |*/ ,
"&bx2220."   /* 204   |*/ , "&bx0202."   /* 205   |*/ , "&bx2222."   /* 206   |*/ , "&bx1202."   /* 207   |*/ ,
"&bx2101."   /* 208   |*/ , "&bx0212."   /* 209   |*/ , "&bx0121."   /* 210   |*/ , "&bx2100."   /* 211   |*/ ,
"&bx1200."   /* 212   |*/ , "&bx0210."   /* 213   |*/ , "&bx0120."   /* 214   |*/ , "&bx2121."   /* 215   |*/ ,
"&bx1212."   /* 216   |*/ , "&bxlr."     /* 217 Ù |*/ , "&bxul."     /* 218 Ú |*/ , "&BOX."      /* 219 Û |*/ ,
"&BOXBOT."   /* 220 Ü |*/ , "&BOXLEFT."  /* 221 Ý |*/ , "&BOXRIGHT." /* 222 Þ |*/ , "&BOXTOP."   /* 223 ß |*/ ,

"&alpha."    /* 224 à |*/ , "&beta."     /* 225 á |*/ , "&Gamma."    /* 226 â |*/ , "&pi."       /* 227 ã |*/ ,
"&Sigma."    /* 228 ä |*/ , "&sigma."    /* 229 å |*/ , "&mu."       /* 230 æ |*/ , "&tau."      /* 231 ç |*/ ,
"&Phi."      /* 232 è |*/ , "&Theta."    /* 233 é |*/ , "&Omega."    /* 234 ê |*/ , "&delta."    /* 235 ë |*/ ,
"&infinity." /* 236 ì |*/ , "&phi."      /* 237 í |*/ , "&memberof." /* 238 î |*/ , "&intersect."/* 239 ï |*/ ,
"&identical."/* 240 ð |*/ , "&pm."       /* 241 ñ |*/ , "&ge."       /* 242 ò |*/ , "&le."       /* 243 ó |*/ ,
"&inttop."   /* 244 ô |*/ , "&intbot."   /* 245 õ |*/ , "&div."      /* 246 ö |*/ , "&nearly."   /* 247 ÷ |*/ ,
"&sup0."     /* 248 ø |*/ , "&lmultdot." /* 249 ù |*/ , "&smultdot." /* 250 ú |*/ , "&sqrt."     /* 251 û |*/ ,
"&supn."     /* 252 ü |*/ , "&sup2."     /* 253 ý |*/ , "&sqbul."    /* 254 þ |*/ , "&rbl."      /* 255   |*/

};

/*============================================================================*
 * text2bm() - Convert ASCII text to BookMaster source.
 *
 * PURPOSE:
 *   This subroutine processes a text string that represents a single line,
 *   converting each character (as needed) to its corresponding BookMaster
 *   symbol and writing one or more lines to the ouput file 'bmfile'.
 *
 *   Of course, output file must be opened for writing (or appending).
 *
 * REMARKS:
 *   This subroutine only works with 8-bit characters.  It uses the value of
 *   a character as an index into the TextBM[] array to determine the string
 *   that should be written to the output file.
 *
 *   If a NULL pointer is stored in the array, then nothing is written to the
 *   output file for that character.  For normal characters (letters, numbers),
 *   a one-character string is stored:  "a", "b", etc.  For other characters,
 *   a BookMaster symbol string is stored: "&bxul.", "&colon.", etc.
 *
 *   Formfeed characters are replaced by the ".pa" SCRIPT control word.  For
 *   this to work properly with SCRIPT, a formfeed character should exist in
 *   the input text line all by itself.
 *
 *   The resulting output lines created by this subroutine should be bracketed
 *   by the :xmp. and :exmp. BookMaster tags.
 *
 * RETURNS:
 *   0, if successful.
 *   1, if the output file is full.
 *============================================================================*/
int text2bm(textstr, bmfile)

char   *textstr;                    /* Input text string */
FILE   *bmfile;                     /* Output BookMaster source file */

{
  uint   idx;                       /* Counter used while expanding tabs */
  char  *cpi;                       /* Character pointers to input, output, */
  char  *cpo;                       /*    and end of output, used during tab */
  char  *cpoend;                    /*    expansion */
  uint   i;                         /* Index into conversion array */
  uchar *text;                      /* Pointer to string of unsigned chars */
  char  *str;                       /* Pointer to string */
  char   intext[EXP_MAX+1];         /* Buffer to hold line w/tabs expanded */

  if (bmfile == NULL)               /* If output file is not open: */
     return(1);                     /*    Return */

 /*------------------------------------------------------------------*
  * Store 'textstr' in intext[], expanding any tabs that are found
  *------------------------------------------------------------------*/

  idx = 0;                          /* Initialize tab expansion counter */
  cpi = textstr;                    /* Point cpi to start of input text string */
  cpo = intext;                     /* Point cpo to start of expanded text buffer */
  cpoend = cpo + EXP_MAX;           /* Point just past end of output text */

  for (;;)                          /* Loop to copy, and expand tabs: */
  {
     if (cpo >= cpoend) break;           /* Quit if output intext[] is full */

     if (idx != 0)                       /* If idx != 0: We're expanding tabs: */
     {
        *cpo = ' ';                           /* Store space in output */
        cpo++;                                /* Bump output pointer */
        idx--;                                /* Decrement tab count */
        continue;                             /* Repeat loop */
     }

     if (*cpi == '\t')                   /* If input character is a tab: */
     {
        idx = 7-((cpo-intext)%8);             /* Modulo division for tab count */
        *cpo = ' ';                           /* Store space in output */
     }
     else                                /* Else: Input character is NOT a tab: */
     {
        *cpo = *cpi;                          /* Simply copy character */
        if (*cpo == '\0') break;              /* Stop if at end of input */
     }

     cpi++;                              /* Step input char position */
     cpo++;                              /* And step output character position */
  } /* end of for loop to copy and expand tabs */

 /*------------------------------------------------------------------*
  * Write the intext[] array's characters to the output file, and
  * convert any special characters to BookMaster symbols, according
  * to the TextBM[] conversion array.
  *------------------------------------------------------------------*/

  line[0] = '\0';                   /* Store null string in line buffer */
  linelen = 0;                      /* And set its length to zero */

  text = intext;                    /* Set of unsigned char pointer, then: */
  for (;;)                          /* For each character in input text string: */
  {
     i = *text;                          /* Store character in index variable */
     if (i == 0) break;                  /* If character is null: end-of-string */
     text++;                             /* Point to next char in text string */

     str = TextBM[i];                    /* Point to text string for character */
     if (str == NULL) continue;          /* If none: Continue with next char */

     i = AddStr(str, bmfile);            /* Add the string to output */
     if (i != 0) return(1);              /* If error: return with error */
  }                                 /* end of for loop */

  strcat(line, "\n");               /* Add newline to end of line buffer */
  linelen += 1;                     /* and increment line length */
  i = fwrite(line, 1, linelen,      /* Write line buffer to output file */
             bmfile);

  if (i == 0)                       /* If error: */
    return(1);                           /* Return with error */
  else                              /* Otherwise: */
    return(0);                           /* Return successfully */
}

/*============================================================================*
 * AddStr() - Add string to output.
 *
 * PURPOSE:
 *   This subroutine adds the string 'str' to the output line buffer defined
 *   by the static 'line[]' array.  If adding 'str' would make the length
 *   of the 'line[]' string larger than OUT_MAX, the current 'line[]' array
 *   is first written to the output file and nulled before adding 'str'.
 *
 * RETURNS:
 *   0 if successful, 1 if error.
 *
 * NOTES:
 *   This subroutine also takes care of adding the correct line continuation
 *   commands.
 *
 *   If the .ct control word is used, then it must be added to the beginning
 *   of the next line.  If the last character on the current line is a space,
 *   it must be converted to &rbl. to keep it from becoming truncated when
 *   the file is uploaded and processed.
 *
 *   If the &cont. symbol is used (recommended with BookMaster), then it must
 *   be added to the end of the current line.  However, the output lines must
 *   be kept "short", the exact definition of short being unknown and
 *   experimentally determined.
 *
 *   It was recommended to me to use &cont. instead of .ct with BookMaster.
 *   Every once in a while, I have found that .ct produces slightly incorrect
 *   results.
 *============================================================================*/

static int AddStr(str, file)

char   *str;                        /* Pointer to string */
FILE   *file;                       /* Output file */

{
  register int len;                 /* Length */
  int   i;                          /* Generic integer */
  char *lastch;                     /* Pointer to last character in line */

  len = strlen(str);                /* Find length of string */
  if ((linelen + len) > OUT_MAX)    /* If str would make line too long: */
  {
//   lastch = line + linelen - 1;        /* Point to last character in line */
//   if (*lastch == ' ')                 /* If the last character is a space: */
//   {
//      strcpy(lastch, "&rbl.");              /* Replace it with space symbol */
//      linelen = strlen(line);               /* And update line length */
//   }

     strcat(line, cont);                 /* Add line with continuation symbol word */
     strcat(line, "\n");                 /* Add newline char to line */
     linelen = strlen(line);             /* Update line length */

     i = fwrite(line, 1,                 /* Write line buffer to output file */
                linelen,
                file);
     if (i == 0) return(1);              /* Be sure write was ok */

     line[0] = '\0';                     /* Reset line buffer/length to null */
//   strcat(line, cont);                 /* Start line with continuation control word */
     linelen = 0;                        /* And set its length */
  }

  strcat(line, str);                /* Add str to end of line */
  linelen += len;                   /* And update line length */

  return(0);                        /* Return */
}
