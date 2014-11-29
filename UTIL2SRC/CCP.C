/*============================================================================*
 * main() module: ccp.c - Conditional copy command - OS/2 2.0 version.
 *
 * (C)Copyright IBM Corporation, 1991, 1992, 1993         Brian E. Yoder
 *
 * This program conditionally copies (based on size, modification time
 * and date, and an optional set of flags) source files to a target
 * directory.  See ccp.doc for more information.
 *
 * This DOS version of the program allocates a buffer to use during a file
 * copy.  The OS/2 version uses the DosCopy() subroutine, which (I suppose)
 * supplies its own copy buffer.
 *
 * 05/11/91 - Created, using crc.c as a template.
 * 05/19/91 - Version 1.0 - Initial DOS version completed.
 * 05/20/91 - Version 1.0 - Ported to initial OS/2 version.  The fcopy()
 *            subroutine in this module uses DosCopy() to copy a file.
 *            Also, the OS/2 file information structure defines the time
 *            and date as a bit record and not as a ushort, so the Fdate and
 *            Ftime fields must be handled differently than on DOS.
 * 01/11/92 - Version 1.1 - Add support for -f flag: force the target to
 *            be overwritten if it is readonly.
 * 03/26/92 - Version 1.2 - If there's only one source file specification,
 *            the target directory defaults to . (the current directory).
 * 07/24/92 - Version 1.2 - For OS/2 2.0 and C Set/2.
 * 09/21/92 - Version 1.3 - Supports the -s flag to copy subdirectories.
 *            Also supports the -t flag to list the target as well as source.
 * 11/11/92 - Call the 16-bit DosFindFirst/Next DOS16FIND* APIs.
 *            When scanning a directory on an HPFS drive, the 32-bit APIs
 *            skip over any name that begins with: ! # $ % ( ) - + ,
 * 09/28/93 - If copying from a non-HPFS (such as FAT) filesystem, force
 *            the target name to be lowercase.
 *============================================================================*/

static char ver[]  = "ccp: version 1.4";
static char copr[] = "(c)Copyright IBM Corporation, 1993.  All rights reserved.";

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>

#include <sys/stat.h>               /* Used by open(), etc. */
#include <fcntl.h>
#include <io.h>

#include "util.h"

#define CCP_DIFFERENT 0             /* Copy if source is different than target */
#define CCP_LATER     1             /* Copy if source is later than target */

#define CPE_FULL      1             /* Error codes from fcopy() */
#define CPE_OTHER     2
#define CPE_MKDIR     3             /* Error if we can't make a directory */

static USHORT attrib =              /* Set up attribute for findfirst */
           _A_NORMAL  |
           _A_RDONLY  |                   /* These labels are from <dos.h> */
           _A_HIDDEN  |
           _A_SYSTEM  |
        /* _A_VOLID   |  */               /* OS/2 doesn't support _A_VOLID */
           _A_SUBDIR  |
           _A_ARCH;

/* Not needed if we're using DosCopy() */
#define CPLEN_1  32000              /* Copy buffer length: 1st choice */
#define CPLEN_2  2048               /*                     2nd choice */

#define XLEN     2048               /* Total length of compiled reg expressions */
static  char     xsrcbuff[XLEN];    /* Buffer to hold compiled regular expressions */

#define ELEN     4096               /* Length of expression/filename buffer */
static  char     buff[ELEN+1];      /* Buffer for output filenames and expansions */

/*============================================================================*
 * Full prototypes for private (static) functions
 *============================================================================*/

static void       syntax       ( void );
static int        excludef     ( char *, int, char ** );
static int        copyit       ( char *, ulong, ushort, ushort, ushort,
                                 char *, int, int );
static int        fcopy        ( char *, ushort, ushort, ushort, char *, int );

/*============================================================================*
 * Main Program Entry Point
 *============================================================================*/

main(argc, argv)

int argc;                           /* arg count */
char *argv[];                       /* arg pointers */

{
  int     rc;                       /* Return code storage */
  char   *str;                      /* Pointer to string */
  long    lrc;                      /* Long return code */
  char   *flagstr;                  /* Pointer to string of flags */

  char   *tdir;                     /* Pointer to target directory name */
  uint    tlen;                     /* Length of target directory name */

  int     srcc;                     /* Count and pointer to array of */
  char  **srcv;                     /* source file specifications */

  int     xsrc;                     /* Count and pointer to array of */
  char  **xsrv;                     /* xsource (exclusion) file specifications */
  PATH_NAME *pn;                    /* Pointer returned by decompose() */

  int     cnt;                      /* General count */
  char  **charv;                    /* General character pointer vector */
  char   *next_eexpr;               /* Pointer returned by rexpand() */
  char   *xsrccurr;                 /* Current pointer within xsrcbuff[] */
  char   *xsrcnext;                 /* Pointer returned by rcompile() */

  int     uniq;                     /* 'uniq' flag for slmake() */
  SLPATH *speclist;                 /* Pointer to linked list of path specs */
  ushort  mflags;                   /* Match flags for slrewind() */
  ushort  recurse;                  /* Recursive directory descent? */

  FINFO  *fdata;                    /* Pointers returned by slmatch() */
  SLPATH *pdata;
  SLNAME *ndata;

  char   *sourcename;               /* Pointers to name strings */
  char   *targetname;
  char   *basename;
  uint    sourcelen;                /* And the lengths of these strings */
  uint    targetlen;
  uint    baselen;

  ushort  sdate;                    /* Source file's time/date */
  ushort  stime;

  int     ccopy;                    /* Conditional copy: different/later/etc. */
  int     mustexist;                /* Must target file exist before copying? */
  int     copyfiles;                /* Actually perform the copy? TRUE/FALSE */
  int     showx;                    /* Show names of files excluded (not copied)? */
  int     force;                    /* Force readonly files to be overwritten? */
  int     showtarget;               /* Show target as well as source? */

  ulong   copycnt = 0L;             /* No. of files copied */

/*----------------------------------------------------------------------------*
 * Set initial values of flags:
 *----------------------------------------------------------------------------*/

  uniq      = TRUE;                 /* Build SLPATHs only for unique paths */
  recurse   = FALSE;                /* Don't descend subdirectories */
  ccopy     = CCP_DIFFERENT;        /* Copy if source is different than target */
  mustexist = FALSE;                /* Target file doesn't have to exist */
  copyfiles = TRUE;                 /* Yes, we want to copy files */
  showx     = FALSE;                /* Don't show names of excluded files */
  force     = FALSE;                /* No, don't overwrite readonly target files */
  showtarget= FALSE;                /* No, just show source filenames */

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
           case 'l':
           case 'L':
              ccopy = CCP_LATER;              /* Copy if source is later than target */
              break;

           case 'e':
           case 'E':
              mustexist = TRUE;               /* Target file must exist before copying */
              break;

           case 'n':
           case 'N':
              copyfiles = FALSE;              /* Don't copy any files */
              break;

           case 'x':
           case 'X':
              showx = TRUE;                   /* Show files excluded (not copied) */
              break;

           case 'f':
           case 'F':
              force = TRUE;                   /* Force overwriting readonly files */
              break;

           case 'g':                          /* 'g' means group in order that */
           case 'G':                          /* filespecs were listed on cmd line */
              uniq = FALSE;
              break;

           case 's':                          /* 's' means recurse subdirectories */
           case 'S':
              recurse = TRUE;
              break;

           case 't':                          /* 't' means show target filenames */
           case 'T':
              showtarget = TRUE;
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

 /*---------------------------------------------------------------------------*
  * Store pointer to the name of the target directory, and its length
  *---------------------------------------------------------------------------*/

  if (argc <= 0) syntax();          /* If no arguments: Display syntax */

  if (argc == 1)                    /* If only one argument: */
  {                                      /* Assume it's a source filespec */
     tdir = ".";                         /* Assume target is current directory */
  }
  else                              /* Else: last argument is target directory */
  {
     tdir = argv[argc-1];                /* Point to last argument: target dir */
     argc--;                             /* Decrement arg count to exclude last arg */
  }

  tlen = strlen(tdir);              /* Store length of target directory */

 /*---------------------------------------------------------------------------*
  * Find out how many source file specifications we have (at least one needed)
  *---------------------------------------------------------------------------*/

  srcc = 0;                         /* Initialize to no source specs */
  srcv = argv;                      /* Point source spec vector to 1st arg */

  while (argc > 0)                  /* For each argument: */
  {
     if ((*argv)[0] == '!')             /* If we found a !: */
        break;                               /* The break out of loop */
     srcc++;                            /* We have a source spec: Update count */
     argc--;                            /* Decrement no. of arguments left */
     argv++;                            /* And point to next argument */
  }

  if (srcc <= 0)                    /* If no source file specs: Error */
  {
     fprintf(stderr, "ccp: Source file specification(s) are missing.\n");
     return(2);
  }

 /*---------------------------------------------------------------------------*
  * Find out how many xsource (exclusion) file specifications we have.
  * If we broke out of the previous loop because of a !, then we may have
  * xsource specs.  We allow the ! and the first xsource spec to either
  * be part of the same argument or be in separate arguements (i.e. we
  * allow: '!*.exe' and '! *.exe').
  *---------------------------------------------------------------------------*/

  xsrc = 0;                         /* Assume no xsource specs, then: */

  if ( (argc >= 1) &&               /* If there's at least one arg left, */
     ( (*argv)[0] == '!') )         /* and it is a !, then: */
  {                                 /* We have xsource specifications: */
     if ((*argv)[1] != '\0')             /* If ! is not by itself: */
     {                                        /* Then rest of arg is an xsource */
        xsrc = 1;                             /* So we have at least one xsource */
        xsrv = argv;                          /* Point xsrv to current argv */
        (*xsrv)++;                            /* And bump its pointer past the ! */
     }
     else                                /* Else: the ! is by itself: */
        xsrv = argv + 1;                      /* xsrv vector starts at next arg */

     argc--;                             /* Decrement arg count */
     argv++;                             /* And point to next argument */

     while (argc > 0)                    /* For each argument left: */
     {
        xsrc++;                               /* It's an xsource spec */
        argc--;                               /* Decrement arg count */
        argv++;                               /* And point to next argument */
     }

     if (xsrc == 0)                      /* If we have no xsource (but found a !): */
     {                                        /* Error! */
        fprintf(stderr, "ccp: Found ! but there are no xsource specifications.\n");
        return(2);
     }
  }

 /*---------------------------------------------------------------------------*
  * TEST: Display source, xsource, and target specifications
  *---------------------------------------------------------------------------*/

//printf("\n");
//printf("Target directory: '%s'\n", tdir);
//
//printf("Source specs:\n");
//cnt = srcc;
//charv = srcv;
//while (cnt > 0)
//{
//   printf("   '%s'\n", *charv);
//   cnt--;
//   charv++;
//}
//
//printf("Xsource specs:\n");
//cnt = xsrc;
//charv = xsrv;
//while (cnt > 0)
//{
//   printf("   '%s'\n", *charv);
//   cnt--;
//   charv++;
//}
//
//printf("\n");
//exit(0);

 /*---------------------------------------------------------------------------*
  * Be sure that the target directory exists
  *---------------------------------------------------------------------------*/

  if (!isdir(tdir))
  {
     fprintf(stderr, "ccp: '%s' is not a valid target directory.\n", tdir);
     return(2);
  }

 /*---------------------------------------------------------------------------*
  * Expand all xsource file specifications into regular expressions and compile
  * While we're at it, be sure that none has any drive or path information.
  *
  * Each xsource spec is expanded into a regular expression and stored in
  * the 'xsrcbuff' buffer for compiling.  The compiled regular expressions
  * are stored back-to-back in the 'xsrcbuff' buffer.
  *
  * As each xsource spec is processed, the xsource pointer in the 'xsrv' array
  * is overwritten with a pointer to its compiled regular expression in the
  * buffer.
  *---------------------------------------------------------------------------*/

  cnt = xsrc;                       /* Set count to no. of xsource specs */
  charv = xsrv;                     /* And set up vector pointer to 1st one */
  xsrccurr = xsrcbuff;              /* Point xsrccurr to start of buffer */

  while (cnt > 0)                   /* For each xsource spec: */
  {
     pn = decompose(*charv);             /* Check for d:path within it */
     if (pn->pathlen != 0)               /* If xsource spec contains d:path: */
     {                                        /* Error! */
        fprintf(stderr, "ccp: !xsource '%s' contains a drive and/or path.\n", *charv);
        return(2);
     }

     rc = rexpand(*charv, buff,          /* Expand it into a regular expression */
                  &buff[ELEN+1], &next_eexpr);
     if (rc != 0)
     {
        fprintf(stderr, "ccp: !xsource specification is too long: '%s'\n", *charv);
        return(2);
     }

     strupr(buff);                       /* Convert spec to upper case */
     rc = rcompile(buff, xsrccurr,       /* Compile the regular expression */
                   &xsrcbuff[XLEN+1], '\0',  /* rcompile() updates xsrcnext to */
                   &xsrcnext);               /* point to where next one will be stored */
     if (rc != 0)
     {
        fprintf(stderr, "ccp: !xsource specification is not valid: '%s'\n", *charv);
        return(2);
     }

     *charv = xsrccurr;                  /* Overwrite with pointer to compiled expr */
     xsrccurr = xsrcnext;                /* And update past compiled expression */

     cnt--;                              /* And process next xsource spec */
     charv++;
  }

 /*---------------------------------------------------------------------------*
  * Process source file specifications and build specification list
  *---------------------------------------------------------------------------*/

  rc = slmake(&speclist, uniq, TRUE, srcc, srcv);

  if (rc != 0)                      /* If there was an error: */
  {                                 /* Analyze rc, show msg, and return */
     if (rc == REGX_MEMORY)
        fprintf(stderr, "ccp: Out of memory while processing '%s'.\n",
           slerrspec());
     else
        fprintf(stderr, "ccp: source specification is not valid: '%s'.\n",
           slerrspec());

     return(2);
  }

 /*---------------------------------------------------------------------------*
  * If we aren't going to actually copy any files, say so now!
  *---------------------------------------------------------------------------*/

  if (!copyfiles)
  {
     fprintf(stderr, "ccp: *** No files will actually be copied ***\n");
     fflush(stderr);
  }

 /*---------------------------------------------------------------------------*
  * Conditionally copy each matching source file in the specification list
  *---------------------------------------------------------------------------*/

  mflags = 0;                       /* Set list of files to match to include: */
  mflags = mflags | SL_NORMAL;           /* Ordinary files only */

  slrewind(speclist, mflags, recurse);/* Initialize slmatch() */
  for (;;)                          /* Loop to find all matching files: */
  {
    /*-----------------------------------------------------*
     * Get next matching file, if any
     *-----------------------------------------------------*/

     fdata = slmatch(&pdata,             /* Get next matching file */
                     &ndata);
     if (fdata == NULL)                  /* If none: */
        break;                                /* Done */

    /*-----------------------------------------------------*
     * Before we store names in buff[], be sure they will
     * fit by calculating the approx. max lengths of names.
     * Lengths are approx. at first, since we don't know
     * if we need a path separator.  Later, we'll find the
     * exact lengths, after calling pathcat().
     *-----------------------------------------------------*/

     baselen = strlen(fdata->Fname);     /* Calculate true length of base name */

     sourcelen = strlen(pdata->path) + baselen + 1;  /* approx. */
     targetlen = strlen(tdir) + baselen + 1;         /* approx. */

     if (recurse)
        targetlen += sourcelen - baselen + 1;        /* very approx. */

     if ((baselen + sourcelen + targetlen + 3) > ELEN)
     {                                   /* Add 3 for the null terminators! */
        fprintf(stderr, "ccp: Internal error: Names are too long.\n");
        return(2);
     }

    /*-----------------------------------------------------*
     * Store names in the buff[] buffer, as 3 strings:
     *    sourcename,0    Full pathname of source file
     *    targetname,0    Full pathname of target file
     *    basename,0      Uppercase filename portion
     *-----------------------------------------------------*/

     sourcename = buff;                  /* Store full path+name of source */
     strcpy(sourcename, pdata->path);
     pathcat(sourcename, fdata->Fname);
     sourcelen = strlen(sourcename);          /* And store its true length */

     targetname = sourcename +           /* Store full path+name of target */
                  sourcelen + 1;
     strcpy(targetname, tdir);

     if (recurse)                        /* If recursing subdirectories: */
     {
        str = pdata->path +                   /* Point str past base portion */
             pdata->bpathlen;                 /* of the path */
        if (*str == '\\') str++;              /* Bump past path separator, if any */

        pathcat(targetname, str);             /* And add the recursed subdir's name */
     }                                        /* to the target path */

     pathcat(targetname, fdata->Fname);
     targetlen = strlen(targetname);          /* And store its true length */

     if (!pdata->hpfs)                        /* If source is not HPFS: */
        strlwr(targetname);                      /* Make targetname lowercase */

     basename = targetname +             /* Store upper-case base name */
                targetlen + 1;
     strcpy(basename, fdata->Fname);
     strupr(basename);

    /*-----------------------------------------------------*
     * TEST: Display name strings
     *-----------------------------------------------------*/
/*
     printf("Copy from: '%s'\n", sourcename);
     printf("Copy to:   '%s'\n", targetname);
     printf("Base name: '%s'\n", basename);
     printf("\n");
*/
    /*-----------------------------------------------------*
     * Compare basename against compiled xsource expressions
     * to see if we should exclude this source file
     *-----------------------------------------------------*/

     if (excludef(basename, xsrc, xsrv)) /* If we are to exclude this file: */
     {
        if (showx)                            /* If we are to display its name: */
        {
           printf("! %s\n", sourcename);           /* Display its name */
           fflush(stdout);
        }
     }
     else                                /* Else: user didn't exclude file: */
     {
         /*--------------------------------------------*
          * Check to see if we should copy this file
          *--------------------------------------------*/
          memcpy(&sdate,                      /* Store source file's date/time */
                 &fdata->fdateLastWrite,
                 sizeof(ushort));
          memcpy(&stime,
                 &fdata->ftimeLastWrite,
                 sizeof(ushort));
          if (copyit(sourcename,              /* If the file should be copied: */
                     fdata->Fsize,
                     sdate, /*fdata->Fdate,*/
                     stime, /*fdata->Ftime,*/
                     fdata->Fmode,
                     targetname,
                     mustexist,
                     ccopy))
          {
             if (!showtarget)                      /* Display either: */
                printf("%s\n", sourcename);             /* Just the source */
             else                                  /* or: */
                printf("%s -> %s\n", sourcename,        /* Both source and target */
                                     targetname);
             fflush(stdout);
             rc = 0;                               /* Init rc to zero */
             if (copyfiles)                        /* If we are to perform the copy: */
             {
                if (recurse)                            /* If recursing subdirs: */
                {
                   rc = makepath(targetname);                /* We might need to make */
                   if (rc != 0)                              /* them in the target dir */
                      rc = CPE_MKDIR;
                }

                if (rc == 0)                            /* If no errors so far: */
                {
                  rc = fcopy(sourcename,                  /* Perform the copy */
                             sdate,
                             stime,
                             fdata->Fmode,
                             targetname,
                             force);
                }

                switch (rc)                             /* Check return code from copy: */
                {
                   case 0:                                   /* If ok: */
                      break;                                      /* Keep going */
                   case CPE_FULL:                            /* If target is full: */
                      fflush(stdout);                             /* Display error msg */
                      fprintf(stderr, "ccp: Target is full when copying to %s\n",
                         targetname);
                      fflush(stderr);
                      return(2);                                  /* Return: can't copy if full */
                      break;
                   case CPE_MKDIR:                           /* If we can't make a dir: */
                      fflush(stdout);                             /* Display error msg */
                      fprintf(stderr, "ccp: Can't make a directory within %s\n",
                         targetname);
                      fflush(stderr);
                      break;
                   default:                                  /* If other error: */
                      fflush(stdout);
                      fprintf(stderr, "ccp: Error while copying to %s\n",
                         targetname);
                      fflush(stderr);
                      break;
                } /* end of switch stmt to process fcopy() return code */
             } /* end of 'if (copyfiles)' */

             if (rc == 0) copycnt++;               /* Update count of files copied */

          } /* end of 'if the file should be copied */
          else                                /* Else: ccp says "don't copy file": */
          {
             if (showx)                            /* If we are to display its name: */
             {
                printf("x %s\n", sourcename);           /* Display its name */
                fflush(stdout);
             }
          } /* end of check to see if file should be copied */
     } /* end of check to see if user wants to exclude this file */
  } /* end of loop to find all matching files */

 /*---------------------------------------------------------------------------*
  * Display (on stderr) all source filespecs that had no matching files
  *---------------------------------------------------------------------------*/

  lrc = slnotfound(speclist);       /* Display path\names w/no matching files */
  if (lrc == 0L)                    /* If all fspecs were matched: */
     rc = 0;                             /* Setup for return successfully */
  else                              /* Otherwise:  One or more not found: */
     rc = 2;                             /* Setup for return with error */

 /*---------------------------------------------------------------------------*
  * Display (on stdout) how many files we copied
  *---------------------------------------------------------------------------*/

  fflush(stdout);                   /* First flush any stdout filenames */
  fprintf(stderr, "%lu file(s) copied.\n", copycnt);

  return(rc);                       /* Return with proper exit code */

} /* end of main() */

/*============================================================================*
 * syntax() - Display command syntax and exit to operating system!
 *============================================================================*/
static void syntax()
{
  fprintf(stderr, "%s\n", ver);
  fprintf(stderr, "Usage: ccp [-flags] source ... [! Xsource ...] targetdir\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "The ccp program conditionally copies files that match the source file\n");
  fprintf(stderr, "specification(s), excluding those whose names match the optional Xsource\n");
  fprintf(stderr, "file specification(s), to the target directory. Source files are copied if\n");
  fprintf(stderr, "they don't exist in the target directory or if they have a different time,\n");
  fprintf(stderr, "date, or size in the target directory. The following flags are supported:\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  l  If a copy of the source file exists in the target directory, copy the\n");
  fprintf(stderr, "     source file only if it is later than the one in the target directory.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  e  Don't copy a source file that doesn't already exist in the\n");
  fprintf(stderr, "     target directory.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  f  Force copy even if target file is read-only.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  n  Don't copy any files; just list the names of the files that\n");
  fprintf(stderr, "     would have been copied.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  s  Descend subdirectories and preserve them in the target directory.\n");
//fprintf(stderr, "\n");
//fprintf(stderr, "  t  Show the names of each target filename as well as each source filename.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  x  Also list the names of files that are being excluded (not copied).\n");
  exit(1);
}

/*============================================================================*
 * excludef() - Should we exclude the file?
 *
 * REMARKS:
 *   This subroutine compares the base name of a file to a list of compiled
 *   regular expressions.
 *
 * RETURNS:
 *   TRUE: If the file matches one of the regular expressions: Exclude it!
 *   FALSE: If the file matches none of the regular expressions.
 *============================================================================*/

static int excludef(

char    *name ,                     /* Uppercase version of base filename */
int      regc ,                     /* No. of regular expressions */
char   **regv )                     /* Pointer to array of regular expressions */

{
  while (regc > 0)                  /* For each expression in array: */
  {
     if (rmatch(*regv, name) == 0)       /* If we found a match: */
        return(TRUE);                         /* Return TRUE */
     regc--;                             /* Else try next expression */
     regv++;
  }

  return(FALSE);                    /* We matched none of the expressions */
}

/*============================================================================*
 * copyit() - Should we copy the file?
 *
 * REMARKS:
 *   This subroutine compares source file with the target file (if it exists),
 *   checks the flags passed to it, and determines whether or not we should
 *   copy the file.
 *
 * RETURNS:
 *   TRUE: If the file should be copied.
 *   FALSE: If the file should NOT be copied.
 *
 * SIDE EFFECTS:
 *   If the target file exists but is a directory, this routine returns
 *   FALSE (don't copy) but prints an error message.
 *============================================================================*/

static int copyit(

char    *source      ,              /* Full pathname of source file */
ulong    ssize       ,              /* Size of source file */
ushort   sdate       ,              /* Date of source file */
ushort   stime       ,              /* Time of source file */
ushort   smode       ,              /* File mode of source file */
char    *target      ,              /* Full pathname of target file */
int      MustExist   ,              /* Must target exist (TRUE, FALSE) */
int      condition   )              /* CCP_DIFFERENT, CCP_LATER ... */

{
  int    rc;                        /* Return code storage */
  ushort tdate;                     /* Target file's date/time */
  ushort ttime;

  ulong   scnt;                     /* Count used by findfirst */
  ushort  scnt16;
  HDIR    findh;                    /* Handle returned by DosFindFirst() */

  FINFO  finfo;                     /* File information structure */

  findh = HDIR_CREATE;              /* Set initial value of handle */
  scnt = 1;                         /* Find target file */
  scnt16=1;
//rc = DosFindFirst(target,
////      &findh, attrib,
////      &finfo, sizeof(FINFO),
////      &scnt,
////      (ULONG)FIL_STANDARD);          /* Was 0L for 1.3. Is 1 for 2.0  */
  rc = DOS16FINDFIRST(target,
          &findh, (USHORT)attrib,
          &finfo, sizeof(FINFO),
          &scnt16, 0L);
  scnt=(ULONG)scnt16;

  if ((rc != 0) || (scnt == 0))     /* If target doesn't exist: */
  {
     if (MustExist)                      /* If target must exist: */
        return(FALSE);                        /* Then don't perform a copy */
     else                                /* Else target doesn't have to exist: */
        return(TRUE);                         /* Then we need to copy, period */
  }
  else                              /* Else target exists: */
  {
//// DosFindClose(findh);                /* (Close the handle first) */
     DOS16FINDCLOSE(findh);              /* (Close the handle first) */
     if ((finfo.Fmode & _A_SUBDIR) != 0) /* Be sure it's not a directory */
     {
        fprintf(stderr, "ccp: Target file is a directory: %s\n", target);
        fflush(stderr);
        return(FALSE);
     }

      memcpy(&tdate,                      /* Store target file's date/time */
             &finfo.fdateLastWrite,
             sizeof(ushort));
      memcpy(&ttime,
             &finfo.ftimeLastWrite,
             sizeof(ushort));

     switch (condition)                  /* Check condition for copy: */
     {
        case CCP_DIFFERENT:                   /*---- Time/date/size must be different: */
           if (ssize != finfo.Fsize) return(TRUE);
           if (sdate != tdate) return(TRUE);
           if (stime != ttime) return(TRUE);
           return(FALSE);                          /* Source/target are the same */
           break;

        case CCP_LATER:                       /*---- Source must be later: */
          /*---------------------------------------------*
           *   The source is later than the target if:
           *      (date(source) > date(target)), or:
           *      (date(source) == date(target)) &&
           *      (time(source) > time(target))
           *---------------------------------------------*/
           if (sdate > tdate) return(TRUE);
           if ((sdate == tdate) &&
               (stime > ttime)) return(TRUE);
           return(FALSE);                          /* Source is same or earlier */
           break;
     }
  }

  return(FALSE);                    /* Will only get here if 'condition' is bad */
}

/*============================================================================*
 * fcopy() - Copy file (OS/2 version).
 *
 * REMARKS:
 *   This subroutine copies the source file to the target file, overwriting
 *   the target file if it already exists.
 *
 *   The OS/2 version requires no copy buffer, since it uses OS/2's
 *   OS/2's DosCopy() subroutine.
 *
 *   If an error occurs, the target file (or what there is of it) is erased.
 *
 * RETURNS:
 *   0         : Successful.
 *   CPE_FULL  : The target is full.
 *   CPE_OTHER : Other error.
 *============================================================================*/

#define _COPY_OP   0x0001           /* Replace target, Copy even if target exists */

static int fcopy(

char    *source ,                   /* Full pathname of source file */
ushort   sdate  ,                   /* Date of source file */
ushort   stime  ,                   /* Time of source file */
ushort   smode  ,                   /* File mode of source file */
char    *target ,                   /* Full pathname of target file */
int      force  )                   /* Force copy if target is readonly? */

{
  int    rc;                        /* Return code storage */
  int    cperr;                     /* File copy error code */

 /*---------------------------------------------------------------------------*
  * Copy the source file to the target file:
  *---------------------------------------------------------------------------*/

  cperr = DosCopy(source, target,   /* Copy the file */
                 _COPY_OP);

 /*---------------------------------------------------------------------------*
  * If access to target file was denied and force is TRUE, make file
  * writable and try again
  *---------------------------------------------------------------------------*/

  if ((cperr == ERROR_ACCESS_DENIED) &&
     (force == TRUE))               /* If target is readonly and force is TRUE: */
  {
     cperr = chmod(target, S_IWRITE);    /* Make target writable */
     if (cperr == 0)
     {
        cperr = DosCopy(source, target,  /* Again, copy the file */
                       _COPY_OP);
     }
  }

 /*---------------------------------------------------------------------------*
  * Done: Check for errors and return
  *---------------------------------------------------------------------------*/

  if (cperr != 0)                   /* If error: */
     return(CPE_OTHER);                  /* Return error code */

  return(0);                        /* Done with copy */
}
