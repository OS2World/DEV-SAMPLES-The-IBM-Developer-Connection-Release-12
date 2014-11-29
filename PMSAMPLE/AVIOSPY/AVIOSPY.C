/*---------------------------------------------------------------------------+
+----------------------------------------------------------------------------+
|                                                                            |
| mousplay                                                                     |
|                                                                            |
| Program to demonstrate Playback hook usage                                 |
+-------------------------------------+--------------------------------------+
| Version: 1.0                        |   Marc Fiammante (FIAMMANT at LGEVM2)|
+-------------------------------------+--------------------------------------+
|                                                                            |
+-------------------------------------+--------------------------------------+
| History:                                                                   |
| --------                                                                   |
|                                                                            |
| created: Marc Fiammante September 1990                                     |
+---------------------------------------------------------------------------*/
#include  "viospy.h"
/*************************************************************************/
/* Vio related            variables                                      */
/*************************************************************************/
/*************************************************************************/
/* Main proc dialog related variables                                    */
/*************************************************************************/
MRESULT EXPENTRY  AvioWindowProc      (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
HAB        hab;
HMQ        hmq;
HWND       hwndFrame;
HWND       hwndClient;
#define   NUMATTR         3         /* Numattr value for 4-byte PS */
#define   PSWIDTH         80
HDC    hdcVio;                      /* Window device context handle     */
HVPS       hpsVio;                      /* AVIO Presentation space handle   */
USHORT usPsWidth;            /* Presentation space width in AVIO units  */
USHORT usPsDepth;            /* Presentation space depth in AVIO units  */
USHORT usWndWidth;           /* Window width in AVIO units              */
USHORT usWndDepth;           /* Window Depth in AVIO units              */
USHORT usVioBufLen;          /* Length of AVIO buffer                   */
PBYTE  pVioBuf;              /* Address of AVIO buffer                  */
typedef   CHAR   SLINE[80];
typedef   SLINE   SCREEN[25];
extern SCREEN    pascal Sessions[];
extern USHORT    pascal  Session;                /* DEBUG Session number              */
extern USHORT    pascal  XCursor;                /* Session cursor position           */
extern USHORT    pascal  YCursor;                /*                                   */
extern HMQ     pascal  AvioHmq; /* Spying queue                      */
typedef struct _Orig
   {
   SHORT x;
   SHORT y;
   }
ORIG;
typedef ORIG FAR *PORIG;
USHORT usTemp;                              /* temp variable              */
HFILE  hwndkbd;
FILE *Debug;
USHORT    SessID;
PID       ProcID;
STARTDATA StartData;
CHAR      ProgramName[80];
CHAR      Disks[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
/**********************  Start of main procedure  ***********************/
void cdecl main(void)
{
  ULONG  flCreate;
  QMSG qmsg;
  USHORT  rc,Disk,DirPathLen;
  ULONG   Logical;
  DosQCurDisk(&Disk,&Logical);
  ProgramName[0]=Disks[Disk-1];
  ProgramName[1]=':';
  ProgramName[2]='\\';
  DirPathLen=78;
  DosQCurDir(Disk,ProgramName+3,&DirPathLen);
  if (ProgramName[strlen(ProgramName)-1]!='\\') {
     strcat(ProgramName,"\\");
  } /* endif */
  strcat(ProgramName,"VIOSPYM.EXE");
  StartData.Length=sizeof(STARTDATA);/* length of data structure in bytes                */
  StartData.Related=1;       /* 0 = independent session, 1 = child session       */
  StartData.FgBg=1;          /* 0 = start in foreground, 1 = start in background */
  StartData.TraceOpt=0;      /* 0 = no trace, 1 = trace                          */
  StartData.PgmTitle="Spied Fullscreen";      /* address of program title                         */
  StartData.PgmName=ProgramName;     /* address of program name     */
  StartData.PgmInputs="";     /* input arguments                                  */
  StartData.TermQ=NULL;         /* address of program queue name                    */
  StartData.Environment=NULL;   /* environment string                               */
  StartData.InheritOpt=1;    /* where are handles and environment inherited from */
                           /* 0 = inherit from shell , 1 = inherit from caller */
  StartData.SessionType=1; /* Full screen session type               */
  StartData.IconFile=NULL;      /* address of icon definition                       */
  StartData.PgmHandle=NULL;     /* program handle                                   */
  StartData.PgmControl=0;    /* initial state of windowed application            */
  StartData.InitXPos=0;      /* x coordinate of initial session window           */
  StartData.InitYPos=0;      /* y coordinate of initial session window           */
  StartData.InitXSize=0;     /* initial size of x                                */
  StartData.InitYSize=0;     /* initial size of y                                */
  rc=DosStartSession(&StartData,&SessID,&ProcID);
  Debug=fopen("DEBUG.DAT","w");
  fprintf(Debug,"Start rc=%d Pgm=%s\n",rc,ProgramName);
  if (rc==0) {
      DosSelectSession(0,0L);    /* Switch us then to foreground */
      hab = WinInitialize( NULL );          /* Initialize PM                */
      hmq = WinCreateMsgQueue( hab, 0 );    /* Create a message queue       */
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
                   "AVIO",                  /* Client window class name     */
                   "",                      /* No window text               */
                   0L,                      /* No special class style       */
                   NULL,                    /* Resource is in .EXE file     */
                   ID_WINDOW,               /* Frame window identifier      */
                   &hwndClient              /* Client window handle         */
                   );
       WinSetWindowPos( hwndFrame,           /* Shows and activates frame    */
                       HWND_TOP,            /* window at position 100, 100, */
                       100, 100, 200, 200,  /* and size 200, 200.           */
                       SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW
                     );

       DosOpen2("KBD$", (PHFILE)&hwndkbd, &usTemp,
                           0L,
                           FILE_READONLY,
                           FILE_OPEN,
                           (ULONG)OPEN_ACCESS_READONLY |
                           OPEN_SHARE_DENYREADWRITE |
                           OPEN_FLAGS_FAIL_ON_ERROR,
                           (PEAOP)NULL,
                           0L);
       if (hwndFrame!=(HWND)NULL) {
         AvioHmq=hmq;
        /* Main message-processing loop - get and dispatch messages until   */
        /* WM_QUIT received                                                 */
        while( WinGetMsg( hab, &qmsg, (HWND)NULL, 0, 0 ) )
          WinDispatchMsg( hab, &qmsg );
         WinDestroyWindow( hwndFrame );
       } /* endif */
      AvioHmq=NULL;
      WinDestroyMsgQueue( hmq );
      WinTerminate( hab );
  } /* endif */
  fclose(Debug);
  DosExit( 1, 0 );
}
MRESULT EXPENTRY AvioWindowProc(HWND hwnd,USHORT  msg,MPARAM mp1,MPARAM mp2 )
{
 POINTL  ptl;
 HPS     hps;
 RECTL   rcl;
 ORIG    Origin;
 USHORT  command;
 USHORT  prty;
 SHORT   handle;
 char    mro[40],mri[40];
 struct  MONDATA { USHORT     MonFlagWord;
                   KBDKEYINFO kd;
                   USHORT     KbdDDFlagWord;} KeyPacket;
 typedef struct _KEYSHIFT {
     USHORT Shift;
     UCHAR  Nls;
  } KEYSHIFT;
  KEYSHIFT Keyshift;
 UCHAR Interim;

 switch ( msg )
     {

     case WM_USER+1:
        WinInvalidateRegion( hwnd, NULL, FALSE );
        break;
     case WM_CREATE:

         /*****************************************************************/
         /* Create AVIO PS to go with the client area.                    */
         /*                                                               */
         /* This initializes the global structure DevCell, which defines  */
         /* the AVIO character cell size.                                 */
         /*****************************************************************/
         /*************************************************************************/
         /* Open window DC for client area.                                       */
         /*************************************************************************/

         hdcVio = WinOpenWindowDC( hwnd );

         /*************************************************************************/
         /* Create a NUMATTR+1 byte/cell Presentation Space.                      */
         /*************************************************************************/
         usPsWidth   = PSWIDTH;
         usPsDepth   = 25;

         /*************************************************************************/
         /* One size of Presentation Space is created for all modes.              */
         /* Even in browse mode, filesize is no indication of the number of       */
         /* lines or other Presentation Space layout requirements.                */
         /*************************************************************************/

          VioCreatePS(  (PHVPS)&hpsVio, usPsDepth, usPsWidth, 0, NUMATTR, (HVPS)0 );
          VioAssociate( hdcVio, hpsVio );
          DosBeep(1500,100);
         break;



     case WM_DESTROY:
     /* Tidy up resources when the window is destroyed.   */
         VioAssociate( (HDC)NULL, hpsVio );
         VioDestroyPS( hpsVio );
         break;


     case WM_CLOSE:
         WinPostMsg( hwnd, WM_QUIT, 0L, 0L );
         break;
      case WM_CHAR:
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
             DosMonReg(handle,mri,mro,2,Session);
             DosMonWrite(mro,(char *)&KeyPacket,sizeof(struct MONDATA));
             DosMonClose(handle);
             DosSetPrty(2,(prty&0xff)>>8,prty&0xff,0);
          } /* endif */
         break;

     case WM_COMMAND:
      command = SHORT1FROMMP(mp1);      /* Extract the command value    */
      switch (command)
      {
        case ID_EXITPROG:
          WinPostMsg( hwnd, WM_CLOSE, 0L, 0L );
          break;
        default:
          return WinDefWindowProc( hwnd, msg, mp1, mp2 );
      }
      break;
     case WM_PAINT:
           hps = WinBeginPaint( hwnd, (HPS)NULL, &rcl );
           /***************************************************************/
           /* Clipping will occur to window boundary, so don't be         */
           /* selective                                                   */
           /***************************************************************/
           VioWrtCharStr((PCH)&Sessions[Session][0][0],2000,0,0,hpsVio);
           VioSetCurPos(XCursor,YCursor,hpsVio);
           VioShowPS( usPsDepth, usPsWidth, 0, hpsVio );
           WinEndPaint( hps );
           break;
     case WM_SIZE:
     /***************************************************************/
     /* If the window is associated with an AVIO Presentation Space,*/
     /* WM_SIZE must call WinDefWindowProc before accessing the     */
     /* window.                                                     */
     /***************************************************************/
          WinDefAVioWindowProc( hwnd, msg, mp1, mp2 );

     default:
         return( WinDefWindowProc( hwnd, msg, mp1, mp2 ));
         break;
     }

 return( (MRESULT)FALSE );
}
