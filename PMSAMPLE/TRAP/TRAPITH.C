#define INCL_BASE
#define INCL_DOS
#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include "pmdebug.h"
#pragma data_seg(SHRDATA)
HWND  Active;
HMQ   hmqDebugee;
PID   hpid;
TID   htid;
HWND  Debugger;
BOOL  Trace;
#pragma data_seg()
VOID APIENTRY ithread(ULONG);
BOOL  ThreadStarted=FALSE;
VOID DebugMsg(PSZ Message) {
  HFILE  DebugFile;
  ULONG  NumBytes,Action;
  ULONG  Location;
  PTIB   ptib;
  PPIB   ppib;
  char   FileName[35];
  CHAR   WorkBuffer1[80];
  DosGetInfoBlocks(&ptib,&ppib);
  strcpy(FileName,"G:\\UNL\\");
  _itoa(ppib->pib_ulpid,WorkBuffer1,16);
  strcat(FileName,WorkBuffer1);
  strcat(FileName,"_");
  _itoa(ptib->tib_ptib2->tib2_ultid ,WorkBuffer1,16);
  strcat(FileName,WorkBuffer1);
  strcat(FileName,".DAT");
  strcpy(WorkBuffer1,Message);
  DosOpen(FileName, (PHFILE)&DebugFile,
                &Action,0L,0,
                OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW,
                OPEN_ACCESS_READWRITE    | OPEN_SHARE_DENYNONE |
                OPEN_FLAGS_WRITE_THROUGH | OPEN_FLAGS_NO_CACHE ,
                (ULONG)0);
  DosSetFilePtr(DebugFile,0L,FILE_END,&Location);
  strcat(WorkBuffer1,"\n");
  DosWrite(DebugFile,WorkBuffer1,strlen(WorkBuffer1),&NumBytes);
  DosClose(DebugFile);
}
VOID EXPENTRY HandleSend(HAB  hab, PSMHSTRUCT  pSmhStruct, BOOL  fInterThread)
{
     HMQ   hmq;
     PID   pid;
     TID   tid;
     CHAR Buffer[80];
     APIRET rc;
     ULONG time;
     rc=DosEnterCritSec();
     if (rc==0) {
        if (!ThreadStarted) {
           ThreadStarted=TRUE;
           DosExitCritSec();
           rc=DosCreateThread(&tid,ithread,0,0,0x3000);
        } else {
           DosExitCritSec();
        } /* endif */
     } /* endif */
}
void EXPENTRY SetSpy( HWND sDebugger,HWND  sActive, HMQ   shmqDebugee, PID   shpid, TID   shtid)
{
  Debugger  =sDebugger;
  Active    =sActive;
  hmqDebugee=shmqDebugee;
  hpid      =shpid;
  htid      =shtid;
}
MRESULT EXPENTRY KillerProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 ) {
   PCHAR Trap;
   if ( msg==WM_USER+1 ) {
      Trap=0;  /* Create a NULL pointer       */
      *Trap=0; /* use it to Generate the Trap */
      return FALSE;
   } else {
      return WinDefWindowProc( hwnd, msg, mp1, mp2 );
   }
}
VOID APIENTRY ithread(ULONG parm) {
  HAB   hab;
  HMQ   hmq;
  APIRET rc;
  ULONG ResetCnt;
  QMSG  qmsg;
  ERRORID Err;
  ULONG time;
  HWND  Killer;
  hab = WinInitialize(0);               /* Initialize PM                */
  hmq = WinCreateMsgQueue( hab, 0 );    /* Create a message queue       */
  WinRegisterClass(                     /* Register window class        */
     hab,                               /* Anchor block handle          */
     "Killer",                          /* Window class name            */
     KillerProc,                        /* Address of window procedure  */
     0L,                                /* No special class style       */
     0                                  /* 1  extra window double word  */
     );
   Killer=WinCreateWindow(HWND_OBJECT,
                           "Killer",
                           "Killer",
                           0,
                           0,
                           0,
                           1,
                           1,
                           HWND_OBJECT,
                           HWND_BOTTOM,
                           6543,
                           0,
                           0);
  while( WinGetMsg( hab, &qmsg, 0, 0, 0 ) ) {
           WinDispatchMsg( hab, &qmsg );
  }
  WinDestroyMsgQueue( hmq );            /* and                          */
  WinTerminate( hab );                  /* terminate the application    */
  DosExit(EXIT_THREAD,0);
}
