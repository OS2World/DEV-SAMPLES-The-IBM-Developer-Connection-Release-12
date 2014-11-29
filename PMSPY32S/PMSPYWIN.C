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
/*Ё PMSPYWIN.C                                                               Ё*/
/*Ё                                                                          Ё*/
/*Ё "Client Area" window Procedure for the PMSPY Main Window                 Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Includes                                                                 Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
#include "pmspy.h"                      /* Resource symbolic identifiers      */

#include "pmspyIPF.h"                   /* IPF panel identifiers              */
#include "pmspyINI.h"                   /* PMSPY.INI definitions              */

#include <io.h>

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё template for each item added to the "owner drawn" SPY list               Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
typedef enum
{
  LIST_IS_TEXT  = '0',
  LIST_IS_MSG

} LIST_IS;

typedef struct
{
  CHAR        logCLR,                /* logical color index                   */
              useHalfTone;           /* draw using DT_HALFTONE? ('1'=yes)     */
  LIST_IS     listIS;                /* is this a MSG?                        */
  CHAR        szMsg[L_NOTES_TEXT];   /* items text                            */

} LIST_DATA;

typedef struct
{
  USHORT      usHookID;              /* type of "hook" that capture MSG       */
  QMSG        qmsgSpy;               /* MSG data from "hook"                  */

}  LIST_MSG_DATA,
 *PLIST_MSG_DATA;

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Spy Window Procedure Data                                                Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
static CHAR      szFileName[CCHMAXPATH],
                 szHelpFile[] = "PMSPY.HLP";

static SPY_DATA  spyData;
static PSPY_DATA pSpyData      =  &spyData;

static ULONG     NbrOfMessages = 0;

static HWND      hwndMsgList   = NULLH, /* Msg list scroll list window handle */
                 hwndFrame     = NULLH, /* Frame (Parent of this CLIENT window) */
                 hwndMenu      = NULLH, /* Frame's menu                       */
                 hwndIPF       = NULLH; /* IPF handle                         */

static LONG   lItemHeight;  /* height of an item returned by WM_MEASUREITEM   */
static LONG   lNbrOfItems;  /* nbr of items in the list neede to fill client window */

static SPY_TITLES spyCurrent = tStandard;  /* current AGENT state             */

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё PMSPY INI file data items                                                Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
INI_DATA iniAgent;                   /* Note: initialized by ReadSpyINI()     */

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Toggle menu 'check' on exclusive items                                   Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
static VOID ToggleMenuChecking(LONG idDecheck,
                               LONG idCheck)
{
  WinSendMsg(hwndMenu,
             MM_SETITEMATTR,                           /* de-check first */
             MPFROM2SHORT(LOUSHORT(idDecheck), TRUE),
             MPFROM2SHORT(MIA_CHECKED,         0) );

  WinSendMsg(hwndMenu,
             MM_SETITEMATTR,                           /* then check */
             MPFROM2SHORT(LOUSHORT(idCheck), TRUE),
             MPFROM2SHORT(MIA_CHECKED,       MIA_CHECKED) );
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Check/Decheck a menu pulldown item                                       Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
static VOID CheckMenuPulldown(LONG idItem,
                              BOOL bCheck)
{
  WinSendMsg(hwndMenu,                                     /* Menu window */
             MM_SETITEMATTR,                               /* Action */
             MPFROM2SHORT(LOUSHORT(idItem), TRUE),         /* Go look for it! */
             MPFROM2SHORT(MIA_CHECKED,
                          bCheck ? MIA_CHECKED : 0) );     /* do right thing */
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Enable/Disable menu pulldown item                                        Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
static VOID EnableMenuPulldown(LONG idItem,
                               BOOL bEnable)
{
  WinSendMsg(hwndMenu,                                     /* Menu window */
             MM_SETITEMATTR,                               /* Action */
             MPFROM2SHORT(LOUSHORT(idItem), TRUE),         /* Go look for it! */
             MPFROM2SHORT(MIA_DISABLED,
                          bEnable ? 0 : MIA_DISABLED) );   /* do right thing */
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Clear the Message List                                                   Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
static VOID ClearMessageList( VOID )
{
  /************************************************************
  * Obtain number of items currently in the ListBox
  ************************************************************/
  USHORT usItems = SHORT1FROMMR( WinSendMsg(hwndMsgList,
                                            LM_QUERYITEMCOUNT,
                                            NULL,
                                            NULL)),
         usI;

  PVOID pHandle;

  /************************************************************
  * We must scan all ListBox items and free "handle" data
  ************************************************************/
  for (usI = 0;               /* Init:  at first LB item     */
       usI < usItems;         /* While: all not checked      */
       usI++)                 /* Iter:  to next  LB item     */
  {
    /************************************************************
    * If this item has an attached "handle", free it's memory
    ************************************************************/
    if (pHandle = PVOIDFROMMR( WinSendMsg(hwndMsgList,
                                          LM_QUERYITEMHANDLE,
                                          MPFROMSHORT(usI),
                                          NULL)) )
      FreeMemory(pHandle);
  }

  /************************************************************
  * "Lock" the ListBox as the items are deleted
  ************************************************************/
  WinLockWindowUpdate(HWND_DESKTOP, hwndMsgList);

  /************************************************************
  * Delete all of the ListBox items
  ************************************************************/
  WinSendMsg(hwndMsgList, LM_DELETEALL, NULL, NULL);

  /************************************************************
  * Finally, "unlock" the ListBox so it is only redrawn once
  ************************************************************/
  WinLockWindowUpdate(HWND_DESKTOP, NULLHANDLE);

  /************************************************************
  * Reset # items in the ListBox
  ************************************************************/
  NbrOfMessages = 0;
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Open/Append a Profile                                                    Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
typedef enum {UseDefault, PromptForFile, UsePassedFile} PPF_MODE;

VOID ProcessProfileFile(HWND        hwndCur,
                        HWND        hwndFrame,
                        PSPY_DATA   pSpyData,
                        BOOL        bOpen,  /* T=Open, F=Append */
                        PPF_MODE    mode,
                        PSZ         useName)

{
  register USHORT i;
           MSG    lowMsg, highMsg;

  FILEDLG  *FileDlg;

  /* Prompt user for PROFILE name IF we need to.... */

  switch( mode )
  {
    case PromptForFile:

         FileDlg           = calloc(sizeof(FILEDLG), 1);

         FileDlg->cbSize   = sizeof(FILEDLG);
         FileDlg->fl       = FDS_OPEN_DIALOG | FDS_CENTER;
         FileDlg->pszTitle = Strings[IDS_PROFILE_OPEN];
         strcpy(FileDlg->szFullFile, iniAgent.szProfile);

         WinFileDlg(HWND_DESKTOP, hwndCur, FileDlg);

         if (FileDlg->lReturn != DID_OK)
         {
            free(FileDlg);
            return;
         }

         strcpy(szFileName,         FileDlg->szFullFile);
         strcpy(iniAgent.szProfile, szFileName);

         free(FileDlg);
    break;

    default:    /* nothing to do.... */ ;
  }

  /*** reset SPY controls IFF open ***/

  if (bOpen)
    ResetSpyData(pSpyData);

  /*** Load the appropriate Profile ***/

  switch( mode )
  {
    case UsePassedFile:
         OpenProfile(hwndCur, hwndFrame, pSpyData, useName);
    break;

    case PromptForFile:
         OpenProfile(hwndCur, hwndFrame, pSpyData, szFileName);
    break;

    default:
         OpenProfile(hwndCur, hwndFrame, pSpyData, NULL);
    break;
  }

  /*** Pass new Message Range to DLL servant ***/

  if (pSpyData->TotalMsgs != 0)
  {
    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Process all of the MSG slots                                         Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    for(/* Initialize */ i       = 0,         /* @ first MSG_SLOT */
                         lowMsg  = 0xFFFF,    /* outrageous LOW */
                         highMsg = 0;         /* outrageous HIGH */

        /* While      */ i < NO_MSG_SLOTS;    /* while not all examined */

        /* Iterate    */ i++)
    {
      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Check for range change IFF message(s) in this slot                 Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      if (pSpyData->Msgs[i].uNoMsgs != 0)
      {
        if (pSpyData->Msgs[i].MsgLow < lowMsg)     /* new LOW range? */
          lowMsg = pSpyData->Msgs[i].MsgLow;

        if (pSpyData->Msgs[i].MsgHigh > highMsg)   /* new HIGH range? */
          highMsg = pSpyData->Msgs[i].MsgHigh;
      }
    }

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Set new MSG range to optimize processing                             Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    SpySetTargetMsgRange(SpyInstance, lowMsg, highMsg);
  }
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Build array with the names of all Non-proportional FONT names            Ё*/
/*Ё                                                                          Ё*/
/*Ё Input:   hWnd.....HWND of window (for obtaining a temporary HPS)         Ё*/
/*Ё                                                                          Ё*/
/*Ё OutPut:  NULL.....unable to determine FONTS                              Ё*/
/*Ё          !NULL....pointer to first CHAR szFacename[FACESIZE] element;    Ё*/
/*Ё                   variable length list ended when szFacename[0] = NULL   Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
PFONTNAME DetermineFixedFonts(HWND  hWnd)

{
   LONG             lReqFonts     = 0,          /* # FONTS requested */
                    lTotFonts     = 0;          /* # FONTS not processed */

   register USHORT   usI,
                     usFixed;

   HPS               hPS;

   PFONTMETRICS      pfmList        = NULL;
   PFONTNAME         pfnList        = NULL;

   FONTNAME          fnBuild;

   /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
   /*Ё First, we need a HPS before we can QUERY about the FONTS...           Ё*/
   /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

   hPS = WinGetPS(hWnd);

   /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
   /*Ё Determine maximum number of FONTs possible                            Ё*/
   /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

   lTotFonts = GpiQueryFonts(hPS,                 /* Handle(PS) */
                             QF_PUBLIC |          /* Enumerate all PUBLIC fonts */
                             QF_PRIVATE,          /*           and PRIVATE fonts */
                             NULL,                /* include all available fonts */
                             &lReqFonts,          /* use ZERO to get total */
                             (LONG)
                             sizeof(FONTMETRICS), /* sizeof(ONE font metrics) */
                             pfmList);            /* address(1st font metrics) */

   /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
   /*Ё Allocate array big enough for all those FONTs possible                Ё*/
   /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
   pfmList = AllocateMemory( (size_t) (lTotFonts * sizeof(FONTMETRICS)) );

   /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
   /*Ё Enumerate all those FONTs possible                                    Ё*/
   /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

   lReqFonts = lTotFonts;                     /* request all of them! */

   lTotFonts = GpiQueryFonts(hPS,             /* Handle(PS) */
                             QF_PUBLIC |      /* Enumerate all PUBLIC fonts */
                             QF_PRIVATE,      /*           and PRIVATE fonts */
                             NULL,            /* include all available fonts */
                             &lReqFonts,      /* Max # fonts requesting */
                             (LONG)
                             sizeof(FONTMETRICS), /* sizeof(ONE font metrics) */
                             pfmList);        /* address(font metrics) */

   WinReleasePS(hPS);

   /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
   /*Ё Now, determine how many are Fixed width FONTs                         Ё*/
   /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

   for(usI     =                    /* Initialize: start @ first FONT */
       usFixed = 0;                 /*             no fixed FONTS, yet... */

       usI < LOUSHORT(lReqFonts);   /* While:      not all examined */
       usI++)                       /* Iterate:    move to the next FONT */
   {
     /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
     /*Ё Increment count if a Fixed width FONTs                              Ё*/
     /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

     if (pfmList[usI].fsType  & FM_TYPE_FIXED)
       usFixed++;
   }

   /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
   /*Ё Allocate and build list with names of Fixed width FONTs               Ё*/
   /*Ё                                                                       Ё*/
   /*Ё - include an extra FONT name slot for the terminating NULL name...    Ё*/
   /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
   pfnList = AllocateMemory( (size_t)((usFixed + 1) * sizeof(FONTNAME)) );

   for(usI     =                    /* Initialize: start @ first FONT */
       usFixed = 0;

       usI < LOUSHORT(lReqFonts);   /* While:      not all examined */
       usI++)                       /* Iterate:    move to the next FONT */
   {
     /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
     /*Ё Copy FONT name if a Fixed width FONTs                               Ё*/
     /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
     if (pfmList[usI].fsType & FM_TYPE_FIXED)
     {
       PFONTNAME pFont;
       USHORT    i;

       sprintf(&fnBuild[0],
               "%d.%s",
               pfmList[usI].sNominalPointSize / 10,
               pfmList[usI].szFacename);

       /* test if this font is bold or italic */

       if (strstr(fnBuild, "Bold"  )) continue;
       if (strstr(fnBuild, "Italic")) continue;

       /* test if this font is already used */

       for (pFont = pfnList,                              /* @ first FONT */
            i     = 0;

            i < usFixed;

            pFont++,
            i++)
       {
         if (strcmp(pFont[0], fnBuild) == 0)              /* already added?   */
           break;
       }

       if (i >= usFixed)
       {
         strcpy(pfnList[usFixed], fnBuild);

         usFixed++;
       }
     }
   }

   FreeMemory(pfmList);

   /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
   /*Ё Finally, return the results                                           Ё*/
   /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
   return( pfnList );
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Spy Window Procedure                                                     Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
MRESULT SpyWindowProc(HWND   hwnd,
                      MSG    msg,
                      MPARAM mp1,
                      MPARAM mp2)
{
  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Stack based AUTOMATIC variables                                        Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  MRESULT              wpResult = NULL;

  HPS                  hps;                    /* Presentation space handle   */

  RECTL                rectl;                  /* Window rectangle            */

  USHORT               usWork;

  PMSG_ITEM            pMsg;
  PLIST_MSG_DATA       pHandle;

  CHAR                 szText[256],
                       szFormat[64];

  HWND                 hwndTarget;

  HMQ                  hmqTarget;

  POINTL               ptrPos;

  FILEDLG              *FileDlg;

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё STATIC variables preserved across each MSG                             Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  static BOOL          bAutoScrollList             = BOOL_TRUE,

                       bWindowIsObject             = BOOL_FALSE,
                       bIncludeUndefinedWM_USER    = BOOL_FALSE,
                       bIncludeUndefinedMsgs       = BOOL_FALSE,
                       bSelecting                  = BOOL_FALSE;

  static LONG          MaxNbrOfMessages      = 0,  /* dynamically set from .RC */
                       MaxDeleteItems        = 1;  /* dynamically set from .RC */

  static HPOINTER      hSpy                  = NULLH,
                       hOld                  = NULLH,
                       hIllegal              = NULLH,
                       hLast                 = NULLH;

  static PFONTNAME     pFontList             = NULL;

  static HMQ           hmqSPY                = NULLH;

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Process this MSG                                                       Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  switch(msg)
  {
    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Message from one of our "hook" procedures                            Ё*/
    /*Ё                                                                      Ё*/
    /*Ё Mp1............PQMSG to data for "hooked" MSG                        Ё*/
    /*Ё Mp2............SHORT1FROMMP = HK_* ID of message source              Ё*/
    /*Ё                SHORT2FROMMP = Not Used                               Ё*/
    /*Ё                                                                      Ё*/
    /*Ё - decide if this MSG should be placed in the "spy list"              Ё*/
    /*Ё - if so, also check if a "trigger"                                   Ё*/
    /*Ё                                                                      Ё*/
    /*Ё Notes:                                                               Ё*/
    /*Ё ------                                                               Ё*/
    /*Ё - since this is the most time critical MSG, it should be left as     Ё*/
    /*Ё   the FIRST case of the switch statement!                            Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case PMSPY_MSG_FROM_HOOK:
    {
      PQMSG     pSpyMsg = PVOIDFROMMP(mp1);  /* locate passed "hook" data     */

      PSZ       pszHK   = SHORT1FROMMP(mp2) == HK_INPUT  /* Sent or Posted?   */
                          ? Strings[IDS_MSG_POSTED]
                          : Strings[IDS_MSG_SENT];

      MSG_COLOR ClrFG;                       /* Msg color */
      USHORT    thisIS;                      /* this a MSG or TEXT item?    */

      szText[0] = 0;                         /* indicate "no msg formatted" */

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Is this a "known" MSG?                                             Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      if ( pMsg = FindMsg(pSpyData, pSpyMsg->msg) )
      {
        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё YES: is MSG defined to be included in list?                      Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        if ( ! pMsg->Include )
          break;                                  /* NO ==> ignore            */

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё MSG specific chores                                              Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        ClrFG  = pMsg->ClrFG;                     /* using real MSG's color   */
        thisIS = LIST_IS_MSG;                     /* this is a MSG!           */

        pMsg->aulTimes[MSG_TIMES_EVER ]++;        /* # times "seen" EVER      */
        pMsg->aulTimes[MSG_TIMES_SINCE]++;        /* # times "seen" SINCE     */

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Should this MSG "trigger" an automatic THAW?                     Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        if (pMsg->TriggerThaw && (slmState == SLM_FROZEN) )
        {
          slmState = SLM_THAWED;

          WinSendMsg(hwnd,
                     PMSPY_ADD_TO_LIST,
                     MPFROMP(Strings[IDS_STAT_TRIGGERED_THAW]),
                     MPFROM2SHORT(Color_Status, LIST_IS_TEXT) );
        }

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё If list is FROZEN, then ignore this MSG                          Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        if ( slmState == SLM_FROZEN )
          break;

        /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Format "standard" MSG                                         Ё*/
        /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        sprintf(szText,
                Strings[IDS_FMT_STANDARD],
                pMsg->pDesc,
                pSpyMsg->mp1,
                pSpyMsg->mp2,
                pszHK,
                pSpyMsg->hwnd);
      }
      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё This message is NOT Defined                                        Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      else
      {
        thisIS = LIST_IS_TEXT;     /* if used, this should be considered TEXT */

        /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Is it a user MSG?                                             Ё*/
        /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        if ( pSpyMsg->msg >= WM_USER ) /* WM_USER message? */
        {
          /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
          /*Ё Yes: configured to include undefined WM_USER messages?        Ё*/
          /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
          if ( !bIncludeUndefinedWM_USER )
            break;                              /* NO ==> ignore             */

          /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
          /*Ё Format undefined WM_USER message                              Ё*/
          /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
          ClrFG = Color_UndefinedWM_USER;

          sprintf(szFormat,
                  Strings[IDS_FMT_USER_MSG],
                  pSpyMsg->msg );

          sprintf(szText,
                  Strings[IDS_FMT_STANDARD],
                  szFormat,
                  pSpyMsg->mp1,
                  pSpyMsg->mp2,
                  pszHK,
                  pSpyMsg->hwnd);
        }
        /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё This is an undefined system message (< WM_USER)               Ё*/
        /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        else
        {
          /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
          /*Ё Yes: configured to include undefined system messages?         Ё*/
          /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
          if ( !bIncludeUndefinedMsgs )
            break;

          /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
          /*Ё Format undefined system message                               Ё*/
          /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
          ClrFG = Color_UndefinedMsg;

          sprintf(szFormat,
                  Strings[IDS_FMT_UNKNOWN_MSG],
                  pSpyMsg->msg );

          sprintf(szText,
                  Strings[IDS_FMT_STANDARD],
                  szFormat,
                  pSpyMsg->mp1,
                  pSpyMsg->mp2,
                  pszHK,
                  pSpyMsg->hwnd);
        }
      }

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Did we end up with a formatted message to add to the "Spy list"?   Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      if (szText[0] != 0)
      {
        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё add the message                                                  Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        usWork = SHORT1FROMMR( WinSendMsg(hwnd,
                                          PMSPY_ADD_TO_LIST,
                                          MPFROMP(szText),
                                          MPFROM2SHORT(ClrFG, thisIS)) );

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё if "real" message added, set it's HANDLE to a copy of QMSG       Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        if (thisIS == LIST_IS_MSG)
        {
          pHandle = AllocateMemory( sizeof(*pHandle) );

          pHandle->usHookID = SHORT1FROMMP(mp2);
          pHandle->qmsgSpy  = *pSpyMsg;

          WinSendMsg(hwndMsgList,
                     LM_SETITEMHANDLE,          /* Message                    */
                     MPFROMSHORT(usWork),       /* Index of list item         */
                     MPFROMP(pHandle));         /* Handle of list item        */
        }

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Should this MSG "trigger" an automatic FREEZE?                   Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        if (pMsg && pMsg->TriggerFreeze && (slmState == SLM_THAWED) )
        {
          slmState = SLM_FROZEN;

          WinSendMsg(hwnd,
                     PMSPY_ADD_TO_LIST,
                     MPFROMP(Strings[IDS_STAT_TRIGGERED_FREEZE]),
                     MPFROM2SHORT(Color_Status, LIST_IS_TEXT) );
        }
      }
    }
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Add a text Message to the SPY list                                   Ё*/
    /*Ё                                                                      Ё*/
    /*Ё Mp1............PSZ to message text                                   Ё*/
    /*Ё Mp2............LOUSHORT = binary Color index                         Ё*/
    /*Ё                HIUSHORT = is this a MSG or TEXT                      Ё*/
    /*Ё                                                                      Ё*/
    /*Ё Result.........index of new item in the SPY list                     Ё*/
    /*Ё                                                                      Ё*/
    /*Ё a) if list is full, make room                                        Ё*/
    /*Ё b) add to list                                                       Ё*/
    /*Ё c) LOG if logging active                                             Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case PMSPY_ADD_TO_LIST:
    {
      LIST_DATA  AddData;

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё is the SPY list now full?                                          Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      if ( (++NbrOfMessages) == MaxNbrOfMessages)
      {
        NbrOfMessages -= MaxDeleteItems;      /* Yes...delete group of "oldest" */

        /************************************************************
        * "Lock" the ListBox to minimize performance impact
        ************************************************************/
        WinLockWindowUpdate(HWND_DESKTOP, hwndMsgList);

        for (usWork = 0;                      /* Init: */
             usWork < MaxDeleteItems;         /* While: limit not deleted */
             usWork++)                        /* Iter:  increment count */
        {
          /************************************************************
          * If first item has an attached "handle", free it's memory
          ************************************************************/
          if (pHandle = PVOIDFROMMR( WinSendMsg(hwndMsgList,
                                                LM_QUERYITEMHANDLE,
                                                MPFROMSHORT(0),
                                                NULL)) )
            FreeMemory(pHandle);

          /************************************************************
          * Delete the first item
          ************************************************************/
          WinSendMsg(hwndMsgList, LM_DELETEITEM, MPFROMSHORT(0), NULL);
        }

        /************************************************************
        * Finally, "unlock" the ListBox so it is only redrawn once
        ************************************************************/
        WinLockWindowUpdate(HWND_DESKTOP, NULLHANDLE);
      }

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Final formatting of message before adding to list                  Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      AddData.logCLR      = (CHAR)   (CHAR1FROMMP(mp2) + COLOR_BASE);
      AddData.listIS      = (LIST_IS) CHAR3FROMMP(mp2);

      AddData.useHalfTone = '0';                   /* force to FULL tone      */

      strcpy(AddData.szMsg, mp1);

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё insert the message at the end of the list (since time ordered)     Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      wpResult = WinSendMsg(hwndMsgList,
                            LM_INSERTITEM,
                            MPFROMSHORT(LIT_END),
                            MPFROMP(&AddData));

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Make added item visible (by forcing it to the top of list)         Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      if (bAutoScrollList)
        WinSendMsg(hwndMsgList,
                   LM_SETTOPINDEX,
                   MPFROMSHORT(max(0, NbrOfMessages -  lNbrOfItems)),
                   NULL);

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё are we logging to a file?                                          Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      if (bCurrentlyLogging)
        fprintf(pLogFile, "%s\n", AddData.szMsg);
    }
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Messages from control windows we "own"                               Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_CONTROL:
    {
         /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
         /*Ё "open" request from the "spy list"?                             Ё*/
         /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
         if ( (SHORT1FROMMP(mp1) == ID_SPYLIST) &&
              (SHORT2FROMMP(mp1) == LN_ENTER) )
         {
           /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
           /*Ё "locate" the selected item                                    Ё*/
           /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
           usWork = SHORT1FROMMR( WinSendMsg(hwndMsgList,
                                             LM_QUERYSELECTION,
                                             MPFROMSHORT(LIT_FIRST),
                                             NULL) );
           pMsg   = NULL;

           /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
           /*Ё if it has a "handle" value then its a MSG                     Ё*/
           /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
           if (pHandle = PVOIDFROMMR(WinSendMsg(hwndMsgList,
                                                LM_QUERYITEMHANDLE,
                                                MPFROMSHORT(usWork),
                                                NULL)) )
           {
             /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
             /*Ё is MSG definition still available?                          Ё*/
             /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
             if ( pMsg = FindMsg(pSpyData, pHandle->qmsgSpy.msg) )
             {
               /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
               /*Ё Format MSG decompositon dialog data                       Ё*/
               /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
               MD_DATA msgData;

               memset(&msgData, 0, (size_t)sizeof(msgData) );

               msgData.pMsg     = pMsg;
               msgData.qmsgItem = pHandle->qmsgSpy;

               /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
               /*Ё Display MSG decompositon dialog                           Ё*/
               /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
               WinDlgBox(HWND_DESKTOP,
                         hwnd,
                         MsgDlgProc,
                         hmodNLS,
                         DLG_MD,
                         &msgData);
             }
           }

           if (pMsg == NULL)
             WinAlarm(HWND_DESKTOP, WA_NOTE);
         }
         /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
         /*Ё NO: just let PM handle...                                       Ё*/
         /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
         else
           wpResult = WinDefWindowProc(hwnd, msg, mp1, mp2);
    }
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Provide size of item to be drawn in the SPY message list             Ё*/
    /*Ё                                                                      Ё*/
    /*Ё - this is part of the responsibility of an OWNERDRAW listbox         Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_MEASUREITEM:
    {
         FONTMETRICS Metrics;
         hps = WinGetPS(hwnd);
         GpiQueryFontMetrics(hps, (LONG)sizeof(FONTMETRICS), &Metrics );
         WinReleasePS(hps);
         lItemHeight = Metrics.lMaxBaselineExt;

         wpResult = MRFROM2SHORT( LOUSHORT(Metrics.lMaxBaselineExt),
                                  L_NOTES_TEXT * LOUSHORT(Metrics.lAveCharWidth));         /* item WIDTH */
    }
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Draw the specified item in the SPY message list                      Ё*/
    /*Ё                                                                      Ё*/
    /*Ё - this is part of the responsibility of an OWNERDRAW listbox         Ё*/
    /*Ё - the ITEM text is really the data encoded LIST_DATA structure       Ё*/
    /*Ё   built of ASCII characters to 'survive' being added to the listbox  Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_DRAWITEM:
    {
      LIST_DATA  DrawData;

      POWNERITEM pOwn = PVOIDFROMMP(mp2);

      ULONG      ulFlags = DT_LEFT     |      /* Horizontally left justified */
                           DT_VCENTER  |      /* Vertically   centered */
                           DT_ERASERECT;      /* auto ERASE rectangle, Thanks! */

      if ( (pOwn->fsState    == pOwn->fsStateOld) || /* not a selection change */
           (pOwn->fsStateOld && pOwn->fsState) )
      {
        /* Get text of item to DRAW */

        WinSendMsg(pOwn->hwnd,
                   LM_QUERYITEMTEXT,
                   MPFROM2SHORT(pOwn->idItem, sizeof(DrawData)),
                   MPFROMP(&DrawData) );

        /* Determine if "half tone" drawing is required... */

        if ( DrawData.useHalfTone == '1' )
          ulFlags |= DT_HALFTONE;

        /* Draw using specified COLOR */

        WinDrawText(pOwn->hps,          /* target HPS */
                    -1,                 /* using PSZ string, let PM calculate */
                    DrawData.szMsg,     /* item text */
                    &pOwn->rclItem,     /* target list item rectangle */
ExternalColorTranslation[pSpyData->LogicalColorTranslation[DrawData.logCLR-COLOR_BASE].iExtColor].lClrValue,
                    SYSCLR_ENTRYFIELD,  /* BG color for ListBox */
                    ulFlags);
      }
      wpResult = MRFROMSHORT(TRUE);    /* Keep PM from drawing it too */
    }
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Frame has been re-sized...adjust client area size to fit within it   Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_SIZE:

         WinQueryWindowRect(hwnd,&rectl);
         WinSetWindowPos(hwndMsgList,
                         HWND_TOP,
                         0,                      /*  X */
                         0,                      /*  Y */
                         rectl.xRight,           /* cX */
                         rectl.yTop,             /* cY */
                         SWP_SIZE | SWP_MOVE);

         lNbrOfItems = rectl.yTop / max(1L, lItemHeight);
      break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё the user is trying to CLOSE us...                                    Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_CLOSE:

         SpyUnSetTarget(SpyInstance);

         WinSendMsg(hwnd,
                    PMSPY_ADD_TO_LIST,
                    MPFROMP(Strings[IDS_STAT_CLOSE]),
                    MPFROM2SHORT(Color_Status, LIST_IS_TEXT) );

         /* close logfile if still open */
         if (pLogFile)
           fclose(pLogFile);

         /* tell main window to terminate */
         wpResult = WinDefWindowProc(hwnd, msg, mp1, mp2);
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё This message is sent each time a PULLDOWN menu is to be used         Ё*/
    /*Ё                                                                      Ё*/
    /*Ё - we dynamically enable/disable various items based on our 'state'   Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_INITMENU:

         switch( SHORT1FROMMP(mp1) )
         {
           case ID_AB_SPYON:
                /************************************************************
                * What's current state?
                *************************************************************/
                switch (spyCurrent)
                {
                  /**********************************************************
                  * SPYing is active...
                  ***********************************************************/
                  case tWindow:
                  case tObject:
                  case tQueue:
                  case tQueueSystem:

                       EnableMenuPulldown(ID_SELECT,          TRUE);
                       EnableMenuPulldown(ID_SELECT_OBJECT,   TRUE);
                       EnableMenuPulldown(ID_SELECT_SYSTEM_Q, TRUE);

                       EnableMenuPulldown(ID_DESELECT,        TRUE);
                  break;

                  /**********************************************************
                  * SPYing is NOT active...
                  ***********************************************************/
                  default:

                       EnableMenuPulldown(ID_SELECT,          BOOL_TRUE);
                       EnableMenuPulldown(ID_SELECT_OBJECT,   BOOL_TRUE);
                       EnableMenuPulldown(ID_SELECT_SYSTEM_Q, BOOL_TRUE);

                       EnableMenuPulldown(ID_DESELECT,        BOOL_FALSE);
                }
           break;

           case ID_AB_SPYWHAT:

                if ( SpyQueryTargetIsWindow(SpyInstance)  )
                  ToggleMenuChecking(IDD_QUE, IDD_WND);
                else
                  ToggleMenuChecking(IDD_WND, IDD_QUE);

                /************************************************************
                * What's current state?
                *************************************************************/
                switch (spyCurrent)
                {
                  /**********************************************************
                  * SPYing on SYSTEM QUEUE...
                  ***********************************************************/
                  case tQueueSystem:
                       EnableMenuPulldown(IDD_QUE,  BOOL_FALSE);
                       EnableMenuPulldown(IDD_WND,  BOOL_FALSE);
                  break;

                  /**********************************************************
                  * All other SPYing modes...
                  ***********************************************************/
                  default:
                       EnableMenuPulldown(IDD_QUE,  BOOL_TRUE);
                       EnableMenuPulldown(IDD_WND,  BOOL_TRUE);
                }
           break;

           case ID_AB_FILE:
                ToggleMenuChecking(pLogFile == NULL ? ID_F_OPENLOG   : ID_F_CLOSELOG,
                                   pLogFile == NULL ? ID_F_CLOSELOG  : ID_F_OPENLOG);

                EnableMenuPulldown(ID_F_OPENLOG,
                                   pLogFile == NULL ? BOOL_TRUE  : BOOL_FALSE);
                EnableMenuPulldown(ID_F_OPENLOG_SNAPSHOT,
                                   pLogFile == NULL ? BOOL_TRUE  : BOOL_FALSE);
                EnableMenuPulldown(ID_F_CLOSELOG,
                                   pLogFile == NULL ? BOOL_FALSE : BOOL_TRUE);
           break;

           case ID_AB_OPTIONS:
                /* decide which of the Freeze/Thaw options is active now */
                CheckMenuPulldown(ID_O_THAW,
                                  slmState == SLM_THAWED ? BOOL_TRUE : BOOL_FALSE);
                CheckMenuPulldown(ID_O_FREEZE,
                                  slmState == SLM_FROZEN ? BOOL_TRUE : BOOL_FALSE);

                if (pLogFile != NULL) /* LOGing active? */
                {
                  ToggleMenuChecking(bCurrentlyLogging ? ID_O_STOPLOG  : ID_O_STARTLOG,
                                     bCurrentlyLogging ? ID_O_STARTLOG : ID_O_STOPLOG);
                  EnableMenuPulldown(ID_O_STARTLOG, bCurrentlyLogging
                                                    ? BOOL_FALSE
                                                    : BOOL_TRUE);
                  EnableMenuPulldown(ID_O_STOPLOG,  bCurrentlyLogging);
                }
                else
                {
                  EnableMenuPulldown(ID_O_STARTLOG, BOOL_FALSE);
                  EnableMenuPulldown(ID_O_STOPLOG,  BOOL_FALSE);
                } /* endif */

                CheckMenuPulldown(ID_O_SCROLL,     bAutoScrollList );
           break;

           case ID_AB_FILTER:
                CheckMenuPulldown(ID_F_INC_WM_USER,   bIncludeUndefinedWM_USER );
                CheckMenuPulldown(ID_F_INC_UNDEFINED, bIncludeUndefinedMsgs );
           break;
         } /* endswitch */
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё "Snapshot" the current SPY list contents to the LOG file             Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case PMSPY_SNAPSHOT:
    {
         LIST_DATA  ListData;

         /********************************************************************
         * Only "Snapshot" if LOGing is active....
         *********************************************************************/
         if (pLogFile != NULL)
         {
           fputs(Strings[IDS_STAT_SNAPSHOT_ON], pLogFile);
           fputc('\n',   pLogFile);

           /******************************************************************
           * Process all SPY list items
           *******************************************************************/
           for(/* Initialize: start at first LISTBOX item */
               usWork = 0;

               /* While:      able to still get LISTBOX item text */
               WinSendMsg(hwndMsgList,
                          LM_QUERYITEMTEXT,
                          MPFROM2SHORT(usWork,            /* current LB item */
                                       sizeof(ListData)), /* max TEXT size   */
                          MPFROMP(&ListData)) != 0;

               /* Iterate:    move to next LISTBOX item */
               usWork++)
           {
              /***************************************************************
              * use FPUTS/FPUTC pair since faster than FPRINTF
              ***************************************************************/
              fputs(ListData.szMsg, pLogFile);
              fputc('\n',           pLogFile);
           }

           fputs(Strings[IDS_STAT_SNAPSHOT_OFF], pLogFile);
           fputc('\n',   pLogFile);

         }
    }
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё This message is sent each time a PULLDOWN menu is selected           Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_COMMAND:

      switch (SHORT1FROMMP(mp1))
      {
        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Exit from PMSPY                                                  Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_EXIT:
             WinPostMsg(hwnd, WM_CLOSE, NULL, NULL);
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Display "Product information"....                                Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_H_ABOUT:
             WinDlgBox(HWND_DESKTOP,
                       hwnd,
                       (PFNWP)SpyLogoDlgProc,
                       hmodNLS,
                       IDD_ABOUT,
                       NULL);
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё User want to "point and shoot" at the window to be SPYed...      Ё*/
        /*Ё                                                                  Ё*/
        /*Ё - WinSetCapture will route all MOUSE messages to this window     Ё*/
        /*Ё - The MOUSE messages are processing below                        Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_SELECT:

             SpyUnSetTarget(SpyInstance);

             UpdateSpyTitle(hwndFrame,
                            spyCurrent = tStandard);

             bSelecting = TRUE;

             WinSetCapture(HWND_DESKTOP, hwnd);

             hOld = WinQueryPointer(HWND_DESKTOP);
             WinSetPointer(HWND_DESKTOP, hSpy);

             /* force the window in the background */
             WinSetWindowPos(hwndFrame, HWND_BOTTOM, 0, 0, 0, 0, SWP_ZORDER);
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё User want to "spy" on the SYSTEM MESSAGE QUEUE...                Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_SELECT_SYSTEM_Q:

             /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
             /*Ё Make user verify "spying" on PM's system queue              Ё*/
             /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
             if (MBID_YES != MsgDisplay(hwnd,
                                        swcntrl.szSwtitle,
                                        Strings[IDS_MSG_SYSTEM_Q_VERIFY],
                                        IDS_MSG_SYSTEM_Q_VERIFY,
                                        MB_ICONQUESTION |
                                        MB_YESNO |
                                        MB_MOVEABLE) )
               break;

             /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
             /*Ё Ensure "controls" indicate QUEUE being processed            Ё*/
             /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
             SpySetTargetIsWindow(SpyInstance, BOOL_FALSE);

             bWindowIsObject = BOOL_FALSE;

             /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
             /*Ё Ensure target is the SYSTEM MESSAGE QUEUE                   Ё*/
             /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
             SpyUnSetTarget(SpyInstance);   /* discard previous HWND/HMQ      */

             SpySetTarget(SpyInstance,
                          NULLH,            /* no specific HWND...            */
                          NULLH);           /* if HMQ == NULL, then SYSTEM    */

             /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
             /*Ё Inform user what we've done                                 Ё*/
             /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
             UpdateSpyTitle(hwndFrame,
                            spyCurrent = tQueueSystem);
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё User want to select an OBJECT window for SPYing...               Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_SELECT_OBJECT:
        {
           static OW_DATA OWData =
           {                            /* these are FIELD ORDER dependent!   */

             NULLH,                     /* hwndNotify:   dynamically filled   */
             PMSPY_OW_RESULT,           /* uNotifyMsg:   dialog result MSG    */
             BOOL_TRUE,                 /* display DESKTOP windows?           */
             BOOL_TRUE                  /* display OBJECT  windows?           */
           };

           OWData.hwndNotify = hwnd;                           /* result to ME! */
           OWData.pfnCurrent = (PFONTNAME)iniAgent.szListFont; /* current FONT*/
           OWData.hmqSPY     = hmqSPY;                         /* agent's HMQ */

           WinDlgBox(HWND_DESKTOP,
                     hwnd,
                     ObjectDlgProc,
                     hmodNLS,
                     ID_OW_DLG,
                     &OWData);
        }
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё "turn off" SPYing.....                                           Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_DESELECT:

             SpyUnSetTarget(SpyInstance);
             bWindowIsObject = BOOL_FALSE;

             UpdateSpyTitle(hwndFrame,
                            spyCurrent = tStandard);

             WinSendMsg(hwnd,
                        PMSPY_ADD_TO_LIST,
                        MPFROMP(Strings[IDS_STAT_DESELECTED]),
                        MPFROM2SHORT(Color_Status, LIST_IS_TEXT) );
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Clear the contents of the message list                           Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_O_CLEAR:
             ClearMessageList();
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё "Thaw" the message list (enable message additions)               Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_O_THAW:
             slmState = SLM_THAWED;
             WinSendMsg(hwnd,
                        PMSPY_ADD_TO_LIST,
                        MPFROMP(Strings[IDS_STAT_LIST_ON]),
                        MPFROM2SHORT(Color_Status, LIST_IS_TEXT) );
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё "Freeze" the message list (disable message additions)            Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_O_FREEZE:
             slmState = SLM_FROZEN;
             WinSendMsg(hwnd,
                        PMSPY_ADD_TO_LIST,
                        MPFROMP(Strings[IDS_STAT_LIST_OFF]),
                        MPFROM2SHORT(Color_Status, LIST_IS_TEXT) );
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Close the LOG file                                               Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_CLOSELOG:
             WinSendMsg(hwnd, WM_COMMAND, MPFROMSHORT(ID_O_STOPLOG), NULL);
             fclose(pLogFile);
             pLogFile = (FILE *)NULL;
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Open the LOG file                                                Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_OPENLOG:
        case ID_F_OPENLOG_SNAPSHOT:

          FileDlg           = calloc(sizeof(FILEDLG), 1);

          FileDlg->cbSize   = sizeof(FILEDLG);
          FileDlg->fl       = FDS_SAVEAS_DIALOG | FDS_ENABLEFILELB | FDS_CENTER;
          FileDlg->pszTitle = Strings[IDS_LOGFILE_OPEN];
          strcpy(FileDlg->szFullFile, iniAgent.szLog);

          WinFileDlg(HWND_DESKTOP, hwnd, FileDlg);

          if (FileDlg->lReturn != DID_OK) /* other than 'OK' pushbutton used? */
          {
            free(FileDlg);
            break;
          } /* endif */

          strcpy(szFileName,     FileDlg->szFullFile);
          strcpy(iniAgent.szLog, szFileName);     /* save for next OPEN request */

          free(FileDlg);

          /* if file exists but they don't want to replace, quit */
          if ( ( _access(szFileName, 0) == 0) &&      /* already exist? */
               (MsgDisplay(hwnd,
                           swcntrl.szSwtitle,
                           Strings[IDS_MSG_LOG_EXISTS],
                           IDS_MSG_LOG_EXISTS,
                           MB_ICONQUESTION | MB_YESNO | MB_MOVEABLE,
                           szFileName ) != MBID_YES) )
            break;

          pLogFile = fopen(szFileName, "w");

          /****************************************************************
          * If necessary, "snapshot" the current messages in the SPY list
          ****************************************************************/
          if (SHORT1FROMMP(mp1) == ID_F_OPENLOG_SNAPSHOT)
            WinSendMsg(hwnd, PMSPY_SNAPSHOT, NULL, NULL);

          /*** N O T E:  falling through...... ***/

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Start logging to the LOG file                                    Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_O_STARTLOG:
             bCurrentlyLogging = BOOL_TRUE;

             if (SpyQueryTargetWindow(SpyInstance) != SPY_BAD_HWND )
             {
               WinSendMsg(hwnd,
                          PMSPY_ADD_TO_LIST,
                          MPFROMP(Strings[IDS_STAT_LOG_ON]),
                          MPFROM2SHORT(Color_Status, LIST_IS_TEXT) );

               if (SpyQueryTargetIsWindow(SpyInstance))
               {
                 fprintf(pLogFile,
                         Strings[IDS_FMT_SPY_WINDOW],
                         SpyQueryTargetWindow(SpyInstance));
               }
               else
               {
                 hmqTarget = SpyQueryTargetQueue(SpyInstance);

                 fprintf(pLogFile,
                         hmqTarget == NULLH
                         ? Strings[IDS_FMT_SPY_QUEUE_SYSTEM]
                         : Strings[IDS_FMT_SPY_QUEUE],
                         hmqTarget);
               } /* endif */
               fputc('\n', pLogFile);
             } /* endif */
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Stop logging to the LOG file                                     Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_O_STOPLOG:
          bCurrentlyLogging = BOOL_FALSE;
          WinSendMsg(hwnd,
                     PMSPY_ADD_TO_LIST,
                     MPFROMP(Strings[IDS_STAT_LOG_OFF]),
                     MPFROM2SHORT(Color_Status, LIST_IS_TEXT) );
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Append profile to current message set                            Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_APPENDPROFILE:
             ProcessProfileFile(hwnd,
                                hwndFrame,
                                pSpyData,
                                BOOL_FALSE,
                                PromptForFile,
                                NULL);
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Replace current message set with new profile                     Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_OPENPROFILE:
             ProcessProfileFile(hwnd,
                                hwndFrame,
                                pSpyData,
                                BOOL_TRUE,
                                PromptForFile,
                                NULL);
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Save current message set in a profile                            Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_SAVEPROFILE:

          FileDlg           = calloc(sizeof(FILEDLG), 1);

          FileDlg->cbSize   = sizeof(FILEDLG);
          FileDlg->fl       = FDS_SAVEAS_DIALOG | FDS_ENABLEFILELB | FDS_CENTER;
          FileDlg->pszTitle = Strings[IDS_PROFILE_SAVE];
          strcpy(FileDlg->szFullFile, iniAgent.szProfile);

          WinFileDlg(HWND_DESKTOP, hwnd, FileDlg);

          if (FileDlg->lReturn != DID_OK)                  /* 'OK' used?      */
          {
            free(FileDlg);
            break;
          } /* endif */

          strcpy(szFileName,         FileDlg->szFullFile);
          strcpy(iniAgent.szProfile, szFileName);

          free(FileDlg);

          if ( ( _access(szFileName, 0) == 0) &&      /* already exist?       */
               (MsgDisplay(hwnd,                      /* wanna overwrite?     */
                           swcntrl.szSwtitle,
                           Strings[IDS_MSG_PROFILE_EXISTS],
                           IDS_MSG_PROFILE_EXISTS,
                           MB_ICONQUESTION | MB_YESNO | MB_MOVEABLE,
                           szFileName ) != MBID_YES) )
            break;

          SaveProfile(hwndFrame, pSpyData, szFileName);
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Replace the current message set with the "default" profile       Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_OPENDEFAULT:
            ProcessProfileFile(hwnd, hwndFrame, pSpyData, BOOL_TRUE,  UseDefault, NULL);
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Add "default" profile to the current message set                 Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_APPENDDEFAULT:
            ProcessProfileFile(hwnd, hwndFrame, pSpyData, BOOL_FALSE, UseDefault, NULL);
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Browse the "default" profile                                     Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_O_BROWSEDEFAULT:

             WinSendMsg(hwndIPF,                    /* hwnd: IPF */
                        HM_DISPLAY_HELP,            /* msg:  display panel */
                        MPFROMSHORT(PID_DEFAULT_PROFILE), /* mp1:  which panel */
                        MPFROMSHORT(HM_RESOURCEID)); /* mp2: mp1 is ID */

        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Include all current messages                                     Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_INCLUDE:
          IncludeAllMsgs( pSpyData, BOOL_TRUE,  Color_Asis );
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Exclude all current messages                                     Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_EXCLUDE:
          IncludeAllMsgs( pSpyData, BOOL_FALSE, Color_Asis );
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Selectively include/exclude messages                             Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_SELECTIVE:

           pSpyData->pfnLB = (PFONTNAME)iniAgent.szListFont; /* current FONT*/

           WinDlgBox( HWND_DESKTOP,
                      hwnd,
                      (PFNWP)FilterDlgProc,
                      hmodNLS,
                      IDD_FILTER,
                      pSpyData);
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Selectively include/exclude message groups                       Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_GROUP:

           pSpyData->pfnLB = (PFONTNAME)iniAgent.szListFont; /* current FONT*/

           WinDlgBox( HWND_DESKTOP,
                      hwnd,
                      (PFNWP)GroupDlgProc,
                      hmodNLS,
                      ID_GRP_DLG,
                      pSpyData);
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё "Spy" specific HWND                                              Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case IDD_WND:
          SpySetTargetIsWindow(SpyInstance, BOOL_TRUE);

          if ( SpyQueryTargetWindow(SpyInstance) != SPY_BAD_HWND)
            UpdateSpyTitle(hwndFrame,
                           spyCurrent = (SPY_TITLES)
                                        (bWindowIsObject ? tObject
                                                         : tWindow),
                           SpyQueryTargetWindow(SpyInstance));
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё "Spy" specific HMQ (allows many HWNDs to be "seen")              Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case IDD_QUE:
          SpySetTargetIsWindow(SpyInstance, BOOL_FALSE);

          if ( SpyQueryTargetQueue(SpyInstance) != SPY_BAD_HMQ)
            UpdateSpyTitle(hwndFrame,
                           spyCurrent = tQueue,
                           SpyQueryTargetQueue(SpyInstance));
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё toggle "Include undefined WM_USER msg" flag                      Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_INC_WM_USER:
             bIncludeUndefinedWM_USER ^= BOOL_TRUE;  /* flip-flop current value */
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё toggle "Include undefined msg" flag                              Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_F_INC_UNDEFINED:
             bIncludeUndefinedMsgs ^= BOOL_TRUE;  /* flip-flop current value */
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё toggle "Auto scroll list" flag                                   Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_O_SCROLL:
             bAutoScrollList ^= BOOL_TRUE;  /* flip-flop current value */
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Notes dialog                                                     Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_O_NOTES:
        {
           static NOTES_DATA NotesData =
           {
             NULLH,                     /* hwndNotify:   dynamically filled */
             PMSPY_NOTES_RESULT,        /* uNotifyMsg:   dialog result MSG */

             BOOL_FALSE,                /* fUseData:     use dialog data? */

             { "" }                     /* szText:       initially NULL */
           };

           NotesData.hwndNotify = hwnd; /* return result to ME! */

           WinDlgBox( HWND_DESKTOP,
                      hwnd,
                      (PFNWP)NotesDlgProc,
                      hmodNLS,
                      DLG_NOTES,
                      &NotesData);
        } break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Save the options data                                            Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_O_SAVEOPTS:
          WinSendMsg(hwnd, WM_SAVEAPPLICATION, NULL, NULL);
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё FONT dialog                                                      Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        case ID_O_FONT:
        {
           static FONT_DATA FontData =
           {
             NULLH,                     /* hwndNotify:   dynamically filled */
             PMSPY_FONT_RESULT,         /* uNotifyMsg:   dialog result MSG */

             BOOL_FALSE                 /* fUseData:     use dialog data? */
           };

           FontData.hwndNotify = hwnd;                 /* return result to ME! */
           FontData.pfnList    = pFontList;            /* pass list of FONTS */
           FontData.pfnCurrent = (PFONTNAME)iniAgent.szListFont;  /* current FONT */

           WinDlgBox( HWND_DESKTOP,
                      hwnd,
                      (PFNWP)FontDlgProc,
                      hmodNLS,
                      DLG_FONT,
                      &FontData);
        }
        break;

        /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
        /*Ё Pulldown we don't care about...but check if some IPF work to do! Ё*/
        /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
        default:
            HandleIPF(hwnd, msg, mp1, mp2, &wpResult);

      }
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё User pressed mouse button #1..                                       Ё*/
    /*Ё                                                                      Ё*/
    /*Ё - if they are 'selecting' a window, determine which window is now    Ё*/
    /*Ё   under the POINTER and begin "spying"                               Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_BUTTON1DOWN:

         if (bSelecting)
         {
           /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
           /*Ё Was the last window valid?                                    Ё*/
           /*Ё                                                               Ё*/
           /*Ё - ignore is so                                                Ё*/
           /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
           if (hLast == hIllegal)
           {
              WinAlarm(HWND_DESKTOP, WA_ERROR );
           }
           else
           {
             bSelecting = BOOL_FALSE;

             WinSetPointer(HWND_DESKTOP, hOld);
             WinSetCapture(HWND_DESKTOP, NULLH);

             ptrPos.x = (LONG)SHORT1FROMMP(mp1);
             ptrPos.y = (LONG)SHORT2FROMMP(mp1);

             WinMapWindowPoints(hwnd, HWND_DESKTOP, (PPOINTL)&ptrPos, 1);

             hwndTarget = WinWindowFromPoint(HWND_DESKTOP,
                                             (PPOINTL)&ptrPos,
                                             BOOL_TRUE);

             hmqTarget  = (HMQ)WinQueryWindowULong(hwndTarget, QWL_HMQ);

             SpySetTarget(SpyInstance, hwndTarget, hmqTarget);
             bWindowIsObject = BOOL_FALSE;

             UpdateSpyTitle(hwndFrame,
                            spyCurrent = (SPY_TITLES)
                                         (SpyQueryTargetIsWindow(SpyInstance)
                                          ? tWindow
                                          : tQueue),
                            SpyQueryTargetIsWindow(SpyInstance)
                            ? hwndTarget
                            :  hmqTarget);
           }
         }
         else
           wpResult = WinDefWindowProc(hwnd, msg, mp1, mp2);
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё User pressed mouse button #2..                                       Ё*/
    /*Ё                                                                      Ё*/
    /*Ё - if they are 'selecting' a window, display the window class, handle,Ё*/
    /*Ё   and queue that is under the POINTER                                Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_BUTTON2DOWN:

         if (bSelecting)
         {
           ptrPos.x = (LONG)SHORT1FROMMP(mp1);
           ptrPos.y = (LONG)SHORT2FROMMP(mp1);

           WinMapWindowPoints(hwnd, HWND_DESKTOP, (PPOINTL)&ptrPos, 1);

           hwndTarget = WinWindowFromPoint(HWND_DESKTOP,
                                           (PPOINTL)&ptrPos, BOOL_TRUE);
            hmqTarget = (HMQ)WinQueryWindowULong(hwndTarget, QWL_HMQ);

           WinQueryClassName(hwndTarget, sizeof(szText), szText);

           UpdateSpyTitle(hwndFrame,
                          spyCurrent = tSpyWho,
                          TranslateClassName(szText), hwndTarget, hmqTarget);
         }
         else
           wpResult = WinDefWindowProc(hwnd, msg, mp1, mp2);
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё User double clicked button #2...                                     Ё*/
    /*Ё                                                                      Ё*/
    /*Ё - if they are 'selecting' a window, cancel operation                 Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_BUTTON2DBLCLK:

         if (bSelecting)
         {
           bSelecting = BOOL_FALSE;

           WinSetPointer(HWND_DESKTOP, hOld);
           WinSetCapture(HWND_DESKTOP, NULLH);

           UpdateSpyTitle(hwndFrame,
                          spyCurrent = tStandard);
         }
         else
           wpResult = WinDefWindowProc(hwnd, msg, mp1, mp2);
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё The mouse has moved....                                              Ё*/
    /*Ё                                                                      Ё*/
    /*Ё - if they are 'selecting' a window insure that our POINTER is used   Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_MOUSEMOVE:

      if (bSelecting)
      {
        ptrPos.x = (LONG)SHORT1FROMMP(mp1);
        ptrPos.y = (LONG)SHORT2FROMMP(mp1);

        WinMapWindowPoints(hwnd, HWND_DESKTOP, (PPOINTL)&ptrPos, 1);

        hwndTarget = WinWindowFromPoint(HWND_DESKTOP,
                                        (PPOINTL)&ptrPos, BOOL_TRUE);
         hmqTarget = (HMQ)WinQueryWindowULong(hwndTarget, QWL_HMQ);

        hLast = (hmqTarget == hmqSPY)                    /* this PMSPY?      */
                ? hIllegal
                : hSpy;

        WinSetPointer(HWND_DESKTOP, hLast);
      }
      else
        wpResult = WinDefWindowProc(hwnd, msg, mp1, mp2);
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Re-Paint the client area                                             Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_PAINT:

         hps = WinBeginPaint( hwnd, NULLH, &rectl );

         WinFillRect(hps, &rectl, SYSCLR_WINDOW );

         WinEndPaint( hps );
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Notes dialog result                                                  Ё*/
    /*Ё                                                                      Ё*/
    /*Ё Mp1: PNOTES_DATA                                                     Ё*/
    /*Ё Mp2: NULL (not used)                                                 Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case PMSPY_NOTES_RESULT:
    {
       PNOTES_DATA pNotes = PVOIDFROMMP(mp1);

       if (pNotes->fUseData)
       {
         WinSendMsg(hwnd,
                    PMSPY_ADD_TO_LIST,
                    MPFROMP(pNotes->szText),
                    MPFROM2SHORT(Color_Notes, LIST_IS_TEXT) );
       }
    }
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё FONT dialog result                                                   Ё*/
    /*Ё                                                                      Ё*/
    /*Ё Mp1: PFONT_DATA                                                      Ё*/
    /*Ё Mp2: NULL (not used)                                                 Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case PMSPY_FONT_RESULT:
    {
       PFONT_DATA pData = (PFONT_DATA) mp1;

       if (pData->fUseData)
       {
         SetListboxFont(hwndMsgList,
                        strcpy(&iniAgent.szListFont[0],
                               (PSZ) pData->pfnSelected) );
       }
    }
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё OBJECT Window dialog result                                          Ё*/
    /*Ё                                                                      Ё*/
    /*Ё Mp1: POW_DATA                                                        Ё*/
    /*Ё Mp2: NULL (not used)                                                 Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case PMSPY_OW_RESULT:
    {
       POW_DATA pOW = (POW_DATA) mp1;

       if (pOW->fUseData)
       {
          /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
          /*Ё Switch to SPYing this OBJECT window...                         Ё*/
          /*Ё                                                                Ё*/
          /*Ё - clear old SPYee                                              Ё*/
          /*Ё - set   new SPYee                                              Ё*/
          /*Ё - update title                                                 Ё*/
          /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
          SpyUnSetTarget(SpyInstance);

          SpySetTarget(SpyInstance, pOW->hwndOW, pOW->hmqOW);

          if ( bWindowIsObject = pOW->fHwndIsOBJECT )
          {
            spyCurrent = (SPY_TITLES)(pOW->fSpyHWND ? tObject : tQueue);
          }
          else
          {
            spyCurrent = (SPY_TITLES)(pOW->fSpyHWND ? tWindow : tQueue);
          }

          SpySetTargetIsWindow(SpyInstance, pOW->fSpyHWND);

          UpdateSpyTitle(hwndFrame,
                         spyCurrent,
                         pOW->fSpyHWND
                         ? pOW->hwndOW
                         : pOW->hmqOW);
       }
    }
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Save current PMSPY setting for this "agent"                          Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_SAVEAPPLICATION:

         WriteSpyINI( hwndFrame,                  /* agent's frame HWND */
                      SpyInstance + 1,            /* one-origin "agent" */
                      &iniAgent);                 /* agent's data       */
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Special one-time initialization processing                           Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_CREATE:
    {
      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Determine all possible Fixed FONTs....                             Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      pFontList = DetermineFixedFonts( hwnd );

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Initialize SPY data for Processing                                 Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      InitializeSpyData(pSpyData);

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Load the 'default' PROFILE                                         Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      ProcessProfileFile(hwnd, hwndFrame, pSpyData, BOOL_TRUE, UseDefault, NULL);

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Load our special POINTER for selecting windows                     Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      hSpy = WinLoadPointer(HWND_DESKTOP, hmodNonNLS, ID_SPY_POINTER);

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Load "illegal operation" POINTER for selecting windows             Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      hLast    =
      hIllegal = WinQuerySysPointer(HWND_DESKTOP, SPTR_ILLEGAL, BOOL_FALSE);

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Gather info about this client area's FRAME windows                 Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      hwndFrame= WinQueryWindow(hwnd, QW_PARENT);
      hwndMenu = WinWindowFromID(hwndFrame, FID_MENU);

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Gather info about this client area's message queue                 Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      hmqSPY = (HMQ)WinQueryWindowULong(hwnd, QWL_HMQ);

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Initiate IPF support                                               Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      if ( hwndIPF = InitializeIPF(hwndFrame,
                                   ID_AB_HELP,
                                   szHelpFile,
                                   Strings[IDS_HELP_TITLE_MAIN]) )
        MsgDisplayShouldEnableHELP();

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Create PM SPY message listbox                                      Ё*/
      /*Ё                                                                    Ё*/
      /*Ё - set to non-porportional font                                     Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      hwndMsgList=WinCreateWindow(hwnd,
                                  WC_LISTBOX,
                                  "classPMSPYlist",
                                  WS_VISIBLE   | WS_TABSTOP |
                                  LS_OWNERDRAW | LS_NOADJUSTPOS | LS_HORZSCROLL,
                                  0, 0, 0, 0,
                                  hwnd,
                                  HWND_TOP,
                                  ID_SPYLIST,
                                  NULL,
                                  NULL);

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Set operational defaults                                           Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      MaxNbrOfMessages   = atol(Controls[CTL_MAX_MESSAGES]);
      MaxDeleteItems     = atol(Controls[CTL_MAX_DELETEITEM]);

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Our basic PM requirements are complete...time to register          Ё*/
      /*Ё with our DLL resident support team                                 Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      if ( SpyDllVersion() != DLLVERSION )   /* compatible .DLL and .EXE? */
      {
        MsgDisplay(HWND_DESKTOP,
                   Strings[IDS_TITLE],
                   Strings[IDS_FMT_LEVEL],
                   IDS_FMT_LEVEL,
                   MB_CUACRITICAL | MB_CANCEL,
                   DLLVERSION,
                   SpyDllVersion());

        WinPostMsg(hwndFrame, WM_CLOSE, NULL, NULL);

        break;
      }

      /* can we register? */

      if ( (SpyInstance = SpyRegister(hwnd,                /* CLIENT gets data */
                                      PMSPY_MSG_FROM_HOOK))/* in this MSG */
           == SPY_BAD_SPYEE )
      {
        MsgDisplay(HWND_DESKTOP,
                   Strings[IDS_TITLE],
                   Strings[IDS_MSG_TOO_MANY_SPIES],
                   IDS_MSG_TOO_MANY_SPIES,
                   MB_CUACRITICAL| MB_CANCEL,
                   SpyQueryDllRegisterMax() );

        WinPostMsg(hwndFrame, WM_CLOSE, NULL, NULL);

        break;
      }

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Get default or saved AGENT data                                    Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      ReadSpyINI(hwndFrame, SpyInstance + 1, &iniAgent);

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Process any command line arguments...                              Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      { INT  j;

        for (j = 1;      /* Initialize (skip [0] which is program name */
             j < ArgC;   /* While more left to check */
             j++)        /* Iterate to next one */
        {
          strupr( ArgV[j] );

          if ( (strncmp(ArgV[j],
                        Strings[IDS_PARM_PROFILE],
                        strlen(Strings[IDS_PARM_PROFILE])) == 0) ||
               (strncmp(ArgV[j],
                        Strings[IDS_PARM_APPEND],
                        strlen(Strings[IDS_PARM_APPEND])) == 0) )
          {
            strcpy(szFileName, ArgV[j]+strlen(Strings[IDS_PARM_APPEND]));

            ProcessProfileFile(hwnd,
                               hwndFrame,
                               pSpyData,
                               strncmp( ArgV[j],
                                        Strings[IDS_PARM_APPEND],
                                        strlen(Strings[IDS_PARM_APPEND])) == 0
                                        ? BOOL_FALSE
                                        : BOOL_TRUE,
                               UsePassedFile,
                               szFileName);

          }
          else
          {
            /* unknown command line parameter will be ignored */ ;
          }
        } /* endfor */

      }

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё finally, some final PM housekeeping                                Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      WinSetWindowPos(hwndFrame,
                      HWND_TOP,
                      iniAgent.swpAgent.x,
                      iniAgent.swpAgent.y,
                      iniAgent.swpAgent.cx,
                      iniAgent.swpAgent.cy,
                      SWP_MOVE | SWP_SIZE | SWP_ZORDER | SWP_ACTIVATE | SWP_SHOW);

      sprintf(swcntrl.szSwtitle,
              Strings[IDS_FMT_TITLE],
              Strings[IDS_TITLE], SpyInstance + 1);

      UpdateSpyTitle(hwndFrame,
                     spyCurrent = tStandard);

      swcntrl.hwnd = hwndFrame;

      hSwitch = WinAddSwitchEntry((PSWCNTRL)&swcntrl);

      /* default to "Spying" on the window... */
      SpySetTargetIsWindow(SpyInstance, BOOL_TRUE);

      /********************************************************************
      * finally, queue message for later final start-up processing
      *********************************************************************/
      WinPostMsg(hwnd, PMSPY_STARTUP, NULL, NULL);
    }
    break;

    /********************************************************************
    * Do special non-WM_CREATE start-up processing...
    ********************************************************************/
    case PMSPY_STARTUP:
    {
         /********************************************************************
         * This is required since changing the LISTBOX font doesn't
         * work properly at WM_CREATE time
         ********************************************************************/
         SetListboxFont(hwndMsgList, iniAgent.szListFont);
    }
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё We don't need to handle any other messages...                        Ё*/
    /*Ё                                                                      Ё*/
    /*Ё If this isn't an IPF message, let PM do it's default "thing"         Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    default:
         if ( !HandleIPF(hwnd, msg, mp1, mp2, &wpResult) )
           wpResult = WinDefWindowProc(hwnd, msg, mp1, mp2);

  } /* endswitch */

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Return result of this MSG's processing...                              Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  return( wpResult );
}
