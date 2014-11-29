// driver.c testcase for printer setup subroutine
// Monte Copeland for DevCon 7

#define INCL_PM
#define INCL_DOS
#define INCL_SPLDOSPRINT
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "setup.h"
#include "driver.h"
#include "pmassert.h"

// all global variables are in one C struct; see driver.h
GLOBALS globals;


// ------------------------------------------------------------------------------------------------------------
int main( int argc, char **argv )
{
  HMQ           hmq;
  QMSG          qmsg;
  ULONG         ulWork;
  SWP           swp;
  HWND          hwnd;


  // initialize globals struct to defaults
  memset( &globals, 0, sizeof( globals ));

  if( argc > 1 )
  {
    strcpy( globals.szFileName, argv[ 1 ] );
  }

  // PM application init
  globals.hab = WinInitialize( 0 );
  hmq = WinCreateMsgQueue( globals.hab, 0);

  WinRegisterClass( globals.hab, CLASSNAME, (PFNWP)ClientWinProc, CS_SIZEREDRAW, 0 );
  ulWork = FCF_SYSMENU | FCF_TITLEBAR | FCF_SIZEBORDER | FCF_MINMAX | FCF_MENU | FCF_TASKLIST;
  globals.hwndFrame = WinCreateStdWindow( HWND_DESKTOP, 0, &ulWork, CLASSNAME, CAPTION, 0, (HMODULE)0, ID_DRIVER, &hwnd );
  assert( globals.hwndFrame  );
  assert( hwnd );
  assert( hwnd == globals.hwndClient );

  WinQueryTaskSizePos( globals.hab, 0, &swp );
  ulWork = SWP_SIZE | SWP_MOVE | SWP_ZORDER | SWP_SHOW | SWP_ACTIVATE | SWP_RESTORE;
  WinSetWindowPos( globals.hwndFrame, HWND_TOP, swp.x, swp.y, swp.cx/2, swp.cy/2, ulWork );

  // message loop
  while( WinGetMsg( globals.hab, &qmsg, (HWND)0, 0, 0 )) {
    WinDispatchMsg( globals.hab, &qmsg );
  }

  if( !globals.fCloseMsgSent ) {
    // given a WM_QUIT without WM_CLOSE; must be shutdown or Close from Window List
    WinPostMsg( globals.hwndObject, WM_QUIT, 0, 0 );
  }

  // clean up
  CleanupPrinter( &globals.Setup );

  WinDestroyWindow( globals.hwndFrame );
  WinDestroyMsgQueue( hmq );
  WinTerminate( globals.hab );

  DosWaitThread( &globals.tidObject, DCWW_WAIT );
  return 0;
}

// ------------------------------------------------------------------------------------------------------------
MRESULT EXPENTRY ClientWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  BOOL                fOK;
  CHAR                szWork[ 256 ];
  HPS                 hps;
  PSZ                 psz;
  RECTL               rectl;
  ULONG               rc;

  switch( msg ) {
  case WM_CLOSE:
    globals.fCloseMsgSent = TRUE;
    WinSendMsg( hwnd, WM_USER_DISABLE_CLIENT, 0, 0 );
    WinPostMsg( globals.hwndObject, WM_QUIT, 0, 0 );
    return 0;

  case WM_COMMAND:
    switch( SHORT1FROMMP( mp1 )) {
    case IDM_PRINT:

      psz = NULL;

      if( 0 == globals.Setup.cQueues )
      {
        psz = "You have no printers on this system.";
      }
      else if( !globals.Setup.hpsPrinterInfo )
      {
        psz = "There is a problem with the printer setup. Try a different printer.";
      }
      else if( 0 == strlen( globals.szFileName ))
      {
        psz = "Give file name on the command line.";
      }

      if( psz )
      {
        // Error detected.
        WinMessageBox( HWND_DESKTOP, globals.hwndFrame, psz, CAPTION, 0, MB_CANCEL );
      }
      else
      {
        // OK. Object window may discover a file-not-found problem.
        WinSendMsg( hwnd, WM_USER_DISABLE_CLIENT, 0, 0 );
        WinPostMsg( globals.hwndObject, WM_USER_PRINT, (MPARAM)hwnd, 0);
      }
      break;

    case IDM_PRINTERSETUP:
      rc = SetupPrinter( &globals.Setup, SETUP_SHOWDIALOG );
      assert( 0 == rc );
      break;
    }
    return 0;

  case WM_CREATE:
    // Store important window handles.
    globals.hwndFrame    = WinQueryWindow( hwnd, QW_PARENT );
    globals.hwndTitle    = WinWindowFromID( globals.hwndFrame, FID_TITLEBAR );
    globals.hwndMenu     = WinWindowFromID( globals.hwndFrame, FID_MENU );
    globals.hwndClient   = hwnd;

    if( strlen( globals.szFileName ))
    {
      sprintf( szWork, "%s: %s", CAPTION, globals.szFileName );
      WinSetWindowText( globals.hwndTitle, szWork );
    }

    // Make first call to printer setup, but don't show any dialogs.
    globals.Setup.hab               = globals.hab;
    globals.Setup.lWorldCoordinates = PU_TWIPS;
    globals.Setup.hwndFrame         = globals.hwndFrame;
    rc = SetupPrinter( &globals.Setup, SETUP_NODIALOG );
    assert( 0 == rc );

    // Create thread 2 to operate object window.
    WinSendMsg( hwnd, WM_USER_DISABLE_CLIENT, 0, 0 );
    globals.tidObject = _beginthread( threadmain, NULL, LEN_STACK, NULL );
    assert( globals.tidObject );
    return 0;

  case WM_MOUSEMOVE:
    WinSetPointer( HWND_DESKTOP, WinQuerySysPointer( HWND_DESKTOP, globals.fBusy ? SPTR_WAIT:SPTR_ARROW, FALSE ));
    return (MRESULT)1;

  case WM_PAINT:
    hps = WinBeginPaint( hwnd, 0, &rectl );
    fOK = WinFillRect( hps, &rectl, SYSCLR_WINDOW );
    assert( fOK );
    WinEndPaint( hps );
    return 0;

  case WM_USER_ACK:
    // Object window acks. mp1 is msg posted, mp2 is result code.
    WinSendMsg( hwnd, WM_USER_ENABLE_CLIENT, 0, 0 );
    switch( (LONG)mp1 ) {
    case WM_CREATE:
      break;
    case WM_USER_PRINT:
      if( mp2 )
      {
        // problem printing. Give a message about it.
        sprintf( szWork, "File '%s' not found.", globals.szFileName );
        WinMessageBox( HWND_DESKTOP, globals.hwndFrame, szWork, CAPTION, 0, MB_CANCEL );
      }
      break;
    }
    return 0;

  case WM_USER_DISABLE_CLIENT:
    globals.fBusy = TRUE;
    WinEnableWindow( globals.hwndClient, FALSE );
    WinEnableWindow( globals.hwndMenu,   FALSE );
    return 0;

  case WM_USER_ENABLE_CLIENT:
    globals.fBusy = FALSE;
    WinEnableWindow( globals.hwndClient, TRUE );
    WinEnableWindow( globals.hwndMenu,   TRUE );
    return 0;
  }
  return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}
