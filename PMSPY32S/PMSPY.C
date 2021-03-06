/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*�                                                                          �*/
/*� PROGRAM NAME: PMSPY                                                      �*/
/*� -------------                                                            �*/
/*�  A PM program that is used to look at or 'spy' on the message queue of   �*/
/*�  other PM applications windows.                                          �*/
/*�                                                                          �*/
/*� COPYRIGHT:                                                               �*/
/*� ----------                                                               �*/
/*�  Copyright (C) International Business Machines Corp., 1992               �*/
/*�                                                                          �*/
/*� DISCLAIMER OF WARRANTIES:                                                �*/
/*� -------------------------                                                �*/
/*�  The following [enclosed] code is sample code created by IBM Corporation.�*/
/*�  This sample code is not part of any standard IBM product and is provided�*/
/*�  to you solely for the purpose of assisting you in the development of    �*/
/*�  your applications.  The code is provided "AS IS", without warranty of   �*/
/*�  any kind.  IBM shall not be liable for any damages arising out of your  �*/
/*�  use of the sample code, even if they have been advised of the           �*/
/*�  possibility of such damages.                                            �*/
/*�                                                                          �*/
/*� REVISION LEVEL: 1.0                                                      �*/
/*� ---------------                                                          �*/
/*�                                                                          �*/
/*� WHAT THIS PROGRAM DOES:                                                  �*/
/*� -----------------------                                                  �*/
/*�  This program uses a system APIs to 'hook' into application queues in    �*/
/*�  order to display the messages being received by that application.  This �*/
/*�  is very useful when debugging PM applications where the order of        �*/
/*�  messages and/or whether or not they are actually received makes a       �*/
/*�  difference while debugging.                                             �*/
/*�                                                                          �*/
/*� WHAT THIS PROGRAM DEMONSTRATES:                                          �*/
/*� -------------------------------                                          �*/
/*�  This program demonstrates how to 'hook' into PM using the system APIS   �*/
/*�  in order to see what messages an application is receiving.              �*/
/*�                                                                          �*/
/*� WHAT YOU NEED TO COMPILE THIS PROGRAM:                                   �*/
/*� --------------------------------------                                   �*/
/*�                                                                          �*/
/*�  REQUIRED FILES:                                                         �*/
/*�  ---------------                                                         �*/
/*�                                                                          �*/
/*�  For PMSPY.EXE...                                                        �*/
/*�    PMSPY    C     - main source file                                     �*/
/*�    PMSPY    H     - main PMSPY header file used by many of the files     �*/
/*�    PMSPYABT C     - procedure to display logo or 'about' screen          �*/
/*�    PMSPYFIL C     - message filter dialog procedure                      �*/
/*�    PMSPYFON C     - font selection dialog procedure                      �*/
/*�    PMSPYGRP C     - group message filter dialog procedure                �*/
/*�    PMSPYIPF C     - IPF processing related code                          �*/
/*�    PMSPYIPF H     - IPF panel id header file                             �*/
/*�    PMSPYMSG C     - MSG and GROUP data structure support routines        �*/
/*�    PMSPYNOT C     - 'notes' dialog procedure                             �*/
/*�    PMSPYOW  C     - object window selection dialog procedure             �*/
/*�    PMSPYPRO C     - profile manipulation routines                        �*/
/*�    PMSPYUTL C     - miscellaneous utility routines                       �*/
/*�    PMSPYWIN C     - primary PMSPY client area window procedure           �*/
/*�    PMSPY    DEF   - module definition file                               �*/
/*�    PMSPY    ICO   - icon file                                            �*/
/*�    PMSPY    MAK   - make file                                            �*/
/*�    PMSPY    PTR   - pointer file (magnifying glass)                      �*/
/*�    PMSPY    RC    - resource file                                        �*/
/*�    PMSPYRC  H     - resource id header file                              �*/
/*�    PMSPYDEF PRO   - pmspy profile                                        �*/
/*�    PMSPY    IPF   - doc file that get built into HLP file                �*/
/*�                                                                          �*/
/*�  For PMSPYDEF.EXE...                                                     �*/
/*�    PMSPYDEF C     - main source file to build PMSPY (default) profiles   �*/
/*�                                                                          �*/
/*�  For PMSPYHK.DLL...                                                      �*/
/*�    PMSPYHK  C     - main source file for DLL that 'hooks' into system    �*/
/*�    PMSPYHK  DEF   - module definition file for PMSPYHK                   �*/
/*�    PMSPYHK  H     - header file for PMSPYHK                              �*/
/*�                                                                          �*/
/*�  For PMSPYNLS.DLL...                                                     �*/
/*�    PMSPYNLS C     - stub file for creating NLS DLL                       �*/
/*�    PMSPYNLS DEF   - module definition file for PMSPYNLS                  �*/
/*�    PMSPYNLS RC    - resource file                                        �*/
/*�                                                                          �*/
/*�  For SPY-PROF.EXE...                                                     �*/
/*�    SPY-PROF C     - source for program used to build application specific�*/
/*�                     profile files                                        �*/
/*�    SPY-PROF MAK   - SPY-PROF make file                                   �*/
/*�                                                                          �*/
/*�  REQUIRED LIBRARIES:                                                     �*/
/*�  -------------------                                                     �*/
/*�                                                                          �*/
/*�    OS2386.LIB     - OS/2 API library                                     �*/
/*�                                                                          �*/
/*�  REQUIRED PROGRAMS:                                                      �*/
/*�  ------------------                                                      �*/
/*�                                                                          �*/
/*�    IBM C Set/2 Compiler (icc.exe)                                        �*/
/*�    IBM Linker (link386.exe)                                              �*/
/*�    Resource Compiler (rc.exe)                                            �*/
/*�                                                                          �*/
/*� EXPECTED INPUT:                                                          �*/
/*� ---------------                                                          �*/
/*�                                                                          �*/
/*�    You can start the program minimized by using the /MIN option.         �*/
/*�                                                                          �*/
/*� EXPECTED OUTPUT:                                                         �*/
/*� ----------------                                                         �*/
/*�                                                                          �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� PMSPY.C                                                                  �*/
/*�                                                                          �*/
/*� Program to spy on other windows message queues                           �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� Includes                                                                 �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
#define DEFINE_VARIABLES

#define INCL_DOSPROCESS                 /* need DosExitList stuff....         */

#include "pmspy.h"                      /* Resource symbolic identifiers      */

static HAB      hab           = NULLH;
static HMQ      hmq           = NULLH;

static HWND     hwndFrame     = NULLH;         /* FRAME window handle                */

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� SUBROUTINE_NAME  (LoadStringTable)                                       �*/
/*�                                                                          �*/
/*�                                                                          �*/
/*�                       Module Level Design                                �*/
/*�                                                                          �*/
/*� Function:                                                                �*/
/*�     To load STRINGTABLE items from their disk resource and place pointers�*/
/*�     to them in passed string table array.                                �*/
/*�                                                                          �*/
/*�     Because DosGetResourc2 is used, we get some special benefits:        �*/
/*�                                                                          �*/
/*�     (1) only one I/O operation is required for each 16 strings (much     �*/
/*�         better than DosGetResource or WinLoadString...)                  �*/
/*�                                                                          �*/
/*�     (2) the STRINGTABLE resource segment is now SHARED among all programs�*/
/*�         that access the same STRINGTABLE segment concurrently, memory    �*/
/*�         consumption by multiple instances of the same program is reduced �*/
/*�         as well as speeding up their initialization phase (because the   �*/
/*�         segment is already present in memory and OS/2 doesn't read it    �*/
/*�         from disk again (i hope!)                                        �*/
/*�                                                                          �*/
/*� Input:                                                                   �*/
/*�     HMODULE hmodRes         tells where to load STRINGTABLE resource from�*/
/*�                              - NULL      = use .EXE                      �*/
/*�                              - Otherwise = value from WinLoadModule()    �*/
/*�                                                                          �*/
/*�     ULONG   usTableItems     total # items in pszTable[]                 �*/
/*�                                                                          �*/
/*�     PSZ     pszTable[]       table to file with pointers to each string  �*/
/*�                                                                          �*/
/*� Output:                                                                  �*/
/*�     Returns  TRUE....if all strings loaded                               �*/
/*�                      - pszTable[] has been filled with pointers to       �*/
/*�                        items in the resource segment(s)                  �*/
/*�                      - use FreeStringTable to cleanup pszTable[]         �*/
/*�                                                                          �*/
/*�              FALSE...if error was detected                               �*/
/*�                                                                          �*/
/*� Logic:                                                                   �*/
/*�                                                                          �*/
/*�     for(i = 0;             Initialize: from first string                 �*/
/*�     |   i < usTableItesm   while:      more strings to go                �*/
/*�     |   i++)               Interate:   move to the next string           �*/
/*�     |                                                                    �*/
/*�     | if (this string is in a new RESOURCE segment)                      �*/
/*�     | |  call DosGetResource2 to "get" it                                �*/
/*�     | |  set P = address(first string in new segment)                    �*/
/*�     |                                                                    �*/
/*�     | set pszTable[i] = P + 1     real string starts at byte after length�*/
/*�     |                                                                    �*/
/*�     | P = P + *P + 1              point at next string in segment        �*/
/*�     |                                                                    �*/
/*�                                                                          �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
static ULONG  ulStringsPerRes = 16;    /* # strings in each RESOURCE segment  */

BOOL LoadStringTable(HMODULE hmodRes,
                     ULONG   ulTableItems,
                     PSZ     pszTable[] )

{
  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Local structure definitions                                            �*/
  /*�                                                                        �*/
  /*� - these items define templates for overlaying the RESOURCE segment     �*/
  /*�   read by DosGetResource/DosGetResource2                               �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
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

  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Local variables                                                        �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
  BOOL   bOK = BOOL_TRUE;               /* well, start as an OPTIMIST... */

  ULONG  ulI;                           /* current string table value */

  PMST_UPTR   P;                        /* multi-function POINTER */

  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Set all STRING table pointers to NULL before starting...               �*/
  /*� (since we can't make any assumptions about state of passed table!)     �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
  memset(pszTable, 0, ulTableItems * sizeof(pszTable[0]) );

  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Process all of the string items (as defined by caller)                 �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
  for( ulI = 0;                      /* Initialize: start at first string   */
       ulI < ulTableItems;           /* While:      more strings to process */
       ulI++)                        /* Iterate:    to the next string      */
  {
    /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
    /*� Need to load a new RESOURCE segment?                                 �*/
    /*�                                                                      �*/
    /*� this happens if: (1) this is the first STRING being processed        �*/
    /*�                  (2) all STRINGs from this SEGMENT have been         �*/
    /*�                                                            processed �*/
    /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
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

      /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
      /*� Locate first string in this new RESOURCE segment                   �*/
      /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
      P.pS = &P.pR->pmstString;
    }

    /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
    /*� Set address of this STRING in caller's table                         �*/
    /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
    pszTable[ulI] = &P.pS->cString[0];

    /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
    /*� Move to the next STRING in this segment                              �*/
    /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
    P.pC += P.pS->ucLength + 1;
  }

  return( bOK );
}

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� SUBROUTINE_NAME  (FreeStringTable)                                       �*/
/*�                                                                          �*/
/*�                                                                          �*/
/*�                       Module Level Design                                �*/
/*�                                                                          �*/
/*� Function:                                                                �*/
/*�     To free STRINGTABLE items previously loaded by "LoadStringTable"     �*/
/*�     (that way, if we're the last process to reference them, they will be �*/
/*�     freed (discarded from system memory)                                 �*/
/*�                                                                          �*/
/*� Input:                                                                   �*/
/*�     ULONG   ulTableItems     total # items in pszTable[]                 �*/
/*�                                                                          �*/
/*�     PSZ     pszTable[]       table to file with pointers to each string  �*/
/*�                                                                          �*/
/*� Output:                                                                  �*/
/*�     Returns  TRUE....if all strings freed                                �*/
/*�                      - pszTable[] has been filled with NULL pointers     �*/
/*�                                                                          �*/
/*�              FALSE...if error was detected                               �*/
/*�                                                                          �*/
/*� Logic:                                                                   �*/
/*�                                                                          �*/
/*�     result = TRUE (start as an OPTIMIST!)                                �*/
/*�                                                                          �*/
/*�     for(i = 0;                 Initialize: from first string             �*/
/*�     |   i < usTableItesm       while:      more strings to go            �*/
/*�     |   i += ItemsPerSegment   Interate:   move to the next seg's 1st    �*/
/*�     |                                                              string�*/
/*�     |                                                                    �*/
/*�     | set P = segment address of first string in this segment            �*/
/*�     |                                                                    �*/
/*�     | if ( DosFreeResource fails to "free" segment)                      �*/
/*�     | | result = FALSE (at least ONE error was detected)                 �*/
/*�                                                                          �*/
/*�     set pszTable[] to all NULL pointers                                  �*/
/*�                                                                          �*/
/*�     return(result)                                                       �*/
/*�                                                                          �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
BOOL FreeStringTable(ULONG   ulTableItems,
                     PSZ     pszTable[] )

{
  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Local variables                                                        �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
   BOOL   bOK = BOOL_TRUE;               /* well, start as an OPTIMIST... */

   ULONG  ulI;                           /* current string table value */

   PVOID  P;                             /* working POINTER */

  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Process all of the string items (as defined by caller)                 �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
  for( ulI = 0;                   /* Initialize: start at first string */
       ulI <  ulTableItems;       /* While:      more strings to process */
       ulI += ulStringsPerRes)    /* Iterate:    to the next seg's 1st string */
  {
    /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
    /*� Generate address with current SEGMENT, but a zero OFFSET (since      �*/
    /*� this is what DosGetResource2 returned to us when we loaded them!)    �*/
    /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
    P = &(pszTable[ulI]);

    /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
    /*� Free the whole SEGMENT...                                            �*/
    /*�                                                                      �*/
    /*� Note: we don't stop the loop if an error is detected, we just keep   �*/
    /*�       on freeing the remaining STRINGS...(seems like best thing to   �*/
    /*�       do in this scenario)                                           �*/
    /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
    if ( DosFreeResource( P ) != 0 )
    {
      bOK = BOOL_FALSE;                                  /* error detected... */
    }
  }

  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Disable any further use of this STRING table by setting all pointers   �*/
  /*� to NULL...                                                             �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
  memset(pszTable, 0, ulTableItems * sizeof(pszTable[0]) );

  return( bOK );
}

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� ExitListRoutine                                                          �*/
/*�                                                                          �*/
/*� - this routine is called during OS/2 exit list processing.  it is where  �*/
/*�   ALL housekeeping/cleanup occurs...                                     �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
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

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� Main                                                                     �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
int main(int   argc,
         char *argv[] )
{
           QMSG   qmsg;

           ULONG  CtrlData = FCF_STANDARD ^ FCF_TASKLIST;

  static   CHAR   ClassName[] = "classPMSPY";  /* Window Class name */

           HWND   hwndClient;                  /* CLIENT area window handle          */

  /* First, establish our ExitList handler (just in case we crash!) */

  DosExitList(EXLST_ADD, &ExitListRoutine);

  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Let's get SPYing...                                                    �*/
  /*�                                                                        �*/
  /*� Programming Note: a DO...WHILE block is used here to simplify          �*/
  /*�                   our logic, nesting, etc. that's required to          �*/
  /*�                   handle any errors we detect during initialization.   �*/
  /*�                                                                        �*/
  /*�                   all that's needed is a BREAK statement to bail out   �*/
  /*�                   of the DO...WHILE loop!                              �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

  BEGIN_DO_ONCE_BLOCK

    if ( (hab = WinInitialize(0)) == NULLH )
      break;

    ArgC = argc;
    ArgV = argv;

    /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
    /*� Get access to our NLS resource DLL once and for all                  �*/
    /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
    if (DosLoadModule(NULL, 0, "PMSPY", &hmodNLS) != 0 )
      EXIT_DO_ONCE_BLOCK;

    /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
    /*� Load all of our strings, once and for all                            �*/
    /*�                                                                      �*/
    /*� - NLS enabled (externally visible) strings                           �*/
    /*� - non-NLS enabled strings (our internal control data)                �*/
    /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
    if ( !LoadStringTable(hmodNLS,    /* load STRINGS from .DLL */
                          sizeof(Strings) / sizeof(Strings[0]),/* max # Strings[] */
                          Strings) )  /* start of string table */
      EXIT_DO_ONCE_BLOCK;

    if ( !LoadStringTable(0,           /* load STRINGS from .EXE */
                          sizeof(Controls) / sizeof(Controls[0]),/* max # Strings[] */
                          Controls) )  /* start of string table */
      EXIT_DO_ONCE_BLOCK;

    /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
    /*� Create required PM resources                                         �*/
    /*�                                                                      �*/
    /*� Note: the FRAME window is intentionally created INVISIBLE...         �*/
    /*�                                                                      �*/
    /*�       it is sized & positioned from (possibly) saved INI values      �*/
    /*�       during WM_CREATE processing in the CLIENT window proc          �*/
    /*�       (PMSPYWIN.C)                                                   �*/
    /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
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

    /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
    /*� Process the PM Message queue                                         �*/
    /*�                                                                      �*/
    /*� - get the next Msg                                                   �*/
    /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
    while ( WinGetMsg(hab,       /* Required anchor block */
                      &qmsg,     /* Addr of msg structure */
                      NULLH,     /* Filter window (none)  */
                      0,         /* Filter begin    "     */
                      0) )       /* Filter end      "     */
    {
      WinDispatchMsg( hab, &qmsg );
    }

  END_DO_ONCE_BLOCK;

  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� NOTE: all cleanup is now done in the ExitList routine...               �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
  DosExit(EXIT_PROCESS, 0);

  return(0);            /* satisfy compiler's requirement for main() 'return' */
}
