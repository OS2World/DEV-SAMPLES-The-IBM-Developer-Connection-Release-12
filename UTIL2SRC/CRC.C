/*============================================================================*
 * main() module: crc.c - Test for crcfile() subroutine.
 *
 * (C)Copyright IBM Corporation, 1989, 1990, 1991, 1992.       Brian E. Yoder
 *
 * This program calls the crcfile() subroutine for each file that is
 * specified on the command line.  It displays each file's length and CRC.
 *
 * See the crcfile.c module for more information.
 *
 * The code to process the file specifications on the command line was
 * taken from the chmod.c source file.
 *
 * 10/30/90 - Created for PS/2 AIX.
 * 10/30/90 - Initial version.
 * 05/02/91 - Ported from PS/2 AIX to DOS.
 * 05/04/91 - Updated per new slrewind() interface in speclist.c file.
 * 05/08/91 - Updated per new slmake() interface in speclist.c file.
 *            Also, added support for -s (subdirectory descent) flag.
 * 06/07/92 - Added -l flag to calculate long (32-bit) CRC values.
 *============================================================================*/

static char copr[] = "(c)Copyright IBM Corporation, 1991.  All rights reserved.";

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/types.h>

#include "util.h"

static  char     buff[2048+1];      /* Buffer for output filenames */

/*============================================================================*
 * Define internal functions to allow forward access
 *============================================================================*/

static void syntax();

/*============================================================================*
 * Main Program Entry Point
 *============================================================================*/

main(argc, argv)

int argc;                           /* arg count */
char *argv[];                       /* arg pointers */

{
  int  rc;                          /* Return code storage */
  long    lrc;                      /* Long return code */
  char   *flagstr;                  /* Pointer to string of flags */

  ushort crc;                       /* CRC value: 16-bit */
  ulong  crc32;                     /* CRC value: 32-bit */
  ulong  len;                       /* File length */

  SLPATH *speclist;                 /* Pointer to linked list of path specs */
  ushort  mflags;                   /* Match flags for slrewind() */
  ushort  recurse;                  /* Recursive directory descent? */
  ushort  find32;                   /* Find 32-bit CRC? */

  FINFO  *fdata;                    /* Pointers returned by slmatch() */
  SLPATH *pdata;
  SLNAME *ndata;

  recurse = FALSE;                  /* Initially: don't descend subdirectories */
  find32 = FALSE;                   /* Initially: find 16-bit CRC, as before */

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
           case 's':
           case 'S':
              recurse = TRUE;
              break;

           case 'l':
           case 'L':
              find32 = TRUE;
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

  if (argc <= 0) syntax();          /* If no arguments: Display syntax */

 /*---------------------------------------------------------------------------*
  * Process file specifications and build specification list
  *---------------------------------------------------------------------------*/

  rc = slmake(&speclist, TRUE, TRUE, argc, argv);

  if (rc != 0)                      /* If there was an error: */
  {                                 /* Analyze rc, show msg, and return */
     if (rc == REGX_MEMORY)
        fprintf(stderr, "Out of memory while processing '%s'.\n",
           slerrspec());
     else
        fprintf(stderr, "Invalid filespec: '%s'.\n",
           slerrspec());

     return(2);
  }

 /*---------------------------------------------------------------------------*
  * Attempt to match files to the specification list
  *---------------------------------------------------------------------------*/

  mflags = 0;                       /* Set list of files to match to include: */
  mflags = mflags | SL_NORMAL |          /* Ordinary files */
                    SL_HIDDEN |          /* Hidden files */
                    SL_SYSTEM;           /* System files */

  slrewind(speclist, mflags, recurse);/* Initialize slmatch() */
  for (;;)                          /* Loop to find all matching DOS files: */
  {
     fdata = slmatch(&pdata,             /* Get next matching DOS file */
                     &ndata);
     if (fdata == NULL)                  /* If none: */
        break;                                /* Done */

     strcpy(buff, pdata->path);          /* Store path portion of filename */
     pathcat(buff, fdata->Fname);        /* Add name to end of path string */
//   strlwr(buff);                       /* And convert to lower case */

     if (find32)                         /* Find file's CRC and length */
        rc = crcfile32(buff, &crc32, &len);
     else
        rc = crcfile(buff, &crc, &len);

     if (rc != 0)                        /* If error: */
     {                                        /* Display message */
        fprintf(stderr, "crc: Cannot process file '%s'.\n", buff);
     }
     else                                /* Else: No error: */
     {                                        /* Display CRC, length, name */
        if (find32)
           printf("%12lu  0x%08lX  %s\n", len, crc32, buff);
        else
           printf("%12lu  0x%04X  %s\n",  len, crc, buff);
     }
  }

 /*---------------------------------------------------------------------------*
  * Display (on stderr) all filespecs that had no matching files
  *---------------------------------------------------------------------------*/

  lrc = slnotfound(speclist);       /* Display path\names w/no matching files */
  if (lrc == 0L)                    /* If all fspecs were matched: */
     return(0);                          /* Return successfully */
  else                              /* Otherwise:  One or more not found: */
     return(2);                          /* Return with error */

} /* end of main() */

/*============================================================================*
 * syntax() - Display command syntax and exit to operating system!
 *============================================================================*/
static void syntax()
{
  fprintf(stderr, "\n");
  fprintf(stderr, "Usage: crc [-s] [-l] fspec ...\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  This program finds the length and CRC for each file that\n");
  fprintf(stderr, "  matches the file specifications.  The information for each\n");
  fprintf(stderr, "  file is displayed on stdout as follows:\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "     length  CRC  filename\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  The length is displayed in decimal format.  The CRC is displayed\n");
  fprintf(stderr, "  in hexadecimal format with a leading '0x'.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  If -s is specified, then crc will descend subdirectories\n");
  fprintf(stderr, "  to look for files that match the file specifications.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  If -l (letter el) is specified, then crc will find the 32-bit\n");
  fprintf(stderr, "  (long) CRC for each file.  The default is to find the 16-bit\n");
  fprintf(stderr, "  CRC for each file.\n");
  fprintf(stderr, "\n");

//fprintf(stderr, "\n");
//fprintf(stderr, "Examples:\n");
//fprintf(stderr, "\n");
//fprintf(stderr, "  crc *.c *.h       Display CRCs for all .c and .h files in the\n");
//fprintf(stderr, "                    current directory.\n");
//fprintf(stderr, "\n");
//fprintf(stderr, "  crc -s c:\\        Display CRCs for all files on drive C.\n");
//fprintf(stderr, "\n");
  exit(1);
}
