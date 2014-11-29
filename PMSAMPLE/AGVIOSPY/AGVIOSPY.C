/*---------------------------------------------------------------------------+
+----------------------------------------------------------------------------+
|                                                                            |
| GAVIOSPY                                                                   |
|                                                                            |
| Program to demonstrate VioGlobalReg usage                                  |
+-------------------------------------+--------------------------------------+
| Version: 1.0                        |   Marc Fiammante (FIAMMANT at LGEVM2)|
+-------------------------------------+--------------------------------------+
|                                                                            |
+-------------------------------------+--------------------------------------+
| History:                                                                   |
| --------                                                                   |
|                                                                            |
| created: Marc Fiammante October 1990                                       |
+---------------------------------------------------------------------------*/
#define INCL_WIN
#include  "gviospy.h"
/*************************************************************************/
/* Vio related            variables                                      */
/*************************************************************************/
/*************************************************************************/
/* Main proc dialog related variables                                    */
MRESULT EXPENTRY  FrameWindowProc(HWND hwnd,USHORT  msg,MPARAM mp1,MPARAM mp2 );
HAB        hab;
HMQ        hmq;
HWND       hwndFrame;
HWND       hwndClient;
HWND       hwndSession[16];      /* Per session Window handle          */
HWND       hwndSessionClient[16];
/*************************************************************************/
/* AVIO proc dialog related variables                                    */
MRESULT    EXPENTRY  AvioWindowProc      (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
#define    NUMATTR         3     /* Numattr value for 4-byte PS        */
#define    PSWIDTH         80
#define    PSDEPTH         25
HDC        hdcVio[16];           /* Per session Window device context handle  */
HVPS       hpsVio[16];           /* Per session AVIO Presentation space handle*/
USHORT     usPsWidth;            /* Presentation space width in AVIO units    */
USHORT     usPsDepth;            /* Presentation space depth in AVIO units    */
USHORT     usWndWidth;           /* Window width in AVIO units                */
USHORT     usWndDepth;           /* Window Depth in AVIO units                */
/*----------------------------------------------------------*/
/*-- External variables updated in the GlobalReg DLL -------*/
typedef    CHAR    SLINE[80];
typedef    SLINE   SCREEN[25];
extern     SCREEN  pascal  Sessions[];
extern     USHORT  pascal  XCursor[]; /* Session cursor position           */
extern     USHORT  pascal  YCursor[];                /*                                   */
extern     HMQ     pascal  AvioHmq; /* Spying queue                      */
extern     BOOL    pascal  WroteToSess[];
/*----------------------------------------------------------*/
HFILE      hwndkbd;                     /* Keyboard Handle                 */
/*----------------------------------------------------------*/
BOOL      SessionVisible[16]={FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,
                              FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};

/*----------------------------------------------------------------------*/
/*---------------------  Start of main procedure  ----------------------*/
FILE       *Debug;
void cdecl main(void)
{
  ULONG  flCreate;
  QMSG qmsg;
  USHORT  rc;
  USHORT  Session;
  CHAR    SessionTitle[80];
  STATUSDATA Status;
  HACCEL hAccel;
  USHORT     usTemp;                        /* temp variable Open KBD      */
//  Debug=fopen("DEBUG.DAT","w");
//  fprintf(Debug,"Start 1\n");
  hab = WinInitialize( NULL );          /* Initialize PM                */
  hmq = WinCreateMsgQueue( hab, 0 );    /* Create a message queue       */
  WinRegisterClass(                     /* Register window class        */
     hab,                               /* Anchor block handle          */
     "FRAME",                           /* Window class name            */
     FrameWindowProc,                    /* Address of window procedure  */
     CS_SIZEREDRAW,                     /* Class style                  */
     0                                  /* No extra window words        */
     );
  WinRegisterClass(                     /* Register window class        */
     hab,                               /* Anchor block handle          */
     "AVIO",                            /* Window class name            */
     AvioWindowProc,                    /* Address of window procedure  */
     CS_SIZEREDRAW,                     /* Class style                  */
     0                                  /* No extra window words        */
     );

  flCreate = FCF_STANDARD &             /* Set frame control flags to   */
             ~FCF_SHELLPOSITION;        /* standard except for shell    */
                                        /* positioning.                 */

  hwndFrame = WinCreateStdWindow(
              HWND_DESKTOP,            /* Desktop window is parent     */
              0L,                      /* No frame styles              */
              &flCreate,               /* Frame control flag           */
              "FRAME",                 /* Client window class name     */
              "",                      /* No window text               */
              0L,                      /* No special class style       */
               NULL,                    /* Resource is in .EXE file     */
               ID_WINDOW,               /* Frame window identifier      */
               &hwndClient              /* Client window handle         */
                   );
  WinSetWindowPos( hwndFrame,           /* Shows and activates frame    */
                   HWND_TOP,            /* window at position 100, 100, */
                   100, 100, 300, 300,  /* and size 200, 200.           */
                   SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW
                 );
  WinSetAccelTable(hab,NULL,NULL);
  WinLoadAccelTable(hab,NULL,ID_WINDOW,&hAccel);
  WinSetAccelTable(hab,hAccel,hwndFrame);
  /*--------------------------------------------------------------------*/
  /* Create 16 frame window to display the Fullscreen session if exist  */
  for (Session=0;Session<16;Session++ ) {
//    fprintf(Debug,"Start session %d\n",Session);
    sprintf(SessionTitle,"Fullscreen Session %d",Session);
    flCreate = FCF_TITLEBAR | FCF_SIZEBORDER | FCF_SYSMENU
              | FCF_MINMAX | FCF_ICON;   /* Set AVIO  windows frame   */
                                         /* control flags             */
    hwndSession[Session] = WinCreateStdWindow(
                hwndClient,              /* Frame   window is parent */
                0L,                      /* No frame styles           */
                &flCreate,               /* Frame control flag        */
                "AVIO",                  /* Client window class name  */
                SessionTitle,            /* Fullscreen # title        */
                0L,                      /* No special class style    */
                NULL,                    /* Resource is in .EXE file  */
                ID_SESSIONSTART+Session, /* Session Window identifier */
                &hwndSessionClient[Session] /* Client window handle   */
                );
    /*----------------------------------------------------------------*/
    /* Now get session validity and Show only active fullscreens      */
    /* DesSetSession will give us a rc                                */
    Status.Length =sizeof(STATUSDATA);
    Status.SelectInd=0;
    Status.BondInd=0;
    rc=DosSetSession(Session,&Status);
    if ((rc==ERROR_SMG_INVALID_SESSION_ID)||
      (rc==ERROR_SMG_NO_SESSIONS)       ||
      (rc==ERROR_SMG_SESSION_NOT_FOUND )||
      (WroteToSess[Session]==FALSE)        ) {
         WinSetWindowPos( hwndSession[Session],/* Hide the session  */
               HWND_BOTTOM,
               20*Session, 20*Session,
               300, 100,
               SWP_HIDE
                );
            SessionVisible[Session]=FALSE;
    } else {
            WinSetWindowPos( hwndSession[Session],  /* Shows and activates frame    */
                     HWND_TOP,
                     20*Session, 20*Session,
                     300, 100,
                     SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW
                  );
            SessionVisible[Session]=TRUE;
    }
    if (hwndSession[Session]==(HWND)NULL) DosBeep(200,100);
  } /* endfor */

//  fprintf(Debug,"Open Kbd\n" );
  /*--------------------------------------------------------------------*/
  /* Get the Keyboard Device driver handle                              */
  DosOpen2("KBD$", (PHFILE)&hwndkbd, &usTemp,
                           0L,
                           FILE_READONLY,
                           FILE_OPEN,
                         (ULONG)OPEN_ACCESS_READONLY |
                         OPEN_SHARE_DENYREADWRITE |
                         OPEN_FLAGS_FAIL_ON_ERROR,
                         (PEAOP)NULL,
                         0L);
  /*--------------------------------------------------------------------*/
  /* Now start the display                                              */
  if (hwndFrame!=(HWND)NULL) {
//  fprintf(Debug,"Frame\n" );
         AvioHmq=hmq;
        /* Main message-processing loop - get and dispatch messages until   */
        /* WM_QUIT received                                                 */
//  fprintf(Debug,"Loop\n" );
        while( WinGetMsg( hab, &qmsg, (HWND)NULL, 0, 0 ) )
          WinDispatchMsg( hab, &qmsg );
         WinDestroyWindow( hwndFrame );
  } /* endif */
  AvioHmq=NULL;
//  fprintf(Debug,"End\n" );
  WinDestroyMsgQueue( hmq );
  WinTerminate( hab );
//  fclose(Debug);
  DosExit( 1, 0 );
}
/*--------------------------------------------------------------------*/
/* Main frame client windowproc it will receive messages              */
/* From the GlobalRef DLL                                             */
MRESULT EXPENTRY FrameWindowProc(HWND hwnd,USHORT  msg,MPARAM mp1,MPARAM mp2 )
{
 HPS     hps;
 RECTL   rcl;
 USHORT  command,rc;
 STATUSDATA Status;
 switch ( msg )
     {
      /*--------------------------------------------*/
      /*  This message comes from the GlobalReg DLL */
      case WM_USER+1:
        /*-----------------------------------------------------------------*/
        /* Now get session validity and Show only if it is valid fullscreen*/
        Status.Length =sizeof(STATUSDATA);
        Status.SelectInd=0;
        Status.BondInd=0;
        rc=DosSetSession(SHORT1FROMMP(mp1),&Status);
        if ((rc==ERROR_SMG_INVALID_SESSION_ID)||
            (rc==ERROR_SMG_NO_SESSIONS)       ||
            (rc==ERROR_SMG_SESSION_NOT_FOUND )  ) {
            WinSetWindowPos( hwndSession[SHORT1FROMMP(mp1)],/* Hide the session  */
                     HWND_BOTTOM,
                     20*SHORT1FROMMP(mp1), 20*SHORT1FROMMP(mp1),
                     300, 100,
                     SWP_MINIMIZE | SWP_DEACTIVATE | SWP_HIDE
                  );
            SessionVisible[SHORT1FROMMP(mp1)]=FALSE;
        } else {
            if (!SessionVisible[SHORT1FROMMP(mp1)]) {
// fprintf(Debug,"Activate Local Session%d\n",SHORT1FROMMP(mp1));
              WinSetWindowPos( hwndSession[SHORT1FROMMP(mp1)],  /* Shows and activates frame    */
                       HWND_TOP,
                       20*SHORT1FROMMP(mp1), 20*SHORT1FROMMP(mp1),
                       300, 100,
                       SWP_SIZE | SWP_MOVE |  SWP_SHOW
                    );
// fprintf(Debug,"After Show     Local Session%d\n",SHORT1FROMMP(mp1));
              WinSetActiveWindow(HWND_DESKTOP,hwndSession[SHORT1FROMMP(mp1)]);
// fprintf(Debug,"After Activate Local Session%d\n",SHORT1FROMMP(mp1));
              WinSetFocus(HWND_DESKTOP,hwndSessionClient[SHORT1FROMMP(mp1)]);
// fprintf(Debug,"After Set Focus      Session%d\n",SHORT1FROMMP(mp1));
              SessionVisible[SHORT1FROMMP(mp1)]=TRUE;
           }
           else WinSendMsg(hwndSession[SHORT1FROMMP(mp1)],WM_USER+2,mp1,mp2);
        } /* endif */
        break;


     case WM_CLOSE:
         WinPostMsg( hwnd, WM_QUIT, 0L, 0L );
         break;
     case WM_PAINT:
// fprintf(Debug,"Paint main \n");
         hps = WinBeginPaint( hwnd, (HPS)NULL, &rcl );
         WinFillRect( hps, &rcl,CLR_BLUE  );/* Fill invalid rectangle       */
         WinEndPaint( hps );
         break;

     case WM_COMMAND:
      command = SHORT1FROMMP(mp1);      /* Extract the command value    */
      switch (command)
      {
        case ID_EXITPROG:
// fprintf(Debug,"Exit main \n");
          WinPostMsg( hwnd, WM_CLOSE, 0L, 0L );
          break;
        default:
          return WinDefWindowProc( hwnd, msg, mp1, mp2 );
      }
      break;

     default:
         return( WinDefWindowProc( hwnd, msg, mp1, mp2 ));
         break;
     }

 return( (MRESULT)FALSE );
}
MRESULT EXPENTRY AvioWindowProc(HWND hwndAvio,USHORT  msg,MPARAM mp1,MPARAM mp2 )
{
 HPS     hps;
 RECTL   rcl;
 USHORT  prty;
 SHORT   handle;
 USHORT  LocalSession;
 char    mro[40],mri[40];
 struct  MONDATA { USHORT     MonFlagWord;
                   KBDKEYINFO kd;
                   USHORT     KbdDDFlagWord;} KeyPacket;
 typedef struct _KEYSHIFT {
                   USHORT Shift;
                   UCHAR  Nls; } KEYSHIFT;
 KEYSHIFT Keyshift;
 UCHAR Interim;
 USHORT rc;
 STATUSDATA Status;
 LocalSession=WinQueryWindowUShort(WinQueryWindow(hwndAvio,QW_PARENT,FALSE),
                                   QWS_ID)-ID_SESSIONSTART;

 switch ( msg )
     {

     case WM_USER+2:
        WinInvalidateRegion( hwndAvio, NULL, FALSE );
        break;
     case WM_CREATE:
         /*****************************************************************/
         /* Create AVIO PS to go with the client area.                    */
         /*                                                               */
         /* This initializes the global structure DevCell, which defines  */
         /* the AVIO character cell size.                                 */
         /*****************************************************************/
         /*****************************************************************/
         /* Open window DC for client area.                                       */
         /*****************************************************************/

         hdcVio[LocalSession] = WinOpenWindowDC( hwndAvio );

         /*****************************************************************/
         /* Create a NUMATTR+1 byte/cell Presentation Space.                      */
         /*****************************************************************/
         usPsWidth   = PSWIDTH;
         usPsDepth   = PSDEPTH;

         /*************************************************************************/
         /* One size of Presentation Space is created for all modes.              */
         /* Even in browse mode, filesize is no indication of the number of       */
         /* lines or other Presentation Space layout requirements.                */
         /*************************************************************************/

          VioCreatePS(  (PHVPS)&hpsVio[LocalSession], usPsDepth, usPsWidth, 0, NUMATTR, (HVPS)0 );
          VioAssociate( hdcVio[LocalSession], hpsVio[LocalSession] );
         break;



     case WM_DESTROY:
     /* Tidy up resources when the window is destroyed.   */
         VioAssociate( (HDC)NULL, hpsVio[LocalSession] );
         VioDestroyPS( hpsVio[LocalSession] );
         break;


     case WM_CHAR:
        /*-----------------------------------------------------------------*/
        /* Now get session validity and Play char only if  valid fullscreen*/
          Status.Length =sizeof(STATUSDATA);
          Status.SelectInd=0;
          Status.BondInd=0;
          rc=DosSetSession(LocalSession,&Status);
          if ((rc==ERROR_SMG_INVALID_SESSION_ID)||
              (rc==ERROR_SMG_NO_SESSIONS)       ||
              (rc==ERROR_SMG_SESSION_NOT_FOUND )  ) {
               if (SessionVisible[SHORT1FROMMP(mp1)]) {
                  WinSetWindowPos( hwndSession[LocalSession],/* Hide the session  */
                        HWND_BOTTOM,
                        0,0,
                        0,0,
                        SWP_MINIMIZE | SWP_DEACTIVATE | SWP_HIDE
                      );
                  SessionVisible[LocalSession]=FALSE;
               }
               DosBeep(1400,20);
               break;
          }
        /*-----------------------------------------------------------------*/
        /* Now get session validity and Play char only if  valid fullscreen*/
        /*------- Monitor only the down transitions and held */
         if ((SHORT1FROMMP(mp1)&KC_KEYUP)==0x0000) {
             DosGetPrty(2,&prty,0);
             /* Initialise the keyboard device driver monitor packet */
             memset(&KeyPacket,0,sizeof(KeyPacket));
             /* Get character and scan code */
             KeyPacket.kd.chChar  =LOUCHAR(SHORT1FROMMP(mp2));
             KeyPacket.kd.chScan  =HIUCHAR(SHORT1FROMMP(mp2));
             /* 17 May 91 Include mandatory scan code in Monitor Flag Word */
             KeyPacket.MonFlagWord=MAKEUSHORT(0,KeyPacket.kd.chScan);
             /* Get the interim flag */
             DosDevIOCtl((PCHAR)&Interim, 0L, 0x0072, 0x0004, hwndkbd);
             KeyPacket.kd.fbStatus=Interim;
             /* Complete the interim flag */
             if (SHORT1FROMMP(mp1)&KC_VIRTUALKEY) {
                KeyPacket.kd.fbStatus|=0x02;
             } /* endif */
             KeyPacket.kd.fbStatus|=0x40;
             /* Get the shift status */
             DosDevIOCtl((PCHAR)&Keyshift, 0L, 0x0073, 0x0004, hwndkbd);
             KeyPacket.kd.fsState =Keyshift.Shift;
// fprintf(Debug,"Status  = %2.2X\n",KeyPacket.kd.fbStatus);
// fprintf(Debug,"Shift   = %4.4X\n",KeyPacket.kd.fsState);
// fprintf(Debug,"Scancode= %2.2X\n",KeyPacket.kd.chScan);
// fprintf(Debug,"char    = %2.2X\n",KeyPacket.kd.chChar);
             DosSetPrty(2,3,0,0);
             /* Write through monitoring to target  session */
             DosMonOpen("\\DEV\\KBD$",&handle);
             mro[0]=40;
             mro[1]=0;
             mri[0]=40;
             mri[1]=0;
             rc=DosMonReg(handle,mri,mro,2,LocalSession);
             if (rc==0) {
                DosMonWrite(mro,(char *)&KeyPacket,sizeof(struct MONDATA));
             } else {
                DosBeep(1400,50);
             } /* endif */
             DosMonClose(handle);
             DosSetPrty(2,(prty&0xff)>>8,prty&0xff,0);
          } /* endif */
         break;
     /*-----------------------------------------------------------------*/
     /* Now get Fullscreen content to the right AVIO space              */
     case WM_PAINT:
// fprintf(Debug,"Paint Local Session %d\n",LocalSession);
           hps = WinBeginPaint( hwndAvio, (HPS)NULL, &rcl );
           /***************************************************************/
           /* Clipping will occur to window boundary, so don't be         */
           /* selective                                                   */
           /***************************************************************/
           VioWrtCharStr((PCH)&Sessions[LocalSession][0][0],2000,0,0,hpsVio[LocalSession]);
           VioSetCurPos(XCursor[LocalSession],YCursor[LocalSession],hpsVio[LocalSession]);
           VioShowPS( usPsDepth, usPsWidth, 0, hpsVio[LocalSession] );
           WinEndPaint( hps );
           break;
     case WM_SIZE:
// fprintf(Debug,"Size  Local Session %d\n",LocalSession);
     /***************************************************************/
     /* If the window is associated with an AVIO Presentation Space,*/
     /* WM_SIZE must call WinDefWindowProc before accessing the     */
     /* window.                                                     */
     /***************************************************************/
          WinDefAVioWindowProc( hwndAvio, msg, mp1, mp2 );

     default:
         return( WinDefWindowProc( hwndAvio, msg, mp1, mp2 ));
         break;
     }

 return( (MRESULT)FALSE );
}
