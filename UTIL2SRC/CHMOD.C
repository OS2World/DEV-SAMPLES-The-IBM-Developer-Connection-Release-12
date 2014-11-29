/*============================================================================*
 * main() module: chmod.c - Change file mode.
 *
 * (C)Copyright IBM Corporation, 1991, 1992.                     Brian E. Yoder
 *
 * This program is a loose adaptation of the AIX 'chmod' command.  It changes
 * a file's attributes (mode).  Unlike the DOS ATTRIB command, it allows you
 * to change the mode of system and hidden files and directories.
 *
 * This program views a "readonly" file as having no write capability and
 * NOT as having readonly capability.  DOS and OS/2 seem to me to be a
 * bit odd in their treatment of 'readonly' as a capability!
 *
 * However, a "hidden" file, although excluded from directory searches by
 * the DIR, COPY, and other commands, is perfectly readable by the open()
 * subroutine:  I can look at the hidden '\ibmdos.com' file using the E3
 * editor.  A hidden file does NOT lack the capability of being read.
 *
 * The chmod() library call only lets you set/reset the readonly attribute.
 * Therefore, the DOS-only _dos_setfileattr() subroutine is used to set
 * file attributes.
 *
 * See the chmod.doc file for more information on use and examples.
 *
 * 04/18/91 - Created.
 * 04/19/91 - Version 1.0 - We cannot change the attributes of a directory.
 * 04/30/91 - Version 1.0 - Ported to OS/2.
 * 05/06/91 - Version 1.1 - Updated per new slrewind() interface in speclist.c.
 * 05/09/91 - Version 1.2 - Support new interface to slmake(): Don't append
 *                          "\*" to end of fspecs that are directory names.
 * 11/20/91 - Version 1.3 - Support -R to recursively descend directories.
 * 07/24/92 - Version 1.3 - For OS/2 2.0 and C Set/2. pathmode is uint.
 *============================================================================*/

static char copr[] = "(c)Copyright IBM Corp, 1991, 1992.  All rights reserved.";
static char ver[]  = "Version 1.3  07/24/91";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "util.h"

#define NO       0
#define YES      1

static  char     buff[2048+1];      /* Buffer for output filenames */

/*============================================================================*
 * Define internal functions to allow forward access
 *============================================================================*/

static void       syntax       ( void );
static int        fchmod       ( char *, uint, char, int, int, int, int );

/*============================================================================*
 * Main Program Entry Point
 *============================================================================*/

main(argc, argv)

int argc;                           /* arg count */
char *argv[];                       /* arg pointers */

{
  int     rc;                       /* Return code store area */
  long    lrc;                      /* Long return code */
  char   *modespec;                 /* Pointer to mode specification string */
  char    cf;                       /* Current flag character (if any) */

  char    opchar;                   /* Operation char: +-= */
  int     w_mode;                   /* Mode settings */
  int     s_mode;
  int     h_mode;
  int     a_mode;

  SLPATH *speclist;                 /* Pointer to linked list of path specs */
  ushort  mflags;                   /* Match flags for slrewind() */
  ushort  recurse = FALSE;          /* Recursive directory descent? */

  FINFO  *fdata;                    /* Pointers returned by slmatch() */
  SLPATH *pdata;
  SLNAME *ndata;

 /*---------------------------------------------------------------------------*
  * Check arguments
  *---------------------------------------------------------------------------*/

  argc--;                           /* Ignore 1st argument (program name) */
  argv++;

  if (argc != 0)                    /* If we have at least one argument: (v1.3) */
  {
     if (strcmpi(*argv, "-R") == 0)      /* If it's -R: */
     {
        recurse = TRUE;                       /* Set flag to allow dir descent */
        argc--;                               /* And throw away the flag */
        argv++;
     }                                   /* endif */
  }                                 /* endif */

  if (argc < 2)                     /* If we don't have enough arguments: */
     syntax();                           /* Display syntax and exit */

  modespec = *argv;                 /* 1st argument is mode specification */
  argc--;                           /* Remaining args are file specs */
  argv++;

 /*---------------------------------------------------------------------------*
  * Process mode specification and store in 'opchar' and 'modeset'
  *
  * Initially, we will set the readonly bit if 'w' is specified and leave it
  * 0 if 'w' is not specified.  Once 'modeset' has been set up, we will
  * invert the readonly bit before continuing on.
  *---------------------------------------------------------------------------*/

  w_mode = FALSE;                   /* Initialize modes to 'not specified' */
  s_mode = FALSE;
  h_mode = FALSE;
  a_mode = FALSE;

  opchar = *modespec;               /* Check first char: operation: */
  switch (opchar)
  {
     case '-':                           /* It must be one of these */
     case '+':
     case '=':
        break;
     default:                            /* Anything else is an error */
        syntax();
        break;
  }

  modespec++;                       /* Bump modespec past the operation */
  cf = *modespec;                   /* Store first mode letter specified */
  if (cf == '\0') syntax();         /* If none: error */

  while (cf != '\0')                /* For each mode letter: */
  {
     switch(cf)                          /* Check mode letter and update mode */
     {                                   /* setting appropriately */
        case 'w':
           w_mode = TRUE;
           break;
        case 's':
           s_mode = TRUE;
           break;
        case 'h':
           h_mode = TRUE;
           break;
        case 'a':
           a_mode = TRUE;
           break;
        default:                            /* Anything else is an error */
           syntax();
           break;
     }
     modespec++;                         /* Point to next mode letter */
     cf = *modespec;                     /* And store it in cf */
  }

 /*---------------------------------------------------------------------------*
  * Process file specifications and build specification list
  *---------------------------------------------------------------------------*/

  rc = slmake(&speclist, TRUE, FALSE, argc, argv);

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
     pathcat(buff, fdata->achName);      /* Add name to end of path string */

     rc = fchmod(buff, fdata->attrFile,  /* Set new mode for file */
                 opchar, w_mode,
                         s_mode,
                         h_mode,
                         a_mode);

     if (rc != 0)                        /* If error: Dipslay and continue */
        fprintf(stderr, "chmod: Can't change mode for '%s'\n", buff);
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
static void syntax(void)
{
  fprintf(stderr, "%s\n", ver);
  fprintf(stderr, "Usage:  chmod [-R] [+-=][wsha] fspec ...\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Exactly one file mode operator must be followed by one or more\n");
  fprintf(stderr, "file more mode letters, with no intervening spaces.  One or more\n");
  fprintf(stderr, "file specifications must follow.  All files, including system and\n");
  fprintf(stderr, "hidden files, that match the file specification(s) are processed.\n");
  fprintf(stderr, "Directories and volume IDs are ignored.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "The file mode operator can be one of the following:\n");
  fprintf(stderr, "  +  Set the specified file mode(s).\n");
  fprintf(stderr, "  -  Reset the specified file mode(s).\n");
  fprintf(stderr, "  =  Set the specified file mode(s) and reset those not specified.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "The file modes that can be specified are:\n");
  fprintf(stderr, "  w  Write permission.\n");
  fprintf(stderr, "  s  System attribute.\n");
  fprintf(stderr, "  h  Hidden attribute.\n");
  fprintf(stderr, "  a  Archive attribute.\n");
  fprintf(stderr, "\n");
  exit(1);
}

/*============================================================================*
 * fchmod() - Change file's mode (attributes).        For DOS Only
 *
 * REMARKS:
 *   This subroutine changes the attributes for the specified file or
 *   directory, depending upon the operation.  The write permission,
 *   system attribute, hidden attribute, and archive bit are specified
 *   by the w, s, h, and a flags, respectively.  A value of TRUE means
 *   the flag was specified; FALSE means it was omitted.
 *
 *   The opchar tells what to do with the flags:
 *      + means set the attributes whose flags are TRUE.  For example,
 *        if the opchar is + and only the a flag is TRUE, then the archive
 *        bit will be set for the specified file.
 *
 *      - means to reset the attributes whose flags are TRUE.
 *
 *      = means to set the attributes whose flags are TRUE and reset the
 *        attributes whose flags are FALSE.
 *
 *   The bits in the pathmode variable are defined by the _A_ #defines
 *   in <dos.h>.  Only the following file attributes are processed:
 *       _A_RDONLY
 *       _A_SYSTEM
 *       _A_HIDDEN
 *       _A_ARCH
 *
 * RETURNS:
 *   0: Successful.
 *   Other: error.
 *============================================================================*/

static int fchmod(

char    *path      ,                /* Path/name of file or directory */
uint     pathmode  ,                /* Current file mode */
char     opchar    ,                /* Operation: - + or = */
int      w         ,                /* Modes specified: write (NOT readonly) */
int      s         ,                /*                  system */
int      h         ,                /*                  hidden */
int      a         )                /*                  archive */

{
  int   rc;                         /* Return code storage */

/*printf("Before: pathmode = 0x%04X\n", pathmode);*/

 /*---------------------------------------------------------------------------*
  * Depending upon opchar and the flags, alter the file's attributes that are
  * stored in 'pathmode'.  To set an attribute, OR its _A_ label with the
  * current mode.  To reset an attribute, AND the bit-wise inversion of
  * its _A_ label.  To get the bit-wise inversion of a label, exclusive OR the
  * label with 0xFFFF.
  *
  * Note:  To set write permission, we must reset the readonly bit.  To remove
  * write permission, we must set the readonly bit.
  *---------------------------------------------------------------------------*/

  switch (opchar)                   /* Depending upon the operation: */
  {
     case '+':
       /*------------------------------------------------------------*
        * Set the specified attributes
        *------------------------------------------------------------*/
        if (w)
           pathmode = pathmode & (0xFFFFFFFF ^ _A_RDONLY);
        if (s)
           pathmode = pathmode | _A_SYSTEM;
        if (h)
           pathmode = pathmode | _A_HIDDEN;
        if (a)
           pathmode = pathmode | _A_ARCH;
        break;

     case '-':
       /*------------------------------------------------------------*
        * Reset the specified attributes
        *------------------------------------------------------------*/
        if (w)
           pathmode = pathmode | _A_RDONLY;
        if (s)
           pathmode = pathmode & (0xFFFFFFFF ^ _A_SYSTEM);
        if (h)
           pathmode = pathmode & (0xFFFFFFFF ^ _A_HIDDEN);
        if (a)
           pathmode = pathmode & (0xFFFFFFFF ^ _A_ARCH);
        break;

       /*------------------------------------------------------------*
        * Set the specified attributes and reset those not specified
        *------------------------------------------------------------*/
     case '=':
        if (w)
           pathmode = pathmode & (0xFFFFFFFF ^ _A_RDONLY);
        else
           pathmode = pathmode | _A_RDONLY;

        if (s)
           pathmode = pathmode | _A_SYSTEM;
        else
           pathmode = pathmode & (0xFFFFFFFF ^ _A_SYSTEM);

        if (h)
           pathmode = pathmode | _A_HIDDEN;
        else
           pathmode = pathmode & (0xFFFFFFFF ^ _A_HIDDEN);

        if (a)
           pathmode = pathmode | _A_ARCH;
        else
           pathmode = pathmode & (0xFFFFFFFF ^ _A_ARCH);
        break;
  }

/*printf("After:  pathmode = 0x%04X\n", pathmode);*/

 /*---------------------------------------------------------------------------*
  * Change the file's attributes to the value stored in 'pathmode'
  *---------------------------------------------------------------------------*/

//rc = _dos_setfileattr(path, pathmode);           /* DOS */
//rc = DosSetFileMode(path, pathmode, 0L);         /* OS/2 16-bit */
  rc = SetFileMode(path, pathmode);                /* OS/2 32-bit */

  return(rc);
}
