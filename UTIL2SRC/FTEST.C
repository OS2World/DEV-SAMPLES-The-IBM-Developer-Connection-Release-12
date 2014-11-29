/*============================================================================*
 * main() module: ftest.c - Test various file functions.  OS/2 2.0 and C Set/2
 *
 * (C)Copyright IBM Corporation, 1991, 1992                      Brian E. Yoder
 *
 * 04/05/91 - Created.
 * 04/17/91 - Initial version.
 * 04/29/91 - Ported to OS/2.
 * 05/06/91 - Updated per new slrewind() interface in speclist.c file.
 * 05/09/91 - Updated per new slmake() interface in speclist.c file.
 * 05/16/91 - Added check for maximum system path length.
 * 04/13/92 - Added tests for current drive and filesystem type, and ishpfs().
 *            The structure and info for DosQFSAttach() is in bsedos.h.
 * 07/23/92 - Ported to OS/2 2.0 and removed query for max. pathname.
 * 07/24/92 - Added getwidth() function from ls.c, for testing.
 * 07/30/92 - Added IsDir() to test stat() under 2.0 and with long HPFS names.
 *            The C Set/2 migration library version of stat() works!
 *            (The IBM C/2 1.1 library version of stat() didn't.)
 * 09/19/92 - Print the bpathlen field of SLPATH, and added test for makepath().
 * 04/20/93 - Fix an apparent bug in the D option processing whereby the ptr   .
 *            to the remote name was 2 bytes past where it should have been.
 *            The original code worked for 16-bit OS/2 programs, but not
 *            for 32-bit programs. Strange--needs to be looked into sometime.
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "util.h"

/*----------------------------------------------------------------------------*
 * Static data
 *----------------------------------------------------------------------------*/

static  int   uniq = TRUE;

static  char  buff[2048];

static  PATH_NAME *pn;
static  char  path[1024];
static  char  name[1024];

static  ushort  maxpath;            /* Maximum system path length */

/*============================================================================*
 * Define internal functions to allow forward access
 *============================================================================*/

static void       syntax       ( void );
static uint       getwidth     ( void );
static int        IsDir        ( char * );

/*============================================================================*
 * Main Program Entry Point
 *============================================================================*/

main(argc, argv)

int argc;                           /* arg count */
char *argv[];                       /* arg pointers */

{
  int     rc;                       /* Return code store area */
  char   *str;                      /* Pointer to string */
  char    id;                       /* Test id */
  int     dwidth;                   /* Width of display */

  ushort  mflags;                   /* For slrewind() */
  SLPATH *speclist;                 /* Pointer to linked list of path specs */

  FINFO  *fdata;                    /* Pointers returned by slmatch() */
  SLPATH *pdata;
  SLNAME *ndata;

  ulong   lval;
  char   *endcvt;

  ulong   drivenum;                 /* Drive number */
  ulong   drivemap;                 /* Drive map */
  char    drivestr[3];              /* String: "D:" */
  char   *fsname = "";              /* Pointer to filesystem name */

  char    sbuff[128];               /* Character array buffer */
  ULONG   bufferlen = sizeof(sbuff);
  FSQBUFFER2 *buffer;               /* For DosQFSAttach() */
  USHORT *ubuff;                    /* Pointer to USHORT array */

 /*----------------------------------------------------------------------------*
  * Check arguments
  *----------------------------------------------------------------------------*/

  argc--;                 /* Ignore 1st argument (program name) */
  argv++;

  if (argc < 1)           /* Be sure there are at least two arguments */
     syntax();

  id = *(argv[0]);        /* Store test identifier */

  argc--;                 /* Bump args past test identifier */
  argv++;

 /*----------------------------------------------------------------------------*
  * In case we call slrewind, set up mflags:
  *----------------------------------------------------------------------------*/

  mflags = 0;
  mflags = mflags | SL_NORMAL |
                    SL_SYSTEM |
                    SL_HIDDEN |
                    SL_DIR    |
                    SL_DOTDIR |
                    SL_VOLID;

 /*----------------------------------------------------------------------------*
  * Run tests, depending upon test id:
  *----------------------------------------------------------------------------*/

  switch (id)
  {
    /*---------------------------------------------------------*
     * Test decompose()
     *---------------------------------------------------------*/
     case 'd':
        if (argc < 1)
        {
           printf("ftest d [d:][path]name\n");
           return(2);
        }
        printf("Testing decompose()...\n");
        printf("String is '%s'...\n", argv[0]);
        pn = decompose(argv[0]);

        memcpy(path, pn->path,       /* Store path portion and terminate it */
                     pn->pathlen);
        *(path+pn->pathlen) = '\0';

        memcpy(name, pn->name,       /* Store name portion and terminate it */
                     pn->namelen);
        *(name+pn->namelen) = '\0';

        printf("[d:path] = '%s'\n", path);
        printf("name     = '%s'\n", name);
        break;

    /*---------------------------------------------------------*
     * Test isdir()
     *---------------------------------------------------------*/
     case 'i':
        if (argc < 1)
        {
           printf("ftest i pathname\n");
           return(2);
        }
        printf("Testing isdir()...\n");
        rc = isdir(argv[0]);
        if (rc == TRUE)
           printf("    String '%s' is a directory.\n", argv[0]);
        else
           printf("--- String '%s' is not a directory.\n", argv[0]);
        break;

    /*---------------------------------------------------------*
     * Test for directory using stat().
     *---------------------------------------------------------*/
     case 'I':
        if (argc < 1)
        {
           printf("ftest I pathname\n");
           return(2);
        }
        printf("Testing stat()...\n");
        rc = IsDir(argv[0]);
        if (rc == TRUE)
           printf("    String '%s' is a directory.\n", argv[0]);
        else
           printf("--- String '%s' is not a directory.\n", argv[0]);
        break;

    /*---------------------------------------------------------*
     * Check current drive and its filesystem
     *---------------------------------------------------------*/
     case 'D':
        printf("Querying current drive's filesystem...\n");

        rc = DosQueryCurrentDisk(&drivenum, &drivemap);

        drivestr[0] = 'A' + drivenum - 1;
        drivestr[1] = ':';
        drivestr[2] = '\0';

        printf("Current drive: %s\n", drivestr);

        rc = DosQFSAttach(drivestr,        /* Query FS name for this drive */
                      0L,                  /* Ordinal is ignored */
                      FSAIL_QUERYNAME,     /* Info level: query name */
                      (FSQBUFFER2 *)sbuff, /* Pointer to info buffer */
                      &bufferlen);         /* Size of buffer */

        printf("DosQFSAttach() returned %d\n", rc);

        if (rc == 0)                       /* If ok: */
        {
           ubuff = (USHORT *)sbuff;             /* Point ubuff to start of buffer */
           buffer = (FSQBUFFER2 *)sbuff;        /* Point buffer to FS info hdr */
           switch (buffer->iType)               /* Check iType of filesystem: */
           {
              case FSAT_LOCALDRV:
                 fsname = &sbuff[ 7 + ubuff[2] ];    /* FAT or HPFS */
                 break;

              case FSAT_REMOTEDRV:
                 fsname = &sbuff[ 7 + ubuff[2] ];    /* usually LAN */

                 str = &sbuff[ 7 + ubuff[2] ];
                 str += strlen(str);
                 str += 1;
//(4/20/93)--->  str += 2;
                 printf("Remote name:   %s\n", str);

                 break;

              default:
                 fsname = "*unknown*";
                 break;
           }

           printf("Filesystem:    %s\n", fsname);
        }

        break;

    /*---------------------------------------------------------*
     * Test ishpfs()
     *---------------------------------------------------------*/
     case 'h':
        if (argc < 1)
        {
           printf("ftest h pathname\n");
           return(2);
        }
        printf("Testing ishpfs()...\n");
        rc = ishpfs(argv[0]);
        if (rc == TRUE)
           printf("    Filesystem on %s is HPFS.\n", argv[0]);
        else
           printf("    Filesystem on %s is *not* HPFS.\n", argv[0]);
        break;

    /*---------------------------------------------------------*
     * Test pathcat()
     *---------------------------------------------------------*/
     case 'p':
        if (argc < 2)
        {
           printf("ftest p [d:][path] [name]\n");
           return(2);
        }
        printf("Testing pathcat()...\n");
        if (argc != 2)
        {
           printf("--- Two strings needed for pathcat().\n");
           return(0);
        }
        strcpy(buff, argv[0]);
        pathcat(buff, argv[1]);
        printf("Result: '%s'\n", buff);
        break;

    /*---------------------------------------------------------*
     * Test slmake()
     *---------------------------------------------------------*/
     case 's':
        if (argc < 1)
        {
           printf("ftest s fspec ...\n");
           return(2);
        }
        printf("Testing slmake()...\n");
        rc = slmake(&speclist, uniq, TRUE, argc, argv);
        switch (rc)
        {
           case 0:
              break;

           case REGX_MEMORY:
              fprintf(stderr, "Out of memory while processing '%s'.\n",
                 slerrspec());
              return(1);
              break;

           default:
              fprintf(stderr, "Invalid filespec: '%s'.\n",
                 slerrspec());
              return(1);
              break;
        }
        sldump(speclist);            /* Dump the specification list */
        break;

    /*---------------------------------------------------------*
     * Test slmatch() (need to call slmake() first)
     *---------------------------------------------------------*/
     case 'm':
        if (argc < 1)
        {
           printf("ftest m fspec ...\n");
           return(2);
        }
        printf("Testing slmatch()...\n");
        rc = slmake(&speclist, uniq, TRUE, argc, argv);
        switch (rc)
        {
           case 0:
              break;

           case REGX_MEMORY:
              fprintf(stderr, "Out of memory while processing '%s'.\n",
                 slerrspec());
              return(1);
              break;

           default:
              fprintf(stderr, "Invalid filespec: '%s'.\n",
                 slerrspec());
              return(1);
              break;
        }
        printf("'Pathname'  'Filename'  'Filename specification':\n");
        slrewind(speclist, mflags,   /* Initialize slmatch() */
                           TRUE);
        for (;;)                     /* Loop to find all matching files: */
        {
           fdata = slmatch(&pdata,
                           &ndata);
           if (fdata == NULL)
              break;
           printf("   '%s' (%u) '%s' '%s'%s\n",
              pdata->path,
              pdata->bpathlen,
              fdata->achName,               /* For OS/2 */
              ndata->name,
              pdata->inserted ? " (inserted)" : "");
        }
        break;

    /*---------------------------------------------------------*
     * Test SetFileMode().  File mode bits are:
     *
     * FILE_NORMAL    =  0x0000  =  0000 0000 (binary)
     * FILE_READONLY  =  0x0001  =  0000 0001
     * FILE_HIDDEN    =  0x0002  =  0000 0010
     * FILE_SYSTEM    =  0x0004  =  0000 0100
     * FILE_DIRECTORY =  0x0010  =  0001 0000
     * FILE_ARCHIVED  =  0x0020  =  0010 0000
     *---------------------------------------------------------*/
     case 'f':
        if (argc < 2)
        {
           printf("ftest f mode filename\n");
           return(2);
        }

        lval = strtoul(argv[0],      /* Convert to ulong, using */
                       &endcvt,      /* 1st char of string to */
                       0);           /* determine the base */

        if (*endcvt != '\0')
        {
           printf("'%s' is not a valid number.\n", argv[0]);
           return(2);
        }

        printf("Testing SetFileMode()...\n");
        printf("Mode: 0x%08x   File: %s\n", lval, argv[1]);

        rc = SetFileMode(argv[1], (uint)lval );
        printf("Return value: %d\n", rc);
        break;

    /*---------------------------------------------------------*
     * Check display width
     *---------------------------------------------------------*/
     case 'w':
        dwidth = getwidth();
        printf("The current display window has %d columns.\n", dwidth);
        break;

    /*---------------------------------------------------------*
     * Test makepath()
     *---------------------------------------------------------*/
     case 'P':
        printf("Testing makepath()...\n");
        rc = makepath(argv[0]);
        printf("%s\n", rc==0 ? "Ok" : "failed");
        break;

    /*---------------------------------------------------------*
     * We don't know about any other test id
     *---------------------------------------------------------*/
     default:
        printf("--- Test id '%c' is not supported.\n", id);
        break;
  }

  return(0);                        /* Done with main(): Return */

} /* end of main() */

/*============================================================================*
 * syntax() - Display command syntax and exit to operating system!
 *============================================================================*/
static void syntax()
{
  fprintf(stderr, "Usage:  ftest id string [string ...]\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Valid test ids:\n");
  fprintf(stderr, "    d - Test decompose().\n");
  fprintf(stderr, "    h - Test ishpfs().\n");
  fprintf(stderr, "    i - Test isdir().\n");
  fprintf(stderr, "    I - Check for directory using stat().\n");
  fprintf(stderr, "    p - Test pathcat().\n");
  fprintf(stderr, "    s - Test slmake().\n");
  fprintf(stderr, "    m - Test slmatch().\n");
  fprintf(stderr, "    D - Query current drive and its filesystem.\n");
  fprintf(stderr, "    f - Test SetFileMode().\n");
  fprintf(stderr, "    w - Print the display's width.\n");
  fprintf(stderr, "    P - Test makepath().\n");
  fprintf(stderr, "\n");
  exit(1);
}

/*============================================================================*
 * getwidth() - Gets width of display.
 *
 * REMARKS:
 *   This subroutine gets the width of the current display.  If this width
 *   is wider than MAXWIDTH, it returns MAXWIDTH.
 *============================================================================*/

static uint getwidth()
{
  int  rc;                          /* Return code storage */
  VIOMODEINFO vmi;                  /* Return info from vio */
  uint dwidth;                      /* Width of display */

  rc = VioGetMode(&vmi, 0);         /* Get video mode */

  if (rc != 0)                      /* If error: */
     dwidth = 80;                      /* Assume 80 cols */
  else                              /* Else no error: */
  {                                    /* Extract info from buffer */
     dwidth = vmi.col;
  }

  return(dwidth);                   /* Return the width */
}

/*============================================================================*
 * IsDir() - Does a string represent a valid directory name?
 *
 * REMARKS:
 *   This subroutine checks to see if a string represents a valid directory
 *   name.
 *
 *   A null string represents the current directory and is a valid directory.
 *
 *   The stat() function call for the IBM C/2 1.1 library doesn't support
 *   long filenames.  Let's see what it does using the C Set/2 migration
 *   library.
 *
 * RETURNS:
 *   FALSE, if it 'dirstr' is not a valid directory.
 *   TRUE, if it is.
 *============================================================================*/
int IsDir(

char *dirstr )                      /* Pointer to a directory name string */

{
  int rc;                           /* Return code store area */
  struct stat statb;                /* stat() structure information buffer */
  int dlen;                         /* Length of dirstr string */
  char *enddirstr;                  /* Pointer to end of dirstr specification */

 /*---------------------------------------------------------------------------*
  * Get length of dirstr and a pointer to the last character
  *---------------------------------------------------------------------------*/

  dlen = strlen(dirstr);            /* Get length */
  enddirstr = dirstr + dlen - 1;    /* Store pointer to the end */

  if (dlen == 0)                    /* If string is zero length: */
     return(TRUE);                       /* It's the current directory */

 /*---------------------------------------------------------------------------*
  * Take care of checks for special cases
  *---------------------------------------------------------------------------*/

  if (strcmp(dirstr, ".") == 0)     /* If "." it's a valid dir spec */
     return(TRUE);

  if (strcmp(dirstr, "..") == 0)    /* If ".." it's a valid dir spec */
     return(TRUE);

  if ((dlen == 2) &&                /* If "d:" drive spec: it's valid */
      (*enddirstr == ':'))
     return(TRUE);

 /*---------------------------------------------------------------------------*
  * Be sure that the dirstr path exists and is a directory
  *---------------------------------------------------------------------------*/

  rc = stat(dirstr, &statb);

  if (rc != 0)
     return(FALSE);

  if ((statb.st_mode & S_IFDIR) == 0)
     return(FALSE);

  return(TRUE);                     /* We passed all of the tests */
}
