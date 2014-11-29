/**************************************************************************
 * OSA Test Application OSATEST
 *
 * Name: osaobj.c
 *
 * Description:   The object window procedure on thread 2.
 *
 *             Tasks asked of the object window are not bound by 1/10
 *             second rule.  Tasks are given to the object window to
 *             perform via WM_USER_* messages.
 *
 *             When tasks are completed, the object window posts a
 *             WM_USER_ACK message to the window that requested the task.
 *
 ****************************************************************************/

/* os2 includes */
#define INCL_DEV
#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS
#define INCL_WINDIALOGS
#define INCL_WINERRORS
#define INCL_WINMENUS
#define INCL_WINMLE
#define INCL_WINSTDFILE
#define INCL_WINSTDFONT
#define INCL_WINWINDOWMGR
#define INCL_OSAAPI
#define INCL_OSA
#include <os2.h>

/* c language includes */
#include <ctype.h>
#include <memory.h>
#include <process.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\stat.h>
#include <sys\types.h>

/* application includes */
#include "pmassert.h"
#include "osatest.h"
#include "osadlg.h"
#include "ucmenus.h"

/*************************************************************************
 *
 * Name: threadmain
 *
 * Description: Similar in nature to main(), except this occurs on thread 2.
 *              Called by _beginthread() in prtcreat.c
 *
 * API's:       WinInitialize
 *              WinCreateMsgQueu
 *              WinRegisterClass
 *              WinCreateWindow
 *              WinPostMsg
 *              WinGetMsg
 *              WinDispatchMsg
 *              WinDestroyWindow
 *              WinDestroyMsgQueue
 *              WinTerminate
 *
 * Parameters: pv, a pointer to the main block of program parameters
 *
 * Return:  [none]
 *
 **************************************************************************/
void _Optlink threadmain( void *pv )
{

   PMAIN_PARM pmp;
   BOOL       bOK;
   HAB        hab;
   HMQ        hmq;
   QMSG       qmsg;

   /* copy and convert pvoid parmeter to a pointer to the main parameter block */
   pmp = (PMAIN_PARM) pv;

   /* thread initialization */
   hab = WinInitialize( 0 );
   hmq = WinCreateMsgQueue( hab, 0 );

   /*
    * ensure that object window does not receive a WM_QUIT on system shutdown
    */
   WinCancelShutdown(hmq, TRUE);

   bOK = WinRegisterClass( hab,
                                OBJECTCLASSNAME,
                                ObjectWinProc,
                                0L,
                                sizeof( PMAIN_PARM ) );
   pmassert( hab, bOK );

   /*
    * create a worker window where the parent is the PM object window,
    * it operates on thread 2,
    * has no visible windows on the desktop,
    * and is not bound by the 1/10 second message processing rule.
    */
   pmp->hwndObject = WinCreateWindow(
                       HWND_OBJECT,       /* parent */
                       OBJECTCLASSNAME,   /* class name */
                       "",                /* no caption needed */
                       0L,                /* style */
                       0L,                /* x */
                       0L,                /* y */
                       0L,                /* cx */
                       0L,                /* cy */
                       HWND_OBJECT,       /* owner */
                       HWND_BOTTOM,       /* position (nop) */
                       0L,                /* id (nop) */
                       (PVOID)pmp,        /* parms passed to wm_create case */
                       (PVOID)NULL);      /* presparams */

   pmassert( hab, pmp->hwndObject );

   /*
    * at this point, application has completely initialized
    * wm_create processing in the object window procedure has completed.
    * let client window know about it
    */
   bOK = WinPostMsg( pmp->hwndClient, WM_USER_ACK, (MPARAM)0L, (MPARAM)0L );
   pmassert( pmp->hab, bOK );


   /*
    * dispatch messages; these messages will be mostly user-defined messages
    * processed on thread 2
    */
   while( WinGetMsg ( hab, &qmsg, (HWND)NULLHANDLE, 0L, 0L ))
   {
      WinDispatchMsg ( hab, &qmsg );
   }


   /* make thread one terminate */
   WinPostMsg( pmp->hwndClient, WM_QUIT, (MPARAM)0L, (MPARAM)0L );

   /* clean up */
   WinDestroyWindow( pmp->hwndObject );
   WinDestroyMsgQueue( hmq );
   WinTerminate( hab );

   /* end this thread */
   _endthread();

}  /*  end of threadmain()  */


/**************************************************************************
 *
 * Name: ObjectWinProc
 *
 * Description: a window procedure like most others, except it is not
 *              responsible for any visible windows or presentation. It
 *              exists to perform lengthy tasks on thread2 of the
 *              application.  WM_* messages appear here in alphabetical order.
 *
 * Parameters: mp1 = window handle to acknowledge upon completion of the task
 *             (hwndToAck)
 *             mp2 is an extra parameter and depends on the task
 *
 * returns: an acknowlegement of completion of the task using WinPostMsg
 *   if success: WinPostMsg( hwndToAck, WM_USER_ACK,  msg, rc );
 *               return rc;
 *
 *   if not:     WinPostMsg( hwndToAck, WM_NACK_*   , msg, rc );
 *               return rc;
 *
 *   where msg was the WM_USER_* message that was posted to the object window
 *   and rc is a return code. Depending on the hwndToAck, returning
 *   the result code can be as important as posting it; in particular,
 *   the object window may send synchronous messages to itself and may
 *   check the result code via the return code of WinSendMsg()
 *
 ***************************************************************************/
MRESULT EXPENTRY ObjectWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   HAB                hab;
   HWND               hwndToAck;
   LONG               lRC;
   PMAIN_PARM         pmp;
   ULONG              nackmsg;
   SWP                swp,swp2;
   ULONG              ulMenuCx;
   ULONG              ulMenuCy;


   /* store the handle of the window to ack upon task completion; */
   hwndToAck = (HWND)mp1;
   hab = WinQueryAnchorBlock( hwnd );
   pmp = (PMAIN_PARM) WinQueryWindowULong( hwnd, QWL_USER );


   switch( msg )
   {

   case WM_CREATE:
     /* mp1 is pointer to main paramters; save it in object window words */
     pmp = (PMAIN_PARM)mp1;
     WinSetWindowULong( hwnd, QWL_USER, (ULONG) pmp );

     /* do more startup processing whilst on thread 2 */

     /* Read .ini for filename, mode, printer, and driver data */
     GetProfile( pmp );

     WinQueryWindowPos(pmp->hwndClient,&swp);
     WinQueryWindowPos(pmp->hwndFrame,&swp2);
     if(swp2.cx == 0)
     {
       WinShowWindow(pmp->hwndFrame, TRUE);
       WinQueryWindowPos(pmp->hwndClient,&swp);
       WinQueryWindowPos(pmp->hwndFrame,&swp2);
     }
     WinSendMsg(pmp->hwndCommandBar, UCMENU_QUERYSIZE, &ulMenuCx, &ulMenuCy);
     WinSetWindowPos(pmp->hwndFrame, HWND_TOP,
                     0,
                     0,
                     ulMenuCx + (swp2.cx - swp.cx),
                     swp2.cy - swp.cy,
                     SWP_SIZE | SWP_SHOW );

     return (MRESULT)NULL;


   case WM_USER_CLOSE:
     /* write settings to ini */
     SaveProfile(pmp);

     /*
      * Tell dialogs to close
      */
     if(pmp->hwndScriptEditor)
       WinSendMsg(pmp->hwndScriptEditor, WM_COMMAND, (MPARAM)IDD_SCRED_CLOSE, NULL);

     if(pmp->hwndTestEvent)
       WinSendMsg(pmp->hwndTestEvent, WM_COMMAND, (MPARAM)IDD_CANCELEVENT, NULL);

     /*
      * post a quit to object window;
      * when msg loop falls out in threadmain, then post wm_quit to client
      */
     WinPostMsg( hwnd, WM_QUIT, 0, 0 );
     break;
   }

   /* default: */
   return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}  /* end of ObjectWinProc() */


/***************************  End of prtobj.c ****************************/
