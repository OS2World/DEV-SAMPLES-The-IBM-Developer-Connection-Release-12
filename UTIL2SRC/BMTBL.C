/*============================================================================*
 * main() module: bmtbl.c - Build a BookMaster table.
 *
 * (C)Copyright IBM Corporation, 1991.                    Brian E. Yoder
 *
 * The 'bmtbl' command converts a simple table in an ASCII text file into a
 * BookMaster table.  This makes it easy to translate text information that
 * is already in table form into a correct BookMaster table description.
 * Once the BookMaster table description has been built automatically (the
 * tedious part), you can modify and update it as needed.
 *
 * 09/10/91 - Created.
 * 09/13/91 - 1.0 - Initial version completed.
 * 09/26/91 - 1.0 - Updated module header's comments.  No code change.
 * 07/24/92 - Changed "rt" to "r" for fopen(). C Set/2 doesn't allow "rt".
 *============================================================================*/

static char ver[]  = "bmtbl: version 1.0";
static char copr[] = "(c)Copyright IBM Corporation, 1991.  All rights reserved.";

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>

#include "util.h"
#include "bmtbl.h"

/*============================================================================*
 * The list of COL structures that defines a row in the table.
 *============================================================================*/

static TROW tblrow = { NULL, NULL, 0 };

static int  getrowDone = FALSE;     /* Used only by getrow() */

/*============================================================================*
 * Full prototypes for private (static) functions in this module
 *============================================================================*/

static void       syntax       ( void );

static int        defrow       ( FILE *in, TROW *trow );
static COL       *addcol       ( TROW *trow, uint start, uint width );
static CELL      *addcell      ( COL *col, char *text, uint textlen );
static void       clrcells     ( COL *col );
static void       clrcols      ( TROW *trow );

static void       tabletag     ( FILE *out, TROW *trow );
static void       tableattr    ( FILE *out, char *attr, char *val, uint cnt );
static void       tableend     ( FILE *out );
static void       headingtag   ( FILE *out, TROW *trow );
static void       rowtag       ( FILE *out, TROW *trow );

static void       bldrows      ( FILE *in, FILE *out, TROW *trow, int hdr );
static int        getrow       ( FILE *in, TROW *trow );
static char      *strip        ( char *str, uint len );
static void       tablerow     ( FILE *out, TROW *trow );

/*============================================================================*
 * Main Program Entry Point
 *============================================================================*/

main(argc, argv)

int argc;                           /* arg count */
char *argv[];                       /* arg pointers */

{
  int     rc;                       /* Return code storage */
  long    lrc;                      /* Long return code */
  char   *flagstr;                  /* Pointer to string of flags */

  char   *inname;                   /* Pointer to name of input file */
  char   *outname;                  /* Pointer to name of output file */

  int     h_flag;                   /* Flags */
  int     d_flag;
  int     r_flag;

  FILE   *infile;                   /* Pointers to input/output files */
  FILE   *outfile;

/*----------------------------------------------------------------------------*
 * Set initial values of flags:
 *----------------------------------------------------------------------------*/

  h_flag    = FALSE;                /* Don't put header in table */
  d_flag    = FALSE;                /* Don't build a tdef: Build the entire table */
  r_flag    = FALSE;                /* Don't reference a tdef: Put all parms in table */

/*----------------------------------------------------------------------------*
 * Be sure we have at least one argument:
 *----------------------------------------------------------------------------*/

  argc--;                           /* Ignore 1st argument (program name) */
  argv++;

  if (argc <= 0) syntax();          /* If no arguments: Display syntax */

 /*---------------------------------------------------------------------------*
  * Process flags, if any
  *---------------------------------------------------------------------------*/

  flagstr = *argv;                  /* Point 'flagstr' to argument */
  if (*flagstr == '-')              /* If it begins with '-': It's a list of flags */
  {
     flagstr++;                          /* Point past the dash */

     while (*flagstr != '\0')            /* For each character in flag string: */
     {
        switch (*flagstr)
        {
           case 'h':
           case 'H':
              h_flag = TRUE;
              break;

           default:
              fprintf(stderr, "Invalid flag '%c'.  For help, enter command with no arguments.\n",
                 *flagstr);
              exit(2);
              break;
        }
        flagstr++;                            /* Check next character */
     }

     argc--;                             /* Done with flags: Discard them */
     argv++;
  }

 /*---------------------------------------------------------------------------*
  * Store pointers to the names of input, output files
  *---------------------------------------------------------------------------*/

  if (argc != 2) syntax();          /* If wrong no. of args: Display syntax */

  inname = argv[0];                 /* Point to name of input file */
  outname = argv[1];                /* Point to name of output file */

 /*---------------------------------------------------------------------------*
  * Open the input file
  *---------------------------------------------------------------------------*/

  infile = fopen(inname, "r");
  if (infile == NULL)
  {
     fprintf(stderr, "Cannot access input file: '%s'.\n", inname);
     return(2);
  }

 /*---------------------------------------------------------------------------*
  * Build the TROW structure that defines the table
  *---------------------------------------------------------------------------*/

  rc = defrow(infile, &tblrow);
  if (rc != 0) return(2);

 /*---------------------------------------------------------------------------*
  * Create the output file
  *---------------------------------------------------------------------------*/

  outfile = fopen(outname, "w");
  if (outfile == NULL)
  {
     fprintf(stderr, "Cannot create output file: '%s'.\n", outname);
     return(2);
  }

 /*---------------------------------------------------------------------------*
  * Build the table definition tag and attributes
  *---------------------------------------------------------------------------*/

  tabletag(outfile, &tblrow);

 /*---------------------------------------------------------------------------*
  * Build each of the table's rows (and heading row, if specified)
  *---------------------------------------------------------------------------*/

  bldrows(infile, outfile, &tblrow, h_flag);

 /*---------------------------------------------------------------------------*
  * End the table definition
  *---------------------------------------------------------------------------*/

  tableend(outfile);

 /*---------------------------------------------------------------------------*
  * Done
  *---------------------------------------------------------------------------*/

  return(0);                        /* Return */

} /* end of main() */

/*============================================================================*
 * syntax() - Display command syntax and exit to operating system!
 *============================================================================*/
static void syntax()
{
  fprintf(stderr, "%s\n", ver);
  fprintf(stderr, "Usage: bmtbl [-flags] infile outfile\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "The bmtbl program processes the input text file, converts it to\n");
  fprintf(stderr, "a BookMaster table definition, and writes the table definition\n");
  fprintf(stderr, "to the output file.  The following flags are supported:\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  h  The first row of text contains the table's heading.\n");
  fprintf(stderr, "\n");
  exit(2);
}

/*============================================================================*
 * defrow
 *
 * REMARKS:
 *   This subroutine reads the first line of the input file.  The first
 *   character in the line is ignored (since it is part of the extra column
 *   that was added to the table to separate the rows!).  The rest of the
 *   line contains a non-blank character just past the end of each column
 *   (and just before the beginning of the next column).
 *
 *   This subroutine initializes the TROW structure with the information that
 *   will define each row of the table: how many columns, the width of each
 *   column, the total width of the table, etc.
 *
 * RETURNS:
 *   0         : Successful.
 *   Other:    : Error.  An error message is written to stderr.
 *
 * NOTES:
 *   Since we assume that a text editor was used to add an extra line and
 *   extra column to the original text, we will ignore the first character
 *   (1st column) of the input line.
 *
 *   The cstart variable is used to store the index (offset) of the start
 *   of a column, relative to the second character of each line of the input
 *   file.  Remember that the first character position of each line is used
 *   to determine whether the line contains text or a row separator.
 *
 *   The cstart of a column is equal to the cstart of the previous column +
 *   the width of the previous column + one (to skip past the column that
 *   contains the non-blank character!).
 *============================================================================*/

static int defrow(

FILE    *in,                        /* Input file */
TROW    *trow )                     /* Pointer to TROW structure for table */

{
  int    rc;                        /* Return code storage */
  char  *line;                      /* Pointer to line information */
  char  *end;                       /* Pointer to end of line */
  uint   len;                       /* Length of line */
  uint   cnt;                       /* Counter */
  char   ch;                        /* Character from line */
  uint   cstart;                    /* Current column: Starting col. no. */
  uint   cwidth;                    /*                 Width */
  COL   *col;                       /* Pointer to structure */
  char   lbuff[MAX_LINE+1];         /* Buffer to hold input line from file */

 /*---------------------------------------------------------------------------*
  * Read the line.
  *---------------------------------------------------------------------------*/

  line = fgets(lbuff,               /* Read the line into lbuff[] */
               MAX_LINE, in);
  if (line == NULL)
  {
     fprintf(stderr, "Unexpected end-of-file encountered.\n");
     return(2);
  }

  if (*line != '\0') line++;        /* Ignore 1st character in line */

  len = strlen(line);               /* Find length of line */
  if (len <= 1)                     /* If newline or nothing: */
  {
     fprintf(stderr, "First line is blank.  Table cannot be built.\n");
     return(2);
  }

  end = line + len - 1;             /* Point to the end of the line */
  if (*end == '\n')                 /* If last char is a newline: */
  {                                 /* (it's supposed to be with fgets) */
     *end = '\0';                        /* Turn it into a null byte */
     len--;                              /* And update length */
  }

 /*---------------------------------------------------------------------------*
  * Loop to look for all non-blank characters in line to define columns.
  *---------------------------------------------------------------------------*/

  cstart = 0;                       /* First column starts at offset 0 */
  cwidth = 0;                       /* Initialize column width */

  for (;;)                          /* Loop to look for all columns: */
  {
     ch = *line;                         /* Get next character */
     if (ch == '\0')                     /* Done if we reached the end */
        break;

     if (ch != ' ')                      /* If we found a non-blank character: */
     {                                        /* It's past the end of the column */
        if (cwidth == 0)                      /* Be sure the column has at least 1 char */
        {
           fprintf(stderr, "A column length of zero was found.  Table cannot be built.\n");
           return(2);
        }

//      printf("cstart=%d   cwidth=%d\n",     /* For test purposes */
//              cstart,     cwidth);

        col = addcol(trow, cstart, cwidth);   /* Add column to table row def */
        if (col == NULL) return(2);

        cstart = cstart + cwidth + 1;         /* Calculate offset of start of next column */
        cwidth = 0;                           /* Initialize length of next column */
     }
     else                                /* Else: character is blank: */
        cwidth++;                             /* Update length of current column */

     line++;                             /* Point to next char and continue */
  } /* end of loop to look for all columns */

 /*---------------------------------------------------------------------------*
  * Be sure we have at least one column in the table
  *---------------------------------------------------------------------------*/

  if (trow->cols == 0)
  {
     fprintf(stderr, "Table has no columns and won't be built.\n");
     return(2);
  }

  return(0);                        /* Done */
}

/*============================================================================*
 * addcol
 *
 * REMARKS:
 *   This subroutine adds a new COL structure to the linked list that is
 *   contained in the TROW structure.
 *
 *   The column's starting offset is the offset from the second character
 *   in each line of the input text file.  Again, the first character in
 *   each line is set to non-blank to delimit rows in the input text file.
 *
 * RETURNS:
 *   Pointer to new COL structure, or NULL if there is not enough memory.
 *============================================================================*/

static COL *addcol(

TROW    *trow,                      /* Table row definition */
uint     start,                     /* Column's starting offset */
uint     width )                    /* Column's width */

{
  int    rc;                        /* Return code storage */
  COL   *col;                       /* Pointer to new data structure */

 /*---------------------------------------------------------------------------*
  * Allocate the structure and fill it in
  *---------------------------------------------------------------------------*/

  col = malloc(sizeof(COL));        /* Allocate the memory */
  if (col == NULL)                  /* Be sure it's available */
  {
     fprintf(stderr, "*** Out of memory.\n");
     return(NULL);
  }

  memset(col, 0, sizeof(COL));      /* Fill structure with nulls */

  col->start = start;               /* Save column's starting offset */
  col->width = width;               /* and its width */

 /*---------------------------------------------------------------------------*
  * Add it to the linked list
  *---------------------------------------------------------------------------*/

  if (trow->first == NULL)          /* If no items in list: */
  {
     trow->first = col;                  /* Point first/last pointers to new item */
     trow->last  = col;
  }
  else                              /* Else: there's an item in the list */
  {
     trow->last->next = col;             /* Link current last item to this one */
     trow->last = col;                   /* And make this one the last one */
  }

 /*---------------------------------------------------------------------------*
  * Update table's row information: total columns, etc.
  *---------------------------------------------------------------------------*/

  trow->cols++;                     /* Update no. of columns */
  trow->twidth += width;            /* Update total width of all columns */

  return(col);                      /* Done */
}

/*============================================================================*
 * addcell
 *
 * REMARKS:
 *   This subroutine adds a new CELL structure to the linked list that is
 *   contained in the COL structure for the current column.  The CELL
 *   structure contains a single line of text that belong's in this
 *   column position for the current row in the table.
 *
 *   The text length does not include any null terminating byte.
 *
 *   The data structure, the text, and the text's terminating null byte
 *   are contained within a single malloc'd block.  This reduces memory
 *   fragmentation and simplifies cleanup.
 *
 * RETURNS:
 *   Pointer to new CELL structure, or NULL if there is not enough memory.
 *============================================================================*/

static CELL *addcell(

COL     *col,                       /* Pointer to column's COL structure */
char    *text,                      /* Pointer to text */
uint     textlen )                  /* Length of the text */

{
  int       rc;                     /* Return code storage */
  uint      mlen;                   /* Length of allocated memory block */
  CELL     *cell;                   /* Pointer to new data structure */
  char     *newmem;                 /* Pointer to newly malloc'd memory */
  char     *newtext;                /* Pointer to new text (in new memory) */

 /*---------------------------------------------------------------------------*
  * Allocate the memory and fill it in
  *---------------------------------------------------------------------------*/

  mlen = sizeof(CELL) +             /* Calulate size of memory block needed */
         textlen + 1;

  newmem = malloc(mlen);            /* Allocate the memory */
  if (newmem == NULL)               /* Be sure it's available */
  {
     fprintf(stderr, "*** Out of memory.\n");
     return(NULL);
  }
  memset(newmem, 0, mlen);          /* Fill new memory with nulls */

  cell = (CELL *)newmem;            /* Structure is in beginning of block */

  newtext = newmem + sizeof(CELL);  /* The text string will occupy the rest */
  memcpy(newtext, text, textlen);   /* Copy the text just past the CELL */
  newtext[textlen] = '\0';          /* Terminate the text with null byte */

  cell->text = newtext;             /* Store pointer to new text string */
  cell->textlen = strlen(text);     /* And store its length */

 /*---------------------------------------------------------------------------*
  * Add it to the linked list
  *---------------------------------------------------------------------------*/

  if (col->first == NULL)           /* If no items in list: */
  {
     col->first = cell;                  /* Point first/last pointers to new item */
     col->last  = cell;
  }
  else                              /* Else: there's an item in the list */
  {
     col->last->next = cell;             /* Link current last item to this one */
     col->last = cell;                   /* And make this one the last one */
  }

  return(cell);                     /* Done */
}

/*============================================================================*
 * clrcells
 *
 * REMARKS:
 *   This subroutine frees all of the CELL structures for a particular
 *   COL structure.
 *
 * RETURNS:
 *   None.
 *============================================================================*/

static void clrcells(

COL     *col )                      /* Pointer to COL structure */

{
  CELL  *s;                         /* Pointers to structures to free */
  CELL  *snext;

  s = col->first;                   /* Point to first structure in list */
  while (s != NULL)                 /* For each structure in list */
  {
     snext = s->next;                    /* Point to next before s is gone */
     free(s);                            /* Free the structure */
     s = snext;                          /* Reset s to point to next */
  }

  col->first = NULL;                /* Reset linked list pointers to 'empty' */
  col->last = NULL;

  return;                           /* Done */
}

/*============================================================================*
 * clrcols
 *
 * REMARKS:
 *   This subroutine frees all of the CELL structures for all COL structures
 *   that are linked to the TROW structure.  It does NOT free the COL
 *   structures: they define the column structure of the table and remain
 *   constant throughout the life of this program.
 *
 * RETURNS:
 *   Nothing.
 *============================================================================*/

static void clrcols(

TROW    *trow )                     /* Pointer to TROW structure */

{
  COL   *s;                         /* Pointer to COL structure */

  s = trow->first;                  /* Point to first structure in list */
  while (s != NULL)                 /* For each structure in list */
  {
     clrcells(s);                        /* Clear its CELLs */
     s = s->next;                        /* Point to next COL and continue */
  }

  return;                           /* Done */
}

/*============================================================================*
 * tabletag
 *
 * REMARKS:
 *   This subroutine builds the :table tag and all of its attributes.
 *   It writes this information to the output file.
 *
 *   Lots of attributes (and their BookMaster default values, according
 *   to version 3.0) are listed so that the user can change them as
 *   desired without having to look them up.
 *
 * RETURNS:
 *   Nothing.
 *============================================================================*/

static void tabletag(

FILE    *f,                         /* Output file */
TROW    *trow )                     /* Pointer to TROW structure for table */

{
  int    rc;                        /* Return code storage */

 /*---------------------------------------------------------------------------*
  * Begin the table tag
  *---------------------------------------------------------------------------*/

  fprintf(f, ".*\n");
  fprintf(f, ":table\n");

 /*---------------------------------------------------------------------------*
  * Add table attributes that are dependent upon the number of columns
  *---------------------------------------------------------------------------*/

  tableattr(f, "cols"     ,  "*"      ,  trow->cols);
  tableattr(f, "hp"       ,  "0"      ,  trow->cols);
  tableattr(f, "concat"   ,  "yes"    ,  trow->cols);
  tableattr(f, "align"    ,  "left"   ,  trow->cols);
  tableattr(f, "valign"   ,  "top"    ,  trow->cols);

 /*---------------------------------------------------------------------------*
  * Finish off the table tag with a subset of allowable attributes
  *---------------------------------------------------------------------------*/

  fprintf(f, "    rules=both\n");
  fprintf(f, "    frame=box\n");
  fprintf(f, "    hdframe=rules\n");

  fprintf(f, "    width=column\n");

  fprintf(f, "    split=no\n");
  fprintf(f, "    talign=left\n");
  fprintf(f, "    scale='1'.\n");
  fprintf(f, ".*\n");

  return;                           /* Done */
}

/*============================================================================*
 * tableattr
 *
 * REMARKS:
 *   This subroutine writes a table attribute and its initial values
 *   to the output file.  It writes one initial value for each column
 *   in the table.  This makes it easier for someone to change the
 *   initial values for each column long after the table is built and
 *   embedded within a BookMaster document.
 *
 * RETURNS:
 *   Nothing.
 *
 * EXAMPLE:
 *   The statement:  tableattr(f, "cols", "*", 4);
 *   Produces:       cols='* * * *'
 *============================================================================*/

static void tableattr(

FILE    *f,                         /* Output file */
char    *attr,                      /* Pointer to name of attribute */
char    *val,                       /* Pointer to initial value */
uint     cnt )                      /* No. of columns in the table */

{
  fprintf(f, "    %s = '", attr);   /* Print the attribute name and quote */

  while (cnt > 0)                   /* For each column: */
  {
     fprintf(f, "%s", val);              /* Print the value */
     cnt--;
     if (cnt != 0) fprintf(f, " ");      /* If not last column: add a space */
  }

  fprintf(f, "'\n");                /* Add ending quote and newline */
  return;                           /* Done */
}

/*============================================================================*
 * tableend
 *
 * REMARKS:
 *   This subroutine writes the end-of-table tag to the output file.
 *
 * RETURNS:
 *   Nothing.
 *============================================================================*/

static void tableend(

FILE    *f )                        /* Output file */

{
  fprintf(f, ".*\n");
  fprintf(f, ":etable.\n");
  return;                           /* Done */
}

/*============================================================================*
 * bldrows
 *
 * REMARKS:
 *   This subroutine assumes that the TROW structure has been set up and
 *   defines the columns in the table.  It reads the input file and writes
 *   the correct BookMaster tags and text to define all of the rows in the
 *   table.
 *
 *   If the hdr variable is TRUE, then the first row of the table is defined
 *   as a table heading instead of as an ordinary row.
 *
 * RETURNS:
 *   0         : Successful.
 *   Other:    : Error.  An error message is written to stderr.
 *============================================================================*/

static void bldrows(

FILE    *in,                        /* Input file */
FILE    *out,                       /* Output file */
TROW    *trow,                      /* Pointer to TROW structure for table */
int      hdr )                      /* First row is table heading? (TRUE/FALSE) */

{
  int    rc;                        /* Return code storage */

 /*---------------------------------------------------------------------------*
  * Loop to build all rows of the table (including heading, if specified)
  *---------------------------------------------------------------------------*/

  for (;;)                          /* Loop for each row: */
  {
    /*------------------------------------------------------------*
     * Get the next row of information for the table
     *------------------------------------------------------------*/

     rc = getrow(in, trow);              /* Get the text for the row */
     if (rc != 0)                        /* If we're done: break out */
        break;

    /*------------------------------------------------------------*
     * Write the appropriate beginning tag for this row
     *------------------------------------------------------------*/

     if (hdr == TRUE)                    /* If this is the first row and */
     {                                   /* we need a heading: */
        fprintf(out, ":thd.\n");               /* Do this */
     }
     else                                /* Otherwise: */
     {
        fprintf(out, ".*-------\n");           /* Add a small separator line */
        fprintf(out, ":row.\n");               /* And begin the row */
     }

    /*------------------------------------------------------------*
     * Write the text in all columns for this row to output file
     *------------------------------------------------------------*/

     tablerow(out, trow);

    /*------------------------------------------------------------*
     * If we wrote a heading, end it properly and then set hdr to
     * FALSE:  We only write one heading at the most.
     *------------------------------------------------------------*/

     if (hdr == TRUE)                    /* If this is the first row and */
     {                                   /* we have a heading: */
        fprintf(out, ":ethd.\n");             /* End the heading */
        hdr = FALSE;                          /* Only need one heading! */
     }
  } /* end of loop for each row */

  return;                           /* Done */
}

/*============================================================================*
 * getrow
 *
 * REMARKS:
 *   This subroutine gets the text for the next row in the table and stores
 *   it in the TROW structure.  It stores the text for each column (cell)
 *   in the column's COL structure within TROW.  The text is contains as
 *   zero or more CELL structures linked to a COL structure.
 *
 *   This subroutine sets the static getrowDone variable to TRUE when it's
 *   done.  If we have a row and then get to the end of the file, we want
 *   to save the fact that we reached the end of the file.  That way, we
 *   can return the row successfully to the caller this time, and tell the
 *   caller that we're done the next time.
 *
 * RETURNS:
 *   0         : Successful.
 *   Other:    : End of input file reached: there is no more text to read.
 *
 * NOTES:
 *   Remember that if the first character (column) in a line is non-blank,
 *   then the current row is complete.  Otherwise, we ignore this character
 *   and process the rest of the line.
 *
 *   If the first character in a line is a newline, then the line is blank
 *   and completely ignored.
 *============================================================================*/

static int getrow(

FILE    *in,                        /* Input file */
TROW    *trow )                     /* Pointer to TROW structure for table */

{
  int    rc;                        /* Return code storage */

  COL   *col;                       /* Pointer to a column structure */
  char  *coltxt;                    /* Pointer to start of column's text */
  CELL  *cell;                      /* Pointer to a text cell structure */

  char   ch;                        /* Character */
  char  *line;                      /* Pointer to line buffer */
  uint   len;                       /* Line length */
  char   lbuff[MAX_LINE+1];         /* Buffer to hold input line from file */

  if (getrowDone == TRUE)           /* If we're done: Don't do anything more */
     return(1);

 /*---------------------------------------------------------------------------*
  * Initialization
  *---------------------------------------------------------------------------*/

  clrcols(trow);                    /* Clear all text from previous column */

 /*---------------------------------------------------------------------------*
  * Loop to read and store the text from all lines in the row
  *---------------------------------------------------------------------------*/

  for (;;)                          /* Loop for each input text line: */
  {
    /*------------------------------------------------------------*
     * Get next line from input file and store it.  Convert line to
     * a space-padded length of MAX_LINE so we can easily pull
     * columns from it.
     *------------------------------------------------------------*/

     memset(lbuff, ' ', MAX_LINE);       /* Fill line buffer with spaces, and */
     lbuff[MAX_LINE] = '\0';             /* Terminate it with a null byte */

     line = fgets(lbuff, MAX_LINE, in);  /* Get the next line from the file */
     if (line == NULL)                   /* If we've reached the end of file: */
     {
        getrowDone = TRUE;                    /* Remember that we have */
        break;                                /* And break out of loop */
     }

     ch = *line;                         /* Check first character in line: */
     if (ch == '\n') continue;           /* If newline: line is blank: Ignore it */
     if (ch != ' ') break;               /* If not blank: we're done with row */

     len = strlen(line);                 /* Get length of line we just read */
     lbuff[len] = ' ';                   /* Turn null byte back into space */

     if ((len != 0) &&                   /* If last char in line is newline: */
         (lbuff[len-1] == '\n'))         /* Then turn it into a space, too */
             lbuff[len-1] = ' ';

     line++;                             /* Bump past initial column of line */

    /*------------------------------------------------------------*
     * Store the information from each column in the column's COL
     * structure.  Strip off leading and trailing spaces before
     * storing.  Don't store zero-length strings.
     *------------------------------------------------------------*/

     col = trow->first;                  /* Start with first column */
     while (col != NULL)                 /* For each column in table: */
     {
        coltxt = line + col->start;           /* Point to start of text for column */
        coltxt = strip(coltxt, col->width);   /* Strip off excess blanks */
        if (*coltxt != '\0')                  /* If text string is not null: */
        {
           cell = addcell(col, coltxt,             /* Add text to column */
                          strlen(coltxt));
           if (cell == NULL) return(1);            /* If out of memory: done */
        }

        col = col->next;                      /* Go on to next column */
     } /* end of loop to process each column in table */

  } /* end of loop for each input text line */

 /*---------------------------------------------------------------------------*
  * Done with the row
  *---------------------------------------------------------------------------*/

  return(0);                        /* Done */
}

/*============================================================================*
 * strip
 *
 * REMARKS:
 *   This subroutine stores a null byte past the end of the last non-blank
 *   character in the string.  It then returns a pointer to the first
 *   non-blank character in the string.
 *
 *   If all characters in the string are blank, then str returns with
 *   a pointer to a null byte.  The null byte will actually be put into
 *   *str.
 *
 *   Be sure that there are len+1 bytes available (starting at str) that
 *   this subroutine can modify!
 *============================================================================*/

static char *strip(

char    *str,                       /* Pointer to string */
uint     len )                      /* Length of string */

{
  char   ch;                        /* Character */
  char  *end;                       /* Pointer to end of string */

  if (len == 0) return(str);        /* Don't need to strip null strings! */

 /*---------------------------------------------------------------------------*
  * Strip trailing blanks by putting a null byte past last non-blank character
  *---------------------------------------------------------------------------*/

  end = str + len - 1;              /* Point to last char in string */
  while (len > 0)                   /* For each character in the string: */
  {
     if (*end != ' ')                    /* If we're pointing to non-blank char: */
     {
        end[1] = '\0';                        /* Store a null byte just past it */
        break;                                /* And break out of loop */
     }

     len--;                              /* Char is blank: decrement len, and */
     end--;                              /* Go to previous character in string */
  }

  if (len == 0)                     /* If we checked and all chars are blank: */
     *str = '\0';                        /* Make the string zero-length */

 /*---------------------------------------------------------------------------*
  * Strip leading blanks by advancing 'str' until it points to first non-blank
  * character or to the null terminating byte, whichever comes first
  *---------------------------------------------------------------------------*/

  for (;;)                          /* Loop to find first non-blank char: */
  {
     ch = *str;                          /* Get current character */
     if (ch == '\0')                     /* If at end of string: */
        break;                                /* We're done */
     if (ch != ' ')                      /* If character is not blank: */
        break;                                /* We're done */
     str++;                              /* Char is blank: skip past it */
  }                                      /* and continue looping */

  return(str);                      /* Done */
}

/*============================================================================*
 * tablerow
 *
 * REMARKS:
 *   This subroutine writes the tags and text for a row in the table.
 *   The TROW structure contains a list of COL structures, one for
 *   each column in the table.  The text for each column is stored
 *   in a list of CELL structures that are linked to that column's COL
 *   structure.
 *
 * RETURNS:
 *   Nothing.
 *============================================================================*/

static void tablerow(

FILE    *f,                         /* Output file */
TROW    *trow )                     /* Pointer to TROW structure for table */

{
  int    rc;                        /* Return code storage */
  COL   *col;                       /* Pointer to column structure */
  CELL  *cell;                      /* Pointer to cell text structure */

 /*---------------------------------------------------------------------------*
  * Write the text for all columns in the row
  *---------------------------------------------------------------------------*/

  col = trow->first;                /* Start with first column */
  while (col != NULL)               /* For each column in the table row: */
  {
    /*-------------------------------------------------------*
     * Write the text for the current column
     *-------------------------------------------------------*/

     fprintf(f, ":c.\n");                /* Write column tag */

     cell = col->first;                  /* Start with first line of text: */
     while (cell != NULL)                /* For each line of text in column: */
     {
       /*-----------------------------------------------*
        * Loop to write all lines of text for column
        *-----------------------------------------------*/

        if (cell->textlen != 0)               /* If cell's text is not null: */
           fprintf(f, "%s\n", cell->text);         /* Write it to output file */
        cell = cell->next;                    /* Go to next line of text */

     } /* end of loop for each line of text in column */

     col = col->next;                    /* Go to next column */
  } /* end of loop for each column in the table row */

  return;                           /* Done */
}
