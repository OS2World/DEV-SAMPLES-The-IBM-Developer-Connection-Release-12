/*============================================================================*
 * main() module: cdir.c - Change directory.
 *
 * (C)Copyright IBM Corporation, 1991.                           Brian E. Yoder
 *
 * This program is a loose adaptation of the AIX 'cd' command.  See the
 * CDX.DOC file for usage information.
 *
 * On OS/2, a program cannot change the current drive and path.  Therefore,
 * we only write the d:path (and d:, if specified) to stdout.  The cdx
 * command uses this information to change the current working drive and path.
 *
 * 04/17/91 - Created.
 * 04/15/91 - Initial version 1.0.
 * 04/30/91 - Ported to OS/2.  Is called by cdx.cmd -- will not work directly.
 *            Also, if we are changing to a relative directory that is not
 *            within our current path, we no longer write the name to stdout.
 *            AIX's cd command did, but with OS/2's 'prompt $p$g', it should
 *            be obvious where we are.
 *============================================================================*/

static char copr[] = "(c)Copyright IBM Corporation, 1991.  All rights reserved.";
static char ver[]  = "Version 1.0,  4/30/91";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "util.h"

#define NO       0
#define YES      1

#define  MAX_CPY 1024               /* Max length of strings, during strcpy() */
#define  MAX_LEN 2048               /* Max buffer lengths */

/*----------------------------------------------------------------------------*
 * Buffers used by this program
 *----------------------------------------------------------------------------*/

static char cdbuff[MAX_LEN+1];      /* Buffer for contents of CDPATH var */
static char cdname[MAX_LEN+1];      /* Buffer for contents of a name */

/*============================================================================*
 * Define internal functions to allow forward access
 *============================================================================*/

static int        gopath       ( char  *);

/*============================================================================*
 * Main Program Entry Point
 *============================================================================*/

main(argc, argv)

int argc;                           /* arg count */
char *argv[];                       /* arg pointers */

{
  int     rc;                       /* Return code store area */
  long    lrc;                      /* Long return code */
  char   *path;                     /* Pointer to drive/path specification */
  char   *home;                     /* Pointer to HOME var string */
  char   *cdpath;                   /* Pointer to CDPATH var string */
  char   *pstr;                     /* Pointer to generic string */

  argc--;                           /* Ignore 1st argument (program name) */
  argv++;

 /*---------------------------------------------------------------------------*
  * Store pointers to environment variables:
  *---------------------------------------------------------------------------*/

  home = getenv("HOME");
  cdpath = getenv("CDPATH");

 /*---------------------------------------------------------------------------*
  * If no arguments entered, try to find/go to HOME directory
  *---------------------------------------------------------------------------*/

  if (argc == 0)                    /* If no arguments entered: */
  {
     if (home != NULL)                   /* If we have a HOME path: */
     {
        strncpy(cdname, home, MAX_CPY);       /* Copy it to buffer */
        rc = gopath(cdname);                  /* Go to it */
        if (rc != 0)
           fprintf(stderr, "'%s': No such directory.\n", cdname);
     }
     else                                /* Else we don't have a home path: */
     {                                        /* Display error */
        fprintf(stderr, "No HOME path defined.\n");
        rc = 2;                               /* Set error return code */
     }
     return(rc);                         /* Return to caller */
  }

  path = *argv;                     /* Point path to argument string */
  if (*path == '\0')                /* If path string is zero length: */
     return(0);                     /* Then it's ok to stay where we are */

 /*---------------------------------------------------------------------------*
  * If we have a drive specification or an absolute path,
  * then attempt to go to drive/path
  *---------------------------------------------------------------------------*/

  if ( hasdrive(path)  ||           /* If we have a drive spec in the path: */
       (*path == '\\') ||           /* Or the path is absolute: */
       (*path == '/') )
  {
     rc = gopath(path);                  /* Go to it */
     if (rc != 0)                        /* See if it worked */
        fprintf(stderr, "'%s': No such directory.\n", path);
     return(rc);                         /* Return */
  }

 /*---------------------------------------------------------------------------*
  * We have a relative path:  first try to go directly to it
  *---------------------------------------------------------------------------*/

  rc = gopath(path);                /* Try to go to it */
  if (rc == 0)                      /* If we succeed: then return */
     return(0);                     /* Otherwise, we'll go check cdpath */

 /*---------------------------------------------------------------------------*
  * We have no drive spec:  If we have a relative path, look in current
  * dir and then CDPATH for it
  *---------------------------------------------------------------------------*/

  if (cdpath != NULL)               /* If we have a CDPATH path list: */
     strncpy(cdbuff, cdpath,        /* Copy it to the buffer */
             MAX_CPY);              /* Otherwise, we're out of luck */
  else
  {
     fprintf(stderr, "'%s': No such directory.\n", path);
     return(2);
  }

  pstr = strtok(cdbuff, ";");       /* Point to 1st path in list, and */
  while (pstr != NULL)              /* Loop to check paths: */
  {
     strncpy(cdname, pstr, MAX_CPY);     /* Copy CDPATH path to buffer */
     if (cdname[0] != '\0')              /* If it has a non-zero length: */
     {
        pathcat(cdname, path);                /* Append our path string to it */
        if (isdir(cdname))                    /* If it's a valid directory: */
        {
           rc = gopath(cdname);                    /* Go to it */
           if (rc == 0)                            /* If ok: */
           {
// OS/2 ver:  printf("%s\n", cdname);                   /* Print the new dir name */
              return(0);                                /* Return */
           }
        }                                     /* Endif */
     }                                   /* Either: 0-length, not a dir, or */
     pstr = strtok(NULL, ";");           /* failed gopath(): Point to next path */
  }                                 /* End of path check loop */

 /*---------------------------------------------------------------------------*
  * We have looked everywhere, but the path cannot be found
  *---------------------------------------------------------------------------*/

  fprintf(stderr, "'%s': No such directory.\n", path);
  return(2);                        /* Return */

} /* end of main() */

/*============================================================================*
 * gopath() - Go to a specified drive and path.
 *
 * REMARKS:
 *   This subroutine changes the current directory on a specified drive,
 *   and changes the current drive to the specified drive.  If the drive
 *   is missing, then the current drive is not changed.
 *
 *   The best we can do for error checking is to verify that the specified
 *   drive and path exists.  If the system() command to change the working
 *   drive fails, we're out of luck -- DOS won't tell us.
 *
 *   For OS/2, this subroutine only writes the pathname (and drive, if
 *   applicable) to stdout.  It's up to the calling 'cdx' command to
 *   use this output to change the session's current path and drive.
 *
 * RETURNS:
 *   0: Successful.
 *   2: Failed.
 *============================================================================*/

static int  gopath(path)

char    *path;                      /* Pointer to [d:][path] string */

{
  int  rc;                          /* Return code storage */
  char drv[3];                      /* To store drive string */

  if (*path == '\0')                /* If path is a null string: */
     return(0);                     /* Then it's ok to stay where we are */

  if (!isdir(path))                 /* If it's not a path, then we can't */
     return(2);                     /* possibly go to it */

//chdir(path);   /* DOS method */   /* Set new current working directory */

  printf("%s", path);               /* Write path to stdout */

  if (hasdrive(path))               /* If the path has a drive spec */
  {
     memcpy(drv, path, 2);               /* Store drive and colon string */
     drv[2] = '\0';                      /* in the drv[] buffer.  Then: */
//   system(drv);                        /* Invoke DOS to set new default drive */
     printf(" %s", drv);                 /* Write path to stdout */
  }

  printf("\n");                     /* Add newline */
  fflush(stdout);                   /* Flush stdout buffer */
  return(0);                        /* Ok, to the best of our knowledge */
}
