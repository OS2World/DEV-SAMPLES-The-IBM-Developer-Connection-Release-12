/*************** Password Sample Program Source Code File (.C) ****************/
/*                                                                            */
/* PROGRAM NAME: hotk                                                        */
/* -------------                                                              */
/*                                                                            */
/* COPYRIGHT:                                                                 */
/* ----------                                                                 */
/*  (C) Copyright International Business Machines Corporation 1988, 1989      */
/*  Copyright (c) 1988, 1989 Microsoft Corporation                            */
/*                                                                            */
/******************************************************************************/

#define INCL_GPIPRIMITIVES              /* Selectively include          */
#define INCL_DOS                        /* the PM header file           */
#define INCL_WIN                        /* the PM header file           */
#define INCL_DOSERRORS

#include <os2.h>                        /* PM header file               */
#include <stdlib.h>                     /* C/2 string functions         */
#include <string.h>                     /* C/2 string functions         */
#include <stdio.h>                      /* C/2 string functions         */
#include "hotk.h"                      /* Resource symbolic identifiers*/
CHAR szString[STRINGLENGTH];            /* procedure.                   */
CHAR wMsg[STRINGLENGTH];            /* procedure.                   */
HFILE      hwndkbd;                     /* Keyboard Handle                 */
ULONG   ulTemp;
ULONG   ulTemp1;
ULONG   ulTemp2;
BOOL     OnOff=1;
PSZ   Labels[]={"Enable","Disable"};

/************************************************************************/
/* Function prototypes                                                  */
/************************************************************************/
MRESULT EXPENTRY MyWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
void cdecl main( void );
/************************************************************************/

                                        /* Define parameters by type    */
HAB  hab;                               /* PM anchor block handle       */
HWND hwndFrame;                         /* Frame window handle          */
HWND hwndClient;                        /* Client area window handle    */
HWND hwndButton;                        /* Client area window handle    */
USHORT hotkl=0;
#pragma pack(1)
struct _Packet {
   SHORT State;
   CHAR  Make;
   CHAR  Break;
   SHORT Id;
} Packet;
#pragma pack()
APIRET RetI=0;
APIRET RetO=0;
/**********************  Start of main procedure  ***********************/
void cdecl main(  )
{
   HMQ  hmq;                             /* Message queue handle         */
   QMSG qmsg;                            /* Message from message queue   */
   ULONG flCreate;                       /* Window creation control flags*/

   hab = WinInitialize( 0 );          /* Initialize PM                */
   hmq = WinCreateMsgQueue( hab, 0 );    /* Create a message queue       */
   WinRegisterClass(                     /* Register window class        */
     hab,                               /* Anchor block handle          */
     "MyWindow",                        /* Window class name            */
     MyWindowProc,                      /* Address of window procedure  */
     CS_SIZEREDRAW,                     /* Class style                  */
     0                                  /* No extra window words        */
     );

   flCreate=
          FCF_TITLEBAR | FCF_SYSMENU | FCF_MENU | FCF_SIZEBORDER | FCF_MINMAX |
                      FCF_TASKLIST | FCF_ACCELTABLE
            ;
   hwndFrame = WinCreateStdWindow(
               HWND_DESKTOP,            /* Desktop window is parent     */
               0L,                      /* No frame styles              */
               &flCreate,               /* Frame control flag           */
               "MyWindow",              /* Client window class name     */
               "Keyboard Lock",         /* Title                        */
               0L,                      /* No special class style       */
               0,                       /* Resource is in .EXE file     */
               ID_WINDOW,               /* Frame window identifier      */
               &hwndClient              /* Client window handle         */
               );
    hwndButton=WinCreateWindow( hwndClient,
                          WC_BUTTON,
                          "Disable",
                          WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
                          10 , 10,80, 30,
                          hwndClient,
                          HWND_TOP,
                          ID_TOGGLE,
                          NULL,
                          NULL );

    WinSetWindowPos( hwndFrame,         /* Shows and activates frame    */
                   HWND_TOP,            /* window at position 100, 100, */
                   100, 100, 300, 300,  /* and size 200, 200.           */
                   SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW
                 );
       RetO=DosOpen("KBD$", (PHFILE)&hwndkbd, &ulTemp,
                         0L,
                         FILE_NORMAL,
                         FILE_OPEN,
                         OPEN_ACCESS_READWRITE |
                         OPEN_SHARE_DENYREADWRITE |
                         OPEN_FLAGS_FAIL_ON_ERROR,
                         (PEAOP2)0);


/************************************************************************/
/* Get and dispatch messages from the application message queue         */
/* until WinGetMsg returns FALSE, indicating a WM_QUIT message.         */
/************************************************************************/
  while( WinGetMsg( hab, &qmsg, 0, 0, 0 ) ) {
    WinDispatchMsg( hab, &qmsg );
    }
  WinSetSysModalWindow(HWND_DESKTOP,0);
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
  switch( msg )
  {

    case WM_COMMAND:
      /******************************************************************/
      /* When the user chooses option 1, 2, or 3 from the Options pull- */
      /* down, the text string is set to 1, 2, or 3, and                */
      /* WinInvalidateRegion sends a WM_PAINT message.                  */
      /* When Exit is chosen, the application posts itself a WM_CLOSE   */
      /* message.                                                       */
      /******************************************************************/
      command = SHORT1FROMMP(mp1);      /* Extract the command value    */
      switch (command)
      {
        case ID_TOGGLE:
          Packet.Id=-1;
          ulTemp1=sizeof(Packet);
          ulTemp2=0;
          RetI=DosDevIOCtl( hwndkbd ,0x4L,0x56L,&Packet,sizeof(Packet),&ulTemp1,0L,0L,
                          &ulTemp2);
          OnOff=1-OnOff;
          WinSetWindowText(hwndButton,Labels[OnOff]);
          if (OnOff) {
             WinSetSysModalWindow(HWND_DESKTOP,0);
          } else {
             WinSetSysModalWindow(HWND_DESKTOP,hwndFrame);
          } /* endif */
          WinInvalidateRegion( hwnd, 0, TRUE );
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
      pt.x=10;pt.y=210;
      strcpy(szString,"Click on Disable or Enable to");
      GpiCharStringAt( hps, &pt, (LONG)strlen( szString ), szString );
      pt.x=10;pt.y=190;
      strcpy(szString,"Disable or Activate Hot keys");
      GpiCharStringAt( hps, &pt, (LONG)strlen( szString ), szString );
      pt.x=10;pt.y=170;
      sprintf(szString,"Ret IOCTL= %X , HwndKbd =%X, Ret Open=%X",RetO,hwndkbd,RetI);
      GpiCharStringAt( hps, &pt, (LONG)strlen( szString ), szString );
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
/**********************  End of window procedure  ***********************/
