/*============================================================================*
 * module: lbuf.c - Buffered line input.
 *
 * (C)Copyright IBM Corporation, 1991.                    Brian E. Yoder
 *
 * This module contains subroutines for buffered ASCII text line input.
 * The lgets() subroutine works like fgets() except that it doesn't choke on
 * non-ASCII, null, or end-of-text characters.
 *
 * This module also contains subroutines to manage buffered input streams:
 *      newlbuf() - Create a new LBUF buffered stream.
 *      setlbuf() - Bind an open file descriptor to an LBUF buffered stream.
 *      lgets()   - Get the next line from an LBUF buffered stream.
 *
 * Note:  Never directly allocate or declare storage for an LBUF structure.
 * Always use newlbuf() to create LBUF buffered streams and setlbuf() to
 * set them up.  That way, any necessary initialization is taken care of.
 *
 * 10/17/91 - Created for DOS and OS/2.
 * 10/26/91 - Initial version.
 * 09/30/93 - Treat non-ASCII characters, nulls, and end-of-text characters
 *            as newlines. See lgets() for why.
 * 11/17/93 - Add formfeed to list of allowable characters in lgets().
 *============================================================================*/

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <ctype.h>

#include "util.h"

/*============================================================================*
 * newlbuf() - Create a new LBUF buffered stream.
 *
 * REMARKS:
 *   This subroutine allocates space for the LBUF structure type and its I/O
 *   buffer.  The structure and buffer are contained within the same malloc'd
 *   block of memory.
 *
 * RETURNS:
 *   A pointer to the new LBUF structure.  A NULL pointer is returned if
 *   there is not enough memory for the structure and its I/O buffer.
 *
 * NOTES:
 *   Freeing an LBUF buffered stream involves closing its file descriptor
 *   (if != -1) and freeing the memory block (LBUF *).
 *============================================================================*/
LBUF *newlbuf(

uint      bufflen )                 /* Size of I/O buffer, in bytes */

{
  int     rc;                       /* Return code storage */
  char   *newmem;                   /* Pointer to newly allocated memory */
  uint    memlen;                   /* Length of newly allocation memory */
  uint    memmin;                   /* Minimum amount of memory required */
  LBUF   *stream;                   /* Pointer to newly allocated LBUF */

 /*---------------------------------------------------------------------------*
  * Allocate a block of memory to hold the LBUF structure + the I/O buffer
  *---------------------------------------------------------------------------*/

  if (bufflen == 0) return(NULL);   /* If buffer len is 0: just return */

  memlen = sizeof(LBUF) + bufflen;  /* Calculate amount of memory needed */

  memmin = sizeof(LBUF) + BUFSIZ;   /* Determine minimum amount we should have */
  if (memlen < memmin)              /* If memlen is too small (overflow when */
  {                                 /* adding, or because bufflen < BUFSIZ): */
     memlen = memmin;                    /* Set memlen = mimimum required.  Our */
     bufflen = BUFSIZ;                   /* I/O buffer should at least be BUFSIZ */
  }                                      /* or we have no advantage over fgets */

  newmem = malloc(memlen);          /* Allocate the block of memory */

  if (newmem == NULL) return(NULL); /* If we couldn't get the memory: return */

  stream = (LBUF *)newmem;          /* LBUF resides at the start of the block */

 /*---------------------------------------------------------------------------*
  * Initialize the LBUF structure:
  *---------------------------------------------------------------------------*/

  memset(newmem, 0, memlen);        /* Fill LBUF + I/O buffer with nulls */

  stream->fd = -1;                  /* Set file descriptor to -1 (not open) */
  stream->bufflen = bufflen;        /* Store length of I/I buffer */

  stream->buff = newmem +           /* Store pointer to I/O buffer: just past */
                 sizeof(LBUF);      /* the LBUF structure */

  stream->next = newmem + memlen;   /* Set 'next' pointer just past end of I/O buffer */
  stream->end = stream->next;       /* Set end pointer = next pointer */

  return(stream);                   /* Done: Return pointer to LBUF structure */
}

/*============================================================================*
 * setlbuf() - Bind an open file descriptor to an LBUF buffered stream.
 *
 * REMARKS:
 *   This subroutine stores the specified file descriptor in the LBUF
 *   stream.  It is the caller's responsibility to provide a file descriptor
 *   that is open for reading.
 *
 *   If the LBUF stream already contains an open file descriptor, then it
 *   is closed before storing the new file descriptor.
 *
 * RETURNS:
 *   Nothing.
 *
 * NOTES:
 *   Don't directly allocate or declare storage for an LBUF structure:
 *   always use newlbuf() to create a new LBUF buffered stream.
 *
 *   For DOS and OS/2, the file descriptor may be open for reading in either
 *   binary or text modes -- it won't matter to the lgets() subroutine.
 *   However, for performance reasons you should open the file in binary mode.
 *============================================================================*/
void setlbuf(

LBUF     *stream,                   /* Pointer to LBUF stream */
int       fd )                      /* Open file descriptor */

{
  int     rc;                       /* Return code storage */

  if (stream->fd != -1)             /* If stream is open: */
  {
     close(stream->fd);                  /* Close it */
     stream->fd = -1;                    /* Set file descriptor to -1 */
  }

  stream->fd = fd;                  /* Store file descriptor */

  stream->end = stream->buff +      /* Initialize 'end' to point one character */
                stream->bufflen;    /* past end of stream's I/O buffer */

  stream->next = stream->end;       /* Point 'next' past end of buffer, too. */
                                    /* Therefore, the next call to lgets() */
                                    /* will see there's no data in the I/O */
                                    /* buffer and will start reading */

  stream->ferrno = 0;               /* Reset stream's error condition */
  stream->seof = FALSE;             /* And reset its end-of-stream flag */

  return;                           /* Done: Return */
}

/*============================================================================*
 * lgets()   - Get the next line from an LBUF buffered stream.
 *
 * REMARKS:
 *   This subroutine reads a line from the LBUF stream and stores it in
 *   the specified string.  It reads characters from the stream up to
 *   and including the first new-line character (\n), up to the end of
 *   the stream, or until n-1 characters have been read, which ever comes
 *   first.   The subroutine then stores a zero character at the end of
 *   the string.  The string includes the new-line character, if it was
 *   read.
 *
 *   Non-ASCII non-control characters, nulls, and DOS end-of-text characters
 *   are treated as newline characters. This facilitates reading binary files
 *   and keeping  a string from being split across two calls to lgets().
 *   Carriage returns are completely ignored: lines are delimited by newline
 *   characters as both unix and text-mode DOS programs do.
 *
 *   Before calling lgets, first call newlbuf to create an LBUF stream.
 *   Then, open a file and call setlbuf to store its file descriptor
 *   in the LBUF structure.  The stream is now ready to be read by
 *   lgets.
 *
 *   It is the caller's responsibility to ensure that the string can
 *   hold up to n characters, including the null string terminating
 *   byte.
 *
 * RETURNS:
 *   The lgets subroutine returns a pointer to the string.  A NULL pointer
 *   is returned if the end of the stream was encountered and no characters
 *   were read, if an error condition is encountered, or if the stream
 *   doesn't contain an open file descriptor.
 *
 *   If n is 1, then str will be empty (*str contains '\0').
 *
 * NOTES:
 *   This subroutine uses the stream's 'next' field as a pointer to the next
 *   character position within the I/O buffer from which to obtain a character.
 *============================================================================*/
char *lgets(

char     *str,                      /* Pointer to location to store string */
uint      n,                        /* Max string length, including null at end */
LBUF     *stream )                  /* Pointer to LBUF buffered stream */

{
  int     rc;                       /* Return code storage */
  int     rcnt;                     /* Count returned from reading */
  char   *ostr;                     /* Pointer to output position within str */
  int     c;                        /* Character from the stream */
  char   *snext;                    /* Placeholder to next char from I/O buffer */
  char   *send;                     /* Pointer past end of I/O buffer */

  if (n == 0)                       /* If n is zero: We can't even hold the */
     return(NULL);                  /* terminating null character */

  if ( (stream->seof == TRUE) ||    /* If already at the end of the stream, or */
       (stream->ferrno != 0) )      /* If we got an error on a previous I/O: */
  {
     *str = '\0';                        /* Make output string zero length */
     return(NULL);                       /* Don't do anything */
  }

  n--;                              /* Set n = no. of characters, NOT counting */
                                    /* the terminating null character */

  ostr = str;                       /* Point to first position within str */
  snext = stream->next;             /* Get stream's next I/O buffer position */
  send  = stream->end;              /* Get stream's end-of-data position */

 /*---------------------------------------------------------------------------*
  * Loop to put characters from stream into the caller's string
  *---------------------------------------------------------------------------*/

  for (;;)                          /* Loop to store characters in str: */
  {
     if (n == 0) break;                  /* If we filled 'str': quit loop */

    /*-------------------------------------------------------------------*
     * If stream's I/O buffer needs refilling, then fill it and reset
     * 'send' to point just past end of last byte read, and 'snext' to
     * point to the start of the buffer.
     *-------------------------------------------------------------------*/

     if (snext >= send)                  /* If we've run out of data in buffer: */
     {
        rcnt = read(stream->fd,               /* Read the next segment from file */
                    stream->buff,
                    stream->bufflen);

        if (rcnt == -1)                       /* If I/O error: */
        {
           stream->ferrno = errno;                 /* Store errno, and */
           break;                                  /* Stop storing chars */
        }

        if (rcnt == 0)                        /* If we're at the end: */
        {
           stream->seof = TRUE;                    /* Set end-of-stream */
           break;                                  /* Stop storing chars */
        }

        snext = stream->buff;                 /* Point 'next' to start of data */

        send =  snext + rcnt;                 /* Point 'end' one character past */
                                              /* end of data in I/O buffer */
     } /* end of if stmt to check for data in buffer */

    /*-------------------------------------------------------------------*
     * Store next character from stream's buffer into the output string
     *-------------------------------------------------------------------*/

     c = *snext;                         /* Store character from buffer in c */

     if (isascii(c))                    /* If c is an ASCII control char: */
     {                                  /* only allow certain ones: Turn the */
        if (iscntrl(c))                 /* rest into newlines */
        {
           switch (c)
           {                            /* Allowable ctrl chars: */
              case '\n':                     /* newline */
              case '\r':                     /* carriage return */
              case '\t':                     /* tab */
              case '\b':                     /* backspace */
              case '\f':                     /* formfeed */
              case 27:                       /* escape */
                 break;

              default:                  /* Turn the rest into newlines */
                 c = '\n';
           }
        }
     }

     switch (c)                          /* Check the character again: */
     {
        case '\r':                            /* For carriage returns: */
           snext++;                                /* Skip over them */
           n--;
           break;

        default:                              /* All others: */
           *ostr = c;                              /* Store it in output string */
           ostr++;                                 /* Bump output pointer */
           snext++;                                /* Point to next char in buffer */
           n--;                                    /* Adjust n to no. of chars */
           break;
     }

     if (c == '\n')                      /* If c is also a new-line: */
        break;                                /* Break out: We have a line */

  } /* end of for(;;) loop to store characters in str */

 /*---------------------------------------------------------------------------*
  * Done storing characters.
  *
  * Note: If ostr still points to the beginning of str, then we must have
  * gotten an error or to the end of the file without storing anything.
  *---------------------------------------------------------------------------*/

  *ostr = '\0';                     /* Terminate string with null char */
  stream->next = snext;             /* Store stream's I/O buffer placeholder */
  stream->end  = send;              /* Store stream's end-of-data pointer */

  if (ostr == str)                  /* If we haven't stored anything at all: */
     return(NULL);                  /* Then return NULL */

  return(str);                      /* Return pointer to caller's string */
}
