// pmapp.c

/*

This is a test program that shows how a PM application can detect
a kill signal and still terminate in the normal way.

The exception handler jumps back into the main() function where
it posts itself a WM_QUIT message and jumps back into the
get/dispatch message loop.

PM apps are not guaranteed to receive a WM_CLOSE. (Press Del from
the window list.) Thus, the WM_CLOSE case in the window procedure
is not a good place for clean up code. Put PM app cleanup code
after the get/dispatch message loop.

This program shows its process ID in its window. Use the KILL.EXE
utility to kill it.

*/

// os2 includes
#define INCL_DOS
#define INCL_PM
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>


#define assert(exp) { if(!(exp)) {\
  char ach[ 64 ]; USHORT rc;\
  sprintf( ach, "Line %d\nFile %s\nLast Error %p\nExpression %s\n", __LINE__, __FILE__, WinGetLastError(0), #exp );\
  rc = WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, ach, "Assertion failed. Continue?", 0, MB_YESNO  );\
  if( rc == MBID_NO ) exit( 1 ); }\
}

#define ID_APP            100
#define APP_CLASS_CLIENT  "APPClient"
#define APP_TITLE         "Application"

//-----------------------------------------------------------------------
// typedef a "super" registration record

struct _regrec
{
  PVOID                 pnext;       // same as OS/2's
  PFN                   pfn;         // same as OS/2's
  EXCEPTIONREPORTRECORD report;      // a copy of the exception report
  jmp_buf               jumpBuffer;  // for C setjmp()/longjmp()
};
typedef struct _regrec REGREC, *PREGREC;



// ----------------------------------------------------------------------
// the exception handler function

ULONG APIENTRY Handler( PEXCEPTIONREPORTRECORD p1, PREGREC p2, PCONTEXTRECORD p3, PVOID pv )
{
  APIRET           rc;

  if( XCPT_SIGNAL == p1->ExceptionNum )
  {
    // make a copy of the exception report record
    p2->report = *p1;

    // enable further signals
    rc = DosAcknowledgeSignalException( p1->ExceptionInfo[ 0 ] );
    assert( 0 == rc );

    // jump to setjmp(); TRUE means an exception happened
    longjmp( p2->jumpBuffer, TRUE );
  }

  // not interested; system should continue search
  return XCPT_CONTINUE_SEARCH;
}


// ----------------------------------------------------------------------
MRESULT APIENTRY ClientWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  CHAR             szWork[ 64 ];
  HPS              hps;
  RECTL            rectl;
  POINTL           pointl;
  PPIB             ppib;
  PTIB             ptib;



  switch( msg ) {
  case WM_PAINT:
    hps = WinBeginPaint( hwnd, 0, &rectl );
    WinFillRect( hps, &rectl, SYSCLR_WINDOW );

    DosGetInfoBlocks( &ptib, &ppib );
    sprintf( szWork, "Process ID = %X hex.", ppib->pib_ulpid );

    pointl.x = pointl.y = 60;
    GpiCharStringAt( hps, &pointl, strlen( szWork ), szWork );

    WinEndPaint( hps );
    return 0;
  }
  // default
  return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}


// ----------------------------------------------------------------------
int main()
{
  APIRET           rc;
  BOOL             fException;
  BOOL             fOK;
  HAB              hab;
  HMQ              hmq;
  HWND             hwndClient;
  HWND             hwndFrame;
  QMSG             qmsg;
  REGREC           regrec;
  ULONG            flCreate;
  ULONG            ul;


  // defer async exceptions while setting handler and jump buffer
  DosEnterMustComplete( &ul );

  regrec.pfn = (PFN)Handler;
  rc = DosSetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD) &regrec );
  assert( rc == 0 );

  // record a known place of execution
  fException = setjmp( regrec.jumpBuffer );

  DosExitMustComplete( &ul );

  if( fException )
  {
    WinPostMsg( hwndClient, WM_QUIT, 0, 0 );
    goto reentry;
  }



  // PM application init
  hab = WinInitialize( 0 );
  hmq = WinCreateMsgQueue( hab, 0 );

  // register client window class
  fOK = WinRegisterClass( hab, APP_CLASS_CLIENT, (PFNWP)ClientWinProc, CS_SIZEREDRAW | CS_CLIPCHILDREN, 0 );
  assert( fOK );

  flCreate = FCF_SYSMENU | FCF_SIZEBORDER | FCF_TITLEBAR | FCF_MINMAX | FCF_SHELLPOSITION | FCF_TASKLIST;

  // standard window create; returns after WM_CREATE processing finishes
  hwndFrame = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE, &flCreate, APP_CLASS_CLIENT, APP_TITLE, 0, 0, ID_APP, &hwndClient );
  assert( hwndFrame );
  assert( hwndClient );


reentry:

  // dispatch user input messages
  while( WinGetMsg( hab, &qmsg, 0, 0, 0 ))
  {
    WinDispatchMsg( hab, &qmsg );
  }


  // clean up code here

  // wrap up
  WinDestroyWindow( hwndFrame );
  WinDestroyMsgQueue( hmq );
  WinTerminate( hab );


  rc = DosUnsetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD)&regrec );
  assert( 0 == rc );

  // exit the process
  return 0;
}
