/*************** whatis C Sample Program Source Code File (.C) ****************/
/*                                                                            */
/* PROGRAM NAME: whatis                                                       */
/* -------------                                                              */
/*  Presentation Manager Standard Window C Sample Program                     */
/*                                                                            */
/* WHAT THIS PROGRAM DOES:                                                    */
/* -----------------------                                                    */
/* Displays all information about the object under the mouse                  */
/* And the menu item text if any selected (button1 held down)                 */
/*                                                                            */
/*                                                                            */
/* WHAT YOU NEED TO COMPILE THIS PROGRAM:                                     */
/* --------------------------------------                                     */
/*                                                                            */
/*  REQUIRED FILES:                                                           */
/*  ---------------                                                           */
/*                                                                            */
/*    whatis.C       - Source code                                            */
/*    whatis.MAK     - Make file for this program                             */
/*    whatis.DEF     - Module definition file                                 */
/*    whatis.H       - Application header file                                */
/*    whatis.ICO     - Icon file                                              */
/*    whatis.L       - Linker automatic response file                         */
/*    whatis.RC      - Resource file                                          */
/*                                                                            */
/*    OS2.H          - Presentation Manager include file                      */
/*    STRING.H       - String handling function declarations                  */
/*    STRLIB.H       - Convertion handling function declarations              */
/*    STDIO.H        - I/O    handling function declarations                  */
/*                                                                            */
/*  REQUIRED LIBRARIES:                                                       */
/*  -------------------                                                       */
/*                                                                            */
/*    OS2.LIB        - Presentation Manager/OS2 library                       */
/*    LLIBCE.LIB     - Protect mode/standard combined large model C library   */
/*                                                                            */
/*  REQUIRED PROGRAMS:                                                        */
/*  ------------------                                                        */
/*                                                                            */
/*    IBM C Compiler                                                          */
/*    IBM Linker                                                              */
/*    Resource Compiler                                                       */
/*                                                                            */
/******************************************************************************/

#define INCL_DOS                        /* Selectively include          */
#define INCL_VIO                        /* Selectively include          */
#define INCL_GPIPRIMITIVES              /* Selectively include          */
#define INCL_WIN                        /* the PM header file           */

#include <os2.h>                        /* PM header file               */
#include <stdio.h>                      /* C/2 I/O    functions         */
#include <string.h>                     /* C/2 string functions         */
#include <stdlib.h>                     /* C/2 conver functions         */
#include "whatis.h"                     /* Resource symbolic identifiers*/


/************************************************************************/
/* Function prototypes                                                  */
/************************************************************************/
MRESULT EXPENTRY MyWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
BOOL EXPENTRY InputHookProc(HAB habSpy, PQMSG pQmsg, BOOL bRemove);
VOID EXPENTRY SendMsgHookProc(HAB habSpy, PSMHSTRUCT pSmh, BOOL bTask);
void cdecl main( void );

                                        /* Define parameters by type    */
HAB  hab;                               /* PM anchor block handle       */
HWND hwndClient;                      /* Client area window handle    */
HWND LastCreated;
CHAR ClassName[80];
CHAR MenuClass[10];
HWND Window;
HWND LastWindow=(HWND)0;
extern CREATESTRUCT    CreateStruct;
char   TheBuffer[256];
extern CHAR  SharedText[];
extern CHAR  SharedText1[];
extern CHAR  AvioText[];
extern CHAR  Directory[];
extern HWND  TargetWindow;
extern ULONG    SegSize;
extern PVOID    UserPtr;
extern USHORT   xCur;
extern USHORT   yCur;
extern USHORT   QUERYTEXT;
extern LONG     Handle0;
extern LONG     Handle1;
extern LONG     Handle2;
extern LONG     Handle3;
extern LHANDLE   Journal1;
extern BOOL      Success1;
extern BOOL      Success2;
extern ERRORID   Err1;
extern ERRORID   Err2;
extern ERRORID   Err3;
typedef struct _HEAPSUM { PID Pid;
                          USHORT Size; } HEAPSUM;
HEAPSUM HeapSum[256];
USHORT  HeapNumber=0;
HMODULE hDll;                           /* Spy module handle            */
HWND hwndFrame;                       /* Frame window handle          */
HWND SpyWindow;                       /* Frame window handle          */
MRESULT EXPENTRY MyDlgProc( HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY SpyProc( HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2 );
HMQ     hmq;                             /* Message queue handle         */
#define WM_ATOMFIRST 0xC000
/**********************  Start of main procedure  ***********************/
void main(  )
{
  QMSG qmsg;                            /* Message from message queue   */
  ULONG flCreate;                       /* Window creation control flags*/
  hab = WinInitialize( 0 );          /* Initialize PM                */
  hmq = WinCreateMsgQueue( hab, 0 );    /* Create a message queue       */
  /* -------------------------------------------------------------------*/
  /* Add a user message to get vio content -----------------------------*/
  QUERYTEXT=WinAddAtom(WinQuerySystemAtomTable(),"WITT_QUERY_TEXT");
  if (QUERYTEXT<WM_ATOMFIRST) {
     DosBeep(1400,20);
     DosBeep(1200,20);
     DosBeep(1000,20);
     DosBeep(1200,20);
     DosBeep(1400,20);
  } /* endif */
  WinRegisterUserMsg(hab,QUERYTEXT,
                             DTYP_LONG , RUM_IN,
                             DTYP_LONG , RUM_IN,
                             DTYP_LONG);

  WinRegisterClass(                     /* Register window class        */
     hab,                               /* Anchor block handle          */
     "MyWindow",                        /* Window class name            */
     MyWindowProc,                      /* Address of window procedure  */
     CS_SIZEREDRAW,                     /* Class style                  */
     0                                  /* No extra window words        */
     );
  sprintf(MenuClass,"#%d",WC_MENU);
  flCreate = FCF_STANDARD &             /* Set frame control flags to   */
             ~FCF_SHELLPOSITION;        /* standard except for shell    */
                                        /* positioning.                 */

   hwndFrame = WinCreateStdWindow(
               HWND_DESKTOP,            /* Desktop window is parent     */
               0L,                      /* No frame styles              */
               &flCreate,               /* Frame control flag           */
               "MyWindow",              /* Client window class name     */
               "",                      /* No window text               */
               0L,                      /* No special class style       */
               0,                    /* Resource is in .EXE file     */
               ID_WINDOW,               /* Frame window identifier      */
               &hwndClient              /* Client window handle         */
               );
  TargetWindow=hwndClient;
  WinSetWindowPos( hwndFrame,           /* Shows and activates frame    */
                   HWND_TOP,            /* window at position 100, 100, */
                   50, 50, 450, 250,  /* and size 200, 200.           */
                   SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW
                 );


  DosQueryModuleHandle( "WHATISD", (PHMODULE)&hDll);
  WinSetHook(hab, 0, HK_INPUT  , (PFN)InputHookProc, hDll);
  WinSetHook(hab, 0, HK_SENDMSG, (PFN)SendMsgHookProc, hDll);
/************************************************************************/
/* Get and dispatch messages from the application message queue         */
/* until WinGetMsg returns FALSE, indicating a WM_QUIT message.         */
/************************************************************************/
  while( WinGetMsg( hab, &qmsg, 0, 0, 0 ) )
    WinDispatchMsg( hab, &qmsg );
  WinReleaseHook(hab , 0, HK_INPUT  , (PFN)InputHookProc, hDll);
  WinReleaseHook (hab, 0, HK_SENDMSG, (PFN)SendMsgHookProc, hDll);
  TargetWindow=(HWND)0;

  WinDestroyWindow( hwndFrame );        /* Tidy up...                   */
  WinDestroyMsgQueue( hmq );            /* and                          */
  WinTerminate( hab );                  /* terminate the application    */
}
/***********************  End of main procedure  ************************/
/*********************  Start of window procedure  **********************/
MRESULT EXPENTRY MyWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  USHORT command;                       /* WM_COMMAND command value     */
  HPS    hps;                           /* Presentation Space handle    */
  RECTL  rc;                            /* Rectangle coordinates        */
  POINTL pt;                            /* String screen coordinates    */
  POINTL PointerPos;                    /* Position of pointer         */
  ULONG  WinId;
  ULONG  ClassId,Length;
  CHAR Buffer[400];
  SHORT ItemId;
  HWND  Active,Focus,Parent,Owner;
  LONG  HitTest;
  ULONG Style;
  PID Pid;
  TID Tid;
  HDC  WindowDC;
  HMQ  wHmq;
  USHORT Messages;
  MQINFO Mqinfo ;
  MRESULT Range;
  USHORT  Pos,Count,ResRes;
  PVOID   Ptr;
  ULONG  Info;
  switch( msg )
  {
   case WM_USER+WM_USER+1:
        WinInvalidateRect( hwnd, 0, TRUE );
        break;
   case WM_USER+WM_BUTTON2DOWN:
        Focus = WinQueryFocus(HWND_DESKTOP);
        WinPostMsg(Focus,QUERYTEXT,0,0);
        break;
   case WM_USER+WM_FOCUSCHANGE:
      WinInvalidateRect( hwnd, 0, TRUE );
      break;
   case WM_USER+WM_CREATE:
      LastCreated=WinWindowFromID(CreateStruct.hwndParent,CreateStruct.id);
      WinInvalidateRect( hwnd, 0, TRUE );
      break;
   case WM_USER+WM_MENUSELECT:
      WinInvalidateRect( hwnd, 0, TRUE );
      break;
    case WM_USER+WM_MOUSEMOVE:
      /******************************************************************/
      WinQueryPointerPos(HWND_DESKTOP,&PointerPos);
      Window=WinWindowFromPoint(HWND_DESKTOP,&PointerPos, TRUE);
      if (Window!=LastWindow) {
          WinInvalidateRect( hwnd, 0, TRUE );
      }
      break;
    case WM_COMMAND:
      /******************************************************************/
      /* When Exit is chosen, the application posts itself a WM_CLOSE   */
      /* message.                                                       */
      /******************************************************************/
      command = SHORT1FROMMP(mp1);      /* Extract the command value    */
      switch (command)
      {
        case ID_HEAP:
          WinDlgBox( HWND_DESKTOP,      /* Place anywhere on desktop    */
                     hwndFrame,         /* Owned by frame               */
                     MyDlgProc,         /* Address of dialog procedure  */
                     0,              /* Module handle                */
                     DLG_LISTBOX,       /* Dialog identifier in resource*/
                     0 );            /* Initialization data          */
          WinInvalidateRegion( hwnd, 0, FALSE ); /* Force a repaint  */

          break;
        case ID_EXITPROG:
          WinPostMsg( hwnd, WM_CLOSE, 0L, 0L );
          break;
        default:
          return WinDefWindowProc( hwnd, msg, mp1, mp2 );
      }
      break;
    case WM_ERASEBACKGROUND:
      /******************************************************************/
      /* Return TRUE to request PM to paint the window background       */
      /* in SYSCLR_WINDOW.                                              */
      /******************************************************************/
      return (MRESULT)( TRUE );
    case WM_PAINT:
      /******************************************************************/
      /* Window contents are drawn here in WM_PAINT processing.         */
      /******************************************************************/
                                        /* Create a presentation space  */
      hps = WinBeginPaint( hwnd, 0, &rc );
      rc.yBottom=30L;
      WinFillRect( hps, &rc,CLR_BACKGROUND  );/* Fill invalid rectangle       */
      GpiSetColor( hps, CLR_NEUTRAL );         /* colour of the text,   */
      GpiSetBackColor( hps, CLR_BACKGROUND );  /* its background and    */
      GpiSetBackMix( hps, BM_OVERPAINT );      /* how it mixes,         */
                                               /* and draw the string...*/
      ResRes=WinPostMsg(hwnd,WM_CHAR,0,0);
      ResRes=WinPostMsg(hwnd,WM_CHAR,0,0);
      ResRes=WinPostMsg(hwnd,WM_CHAR,0,0);
      Ptr=(PVOID)WinQueryWindowPtr(Window,QWP_PFNWP);
      pt.x = 10; pt.y = 230;             /* Set the text coordinates,    */
      sprintf(Buffer,"Jrn %p Succ1 %d Succ2 %d Err1 %p Err2 %p Err3 %p",
                       Journal1,
                       Success1,
                       Success2,
                       Err1,
                       Err2,
                       Err3);
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer);
      sprintf(Buffer,"Post result %d WindowProc %p,GreGetHandle 0=%p, 1=%p, 2=%p, 3=%p",ResRes,Ptr,Handle0,Handle1,Handle2,Handle3);
      pt.x = 10; pt.y = 210;             /* Set the text coordinates,    */
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer);
      sprintf(Buffer,"Spy Text is >%s< ",SharedText1);
      pt.x = 10; pt.y = 190;             /* Set the text coordinates,    */
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer);
      pt.x = 10; pt.y = 170;             /* Set the text coordinates,    */
      wHmq=(HMQ)WinQueryWindowULong(Window,QWL_HMQ);
      WinQueryQueueInfo(wHmq,&Mqinfo,sizeof(Mqinfo));
      sprintf(Buffer,"Pointed Queue handle is: %p ,Queue size %d,Dir : %s",wHmq,Mqinfo.cmsgs,Directory);
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer);
      pt.x = 10; pt.y = 150;             /* Set the text coordinates,    */

      WindowDC = WinQueryWindowDC(Window);
      Parent=WinQueryWindow(Window,QW_PARENT);
      Owner=WinQueryWindow(Window,QW_OWNER);
      WinQueryWindowProcess(Window,&Pid,&Tid);
      sprintf(Buffer,"Window Pointed handle is: %p ,Parent %p,Owner %p",Window,Parent,Owner);
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer);
      pt.x = 10; pt.y = 130;             /* Set the text coordinates,    */
      Style=WinQueryWindowULong(Window,QWL_STYLE);
      sprintf(Buffer,"Styl %4.4lX,Pid %d,DC=%p,Sel:%4.4X,Siz:%p",Style,Pid,WindowDC,
                      SELECTOROF(UserPtr), SegSize);
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer);
      Focus = WinQueryFocus(HWND_DESKTOP);
      Active = WinQueryActiveWindow(HWND_DESKTOP);
      pt.x = 10; pt.y = 110;             /* Set the text coordinates,    */
      sprintf(Buffer,"Handle of Active:%p , of Focus:%p        ",Active,Focus);
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer);
      pt.x = 10; pt.y = 90;             /* Set the text coordinates,    */
      WinQueryClassName(Window,80,ClassName);
      ClassId=atoi(ClassName+1);
      switch (ClassId) {
        case INT_WC_FRAME:
          sprintf(Buffer,"Window Class is %s : standard FRAME %-80s",ClassName," ");
         break;
        case INT_WC_TITLEBAR:
          sprintf(Buffer,"Window Class is %s : standard TITLEBAR %-80s",ClassName," ");
         break;
        case INT_WC_BUTTON:
          sprintf(Buffer,"Window Class is %s : standard BUTTON %-80s",ClassName," ");
         break;
        case INT_WC_ENTRYFIELD:
          sprintf(Buffer,"Window Class is %s : standard ENTRYFIELD %-80s",ClassName," ");
         break;
        case INT_WC_COMBOBOX:
          sprintf(Buffer,"Window Class is %s : standard COMBOBOX  %-80s",ClassName," ");
         break;
        case INT_WC_LISTBOX:
          Range=WinSendMsg(WinWindowFromID(Window,0xC001),SBM_QUERYRANGE,0,0);
          Pos  =(USHORT)WinSendMsg(WinWindowFromID(Window,0xC001),SBM_QUERYPOS,0,0);
          Count=(USHORT)WinSendMsg(Window,LM_QUERYITEMCOUNT ,0,0);
          sprintf(Buffer,"Window Class is %s : standard LISTBOX  Count %d;Visible Items First %d;Last %d %-80s",
                  ClassName,Count,Pos,Pos+Count-SHORT2FROMMR(Range)+SHORT1FROMMR(Range)-1,
                  " ");
         break;
        case INT_WC_MENU:
          sprintf(Buffer,"Window Class is %s : standard MENU  %-80s",ClassName," ");
         break;
        case INT_WC_MLE:
          sprintf(Buffer,"Window Class is %s : standard MLE  %-80s",ClassName," ");
         break;
        case INT_WC_SCROLLBAR:
          Range=WinSendMsg(Window,SBM_QUERYRANGE,0,0);
          Pos  =(USHORT)WinSendMsg(Window,SBM_QUERYPOS  ,0,0);

          sprintf(Buffer,"Window Class is %s : standard SCROLLBAR; range:%d,%d ; pos:%d  %-80s",ClassName,
                          SHORT1FROMMR(Range), SHORT2FROMMR(Range),Pos, " ");
         break;
        case INT_WC_STATIC:
          sprintf(Buffer,"Window Class is %s : standard STATIC  %-80s",ClassName," ");
         break;
      default:
          sprintf(Buffer,"Window Class is %s  %-80s",ClassName," ");
      } /* endswitch */
      Info=(ULONG)WinSendMsg(Window,WM_QUERYFRAMEINFO,0,0);
      if ( FI_FRAME & Info) {
         strcpy(Buffer," Is a true Frame !!!!!!!!! ");
      } /* endif */
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer);
      WinId=WinQueryWindowUShort(Window,QWS_ID);
      switch (WinId) {
      case FID_TITLEBAR:
         sprintf(Buffer,"Window id is TITLEBAR %-80s"," ");
         break;
      case FID_MENU:
         sprintf(Buffer,"Window id is FRAME MENU%-80s"," ");
         break;
      case FID_SYSMENU:
         sprintf(Buffer,"Window id is FRAME SYSMENU%-80s"," ");
         break;
      case FID_MINMAX:
         sprintf(Buffer,"Window id is FRAME MINMAX%-80s"," ");
         break;
      case FID_CLIENT:
         sprintf(Buffer,"Window id is FRAME CLIENT%-80s"," ");
         break;
      case FID_HORZSCROLL:
         sprintf(Buffer,"Window id is FRAME HORIZONTAL SCROLL BAR%-80s"," ");
         break;
      case FID_VERTSCROLL:
         sprintf(Buffer,"Window id is FRAME VERTICAL SCROLL BAR%-80s"," ");
         break;
      default:
          sprintf(Buffer,"Window id is %d %4.4X %-80s",WinId,WinId," ");
      } /* endswitch */
      pt.x = 10; pt.y = 70;             /* Set the text coordinates,    */
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer);
      if (ClassId==(USHORT)WC_MENU) {
         ItemId= (SHORT)WinSendMsg(Window, MM_QUERYSELITEMID,
                            (MPARAM)TRUE,
                             (MPARAM)0);
         if (ItemId!=MIT_NONE) {
            if (WinId==FID_MINMAX) {
               if      (ItemId==SC_MINIMIZE) sprintf(Buffer,"Menu Item is MINIMIZE");
               else if (ItemId==SC_RESTORE)  sprintf(Buffer,"Menu Item is RESTORE");
               else if (ItemId==SC_MAXIMIZE) sprintf(Buffer,"Menu Item is MAXIMIZE");
            } else {
               Length=(USHORT)WinSendMsg(Window,
                          MM_QUERYITEMTEXT,
                          MPFROM2SHORT(ItemId,79),
                          MPFROMP(SharedText));
               SharedText[Length]='\0';
               sprintf(Buffer,"Menu Item %d is %s %-80s",ItemId,SharedText," ");
            }
         } else {
            sprintf(Buffer,"No Menu item selected %-80s"," ");
         } /* endif */
      } else {
         WinQueryWindowText(Window, 79,SharedText);
         sprintf(Buffer,"Window Text is \"%s\" %-80s",SharedText," ");
         if (strcmp(SharedText,"OS/2 Window")==0) {
             strcpy(SharedText,"Salut");
             WinSetWindowText(Window,SharedText);
         } /* endif */
      } /* endif */
      pt.x = 10; pt.y = 50;             /* Set the text coordinates,    */
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer);
      sprintf(Buffer,"Avio: Cursor x=%d y=%d, First line  =%s %-80s",xCur,yCur,AvioText," ");
      pt.x = 10; pt.y = 30;             /* Set the text coordinates,    */
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer);
      sprintf(Buffer,"Saved Focus Handle :%p , Last Created Window :%p    ",
              WinQueryWindowULong(Window,QWL_HWNDFOCUSSAVE),LastCreated);
      pt.x = 10; pt.y = 10;             /* Set the text coordinates,    */
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer);
      LastWindow=Window;
      WinEndPaint( hps );                      /* Drawing is complete   */
      break;
    case WM_CLOSE:
      /******************************************************************/
      /* This is the place to put your termination routines             */
      /******************************************************************/
      WinPostMsg( hwnd, WM_QUIT, 0L, 0L );  /* Cause termination        */
      break;
    default:
      /******************************************************************/
      /* Everything else comes here.  This call MUST exist              */
      /* in your window procedure.                                      */
      /******************************************************************/

      return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  return FALSE;
}
/*****
*****************  End of window procedure  ***********************/
/********************* Start of dialog procedure  ***********************/
MRESULT EXPENTRY MyDlgProc( HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  CHAR PidBuffer[80];
  USHORT ItemId;
  switch ( msg )
  {
    case WM_INITDLG:
      /******************************************************************/
      /* PM sends a WM_INITDLG message when the dialog box is created   */
      /******************************************************************/
        /* Add item to the listbox                                   */
      for (ItemId=0;ItemId<HeapNumber;ItemId++) {
        sprintf(PidBuffer,"Pid:%d  Heap:%d", HeapSum[ItemId].Pid,
                HeapSum[ItemId].Size);
        WinSendDlgItemMsg( hwndDlg,
                           LISTBOX,
                           LM_INSERTITEM,
                           MPFROM2SHORT( LIT_END, 0 ),
                           MPFROMP( PidBuffer )
                         );
      }
      break;

    case WM_COMMAND:                    /* Posted by pushbutton or key  */
      /******************************************************************/
      /* PM sends a WM_COMMAND message when the user presses either     */
      /* the Enter or Escape pushbuttons.                               */
      /******************************************************************/
      switch( SHORT1FROMMP( mp1 ) )     /* Extract the command value    */
      {
        case DID_OK:                    /* The Enter pushbutton or key. */
        case DID_CANCEL:         /* The Cancel pushbutton or Escape key */
          WinDismissDlg( hwndDlg, TRUE );  /* Removes the dialog box    */
          return FALSE;
        default:
          break;
      }
      break;
    default:
      /******************************************************************/
      /* Any event messages that the dialog procedure has not processed */
      /* come here and are processed by WinDefDlgProc.                  */
      /* This call MUST exist in your dialog procedure.                 */
      /******************************************************************/
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
  }
  return FALSE;
}
/********************** End of dialog procedure  ************************/
