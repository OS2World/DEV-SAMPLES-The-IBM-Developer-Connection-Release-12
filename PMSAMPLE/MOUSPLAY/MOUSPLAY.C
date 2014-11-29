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
#include "mousplay.h"
/*************************************************************************/
/* Move and play  related variables                                      */
/*************************************************************************/
extern SHORT   sXCoordinate; /* Make coordinates extern to share    */
extern SHORT   sYCoordinate; /* with the hook DLL                   */
extern CHAR    PlayString[]; /* DLL definition of character string     */
ULONG EXPENTRY MousePlayHookProc(HAB habSpy,BOOL fSkip, PQMSG pQmsg);      /* 4 */
ULONG EXPENTRY KeyPlayHookProc(HAB habSpy,BOOL fSkip, PQMSG pQmsg);        /* 4 */
USHORT TextLen;
/*************************************************************************/
/* Thread related variables                                              */
/*************************************************************************/
void       PlayThread(void  *args);      /* Playing   thread function    */
#define    STACKSIZE 4096
TID        TstTID;                       /* Thread ID                    */
HAB        Thab;
extern     HMQ   Thmq;              /* Thmq is extern to share it with DLL*/
/*************************************************************************/
/* Main proc dialog related variables                                    */
/*************************************************************************/
HWND    hwndModeless;                    /* Dialog handle                */
MRESULT EXPENTRY fnwpPlayDlg( HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2 );
HAB        hab;
HMQ        hmq;
/**********************  Start of main procedure  ***********************/
void main(void)
{
  QMSG qmsg;
  hab = WinInitialize( 0 );          /* Initialize PM                */
  hmq = WinCreateMsgQueue( hab, 0 );    /* Create a message queue       */
  /* create the modeless dialog window                                */
  hwndModeless = WinLoadDlg( HWND_DESKTOP,
                             HWND_OBJECT,
                             fnwpPlayDlg,
                             0,
                             DLG_PLAY,
                             0
                             );
  WinShowWindow( hwndModeless, TRUE );
  /* Main message-processing loop - get and dispatch messages until   */
  /* WM_QUIT received                                                 */
  while( WinGetMsg( hab, &qmsg, (HWND)0, 0, 0 ) )
    WinDispatchMsg( hab, &qmsg );
  WinDestroyWindow( hwndModeless );
  WinDestroyMsgQueue( hmq );
  WinTerminate( hab );
  DosExit( 1, 0 );
}
/**********************  Play dialog             ***********************/
MRESULT EXPENTRY fnwpPlayDlg( HWND hwndDlg, ULONG msg, MPARAM mp1,
                                  MPARAM mp2 )
{
  switch (msg)
  {
    case WM_INITDLG:
      break;
    case WM_COMMAND:
      switch( SHORT1FROMMP( mp1 ) )
      {
        case DID_OK:    /* Enter key pressed or pushbutton selected  */
          /***********************************************************/
          /*                                                         */
          /* Find out X and Y coordinates  and start the playback    */
          /* hook                                                    */
          /***********************************************************/

          if( !WinQueryDlgItemShort( hwndDlg, EF_X, &sXCoordinate, FALSE ))
                                     sXCoordinate=0;
          if( !WinQueryDlgItemShort( hwndDlg, EF_Y, &sYCoordinate, FALSE ))
                                     sYCoordinate=0;
          TextLen=WinQueryDlgItemText( hwndDlg,ID_CHARPLAY, 79, PlayString);
          TstTID=_beginthread(PlayThread,0,STACKSIZE,0);
          return FALSE;
          break;
       case DID_CANCEL: /* Escape key pressed or CANCEL pushbutton selected */
          WinDismissDlg( hwndDlg, TRUE ); /* Finished with dialog box*/
          WinPostQueueMsg(hmq, WM_QUIT, 0L, 0L );/* Cause mousplay to end */
          return FALSE;
        default:
          break;
      }
      break;

    default:  /* Pass all other messages to the default dialog proc */
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
  }
  return FALSE;
}
void PlayThread(void  *args) {
   QMSG qmsg;
   HMODULE hSpyDll;                       /* Dll Module handle              */
   int rc;
   Thab = WinInitialize( 0 );          /* Initialize PM                */
   Thmq = WinCreateMsgQueue( Thab, 0 );    /* Create a message queue       */
   rc=DosQueryModuleHandle("MOUSEDLL", (PHMODULE)&hSpyDll);
   if (rc==0) {
      /* Start a hook                             */
      rc=WinSetHook(Thab, 0, HK_JOURNALPLAYBACK,(PFN)MousePlayHookProc, hSpyDll);
      /* Wait for WM_QUIT message                 */
      while( WinGetMsg(Thab , &qmsg, 0, 0, 0 ) );
      if (TextLen>0) {
         rc=WinSetHook(Thab, 0, HK_JOURNALPLAYBACK,(PFN)KeyPlayHookProc, hSpyDll);
         /* Wait for WM_QUIT message                 */
         while( WinGetMsg(Thab , &qmsg, 0, 0, 0 ) );
      }
   }
   WinDestroyMsgQueue( Thmq );
   WinTerminate( Thab );
   _endthread();
   return;
}
