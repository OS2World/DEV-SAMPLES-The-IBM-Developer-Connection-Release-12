/*============================================================================*
 * module: FUTIL.C - File utility subroutines: OS/2 2.0 (32-bit) version.
 *
 * (C)Copyright IBM Corporation, 1987, 1988, 1991, 1992, 1993.  Brian E. Yoder
 *
 * Functions contained in this module:
 *   cvtftime()    - Convert file's time/date to FTM structure type.
 *   pathcat()     - Concatenate a path string onto a base path string.
 *   isdir()       - Does a string represent a valid directory name?
 *   hasdrive()    - Does a string have a drive specification?
 *   ishpfs()      - Does a string represent a pathname on an HPFS filesystem?
 *   decompose()   - Decompose '[d:path]name' into 'd:path' and 'name'.
 *   SetFileMode() - Set file's mode (replacement for DosSetFileMode).
 *
 * 04/16/91 - Initial version created from functions in the ffind.c module.
 * 04/17/91 - Added hasdrive().
 * 04/30/91 - Ported to OS/2.  Don't need to include <dos.h>.
 * 05/09/91 - Use #defined labels in case stmt in pathcat().
 * 06/11/91 - Updated isdir() to support long directory names.
 * 04/13/92 - Added ishpfs(). For now: valid on locally-attached drives only.
 * 07/27/92 - Ported to OS/2 2.0 and C Set/2. The SetFileMode() was added
 *            to replace the DosSetFileMode() which is in 1.3 but not 2.0.
 * 04/20/93 - Changed ishpfs() to also return TRUE if the filesystem is
 *            a remote NFS filesystem.
 * 09/28/93 - Changed ishpfs() to also return TRUE if the filesystem is
 *            a remote TVFS filesystem.
 *============================================================================*/
#define LINT_ARGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\types.h>
#include <sys\stat.h>

#include "util.h"

/*============================================================================*
 * Static data
 *============================================================================*/

static char  pathsep[] = "\\";   /* Path separator string */

#define SEP '\\'                 /* Path separator character definition */

/*----------------------------------------------------------------------------*
 * Static structure overwritten by calls to decompose()
 *----------------------------------------------------------------------------*/

static PATH_NAME pn;

/*============================================================================*
 * cvtftime() - Convert file's time and date stamps to FTM structure type.
 *
 * REMARKS:
 *   This function converts a file's time and date words into month-day-year
 *   and hours-minutes-seconds and stores them in the FTM structure type.
 *
 *   The file's time word (16 bits) is mapped by DOS as follows:
 *
 *       <   high-order byte   >  <   low-order byte   >
 *       15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 *        h  h  h  h  h  m  m  m  m  m  m  x  x  x  x  x
 *
 *              hh is the number of hours.
 *              mm is the number of minutes.
 *              xx is the number of two-second increments.
 *
 *   The file's date word (16 bits) is mapped by DOS as follows:
 *
 *       <   high-order byte   >  <   low-order byte   >
 *       15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 *        y  y  y  y  y  y  y  m  m  m  m  d  d  d  d  d
 *
 *              yy is the year:  0-119  (1980-2099).
 *              mm is the month.
 *              dd is the day of the month.
 *
 *   See the DOS technical reference for more information.
 *
 * RETURNS: 0
 *============================================================================*/
int cvtftime(

unsigned  ftime ,       /* DOS file time */
unsigned  fdate ,       /* DOS file date */
FTM      *ftm   )       /* Pointer to caller's file time structure */

{
  ftm->ftm_sec =  (ftime & 0x001F) << 1;
  ftm->ftm_min =  (ftime >> 5) & 0x003F;
  ftm->ftm_hr =   (ftime >> 11) & 0x001F;

  ftm->ftm_day =  fdate & 0x001F;
  ftm->ftm_mon =  (fdate >> 5) & 0x000F;
  ftm->ftm_year = 1980 + ((fdate >> 9) & 0x007F);

  return(0);                        /* Return 0: successful */
}

/*============================================================================*
 * pathcat() - Concatenate a path string onto a base path string.
 *
 * Description:
 *   This subroutine concatenates the string 'str' to the 'base' pathname.
 *   The calling program must ensure that the array pointed to by 'base'
 *   is large enough to hold 'base', 'str', a possible additional path
 *   separator, and the null string terminating byte.
 *
 *   If 'base' is a drive specification (such as A: or C:), then 'str'
 *   is simply added to 'base':
 *
 *   a) If 'base' ends with a ":" , the 'str' is simply added to 'base'.
 *      In this case, 'base' is assumed to be a drive specification
 *      with no path information (as in A: or C:).
 *
 *   Otherwise, this subroutine ensures that there is one and only one path
 *   separator between 'base' and 'str' as follows:
 *
 *   b) If 'base' doesn't end with a "/" and 'str' doesn't begin with one,
 *      then a "/" is added to 'base' before 'str' is added to 'base'.
 *
 *   c) If 'base' ends with a "/" or 'str' begins with one (but not both),
 *      then 'str' is simply added to 'base'.
 *
 *   d) Otherwise, 'str' is incremented past its beginning "/" before being
 *      added to 'base'.
 *
 * Returns:
 *   Pointer to the 'base' pathname.
 *
 * Notes:
 *   The following table defines the bit pattern, value, and meaning of
 *   the 'action' variable during the operation of this subroutine
 *   (as long as 'base' is not a drive specification with no path):
 *
 *   Last char   First char     Bit
 *   of 'base'    of 'str'    Pattern   Value   Meaning of Value
 *   ---------   ----------   -------   -----   -------------------------
 *     not /       not /       0   0      0     Add path separator first
 *     not /         /         0   1      1     Just concatenate 'str'
 *       /         not /       1   0      2     Just concatenate 'str'
 *       /           /         1   1      3     First increment 'str'
 *============================================================================*/

#define ADD_SEP      0       /* Special actions */
#define REMOVE_SEP   3

char *pathcat(

char *base ,         /* Pointer to base pathname */
char *str  )         /* Pointer to string to add to the base pathname */

{
  int   action;      /* Action to be taken during concatenation */
  int   baselen;     /* Length of base string */

 /*---------------------------------------------------------------------------*
  * Take care of special cases
  *---------------------------------------------------------------------------*/

  baselen = strlen(base);       /* Store length of base string */

  if (baselen == 0)             /* If length is zero: */
  {
     strcpy(base, str);               /* Just copy 'str' to 'base' */
     return(base);                    /* Return pointer to 'base' */
  }

  if (base[baselen-1] == ':')   /* If last character of base is a colon: */
  {
     strcat(base, str);               /* Append 'str' to end of 'base' */
     return(base);                    /* Return pointer to 'base' */
  }

 /*---------------------------------------------------------------------------*
  * Append 'str' to 'base' with exactly one path separator between them
  *---------------------------------------------------------------------------*/

  action = 0;                   /* Set 'action' to proper value */
  if (base[baselen-1] == SEP)         /* If 'base' ends with '/' */
     action = action | 2;
  if (*str == SEP)                    /* If 'str' begins with '/' */
     action = action | 1;

  switch (action)               /* Depending upon the value of 'action': */
  {
     case ADD_SEP:                    /* Add path separator first */
        strcat(base, pathsep);
        strcat(base, str);
        break;

     case REMOVE_SEP:                 /* First increment 'str' past its path */
        str++;                        /* separator before adding to 'base' */
        strcat(base, str);
        break;

     default:                         /* Just concatenate 'str' */
        strcat(base, str);
        break;
  }

  return(base);                 /* Return pointer to base pathname */
}

/*============================================================================*
 * isdir() - Does a string represent a valid directory name?
 *
 * REMARKS:
 *   This subroutine checks to see if a string represents a valid directory
 *   name.
 *
 *   A null string represents the current directory and is a valid directory.
 *
 *   The stat() function call cannot be used for OS/2, since stat() won't
 *   support long filenames.
 *
 * RETURNS:
 *   FALSE, if it 'dirstr' is not a valid directory.
 *   TRUE, if it is.
 *============================================================================*/
int isdir(

char *dirstr )                      /* Pointer to a directory name string */

{
  int rc;                           /* Return code store area */
  struct stat statb;                /* stat() structure information buffer */
  int dlen;                         /* Length of dirstr string */
  char *enddirstr;                  /* Pointer to end of dirstr specification */

  FILESTATUS3 pinfo;                /* Path information structure */

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

 /*--------------------------------------*
  * For OS/2 and long filename support:
  *--------------------------------------*/

  rc = DosQueryPathInfo(dirstr,     /* Get path information for dirstr */
                    1,                   /* Level 1 information */
                    &pinfo,
                    sizeof(pinfo));

  if (rc != 0)                      /* If error: it's not a directory */
     return(FALSE);

  if ((pinfo.attrFile &             /* If its subdirectory attribute bit */
       _A_SUBDIR) == 0)             /* isn't set: it's not a directory */
     return(FALSE);

 /*--------------------------------------*
  * DOS and OS/2 8.3 filenames only:
  *--------------------------------------*/
 /*--------------------------------------*
  * rc = stat(dirstr, &statb);
  *
  * if (rc != 0)
  *    return(FALSE);
  *
  * if ((statb.st_mode & S_IFDIR) == 0)
  *    return(FALSE);
  *--------------------------------------*/

  return(TRUE);                     /* We passed all of the tests */
}

/*============================================================================*
 * hasdrive() - Does a string have a drive specification?
 *
 * REMARKS:
 *   This subroutine checks to see if a string begins with a letter followed
 *   by a colon.
 *
 * RETURNS:
 *   FALSE, if it has no drive specification.
 *   TRUE, if it does.
 *============================================================================*/
int hasdrive(

char *dirstr )                      /* Pointer to a directory name string */

{

  if ( (strlen(dirstr) >= 2) &&     /* If string has 2 or more characters */
       (dirstr[1] == ':') )         /* And the second character is a colon */
            return(TRUE);           /* Then it contains a drive spec */
  else
            return(FALSE);
}

/*============================================================================*
 * ishpfs() - Is the drive supported by an HPFS or NFS filesystem?
 *
 * REMARKS:
 *   This subroutine checks to see if the string represents an HPFS filesystem.
 *   If the string begins with a drive letter followed by a colon, then that
 *   drive is checked for having an HPFS filesystem.  Otherwise, the current
 *   drive is checked.
 *
 *   The purpose of this function is to allow programs to determine if the
 *   filesystem supports case-senstive names. HPFS always does, and NFS
 *   probably does (assuming that the NFS filesystem exists on an *ix
 *   machine).
 *
 * RETURNS:
 *   FALSE, if it not HPFS or remote NFS.
 *   TRUE, if it is.
 *
 * NOTES:
 *   At this time, only locally-attached filesystems can be checked for HPFS.
 *   Remotely-attached filesystems have a filesystem name of LAN whether or
 *   not they're HPFS.
 *
 *   The structure returned by the file system query function appears to have
 *   the remote name in a different place for 32-bit code than for 16-bit code.
 *   In addition, for remote-attached filesystems, 32-bit code gets the LAN
 *   or NFS strings stored as the remote name and not the filesystem name.
 *============================================================================*/
int ishpfs(

char *pathstr )                     /* Pointer to a pathname string */

{
  int     rc;                       /* Return code */
  ULONG   drivenum;                 /* Drive number */
  ULONG   drivemap;                 /* Drive map */
  char    drivestr[3];              /* String: "D:" */
  char   *fsname = "";              /* Pointer to filesystem name */
  char   *rname  = "";              /* Pointer to remote name */

  char    sbuff[128];               /* Character array buffer */
  ULONG   bufferlen = sizeof(sbuff);
  FSQBUFFER2 *buffer;               /* For DosQFSAttach() */
  USHORT *ubuff;                    /* Pointer to USHORT array */

 /*---------------------------------------------------------------------------*
  * Store the drive spec in drivestr[] as "D:"
  *---------------------------------------------------------------------------*/

  drivestr[1] = ':';                /* Store colon and null terminator for */
  drivestr[2] = '\0';               /* the drive spec string */

  if (hasdrive(pathstr))            /* If path string has a drive spec: */
  {
     drivestr[0] = pathstr[0];           /* Store its drive letter */
  }
  else                              /* Else: determine current drive's letter: */
  {
     rc = DosQueryCurrentDisk(&drivenum, &drivemap);
     drivestr[0] = 'A' + drivenum - 1;
  }

 /*---------------------------------------------------------------------------*
  * Point fsname to the filesystem name for the drive spec in drivestr[]
  *---------------------------------------------------------------------------*/

  rc = DosQFSAttach(drivestr,        /* Query FS name for this drive */
                0L,                  /* Ordinal is ignored */
                FSAIL_QUERYNAME,     /* Info level: query name */
                (FSQBUFFER2 *)sbuff, /* Pointer to info buffer */
                &bufferlen);         /* Size of buffer */

  if (rc == 0)                      /* If ok: */
  {
     ubuff = (USHORT *)sbuff;            /* Point ubuff to start of buffer */
     buffer = (FSQBUFFER2 *)sbuff;       /* Point buffer to FS info hdr */
     switch (buffer->iType)              /* Check iType of filesystem: */
     {
        case FSAT_LOCALDRV:                    /* usually FAT or HPFS */
           fsname = &sbuff[ 7 + ubuff[2] ];
           break;

        case FSAT_REMOTEDRV:                   /* usually LAN  or NFS */
           fsname = &sbuff[ 7 + ubuff[2] ];

           rname = &sbuff[ 7 + ubuff[2] ];
           rname += strlen(rname);
           rname += 1;

           break;
     }
  } /* end if (rc == 0)

 /*---------------------------------------------------------------------------*
  * Check for various local/remote filesystems that probably preserve case
  *---------------------------------------------------------------------------*/

  if (strcmpi(fsname, "HPFS") == 0) /* Check for various local filesystems */
     return(TRUE);

  if (strcmpi(fsname, "TVFS") == 0)
     return(TRUE);

  if (strcmpi(rname, "NFS") == 0)   /* Test for various remote filesystems */
     return(TRUE);

  if (strcmpi(rname, "TVFS") == 0)
     return(TRUE);

  return(FALSE);                    /* Not HPFS: FAT, LAN, or other */
}

/*============================================================================*
 * decompose() - Decompose '[d:path]name' into 'd:path' and 'name'.
 *
 * REMARKS:
 *   This subroutine scans the specified string.  It determines the pointers
 *   and lengths of the drive/path and the name portions of the string.  Of
 *   course, it assumes that the string represents a valid '[d:path\]name'.
 *
 * RETURNS:
 *   A pointer to a static PATH_NAME structure type.  The information in this
 *   structure is overwritten by each call to this subroutine.  Also, the
 *   pointers in this structure point to locations within the original string.
 *
 *   The structure contains a pointer and a length for each portion of the
 *   string.  The first pointer and length describe the d:path portion.
 *   A pointer to the d:path portion always points to the beginning of the
 *   string.  If the string has no d:path portion, then its length is zero.
 *
 *   The second pointer and length describe the name portion of the string.
 *============================================================================*/
PATH_NAME *decompose(

char *str )                         /* Pointer to a [d:path]name string */

{
  int rc;                           /* Return code store area */
  char ch;                          /* A character from the string */
  int slen;                         /* Length of string */
  char *endstr;                     /* Pointer to end of string */
  char *pstr;                       /* Pointer to string (generic) */

 /*---------------------------------------------------------------------------*
  * Get length of string and a pointer to the last character
  *---------------------------------------------------------------------------*/

  slen = strlen(str);               /* Get length */
  endstr = str + slen - 1;          /* Store pointer to the end */

 /*---------------------------------------------------------------------------*
  * Initialize PATH_NAME structure (in the static 'pn' variable)
  *---------------------------------------------------------------------------*/

  pn.path = str;                    /* Init. pointers to point to string */
  pn.name = str;

  pn.pathlen = 0;                   /* Init. path length to zero */
  pn.namelen = slen;                /* Assume whole string is the name */

  if (slen == 0)                    /* If string is zero length: */
     return(&pn);                        /* Then just return */

 /*---------------------------------------------------------------------------*
  * Starting at end of string, scan backwards:
  *---------------------------------------------------------------------------*/

  pstr = endstr;                    /* Point to end of string */
  for (;;)                          /* For each character: */
  {
     ch = *pstr;                         /* Store character */
     if ((ch == SEP) ||                  /* If we found a path separator or */
         (ch == ':'))                    /* a drive colon: */
     break;                                   /* We're done looping */

     if (pstr == str)                    /* If we're at the beginning of string: */
        break;                           /*      Then we're done looping */
     pstr--;                             /* Not done: point to previous char */
  }                                 /* End of loop for each character */

 /*---------------------------------------------------------------------------*
  * ch contains the character that terminated the loop, and pstr points to it
  *---------------------------------------------------------------------------*/

  if (pstr == str)                  /* If we're at the beginning of the string: */
  {
     if (ch != SEP)                      /* If 1st char is not a path separator: */
     {
        pn.name = str;                        /* Then the entire string is the name */
        pn.namelen = slen;                    /* portion (there is no d:path) */
     }
     else                                /* Otherwise: string is '\name': */
     {
        pn.pathlen = 1;                       /* Path portion is the path separator */
        pn.name = pstr+1;                     /* And the name portion is just past it */
        pn.namelen = slen-1;
     }
     return(&pn);                        /* Return */
  }

 /*---------------------------------------------------------------------------*
  * We stopped looping before we got to the beginning of the string
  *---------------------------------------------------------------------------*/

  switch (ch)                       /* Check the character that stopped the loop: */
  {
     case SEP:                           /* PATH SEPARATOR: */
        pn.pathlen = pstr - str;              /* Path portion is before path sep */
        pn.name = pstr + 1;                   /* Name portion is after path sep */
        pn.namelen = endstr - pstr;
        if (*(pstr-1) == ':')                 /* If drive colon precedes path sep: */
           pn.pathlen++;                           /* Then path sep is a part of d:path */
        break;

     case ':':                           /* DRIVE COLON: */
        pn.pathlen = pstr - str + 1;          /* Path portion includes colon */
        pn.name = pstr + 1;                   /* Name portion is after colon */
        pn.namelen = endstr - pstr;
        break;

     default:                            /* WE HAD BETTER NEVER GET HERE! */
        break;
  }

  return(&pn);                      /* Done */
}

/*============================================================================*
 * SetFileMode() - Sets the mode (attributes) for a file.
 *
 * REMARKS:
 *   This subroutine sets the mode bits for an existing file. It performs
 *   the function of the OS/2 1.3 toolkit's DosSetFileMode() subroutine.
 *
 * RETURNS:
 *   0, if successful.  Other, if error.
 *============================================================================*/

/* These are the only bits we're allowed to process */

#define ModeBits  (FILE_READONLY | \
                   FILE_HIDDEN   | \
                   FILE_SYSTEM   | \
                   FILE_ARCHIVED )

int SetFileMode(

char *pathname ,                    /* Pointer to file's name */
uint  fmode    )                    /* Mode to set */

{
  int         rc;
  ulong       curmode;
  ulong       level = 1;
  FILESTATUS3 fbuff;
  ulong       fbufflen = sizeof(FILESTATUS3);

  fmode = fmode & ModeBits;         /* Turn off bits we can't set */

  rc = DosQueryPathInfo(pathname,   /* Get current mode for file */
                        level,
                        &fbuff,
                        fbufflen);

  if (rc == 0)                      /* If ok: */
  {
     curmode = fbuff.attrFile;           /* Get current mode bits */
     curmode &= ~ModeBits;               /* Turn off the ones we can set */
     curmode |= fmode;                   /* Turn on bits caller wants */
     fbuff.attrFile = curmode;           /* Store new mode in structure */

     rc = DosSetPathInfo(pathname,       /* Set new mode for file */
                        level,
                        &fbuff,
                        fbufflen,
                        DSPI_WRTTHRU);
  }                                 /* Endif */

  return(rc);                       /* Return to caller */
}
