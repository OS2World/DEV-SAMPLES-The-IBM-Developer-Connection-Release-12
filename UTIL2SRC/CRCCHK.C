/*============================================================================*
 * main() module: crcchk.c - CRC check utility.
 *
 * (C)Copyright IBM Corporation, 1989, 1990, 1991, 1992.     Brian E. Yoder
 *
 * This program reads a list of files, calculates the length and CRC value of
 * each file, and optionally writes the results (length, CRC, name) to an
 * output file.
 *
 * If a filename listed by the input file also contains a length and CRC value,
 * this program compares the file's actual length and CRC to these values.
 * Discrepancies are logged to stderr.
 *
 * Typically, this program is run at least twice: once to generate a list of
 * lengths and CRC values for a group of files, and once to check to see
 * if any of the listed files have changed.
 *
 * For example, assume 'listfile' is formatted as follows:
 *
 *        # List of files for crcchk program
 *        \u\brian\bin\ccmt.exe
 *        \u\brian\bin\ls.exe
 *        \u\brian\bin\chmod.exe
 *        \u\brian\bin\cdir.exe
 *        \u\brian\bin\ccmt.exe
 *
 * Running the command "crcchk listfile crcfile" might produce the following
 * output file named 'crcfile':
 *
 *        23864  0x48EA  \u\brian\bin\ccmt.exe
 *         1819  0xC4AF  \u\brian\bin\ls.exe
 *         2117  0xF631  \u\brian\bin\chmod.exe
 *        20931  0xE8E3  \u\brian\bin\cdir.exe
 *        23009  0xE69A  \u\brian\bin\ccmt.exe
 *
 * Running the command "crcchk crcfile" would then list, on stderr, all files
 * listed in 'crcfile' that did not have the same length or CRC value as
 * specified in 'crcfile'.  This would indicate which, if any, of the listed
 * files had been changed.
 *
 * Additionally, if any errors occur (can't open input file, can't create
 * output file, bad syntax or missing files specified inside the input file),
 * this program returns a nonzero exit code.
 *
 * See the syntax() subroutine in this module for more information about the
 * use of this program.  See the crcfile.c module for more information about
 * CRCs.
 *
 * 11/01/90 - Created.
 * 11/02/90 - Initial version.
 * 02/25/91 - Fixed spelling error in syntax().
 * 05/02/91 - Ported from PS/2 AIX to DOS.
 * 05/08/91 - If we fail a file's length/CRC check, we still must write an
 *            entry for that file.
 * 06/07/92 - Added ability to process long (32-bit) CRC values.  In the list
 *            file, 0xnnnn or shorter is assumed to be a 16-bit CRC, while
 *            0xnnnnn or longer is assumed to be a 32-bit (long) CRC.
 * 06/16/92 - Added -l flag to calculate long (32-bit) CRC values in the case
 *            where an output file is specified but the input file lists no
 *            CRC for a given file.
 * 07/24/92 - Changed lval from long to ulong.
 *============================================================================*/

static char copr[] = "(c)Copyright IBM Corporation, 1992.  All rights reserved.";

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/types.h>

#include "util.h"

/*============================================================================*
 * Define internal functions to allow forward access
 *============================================================================*/

static void syntax();
int hex2ulong(char *, ulong *);
int dec2ulong(char *, ulong *);

/*============================================================================*
 * Main Program Entry Point
 *============================================================================*/

main(argc, argv)

int argc;           /* arg count */
char *argv[];       /* arg pointers */

{
  int     rc;       /* Return code storage */
  char   *flagstr;  /* Pointer to string of flags */
  ulong   lval;     /* long value */
  int     numargs;  /* No. of arguments processed (not including -flags) */
  char    cf;       /* Current flag character (if any) */

  int     errcnt;   /* Number of errors detected */
  int     f_long;   /* Force long (32-bit) CRCs (see 06/16/92 update note)? */

  char   *infname;  /* Pointers to names of input and output files */
  char   *outfname;
  FILE   *outfile;  /* Output file (optional) */

  int      tokc;    /* Number of tokens found in a line */
  char   **tokv;    /* Pointer to array of token pointers */

  char   *alen;     /* Pointer to ASCII decimal length */
  char   *acrc;     /* Pointer to ASCII hex CRC value */
  char   *filename; /* Name of a file that is listed in the input file */

  ulong   listlen;  /* Converted length and CRC from the list file */
  ulong   listcrc;

  ushort  crc;      /* Actual CRC and length values */
  ulong   crc32;
  ulong   len;

  int     find32;   /* TRUE == Find 32-bit CRC.  FALSE == Find 16-bit CRC */
  ulong   filecrc;  /* For comparison purposes */

  numargs = 0;      /* Initialize number of arguments processed = none */

  infname = NULL;   /* Set name and file pointers to NULL */
  outfname = NULL;
  outfile = NULL;

  f_long = FALSE;   /* Default to generating 16-bit CRCs when none specified */

/*----------------------------------------------------------------------------*
 * Check number of command line arguments:
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
           case 'l':
           case 'L':
              f_long = TRUE;
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

  if ((argc <= 0) || (argc > 2))    /* Verify number of arguments */
     syntax();

/*----------------------------------------------------------------------------*
 * Open the input text file as a customization file:
 *----------------------------------------------------------------------------*/

  infname = argv[0];                /* Store pointer to file name */
  rc = cfopen(infname);             /* Open the input file */
  if (rc != 0)
  {
     fprintf(stderr, "crcchk: Cannot open file: %s\n",
        infname);
     return(1);
  }

/*----------------------------------------------------------------------------*
 * Create the output file if one is specified:
 *----------------------------------------------------------------------------*/

  if (argc >= 2)
  {
     outfname = argv[1];
     outfile = fopen(outfname, "w");
     if (outfile == NULL)
     {
        fprintf(stderr, "crcchk: Cannot create file: %s\n",
           outfname);
        return(1);
     }
  }

/*----------------------------------------------------------------------------*
 * Process each file listed in the input file:
 *----------------------------------------------------------------------------*/

  errcnt = 0;                       /* No errors at start of loop! */
  for (;;)                          /* For each entry in the file: */
  {
    /*------------------------------------------------------------------------*
     * Read the (next) line from the input text file and break it up into
     * tokens (ASCII strings).
     *------------------------------------------------------------------------*/

     tokc = cfread(&tokv);               /* Read the entry */

     if (tokc == 0)                      /* If no tokens: */
        break;                           /*    We're done: break out of loop */

     if ((tokc != 3) &&                  /* Check number of tokens: */
         (tokc != 1))
     {
        fprintf(stderr, "crcchk: Wrong number of tokens: %s, line %lu\n",
           infname, cfline());
        errcnt++;
        continue;
     }

    /*------------------------------------------------------------------------*
     * Store pointers to ASCII tokens from the input text file.
     *------------------------------------------------------------------------*/

     if (tokc == 3)                      /* If 3 tokens: */
     {
         alen = tokv[0];                      /* Format is: 'length crc name' */
         acrc = tokv[1];
         filename = tokv[2];

         if (strlen(acrc) <= 6)               /* If "0x" and 4 or less digits: */
            find32 = FALSE;                        /* It's a 16-bit CRC, not 32-bit */
         else
            find32 = TRUE;                         /* It's a 32-bit CRC */
     }
     else                                /* Else: There's one token: */
     {
         alen = "-";                          /* Format is: 'name' */
         acrc = "-";
         filename = tokv[0];
     }

    /*------------------------------------------------------------------------*
     * In case the file has no CRC associated with it, tell crcchk which
     * type of CRC to generate: 16-bit, if f_long is TRUE; 32-bit, otherwise.
     *------------------------------------------------------------------------*/

     if (strcmp(acrc, "-") == 0)         /* A CRC of "-" means there isn't one */
        find32 = f_long;                 /* listed for this file */

    /*------------------------------------------------------------------------*
     * Get the file's actual length and CRC values:
     *------------------------------------------------------------------------*/

     if (find32)                         /* Find file's actual CRC and length */
        rc = crcfile32(filename, &crc32, &len);
     else
        rc = crcfile(filename, &crc, &len);

     if (rc != 0)                        /* If error: */
     {
        fprintf(stderr, "crcchk: Cannot access file '%s'.\n", filename);
        errcnt++;
        continue;
     }

    /*------------------------------------------------------------------------*
     * Write information to output file if one is present
     *------------------------------------------------------------------------*/

     if (outfile != NULL)                /* Write the information to output file */
     {
        if (find32)
           fprintf(outfile, "%12lu  0x%08lX  %s\n", len, crc32, filename);
        else
           fprintf(outfile, "%12lu  0x%04X  %s\n",  len, crc, filename);
     }

    /*------------------------------------------------------------------------*
     * If valid length and/or CRC values are present, check them.  If they
     * are not present (indicated by being a dash "-"), then ignore them.
     *------------------------------------------------------------------------*/

     if (strcmp("-", alen) != 0)         /* Check length, if not "-": */
     {
        rc = dec2ulong(alen, &listlen);       /* Convert it */
        if (rc != 0)
        {
           fprintf(stderr, "crcchk: Invalid length: '%s'  (in %s, line %lu)\n",
              alen, infname, cfline());
           errcnt++;
           continue;
        }

        if (listlen != len)                   /* Check the value */
        {
           fprintf(stderr, "crcchk: length mismatch for file: %s\n",
              filename);
           errcnt++;
           continue;
        }
     }

     if (strcmp("-", acrc) != 0)         /* Check CRC, if not "-": */
     {
        rc = hex2ulong(acrc, &lval);          /* Convert it */
        if (rc != 0)
        {
           fprintf(stderr, "crcchk: Invalid CRC: '%s'  (in %s, line %lu)\n",
              acrc, infname, cfline());
           errcnt++;
           continue;
        }

        listcrc = lval;                       /* Store listed value */
        if (find32)                           /* If processing 32-bit CRC: */
           filecrc = crc32;                        /* Store actual 32-bit CRC */
        else                                  /* Else: */
           filecrc = crc;                          /* Store actual 16-bit CRC */


      //printf("(temp) listcrc = 0x%08lx , filecrc = 0x%08lx , find32 = %d  %s\n",
      //        listcrc, filecrc, find32, filename);

        if (listcrc != filecrc)               /* Check listed vrs. actual values: */
        {
           fprintf(stderr, "crcchk: CRC mismatch for file: %s\n",
              filename);
           errcnt++;
           continue;
        }
     }

    /*------------------------------------------------------------------------*
     * End of for loop: Process next line of input text file
     *------------------------------------------------------------------------*/

  } /* end of for (;;) loop */

  if (errcnt != 0)                  /* See if any errors occurred.  If so, */
     return(1);                     /* Return with non-zero exit code */

  return(0);                        /* Done with main(): Return */

} /* end of main() */

/*============================================================================*
 * syntax() - Display command syntax and exit to operating system!
 *============================================================================*/
static void syntax()
{
  fprintf(stderr, "Usage: crcchk [-l] infile [outfile]\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "This program reads the input text file (infile) and optionally\n");
  fprintf(stderr, "writes an output text file (outfile).\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Format of the input text file:  Non-blank, non-comment lines must\n");
  fprintf(stderr, "must be formatted as follows.  If the length and CRC values are\n");
  fprintf(stderr, "present, they are compared against the actual values for the file:\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "       [length  CRC]  filename\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "If an output file is specified:  For each filename in the input file,\n");
  fprintf(stderr, "a line is written to the output file as follows:\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "       length  CRC  filename\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "If no CRC is present, then the -l flag tells crcchk to generate a\n");
  fprintf(stderr, "32-bit (long) CRC for the file.  The default in this case is to\n");
  fprintf(stderr, "generate a 16-bit CRC for the file.\n");
  exit(1);
}

/*============================================================================*
 * hex2ulong() - Converts a hex string to a long integer.
 *
 * Purpose:
 *   Converts the string of hexadecimal digits pointed to by 'str' to
 *   an unsigned long integer.  The result is stored at &result.
 *
 * Returns:
 *   0, if no errors.  Otherwise, an error occurred.
 *============================================================================*/
int hex2ulong(str, result)

char  *str;                  /* Pointer to string to be converted */
ulong *result;               /* Pointer to where to store result */

{
  int   base;                /* Base to use during conversion */
  char *ptr;                 /* Pointer to character that terminated conversion */

  base = 16;                 /* Set base for conversion */

  /* Convert input string using input base */

  *result = strtoul(str, &ptr, base);

  /* Check conversion to ensure it was successful: 'ptr' should point to
   * the end of the string */

  if (*ptr != '\0')
     return(1);

  return(0);
}

/*============================================================================*
 * dec2ulong() - Converts a decimal string to a long integer.
 *
 * Purpose:
 *   Converts the string of decimal digits pointed to by 'str' to
 *   an unsigned long integer.  The result is stored at &result.
 *
 * Returns:
 *   0, if no errors.  Otherwise, an error occurred.
 *============================================================================*/
int dec2ulong(str, result)

char  *str;                  /* Pointer to string to be converted */
ulong *result;               /* Pointer to where to store result */

{
  int   base;                /* Base to use during conversion */
  char *ptr;                 /* Pointer to character that terminated conversion */

  base = 10;                 /* Set base for conversion */

  /* Convert input string using input base */

  *result = strtoul(str, &ptr, base);

  /* Check conversion to ensure it was successful: 'ptr' should point to
   * the end of the string */

  if (*ptr != '\0')
     return(1);

  return(0);
}
