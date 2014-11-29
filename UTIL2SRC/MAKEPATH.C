/*============================================================================*
 * module: makepath.c - Make a path.
 *
 * (C)Copyright IBM Corporation, 1992                      Brian E. Yoder
 *
 * Functions contained in this module for use by external programs:
 *   makepath() - Make a path when one of its directories doesn't exist.
 *
 * 09/21/92 - Created.
 * 09/21/92 - Initial version.
 *============================================================================*/
#define LINT_ARGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

//nclude <dir.h>                    /* AIX */
#include <direct.h>                 /* DOS, OS/2 */

#include "util.h"

#define SEP        '\\'     /* Path separator character */

#ifndef PATH_MAX            /* Max. length of a full pathname */
#define PATH_MAX 1024
#endif

/* Mode mask for mkdir() system subroutine (AIX only) */

#define MK_MASK    (S_IRWXU | S_IRWXG | S_IRWXO)

/*---------------------------------------------------------------------------*
 * IsDotDir(fname) returns true if fname is "." or ".."
 *---------------------------------------------------------------------------*/

#ifndef IsDotDir
#define IsDotDir(fname) ((fname)[0] == '.' && (((fname)[1] == '\0') || \
                        ((fname)[1] == '.' && (fname)[2] == '\0')))
#endif

/*============================================================================*
 * makepath() - Make a path when one of its directories doesn't exist.
 *
 * Purpose:
 *   This subroutine takes the full or relative pathname of a file.
 *   It assumes that there is path information and that one or more
 *   directories within the path do not exist.  It also assumes that
 *   the last component of the path is the name of the file itself.
 *   The pathname may also end with a path separator, implying that no
 *   filename is present.
 *
 *   This subroutine checks each directory in the path, starting with
 *   the first one.  When it finds a directory that doesn't exist,
 *   it uses the mkdir() system call to make the directory and all
 *   directories that follow.
 *
 *   For example, assume that you pass the path '/u/fred/dir1/dir2/fname'
 *   to makepath() and that 'dir', 'dir2', and, of course, 'fname' do not
 *   exist.  This subroutine will make the '/u/fred/dir1' directory and
 *   then make the '/u/fred/dir1/dir2' directory.
 *
 * Returns:
 *   0, if no errors.  Nonzero, if an error occurred.
 *============================================================================*/

int makepath(path)

char  *path;                 /* Pointer to 'path/name' of a file */

{
  char  work[PATH_MAX+1];    /* Store 'path/name' here to work on it */

  char *lastsep;             /* Pointer to last path separator before 'name' */
  char *sep;                 /* Pointer to current path separator position */
  char *nextdir;             /* Pointer to next (possibly-nonexistent) dir */

  int   mkflag = FALSE;      /* TRUE=make dir, FALSE=check 1st: it may exist */
  int   mkrc;                /* Store return code from mkdir() here */

 /*=====================================================================*
  * Store 'path' in 'work' buffer, after making sure it's not too long
  *=====================================================================*/

  if ((ulong)strlen(path) > (ulong)PATH_MAX)
     return(2);

  strcpy(work, path);              /* Store path string in buffer */

  lastsep = strrchr(work, SEP);    /* Store location of last separator */
  if (lastsep == NULL)             /* If there are no path separators: */
     return(0);                    /*    Return: there's nothing to do */

  *lastsep = '\0';                 /* Terminate string at end of last dir */

  nextdir = work;                  /* Point to name of first directory */
  if (*nextdir == SEP)             /* Skip leading '/' if present */
     nextdir++;

 /*=====================================================================*
  * Loop to find the first directory that's not there.  When we find it,
  * we will set 'mkflag' to YES.
  *
  * The following pointers are used:
  *  lastsep
  *     Points to the last path separator, just before the filename.
  *     A null byte has been stored in this position, so we are effectively
  *     elminating the filename and working only with the directory(ies).
  *
  *  sep
  *     Used to point to the current separator we are working with.
  *     We will use it to isolate the first part of the path with
  *     the rest of the path.  The first part of the path may contain
  *     at most one (the last one) directory that doesn't exist, or else
  *     mkdir() won't be able to create a directory.
  *
  *  nextdir
  *     Points to the last directory before 'sep'.  We only need to look
  *     at it to see if it's '.' or '..' since these are valid directories
  *     and can be skipped over.
  *=====================================================================*/

  for (;;)                  /* Loop to make each directory in the path */
  {
     sep = strchr(nextdir, SEP);   /* Store location of next separator */
     if (sep == NULL)              /* Note: if 'sep' is NULL, then we must */
        sep = lastsep;             /* point to the end of the path */

     *sep = '\0';                  /* Terminate path at current seperator */

     if (!IsDotDir(nextdir))       /* If not . or .., then check it out: */
     {
        mkrc = 0;                        /* Init to 'no mkdir() error' */
        if (mkflag == FALSE)             /* If we don't know to mkdir or not: */
        {
           if (!isdir(work))                 /* See if directory exists: */
           {                                 /* If it doesn't exist: */
              mkflag = TRUE;                     /* No more will, either */
            //mkrc = mkdir(work, MK_MASK);       /* Make the directory (AIX-style) */
              mkrc = mkdir(work);                /* Make the directory (DOS-style) */
           }
        }
        else                             /* mkflag is TRUE: Skip isdir() test */
        {
         //mkrc = mkdir(work, MK_MASK);      /* Make the directory (AIX-style) */
           mkrc = mkdir(work);               /* Make the directory (DOS-style) */
        }
        if (mkrc != 0)                   /* If we failed mkdir(): */
           return(2);                        /* Return with error */
     }                             /* Endif . or .. */

     if (sep == lastsep)           /* If there are no more dirs in path: */
        break;                     /*    We're done: break out of loop */

     *sep = SEP;                   /* Put separator back */
     nextdir = sep + 1;            /* Next directory to check is past the */
                                   /*    current separator */

  }                         /* end of for loop */

  return(0);
}
