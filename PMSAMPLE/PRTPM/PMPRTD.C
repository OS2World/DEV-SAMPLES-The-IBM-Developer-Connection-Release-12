/*---------------------------------------------------------------------------+

+----------------------------------------------------------------------------+
|                                                                            |
| PMPRTD                                                                     |
|                                                                            |
|*  Description:  This routine is the system input hook which traps the      |
|*                VK_PRINTSCREEN key and sends it to PMPRT                   |
|*                                                                           |
+-------------------------------------+--------------------------------------+
| Version: 1.0                        |   Marc Fiammante (FIAMMANT at LGEVM2)|
+-------------------------------------+--------------------------------------+
| Inspired from OS2PM PROCS FROM OS2TOOLS                                    |
+-------------------------------------+--------------------------------------+
| History:                                                                   |
| --------                                                                   |
|                                                                            |
| created: Marc Fiammante March  1992                                        |
+---------------------------------------------------------------------------*/
#define INCL_DOS                        /* the PM header file           */
#define INCL_WIN                        /* the PM header file           */
#include <os2.h>                        /* PM header file               */
#include "pmprt.h"
HWND pascal   PrtWindow;         /*                              */
/*---------------------------------------------------------------------------+
| Input Hook procedure                                                       |
+---------------------------------------------------------------------------*/
BOOL EXPENTRY InputHookProc(HAB habSpy, PQMSG pQmsg, BOOL bRemove)
{
  static  HMODULE hSpyDll;
  if (PrtWindow!=(HWND)NULL) {
     switch (pQmsg->msg) {
        case WM_CHAR:
            if (SHORT1FROMMP(pQmsg->mp1) & KC_VIRTUALKEY) {
               if (SHORT2FROMMP(pQmsg->mp2)==VK_PRINTSCRN) {
                  WinPostMsg(PrtWindow,WM_USER+pQmsg->msg,pQmsg->mp1,pQmsg->mp2);
               }
            } /* endif */
           break;
       default:
           break;
     } /* endswitch */
  } else {
     DosGetModHandle( PRTHOOK , (PHMODULE)&hSpyDll);
     WinReleaseHook (habSpy, NULL, HK_INPUT  , (PFN)InputHookProc, hSpyDll);
     WinBroadcastMsg(HWND_DESKTOP,WM_NULL,(MPARAM)NULL,(MPARAM)NULL,BMSG_POST);
  }
  return(FALSE); /* pass the message to the next hook or to the application */
}

