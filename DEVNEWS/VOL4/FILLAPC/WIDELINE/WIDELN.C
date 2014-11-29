// Sample program to draw geometric widelines
//

#define INCL_32
#define INCL_GPI
#define INCL_WIN
#include <os2.h>

#include "wideln.h"

HAB     hab;
HWND    hwndFrame;
PFNWP   pfnwpFrame;

#define WIDELINE_CLASSNAME "WIDELINE"

MRESULT EXPENTRY ClientWndProc( HWND hwnd ,USHORT msg ,MPARAM mp1 ,MPARAM mp2 );
VOID DrawPolyLine( HPS hps, PPOINTL pptlStart, LONG idShape );
VOID DrawWideline( HPS hps, PPOINTL pptlStart );
VOID DrawWideline2( HWND hwnd, PPOINTL pptlStart );

LONG lLineEnd  = LINEEND_DEFAULT;
LONG lLineJoin = LINEJOIN_DEFAULT;
LONG idShape   = IDM_RIGHTANGLE;

int cdecl main(VOID)
{

   HMQ hmq;
   HWND hwndclient;
   QMSG qmsg;

   ULONG flCreateFlags =   FCF_BORDER     | FCF_SHELLPOSITION |
                           FCF_TASKLIST   | FCF_TITLEBAR      | FCF_SYSMENU |
                           FCF_SIZEBORDER | FCF_MINMAX        | FCF_MENU ;

   hab = WinInitialize( (USHORT)NULL );

   hmq=WinCreateMsgQueue( hab,0 );

   WinRegisterClass( hab
                   , WIDELINE_CLASSNAME
                   , (PFNWP)ClientWndProc
                   , (ULONG)CS_SIZEREDRAW
                   , (USHORT)256 );

   hwndFrame = WinCreateStdWindow( HWND_DESKTOP
                                 ,  0UL
                                 ,  &flCreateFlags
                                 ,  WIDELINE_CLASSNAME
                                 ,  "Wideline"
                                 ,  WS_VISIBLE
                                 ,  (HMODULE)0
                                 ,  ID_PIN
                                 ,  &hwndclient );



   WinShowWindow( hwndFrame, TRUE );


   while ( WinGetMsg( hab,&qmsg, (HWND)0, 0, 0 ) )
   {
     WinDispatchMsg( hab,&qmsg );
   }

   if ( hwndFrame )
   {
     WinDestroyWindow(hwndFrame);
     WinDestroyMsgQueue(hmq);
     WinTerminate(hab);
   }
   return 0;
}


// ------------------------------------------------------------------------
//
//  Client window procedure.
//
// -------------------------------------------------------------------------
MRESULT EXPENTRY ClientWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{

   switch (msg)
   {
     case  WM_COMMAND :
     {
        switch (SHORT1FROMMP(mp1))
        {
          case IDM_EXIT:
          {
            WinPostMsg( hwnd, WM_CLOSE, 0, 0 );
          }
          break;

          case  IDM_ENDDEFAULT:
          case  IDM_ENDFLAT:
          case  IDM_ENDSQUARE:
          case  IDM_ENDROUND:
          {
            lLineEnd = SHORT1FROMMP(mp1) - IDM_ENDDEFAULT + LINEEND_DEFAULT;
          }
          break;

          case  IDM_JOINDEFAULT:
          case  IDM_JOINFLAT:
          case  IDM_JOINSQUARE:
          case  IDM_JOINROUND:
          {
            lLineJoin = SHORT1FROMMP(mp1) - IDM_JOINDEFAULT + LINEJOIN_DEFAULT ;
          }
          break;

          case IDM_RIGHTANGLE:
          case IDM_TRIANGLE:
          {
           idShape = SHORT1FROMMP(mp1) ;
          }
          break;
        }
        break;
     }
     break;

     case WM_BUTTON1DOWN:
     {
       HPS hps;
       POINTL pointl;

       pointl.x = LOUSHORT(mp1);
       pointl.y = HIUSHORT(mp1);

       hps = WinGetPS( hwnd );

       DrawPolyLine( hps, &pointl, idShape );

       WinReleasePS( hps );
     }
     break;

     case WM_BUTTON2DOWN:
     {
       HPS hps;
       POINTL pointl;

       pointl.x = LOUSHORT(mp1);
       pointl.y = HIUSHORT(mp1);

       hps = WinGetPS( hwnd );

       DrawWideline( hps, &pointl );

       WinReleasePS( hps );

     }
     break;

     case WM_BUTTON1DBLCLK:
     {
       POINTL pointl;

       pointl.x = LOUSHORT(mp1);
       pointl.y = HIUSHORT(mp1);

       DrawWideline2( hwnd, &pointl );

     }
     break;


     case WM_PAINT:
     {
       HPS   hpsPaint ;
       RECTL rectl;

       hpsPaint = WinBeginPaint( hwnd, (HPS)0, &rectl );

       WinFillRect( hpsPaint, &rectl, SYSCLR_WINDOW );

       WinEndPaint( hpsPaint );
     }
     break;

     default:
     {
       return( WinDefWindowProc( hwnd, msg, mp1, mp2 )) ;
     }
   }
}

VOID DrawPolyLine( HPS hps, PPOINTL pptlStart, LONG idShape )
{
  POINTL aptl[2];

  if ( idShape == IDM_RIGHTANGLE )
  {
    aptl[0].x = pptlStart->x + 200;
    aptl[0].y = pptlStart->y ;
    aptl[1].x = aptl[0].x;
    aptl[1].y = pptlStart->y - 200;
  }
  else if ( idShape == IDM_TRIANGLE )
  {
    aptl[0].x = pptlStart->x + 100;
    aptl[0].y = pptlStart->y+100 ;
    aptl[1].x = aptl[0].x+100;
    aptl[1].y = pptlStart->y;
  }

  GpiSetCurrentPosition( hps, pptlStart );

  GpiSetColor( hps, CLR_BLUE );
  GpiPolyLine( hps, 2, aptl );

}

VOID DrawWideline( HPS hps, PPOINTL pptlStart )
{

  GpiSetBackMix( hps, BM_OVERPAINT );

  GpiSetCurrentPosition( hps, pptlStart );
  GpiSetColor( hps, CLR_BLACK );
  GpiSetBackColor( hps, CLR_BLUE );

  GpiSetPattern( hps,PATSYM_DIAG4 );

  GpiBeginPath( hps, 1 );

  DrawPolyLine( hps, pptlStart, idShape );

  GpiEndPath( hps);

  GpiSetLineWidthGeom( hps, 90L );
  GpiSetLineEnd( hps, lLineEnd );
  GpiSetLineJoin( hps, lLineJoin );
  GpiModifyPath( hps, 1, MPATH_STROKE );
  GpiFillPath( hps, 1, FPATH_WINDING );

}


// The following function is the one that appeared in the Developer
// Connection news.

VOID DrawWideline2( HWND hwnd, PPOINTL pptlStart )
{
  POINTL aptl[2];
  HPS    hps;

  hps = WinGetPS( hwnd );

  // Arbitrarily define a line that is 200 long and 200 down from the
  // passed in start position.

  aptl[0].x = pptlStart->x + 100;
  aptl[0].y = pptlStart->y+100 ;
  aptl[1].x = aptl[0].x+100;
  aptl[1].y = pptlStart->y;

  // Set the mix mode, colors and patterns we want to use to fill the
  // wideline when it gets drawn.

  GpiSetBackMix( hps, BM_OVERPAINT );
  GpiSetColor( hps, CLR_BLACK );
  GpiSetBackColor( hps, CLR_BLUE );
  GpiSetPattern( hps,PATSYM_DIAG4 );

  // Set the current position to the passed in start position

  GpiSetCurrentPosition( hps, pptlStart );

  // Start our path definition

  GpiBeginPath( hps, 1 );

  // Draw our simple poly line into the path

  GpiPolyLine( hps, 2, aptl );

  // End the path definition, note that nothing is drawn to the screen at
  // this time.

  GpiEndPath( hps);

  // Set the geometric line width to 100

  GpiSetLineWidthGeom( hps, 100L );

  // Set the line ending and line joining choices we require

  GpiSetLineEnd( hps, LINEEND_ROUND );
  GpiSetLineJoin( hps, LINEJOIN_MITRE );

  // Update the path with this new information

  GpiModifyPath( hps, 1, MPATH_STROKE );

  // Fill the wideline with the current pattern and colors, and display
  // the wideline we have created.

  GpiFillPath( hps, 1, FPATH_WINDING );

  WinReleasePS( hwnd );

}


