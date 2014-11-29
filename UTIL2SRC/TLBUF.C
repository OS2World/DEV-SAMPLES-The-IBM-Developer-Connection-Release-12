/*============================================================================*
 * main() module: tlbuf.c - Test lbuf.c functions.
 *
 * (C)Copyright IBM Corporation, 1991.                           Brian E. Yoder
 *
 * This module tests (and times) the lgets() function in the lbuf.c module.
 *
 * It also times the fgets() function, using setvbuf() to use the same
 * buffer size as used by lgets(), to compare the execution times of these
 * two functions.
 *
 * Note: The environment variable FGETS must be defined (to any value)
 * in order to also run fgets()!
 *
 * Some test results, based on timings to the nearest second:
 *
 * 1. With printf() redirected to NUL, lgets performed just a tiny bit
 *    better than fgets().  A buffer size of 16384 (even multiple of 512)
 *    did the best.  A 122K file was read in 9 secs by lgets and 10 by fgets.
 *
 * 2. With the calls to printf() commented out (when looping to get lines:
 *    see lines marked with %out%), the times got better.  A lot of time
 *    was spent in the printf().  Buffer sizes of 4096, 8192, and 16384 all
 *    resulted in consistent read times of 4 seconds for both lgets and fgets.
 *
 * 10/19/91 - Created.
 * 10/26/91 - Initial version.
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "util.h"

#define MAX_LINE 1024               /* Maximum line length supported */

/*----------------------------------------------------------------------------*
 * Static data
 *----------------------------------------------------------------------------*/

static  char  buff[MAX_LINE];

/*============================================================================*
 * Define internal functions to allow forward access
 *============================================================================*/

static void       syntax();

/*============================================================================*
 * Main Program Entry Point
 *============================================================================*/

main(

int argc      ,                     /* arg count */
char **argv   )                     /* arg pointers */

{
  int     rc;                       /* Return code store area */
  char   *str;                      /* Pointer to string */
  time_t  tstart, tend;             /* Used to determine elapsed time */

  uint    buffsize;                 /* Size of I/O buffer */
  uint    linesize;                 /* Max. line size */
  char   *fname;                    /* Pointer to file name */

  int     fd;                       /* File descriptor */
  LBUF   *istream;                  /* Pointer to buffered input stream */

  FILE   *infile;                   /* Pointer to open input file */
  char   *iobuff;                   /* Pointer to I/O buffer for fgets() */

 /*----------------------------------------------------------------------------*
  * Check arguments
  *----------------------------------------------------------------------------*/

  argc--;                           /* Ignore 1st argument (program name) */
  argv++;

  if (argc < 2)                     /* Be sure there are at least two arguments */
     syntax();

  buffsize = atoi(argv[0]);         /* Convert/store size of I/O buffer */

  linesize = atoi(argv[1]);         /* Convert/store maximum line length */
  if (linesize > MAX_LINE)          /* Be sure it isn't longer than our buff[] */
     linesize = MAX_LINE;

  if (argc >= 3)                    /* If a filename was specified: */
     fname = argv[2];                    /* Store pointer to it */
  else                              /* Else no filename was specified: */
     fname = NULL;                       /* Store a NULL pointer */

 /*----------------------------------------------------------------------------*
  * Open the file (or use stdin, if fname is NULL), and bind the file
  * descriptor to an LBUF buffered stream
  *----------------------------------------------------------------------------*/

  istream = newlbuf(buffsize);
  if (istream == NULL)
  {
     fprintf(stderr, "Cannot allocate memory for istream.\n");
     exit(2);
  }

  if (fname == NULL)
     fd = 0;
  else
  {
     fd = open(fname, O_RDONLY | O_BINARY);
     if (fd == -1)
     {
        fprintf(stderr, "Cannot open file '%s'.\n", fname);
        exit(2);
     }
  }

  setlbuf(istream, fd);

 /*----------------------------------------------------------------------------*
  * Display what we've got so far
  *----------------------------------------------------------------------------*/

  fprintf(stderr, "Input file           : %s\n", fname != NULL ? fname : "(stdin)" );
  fprintf(stderr, "Buffer size (spec'd) : %u\n", buffsize);
  fprintf(stderr, "Buffer size (actual) : %u\n", istream->bufflen);
  fprintf(stderr, "Max line length      : %u\n", linesize);
  fprintf(stderr, "-------------------------------------------------\n");
  fflush(stderr);

 /*----------------------------------------------------------------------------*
  * Read from the stream using lgets()
  *----------------------------------------------------------------------------*/

  tstart = time(NULL);

  for (;;)                          /* Loop: */
  {
     str = lgets(buff, linesize,         /* Get next line from stream */
                 istream);

     if (str == NULL)                    /* If NULL returned: */
        break;                                /* End-of-stream, or error: quit */

     printf("%s\n", buff);               /* Show the line %out% */
  }

  tend = time(NULL);

  if (fd != 0) close(fd);           /* Close the file, if not stdin */

  fprintf(stderr, "Elapsed time using lgets(): %ld seconds.\n",
     tend - tstart);

 /*============================================================================*
  * Use setvbuf() and fgets(), to compare time needed to process the file
  *============================================================================*/

  str = getenv("FGETS");            /* First see of FGETS is in environment */
  if (str == NULL)
     return(0);

  fprintf(stderr, "Reading using fgets()...\n" );

  if (fname == NULL)                /* If no filename: use stdin */
     infile = stdin;
  else                              /* Else: Open the file */
  {
     infile = fopen(fname, "r");
     if (infile == NULL)
     {
        fprintf(stderr, "Cannot fopen file '%s'.\n", fname);
        exit(2);
     }
  }

  iobuff = malloc(buffsize);        /* Allocate I/O buffer */
  if (iobuff == NULL)
  {
     fprintf(stderr, "Cannot allocate %u bytes for I/O buffer.\n", buffsize);
     return(2);
  }

  rc = setvbuf(infile, iobuff, _IOFBF, buffsize);
  if (rc != 0)
  {
     fprintf(stderr, "Call to setvbuf() failed.\n");
     return(2);
  }

 /*----------------------------------------------------------------------------*
  * Read from the stream using fgets()
  *----------------------------------------------------------------------------*/

  tstart = time(NULL);

  for (;;)                          /* Loop: */
  {
     str = fgets(buff, linesize,         /* Get next line from stream */
                 infile);

     if (str == NULL)                    /* If NULL returned: */
        break;                                /* End-of-stream, or error: quit */

     printf("%s\n", buff);               /* Show the line %out% */
  }

  tend = time(NULL);

  if (fd != 0) close(fd);           /* Close the file, if not stdin */

  fprintf(stderr, "Elapsed time using fgets(): %ld seconds.\n",
     tend - tstart);

 /*----------------------------------------------------------------------------*
  * Done
  *----------------------------------------------------------------------------*/

  return(0);                        /* Done with main(): Return */

} /* end of main() */

/*============================================================================*
 * syntax() - Display command syntax and exit to operating system!
 *============================================================================*/
static void syntax()
{
  fprintf(stderr, "Usage:  tlbuf buffsize linesize [fname]\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "   This program uses lgets() to read from the specified file.\n");
  fprintf(stderr, "   If no filename is given, then lgets reads from stdin.\n");
  fprintf(stderr, "   The elasped read/write time, in seconds, is shown.  Direct\n");
  fprintf(stderr, "   standard output to NUL to just time reading.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "   The buffsize argument specifies the size of the I/O buffer.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "   The linesize argument specifies the maximum length of lines\n");
  fprintf(stderr, "   that can be read at one time.  A maximum of %d is supported.\n", MAX_LINE);
  fprintf(stderr, "\n");
  fprintf(stderr, "   If the FGETS environment variable is defined, then the process\n");
  fprintf(stderr, "   is repeated using fgets(), to compare performance.\n");
  fprintf(stderr, "\n");
  exit(1);
}
