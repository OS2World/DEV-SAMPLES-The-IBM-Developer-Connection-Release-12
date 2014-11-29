/**********************************************************************/
/**********************************************************************/
/*                                                                    */
/*  TRAPIT                                                          */
/*                                                                    */
/* EXE . Timer based Ioctl query of the shift status of the Keyboard  */
/* Shift status always is available.                                  */
/* If CTRL+CTRL just Use DosKillProcess of the focus window process   */
/* because it must be the hanger. I could use my SPYALL  technique    */
/* (see PMSAMPLE package) to add a thread to all running PM processes */
/* and generate a trap from that thread to force a fatal exit. I'll   */
/* implement it in case I get feedback that DosKillProcess is not     */
/* Always working                                                     */
/* The ALT+ALT way is more touchy. I use a Send Msg Hook to add a     */
/* Thread to all PM processes and then use that thread to trap the    */
/* Focus process                                                      */
/**********************************************************************/
/* Version: 1.1             |   Marc Fiammante (FIAMMANT at LGEVM2)   */
/*                          |   La Gaude FRANCE                       */
/**********************************************************************/
/*                                                                    */
/**********************************************************************/
/* History:                                                           */
/* --------                                                           */
/*                                                                    */
/* created: Marc Fiammante October   1993                             */
/**********************************************************************/
#define INCL_BASE
#define INCL_WIN
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pmdebug.h"
VOID EXPENTRY HandleSend(HAB  hab, PSMHSTRUCT  pSmhStruct, BOOL  fInterThread);
MRESULT EXPENTRY MyWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
void EXPENTRY SetSpy( HWND sDebugger,HWND  sActive, HMQ   shmqDebugee, PID   shpid, TID   shtid);
HMODULE hModule;
BOOL    Success;
HAB     hab;
HMQ     hmq;                             /* Message queue handle         */
#define ID_WINDOW 2345
HWND    hwndFrame;                       /* Frame window handle          */
HWND    hwndClient;                      /* Frame window handle          */
CHAR    Buffer[80];
void main(int argc,char**argv) {
  QMSG qmsg;                            /* Message from message queue   */
  ULONG flCreate;                       /* Window creation control flags*/
  APIRET rc;
  flCreate = FCF_SYSMENU | FCF_SIZEBORDER | FCF_TITLEBAR | FCF_MINMAX
             | FCF_TASKLIST ;
  hab = WinInitialize(0);               /* Initialize PM                */
  hmq = WinCreateMsgQueue( hab, 0 );    /* Create a message queue       */
  SetSpy(0,0,0,0,0);
  rc=DosQueryModuleHandle("TRAPITH",&hModule);
  if (rc) {
      WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                    "Could not Find DLL",
                    "TRAPIT",
                     1234,
                     MB_OK | MB_ERROR);
  } /* endif */
  WinSetHook( hab, NULLHANDLE, HK_SENDMSG, (PFN)HandleSend, hModule);
  WinRegisterClass(                     /* Register window class        */
     hab,                               /* Anchor block handle          */
     "MyWindow",                        /* Window class name            */
     MyWindowProc,                      /* Address of window procedure  */
     CS_SIZEREDRAW,                     /* No special class style       */
     4                                  /* 1  extra window double word  */
     );
  hwndFrame = WinCreateStdWindow(
               HWND_DESKTOP,            /* Desktop window is parent     */
               0L,                      /* No class style               */
               &flCreate,               /* Frame control flag           */
               "MyWindow",              /* Client window class name     */
               "TRAPIT PM",             /* No window text               */
               0,                       /* No special class style       */
               0,                       /* Resource is in .EXE file     */
               ID_WINDOW,               /* Frame window identifier      */
               &hwndClient              /* Client window handle         */
               );
  if (hwndFrame!=0) {
     ULONG Flags=SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW;
     if (argc>1) {
        if (stricmp(argv[1],"HIDE")==0) {
           Flags &=~(SWP_ACTIVATE | SWP_SHOW);
        } /* endif */
     } /* endif */
     WinSetWindowPos( hwndFrame,     /* Set the size and position of */
                   HWND_TOP,            /* the window before showing.   */
                   100, 50, 270, 230,
                   Flags
                 );
     /************************************************************************/
     /* Get and dispatch messages from the application message queue         */
     /* until WinGetMsg returns FALSE, indicating a WM_QUIT message.         */
     /************************************************************************/
     while( WinGetMsg( hab, &qmsg, 0, 0, 0 ) )
            WinDispatchMsg( hab, &qmsg );
  }
  WinDestroyWindow( hwndFrame );        /* Tidy up...                   */
  WinDestroyMsgQueue( hmq );            /* and                          */
  WinReleaseHook( hab, NULLHANDLE, HK_SENDMSG, (PFN)HandleSend, hModule);
  WinTerminate( hab );                  /* terminate the application    */
  DosExit(EXIT_PROCESS,0);
}
CHAR *LabelText[]={"Keep Both Alt Keys down",
                   "For Trap generation in hanger context",
                   "(From a thread which is generated",
                   "for all running PM processes)",
                   "or Keep Both Ctrl Keys down",
                   "during 1 sec to kill focus hanger",
                   "TRAPIT V1.1",
                   "by Marc Fiammante"};
LONG Color=CLR_BACKGROUND;
HFILE  hKbd;
ULONG  Action;
ULONG  Plen;
ULONG  Dlen;
ULONG  State;
USHORT Shift;
APIRET rc;
MRESULT EXPENTRY MyWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   RECTL    clientrect;                     /* Rectangle coordinates        */
   HPS      hpsPaint;
   POINTL   pt;                            /* String screen coordinates    */
   BOOL     Invalid;
   int      Line;
   ERRORID  Err;
   HWND     Focus,Active;
   HMQ      hmqDeb;
   PID      pid,cpid;
   TID      tid,ctid;
   switch( msg )
   {
   case WM_CREATE:
      WinStartTimer(hab,hwnd,1234,500);
      break;                                /* end the application.     */
   case WM_TIMER:
      rc=DosOpen("KBD$", &hKbd, &Action, 0, FILE_NORMAL, FILE_OPEN,
                 OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE |
                 OPEN_FLAGS_FAIL_ON_ERROR, 0);
      if (rc) {
         char Buf[80];
         sprintf(Buf,"Open rc = %d",rc);
         WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                       Buf,
                       "TRAPIT",
                        1234,
                        MB_OK);
      } /* endif */
      Dlen= sizeof(State);
      rc = DosDevIOCtl( hKbd, 4, 0x73, 0, 0, 0, &State, sizeof(State), &Dlen );
      Shift=(USHORT)State;
      DosClose(hKbd);
      Invalid=FALSE;
      if ((State&0x0500)==0x0500) {
         if (Color!=CLR_RED) {
            Color=CLR_RED;
            Invalid=TRUE;
         } /* endif */
         Focus=WinQueryFocus(HWND_DESKTOP);
         Active=WinQueryActiveWindow(HWND_DESKTOP);
         WinQueryWindowProcess(Focus,&pid,&tid);
         hmqDeb=(HMQ)WinQueryWindowULong(Focus,QWL_HMQ);
         if (hmqDeb==hmq) {
            sprintf(Buffer,"Sorry Can't unhang Myself");
         } else {
            DosKillProcess(DKP_PROCESS,pid);
         } /* endif */
      } else {
         if ((State&0x0A00)==0x0A00) {
            if (Color!=CLR_BLUE) {
               Color=CLR_BLUE;
               Invalid=TRUE;
            } /* endif */
            Focus=WinQueryFocus(HWND_DESKTOP);
            Active=WinQueryActiveWindow(HWND_DESKTOP);
            WinQueryWindowProcess(Focus,&pid,&tid);
            hmqDeb=(HMQ)WinQueryWindowULong(Focus,QWL_HMQ);
            if (hmqDeb==hmq) {
               sprintf(Buffer,"Sorry Can't trap Myself");
            } else {
               HENUM hEnum;
               CHAR Class[20];
               HWND Child;
               hEnum = WinBeginEnumWindows(HWND_OBJECT);
               while ( (Child=WinGetNextWindow(hEnum)) != 0) {
                   WinQueryWindowProcess(Child,&cpid,&ctid);
                   if (cpid==pid) {
                       Class[0]=0;
                       WinQueryClassName(Child,sizeof(Class)-1,Class);
                       if (strcmp(Class,"Killer")==0) {
                           if (WinPostMsg(Child,WM_USER+1,0,0)) {
                              DosBeep(1800,80);
                              DosBeep(600,80);
                              DosBeep(1800,80);
                           }
                       } /* endif */
                   } /* endif */
               }
               WinEndEnumWindows(hEnum);
            } /* endif */
         } else {
            if (Color!=CLR_BACKGROUND) {
               Color=CLR_BACKGROUND;
               Invalid=TRUE;
            }
         } /* endif */
      } /* endif */
      if (Invalid) {
            WinInvalidateRect( hwnd, NULL, TRUE );
      } /* endif */
      break;
   case WM_PAINT:
      hpsPaint=WinBeginPaint( hwnd,0, &clientrect );
      WinFillRect( hpsPaint, &clientrect,Color );/* Fill invalid rectangle       */
      pt.x = 10; pt.y = 190;                    /* Set the text coordinates,    */
      GpiCharStringAt( hpsPaint, &pt, (LONG)strlen(Buffer),Buffer);
      for (Line=0;Line<8;Line++ ) {
         pt.x = 10; pt.y = 170-(20*Line);   /* Set the text coordinates,    */
         GpiCharStringAt( hpsPaint, &pt, (LONG)strlen(LabelText[Line]),LabelText[Line]);
      } /* endfor */
      WinEndPaint( hpsPaint );                        /* Drawing is complete   */
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
