// mshell.c  a set runworkplace=mshell.exe in config.sys shell (os2 2.0)


#define INCL_BASE
#define INCL_PM
#include <os2.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "def.h"
#include "mshell.h"
#include "pmassert.h"



// -------------------------------------------------------------------


int main( int argc, char **argv )
{
  BOOL          bOK;
  HAB           hab;
  HMQ           hmq;
  HSWITCH       hsw;
  HWND          hwndClient;
  HWND          hwndFrame;
  PGLOBALS      pg;
  QMSG          qmsg;
  SWCNTRL       swctl;
  ULONG         CtlData;
  ULONG         rc;


  hab = WinInitialize( 0 );
  hmq = WinCreateMsgQueue( hab, 0 );

  WinRegisterClass( hab, CLASSNAME, ClientWinProc, CS_SIZEREDRAW, sizeof( PGLOBALS ) );

  // create invisible
  CtlData = FCF_SYSMENU | FCF_TITLEBAR | FCF_SIZEBORDER | FCF_MINMAX |  FCF_MENU | FCF_ICON;
  hwndFrame = WinCreateStdWindow( HWND_DESKTOP, 0, &CtlData, CLASSNAME, CAPTION, 0, (HMODULE)0, ID_PMPGM, &hwndClient );
  pmassert( hab, hwndFrame );

  // get global pointer stored by wm_create processing
  pg = (PGLOBALS) WinQueryWindowULong( hwndClient, QWL_USER );
  pmassert( hab, pg );

  // store my msg queue handle in there
  pg->hmq = hmq;

  // set position of minimized mshell window
  bOK = WinSetWindowUShort( pg->hwndFrame, QWS_XMINIMIZE, (USHORT)pg->profile.swpMinimized.x );
  pmassert( hab, bOK );
  bOK = WinSetWindowUShort( pg->hwndFrame, QWS_YMINIMIZE, (USHORT)pg->profile.swpMinimized.y );
  pmassert( hab, bOK );

  // use profiled placement for restored size
  rc = WinSetWindowPos( pg->hwndFrame,
                        (HWND)0,
                        pg->profile.swp.x,
                        pg->profile.swp.y,
                        pg->profile.swp.cx,
                        pg->profile.swp.cy,
                        SWP_SIZE | SWP_MOVE | SWP_ACTIVATE );
  pmassert( hab, rc );

  // add program to task list
  memset( &swctl, 0, sizeof( SWCNTRL ));
  strcpy( swctl.szSwtitle, CAPTION    );
  swctl.hwnd          = hwndFrame;
  swctl.uchVisibility = SWL_VISIBLE;
  swctl.fbJump        = SWL_JUMPABLE;
  hsw = WinAddSwitchEntry( &swctl );

  // make it visible
  WinShowWindow( pg->hwndFrame, TRUE );

  // use break statement to get out of this message loop
  for(;;) {

#if 1
    // until I call winshutdownsystem, toss out quit messages
    if( ! pg->fShutdownCalled ) {
      WinPeekMsg( hab, &qmsg, 0, WM_QUIT, WM_QUIT, PM_REMOVE );
    }

    if( WinGetMsg( hab, &qmsg, 0, 0, 0 )) {
      WinDispatchMsg( hab, &qmsg );
    } else break;

#endif

#if 0
    WinGetMsg( hab, &qmsg, 0, 0, 0 );
    WinDispatchMsg( hab, &qmsg );
#endif

  }


  // wrap up
  WinRemoveSwitchEntry( hsw );
  WinDestroyWindow ( hwndFrame );
  WinDestroyMsgQueue ( hmq );
  WinTerminate ( hab );

  return 0;
}




//--------------------------------------------------------------------------------------------------



MRESULT EXPENTRY ClientWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  PGLOBALS            pg;
  RECTL               rectl;
  ULONG               rc;
  LONG                lrc;
  char                szWork[ LEN_WORKSTRING ];
  FILE                *f;
  SWP                 swp;




  switch( msg ) {



  case WM_CLOSE:
    // mshell does not close, but at least it can minimize and get out of the way
    pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );
    WinSetWindowPos( pg->hwndFrame, HWND_BOTTOM, 0, 0, 0, 0, SWP_MINIMIZE );
    return (MRESULT) 0;


  case WM_CREATE:
    // see create.c
    pg = Create( hwnd );
    break;

  case WM_COMMAND:
    // see menu.c
    return Command( hwnd, msg, mp1, mp2 );


  case WM_CONTROL:
    pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );
    switch( SHORT1FROMMP( mp1 )) {
    case ID_LISTBOX:
      switch( SHORT2FROMMP( mp1 )) {
      case LN_ENTER:
        // user wants to start a program
        WinSendMsg( hwnd, WM_USER_DISABLE_CLIENT, 0, 0 );
        // see object.c
        // get index from listbox; works as index to aStartem array
        lrc = (LONG) WinSendMsg( pg->hwndListbox, LM_QUERYSELECTION, (MPARAM)LIT_FIRST, 0 );
        pmassert( pg->hab, LIT_NONE != lrc );
        // pass index of program to start in mp2
        WinPostMsg( pg->hwndObject, WM_USER_START,  (MPARAM) hwnd, (MPARAM) lrc );
        break;
      }
      break;
    }
    return (MRESULT) 0;


  case WM_ERASEBACKGROUND:
    // see Petzold, WM_PAINT below
    return (MRESULT) 1;



  case WM_MOUSEMOVE:
    // display which pointer?
    if( WinIsWindowEnabled( hwnd )) {
      // not disabled; display regular pointer
      WinSetPointer( HWND_DESKTOP, WinQuerySysPointer( HWND_DESKTOP, SPTR_ARROW, FALSE ));
    } else {
      // disabled; display hourglass
      WinSetPointer( HWND_DESKTOP, WinQuerySysPointer( HWND_DESKTOP, SPTR_WAIT, FALSE ));
    }
    return (MRESULT) 0;








  case WM_NACK_NO_INI:
    pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );
    WinSendMsg( hwnd, WM_USER_ENABLE_CLIENT, 0, 0 );
#define PSZNOINIPROMPT "\
Can't find \\MSHELL.INI, a plain text file used to configure MShell.\n\
\n\
Create a default one?"
    rc = (ULONG)WinMessageBox( HWND_DESKTOP, pg->hwndFrame, PSZNOINIPROMPT, CAPTION, 0, MB_ICONEXCLAMATION | MB_YESNOCANCEL );
    if( MBID_YES == rc ) {
#define PSZDEFINI "\
* MSHELL.INI defines programs that MShell can start\n\
* Configure MSHELL.EXE using the RUNWORKPLACE setting in CONFIG.SYS\n\
* MSHELL.EXE finds its INI in the root of the boot drive\n\
* Each line in the INI file has two parts:\n\
*  1: program title text to appear in MShell window\n\
*  2: the CMD.EXE start command to start the program\n\
* Separate the parts with a semicolon\n\
* Lines starting with ! start automatically at bootup\n\
* Comment lines begin with  *\n\
\n\
* command processors\n\
OS/2 Command Prompt; start /f\n\
DOS Command Prompt; start /f /fs /dos\n\
Win-OS/2; start /fs /dos /c winos2\n\
\n\
* other stuff\n\
Help for Start; start help start\n\
Solitaire; start /f klondike\n\
\n\
* example to start from other than C:\\\n\
* PM Program; d: & cd \\pmpgm & start pmpgm\n"

      if( f = fopen( "\\MSHELL.INI", "w" )) {
        fprintf( f, PSZDEFINI );
        fclose( f );
      }
      // make like user pressed refresh
      WinPostMsg( hwnd, WM_COMMAND, (MPARAM)IDM_REFRESH, 0 );
    }
    return (MRESULT) 0;





  case WM_NACK_NO_SPOOLER:
    pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );
    WinMessageBox( HWND_DESKTOP, pg->hwndFrame, "Sorry, could not start the spooler.", CAPTION, 0, MB_CANCEL );
    return (MRESULT) 0;


  case WM_NACK_SYNTAX_ERROR:
    pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );
    // line number of INI file in mp2
    sprintf( szWork, "Line %d of \\mshell.ini is unrecognized.", (SHORT) mp2 );
    WinMessageBox( HWND_DESKTOP, pg->hwndFrame, szWork, CAPTION, 0, MB_CANCEL );
    return (MRESULT) 0;


  case WM_PAINT:
    // see WM_ERASEBACKGROUND above;
    // see Petzold sections on control windows which are children of the client window
    break;



  case WM_SAVEAPPLICATION:
    // save restored position
    pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );
    WinQueryWindowPos( pg->hwndFrame, &swp );

    if( swp.fl &  SWP_MINIMIZE ) {
      // mshell is currently minimized
      pg->profile.swpMinimized.x  = swp.x;
      pg->profile.swpMinimized.y  = swp.y;
      // get restored size from window words
      pg->profile.swp.x  = WinQueryWindowUShort( pg->hwndFrame, QWS_XRESTORE  );
      pg->profile.swp.y  = WinQueryWindowUShort( pg->hwndFrame, QWS_YRESTORE  );
      pg->profile.swp.cx = WinQueryWindowUShort( pg->hwndFrame, QWS_CXRESTORE );
      pg->profile.swp.cy = WinQueryWindowUShort( pg->hwndFrame, QWS_CYRESTORE );
    } else if( swp.fl &  SWP_MAXIMIZE ) {
      // mshell is currently maximized
      // get restored size from window words
      pg->profile.swp.x  = WinQueryWindowUShort( pg->hwndFrame, QWS_XRESTORE  );
      pg->profile.swp.y  = WinQueryWindowUShort( pg->hwndFrame, QWS_YRESTORE  );
      pg->profile.swp.cx = WinQueryWindowUShort( pg->hwndFrame, QWS_CXRESTORE );
      pg->profile.swp.cy = WinQueryWindowUShort( pg->hwndFrame, QWS_CYRESTORE );
      // get minimized icon position from window words
      pg->profile.swpMinimized.x  = WinQueryWindowUShort( pg->hwndFrame, QWS_XMINIMIZE );
      pg->profile.swpMinimized.y  = WinQueryWindowUShort( pg->hwndFrame, QWS_YMINIMIZE );
    } else {
      // mshell is in the restored state
      pg->profile.swp = swp;
      // get minimized icon position from window words
      pg->profile.swpMinimized.x  = WinQueryWindowUShort( pg->hwndFrame, QWS_XMINIMIZE );
      pg->profile.swpMinimized.y  = WinQueryWindowUShort( pg->hwndFrame, QWS_YMINIMIZE );
    }
    // write to ini
    PrfWriteProfileData( HINI_PROFILE, INI_APP, INIKEY_PROFILE, (PVOID)&pg->profile, sizeof( PROFILE ));

#if 0
    // save restored position
    // TO DO: save icon position
    pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );
    WinQueryWindowPos( pg->hwndFrame, &(pg->profile.swp) );
    if( 0 == ( pg->profile.swp.fl & ( SWP_MINIMIZE | SWP_MAXIMIZE ))) {
      // app in restored state, swp struct fine the way it is.
    }else{
      // app currently min'd or max'd. pull restore info from win words
      pg->profile.swp.x  = WinQueryWindowUShort( pg->hwndFrame, QWS_XRESTORE);
      pg->profile.swp.y  = WinQueryWindowUShort( pg->hwndFrame, QWS_YRESTORE);
      pg->profile.swp.cx = WinQueryWindowUShort( pg->hwndFrame, QWS_CXRESTORE);
      pg->profile.swp.cy = WinQueryWindowUShort( pg->hwndFrame, QWS_CYRESTORE);
    }
    // write to ini
    PrfWriteProfileData( HINI_PROFILE, INI_APP, INIKEY_PROFILE, (PVOID)&(pg->profile), sizeof( PROFILE ));
#endif

    break;



  case WM_SIZE:
    pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );
    WinQueryWindowRect( hwnd, &rectl );
    WinSetWindowPos( pg->hwndListbox, HWND_TOP, 0, 0,  rectl.xRight, rectl.yTop, SWP_SIZE | SWP_MOVE | SWP_SHOW );
    return (MRESULT) 0;



  case WM_USER_ACK:
    // re-enable the window
    pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );
    switch( (ULONG)mp1 ) {
    case WM_CREATE:
      WinSetFocus( HWND_DESKTOP, pg->hwndListbox );
      break;
    }
    WinSendMsg( hwnd, WM_USER_ENABLE_CLIENT, 0, 0 );
    return (MRESULT) 0;



  case WM_USER_DISABLE_CLIENT:
    // this message sent; disable menu action bar as well
    pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );
    WinEnableWindow( pg->hwndClient,  FALSE );
    WinEnableWindow( pg->hwndMenubar, FALSE );
    WinEnableWindow( pg->hwndListbox, FALSE );
    return (MRESULT) 0;


  case WM_USER_ENABLE_CLIENT:
    // this message sent; enable client and action bar
    pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );
    WinEnableWindow( pg->hwndClient,  TRUE );
    WinEnableWindow( pg->hwndMenubar, TRUE );
    WinEnableWindow( pg->hwndListbox, TRUE );
    return (MRESULT) 0;
  }

  return( WinDefWindowProc( hwnd, msg, mp1, mp2 ));
}


