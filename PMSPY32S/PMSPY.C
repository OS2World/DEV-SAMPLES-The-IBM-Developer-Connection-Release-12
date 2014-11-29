/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                                          ³*/
/*³ PROGRAM NAME: PMSPY                                                      ³*/
/*³ -------------                                                            ³*/
/*³  A PM program that is used to look at or 'spy' on the message queue of   ³*/
/*³  other PM applications windows.                                          ³*/
/*³                                                                          ³*/
/*³ COPYRIGHT:                                                               ³*/
/*³ ----------                                                               ³*/
/*³  Copyright (C) International Business Machines Corp., 1992               ³*/
/*³                                                                          ³*/
/*³ DISCLAIMER OF WARRANTIES:                                                ³*/
/*³ -------------------------                                                ³*/
/*³  The following [enclosed] code is sample code created by IBM Corporation.³*/
/*³  This sample code is not part of any standard IBM product and is provided³*/
/*³  to you solely for the purpose of assisting you in the development of    ³*/
/*³  your applications.  The code is provided "AS IS", without warranty of   ³*/
/*³  any kind.  IBM shall not be liable for any damages arising out of your  ³*/
/*³  use of the sample code, even if they have been advised of the           ³*/
/*³  possibility of such damages.                                            ³*/
/*³                                                                          ³*/
/*³ REVISION LEVEL: 1.0                                                      ³*/
/*³ ---------------                                                          ³*/
/*³                                                                          ³*/
/*³ WHAT THIS PROGRAM DOES:                                                  ³*/
/*³ -----------------------                                                  ³*/
/*³  This program uses a system APIs to 'hook' into application queues in    ³*/
/*³  order to display the messages being received by that application.  This ³*/
/*³  is very useful when debugging PM applications where the order of        ³*/
/*³  messages and/or whether or not they are actually received makes a       ³*/
/*³  difference while debugging.                                             ³*/
/*³                                                                          ³*/
/*³ WHAT THIS PROGRAM DEMONSTRATES:                                          ³*/
/*³ -------------------------------                                          ³*/
/*³  This program demonstrates how to 'hook' into PM using the system APIS   ³*/
/*³  in order to see what messages an application is receiving.              ³*/
/*³                                                                          ³*/
/*³ WHAT YOU NEED TO COMPILE THIS PROGRAM:                                   ³*/
/*³ --------------------------------------                                   ³*/
/*³                                                                          ³*/
/*³  REQUIRED FILES:                                                         ³*/
/*³  ---------------                                                         ³*/
/*³                                                                          ³*/
/*³  For PMSPY.EXE...                                                        ³*/
/*³    PMSPY    C     - main source file                                     ³*/
/*³    PMSPY    H     - main PMSPY header file used by many of the files     ³*/
/*³    PMSPYABT C     - procedure to display logo or 'about' screen          ³*/
/*³    PMSPYFIL C     - message filter dialog procedure                      ³*/
/*³    PMSPYFON C     - font selection dialog procedure                      ³*/
/*³    PMSPYGRP C     - group message filter dialog procedure                ³*/
/*³    PMSPYIPF C     - IPF processing related code                          ³*/
/*³    PMSPYIPF H     - IPF panel id header file                             ³*/
/*³    PMSPYMSG C     - MSG and GROUP data structure support routines        ³*/
/*³    PMSPYNOT C     - 'notes' dialog procedure                             ³*/
/*³    PMSPYOW  C     - object window selection dialog procedure             ³*/
/*³    PMSPYPRO C     - profile manipulation routines                        ³*/
/*³    PMSPYUTL C     - miscellaneous utility routines                       ³*/
/*³    PMSPYWIN C     - primary PMSPY client area window procedure           ³*/
/*³    PMSPY    DEF   - module definition file                               ³*/
/*³    PMSPY    ICO   - icon file                                            ³*/
/*³    PMSPY    MAK   - make file                                            ³*/
/*³    PMSPY    PTR   - pointer file (magnifying glass)                      ³*/
/*³    PMSPY    RC    - resource file                                        ³*/
/*³    PMSPYRC  H     - resource id header file                              ³*/
/*³    PMSPYDEF PRO   - pmspy profile                                        ³*/
/*³    PMSPY    IPF   - doc file that get built into HLP file                ³*/
/*³                                                                          ³*/
/*³  For PMSPYDEF.EXE...                                                     ³*/
/*³    PMSPYDEF C     - main source file to build PMSPY (default) profiles   ³*/
/*³                                                                          ³*/
/*³  For PMSPYHK.DLL...                                                      ³*/
/*³    PMSPYHK  C     - main source file for DLL that 'hooks' into system    ³*/
/*³    PMSPYHK  DEF   - module definition file for PMSPYHK                   ³*/
/*³    PMSPYHK  H     - header file for PMSPYHK                              ³*/
/*³                                                                          ³*/
/*³  For PMSPYNLS.DLL...                                                     ³*/
/*³    PMSPYNLS C     - stub file for creating NLS DLL                       ³*/
/*³    PMSPYNLS DEF   - module definition file for PMSPYNLS                  ³*/
/*³    PMSPYNLS RC    - resource file                                        ³*/
/*³                                                                          ³*/
/*³  For SPY-PROF.EXE...                                                     ³*/
/*³    SPY-PROF C     - source for program used to build application specific³*/
/*³                     profile files                                        ³*/
/*³    SPY-PROF MAK   - SPY-PROF make file                                   ³*/
/*³                                                                          ³*/
/*³  REQUIRED LIBRARIES:                                                     ³*/
/*³  -------------------                                                     ³*/
/*³                                                                          ³*/
/*³    OS2386.LIB     - OS/2 API library                                     ³*/
/*³                                                                          ³*/
/*³  REQUIRED PROGRAMS:                                                      ³*/
/*³  ------------------                                                      ³*/
/*³                                                                          ³*/
/*³    IBM C Set/2 Compiler (icc.exe)                                        ³*/
/*³    IBM Linker (link386.exe)                                              ³*/
/*³    Resource Compiler (rc.exe)                                            ³*/
/*³                                                                          ³*/
/*³ EXPECTED INPUT:                                                          ³*/
/*³ ---------------                                                          ³*/
/*³                                                                          ³*/
/*³    You can start the program minimized by using the /MIN option.         ³*/
/*³                                                                          ³*/
/*³ EXPECTED OUTPUT:                                                         ³*/
/*³ ----------------                                                         ³*/
/*³                                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ PMSPY.C                                                                  ³*/
/*³                                                                          ³*/
/*³ Program to spy on other windows message queues                           ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Includes                                                                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define DEFINE_VARIABLES

#define INCL_DOSPROCESS                 /* need DosExitList stuff....         */

#include "pmspy.h"                      /* Resource symbolic identifiers      */

static HAB      hab           = NULLH;
static HMQ      hmq           = NULLH;

static HWND     hwndFrame     = NULLH;         /* FRAME window handle                */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SUBROUTINE_NAME  (LoadStringTable)                                       ³*/
/*³                                                                          ³*/
/*³                                                                          ³*/
/*³                       Module Level Design                                ³*/
/*³                                                                          ³*/
/*³ Function:                                                                ³*/
/*³     To load STRINGTABLE items from their disk resource and place pointers³*/
/*³     to them in passed string table array.                                ³*/
/*³                                                                          ³*/
/*³     Because DosGetResourc2 is used, we get some special benefits:        ³*/
/*³                                                                          ³*/
/*³     (1) only one I/O operation is required for each 16 strings (much     ³*/
/*³         better than DosGetResource or WinLoadString...)                  ³*/
/*³                                                                          ³*/
/*³     (2) the STRINGTABLE resource segment is now SHARED among all programs³*/
/*³         that access the same STRINGTABLE segment concurrently, memory    ³*/
/*³         consumption by multiple instances of the same program is reduced ³*/
/*³         as well as speeding up their initialization phase (because the   ³*/
/*³         segment is already present in memory and OS/2 doesn't read it    ³*/
/*³         from disk again (i hope!)                                        ³*/
/*³                                                                          ³*/
/*³ Input:                                                                   ³*/
/*³     HMODULE hmodRes         tells where to load STRINGTABLE resource from³*/
/*³                              - NULL      = use .EXE                      ³*/
/*³                              - Otherwise = value from WinLoadModule()    ³*/
/*³                                                                          ³*/
/*³     ULONG   usTableItems     total # items in pszTable[]                 ³*/
/*³                                                                          ³*/
/*³     PSZ     pszTable[]       table to file with pointers to each string  ³*/
/*³                                                                          ³*/
/*³ Output:                                                                  ³*/
/*³     Returns  TRUE....if all strings loaded                               ³*/
/*³                      - pszTable[] has been filled with pointers to       ³*/
/*³                        items in the resource segment(s)                  ³*/
/*³                      - use FreeStringTable to cleanup pszTable[]         ³*/
/*³                                                                          ³*/
/*³              FALSE...if error was detected                               ³*/
/*³                                                                          ³*/
/*³ Logic:                                                                   ³*/
/*³                                                                          ³*/
/*³     for(i = 0;             Initialize: from first string                 ³*/
/*³     |   i < usTableItesm   while:      more strings to go                ³*/
/*³     |   i++)               Interate:   move to the next string           ³*/
/*³     |                                                                    ³*/
/*³     | if (this string is in a new RESOURCE segment)                      ³*/
/*³     | |  call DosGetResource2 to "get" it                                ³*/
/*³     | |  set P = address(first string in new segment)                    ³*/
/*³     |                                                                    ³*/
/*³     | set pszTable[i] = P + 1     real string starts at byte after length³*/
/*³     |                                                                    ³*/
/*³     | P = P + *P + 1              point at next string in segment        ³*/
/*³     |                                                                    ³*/
/*³                                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static ULONG  ulStringsPerRes = 16;    /* # strings in each RESOURCE segment  */

BOOL LoadStringTable(HMODULE hmodRes,
                     ULONG   ulTableItems,
                     PSZ     pszTable[] )

{
  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Local structure definitions                                            ³*/
  /*³                                                                        ³*/
  /*³ - these items define templates for overlaying the RESOURCE segment     ³*/
  /*³   read by DosGetResource/DosGetResource2                               ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  typedef struct                           /* single string in RESOURCE       */
  {
     UCHAR  ucLength;
     CHAR   cString[1];

  }    PMST_STRING,
    * PPMST_STRING;

  typedef struct                           /* first part of RESOURCE SEGMENT  */
  {
     USHORT       usCodePage;
     PMST_STRING  pmstString;

  }    PMST_RES,
    * PPMST_RES;

  typedef union                            /* easy pointer movement           */
  {
     PPMST_RES    pR;
     PPMST_STRING pS;
     PCHAR        pC;
     PVOID        pV;

  }  PMST_UPTR;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Local variables                                                        ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  BOOL   bOK = BOOL_TRUE;               /* well, start as an OPTIMIST... */

  ULONG  ulI;                           /* current string table value */

  PMST_UPTR   P;                        /* multi-function POINTER */

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Set all STRING table pointers to NULL before starting...               ³*/
  /*³ (since we can't make any assumptions about state of passed table!)     ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  memset(pszTable, 0, ulTableItems * sizeof(pszTable[0]) );

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Process all of the string items (as defined by caller)                 ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  for( ulI = 0;                      /* Initialize: start at first string   */
       ulI < ulTableItems;           /* While:      more strings to process */
       ulI++)                        /* Iterate:    to the next string      */
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Need to load a new RESOURCE segment?                                 ³*/
    /*³                                                                      ³*/
    /*³ this happens if: (1) this is the first STRING being processed        ³*/
    /*³                  (2) all STRINGs from this SEGMENT have been         ³*/
    /*³                                                            processed ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    if ( (ulI % ulStringsPerRes) == 0)         /* all last segment processed? */
    {
      if (DosGetResource (hmodRes,                    /* from where */
                          RT_STRING,                  /* Res Type = STRINGTABLE */
                          (ulI / ulStringsPerRes) + 1, /* Res ID */
                          &P.pV) )                    /* put PTR here */
      {
        bOK = BOOL_FALSE;                             /* error detected... */
        break;
      }

      /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
      /*³ Locate first string in this new RESOURCE segment                   ³*/
      /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
      P.pS = &P.pR->pmstString;
    }

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Set address of this STRING in caller's table                         ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    pszTable[ulI] = &P.pS->cString[0];

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Move to the next STRING in this segment                              ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    P.pC += P.pS->ucLength + 1;
  }

  return( bOK );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SUBROUTINE_NAME  (FreeStringTable)                                       ³*/
/*³                                                                          ³*/
/*³                                                                          ³*/
/*³                       Module Level Design                                ³*/
/*³                                                                          ³*/
/*³ Function:                                                                ³*/
/*³     To free STRINGTABLE items previously loaded by "LoadStringTable"     ³*/
/*³     (that way, if we're the last process to reference them, they will be ³*/
/*³     freed (discarded from system memory)                                 ³*/
/*³                                                                          ³*/
/*³ Input:                                                                   ³*/
/*³     ULONG   ulTableItems     total # items in pszTable[]                 ³*/
/*³                                                                          ³*/
/*³     PSZ     pszTable[]       table to file with pointers to each string  ³*/
/*³                                                                          ³*/
/*³ Output:                                                                  ³*/
/*³     Returns  TRUE....if all strings freed                                ³*/
/*³                      - pszTable[] has been filled with NULL pointers     ³*/
/*³                                                                          ³*/
/*³              FALSE...if error was detected                               ³*/
/*³                                                                          ³*/
/*³ Logic:                                                                   ³*/
/*³                                                                          ³*/
/*³     result = TRUE (start as an OPTIMIST!)                                ³*/
/*³                                                                          ³*/
/*³     for(i = 0;                 Initialize: from first string             ³*/
/*³     |   i < usTableItesm       while:      more strings to go            ³*/
/*³     |   i += ItemsPerSegment   Interate:   move to the next seg's 1st    ³*/
/*³     |                                                              string³*/
/*³     |                                                                    ³*/
/*³     | set P = segment address of first string in this segment            ³*/
/*³     |                                                                    ³*/
/*³     | if ( DosFreeResource fails to "free" segment)                      ³*/
/*³     | | result = FALSE (at least ONE error was detected)                 ³*/
/*³                                                                          ³*/
/*³     set pszTable[] to all NULL pointers                                  ³*/
/*³                                                                          ³*/
/*³     return(result)                                                       ³*/
/*³                                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL FreeStringTable(ULONG   ulTableItems,
                     PSZ     pszTable[] )

{
  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Local variables                                                        ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
   BOOL   bOK = BOOL_TRUE;               /* well, start as an OPTIMIST... */

   ULONG  ulI;                           /* current string table value */

   PVOID  P;                             /* working POINTER */

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Process all of the string items (as defined by caller)                 ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  for( ulI = 0;                   /* Initialize: start at first string */
       ulI <  ulTableItems;       /* While:      more strings to process */
       ulI += ulStringsPerRes)    /* Iterate:    to the next seg's 1st string */
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Generate address with current SEGMENT, but a zero OFFSET (since      ³*/
    /*³ this is what DosGetResource2 returned to us when we loaded them!)    ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    P = &(pszTable[ulI]);

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Free the whole SEGMENT...                                            ³*/
    /*³                                                                      ³*/
    /*³ Note: we don't stop the loop if an error is detected, we just keep   ³*/
    /*³       on freeing the remaining STRINGS...(seems like best thing to   ³*/
    /*³       do in this scenario)                                           ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    if ( DosFreeResource( P ) != 0 )
    {
      bOK = BOOL_FALSE;                                  /* error detected... */
    }
  }

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Disable any further use of this STRING table by setting all pointers   ³*/
  /*³ to NULL...                                                             ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  memset(pszTable, 0, ulTableItems * sizeof(pszTable[0]) );

  return( bOK );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ ExitListRoutine                                                          ³*/
/*³                                                                          ³*/
/*³ - this routine is called during OS/2 exit list processing.  it is where  ³*/
/*³   ALL housekeeping/cleanup occurs...                                     ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#pragma linkage (ExitListRoutine, system)

VOID ExitListRoutine( ULONG ExitReason )
{
  SpyDeRegister( SpyInstance );       /* De-Register with our DLL */

  if ( hSwitch != NULLH )
     WinRemoveSwitchEntry(hSwitch);

  if ( hwndFrame != NULLH )
    WinDestroyWindow(hwndFrame);

  if ( hmq != NULLH )
    WinDestroyMsgQueue(hmq);

  /* cleanup STRINGTABLEs loaded for us... */

  FreeStringTable(sizeof(Strings) / sizeof(Strings[0]),  /* max # Strings[] */
                  Strings);                              /* start of string table */

  FreeStringTable(sizeof(Controls) / sizeof(Controls[0]), /* max # Controls[] */
                  Controls);                         /* start of string table */

  if ( hmodNLS != 0 )
    DosFreeModule(hmodNLS);

  if ( hab != 0 )
    WinTerminate(hab);

  DosExitList(EXLST_EXIT, NULL);                /* Indicate "done" */
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Main                                                                     ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
int main(int   argc,
         char *argv[] )
{
           QMSG   qmsg;

           ULONG  CtrlData = FCF_STANDARD ^ FCF_TASKLIST;

  static   CHAR   ClassName[] = "classPMSPY";  /* Window Class name */

           HWND   hwndClient;                  /* CLIENT area window handle          */

  /* First, establish our ExitList handler (just in case we crash!) */

  DosExitList(EXLST_ADD, &ExitListRoutine);

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Let's get SPYing...                                                    ³*/
  /*³                                                                        ³*/
  /*³ Programming Note: a DO...WHILE block is used here to simplify          ³*/
  /*³                   our logic, nesting, etc. that's required to          ³*/
  /*³                   handle any errors we detect during initialization.   ³*/
  /*³                                                                        ³*/
  /*³                   all that's needed is a BREAK statement to bail out   ³*/
  /*³                   of the DO...WHILE loop!                              ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

  BEGIN_DO_ONCE_BLOCK

    if ( (hab = WinInitialize(0)) == NULLH )
      break;

    ArgC = argc;
    ArgV = argv;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Get access to our NLS resource DLL once and for all                  ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    if (DosLoadModule(NULL, 0, "PMSPY", &hmodNLS) != 0 )
      EXIT_DO_ONCE_BLOCK;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Load all of our strings, once and for all                            ³*/
    /*³                                                                      ³*/
    /*³ - NLS enabled (externally visible) strings                           ³*/
    /*³ - non-NLS enabled strings (our internal control data)                ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    if ( !LoadStringTable(hmodNLS,    /* load STRINGS from .DLL */
                          sizeof(Strings) / sizeof(Strings[0]),/* max # Strings[] */
                          Strings) )  /* start of string table */
      EXIT_DO_ONCE_BLOCK;

    if ( !LoadStringTable(0,           /* load STRINGS from .EXE */
                          sizeof(Controls) / sizeof(Controls[0]),/* max # Strings[] */
                          Controls) )  /* start of string table */
      EXIT_DO_ONCE_BLOCK;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Create required PM resources                                         ³*/
    /*³                                                                      ³*/
    /*³ Note: the FRAME window is intentionally created INVISIBLE...         ³*/
    /*³                                                                      ³*/
    /*³       it is sized & positioned from (possibly) saved INI values      ³*/
    /*³       during WM_CREATE processing in the CLIENT window proc          ³*/
    /*³       (PMSPYWIN.C)                                                   ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    if ( (hmq = WinCreateMsgQueue(hab,
                                  atoi(Controls[CTL_MAX_PM_Q_SIZE]))) == NULLH)
      EXIT_DO_ONCE_BLOCK;

    WinRegisterClass(                       /* Register Window Class       */
                     hab,                   /* Anchor block handle         */
                     ClassName,             /* Window Class name           */
                     (PFNWP)SpyWindowProc,  /* Address of Window Procedure */
                     CS_SIZEREDRAW,         /* No special class style      */
                     sizeof(PVOID));        /* extra window words          */

    hwndFrame = WinCreateStdWindow(HWND_DESKTOP,  /* Desktop Window is parent */
                                   FS_ICON,
                                   &CtrlData,
                                   ClassName,          /* Window Class name      */
                                   Strings[IDS_TITLE],
                                   0L,                 /* Client style - visible */
                                   hmodNLS,            /* Module handle          */
                                   (USHORT)ID_MAINWND, /* Window ID              */
                                   &hwndClient);       /* Client Window handle   */

    if (hwndFrame == NULLH)
      EXIT_DO_ONCE_BLOCK;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Process the PM Message queue                                         ³*/
    /*³                                                                      ³*/
    /*³ - get the next Msg                                                   ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    while ( WinGetMsg(hab,       /* Required anchor block */
                      &qmsg,     /* Addr of msg structure */
                      NULLH,     /* Filter window (none)  */
                      0,         /* Filter begin    "     */
                      0) )       /* Filter end      "     */
    {
      WinDispatchMsg( hab, &qmsg );
    }

  END_DO_ONCE_BLOCK;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ NOTE: all cleanup is now done in the ExitList routine...               ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  DosExit(EXIT_PROCESS, 0);

  return(0);            /* satisfy compiler's requirement for main() 'return' */
}
