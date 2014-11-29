/*************** Password Sample Program Source Code File (.C) ****************/
/*                                                                            */
/* PROGRAM NAME: PASSW                                                        */
/* -------------                                                              */
/*  Presentation Manager Standard Window C Sample Program                     */
/*                                                                            */
/* COPYRIGHT:                                                                 */
/* ----------                                                                 */
/*  (C) Copyright International Business Machines Corporation 1988, 1989      */
/*  Copyright (c) 1988, 1989 Microsoft Corporation                            */
/*                                                                            */
/* REVISION LEVEL: 1.2                                                        */
/* ---------------                                                            */
/*                                                                            */
/* WHAT THIS PROGRAM DOES:                                                    */
/* -----------------------                                                    */
/*  This program displays a standard window containing the text "Hello".      */
/*  The action bar contains a single choice, Options.                         */
/*  The Options pull-down contains three choices that each                    */
/*  paint a different string in the window.                                   */
/*  It also contains an Exit choice.                                          */
/*                                                                            */
/* WHAT THIS PROGRAM DEMONSTRATES:                                            */
/* -------------------------------                                            */
/*  This program demonstrates how to create and display a standard window,    */
/*  and how to use resources defined in a resource script file.               */
/*                                                                            */
/* WHAT YOU NEED TO COMPILE THIS PROGRAM:                                     */
/* --------------------------------------                                     */
/*                                                                            */
/*  REQUIRED FILES:                                                           */
/*  ---------------                                                           */
/*                                                                            */
/*    **NOTE** : See Readme.C for a list and description of required          */
/*               SYSTEM files.                                                */
/*                                                                            */
/*    passw.C       - Source code                                            */
/*    passw.CMD     - Command file to build this program                     */
/*    passw.MAK     - Make file for this program                             */
/*    passw.DEF     - Module definition file                                 */
/*    passw.H       - Application header file                                */
/*    passw.ICO     - Icon file                                              */
/*    passw.L       - Linker automatic response file                         */
/*    passw.RC      - Resource file                                          */
/*                                                                            */
/*    OS2.H          - Presentation Manager include file                      */
/*    STRING.H       - String handling function declarations                  */
/*                                                                            */
/*  REQUIRED LIBRARIES:                                                       */
/*  -------------------                                                       */
/*                                                                            */
/*    OS2.LIB        - Presentation Manager/OS2 library                       */
/*    SLIBCE.LIB     - Protect mode/standard combined small model C library   */
/*                                                                            */
/*  REQUIRED PROGRAMS:                                                        */
/*  ------------------                                                        */
/*                                                                            */
/*    IBM C Compiler                                                          */
/*    IBM Linker                                                              */
/*    Resource Compiler                                                       */
/*                                                                            */
/* EXPECTED INPUT:                                                            */
/* ---------------                                                            */
/*                                                                            */
/* EXPECTED OUTPUT:                                                           */
/* ----------------                                                           */
/*                                                                            */
/* CALLS USED (Dynamic Link References):                                      */
/* -------------------------------------                                      */
/*                                                                            */
/*    GpiCharStringAt     WinDefWindowProc    WinInvalidateRegion             */
/*    GpiSetBackColor     WinDestroyMsgQueue  WinLoadString                   */
/*    GpiSetBackMix       WinDestroyWindow    WinPostMsg                      */
/*    GpiSetColor         WinDispatchMsg      WinRegisterClass                */
/*    WinBeginPaint       WinEndPaint         WinSetWindowPos                 */
/*    WinCreateMsgQueue   WinGetMsg           WinTerminate                    */
/*    WinCreateStdWindow  WinInitialize                                       */
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
#include <string.h>                     /* C/2 string functions         */
#include <ctype.h>                      /* C/2 string functions         */
#include <conio.h>
#include <dos.h>
#include "passw.h"                      /* Resource symbolic identifiers*/
CHAR szString[STRINGLENGTH];            /* procedure.                   */
CHAR wMsg[STRINGLENGTH];            /* procedure.                   */

/************************************************************************/
/* Function prototypes                                                  */
/************************************************************************/
MRESULT EXPENTRY MyWindowProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 );
void cdecl waitread(void);
void cdecl LoadPassword(void);
int  cdecl TestPassword(void);
void cdecl ActivatePassword(void);
void cdecl main( void );
/************************************************************************/
int xB[3] ={10,70,130};
int cxB[3]={40,40,70};
char *szB[3]={"~Load","~Test","~Activate"};
                                        /* Define parameters by type    */
HAB  hab;                               /* PM anchor block handle       */
HWND hwndFrame;                         /* Frame window handle          */
HWND hwndClient;                        /* Client area window handle    */
HWND hwndStatic;                        /* Client area window handle    */
CHAR   Passw[8];
CHAR   aPassw[8];
USHORT Passwl=0;
/**********************  Start of main procedure  ***********************/
void cdecl main(  )
{
   HMQ  hmq;                             /* Message queue handle         */
   QMSG qmsg;                            /* Message from message queue   */
   ULONG flCreate;                       /* Window creation control flags*/
   int i;

   hab = WinInitialize( NULL );          /* Initialize PM                */
   hmq = WinCreateMsgQueue( hab, 0 );    /* Create a message queue       */
   Passw[0]=0x00;
   wMsg[0]=0x00;
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
               NULL,                    /* Resource is in .EXE file     */
               ID_WINDOW,               /* Frame window identifier      */
               &hwndClient              /* Client window handle         */
               );
    for (i=0;i<3;i++ ) {
         WinCreateWindow( hwndClient,
                          WC_BUTTON,
                          szB[i],
                          WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
                          xB[i], 10,cxB[i], 30,
                          hwndClient,
                          HWND_TOP,
                          ID_BUTTONS+i,
                          NULL,
                          NULL );

    } /* endfor */
    hwndStatic=WinCreateWindow( hwndClient,
                          WC_STATIC,
                          "",
                          WS_VISIBLE | WS_TABSTOP | SS_TEXT,
                          72, 102,66, 24,
                          hwndClient,
                          HWND_TOP,
                          ID_STATIC,
                          NULL,
                          NULL );
    WinSetWindowPos( hwndFrame,         /* Shows and activates frame    */
                   HWND_TOP,            /* window at position 100, 100, */
                   100, 100, 300, 300,  /* and size 200, 200.           */
                   SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW
                 );


/************************************************************************/
/* Get and dispatch messages from the application message queue         */
/* until WinGetMsg returns FALSE, indicating a WM_QUIT message.         */
/************************************************************************/
  while( WinGetMsg( hab, &qmsg, NULL, 0, 0 ) ) {
    WinDispatchMsg( hab, &qmsg );
    }

  WinDestroyWindow( hwndFrame );        /* Tidy up...                   */
  WinDestroyMsgQueue( hmq );            /* and                          */
  WinTerminate( hab );                  /* terminate the application    */
}
/***********************  End of main procedure  ************************/

/*********************  Start of window procedure  **********************/
MRESULT EXPENTRY MyWindowProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
  USHORT command;                       /* WM_COMMAND command value     */
  HPS    hps;                           /* Presentation Space handle    */
  RECTL  rc;                            /* Rectangle coordinates        */
  POINTL pt;                            /* String screen coordinates    */
  int    ret;
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
       case ID_LOAD:
          LoadPassword();
          strcpy(wMsg,"Password Loaded");
          Passwl=0;
          Passw[0]=0x00;
          aPassw[0]=0x00;
          WinInvalidateRegion( hwnd, NULL, TRUE );
          break;
       case ID_TEST:
          ret=TestPassword();
          sprintf(wMsg,"Password test =%4.4X",ret);
          WinInvalidateRegion( hwnd, NULL, TRUE );
          break;
        case ID_ACTIVATE:
          strcpy(wMsg,"Password Activated");
          ActivatePassword();
          WinInvalidateRegion( hwnd, NULL, TRUE );
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
      hps = WinBeginPaint( hwnd, NULL, &rc );
      rc.yBottom=30L;
      WinFillRect( hps, &rc,CLR_BACKGROUND  );/* Fill invalid rectangle       */
      GpiSetColor( hps, CLR_NEUTRAL );         /* colour of the text,   */
      GpiSetBackColor( hps, CLR_BACKGROUND );  /* its background and    */
      GpiSetBackMix( hps, BM_OVERPAINT );      /* how it mixes,         */
      pt.x=10;pt.y=210;
      strcpy(szString,"Key in the password");
      GpiCharStringAt( hps, &pt, (LONG)strlen( szString ), szString );
      pt.x=10;pt.y=190;
      strcpy(szString,"Then Click Load, then Test");
      GpiCharStringAt( hps, &pt, (LONG)strlen( szString ), szString );
      pt.x=10;pt.y=170;
      strcpy(szString,"Then Click Activate when needed");
      GpiCharStringAt( hps, &pt, (LONG)strlen( szString ), szString );
      pt.x=10;pt.y=150;
      GpiCharStringAt( hps, &pt, (LONG)strlen( wMsg ), wMsg );
      pt.x=70;pt.y=100;
      GpiMove(hps,&pt);
      pt.x=140;pt.y=100;
      GpiLine(hps,&pt);
      pt.x=140;pt.y=130;
      GpiLine(hps,&pt);
      pt.x=70;pt.y=130;
      GpiLine(hps,&pt);
      pt.x=70;pt.y=100;
      GpiLine(hps,&pt);
#if 0
      if (Passwl>0) {
         sprintf(szString,"Scancode %2.2X",(int)Passw[Passwl-1]);
         pt.x=10;pt.y=60;
         GpiCharStringAt( hps, &pt, (LONG)strlen( szString ), szString );
      } /* endif */
#endif
      WinEndPaint( hps );                      /* Drawing is complete   */
      break;
    case WM_CHAR:
      /******************************************************************/
      /* Character input is processed here.                             */
      /* The first two bytes of message parameter 2 contain             */
      /* the character code.                                            */
      /******************************************************************/
      if ((SHORT1FROMMP(mp1)&KC_CHAR)&&
          (SHORT1FROMMP(mp1)&KC_SCANCODE)&&
          (isalnum(CHAR1FROMMP(mp2))) ) {
             Passw[Passwl]=CHAR4FROMMP(mp1);
             aPassw[Passwl]='*';
             Passwl++;
             if (Passwl==sizeof(Passw)) {
                Passw[0]=CHAR4FROMMP(mp1);
                aPassw[0]='*';
                Passwl=1;
             } /* endif */
             Passw[Passwl]=0x00;
             aPassw[Passwl]=0x00;
             WinSetWindowText(hwndStatic,aPassw);
             wMsg[0]=0x00;
      } /* endif */
      if (SHORT1FROMMP(mp1)&KC_VIRTUALKEY) {
         if ((SHORT2FROMMP(mp2)==VK_DELETE) ||
             (SHORT2FROMMP(mp2)==VK_NEWLINE) ||
             (SHORT2FROMMP(mp2)==VK_ENTER)) {
             Passwl=0;
             Passw[0]=0x00;
             aPassw[0]=0x00;
         }
      }
      WinInvalidateRegion( hwnd, NULL, TRUE );
      break;                                /* end the application.     */
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

#pragma alloc_text(IOPLSEG,waitread,LoadPassword,TestPassword,ActivatePassword)

void cdecl waitread() {
   while (0x02&inp(0x64)) {
      long int loop;
      for (loop=0;loop<0x0FFFFL;loop++) {
      } /* endfor */
   } /* endwhile */
}
void cdecl LoadPassword() {
   int i;
   waitread();
   outp(0x64,0xA5);
   for (i=0;i<Passwl;i++) {
        waitread();
        outp(0x60,Passw[i]);
   } /* endfor */
   waitread();
   outp(0x60,0x00);
   waitread();
}
int cdecl TestPassword() {
   int i;
   waitread();
   outp(0x64,0xA4);
   waitread();
   i=inp(0x60);
   return i;
}
void cdecl ActivatePassword() {
   waitread();
   outp(0x64,0xA6);
   waitread();
}
