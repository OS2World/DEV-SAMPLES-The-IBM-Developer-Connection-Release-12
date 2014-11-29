/*---------------------------------------------------------------------------+
+----------------------------------------------------------------------------+
|                                                                             |
| PMJRNDLL.C                                                                 |
|                                                                            |
| Program to record and pmayback and test PM applications                    |
+-------------------------------------+--------------------------------------+
| Version: 1.00                       |   Fiammante Marc                     |
+-------------------------------------+--------------------------------------+
| History:                                                                   |
| --------                                                                   |
|                                                                            |
| created: August 1989 by Marc Fiammante                                     |
+---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
| Includes                                                                   |
+---------------------------------------------------------------------------*/

#include "pmjrn.h"
#include <stdlib.h>

#pragma data_seg(GLOBDATA)
QMSG  SharedMsg;

/*---------------------------------------------------------------------------+
| Window Proc Subclass procedure                                             |
| Can be used to get AVIO data from                                          |
| Window Proc Subclass procedure Not currently used remove comment in        |
| Record Proc below to use it                                                |
+---------------------------------------------------------------------------*/
MRESULT EXPENTRY SubclassWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  static PFNWP   OldProc;
  static MRESULT Mresult;
  static HVPS   hpsVio;                      /* AVIO Presentation space handle   */
  static HVPS   LastVio;                      /* AVIO Presentation space handle   */
  static HVPS   TempVio;                      /* AVIO Presentation space handle   */
  static CHAR   VioBuffer[160];
  static HFILE  VioFile;
  static ULONG  Action,RetCode,NumberBytes,Length;
  static VIOMODEINFO VioModeInfo;
  RESULTCODES Result;
  static FirstTime=TRUE;
  OldProc=SpySubclassProcaddr((PFNWP)0,1);
  Mresult=(*OldProc)(hwnd,msg,mp1,mp2);
  if (msg==WM_PAINT) {
     RetCode=VioCreatePS(  (PHVPS)&hpsVio, 1, 1, 0, 1, (HVPS)0 );
     if (hpsVio>1) {
        LastVio=hpsVio-1; /* get the existing AVIO presentation space handle */
     }
     RetCode=VioDestroyPS( hpsVio );
     if (RetCode==NO_ERROR) { /* now read the first  160 chars of AVIO ps */
        if (LastVio>0) {
           Length=160;
           RetCode=VioReadCharStr(VioBuffer,(PUSHORT)&Length,0,0,1);
           DosOpen("D:\\PMJRN\\VIO.DAT", (PHFILE)&VioFile,
                      &Action,0L,0,18,66,(ULONG)0);
           DosWrite(VioFile,VioBuffer,Length,&NumberBytes);
        if (RetCode!=NO_ERROR) DosWrite(VioFile,"\nER\n",4,&NumberBytes);

           DosClose(VioFile);
        } else {
           WinAlarm( HWND_DESKTOP, WA_WARNING);
        }
     } else if (RetCode==ERROR_VIO_NOT_PRES_MGR_SG) {
       Length=160;
       RetCode=VioReadCharStr(VioBuffer,(PUSHORT)&Length,0,0,0);
       if (RetCode!=NO_ERROR) {
           WinAlarm( HWND_DESKTOP, WA_NOTE);
          }
        DosOpen("D:\\PMJRN\\VIO.DAT", (PHFILE)&VioFile,
                      &Action,0L,0,18,66,(ULONG)0);
        DosWrite(VioFile,VioBuffer,Length,&NumberBytes);
        if (RetCode!=NO_ERROR) DosWrite(VioFile,"\nEr\n",4,&NumberBytes);

        DosClose(VioFile);
     }
  }
  return(Mresult);
}
/*---------------------------------------------------------------------------+
| Are we allready installed                                                  |
| INPUT : bSet  1 = query instance 1                                         |
|               2 = query instance 2                                         |
|               3 = install                                                  |
|               5 = deinstall instance 1                                     |
|               6 = deinstall instance 2                                     |
| OUTPUT: for bSet 1,2: TRUE or FALSE                                        |
|                    3: 0 no installation, 1 instance 1, 2 instance 2        |
|                  5,6: FALSE                                                |
+---------------------------------------------------------------------------*/
INT EXPENTRY SpyInstalled(INT bSet) /*1 */
{
  static BOOL installed1 = FALSE;
  static BOOL installed2 = FALSE;
  switch (bSet) {
    case 1:
      return(installed1);
      break;
    case 2:
      return(installed2);
      break;
    case 3:
      if (installed1) {
        if (installed2) {
          return(0); /* no more installations possible */
        } else {
          installed2 = TRUE;
          return(2);
        } /* endif */
      } else {
        installed1 = TRUE;
        return(1);
      } /* endif */
      break;
    case 5:
      return(installed1=FALSE);
      break;
    case 6:
      return(installed2=FALSE);
      break;
    default:
      return(FALSE);
  } /* endswitch */
}

/*---------------------------------------------------------------------------+
| Return the Version of the SpyDll Module                                    |
+---------------------------------------------------------------------------*/

INT EXPENTRY SpyDllVersion(VOID) /* 2 */
{
  return(DLLVERSION);
}

/*---------------------------------------------------------------------------+
| Record Hook procedure                                                      |
+---------------------------------------------------------------------------*/

BOOL EXPENTRY SpyJrnRecordHookProc (HAB habSpy, PQMSG pQmsg) /* 3 */
{
  static  HMODULE hSpyDll;
  static  BOOL FirstTime=TRUE;
  PFN     NewProc;
  PFNWP   OldProc;
  HWND    HwndHook;
  HWND    Target;
/***** Just  if you want to subclass the pointed window then remove the comment
  if (FirstTime) {
      DosQueryModuleHandle( "PMJRNDLL", (PHMODULE)&hSpyDll);
      DosQueryProcAddr(hSpyDll,"#1",(PFN FAR *)&NewProc);
      HwndHook=SpyWndHandle((HWND)0,1);
      OldProc=WinSubclassWindow(HwndHook,(PFNWP)NewProc);
      SpySubclassProcaddr(OldProc,2);
      FirstTime=FALSE;
  }
********************/

  SharedMsg=*pQmsg; /* Get exported message */
  Target=TargetWindow(0,1);
  WinSendMsg(Target, WM_USER + 1 ,0,0);

  if (!RecordOnQueue(TRUE,1)) {
     Target=TargetWindow(0,1);
     WinPostMsg(Target, WM_QUIT ,0,0);
     DosQueryModuleHandle( "PMJRNDLL", (PHMODULE)&hSpyDll);
     WinAlarm( HWND_DESKTOP, WA_ERROR);
     WinReleaseHook(habSpy, 0, HK_JOURNALRECORD,(PFN)SpyJrnRecordHookProc, hSpyDll);
     FirstTime=TRUE;
  }
  return(TRUE);
}

/*---------------------------------------------------------------------------+
| Playback Hook procedure                                                    |
+---------------------------------------------------------------------------*/

ULONG EXPENTRY SpyJrnPlayHookProc (HAB habSpy,BOOL fSkip, PQMSG pQmsg) /* 4 */
{
  USHORT  RetCode;
  HWND    Target;
  static  HMODULE hSpyDll;
  static  LONG last_time,               /* Time last message read in*/
               last_msg_time,           /* Time in last message     */
               time_til_next;           /* Waiting time             */
  static  FirstTime=TRUE;
  LONG    time;                         /* Remaining waiting time   */
  static QMSG cur_msg;
  static BOOL InitTime=TRUE;
  USHORT  Action,NumberBytes;
  if (PlayOnQueue(TRUE,1)) {
     if (FirstTime==TRUE) {
         FirstTime=FALSE;
         InitTime=TRUE;
         Target=TargetWindow(0,1);
         WinSendMsg(Target, WM_USER + 1 ,0,0);
         cur_msg=SharedMsg; /* Get exported message */
         last_msg_time=cur_msg.time;         /* Set last msg time        */
         time_til_next=0L;                   /* Set waiting time         */
     } /* FirstTime==TRUE */
     if (!fSkip) {                   /* If not going to next msg */
         if (InitTime) last_time=pQmsg->time;/* Set time read in  */
         InitTime=FALSE;
         time=time_til_next-     /* Calc remaining wait      */
              (pQmsg->time-last_time);
         if (time<0)             /* Wait must be > 0         */
              time=0L;
         if (time>300L) time=300L;
         *pQmsg=cur_msg;              /* Copy over msg            */
         return(time);                         /* Return wait              */
     } else { /* Read next record */
        InitTime=TRUE;
        Target=TargetWindow(0,1);
        WinSendMsg(Target, WM_USER + 1 ,0,0);
        if (PlayOnQueue(TRUE,1)==FALSE) {
           WinAlarm( HWND_DESKTOP, WA_ERROR);
           WinAlarm( HWND_DESKTOP, WA_WARNING);
           WinPostMsg(Target, WM_QUIT ,0,0);
           DosQueryModuleHandle( "PMJRNDLL", (PHMODULE)&hSpyDll);
           WinReleaseHook(habSpy, 0, HK_JOURNALPLAYBACK,(PFN)SpyJrnPlayHookProc, hSpyDll);
           /* Broadcast to all queue an WM_0 to make release effective */
           WinBroadcastMsg( HWND_DESKTOP ,WM_NULL,0,0,BMSG_POSTQUEUE);
           FirstTime=TRUE;
           WinAlarm( HWND_DESKTOP, WA_ERROR);
           WinAlarm( HWND_DESKTOP, WA_WARNING);
           WinAlarm( HWND_DESKTOP, WA_ERROR);
           return(0L);
        }
        cur_msg=SharedMsg; /* Get exported message */
        time_til_next=cur_msg.time-            /* Set waiting time         */
                      last_msg_time;
        last_msg_time=cur_msg.time;            /* Set last msg time        */
     } /* !Fskip */
  } else  {
      Target=TargetWindow(0,1);
      WinPostMsg(Target, WM_QUIT ,0,0);
      DosQueryModuleHandle("PMJRNDLL", (PHMODULE)&hSpyDll);
      WinAlarm( HWND_DESKTOP, WA_ERROR);
      WinReleaseHook(habSpy, 0, HK_JOURNALPLAYBACK,(PFN)SpyJrnPlayHookProc, hSpyDll);
      FirstTime=TRUE;
     if (!fSkip) {                   /* If not going to next msg */
         if (InitTime) last_time=pQmsg->time;/* Set time read in  */
         InitTime=FALSE;
         time=time_til_next-     /* Calc remaining wait      */
              (pQmsg->time-last_time);
         if (time<0)             /* Wait must be > 0         */
              time=0L;
         if (time>300L) time=300L;
         *pQmsg=cur_msg;              /* Copy over msg            */
         return(time);                         /* Return wait              */
     }
  }
  return(0L);
}
/*---------------------------------------------------------------------------+
| Are we recording                                                           |
| INPUT : bSet  1 = query instance are we recording                          |
|               2 = install/deinstall instance                               |
| OUTPUT: for bSet 1  : TRUE or FALSE                                        |
|                  2  : value                                                |
+---------------------------------------------------------------------------*/
BOOL EXPENTRY RecordOnQueue(BOOL value, INT bSet) /* 5 */
{
  static BOOL recording  = FALSE;
  switch (bSet) {
    case 1:
      return(recording);
      break;
    case 2:
      return(recording=value);
      break;
    default:
      return(FALSE);
  } /* endswitch */
}
/*---------------------------------------------------------------------------+
| Are we playing back                                                        |
| INPUT : bSet  1 = query playing status                                     |
|               2 = install/deinstall playing status                         |
| OUTPUT: for bSet 1  : TRUE or FALSE                                        |
|                  2  : value                                                |
+---------------------------------------------------------------------------*/
BOOL EXPENTRY PlayOnQueue(BOOL value, INT bSet) /* 6 */
{
  static BOOL playing  = FALSE;
  switch (bSet) {
    case 1:
      return(playing);
      break;
    case 2:
      return(playing=value);
      break;
    default:
      return(FALSE);
  } /* endswitch */
}
/*---------------------------------------------------------------------------+
| Object window handle                                                       |
| INPUT : bSet  1 = query handle                                             |
|               2 = install/deinstall handle                                 |
| OUTPUT: for bSet 1  : value or 0                                        |
|                  2  : value                                                |
+---------------------------------------------------------------------------*/
HWND EXPENTRY TargetWindow( HWND value, INT bSet) /* 6 */
{
  static HWND Target  = 0;
  switch (bSet) {
    case 1:
      return(Target);
      break;
    case 2:
      return(Target=value);
      break;
    default:
      return((HWND)0);
  } /* endswitch */
}
/*---------------------------------------------------------------------------+
| What Handle are we spying                                                  |
| INPUT : bSet  1 = query handle                                             |
|               2 = install/deinstall handle                                 |
| OUTPUT: for bSet 1  : handle                                               |
|                  2  : handle                                               |
+---------------------------------------------------------------------------*/
HWND EXPENTRY SpyWndHandle(HWND InHandle, INT bSet) /* 8 */
{
  static HWND Handle = (HWND)0;
  switch (bSet) {
    case 1:
      return(Handle);
      break;
    case 2:
      return(Handle=InHandle);
      break;
    default:
      return((HWND) 0);
  } /* endswitch */
}
/*---------------------------------------------------------------------------+
| What is the old window Proc Address                                        |
| INPUT : bSet  1 = query Proc address                                       |
|               2 = set Proc address                                         |
| OUTPUT: for bSet 1  : values                                               |
|                  2  : value                                                |
+---------------------------------------------------------------------------*/
PFNWP   EXPENTRY SpySubclassProcaddr(PFNWP DefaultProc , INT bSet)
{
  static PFNWP ProcAddr=(PFNWP)0;
  switch (bSet) {
    case 1:
      return(ProcAddr);
      break;
    case 2:
      return(ProcAddr=DefaultProc);
      break;
    default:
      return((PFNWP) 0);
  } /* endswitch */
}
/*---------------------------------------------------------------------------+
| What is the HAB of the window                                              |
| INPUT : bSet  1 = query hab                                                |
|               2 = set hab                                                  |
| OUTPUT: for bSet 1  : values                                               |
|                  2  : value                                                |
+---------------------------------------------------------------------------*/
HAB     EXPENTRY SpyHabHandle(HAB  hab , INT bSet)
{
  static HAB SaveHab=(HAB)0;
  switch (bSet) {
    case 1:
      return(SaveHab);
      break;
    case 2:
      return(SaveHab=hab);
      break;
    default:
      return((HAB) 0);
  } /* endswitch */
}
