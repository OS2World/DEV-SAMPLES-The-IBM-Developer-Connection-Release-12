/*---------------------------------------------------------------------------+
| Pmjrn   Window Procedure
|
| Change History:
| ---------------
|
+---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
| Includes                                                                   |
+---------------------------------------------------------------------------*/

#define INCL_WINSTDFILE  /* Window Standard File Functions       */
#include "pmjrn.h"                   /* Resource symbolic identifiers*/


#define PM_11   0x0A0A                 /* WinQueryVersion for 1.1 */
#define PM_12   0x140A                 /* WinQueryVersion for 1.2 */


/***************************************************************************
*
* Routine:    MsgDisplay
*
* Function:   Display MessageBox with variable substitution
*
***************************************************************************/

USHORT MsgDisplay( HWND   hWnd,
                   PSZ    strCap,
                   PSZ    strFormat,
                   USHORT mb_id,
                   USHORT style,
                   ...)

{
  CHAR          MessageText[256];                       /* Message box output           */
  va_list       arg_ptr;                                /* Variable argument list ptr   */

 /***************************************************************************************/
 /*                                                                                     */
 /*                                MAINLINE                                             */
 /*                                                                                     */
 /***************************************************************************************/

  va_start(arg_ptr, style);                             /* Start variable list access   */
  return( vsprintf(                                     /* Format the message box output*/
                   MessageText,                         /*  Buffer to put it in         */
                   strFormat,                           /*  Format string               */
                   arg_ptr) == -1                       /*  Parameters                  */

          ? MID_ERROR

          : WinMessageBox(                              /* Display message box          */
                           HWND_DESKTOP,                /*   Desktop handle             */
                           hWnd,                        /*   Client window handle       */
                           MessageText,                 /*   Main body of message text  */
                           strCap,                      /*   Caption of Box             */
                           mb_id,                       /*   ID of message box (Help)   */
                           style | MB_SYSTEMMODAL)      /*   Style of message box       */
        );
}

/*---------------------------------------------------------------------------
| Toggle menu 'check' on exclusive items
+---------------------------------------------------------------------------*/
static VOID ToggleMenuChecking(register USHORT decheck, register USHORT check)
{
  WinSendMsg(hwndMenu, MM_SETITEMATTR,        /* de-check first */
             MPFROM2SHORT(decheck, TRUE),
             MPFROM2SHORT(MIA_CHECKED, 0) );
  WinSendMsg(hwndMenu, MM_SETITEMATTR,        /* then check */
             MPFROM2SHORT(check, TRUE),
             MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED) );
}

/*---------------------------------------------------------------------------+
| Spy Window Procedure                                                       |
+---------------------------------------------------------------------------*/

static HELP_DATA    helpActionBar = {                  /* @C5A - HELP dialog data */
                                      IDT_AB_HELP,     /* HELP resource */
                                      IDS_HELP_TITLE_MAIN
                                    };

static HELP_DATA    helpNews      = {                  /* @C5A - NEWS dialog data */
                                      IDT_NEWS,        /* HELP resource */
                                      IDS_HELP_TITLE_NEWS
                                    };

static union { ULONG     u;
               QVERSDATA q;
             } versionPM = {0};
/*---------------------------------------------------------------------------
| Thread declarations
+---------------------------------------------------------------------------*/
#define STACKSIZE 4096
void _Optlink RecordThread(void *);
void _Optlink PlayThread(void *);
ULONG  Action,RetCode,NumberBytes;
TID  HookTID;
HAB  HabSpy;
HWND                 TraceTopHwnd;
HWND                 TraceHwnd;
HWND                 DeskTopHwnd;
HWND                 FileDlgHwnd;
extern QMSG  SharedMsg;


MRESULT EXPENTRY SpyWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

  HPS                  hps;                    /* Presentation space handle    */
  RECTL                rectl;                  /* Window rectangle             */
  POINTL pt;                            /* String screen coordinates    */
  switch(msg)
  {
    case WM_CLOSE:
      if (hwndHook == 0 && hwndQueue == 0) {
        /* we are not active so quit without asking */
      } else {
        if ( MsgDisplay( hwnd,
                         Strings[IDS_TITLE],
                         Strings[IDS_FMT_OK_TO_EXIT],
                         0,
                         MB_CUAWARNING | MB_OKCANCEL
                       ) == MBID_OK ) {
          if (RecordOnQueue(TRUE,1)) {
             RecordOnQueue(FALSE,2);
          } /*  Not more recording queue  */
          if (PlayOnQueue(TRUE,1)) {
             PlayOnQueue(FALSE,2);
          } /*  Not more recording queue  */
          hwndHook = (HWND)0;
          hwndQueue = (HMQ)0;
        } else {
          /* resume application */
          return(0);
        } /* endif */
      } /* endif */
      /* tell main window to terminate */
      WinPostMsg(hwndFrame, PMSPY_QUIT_NOTICE, (ULONG) 0, (ULONG) 0);
      return(0);
      break;

    case WM_INITMENU:
      switch (LOUSHORT(mp1)) {
        case ID_AB_JOURNALON:
          if (hwndHook == 0 && hwndQueue == 0) {
            ToggleMenuChecking(ID_SELECT, ID_DESELECT);
          } else {
            ToggleMenuChecking(ID_DESELECT, ID_SELECT);
          } /* endif */
          break;
      } /* endswitch */
      break;

    case WM_COMMAND:
      switch (SHORT1FROMMP(mp1)) {
        case ID_T_RECORD:
          if (SpyInstalled(Installed)) {
             if ( RecordOnQueue(TRUE,1)) {
             } else {
                if (hwndHook==0) {
                    WinAlarm( HWND_DESKTOP, WA_ERROR);
                } else { /* If a window has been selected then position it */
                   SpyWndHandle(hwndHook,2);
                   DeskTopHwnd=WinQueryDesktopWindow(hab,0);
                   TraceHwnd=hwndHook;
                   FileDlgHwnd=hwnd;
                   while ((TraceHwnd!=DeskTopHwnd)&&
                          (TraceHwnd!=HWND_DESKTOP))
                     {
                         TraceTopHwnd=TraceHwnd;
                         TraceHwnd=WinQueryWindow(TraceTopHwnd,
                                                   QW_PARENT);

                         WinAlarm( HWND_DESKTOP, WA_WARNING);
                   }


                   WinSetWindowPos(TraceTopHwnd,HWND_TOP,0,0,0,0,
                                   SWP_MOVE |SWP_RESTORE);
                   WinInvalidateRegion(hwndHook, 0, FALSE );
                }
                /* Ask if we are ready to set the Hook if Yes Start thread */
                if ( MsgDisplay( hwnd,
                                 Strings[IDS_TITLE],
                                 Strings[IDS_FMT_OK_TO_HOOK],
                                 0,
                                 MB_ICONQUESTION | MB_OKCANCEL
                                 ) == MBID_OK ) {
                   RecordOnQueue(TRUE,2);

                   HookTID=_beginthread( RecordThread, 0, 0x4000, 0);
                } /* end Message Set Hook */
             } /*  Recording queue  */
          } else {
          } /*  spyinstalled() */
          break;
        case ID_T_PLAYBACK:
          if (SpyInstalled(Installed)) {
             if ( PlayOnQueue(TRUE,1)) {
             } else {
                if (hwndHook==0) {
                    WinAlarm( HWND_DESKTOP, WA_ERROR);
                } else { /* If a window has been selected then position it */
                   DeskTopHwnd=WinQueryDesktopWindow(hab,0);
                   TraceHwnd=hwndHook;
                   FileDlgHwnd=hwnd;
                   while ((TraceHwnd!=DeskTopHwnd)&&
                          (TraceHwnd!=HWND_DESKTOP))
                      {
                          TraceTopHwnd=TraceHwnd;
                          TraceHwnd=WinQueryWindow(TraceTopHwnd,
                                                       QW_PARENT);

                            WinAlarm( HWND_DESKTOP, WA_WARNING);
                      }


                   WinSetWindowPos(TraceTopHwnd,HWND_TOP,0,0,0,0,
                                   SWP_MOVE | SWP_RESTORE);
                   WinInvalidateRegion(hwndHook, 0, FALSE );
                }
                /* Ask if we are ready to set the Hook if Yes Start thread */
                if ( MsgDisplay( hwnd,
                                 Strings[IDS_TITLE],
                                 Strings[IDS_FMT_OK_TO_HOOK],
                                 0,
                                 MB_ICONQUESTION | MB_OKCANCEL
                                 ) == MBID_OK ) {
                   PlayOnQueue(TRUE,2);

                   HookTID=_beginthread( PlayThread, 0, 0x4000, 0);
                } /* end Message Set Hook OK */
             } /*  Recording queue  */
          } else {
          } /*  spyinstalled() */
          break;
        case ID_AB_EXIT:
          WinPostMsg(hwnd, WM_CLOSE, 0L, 0L);
          break;
        case ID_HELP:
          WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)HelpWindowProc,      /* @C5C */
                    0, IDD_HELP, (PVOID)&helpActionBar);
          break;
        case ID_AB_NEWS:
          WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)HelpWindowProc,      /* @C5C */
                    0, IDD_HELP, (PVOID)&helpNews);
          break;
        case ID_SELECT:
          /* capture the mouse */
          bSelecting = TRUE;
          WinSetCapture(HWND_DESKTOP, hwnd);
          hOld = WinQueryPointer(HWND_DESKTOP);
          WinSetPointer(HWND_DESKTOP, hSpy);
          hwndHook = (HWND)0;
          hwndQueue = (HMQ)0;
          /* force the window in the background */
          WinSetWindowPos(hwndFrame, HWND_BOTTOM, 0, 0, 0, 0, SWP_ZORDER);
          break;
        case ID_DESELECT:
          if (RecordOnQueue(TRUE,1)) {
             RecordOnQueue(FALSE,2);
          } /*  Not more recording  */
          if (PlayOnQueue(TRUE,1)) {
             PlayOnQueue(FALSE,2);
          } /*  Not more playing    */
          hwndHook = (HWND)0;
          hwndQueue = (HMQ)0;
          WinSetWindowText(hwndFrame, (PSZ)szWndTitle);
          break;
        default:
          return WinDefWindowProc( hwnd, msg, mp1, mp2 );
      } /* endswitch */
      break;


    case WM_MOUSEMOVE: /* This to keep the pointer shape while pointing a window */
      if (bSelecting) {
          return((MPARAM)0);
      }
      else return(WinDefWindowProc(hwnd, msg, mp1, mp2));
      break;
    case WM_BUTTON1DOWN: /* select for spying */
      if (bSelecting) {
        bSelecting = FALSE;        WinSetPointer(HWND_DESKTOP, hOld);
        WinSetCapture(HWND_DESKTOP, 0);
        ptrPos.x = (LONG)SHORT1FROMMP(mp1);
        ptrPos.y = (LONG)SHORT2FROMMP(mp1);
        WinMapWindowPoints(hwnd, HWND_DESKTOP, (PPOINTL)&ptrPos, 1);
        hwndHook = WinWindowFromPoint(HWND_DESKTOP, (PPOINTL)&ptrPos,
                                      TRUE);
        hwndQueue = (HMQ)WinQueryWindowULong(hwndHook, QWL_HMQ);
        WinSetWindowText(hwndFrame, szText);
      } else {
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
      } /* endif */
      break;

    case WM_ERASEBACKGROUND:
      /******************************************************************/
      /* Return TRUE to request PM to paint the window background       */
      /* in SYSCLR_WINDOW.                                              */
      /******************************************************************/
      return (MRESULT)( TRUE );

    case WM_PAINT:
      hps = WinBeginPaint( hwnd, 0, &rectl );
      GpiSetColor( hps, SYSCLR_WINDOWTEXT );   /* color of the text,    */
      GpiSetBackColor( hps, SYSCLR_WINDOW );   /* its background and    */
      GpiSetBackMix( hps, BM_OVERPAINT );      /* how it mixes,         */
                                               /* and draw the string...*/
      pt.x = 10; pt.y = 80;             /* Set the text coordinates,    */
      GpiCharStringAt( hps, &pt,35L,"If playing or recording you Should ");
      pt.x = 10; pt.y = 60;             /* Set the text coordinates,    */
      GpiCharStringAt( hps, &pt,35L,"point a window of the application  ");
      pt.x = 10; pt.y = 40;             /* Set the text coordinates,    */
      GpiCharStringAt( hps, &pt,35L,"because the stored messages contain");
      pt.x = 10; pt.y = 20;             /* Set the text coordinates,    */
      GpiCharStringAt( hps, &pt,35L,"absolute coordinates               ");
      WinEndPaint( hps );
      break;

    case WM_CREATE:
      /* Get information about which version of PM we're running on */
      versionPM.u = WinQueryVersion(hab);
      MaxNbrOfMessages = atoi(Strings[IDS_MAX_MESSAGES]);
      bSelecting = FALSE;
      hwndHook = (HWND)0;
      hwndQueue = (HMQ)0;
      /* default to "Spying" on the queue */
      return((MPARAM)0);
      break;

    default:
      return(WinDefWindowProc(hwnd, msg, mp1, mp2));
  } /* endswitch */

  return((MPARAM)0);
}

HFILE   JournalFile;
/**********************************************************************/
/*       WindowProc                                                   */
/**********************************************************************/
MRESULT EXPENTRY PmjrnRecordProc(hwnd, msg, mp1, mp2 )

HWND hwnd;                             /* window handle               */
ULONG msg;                            /* message                     */
MPARAM mp1;                            /* first parameter             */
MPARAM mp2;                            /* second parameter            */

{
   if (msg==WM_USER+1) {
       RetCode=DosWrite(JournalFile,&SharedMsg,sizeof(QMSG),&NumberBytes);
   } /* endif */
   return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}
/**********************************************************************/
/*       WindowProc                                                   */
/**********************************************************************/
MRESULT EXPENTRY PmjrnPlayProc(hwnd, msg, mp1, mp2 )

HWND hwnd;                             /* window handle               */
ULONG msg;                            /* message                     */
MPARAM mp1;                            /* first parameter             */
MPARAM mp2;                            /* second parameter            */

{
   if (msg==WM_USER+1) {
       RetCode=DosRead(JournalFile,&SharedMsg,sizeof(QMSG),&NumberBytes);
       if ((RetCode!=NO_ERROR)||(NumberBytes==0)) {
         DosBeep(400,50);
         DosBeep(600,50);
         PlayOnQueue(FALSE,2);
       }
   } /* endif */
   return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}
void _Optlink RecordThread(void * args)
{
  HAB habAsync;
  HMQ hmqAsync;
  HWND hwndDlg;
  BOOL    JrnFileOpen;
  HWND    hTargetWindow;
  QMSG qmsg;
  CHAR          szFileName[CCHMAXPATH];

  FILEDLG FileDlg;
  JrnFileOpen=FALSE;
  habAsync = WinInitialize(0);
  if ( (hmqAsync = WinCreateMsgQueue(habAsync,
                   atoi(Strings[IDS_MAX_PM_Q_SIZE]))) == 0) {
      WinTerminate(habAsync);
      _endthread();
  }
  memset(&FileDlg,0,sizeof(FILEDLG));
  FileDlg.cbSize=sizeof(FILEDLG);
  FileDlg.fl = FDS_HELPBUTTON | FDS_CENTER | FDS_SAVEAS_DIALOG  ;
  FileDlg.pszTitle="Journalling File Save" ;
  strcpy(FileDlg.szFullFile, szJournalPattern);  /* Initial path,     */
  hwndDlg = WinFileDlg(HWND_DESKTOP, HWND_DESKTOP, &FileDlg);

  if (hwndDlg && (FileDlg.lReturn == DID_OK)) {
    /**************************************************************/
    /* Upon successful return of a file, open it for reading and  */
    /* further processing                                         */
    /**************************************************************/
    strcpy(szFileName,FileDlg.szFullFile);  /* Initial path,     */
  } else {
    RetCode = WinMessageBox(HWND_DESKTOP, hwndFrame,
                 (PSZ)"No available file, Recording is stopped",
                 (PSZ)szWndTitle,
                 0,
                 MB_CANCEL | MB_MOVEABLE );
    RecordOnQueue(FALSE,2);
    if (JrnFileOpen) DosClose(JournalFile);
    WinDestroyMsgQueue(hmqAsync);
    WinTerminate(habAsync);
    _endthread();
  }

  RetCode=DosOpen(szFileName, (PHFILE)&JournalFile,
                      &Action,0L,0,18,66,(ULONG)0);
  if (RetCode==NO_ERROR) {
      JrnFileOpen=TRUE;
      if (RecordOnQueue(TRUE,1)) {
         WinSetHook(hab, 0, HK_JOURNALRECORD,(PFN)SpyJrnRecordHookProc, hSpyDll);
      }
    WinRegisterClass(                    /* Register window class       */
      habAsync,                           /* Anchor block handle         */
      "Record",                           /* Window class name           */
      PmjrnRecordProc,                    /* Address of window procedure */
      0L,                                 /* Class Style                 */
      0                                   /* No extra window words       */
                  );
     hTargetWindow=WinCreateWindow(        /* create an object window     */
       HWND_OBJECT,
       "Record",
       "Record",
       0L ,
       0, 0, 0, 0,
       HWND_DESKTOP,
       HWND_BOTTOM,
       1,                        /* Window identifier           */
       0,
       0                      );
      if (hTargetWindow==0) {
         DosBeep(1400,100);
      } /* endif */
      TargetWindow(hTargetWindow,2); /* Make handle available */
      /**********************************************************************/
      /* Get and dispatch messages from the application message queue       */
      /* until WinGetMsg returns FALSE, indicating a WM_QUIT message.       */
      /**********************************************************************/

      while( WinGetMsg( habAsync, &qmsg, 0, 0, 0 ) ) {
            WinDispatchMsg( habAsync, &qmsg );
      }
      TargetWindow(0,2); /* Make handle unavailable */
      WinDestroyWindow( hTargetWindow );    /* Tidy up...               */
  }
  if (JrnFileOpen) DosClose(JournalFile);
  WinDestroyMsgQueue(hmqAsync);
  WinTerminate(habAsync);
  _endthread();
}
void _Optlink PlayThread(void * args)
{
  HAB habAsync;
  HMQ hmqAsync;
  HWND hwndDlg;
  QMSG qmsg;
  BOOL    JrnFileOpen;
  BOOL    TrcFileOpen;
  HWND    hTargetWindow;
  QMSG Qmsg;
  CHAR          szFileName[CCHMAXPATH];

  FILEDLG FileDlg;
  JrnFileOpen=FALSE;
  TrcFileOpen=FALSE;
  habAsync = WinInitialize(0);
  if ( (hmqAsync = WinCreateMsgQueue(habAsync,
                   atoi(Strings[IDS_MAX_PM_Q_SIZE]))) == 0) {
      WinTerminate(habAsync);
      _endthread();
  }
  memset(&FileDlg,0,sizeof(FILEDLG));
  FileDlg.cbSize=sizeof(FILEDLG);
  FileDlg.fl = FDS_HELPBUTTON | FDS_CENTER | FDS_OPEN_DIALOG ;
  FileDlg.pszTitle="Journalling File Open" ;
  strcpy(FileDlg.szFullFile, szJournalPattern);  /* Initial path,     */
  hwndDlg = WinFileDlg(HWND_DESKTOP, HWND_DESKTOP, &FileDlg);

  if (hwndDlg && (FileDlg.lReturn == DID_OK)) {
    /**************************************************************/
    /* Upon successful return of a file, open it for reading and  */
    /* further processing                                         */
    /**************************************************************/
    strcpy(szFileName,FileDlg.szFullFile);  /* Initial path,     */
  } else {
     RetCode = WinMessageBox(HWND_DESKTOP, hwndFrame,
                  (PSZ)"No File selected, Playback is stopped",
                  (PSZ)szWndTitle,
                  0,
                  MB_CANCEL | MB_MOVEABLE );
     PlayOnQueue(FALSE,2);
     if (JrnFileOpen) DosClose(JournalFile);
     WinDestroyMsgQueue(hmqAsync);
     WinTerminate(habAsync);
    _endthread();
  }
  RetCode=DosOpen(szFileName, (PHFILE)&JournalFile,
                      &Action,0L,0,1,64,(ULONG)0);
  if (RetCode==NO_ERROR)
      RetCode=DosRead(JournalFile,&Qmsg,sizeof(QMSG),&NumberBytes);
  if (RetCode==NO_ERROR) {
      JrnFileOpen=TRUE;
      if (PlayOnQueue(TRUE,1)) {
         WinSetHook(hab, 0, HK_JOURNALPLAYBACK,(PFN)SpyJrnPlayHookProc, hSpyDll);
      }
      WinRegisterClass(                    /* Register window class       */
        habAsync,                           /* Anchor block handle         */
        "Play",                           /* Window class name           */
        PmjrnPlayProc,                    /* Address of window procedure */
        0L,                                 /* Class Style                 */
        0                                   /* No extra window words       */
                    );
      hTargetWindow=WinCreateWindow(        /* create an object window     */
         HWND_OBJECT,
         "Play",
         "Play",
         0L ,
         0, 0, 0, 0,
         HWND_DESKTOP,
         HWND_BOTTOM,
         1,                        /* Window identifier           */
         0,
         0                      );
      TargetWindow(hTargetWindow,2); /* Make handle available */
      /**********************************************************************/
      /* Get and dispatch messages from the application message queue       */
      /* until WinGetMsg returns FALSE, indicating a WM_QUIT message.       */
      /**********************************************************************/

      while( WinGetMsg( habAsync, &qmsg, 0, 0, 0 ) ) {
              WinDispatchMsg( habAsync, &qmsg );
      }
      DosBeep(400,50);
      TargetWindow(0,2); /* Make handle unavailable */
      DosBeep(450,50);
      WinDestroyWindow( hTargetWindow );    /* Tidy up...               */
      DosBeep(500,50);
  }
      DosBeep(550,50);
  if (JrnFileOpen) DosClose(JournalFile);
      DosBeep(600,50);
  WinDestroyMsgQueue(hmqAsync);
      DosBeep(650,50);
  WinTerminate(habAsync);
      DosBeep(700,50);
  _endthread();
}
