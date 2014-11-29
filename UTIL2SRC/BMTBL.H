/*============================================================================*
 * bmtbl.h - Definitions used by bmtbl utility.
 *
 * (C)Copyright IBM Corporation, 1991.  All rights reserved. Brian Yoder
 *
 * The table is defined by the TROW structure.  This structure contains
 * a linked list of COL structures, one COL structure for each column
 * in the table.  Once bmtbl has determined how many columns there are,
 * the order and number of COL structures never changes.
 *
 * While processing a row, a number of CELL structures are added to each
 * COL structure to define the text lines for that cell (a cell is a
 * specific row and column within the table).  Before a row is processed,
 * the CELL structures for the previous row are deleted (cleared).
 *
 * 09/11/91 - Created.
 * 09/11/91 - Initial version (OS/2).
 *============================================================================*/

#ifndef _h_BMTBL /* else we've already been included */
#define _h_BMTBL

#define  MAX_LINE  1024             /* Maximum input text file line width */

/*----------------------------------------------------------------------------*
 * Structure that defines a line of text for a cell (specific row and column)
 * in a table.  The CELL structures are linked together to a COL structure
 * to define all of the lines of text for that particular cell in the table.
 *----------------------------------------------------------------------------*/

typedef struct _cell {

     char      *text;               /* Pointer to line of text */
     uint       textlen;            /* Length of the line of text */

     struct _cell *next;            /* Pointer to next CELL for this cell */

} CELL;

/*----------------------------------------------------------------------------*
 * Structure that defines each column of the table
 *----------------------------------------------------------------------------*/

typedef struct _col {

     uint       start;              /* Starting offset of column (0=1st) */
     uint       width;              /* Width of column, in spaces */

     CELL      *first;              /* Pointer to first text line's CELL */
     CELL      *last;               /* Pointer to last text line's CELL */

     struct _col *next;             /* Pointer to COL for next column */

} COL;

/*----------------------------------------------------------------------------*
 * Structure that defines a row in the table.  It contains a linked list of
 * COL structures: one for each column in the table.  Each COL structure
 * contains a linked list of CELL structures: one for each line of text for
 * that cell (column, row).
 *----------------------------------------------------------------------------*/

typedef struct {

     COL       *first;              /* Pointer to first text line's CELL */
     COL       *last;               /* Pointer to last text line's CELL */

     uint       cols;               /* No. of columns in the table */
     uint       twidth;             /* Total width of all columns in the table */

} TROW;

#endif /* _h_BMTBL */
