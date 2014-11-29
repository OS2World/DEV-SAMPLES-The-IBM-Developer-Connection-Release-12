// rxdraw.c
#define INCL_PM
#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "rxdraw.h"
#include "pmassert.h"

// Global variables are in one C struct; see rxdraw.h.
GLOBALS globals;
MRESULT APIENTRY ClientWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
// ------------------------------------------------------------------------------------------------------------
int main( int argc, char **argv )
{
  HMQ           hmq;
  QMSG          qmsg;
  ULONG         ulWork;
  SWP           swp;
  LONG          cx;

  // initialize globals struct to defaults
  memset( &globals, 0, sizeof( globals ));
  strcpy( globals.szRexxFileName, "DRAW.CMD" );
  // parameter is a REXX filename
  if( argc > 1 ) {
    strcpy( globals.szRexxFileName, argv[ 1 ] );
  }
  // PM application init
  globals.hab = WinInitialize( 0 );
  hmq = WinCreateMsgQueue( globals.hab, 0);
  // create a frame window with listbox to display REXX diagnostic output
  ulWork = FCF_SYSMENU | FCF_TITLEBAR | FCF_SIZEBORDER | FCF_MINMAX | FCF_ICON | FCF_TASKLIST;
  globals.hwndOutputFrame = WinCreateStdWindow( HWND_DESKTOP, 0, &ulWork, WC_LISTBOX, OUTPUTCAPTION, LS_NOADJUSTPOS,
                                               (HMODULE)0, ID_RXDRAW, &globals.hwndOutputListbox );
  assert( globals.hwndOutputFrame   );
  assert( globals.hwndOutputListbox );
  // create a frame window to display REXX graphical output
  WinRegisterClass( globals.hab, CLASSNAME, (PFNWP)ClientWinProc, CS_SIZEREDRAW, 0 );
  ulWork = FCF_SYSMENU | FCF_TITLEBAR | FCF_SIZEBORDER | FCF_MINMAX | FCF_MENU | FCF_ICON | FCF_TASKLIST;
  globals.hwndFrame = WinCreateStdWindow( HWND_DESKTOP, 0, &ulWork, CLASSNAME, CAPTION, 0,
                                         (HMODULE)0, ID_RXDRAW, &globals.hwndClient );
  assert( globals.hwndFrame  );
  assert( globals.hwndClient );
  // tile the two windows using shell's preferred placement for one
  WinQueryTaskSizePos( globals.hab, 0, &swp );
  cx = (swp.cx * 2) / 3;
  ulWork = SWP_SIZE | SWP_MOVE | SWP_ZORDER | SWP_SHOW | SWP_ACTIVATE | SWP_RESTORE;
  WinSetWindowPos( globals.hwndOutputFrame, HWND_TOP, swp.x+cx, swp.y, swp.cx-cx, swp.cy, ulWork );
  WinSetWindowPos( globals.hwndFrame, HWND_TOP, swp.x, swp.y, cx, swp.cy, ulWork );
  // message loop
  while( WinGetMsg( globals.hab, &qmsg, 0, 0, 0 ))   WinDispatchMsg( globals.hab, &qmsg );
  if( !globals.fCloseMsgSent ) {
    // got a WM_QUIT without WM_CLOSE; must be shutdown or Close from Window List
    WinPostMsg( globals.hwndObject, WM_QUIT, 0, 0 );
  }
  // clean up
  WinDestroyWindow( globals.hwndFrame );
  WinDestroyWindow( globals.hwndOutputFrame );
  WinDestroyMsgQueue( hmq );
  WinTerminate( globals.hab );
  DosWaitThread( &globals.tidObject, DCWW_WAIT );
  return 0;
}
// ------------------------------------------------------------------------------------------------------------
MRESULT EXPENTRY ClientWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  BOOL                fOK;
  RECTL               rectl;
  SIZEL               sizel;
  HPS                 hps;

  switch( msg ) {
  case WM_CLOSE:
    globals.fCloseMsgSent = TRUE;
    WinSendMsg( hwnd, WM_USER_DISABLE_CLIENT, 0, 0 );
    WinPostMsg( globals.hwndObject, WM_QUIT, 0, 0 );
    return 0;
  case WM_COMMAND:
    switch( SHORT1FROMMP( mp1 )) {
    case ID_RUN:
      WinSendMsg( hwnd, WM_USER_DISABLE_CLIENT, 0, 0 );
      WinInvalidateRect( hwnd, NULL, FALSE );
      WinPostMsg( globals.hwndObject, WM_USER_START_REXX, (MPARAM)hwnd, 0 );
      break;
    }
    return 0;
  case WM_CREATE:
    // disable client window until application initializes
    WinSendMsg( hwnd, WM_USER_DISABLE_CLIENT, 0, 0 );
    globals.hwndFrame    = WinQueryWindow( hwnd, QW_PARENT );
    globals.hwndTitlebar = WinWindowFromID( globals.hwndFrame, FID_TITLEBAR );
    globals.hwndMenubar  = WinWindowFromID( globals.hwndFrame, FID_MENU );
    // create title bar text; set into title bar
    sprintf( globals.szCaption, "%s - %s", CAPTION, globals.szRexxFileName );
    WinSetWindowText( globals.hwndTitlebar, globals.szCaption );
    // get window DC
    globals.hdcScreen = WinOpenWindowDC( hwnd );
    assert( globals.hdcScreen );
    // create a normal PS for the screen; coordinate system is 1/100 inch per unit of measure
    sizel.cx = sizel.cy = 0;
    globals.hpsScreen = GpiCreatePS( globals.hab, globals.hdcScreen, &sizel, PU_LOENGLISH | GPIF_LONG | GPIT_NORMAL | GPIA_ASSOC );
    assert( globals.hpsScreen );
    // object window operates on thread 2
    globals.tidObject = _beginthread( threadmain, NULL, LEN_STACK, NULL );
    assert( globals.tidObject );
    return 0;
  case WM_MOUSEMOVE:
    WinSetPointer( HWND_DESKTOP, WinQuerySysPointer( HWND_DESKTOP, globals.fBusy ? SPTR_WAIT:SPTR_ARROW, FALSE ));
    return (MRESULT)1;
  case WM_PAINT:
    hps = WinBeginPaint( hwnd, globals.hpsScreen, &rectl );
    fOK = WinFillRect( hps, &rectl, SYSCLR_WINDOW );
    assert( fOK );
    if( !globals.fBusy ) {
      // playback the retained GPI drawing orders
      GpiDrawChain( globals.hpsScreen );
    }
    WinEndPaint( hps );
    return 0;
  case WM_USER_ACK:
    // object window has posted ack; re-enable the window
    WinSendMsg( hwnd, WM_USER_ENABLE_CLIENT, 0, 0 );
    return 0;
  case WM_USER_DISABLE_CLIENT:
    globals.fBusy = TRUE;
    WinEnableWindow( globals.hwndClient, FALSE );
    WinEnableMenuItem( globals.hwndMenubar, ID_RUN, FALSE );
    return 0;
  case WM_USER_ENABLE_CLIENT:
    globals.fBusy = FALSE;
    WinEnableWindow( globals.hwndClient, TRUE );
    WinEnableMenuItem( globals.hwndMenubar, ID_RUN, TRUE );
    return 0;
  }
  return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}
