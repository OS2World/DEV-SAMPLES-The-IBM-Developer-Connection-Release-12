// object.c
#define INCL_DOS
#define INCL_PM
#define INCL_REXXSAA
#include <os2.h>
#include <rexxsaa.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "rxdraw.h"
#include "pmassert.h"

// this function handles REXX commands passed to its host environment, RXDRAW
ULONG APIENTRY HandleCommand( PRXSTRING prxCommand,  PUSHORT pusFlags, PRXSTRING prxRet );

// this function handles REXX system exits say and trace
LONG APIENTRY HandleExit( LONG lExit, LONG lSubfunc, PUCHAR pParm );

// REXX function handlers -- the underlying worker routines for special REXX functions in the RXDRAW environment.
ULONG APIENTRY Move(  PSZ pszName, ULONG lArgCount, PRXSTRING prxArgStrings, PSZ pszQueueName, PRXSTRING prxRet );
ULONG APIENTRY Line(  PSZ pszName, ULONG lArgCount, PRXSTRING prxArgStrings, PSZ pszQueueName, PRXSTRING prxRet );
ULONG APIENTRY Text(  PSZ pszName, ULONG lArgCount, PRXSTRING prxArgStrings, PSZ pszQueueName, PRXSTRING prxRet );
ULONG APIENTRY Color( PSZ pszName, ULONG lArgCount, PRXSTRING prxArgStrings, PSZ pszQueueName, PRXSTRING prxRet );
// ----------------------------------------------------------------
// thread 2 code; gets and dispatches object window messages
void _Optlink threadmain( void * pv  )
{
  BOOL       fOK;
  HAB        hab;
  HMQ        hmq;
  QMSG       qmsg;

  // thread initialization
  hab = WinInitialize( 0 );
  hmq = WinCreateMsgQueue( hab, 0 );
  fOK = WinCancelShutdown( hmq, TRUE );
  assert( fOK );
  fOK = WinRegisterClass( hab, OBJECTCLASSNAME, (PFNWP)ObjectWinProc, 0, sizeof( PVOID ));
  assert( fOK );
  // create an object window where the parent is the PM object window
  globals.hwndObject = WinCreateWindow( HWND_OBJECT, OBJECTCLASSNAME, "", 0, 0, 0, 0, 0,
                                    HWND_OBJECT, HWND_BOTTOM, 0, NULL, NULL );
  assert( globals.hwndObject );
  WinPostMsg( globals.hwndClient, WM_USER_ACK, (MPARAM)WM_CREATE,  0 );
  // dispatch messages; these messages will be mostly user-defined messages
  while( WinGetMsg( hab, &qmsg, 0, 0, 0 ))   WinDispatchMsg( hab, &qmsg );
  // make thread 1 terminate
  WinPostMsg( globals.hwndClient, WM_QUIT, 0, 0 );
  // clean up
  WinDestroyWindow( globals.hwndObject );
  WinDestroyMsgQueue( hmq );
  WinTerminate( hab );
  // exit thread 2
  return;
}
// -----------------------------------------------------------------------
// object window procedure on thread 2
MRESULT EXPENTRY ObjectWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  BOOL              fOK;
  HWND              hwndToAck;
  ULONG             rc;
  LONG              rcRexxStart;
  RXSTRING          rexxretval;
  SHORT             rexxrc = 0;
  RXSYSEXIT         aExit[ 3 ];

  // store the handle of the window to ack upon task completion;
  hwndToAck = (HWND)mp1;
  switch( msg ) {
  case WM_CREATE:
    // register REXX function handlers with REXX
    rc = RexxRegisterFunctionExe( "MOVE",   Move   );
    rc = RexxRegisterFunctionExe( "LINE",   Line   );
    rc = RexxRegisterFunctionExe( "TEXT",   Text   );
    rc = RexxRegisterFunctionExe( "COLOR",  Color  );
    assert( rc == 0 );
    // register the command handler
    rc = RexxRegisterSubcomExe( "RXDRAW", (PFN)HandleCommand, NULL );
    assert( 0 == rc );
    // register the exit handler
    rc =  RexxRegisterExitExe( CAPTION, (PFN)HandleExit, NULL );
    assert( rc == 0 );
    return 0;
  case WM_USER_START_REXX:
    // this message posted from WM_COMMAND case in RXDRAW.C; erase output window
    WinSendMsg( globals.hwndOutputListbox, LM_DELETEALL, 0, 0 );
    // delete any previously recorded segment numbered 1
    GpiDeleteSegment( globals.hpsScreen, 1 );
    // start retaining GPI calls; like pressing the record button on a tape recorder
    fOK = GpiSetDrawingMode( globals.hpsScreen, DM_DRAWANDRETAIN );
    assert( fOK );
    // open a new segment numbered 1
    fOK = GpiOpenSegment( globals.hpsScreen, 1 );
    assert( fOK );
    // REXX-provided macro to prepare a RXSTRING; REXX programs may use "return" to return a string here
    MAKERXSTRING( rexxretval, NULL, 0 );
    // identify exits to handle
    aExit[0].sysexit_name = CAPTION;
    aExit[0].sysexit_code = RXSIO;
    aExit[1].sysexit_code = RXENDLST;
    // call the interpreter
    rcRexxStart = RexxStart( 0, NULL, globals.szRexxFileName, NULL, "RXDRAW", RXSUBROUTINE, aExit, &rexxrc, &rexxretval );
    if( rexxretval.strptr ) {
      rc = DosFreeMem( rexxretval.strptr );   // REXX allocated it with DosAllocMem()
      assert( 0 == rc );
    }
    // close the segment to stop recording
    fOK = GpiCloseSegment( globals.hpsScreen );
    assert( fOK );
    fOK = GpiSetDrawingMode( globals.hpsScreen, DM_DRAW );
    assert( fOK );
    if( rcRexxStart != 0  || rexxrc != 0 ) {
      // rcRexxStart < 0 means an interpreter error or possibly an incorrect call to function
      // rexxrc != 0 means that the REXX program ran OK, but it exited with a non-zero result code
      // delete the segment because its validity is in doubt
      GpiDeleteSegment( globals.hpsScreen, 1 );
      WinAlarm( HWND_DESKTOP, WA_NOTE );
    }
    // force an update of the client window
    WinInvalidateRect( globals.hwndClient, NULL, FALSE );
    // acknowledge the originating window
    WinPostMsg( hwndToAck, WM_USER_ACK, (MPARAM)msg, 0 );
    return 0;
  }
  return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}
// ---------------------------------------------------------------------------------------------------------------
ULONG APIENTRY Move( PSZ pszName, ULONG lArgCount, PRXSTRING prxArgStrings, PSZ pszQueueName, PRXSTRING prxRet )
{
  BOOL     fOK;
  POINTL   pt;

  if( lArgCount != 2 ) return 40;
  pt.x = atoi(  prxArgStrings[ 0 ].strptr  );
  pt.y = atoi(  prxArgStrings[ 1 ].strptr  );
  fOK = GpiSetCurrentPosition( globals.hpsScreen, &pt );
  assert( fOK );
  prxRet->strlength = 0;
  return 0;
}
// ---------------------------------------------------------------------------------------------------------------
ULONG APIENTRY Line( PSZ pszName, ULONG lArgCount, PRXSTRING prxArgStrings, PSZ pszQueueName, PRXSTRING prxRet )
{
  BOOL     fOK;
  POINTL   pt;

  if( lArgCount != 2 ) return 40;
  pt.x = atoi(  prxArgStrings[ 0 ].strptr  );
  pt.y = atoi(  prxArgStrings[ 1 ].strptr  );
  fOK = GpiLine( globals.hpsScreen, &pt );
  assert( fOK );
  prxRet->strlength = 0;
  return 0;
}
// ---------------------------------------------------------------------------------------------------------------
ULONG APIENTRY Text( PSZ pszName, ULONG lArgCount, PRXSTRING prxArgStrings, PSZ pszQueueName, PRXSTRING prxRet )
{
  BOOL     fOK;

  if( lArgCount != 1 ) return 40;
  fOK = GpiCharString( globals.hpsScreen, strlen( prxArgStrings[ 0 ].strptr ), prxArgStrings[ 0 ].strptr );
  assert( fOK );
  prxRet->strlength = 0;
  return 0;
}
// ---------------------------------------------------------------------------------------------------------------
ULONG APIENTRY Color( PSZ pszName, ULONG lArgCount, PRXSTRING prxArgStrings, PSZ pszQueueName, PRXSTRING prxRet )
{
  BOOL     fOK;

  if( lArgCount != 1 ) return 40;
  fOK = GpiSetColor( globals.hpsScreen, atoi( prxArgStrings[ 0 ].strptr ));
  assert( fOK );
  prxRet->strlength = 0;
  return 0;
}
// ---------------------------------------------------------------------------------------------------------------
ULONG APIENTRY HandleCommand( PRXSTRING prxCommand,  PUSHORT pusFlags, PRXSTRING prxRet )
{
  *pusFlags = RXSUBCOM_OK;
  prxRet->strlength = 0;
  return 0;
}
// ---------------------------------------------------------------------------------------------------------------
LONG APIENTRY HandleExit( LONG lExit, LONG lSubfunc, PUCHAR pParm )
{
  PRXSTRING                prxs;
  LONG                     index;

  switch( lExit ) {
  case RXSIO:
    switch( lSubfunc ) {
    case RXSIOTRC:
    case RXSIOSAY:
      prxs = (PRXSTRING)pParm;       // in this case, pParm is a PRXSTRING
      assert( prxs->strptr );
      index = (LONG)WinSendMsg( globals.hwndOutputListbox, LM_INSERTITEM, (MPARAM)LIT_END, (MPARAM)prxs->strptr );
      WinSendMsg( globals.hwndOutputListbox, LM_SETTOPINDEX, (MPARAM)index, 0 );
      break;
    }
    break;
  }
  return RXEXIT_HANDLED;
}
