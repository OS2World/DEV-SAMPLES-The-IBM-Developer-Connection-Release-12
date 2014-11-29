/*============================================================================*
 * module: speclist.c - File specification list processing.   OS/2 Version
 *
 * (C)Copyright IBM Corporation, 1991, 1992               Brian E. Yoder
 *
 * A file specification is composed of '[d:][path]name'.  The first part
 * (drive and path) is optional and is known as the path.  The second part
 * is a file name.  This name can contain pattern-matching characters like
 * those that are supported by the AIX shell.
 *
 * This module contains subroutines to give a calling program a findfirst/
 * findnext-like capability with some major enhancements: support pattern-
 * matching characters as the AIX shell does, support more than one file
 * specification, and allow a program to traverse a single directory only
 * once no matter how many file specifications there are.
 *
 * The slmake() subroutine builds a "specification list" data structure
 * based on the file specification(s) given.  The slrewind() subroutine
 * gets your program ready to look for files that match the file
 * specifications.  The slmatch() subroutine works like an enhanced version
 * of DosFindNext() -- it gets the next file that matches the file
 * specifications stored in the specification list.  It is called repeatedly
 * to get all matching files.
 *
 * Once slmatch() has been used to find all matching filenames, slnotfound()
 * can be called to display on stderr a list of all file specifications that
 * did not match any files.
 *
 * Programs that use these subroutines should be linked with a module
 * definition file that contains a NEWFILES statement if they want to
 * match files on "non 8.3" file systems, such as HPFS.
 *
 * The FINFO label is #defined as FILEFINDBUF and is a shorter name to type.
 *
 * 04/05/91 - Created.
 * 04/19/91 - Initial version.
 * 04/30/91 - Ported to OS/2, using <os2.h> instead of <doscalls.h>.  I needed
 *            to make slmatch()'s 'attrib' var USHORT instead of char.  I also
 *            did not set the _A_VOLID bit, since OS/2 doesn't support it.
 * 05/06/91 - Added support for recursive directory descent by slmatch().
 * 05/09/91 - Added support for directory name expansion by slmake().
 * 04/13/92 - Set SLPATH's hpfs flag to TRUE for HPFS filesystems.
 * 07/22/92 - Convert function definitions to the new style.
 * 09/19/92 - Added support for the bpathlen field of SLPATH.
 * 11/11/92 - Call the 16-bit DosFindFirst/Next DOS16FIND* APIs.
 *            When scanning a directory on an HPFS drive, the 32-bit APIs
 *            skip over any name that begins with: ! # $ % ( ) - + ,
 *            This is needed with early releases of OS/2 2.0 only; 2.1 is fine.
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>

#include "util.h"                   /* Also includes <os2.h> */

#define  MAX_CPY 1024               /* Max length of strings, during strcpy() */
#define  MAX_LEN 2048               /* Max buffer lengths */

#define ELEN     1024               /* Length of expression buffers */

/*============================================================================*
 * Static data used by slmake()
 *============================================================================*/

static PATH_NAME *pn = NULL;        /* Pointer to path/name structure */

static char fullname[MAX_LEN+1];    /* Full d:path\name string */
static char path[MAX_LEN+1];        /* The d:path portion of a full filename */
static char name[MAX_LEN+1];        /* The name portion of a full filename */
static char ntmp[MAX_LEN+1];        /* A temp storage area for names */

/*----------------------------------------------------------------------------*
 * Variable set by slmake and used by findpath().  If set to TRUE, then we
 * want to create an SLPATH structure in the spec list only for each unique
 * path.  If FALSE, we want to create one for each path regardless.
 *----------------------------------------------------------------------------*/

static int  unique = TRUE;

/*----------------------------------------------------------------------------*
 * Static pointer to file spec error: set by slmake()
 *----------------------------------------------------------------------------*/

static char *error_spec = "";       /* Initialize to zero-length string */

/*----------------------------------------------------------------------------*
 * Data used to expand patterns and compile regular expressions
 *----------------------------------------------------------------------------*/

static char  eexpr[ELEN];           /* Full regular (expanded) expression */
static char  cexpr[ELEN];           /* Compiled expression */

/*----------------------------------------------------------------------------*
 * Pointers used by slmake() and the subroutines it calls.  They define the
 * beginning and end of the current singly-linked list of SLPATH structures.
 *----------------------------------------------------------------------------*/

static SLPATH *firstpath = NULL;    /* First/last paths in the singly-linked */
static SLPATH *lastpath = NULL;     /* list of SLPATH structures */

/*============================================================================*
 * Data used by slrewind() and slmatch() only!  The slmatch() subroutine
 * contains a state machine, and this data preserves state information
 * between calls.  No other programs should modify this information!
 *============================================================================*/

enum MATCH_STATES {

        MDone,                      /* Done: All filespecs have been checked */
        MFindFirst,                 /* Issue a DOS findfirst on current path */
        MFindNext,                  /* Issue a DOS findnext on current path */
        MCheckMatch                 /* Check current directory entry for match */

};

static int     MatchState = MDone;  /* Initial state is 'done', for safety */
static SLPATH *nextpath = NULL;     /* Pointer updated only by slmatch() */
static ushort  slflags;             /* Flags used to restrict slmatch() */
static ushort  sl_recurse = FALSE;  /* Recursively descend directories? */
static HDIR    findh = HDIR_CREATE; /* Handle returned by DosFindFirst() */
static FINFO   finfo;               /* File information structure */

/*============================================================================*
 * Prototypes for static functions used only within this module
 *============================================================================*/

static SLPATH    *addlist      ( char *, char *, char *, int );
static SLPATH    *addpath      ( char * );
static SLPATH    *insertpath   ( SLPATH *, char * );
static SLNAME    *addname      ( SLPATH *, char *, char *, int );
static SLPATH    *findpath     ( char * );

static void       dumppath     ( SLPATH * );
static void       dumpname     ( SLNAME * );

static int        matchit      ( FINFO *, ushort );

//===========================================================================*
//
// From: <bsedos.h> in 2.0 toolkit
//
//  APIRET APIENTRY  DosFindFirst(
//          PSZ    pszFileSpec,
//          PHDIR  phdir,
//          ULONG  flAttribute,
//          PVOID  pfindbuf,
//          ULONG  cbBuf,
//          PULONG pcFileNames,
//          ULONG  ulInfoLevel);   /* FIL_STANDARD */
//
//  APIRET APIENTRY  DosFindNext(
//          HDIR   hDir,
//          PVOID  pfindbuf,
//          ULONG  cbfindbuf,
//          PULONG pcFilenames);
//
//  APIRET APIENTRY  DosFindClose(HDIR hDir);
//
// From: <bsedos.h> in 1.3 toolkit
//
//  USHORT APIENTRY DosFindFirst(
//          PSZ pszFSpec,          /* path name of files to be found              */
//          PHDIR phdir,           /* directory handle                            */
//          USHORT usAttr,         /* attribute used to search for the files      */
//          PFILEFINDBUF pffb,     /* result buffer                               */
//          USHORT cbBuf,          /* length of result buffer                     */
//          PUSHORT pcSearch,      /* number of matching entries in result buffer */
//          ULONG ulReserved);     /* reserved (must be 0)                        */
//
//  USHORT APIENTRY DosFindNext(
//          HDIR hdir,                      /* directory handle          */
//          PFILEFINDBUF pffb,              /* result buffer             */
//          USHORT cbBuf,                   /* length of result buffer   */
//          PUSHORT pcSearch);              /* number of entries to find */
//
//  USHORT APIENTRY DosFindClose(
//          HDIR hdir);                     /* directory handle */
//===========================================================================*

/*============================================================================*
 * slmake() - Make specification list.
 *
 * REMARKS:
 *   The slmake() subroutine is used to process a list of file specifications
 *   and build a "specification list".
 *
 *   Once the specification list is built, you can use slrewind() and slmatch()
 *   to find matching files.  These subroutines are analogous to the
 *   dosfindfirst() and findnext() subroutines.  However, they work with the
 *   specification list and allow you to use pattern-matching instead of the
 *   "wildcard" metacharacters.
 *
 *   Point 'specv' to an array of file specification pointers and set 'specc'
 *   to the number of pointers in this array (like 'argv' and 'argc').  Point
 *   the 'pathnode' variable to a location in which a pointer to a
 *   specification list pointer can be stored.
 *
 *   If 'uniq' is TRUE, then the specification list will contain only one
 *   SLPATH structure for each unique path in the list of file specifications.
 *   If 'uniq' is FALSE, then there will be one SLPATH structure for each
 *   file specification.  This variable affects the order in which slmatch()
 *   finds files.  It is used to support the -g flag of the 'ls' command.
 *   See the ls.c file for more information.
 *
 *   If 'expand' is TRUE, then for each file specification that is the name
 *   of an existing directory, this subroutine will concatenate a path
 *   separator and a "*", so that slmatch() will match all files within that
 *   directory.  If 'expand' is FALSE, then directory names are left as-is,
 *   so slmatch() will match the only directory and not the files within it.
 *   In this case, however, you will an "invalid filespec" error for file
 *   specifications like "c:" and "\".
 *
 * NOTES ON FREEING MEMORY:
 *   This subroutine builds a specification list for all file specifications
 *   and stores them in a spec list.  The program uses malloc() to store the
 *   information in the list.  To free the memory, you only need to free the
 *   memory addresses by pointers to the SLNAME and SLPATH structures: the
 *   character strings for a structure are stored just past the structure
 *   (in the same malloc'd block as the structure).
 *
 *   Note that if an SLPATH structure has its 'inserted' member as TRUE, you
 *   should only free the storage associated with the SLPATH structure itself.
 *   Any SLNAME structures it points to are shared by an SLPATH structure
 *   whose 'inserted' member is FALSE.  See the insertpath() subroutine.
 *
 * RETURNS:
 *   0    : successful.  A pointer to the start of the specification list is
 *          copied to the location pointed to by *speclist.
 *
 *   Other: Cannot compile a name specification.  Use slerrspec() to get a
 *          pointer to the name specification that could not be compiled.
 *          The error code is one returned by either rexpand() or rcompile().
 *
 * FORMAT OF A SPECIFICATION LIST:
 *   The slmake() subroutine combines all file specifications in a "spec
 *   list".  This consists of a singly-linked list of SLPATH structures.
 *   Each SLPATH structure defines a 'd:path' and also points to a singly-
 *   linked list of SLNAME structures for that path.
 *
 *   For example, assume the following list of file specifications.  Note that
 *   the 'd:path' part of the first two specifications is a null string:
 *
 *       *.c   *.h   \lib\*.lib   \lib\*.obj   \include\*.h
 *
 *   There are 3 unique 'd:path' strings: "" (null), "\lib\", and "\include\".
 *   Therefore, our specification list would look conceptually like this
 *   (if 'uniq' is TRUE):
 *
 *       "" (null) ----------------------------------> "*.c"
 *           |                                           |
 *       "\lib" --------------------> ".lib"           "*.h"
 *           |                           |
 *       "\include" ---> "*.h"        "*.obj"
 *
 *   The slmake() subroutine can be used to build this list.  For this
 *   example, it stores the 3 'd:path' strings in a list of SLPATH structures.
 *   It stores the lists of name specifications in 3 lists of SLNAME structures.
 *
 *   Each SLNAME structure contains a pointer to the name string (i.e. "*.c").
 *   The slmake() subroutine also expands the name string into a regular
 *   expression using rexpand() and compiles it using rcompile().  It stores
 *   the compiled expression in the SLNAME structure -- you can use rmatch()
 *   to compare it with a filename.
 *
 *   Each SLNAME structure also contains a count field.  Your program can use
 *   this field to see how many files matched a given '[d:path]name'.
 *============================================================================*/

int slmake(

SLPATH **speclist ,                 /* Pointer to spec list pointer */
int   uniq        ,                 /* Build an SLPATH only for unique path? */
int   expand      ,                 /* Expand directory names? */
int   specc       ,                 /* No. of file specifications */
char **specv      )                 /* Pointer to array of file specifications */

{
  int rc;                           /* Storage for return codes */

  SLPATH *slp;                      /* Pointer to path info */
  SLNAME *sln;                      /* Pointer to name info */

  char *next_eexpr;                 /* Pointer to next expanded expression */
  char *next_cexpr;                 /* Pointer to next compiled expression */
  int   cexpr_len;                  /* Length of compiled expression */

  firstpath = NULL;                 /* Initialize linked list to 'empty' */
  lastpath = NULL;

  unique = uniq;                    /* Set the static 'unique' variable */

 /*---------------------------------------------------------------------------*
  * Loop for each file specification
  *---------------------------------------------------------------------------*/

  while (specc != 0)                /* For each filespec: */
  {
    /*-----------------------------------------------------*
     * Decopmose each filespec into path and name portions
     *-----------------------------------------------------*/

     strncpy(fullname, *specv, MAX_CPY); /* Copy filespec into buffer */
     if (isdir(fullname) == TRUE)        /* If fullname is a directory: */
        if (expand)                           /* If we're supposed to expand it: */
           pathcat(fullname, "*");                 /* Append "*" to it */

     pn = decompose(fullname);           /* Now: decompose the fullname */

     memcpy(path, pn->path,              /* Store path portion and terminate it */
                  pn->pathlen);
     *(path+pn->pathlen) = '\0';

     memcpy(name, pn->name,              /* Store name portion and terminate it */
                  pn->namelen);
     *(name+pn->namelen) = '\0';

    /*-----------------------------------------------------*
     * Expand name portion into a regular expression and
     * store in 'exxpr[]'
     *-----------------------------------------------------*/

     rc = rexpand(name, eexpr, &eexpr[ELEN+1], &next_eexpr);
     if (rc != 0)                        /* If error: store pointer to */
     {                                   /* original file spec and return */
        error_spec = *specv;
       *speclist = firstpath;                 /* Also: copy pointer to list */
        return(rc);
     }

    /*-----------------------------------------------------*
     * Compile 'eexpr' and store in 'cexpr[]'
     *-----------------------------------------------------*/

     strupr(eexpr);                      /* Convert expanded expr to uppercase */

     rc = rcompile(eexpr, cexpr, &cexpr[ELEN+1], '\0', &next_cexpr);
     if (rc != 0)                        /* If error: store pointer to */
     {                                   /* original file spec and return */
        error_spec = *specv;
       *speclist = firstpath;                 /* Also: copy pointer to list */
        return(rc);
     }

     cexpr_len = next_cexpr - cexpr;     /* Store length */

    /*-----------------------------------------------------*
     * Add path/name to the specification list
     *-----------------------------------------------------*/

     slp = addlist(path, name, cexpr, cexpr_len);
     if (slp == NULL)
        return(REGX_MEMORY);

     specc--;                            /* Decrement array counter, and */
     specv++;                            /* Point to next filespec pointer */
  }

  *speclist = firstpath;            /* Copy pointer to start of list */
  return(0);                        /* Return */
}

/*============================================================================*
 * slerrspec() - Get error file specification.
 *
 * REMARKS:
 *   Returns a pointer to the original file specification that caused the
 *   last error (if any) encountered by a call to mklist().
 *
 * RETURNS:
 *   A pointer to the file specification.
 *============================================================================*/

char *slerrspec()
{
  return(error_spec);               /* Return pointer */
}

/*============================================================================*
 * sldump() - Dumps a specification list.
 *
 * REMARKS:
 *   You should pass to this subroutine a pointer to the first SLPATH structure
 *   in the specification linked list.  This pointer is the one that is copied
 *   to your program by the slmake() subroutine.
 *
 *   This subroutine dumps each data structure in the specification list on
 *   stdout.
 *
 * RETURNS:
 *   Nothing.
 *============================================================================*/

void  sldump(

SLPATH *pdata )                     /* Pointer to specification list */

{
  SLNAME *ndata;                    /* Pointer to a name data structure */

  while (pdata != NULL)             /* For each path structure: */
  {
     dumppath(pdata);                    /* Dump the path structure */
     ndata = pdata->firstname;           /* Point to first name structure */
     while (ndata != NULL)               /* And for each name structure: */
     {
        dumpname(ndata);                      /* Dump it */
        ndata = ndata->next;                  /* Go to next name structure */
     }                                   /* Done with path's name structures */
     pdata = pdata->next;                /* Go to next path structure */
  }                                 /* Done with path structures */

  return;
}

/*============================================================================*
 * slrewind() - Goes to beginning of specification list.
 *
 * REMARKS:
 *   This subroutine initializes the data structures and state information used
 *   by the slmatch() subroutine.  You should pass to this subroutine the
 *   pointer returned by the slmake() subroutine.
 *
 *   The 'mflags' restrict the names that slmatch() will attempt to match.
 *   Each bit represents a set of files (normal, hidden, system, directory,
 *   etc.) that slmatch() will process.  See the SL_ #defines in util.h for
 *   a list of possible flags.  You may inclusive OR as many of these flags
 *   as desired.
 *
 *   The 'recurse' flag tells slmatch() whether or not to recursively descend
 *   subdirectories while attempting to match a file specification.
 *
 *   After calling this subroutine, you can make repeated calls to slmatch()
 *   to find all files that match each path and name combination within the
 *   specification list.
 *
 * RETURNS:
 *   Nothing.
 *============================================================================*/

void  slrewind(

SLPATH *pdata   ,                   /* Pointer to specification list */
ushort  mflags  ,                   /* Flags used to restrict matches */
ushort  recurse )                   /* Should slmatch() recursively descend directories? */

{
  nextpath = pdata;                 /* Store pointer */
  MatchState = MFindFirst;          /* Initialize state machine */
  slflags = mflags;                 /* Store flags */
  sl_recurse = recurse;             /* Store recurse indication */

  if (findh != HDIR_CREATE)         /* If a DosFindFirst handle exists: */
  {
//   DosFindClose(findh);                /* Close it */
     DOS16FINDCLOSE(findh);              /* Close it */
     findh = HDIR_CREATE;                /* And reset it */
  }

  return;
}

/*============================================================================*
 * slmatch() - Gets next matching directory entry.
 *
 * REMARKS:
 *   This subroutine finds the next directory entry that matches the file
 *   specifications in the specification list.
 *
 *   A program normally will call slmake() to build a specification list.
 *   If there are no errors, it calls slrewind().  Then it makes repeated
 *   calls to slmatch() to find all directory entries that match the file
 *   specs in the specification list.
 *
 * RETURNS:
 *   NULL, if there are no more matching directory entries.
 *
 *   Otherwise, this subroutine returns a pointer to a static FINFO
 *   structure that describes the matching directory entry.  It
 *   also copies to your program two pointers to static data structures:
 *   one to the SLPATH data structure and one to the SLNAME data structure
 *   for the entry.
 *
 * NOTES:
 *   The full pathname of the file can be found by concatenating the
 *   pathname found in the SLPATH structure with the filename found
 *   in the FINFO structure.  Use the pathcat() subroutine to do this,
 *   since it adds a path separator when necessary.
 *
 *   The FINFO, SLPATH, and SLNAME structures are for your reference only:
 *   Do not modify these structures for any reason, or this subroutine may
 *   not perform as expected.
 *
 * INTERNAL NOTES:
 *   If we told slrewind() that we want to recursively descend directories
 *   to look for names, then:  When slmatch() finds a subdirectory, it
 *   builds a new SLPATH structure for it and inserts it just after the
 *   current directory's SLPATH structure.  It also points the new SLPATH
 *   to the list of SLNAMES for the current SLPATH.  This not only saves
 *   some memory, but helps out the slpnotfound() subroutine:  If we don't
 *   find a matching name in the current directory but find it sometime
 *   during the directory decent (while processing the new SLPATH), then
 *   we will mark the increment the SLNAME's fcount, indicating that we
 *   indeed matched the filespec somewhere in the directory tree.
 *============================================================================*/

FINFO *slmatch(

SLPATH  **pathdata ,                /* Ptrs to where to to copy ptrs to the */
SLNAME  **namedata )                /* path and name data structures */

{
  int     rc;                       /* Return code storage */
  int     found_match;              /* Did we find a match? */

  ULONG   scnt;                     /* Count (was USHORT for 1.3) */
  USHORT  scnt16;

  FTM     ftm;                      /* File time structure */
  ULONG   attrib;                   /* Attribute (was USHOT for 1.3) */
  SLNAME *pname;                    /* Pointer to name data structure */
  SLNAME *fndname;                  /* Pointer to another name data structure */
  SLPATH *p_slpath;                 /* Pointer to path info structure */

  *pathdata = NULL;                 /* Initially: copy NULL pointers back */
  *namedata = NULL;                 /* to the calling program */

  attrib = 0L;                      /* Set attribute to zero, then: */
  attrib = _A_NORMAL  |             /* Set up attribute for findfirst */
           _A_RDONLY  |             /* These labels are from <dos.h> */
           _A_HIDDEN  |
           _A_SYSTEM  |
        /* _A_VOLID   |  */               /* OS/2 doesn't support _A_VOLID */
           _A_SUBDIR  |
           _A_ARCH;

  if (nextpath == NULL)             /* For safety: If we have a NULL current */
     return(NULL);                  /* path pointer: Don't bother! */

 /*---------------------------------------------------------------------------*
  * This state machine loops until it either finds a matching file or comes
  * to the end (and can find no more matching files).  When either of these
  * events occur, the state machine issues a return() from the subroutine.
  *---------------------------------------------------------------------------*/

  for (;;)                          /* State machine loop */
  {
     switch (MatchState)
     {
       /*-----------------------------------------------*
        * STATE: Done: All filespecs have been checked
        *-----------------------------------------------*/
        case MDone:
           if (findh != HDIR_CREATE) /* If a DosFindFirst handle exists: */
           {
/////         DosFindClose(findh);       /* Close it */
              DOS16FINDCLOSE(findh);     /* Close it */
              findh = HDIR_CREATE;       /* And reset it */
           }
           return(NULL);            /* Return, and stay in same state */
           break;

       /*-----------------------------------------------*
        * STATE: Issue a DOS findfirst on current path
        *-----------------------------------------------*/
        case MFindFirst:
           if (nextpath == NULL)    /* If current path pointer is NULL: */
           {
              MatchState = MDone;        /* Then we're done */
              return(NULL);              /* Return */
           }

           strncpy(fullname,        /* Build "path\*.*" string in fullname[] */
                   nextpath->path,  /* For the DOS findfirst command */
                   MAX_CPY);
           pathcat(fullname, "*.*");

//.............................................................................
//         rc = findfirst(fullname, /* Find first DOS directory entry for path */
//                        attrib, &finfo);
//.............................................................................

           if (findh != HDIR_CREATE) /* If a DosFindFirst handle exists: */
           {
/////         DosFindClose(findh);       /* Close it */
              DOS16FINDCLOSE(findh);     /* Close it */
              findh = HDIR_CREATE;       /* And reset it */
           }
           scnt = 1;                /* Find one at a time */
           scnt16=1;
////       rc = DosFindFirst(fullname,
////               &findh, attrib,
////               &finfo, sizeof(FINFO),
////               &scnt,
////               (ULONG)FIL_STANDARD);  /* Was 0L for 1.3 */
           rc = DOS16FINDFIRST(fullname,
                   &findh, (USHORT)attrib,
                   &finfo, sizeof(FINFO),
                   &scnt16, 0L);
           scnt=(ULONG)scnt16;

           if ((rc != 0) || (scnt == 0)) /* If findfirst failed: */
           {
              nextpath = nextpath->next; /* Go to next path in list */
              MatchState = MFindFirst;   /* And repeat the findfirst state */
           }
           else                     /* If findfirst was successful: */
           {
              MatchState = MCheckMatch;  /* Check to see if we have a match */
           }
           break;

       /*-----------------------------------------------*
        * STATE: Issue a DOS findnext on current path
        *-----------------------------------------------*/
        case MFindNext:
//.............................................................................
//         rc = findnext(&finfo);   /* Find next DOS directory entry for path */
//.............................................................................

           scnt = 1;                /* Find one at a time */
           scnt16=1;
////       rc = DosFindNext(findh,
////               &finfo, sizeof(FINFO),
////               &scnt);
           rc = DOS16FINDNEXT(findh,
                   &finfo, sizeof(FINFO),
                   &scnt16);
           scnt=(ULONG)scnt16;

           if ((rc != 0) || (scnt == 0)) /* If findnext failed: */
           {
              nextpath = nextpath->next; /* Go to next path in list */
              MatchState = MFindFirst;   /* And repeat the findfirst state */
           }
           else                     /* If findnext was successful: */
           {
              MatchState = MCheckMatch;  /* Check to see if we have a match */
           }
           break;

       /*-----------------------------------------------*
        * STATE: Check current directory entry for match
        *-----------------------------------------------*
        * We compare every SLNAME that is linked to the
        * current path.  Therefore, when we're done with
        * the spec list, we will know exactly which
        * name specs matched no files.
        *-----------------------------------------------*/
        case MCheckMatch:
           found_match = FALSE;     /* We haven't found a match yet */

           if (sl_recurse)          /* If we're to recursively search subdirs: */
           {
              if ( ((finfo.attrFile      /* If this entry is a subdirectory, */
                    & _A_SUBDIR) != 0) &&     /* But not . or .. : */
                    (strcmp(finfo.achName, ".")  != 0) &&
                    (strcmp(finfo.achName, "..") != 0))
              {
                 strncpy(ntmp,                /* Build path + name of subdir */
                         nextpath->path,      /* by copying path string and */
                         MAX_CPY);            /* adding the subdir name */
                 pathcat(ntmp, finfo.achName);
                 p_slpath = insertpath(       /* Insert subdir's new path */
                         nextpath, ntmp);     /* structure in list */

                 if (p_slpath != NULL)        /* If we inserted the path ok: */
                 {                            /* Then set its list of names */
                    p_slpath->firstname = nextpath->firstname; /* to those of */
                    p_slpath->lastname = nextpath->lastname;   /* the current path */
                 }
                 else
                 {
                    /* For now: ignore malloc error from inserting new path */
                    /* Just keep returning as many matching names as we can */
                 }
              }                          /* End of block to process directory */
           }                        /* End of block to support recursive descent */

           if (matchit(&finfo, slflags))
           {                        /* If we want to try and match the file: */
              fndname = NULL;          /* Set up loop, then: */
              pname = nextpath->firstname;
              while (pname != NULL)    /* Loop to check each name spec for path: */
              {
                 strncpy(ntmp,              /* Copy name into temp buffer, and */
                         finfo.achName,     /* convert name to uppercase (for */
                         MAX_CPY);          /* case-insensitive comparison) */
                 strupr(ntmp);

                 rc = rmatch(pname->ecname, /* Compare reg expression with name */
                             ntmp);         /* in directory entry */
                 if (rc == RMATCH)          /* If we have a match: */
                 {
                    found_match = TRUE;          /* Remember that we found match */
                    pname->fcount++;             /* Bump match counter for the name */
                    if (fndname == NULL)         /* Store only the first SLNAME */
                       fndname = pname;          /* structure that matched */
                 }
                 pname = pname->next;       /* No match: try next name spec */
              } /* end of while loop to check each name spec */
           } /* end of if (matchit) */

           MatchState = MFindNext;      /* Set next state */
           if (found_match == TRUE)     /* If we found a match: */
           {
              *pathdata = nextpath;        /* Copy path and name pointers */
              *namedata = fndname;         /* to calling program */
              return(&finfo);              /* Return to caller */
           }

           break;

       /*-----------------------------------------------*
        * STATE: Unknown.  Return as if done!
        *-----------------------------------------------*/
        default:
           return(NULL);
           break;
     }
  } /* end of for loop for the state machine */

  return(NULL);                     /* We'll never get here! */
}

/*============================================================================*
 * slnotfound() - Prints fspecs that were not found.
 *
 * REMARKS:
 *   You should pass to this subroutine a pointer to the first SLPATH structure
 *   in the specification linked list.  This pointer is the one that is copied
 *   back by the slmake() subroutine.
 *
 *   This subroutine should be called after the slmatch() subroutine has found
 *   every file that matches those described by the specification list.
 *
 *   This subroutine displays on stderr each path and name that wasn't found
 *   by the slmatch() subroutine
 *
 * RETURNS:
 *   The number of names that were not found.
 *============================================================================*/

long slnotfound(

SLPATH *pdata )                     /* Pointer to specification list */

{
  SLNAME *ndata;                    /* Pointer to a name data structure */
  long    cnt;                      /* Counter */

  cnt = 0L;                         /* Initialize counter */

  while (pdata != NULL)             /* For each path structure: */
  {
     if (pdata->inserted == FALSE)       /* If this entry wasn't inserted by */
     {                                   /* a recursive directory descent: */
        ndata = pdata->firstname;             /* Point to first name structure */
        while (ndata != NULL)                 /* And for each name structure: */
        {
           if (ndata->fcount == 0L)                /* If name's match count is zero: */
           {
              strcpy(ntmp, pdata->path);              /* No files matched: display */
              pathcat(ntmp, ndata->name);             /* path and name that didn't */
              fprintf(stderr, "%s not found\n", ntmp);   /* match */
              cnt++;                                  /* And bump the counter */
           }
           ndata = ndata->next;                    /* Go to next name structure */
        }                                   /* Done with path's name structures */
     }
     pdata = pdata->next;                /* Go to next path structure */
  }                                 /* Done with path structures */

  return(cnt);                      /* Return count of names not found */
}

/*============================================================================*
 * addlist() - Add a path and name to the specification list.
 *
 * REMARKS:
 *   If an SLPATH structure already exists for the specified 'newpath', then
 *   the 'newname' is added to the existing structure.  Otherwise, a new
 *   SLPATH structure is created for the new path and the new name is added
 *   to it.
 *
 *   If the static 'unique' variable is FALSE, then a new SLPATH structure
 *   is created anyway!
 *
 * RETURNS:
 *   A pointer to the SLPATH structure for the path.  If we run out of
 *   memory, a NULL pointer is returned.
 *============================================================================*/

static SLPATH *addlist(

char   *newpath ,                   /* New path's string */
char   *newname ,                   /* New path's name string */
char   *newregx ,                   /* Pointer to (and length of) the */
int     newlen  )                   /* compiled regular expression for name */

{
  SLPATH *p_slpath;                 /* Pointer to path info structure */
  SLNAME *p_slname;                 /* Pointer to name info structure */

  p_slpath = findpath(newpath);     /* See if path entry already exists */
  if (p_slpath == NULL)             /* If path doesn't exist: */
  {
     p_slpath = addpath(newpath);        /* Add new path structure */
     if (p_slpath == NULL)               /* If we ran out of memory: */
        return(NULL);                         /* Return */
  }
  p_slname = addname(p_slpath,      /* Add name to list of names for this path */
                     newname,
                     newregx, newlen);

  if (p_slname == NULL)             /* If we ran out of memory: */
     return(NULL);                       /* Return w/error indication */

  return(p_slpath);                 /* Return pointer */
}

/*============================================================================*
 * addpath() - Adds a new path data structure.
 *
 * REMARKS:
 *   This subroutine adds a new SLPATH structure to the end of the current
 *   linked list of such structures.  It allocates a block of memory large
 *   enough to hold both the path structure and the path string.
 *
 * RETURNS:
 *   A pointer to the new path structure, or NULL if there isn't enough memory.
 *============================================================================*/

static SLPATH *addpath(

char   *newpath )                   /* New path's name */

{
  int     totallen;                 /* Total length needed for memory block */
  char   *memblock;                 /* Pointer to allocated memory */

  SLPATH *p_slpath;                 /* Pointer to path info structure */
  char   *p_path;                   /* Pointers to strings */

 /*---------------------------------------------------------------------------*
  * Allocate a single block of memory for data structure and all strings
  *---------------------------------------------------------------------------*/

  totallen = sizeof(SLPATH) +       /* Calculate total size of memory block */
             strlen(newpath) + 1;   /* needed to hold structure and its strings */

  memblock = malloc(totallen);      /* Allocate the memory */
  if (memblock == NULL)
     return(NULL);

  p_slpath = (SLPATH *)memblock;    /* Store pointer to block and to where */
  p_path = memblock + sizeof(SLPATH);    /* string will be put */

  strcpy(p_path, newpath);          /* Store string in the block */

  p_slpath->next = NULL;            /* Initialize structure */
  p_slpath->firstname = NULL;
  p_slpath->lastname = NULL;
  p_slpath->path = p_path;
  p_slpath->bpathlen = strlen(p_path);

  p_slpath->hpfs = ishpfs(p_path);  /* Set whether or not it's HPFS */

  p_slpath->inserted = FALSE;       /* Mark as having not been inserted */

 /*---------------------------------------------------------------------------*
  * Add the new path structure to the linked list
  *---------------------------------------------------------------------------*/

  if (firstpath == NULL)            /* If list is empty: */
     firstpath = p_slpath;               /* New structure is now first on list */

  if (lastpath != NULL)             /* If end-of-list ptr is not NULL: */
     lastpath->next = p_slpath;     /* Point old end structure to new one */

  lastpath = p_slpath;              /* New structure is now last on list */

  return(p_slpath);                 /* Return pointer to new structure */
}

/*============================================================================*
 * insertpath() - Inserts a new path data structure.
 *
 * REMARKS:
 *   This subroutine inserts a new SLPATH structure after the current SLPATH
 *   structure but before the next (if any) that is linked to it now.
 *
 *   The new path information structure is marked as having been inserted.
 *   This will allow slpnotfound() to skip over paths that have been inserted
 *   as a result of a recursive subdirectory search.
 *
 * RETURNS:
 *   A pointer to the new path structure, or NULL if there isn't enough memory.
 *============================================================================*/

static SLPATH *insertpath(

SLPATH *p_curpath ,                 /* Current path's SLPATH data */
char   *newpath   )                 /* New path's name */

{
  int     totallen;                 /* Total length needed for memory block */
  char   *memblock;                 /* Pointer to allocated memory */

  SLPATH *p_slpath;                 /* Pointer to new path info structure */
  char   *p_path;                   /* Pointers to strings */

  SLPATH *p_nextpath;               /* Pointer to current next path, if any */

 /*---------------------------------------------------------------------------*
  * Allocate a single block of memory for data structure and all strings
  *---------------------------------------------------------------------------*/

  totallen = sizeof(SLPATH) +       /* Calculate total size of memory block */
             strlen(newpath) + 1;   /* needed to hold structure and its strings */

  memblock = malloc(totallen);      /* Allocate the memory */
  if (memblock == NULL)
     return(NULL);

  p_slpath = (SLPATH *)memblock;    /* Store pointer to block and to where */
  p_path = memblock + sizeof(SLPATH);    /* string will be put */

  strcpy(p_path, newpath);          /* Store string in the block */

  p_slpath->next = NULL;            /* Initialize structure */
  p_slpath->firstname = NULL;
  p_slpath->lastname = NULL;
  p_slpath->path = p_path;

  p_slpath->bpathlen = p_curpath->bpathlen;

  p_slpath->hpfs = ishpfs(p_path);  /* Set whether or not it's HPFS */

  p_slpath->inserted = TRUE;        /* Mark as having been inserted */

 /*---------------------------------------------------------------------------*
  * Insert the new path structure between the current and next (if any).
  *---------------------------------------------------------------------------*/

  if (p_curpath->next == NULL)      /* If the current path is the last: */
  {
     p_curpath->next = p_slpath;         /* Then link to current path, */
     lastpath = p_slpath;                /* And point end-of-list to it */
  }
  else                              /* Else: current path is not at the end: */
  {                                 /* Insert new path between current and next paths: */
     p_slpath->next = p_curpath->next;   /* Link new path to the next path */
     p_curpath->next = p_slpath;         /* Link current path to new path */
  }

  return(p_slpath);                 /* Return pointer to new structure */
}

/*============================================================================*
 * addname() - Adds a new name data structure.
 *
 * REMARKS:
 *   This subroutine adds a new SLNAME structure to the list of SLNAME
 *   structures for the 'pdata' path structure.  It allocates a block of
 *   memory large enough to hold both the name structure and its strings.
 *
 * RETURNS:
 *   A pointer to the new name structure, or NULL if there isn't enough memory.
 *============================================================================*/

static SLNAME *addname(

SLPATH *pdata    ,                  /* Pointer to path data structure */
char   *newname  ,                  /* Pointer to name string */
char   *newregx  ,                  /* Pointer to (and length of) the */
int     newlen   )                  /* compiled regular expression for name */

{
  int     totallen;                 /* Total length needed for memory block */
  char   *memblock;                 /* Pointer to allocated memory */

  SLNAME *p_slname;                 /* Pointer to name info structure */
  char   *p_name;                   /* Pointers to strings */
  char   *p_regx;

 /*---------------------------------------------------------------------------*
  * Allocate a single block of memory for data structure and all strings
  *---------------------------------------------------------------------------*/

  totallen = sizeof(SLNAME) +       /* Calculate total size of memory block */
             strlen(newname) + 1 +  /* needed to hold structure and its strings */
             newlen;

  memblock = malloc(totallen);      /* Allocate the memory */
  if (memblock == NULL)
     return(NULL);

  p_slname = (SLNAME *)memblock;    /* Store pointer to block and to where */
  p_name = memblock + sizeof(SLNAME);    /* strings will be put */
  p_regx = p_name + strlen(newname) + 1;

  strcpy(p_name, newname);          /* Store strings in the block */
  memcpy(p_regx, newregx, newlen);

  p_slname->next = NULL;            /* Initialize structure */
  p_slname->name = p_name;
  p_slname->ecname = p_regx;
  p_slname->eclen = newlen;
  p_slname->fcount = 0L;

 /*---------------------------------------------------------------------------*
  * Add the new name structure to the linked list for the specified path
  *---------------------------------------------------------------------------*/

  if (pdata->firstname == NULL)     /* If list is empty: */
     pdata->firstname = p_slname;        /* New structure is now first on list */

  if (pdata->lastname != NULL)      /* If end-of-list ptr is not NULL: */
     (pdata->lastname)->next = p_slname;   /* Point old end structure to new one */

  pdata->lastname = p_slname;       /* New structure is now last on list */

  return(p_slname);                 /* Return pointer */
}

/*============================================================================*
 * findpath() - Finds path structure.
 *
 * REMARKS:
 *   If the static 'unique' variable is FALSE, then a NULL pointer is returned
 *   always: in this case, we want to a new SLPATH structure for each path.
 *
 *   Otherwise, this subroutine looks through the list of SLPATH structures
 *   for a structure whose path name matches 'pathstr'.  It does a case-
 *   insensitive comparison on path strings.
 *
 * RETURNS:
 *   A pointer to the SLPATH structure, or NULL if there is none.
 *============================================================================*/

static SLPATH *findpath(

char *pathstr )                     /* Pointer to path string */

{
  SLPATH *pdata;                    /* Pointer to path data structure */

  if (unique == FALSE)              /* If we want to create new SLPATH for each */
     return(NULL);                  /* path, then always return 'not found' */

  pdata = firstpath;                /* Point to first structure in list */
  while (pdata != NULL)             /* Until we reach the end of the list: */
  {
     if (strcmpi(pdata->path,            /* Compare path strings: */
                pathstr) == 0)           /* If we found one with same 'path': */
        return(pdata);                   /* Then return pointer to structure */

      pdata = pdata->next;               /* No match: go on to next structure */
  }

  return(NULL);                     /* We didn't find it */
}

/*============================================================================*
 * dumppath() - Dumps contents of an SLPATH structure.
 *============================================================================*/

static void dumppath(

SLPATH *pdata )                     /* Pointer to data structure to dump */

{
  printf("Path data structure at 0x%08lX:\n", (long)pdata);
  printf("   Next path structure is at:      0x%08lX\n", (long)(pdata->next));
  printf("   List of name structures: first: 0x%08lX\n", (long)(pdata->firstname));
  printf("   List of name structures: last:  0x%08lX\n", (long)(pdata->lastname));
  if (pdata->path != NULL)
     printf("   Path string:                    '%s'\n", pdata->path);
  else
     printf("   Path string:                    None.\n");

  return;
}

/*============================================================================*
 * dumpname() - Dumps contents of an SLNAME structure.
 *============================================================================*/

static void dumpname(

SLNAME *ndata )                     /* Pointer to data structure to dump */

{
  printf("     Name data structure at 0x%08lX:\n", (long)ndata);
  printf("        Next name structure is at:      0x%08lX\n", (long)(ndata->next));
  if (ndata->name != NULL)
     printf("        Name string:                    '%s'\n", ndata->name);
  else
     printf("        Name string:                    None.\n");
  printf("        Address of compiled expression: 0x%08lX\n", (long)(ndata->ecname));
  printf("        Length of compiled expression:  %d\n", ndata->eclen);
  printf("        No. of filename matches:        %ld\n", ndata->fcount);

  return;
}

/*============================================================================*
 * matchit() - Should we show the file?
 *
 * REMARKS:
 *   This subroutine checks the file entry's attribute with the flags
 *   and determines whether or not we should match this file entry.
 *
 *   The 'mflags' argument tells us what we're looking for.
 *
 * RETURNS:
 *   TRUE: Match the file, or
 *   FALSE: Don't match the file: just skip it.
 *
 * NOTES:
 *   The bits in the FINFO structure's attribute should be checked using
 *   the _A_ #defined labels from <dos.h>.
 *
 *   The bits in the mflags variable should be checked using the SL_
 *   labels from util.h.
 *
 *   Any confusion in this area will result in garbage output!
 *============================================================================*/

static int matchit(

FINFO   *fdata  ,                   /* Pointer to file entry information */
ushort   mflags )                   /* Match flags */

{
 /*---------------------------------------------------------------------------*
  * Check for normal file
  *---------------------------------------------------------------------------*/

  if ((mflags & SL_NORMAL) != 0)    /* If we're looking for a normal file: */
     if ((fdata->attrFile & (_A_HIDDEN |   /* If non of these attributes are set: */
                             _A_SYSTEM |
                             _A_SUBDIR |
                             _A_VOLID)) == 0)
        return(TRUE);                         /* Then we found a normal file */

 /*---------------------------------------------------------------------------*
  * Check for system, hidden, and volid entries
  *---------------------------------------------------------------------------*/

  if ((mflags & SL_SYSTEM) != 0)
     if ((fdata->attrFile & _A_SYSTEM) != 0)
        return(TRUE);

  if ((mflags & SL_HIDDEN) != 0)
     if ((fdata->attrFile & _A_HIDDEN) != 0)
        return(TRUE);

  if ((mflags & SL_VOLID) != 0)
     if ((fdata->attrFile & _A_VOLID) != 0)
        return(TRUE);

 /*---------------------------------------------------------------------------*
  * Check for directory entries
  *---------------------------------------------------------------------------*/

  if ((mflags & SL_DIR) != 0)       /* If we're looking for a directory: */
  {
     if ((fdata->attrFile & _A_SUBDIR) != 0)  /* And the file entry is a directory: */
     {
        if ((strcmp(fdata->achName, ".")  != 0) &&   /* And its name isn't . or .. */
            (strcmp(fdata->achName, "..") != 0))
        return(TRUE);                                       /* Ok */
     }
  }

  if ((mflags & SL_DOTDIR) != 0)    /* If we're looking for a dotted directory: */
  {
     if ((fdata->attrFile & _A_SUBDIR) != 0)  /* And the file entry is a directory: */
     {
        if ((strcmp(fdata->achName, ".")  == 0) ||    /* And its name is . or .. */
            (strcmp(fdata->achName, "..") == 0))
        return(TRUE);                                       /* Ok */
     }
  }

 /*---------------------------------------------------------------------------*
  * It doesn't pass any of our checks: Don't attempt to match the file
  *---------------------------------------------------------------------------*/

  return(FALSE);
}
