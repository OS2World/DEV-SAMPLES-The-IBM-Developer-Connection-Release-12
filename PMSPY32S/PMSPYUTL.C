/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё                                                                          Ё*/
/*Ё PROGRAM NAME: PMSPY                                                      Ё*/
/*Ё -------------                                                            Ё*/
/*Ё  A PM program that is used to look at or 'spy' on the message queue of   Ё*/
/*Ё  other PM applications windows.                                          Ё*/
/*Ё                                                                          Ё*/
/*Ё COPYRIGHT:                                                               Ё*/
/*Ё ----------                                                               Ё*/
/*Ё  Copyright (C) International Business Machines Corp., 1992               Ё*/
/*Ё                                                                          Ё*/
/*Ё DISCLAIMER OF WARRANTIES:                                                Ё*/
/*Ё -------------------------                                                Ё*/
/*Ё  The following [enclosed] code is sample code created by IBM Corporation.Ё*/
/*Ё  This sample code is not part of any standard IBM product and is providedЁ*/
/*Ё  to you solely for the purpose of assisting you in the development of    Ё*/
/*Ё  your applications.  The code is provided "AS IS", without warranty of   Ё*/
/*Ё  any kind.  IBM shall not be liable for any damages arising out of your  Ё*/
/*Ё  use of the sample code, even if they have been advised of the           Ё*/
/*Ё  possibility of such damages.                                            Ё*/
/*Ё                                                                          Ё*/
/*Ё For details on what this program does etc., please see the PMSPY.C file. Ё*/
/*Ё                                                                          Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё PMSPYUTL.C                                                               Ё*/
/*Ё                                                                          Ё*/
/*Ё Utility routines                                                         Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Includes                                                                 Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

#include "pmspy.h"                      /* Resource symbolic identifiers*/

#include "pmspyMM.h"                    /* MMPM/2 header file           */

#include <ctype.h>

typedef struct {                        /* Memory Allocation control structure */

                 ULONG  nAllocs,
                        sAllocs,
                        nFrees;
               } MEM_DATA, *PMEM_DATA;

static MEM_DATA memStats = { 0, 0, 0};

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Routine:    AllocateMemory                                               Ё*/
/*Ё                                                                          Ё*/
/*Ё Function:   Allocate dynamic memory (also, sets to all NULLs)            Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
PVOID AllocateMemory(size_t SizeWanted )

{
  PVOID p;

  /*** Attempt to allocate the requested memory ***/

  if ( (p = calloc( SizeWanted, 1)) == NULL)
  {
    MsgDisplay(HWND_DESKTOP,
               Strings[IDS_TITLE],
               Strings[IDS_MSG_ALLOCATE],
               0,
               MB_CUAWARNING | MB_CANCEL,
               SizeWanted,
               memStats.nAllocs,
               memStats.sAllocs,
               memStats.nFrees);
  }
  else
  {
    /*** Update statistics ***/
    memStats.nAllocs++;
    memStats.sAllocs += SizeWanted;
  }

  return( p );
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Routine:    FreeMemory                                                   Ё*/
/*Ё                                                                          Ё*/
/*Ё Function:   Free memory obtained from AllocateMemory                     Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

VOID FreeMemory(register PVOID p )

{
   if (p == NULL)
   {
    MsgDisplay(HWND_DESKTOP,
               Strings[IDS_TITLE],
               Strings[IDS_MSG_FREE],
               0,
               MB_CUAWARNING | MB_CANCEL,
               p,
               memStats.nAllocs,
               memStats.sAllocs,
               memStats.nFrees);
   }
   else
   {
     memStats.nFrees++;
     free( p );
   } /* endif */
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Routine:    QueryMemory                                                  Ё*/
/*Ё                                                                          Ё*/
/*Ё Function:   Query memory statistics                                      Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

VOID QueryMemory(register PMEM_DATA p )

{
  *p = memStats;                /* return COPY to caller */
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Routine:    MsgDisplay                                                   Ё*/
/*Ё                                                                          Ё*/
/*Ё Function:   Display MessageBox with variable substitution                Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
static BOOL bEnableHELP = BOOL_FALSE;          /* add "help" button?          */

ULONG MsgDisplay( HWND   hWnd,
                  PSZ    strCap,
                  PSZ    strFormat,
                  USHORT mb_id,
                  ULONG  style,
                  ...)                         /* strFormat insert data       */
{
  CHAR          MessageText[512];              /* Message box output          */

  va_list       arg_ptr;                       /* Variable argument list ptr  */

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Ensure "Help" button present if IPF initialized OK                     Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  if ( bEnableHELP )
    style |= MB_HELP;

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё  Format the specified message (insert message variables, etc)          Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  va_start(arg_ptr, style);                     /* Start variable list access */

  vsprintf(MessageText,                         /*  Buffer to put it in       */
           strFormat,                           /*  Format string             */
           arg_ptr);

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё  Display the formatted message & return user's response                Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  return( WinMessageBox(HWND_DESKTOP,           /*   Desktop handle           */
                        hWnd,                   /*   Client window handle     */
                        MessageText,            /*   message text             */
                        strCap,                 /*   Caption of Box           */
                        mb_id,                  /*   ID of message box (Help) */
                        style | MB_MOVEABLE) ); /*   Style of message box     */
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Routine:    MsgDisplay                                                   Ё*/
/*Ё                                                                          Ё*/
/*Ё Function:   Display MessageBox with variable substitution                Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
VOID MsgDisplayShouldEnableHELP( VOID )

{
  bEnableHELP = BOOL_TRUE;

}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Routine: ConvertToNumber                                                 Ё*/
/*Ё ------------------------------------------------------------------------ Ё*/
/*Ё                                                                          Ё*/
/*Ё Purpose:   Convert an Ascii text string to its binary value using        Ё*/
/*Ё            C language format conventions                                 Ё*/
/*Ё                                                                          Ё*/
/*Ё            a) decimal          dddd                                      Ё*/
/*Ё            b) octal           0dddd                                      Ё*/
/*Ё            c) hexidecimal    0xdddd                                      Ё*/
/*Ё                                                                          Ё*/
/*Ё            Complete Format: [<white space>]<number>                      Ё*/
/*Ё                                                                          Ё*/
/*Ё                                                                          Ё*/
/*Ё Prototype: BOOL ConvertToNumber(PSZ   pAscii,                            Ё*/
/*Ё                                 CTN  *pNumber,                           Ё*/
/*Ё                                 CTN   lMin,                              Ё*/
/*Ё                                 CTN   lMax);                             Ё*/
/*Ё                                                                          Ё*/
/*Ё Entry:     pAscii.....pointer to NULL terminated string to attempt to    Ё*/
/*Ё                       convert to binary                                  Ё*/
/*Ё            pNumeric...pointer to variable to receive converted value     Ё*/
/*Ё            lMim.......minimum acceptable range value (inclusive)         Ё*/
/*Ё            lMax.......maximum acceptable range value (inclusive)         Ё*/
/*Ё                                                                          Ё*/
/*Ё Exit:      TRUE.......conversion was successful                          Ё*/
/*Ё                       ==> *pNumber contains converted value              Ё*/
/*Ё                                                                          Ё*/
/*Ё            FALSE......conversion was not successful                      Ё*/
/*Ё                       ==> *pNumber is indeterminate                      Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

BOOL ConvertToNumber(PSZ     pAscii,
                     PUSHORT pNumber,
                     USHORT  lMin,
                     USHORT  lMax)

{
  register BOOL  fOK = BOOL_FALSE;            /* start as a pessimist! */
           PSZ   pEnd;                        /* locates last char converted */

           ULONG ulConversion = strtoul(pAscii,
                                        &pEnd,
                                        0);     /* figure base out */

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Determine if conversion was successful                                 Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

  if (*pEnd == 0)
  {
    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Conversion was successful, is number within allowed range?           Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

    *pNumber = LOUSHORT(ulConversion); /* convert from 32 to 16 bits */

    if ( (*pNumber >= lMin) && (*pNumber <= lMax) )
      fOK = BOOL_TRUE;
  }

  return( fOK );
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Routine: OpenPDTR                                                        Ё*/
/*Ё -------------------------------------------------------------------------Ё*/
/*Ё                                                                          Ё*/
/*Ё Purpose:   Open a Programmer Defined Text Resource (PDTR) for later      Ё*/
/*Ё                                                                  READing Ё*/
/*Ё Prototype: PPDTR_DATA openPDTR(PPDTR_DATA pData)                         Ё*/
/*Ё                                                                          Ё*/
/*Ё Entry:     pData......pointer to PDTR control structure                  Ё*/
/*Ё                                                                          Ё*/
/*Ё Exit:      !NULL......Open was successful                                Ё*/
/*Ё                       ==> ready to Read/Eof/Close                        Ё*/
/*Ё                                                                          Ё*/
/*Ё             NULL......Open failed                                        Ё*/
/*Ё                       ==> probable causes: - invalid hModule, typePDTR,  Ё*/
/*Ё                                              or idPDTR value specified   Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
PPDTR_DATA OpenPDTR(register PPDTR_DATA pData)

{
  register BOOL bOK = BOOL_FALSE;
  PVOID    pVoid;

  if (DosGetResource(pData->hModule,          /* Who to get from    */
                     pData->typePDTR,         /* What Resource Type */
                     pData->idPDTR,           /* What Resource ID   */
                     &pVoid) == 0)            /* Where PTR placed   */
  {
    pData->pTOF    =
    pData->pRead   =
    pData->offPDTR = pVoid;

    bOK = BOOL_TRUE;
  }

  return(bOK ? pData : NULL);
}

#define PDTR_EOF 0x1A                  /* End-Of-File byte */
#define PDTR_EOL 0xD                   /* End-Of-Line byte */

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Routine: EofPDTR                                                         Ё*/
/*Ё --------------------------------------------------------------------     Ё*/
/*Ё                                                                          Ё*/
/*Ё Purpose:   Determine if Open Programmer Defined Text Resource (PDTR)     Ё*/
/*Ё            is at End-Of-File                                             Ё*/
/*Ё                                                                          Ё*/
/*Ё Prototype: BOOL EofPDTR(PPDTR_DATA pData)                                Ё*/
/*Ё                                                                          Ё*/
/*Ё Entry:     pData......pointer to PDTR control structure                  Ё*/
/*Ё                                                                          Ё*/
/*Ё Exit:      TRUE.......File is at EOF                                     Ё*/
/*Ё                                                                          Ё*/
/*Ё            FALSE......File is not at EOF                                 Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

BOOL EofPDTR(register PPDTR_DATA pData)

{
  return( *pData->pRead == PDTR_EOF ? BOOL_TRUE : BOOL_FALSE );
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Routine: GetsPDTR                                                        Ё*/
/*Ё -------------------------------------------------------------------------Ё*/
/*Ё                                                                          Ё*/
/*Ё Purpose:   Get String from an Open Programmer Defined Text Resource(PDTR)Ё*/
/*Ё                                                                          Ё*/
/*Ё Prototype: PSZ GetsPDTR(PPDTR_DATA pData, PSZ pString, USHORT lString)   Ё*/
/*Ё                                                                          Ё*/
/*Ё Entry:     pData......pointer to PDTR control structure                  Ё*/
/*Ё            pString....pointer to area to put string                      Ё*/
/*Ё            lString....length of pString area (must be ONE or more!)      Ё*/
/*Ё                                                                          Ё*/
/*Ё Exit:      NULL.......File is at EOF                                     Ё*/
/*Ё           !NULL.......entry pString value                                Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё PDTR format: <text....><CR><LF>                                          Ё*/
/*Ё              <text....><CR><LF>                                          Ё*/
/*Ё              <text....><CR><LF>                                          Ё*/
/*Ё              <EOF>                                                       Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

PSZ GetsPDTR(         PPDTR_DATA pData,
                      PSZ        pString,
             register USHORT     lString)

{
  register PSZ pWork;                           /* work in pString area */

  /* if at EOF, quit right away */

  if ( *pData->pRead == PDTR_EOF )
    return( NULL );

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Copy line from SEGMENT until: 1) EOL reached                           Ё*/
  /*Ё                               2) pString area filled                   Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  for(/* Init */ pWork = pString;               /* where to put string */

      /* Term */ (*pData->pRead != PDTR_EOL) && /* quit @ EOL */
                 (lString       != 1);          /* or no room left */

      /* Iter */ pWork++,                       /* move to next string place */
                 pData->pRead++,                /* move READ pointer */
                 lString--                      /* adjust remaining size */
     )
    *pWork = *pData->pRead;

  pData->pRead += 2;   /* skip CR/LF */

  *pWork = 0;       /* make NULL terminated.... */

  return( pString );
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Routine: ClosePDTR                                                       Ё*/
/*Ё ----------------------------------------------------------------         Ё*/
/*Ё                                                                          Ё*/
/*Ё Purpose:   Close an Open Programmer Defined Text Resource (PDTR)         Ё*/
/*Ё                                                                          Ё*/
/*Ё Prototype: BOOL ClosePDTR(PPDTR_DATA pData)                              Ё*/
/*Ё                                                                          Ё*/
/*Ё Entry:     pData......pointer to PDTR control structure                  Ё*/
/*Ё                                                                          Ё*/
/*Ё Exit:      0..........PDTR close successful                              Ё*/
/*Ё                                                                          Ё*/
/*Ё            EOF........PDTR close unsuccessful                            Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

int ClosePDTR(register PPDTR_DATA pData)

{
  return (DosFreeResource (pData->offPDTR) == 0 ? 0 : EOF);
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Routine: FindWordInList                                                  Ё*/
/*Ё ----------------------------------------------------------------------   Ё*/
/*Ё                                                                          Ё*/
/*Ё Purpose:   Find a NULL terminated "word" in a "word list"                Ё*/
/*Ё                                                                          Ё*/
/*Ё Prototype: USHORT FindWordInList(PSZ pWord, PWORD_LIST pWordList);       Ё*/
/*Ё                                                                          Ё*/
/*Ё Entry:     pWord......pointer to WORD to look for in pWordList           Ё*/
/*Ё            pWordList..pointer to WORD list                               Ё*/
/*Ё                                                                          Ё*/
/*Ё Exit:      'value' for "word" (or value from End-Of-List marker saying   Ё*/
/*Ё            that the "word" was not found                                 Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

USHORT FindWordInList(PSZ pWord, PWORD_LIST pWordList)

{
  for( /* Init  */   /**** N O N E ****/;

       /* While */   (pWordList->ppWord != NULL) &&
                     (stricmp(pWord, *pWordList->ppWord) != 0);

       /* Iter  */   pWordList++ );

  return(pWordList->vWord );

}


/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Routine: UpdateSpyTitle                                                  Ё*/
/*Ё --------------------------------------------------------                 Ё*/
/*Ё                                                                          Ё*/
/*Ё Purpose:   Update the title of the PMSPY frame                           Ё*/
/*Ё                                                                          Ё*/
/*Ё Prototype: VOID UpdateSpyTitle( HWND         hwndFrame,                  Ё*/
/*Ё                                 SPY_TITLES   whichTitle,                 Ё*/
/*Ё                                 ...);                                    Ё*/
/*Ё                                                                          Ё*/
/*Ё Entry:     hwndFrame....frame window                                     Ё*/
/*Ё            whichTitle...desired title mode                               Ё*/
/*Ё                                                                          Ё*/
/*Ё Exit:      NONE, except title in frame updated                           Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

VOID UpdateSpyTitle(HWND         hwndFrame,
                    SPY_TITLES   whichTitle,
                    ...)
{
  CHAR          szText[256];

  va_list       arg_ptr;                /* Variable argument list ptr */

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Locate start of variable parm(s) passed to us                          Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  va_start(arg_ptr, whichTitle);        /* Start variable list access */

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Generate the new PMSPY title                                           Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  switch( whichTitle )
  {
    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Display information about the "window" the POINTER is currently over Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case tSpyWho:
         vsprintf(szText, Strings[IDS_FMT_PTR_QUEUE], arg_ptr);
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Change title to display "window" being SPYed                         Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case tWindow:
         vsprintf(szText, Strings[IDS_FMT_SPY_WINDOW], arg_ptr);
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Change title to display "object" being SPYed                         Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case tObject:
         vsprintf(szText, Strings[IDS_FMT_SPY_OBJECT], arg_ptr);
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Change title to display application "queue" being SPYed              Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case tQueue:
         vsprintf(szText, Strings[IDS_FMT_SPY_QUEUE], arg_ptr);
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Change title to display that SYSTEM "queue" being SPYed              Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case tQueueSystem:
         vsprintf(szText, Strings[IDS_FMT_SPY_QUEUE_SYSTEM], arg_ptr);
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Change title back to PMSPY standard                                  Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    default:
         strcpy(szText, swcntrl.szSwtitle);
  }

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Update the PMSPY title                                                 Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  WinSetWindowText(hwndFrame, szText);

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё LOG the new PMSPY title, if we need to                                 Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  if ( bCurrentlyLogging )
    fprintf(pLogFile, "%s\n", szText);
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Routine: TranslateClassName                                              Ё*/
/*Ё ------------------------------------------------------------------------ Ё*/
/*Ё                                                                          Ё*/
/*Ё Purpose:  Translate WinQueryClassName to usable WC_* string              Ё*/
/*Ё                                                                          Ё*/
/*Ё Prototype: PSZ TranslateClassName(PSZ pszClassName)                      Ё*/
/*Ё                                                                          Ё*/
/*Ё Entry:     pszClassName....name from WinQueryClassName                   Ё*/
/*Ё                                                                          Ё*/
/*Ё Returns:   if a standard WC_* class, then pointer to usable string       Ё*/
/*Ё                                      else pointer to original class name Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

PSZ TranslateClassName(PSZ pszClassName)

{

           PSZ    pszTranslate;        /* final, translated class name */
  register USHORT i;

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Translate table of WinClassName value to WC_* strings                    Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

static struct
{
  PSZ    pszClassName,            /* binary WC_xxxx value                     */
         pszWC;                   /* human usable text equivalent             */

  CHAR   szClassName[8];          /* "#nnn" version of binary WC_xxxx value   */

} Class2WC[] =

{
  {WC_BUTTON            ,"WC_BUTTON"             },

#ifdef WC_CIRCULARSLIDER
  {WC_CIRCULARSLIDER    ,"WC_CIRCULARSLIDER"     },      /* MMPM/2 control */
#endif

  {WC_COMBOBOX          ,"WC_COMBOBOX"           },
  {WC_CONTAINER         ,"WC_CONTAINER"          },
  {WC_ENTRYFIELD        ,"WC_ENTRYFIELD"         },
  {WC_FRAME             ,"WC_FRAME"              },

#ifdef WC_GRAPHICBUTTON
  {WC_GRAPHICBUTTON     ,"WC_GRAPHICBUTTON"      },      /* MMPM/2 control */
#endif

  {WC_LISTBOX           ,"WC_LISTBOX"            },
  {WC_MENU              ,"WC_MENU"               },
  {WC_MLE               ,"WC_MLE"                },
  {WC_NOTEBOOK          ,"WC_NOTEBOOK"           },
  {WC_SCROLLBAR         ,"WC_SCROLLBAR"          },
  {WC_SLIDER            ,"WC_SLIDER"             },
  {WC_SPINBUTTON        ,"WC_SPINBUTTON"         },
  {WC_STATIC            ,"WC_STATIC"             },
  {WC_TITLEBAR          ,"WC_TITLEBAR"           },
  {WC_VALUESET          ,"WC_VALUESET"           },

  {WC_APPSTAT           ,"WC_APPSTAT"            },
  {WC_DBE_KKPOPUP       ,"WC_DBE_KKPOPUP"        },
  {WC_KBDSTAT           ,"WC_KBDSTAT"            },
  {WC_PECIC             ,"WC_PECIC"              },

  {NULL                 ,NULL                    }   /* End-Of-Table marker   */
};

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Need to perform one-time generation of "#nnn" strings?                 Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  if ( Class2WC[0].szClassName[0] == 0 )
  {
    for(/* Initialize */  i = 0;
        /* While      */  Class2WC[i].pszClassName != NULL;
        /* Iterate    */  i++)
    {
       /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
       /*Ё Generate class name in "#nnn" format                              Ё*/
       /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
       sprintf(Class2WC[i].szClassName,
               "#%d",
               LOUSHORT(Class2WC[i].pszClassName) );
    }
  }

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Translate all WC_* class names to something human understandable       Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  for(/* Initialize */ pszTranslate =  NULL,
                       i            = 0;
      /* While      */ pszTranslate == NULL;
      /* Iterate    */ i++)
  {
     /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
     /*Ё At end of translate table?  if so, use class name ASIS              Ё*/
     /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
     if (Class2WC[i].pszClassName == NULL)
       pszTranslate = pszClassName;
     /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
     /*Ё Is this a WC_* class?  if so, use translation                       Ё*/
     /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
     else if (strcmp(pszClassName, Class2WC[i].szClassName) == 0)
       pszTranslate = Class2WC[i].pszWC;
  }

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Return the translation result                                          Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  return( pszTranslate );
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Module Level Design                                                      Ё*/
/*Ё                                                                          Ё*/
/*Ё                                                                          Ё*/
/*Ё Function:                                                                Ё*/
/*Ё     To center a dialog in the middle of the DESKTOP area                 Ё*/
/*Ё                                                                          Ё*/
/*Ё Input:                                                                   Ё*/
/*Ё     - Dialog's Window Handle                                             Ё*/
/*Ё                                                                          Ё*/
/*Ё                                                                          Ё*/
/*Ё Output:                                                                  Ё*/
/*Ё     - Returns:                                                           Ё*/
/*Ё       a) TRUE........ no error detected (dialog nicely centered!)        Ё*/
/*Ё       b) FALSE....... error detected (logged to RAS error log)           Ё*/
/*Ё                                                                          Ё*/
/*Ё Notes:                                                                   Ё*/
/*Ё     -                                                                    Ё*/
/*Ё                                                                          Ё*/
/*Ё Logic:                                                                   Ё*/
/*Ё     - get size of DESKTOP area (results are 'window' coordinates)        Ё*/
/*Ё     - convert DESKTOP area to 'screen' coordinates                       Ё*/
/*Ё     - get size of Dialog (results are 'window' coordinates)              Ё*/
/*Ё     - convert Dialog area to 'screen' coordinates                        Ё*/
/*Ё     - use WinSetPos to position the Dialog centered on the DESKTOP       Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

BOOL CenterDialog(HWND hwndDialog)     /* Dialog window to center */

{
  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Local Variables                                                        Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  BOOL     fOK      = BOOL_FALSE;    /* start as a pessimist! */

  RECTL    rectlDesktop,             /* Rectl of DeskTop */
           rectlDialog;              /* Rectl of dialog */

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Note: using DO_ONCE_BLOCK for easy error recovery                      Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  BEGIN_DO_ONCE_BLOCK

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Gather information on the DESKTOP                                    Ё*/
    /*Ё                                                                      Ё*/
    /*Ё - get DESKTOP rectangle (in window coordinates)                      Ё*/
    /*Ё - convert to screen coordinates                                      Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    if ( !WinQueryWindowRect(HWND_DESKTOP, &rectlDesktop) ||

         !WinMapWindowPoints(hwndDialog,              /* From: Window coordinates */
                             HWND_DESKTOP,            /* To:   Desktop coordinates */
                             (PPOINTL)&rectlDesktop,  /* address(points) */
                             2)  ||                   /* number of points */

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Gather information on the dialog                                     Ё*/
    /*Ё                                                                      Ё*/
    /*Ё - get DESKTOP rectangle                                              Ё*/
    /*Ё - convert to window coordinates                                      Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
         !WinQueryWindowRect(hwndDialog, &rectlDialog) ||

         !WinMapWindowPoints(hwndDialog,              /* From: Window coordinates */
                             HWND_DESKTOP,            /* To:   Desktop coordinates */
                             (PPOINTL)&rectlDialog,   /* address(points) */
                             2) )
    {
      EXIT_DO_ONCE_BLOCK;
    }

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Position Dialog in center of Desk Top                                Ё*/
    /*Ё                                                                      Ё*/
    /*Ё Centered X position = (Width (DeskTop) - Width (Dialog) ) / 2        Ё*/
    /*Ё Centered Y position = (Height(DeskTop) - Height(Dialog) ) / 2        Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    if ( WinSetWindowPos(hwndDialog,
                         HWND_TOP,                       /* Z_ORDER */
                         (SHORT)(                        /* X position */
                                 ((rectlDesktop.xRight - /*  Desktop height */
                                   rectlDesktop.xLeft) -
                                  (rectlDialog .xRight - /*  Dialog  height */
                                   rectlDialog .xLeft))/2
                                ),

                         (SHORT)(                        /* Y position */
                                 ((rectlDesktop.yTop   - /* Desktop width */
                                   rectlDesktop.yBottom) -
                                  (rectlDialog .yTop   - /* Dialog  width */
                                   rectlDialog .yBottom))/2
                                ),

                         (SHORT)NULL,
                         (SHORT)NULL,
                         SWP_MOVE | SWP_ZORDER) )        /* Flags */

       fOK = BOOL_TRUE;         /* we're a success! */
    else
    {
      EXIT_DO_ONCE_BLOCK;
    }

  END_DO_ONCE_BLOCK;

 /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
 /*Ё Exit                                                                    Ё*/
 /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
 return( fOK );
}

/*---------------------------------------------------------------------------
* Set new listbox FONT
*
* Input:   hwndLB.....HWND of LISTBOX
*          pszFont....new FONT
*
* OutPut:  NONE
+---------------------------------------------------------------------------*/
VOID SetListboxFont(HWND hwndLB,
                    PSZ  pszFont)
{
   /*****************************************************************
   * Local data
   ******************************************************************/
   HPS               hPS;
   FONTMETRICS       Metrics;

   /*****************************************************************
   * Set new LISTBOX font
   ******************************************************************/
   WinSetPresParam(hwndLB,
                   PP_FONTNAMESIZE,
                   (ULONG) strlen(pszFont) + 1,     /* must include NULL! */
                                  pszFont);

   /*****************************************************************
   * Get "metrics" of the new LISTBOX font
   ******************************************************************/
   hPS = WinGetPS(hwndLB);

   GpiQueryFontMetrics(hPS, (LONG)sizeof(FONTMETRICS), &Metrics );

   WinReleasePS(hPS);

   /*****************************************************************
   * Set new LISTBOX height so no inter-row "gap"
   ******************************************************************/
   WinSendMsg(hwndLB,
              LM_SETITEMHEIGHT,
              MPFROMSHORT( LOUSHORT(Metrics.lMaxBaselineExt) ),
              MPFROMP(NULL) );
}
