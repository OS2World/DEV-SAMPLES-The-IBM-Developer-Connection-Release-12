/*============================================================================*
 * util.h - Definitions for functions in util.lib      OS/2 2.0 and C Set/2
 *
 * (C)Copyright IBM Corporation, 1991, 1992.  All rights reserved. Brian Yoder
 *
 * 01/26/90 - Created.
 * 01/26/90 - Initial version (PS/2 AIX).
 * 04/03/91 - Ported to DOS, C/2, and added stuff from AIX's <sys/types.h>.
 * 04/29/91 - Ported to OS/2 - Includes all of <os2.h>, so you don't have to
 *            include it (and shouldn't!).
 * 05/06/91 - Added support for recursive directory descent by slmatch().
 * 05/09/91 - Added support for directory name expansion by slmake().
 * 05/21/91 - Changed Ftime/Fdate to LastWrite instead of LastAccess!!!!!
 * 11/20/91 - Added function prototype for cfisapp() subroutine.
 * 04/13/92 - Added prototype for ishpfs() and added hpfs field to SLPATH.
 * 06/09/92 - Added function prototype for crcfile32() subroutine.
 * 07/22/92 - The file information buffer is FILEFINDBUF3, for 32-bit code.
 * 07/24/92 - Added SetFileMode for use by 32-bit code.
 * 09/19/92 - Added makepath() prototype, and added bpathlen field to SLPATH.
 * 11/11/92 - Added DOS16FIND* functions: Use *instead* of DosFind* APIs:
 *            see change history in speclist.c for more information on why.
 * 09/30/93 - Added definitions for lbuf.c functions.
 *============================================================================*/

#ifndef _h_UTIL /* else we've already been included */
#define _h_UTIL

#define INCL_BASE                   /* Include all of os2.h */
#include <os2.h>

#ifndef TRUE

#define FALSE    0
#define TRUE     1

#endif

/*============================================================================*
 * Some typedefs from AIX's <sys/types.h> that are nice to have around
 *============================================================================*/

#ifndef uint

/* Abbreviations for basic 'C' types with otherwise long names */
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;

/* System 5 compatability constants */
typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;

#endif

/*----------------------------------------------------------------------------*
 * Labels from AIX's <unistd.h>, modified to be meaningful for DOS
 *----------------------------------------------------------------------------*/

/* Symbolic constants for the "access" function: */
#define R_OK 4          /* Test for "Read" Permission */
#define W_OK 2          /* Test for "Write" Permission */
#define F_OK 0          /* Test for existence of file */

/*============================================================================*
 * Return values from rexpand() and rcompile() subroutines
 *============================================================================*/

#define REGX_OK             0       /* Successful */
#define REGX_ENDP          11       /* Range endpoint too large */
#define REGX_NUM           16       /* Bad number */
#define REGX_DIGIT         25       /* \digit out of range */
#define REGX_BADDELIM      36       /* Illegal or missing delimiter */
#define REGX_NOSTR         41       /* No remembered search string */
#define REGX_PARENS        42       /* () imbalance */
#define REGX_LPARENS       43       /* Too many ( */
#define REGX_NUMCNT        44       /* More than two numbers given in {} */
#define REGX_BRACE         45       /* } expected after \ */
#define REGX_NUMSIZE       46       /* First number exceeds second in {} */
#define REGX_BRKT          49       /* [] imbalance */
#define REGX_FULL          50       /* Regular expression expansion overflow */
#define REGX_MEMORY        51       /* Out of memory */

 /*----------------------------------------------------------------------------*
 * Return values from rmatch() and rscan() subroutines
 *----------------------------------------------------------------------------*/

#define RMATCH     0
#define NO_RMATCH  1


/*============================================================================*
 * Structure type used to hold information from the decompose() subroutine.
 * A pathname consists of '[d:path]name'.  This structure holds pointers and
 * lengths to the 'path' [d:path] and 'name' portions.
 *============================================================================*/

typedef struct {

     char      *path;               /* d:path portion, if any: Pointer */
     int        pathlen;            /*                         Length  */

     char      *name;               /* name portion:           Pointer */
     int        namelen;            /*                         Length  */

} PATH_NAME;

/*----------------------------------------------------------------------------*
 * Structure type used by cvftime() to store expanded version
 * of a file's date and time
 *----------------------------------------------------------------------------*/

typedef struct {

     int        ftm_sec;            /* seconds (always even!) */
     int        ftm_min;            /* minutes (0-59) */
     int        ftm_hr;             /* hours (0-23) */
     int        ftm_day;            /* day (1-31) */
     int        ftm_mon;            /* month (1-12) */
     int        ftm_year;           /* year (1980-2099) */

} FTM;

/*============================================================================*
 * Definitions for a specification list
 *
 * A specification list consists of a singly-linked list of SLPATH structures.
 * Each structure defines a unique drive:pathname, and points to a list of
 * SLNAME structures for that pathname.  See the slmake() subroutine
 * for information on how to make this list.
 *
 * The path field points to the (path) directory name. The bpathlen field
 * contains the length of the base path that was originally specified.
 * If the path was inserted (inserted==TRUE), then bpathlen contains the
 * length of the portion of the path name that wasn't inserted.
 *============================================================================*/

typedef struct slname {

     struct slname *next;           /* Pointer to next structure in list */
     char      *name;               /* Pointer to name specification */
     char      *ecname;             /* Expanded/compiled name spec: Pointer */
     int        eclen;              /*                              Length */
     long       fcount;             /* Count of no. of filename matches */

} SLNAME;

typedef struct slpath {

     struct slpath *next;           /* Pointer to next structure in list */
     SLNAME    *firstname;          /* Pointer to list of name specifications */
     SLNAME    *lastname;           /* Pointer to end of list of name specifications */
     char      *path;               /* Pointer to pathname */
     uint       bpathlen;           /* Length of pathname base */
     ushort     inserted;           /* Inserted by slmatch()? TRUE or FALSE */
     ushort     hpfs;               /* Is on HPFS filesystem? TRUE or FALSE */

} SLPATH;

/*----------------------------------------------------------------------------*
 * Attribute (mode) flags for slrewind().  The HIDDEN - DIR flags are the
 * same as their _A_ counterparts from <dos.h>.  The NORMAL and DOTDIR are
 * needed to complete the pattern.
 *----------------------------------------------------------------------------*/

#define SL_HIDDEN               0x02    /* Hidden file */
#define SL_SYSTEM               0x04    /* System file */
#define SL_VOLID                0x08    /* Volume ID file (not used by OS/2) */
#define SL_DIR                  0x10    /* Subdirectory, except . and ..  */

#define SL_NORMAL               0x40    /* Normal file (incl. readonly) */
#define SL_DOTDIR               0x80    /* Directories . and .. */

/*----------------------------------------------------------------------------*
 * _A_ labels that are in dos.h but aren't supported by C Set/2.  They're
 * defined here to maintain source code compatibility with C/2 and MSC.
 *----------------------------------------------------------------------------*/

#define _A_NORMAL     FILE_NORMAL       /* Normal file - No read/write restrictions */
#define _A_RDONLY     FILE_READONLY     /* Read only file */
#define _A_HIDDEN     FILE_HIDDEN       /* Hidden file */
#define _A_SYSTEM     FILE_SYSTEM       /* System file */
#define _A_VOLID              0x0008    /* Volume ID file (not used by OS/2) */
#define _A_SUBDIR     FILE_DIRECTORY    /* Subdirectory */
#define _A_ARCH       FILE_ARCHIVED     /* Archive file */

/*----------------------------------------------------------------------------*
 * Define common labels for DOS and OS/2.  These labels can be used to access
 * the file information data structure returned by findfirst/findnext calls.
 *----------------------------------------------------------------------------*/

#define FINFO FILEFINDBUF3          /* Shorter name, for convenience */

#define Fname achName
#define Fsize cbFile
#define Ftime ftimeLastWrite
#define Fdate fdateLastWrite
#define Fmode attrFile

/* begin DOS16FIND */
/*============================================================================*
 * Pragmas, prototypes, and structures for calling the 16-bit versions
 * of DosFindFirst/Next/Close: DOS16FINDFIRST/NEXT/CLOSE. See speclist.c
 * for examples of calling these functions. Do *not* use the 32-bit
 * DosFind* functions in OS/2 2.0: see speclist.c for why.
 *============================================================================*/

#pragma linkage ( DOS16FINDFIRST, far16 pascal )
#pragma linkage ( DOS16FINDNEXT,  far16 pascal )
#pragma linkage ( DOS16FINDCLOSE, far16 pascal )

typedef struct              {   /* from 1.3 toolkit's bsedos.h */
    FDATE  fdateCreation;
    FTIME  ftimeCreation;
    FDATE  fdateLastAccess;
    FTIME  ftimeLastAccess;
    FDATE  fdateLastWrite;
    FTIME  ftimeLastWrite;
    ULONG  cbFile;
    ULONG  cbFileAlloc;
    USHORT attrFile;
    UCHAR  cchName;
    CHAR   achName[CCHMAXPATHCOMP];
} DIRBUFF16;

#undef  FINFO                       /* Undo 32-bit definition */
#define FINFO DIRBUFF16             /* Shorter name, for convenience */

extern USHORT   DOS16FINDFIRST(
        char *fspec,           /* path name of files to be found              */
        PHDIR phdir,           /* directory handle                            */
        USHORT attr,           /* attribute used to search for the files      */
        DIRBUFF16 *buff,       /* result buffer                               */
        USHORT cbBuf,          /* length of result buffer                     */
        PUSHORT pcSearch,      /* number of matching entries in result buffer */
        ULONG ulReserved);     /* reserved (must be 0)                        */

extern USHORT   DOS16FINDNEXT(
        HDIR hdir,                      /* directory handle          */
        DIRBUFF16 *buff,                /* result buffer             */
        USHORT cbBuf,                   /* length of result buffer   */
        PUSHORT pcSearch);              /* number of entries to find */

extern USHORT   DOS16FINDCLOSE(
        HDIR hdir);                     /* directory handle */

/* end DOS16FIND */

/*============================================================================*
 * LBUF buffered stream structure.  This structure (and the buffer that
 * immediately follows it) should be allocated by the newlbuf subroutine.
 *============================================================================*/

typedef struct _lbuf {

     int        fd;                 /* File descriptor (-1 = none) */
     uint       bufflen;            /* Size of I/O buffer for stream */
     char      *buff;               /* Pointer to the start of the buffer */
     char      *end;                /* Pointer just past end of data in buffer */
     char      *next;               /* Pointer to next location to read */
     int        ferrno;             /* errno from an I/O error (0=no error) */
     int        seof;               /* end-of-stream?  (TRUE/FALSE) */

} LBUF;

/*============================================================================*
 * Function prototypes for library, for strong type-checking
 *============================================================================*/

int        makepath     ( char * );
int        crcfile      ( char *fname, ushort *crc, ulong *flen );
int        crcfile32    ( char *fname, ulong *crc, ulong *flen );

int        cfopen       ( char * );
int        cfsetfile    ( FILE * );
ulong      cfline       ( void );
int        cfisapp      ( void );
int        cfread       ( char *** );
char      *cfreadfile   ( void );
int        cfclose      ( void );
int        cfgetbyname  ( char *, char *, char ***, char ** );
int        cfstrcmpi    ( char *, char * );

int        rexpand      ( char *, char *, char *, char ** );
int        rcompile     ( char *, char *, char *, char, char ** );
int        rmatch       ( char *, char * );
int        rscan        ( char *, char *, char **, char ** );
char      *rcmpmsg      ( int );

int        text2bm      ( char *, FILE * );

int        cvtftime     ( unsigned, unsigned, FTM * );
char      *pathcat      ( char *, char * );
PATH_NAME *decompose    ( char * );
int        isdir        ( char * );
int        hasdrive     ( char * );
int        ishpfs       ( char * );
int        SetFileMode  ( char *, uint );

int        slmake       ( SLPATH **, int, int, int, char** );
void       slrewind     ( SLPATH * , ushort, ushort );
FINFO     *slmatch      ( SLPATH **, SLNAME ** );
void       sldump       ( SLPATH * );
char      *slerrspec    ( void );
long       slnotfound   ( SLPATH * );

LBUF      *newlbuf      ( uint bufflen );
void       setlbuf      ( LBUF *stream , int fd );
char      *lgets        ( char *str, uint n, LBUF *stream );

#endif /* _h_UTIL */
