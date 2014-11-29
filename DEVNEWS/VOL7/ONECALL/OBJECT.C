// object.c
// Monte Copeland for DevCon 7

#define INCL_DOS
#define INCL_PM
#define INCL_SPLDOSPRINT
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "setup.h"
#include "driver.h"
#include "pmassert.h"

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

  // Create an invisible object window where the parent is the PM object window.
  globals.hwndObject = WinCreateWindow( HWND_OBJECT, OBJECTCLASSNAME, "", 0, 0, 0, 0, 0,
                                    HWND_OBJECT, HWND_BOTTOM, 0, NULL, NULL );
  assert( globals.hwndObject );

  WinPostMsg( globals.hwndClient, WM_USER_ACK, (MPARAM)WM_CREATE, 0 );

  // dispatch messages; these messages will be mostly user-defined messages
  while( WinGetMsg( hab, &qmsg, (HWND)0,  0, 0 ))
  {
    WinDispatchMsg( hab, &qmsg );
  }

  // Make thread 1 terminate.
  WinPostMsg( globals.hwndClient, WM_QUIT, 0, 0 );

  // Clean up.
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
  CHAR              szWork[ 256 ];
  FATTRS            fattrs;
  FILE *            f;
  FONTMETRICS       fm;
  HDC               hdc;
  HPS               hps;
  HWND              hwndToAck;
  LONG              lrc;
  PCHAR             pch;
  POINTL            pointl;
  SIZEF             sizef;
  SIZEL             sizel;
  ULONG             ul;



  // store the handle of the window to ack upon task completion;
  hwndToAck = (HWND)mp1;

  switch( msg ) {
  case WM_CREATE:
    return 0;

  case WM_USER_PRINT:
    f = fopen( globals.szFileName, "r" );
    if( f )
    {
      // Use lDCType and pDevOpenData from the PRINTERSETUP structure.
      hdc = DevOpenDC( globals.hab, globals.Setup.lDCType, "*", 9, globals.Setup.pDevOpenData, (HDC)0 );
      assert( hdc );

      // Create the PS. Leave sizel zero because of DC association.
      sizel.cx = 0;
      sizel.cy = 0;
      hps = GpiCreatePS( globals.hab, hdc, &sizel, globals.Setup.lWorldCoordinates | GPIA_ASSOC );
      assert( hps != GPI_ERROR );

      // Tell printer driver to start recording.
      lrc = DevEscape( hdc, DEVESC_STARTDOC, strlen(globals.szFileName), globals.szFileName, NULL, NULL );
      assert( lrc != DEV_ERROR );

      // Create a fixed-space Courier font.
      // I expect to get a scalable font because I used FATTR_FONTUSE_OUTLINE.
      memset( &fattrs, 0, sizeof( fattrs ));
      fattrs.usRecordLength = sizeof( fattrs );
      strcpy( fattrs.szFacename, "Courier" );
      fattrs.fsFontUse = FATTR_FONTUSE_OUTLINE;

      // I supply my own font id == 1.
      lrc = GpiCreateLogFont( hps, NULL, 1, &fattrs );
      assert( lrc == FONT_MATCH );

      fOK = GpiSetCharSet( hps, 1 );
      assert( fOK );

      fOK = GpiQueryFontMetrics( hps, sizeof( fm ), &fm );
      assert( fOK );
      assert( fm.fsDefn & FM_DEFN_OUTLINE );

      // Size it to 9 point. In PU_TWIPS, 20 twips = 1 point, so 180 twips = 9 point.
      sizef.cx = MAKEFIXED( 180, 0 );
      sizef.cy = MAKEFIXED( 180, 0 );
      fOK = GpiSetCharBox( hps, &sizef );
      assert( fOK );

      // ALWAYS query font metrics again after a change to char box attribute.
      fOK = GpiQueryFontMetrics( hps, sizeof( fm ), &fm );
      assert( fOK );

      // Store the imageable size of the page.
      ul = GpiQueryPS( hps, &sizel );
      assert( ul != GPI_ERROR );

      // Start here.
      pointl.x = 0;
      pointl.y = sizel.cy - fm.lMaxBaselineExt;

      while( fgets( szWork, sizeof( szWork ), f ))
      {
        // fgets() leaves a new line at the end. Remove it.
        pch = strrchr( szWork, '\n' );
        assert( pch );
        *pch = 0;

        lrc = GpiCharStringAt( hps, &pointl, strlen( szWork ), szWork );
        assert( lrc );

        pointl.y -= fm.lMaxBaselineExt;
        if( pointl.y < fm.lMaxBaselineExt )
        {
          // eject the page
          lrc = DevEscape( hdc, DEVESC_NEWFRAME, 0, NULL, NULL, NULL );
          assert( lrc != DEV_ERROR );

          pointl.y = sizel.cy - fm.lMaxBaselineExt;
        }
      }

      fOK = GpiSetCharSet( hps, 0 );
      assert( fOK );

      fOK = GpiDeleteSetId( hps, 1 );
      assert( fOK );

      lrc = DevEscape( hdc, DEVESC_ENDDOC, 0, NULL, NULL, NULL );
      assert( lrc != DEV_ERROR );

      GpiAssociate( hps, 0 );
      GpiDestroyPS( hps );
      DevCloseDC( hdc );

      lrc = 0;
    }
    else
    {
      lrc = 2;
    }

    WinPostMsg( hwndToAck, WM_USER_ACK, (MPARAM)msg, (MPARAM)lrc );
    return (MRESULT)lrc;
  }
  return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

