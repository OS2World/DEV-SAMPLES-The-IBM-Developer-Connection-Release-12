// Simple clipping example using paths
//

#define  INCL_DOSPROCESS
#define  INCL_32
#define  INCL_GPI
#define  INCL_WIN
#include <os2.h>

#include <string.h>
#include <stdlib.h>
#include <memory.h>

HAB     hab;
HWND    hwndFrame;

MRESULT EXPENTRY ClientWndProc( HWND hwnd ,ULONG msg ,MPARAM mp1 ,MPARAM mp2 );
VOID DrawClippedLines( HWND hwnd );

#define CLPPTH_CLASSNAME "CLIPPATH"

int cdecl main(VOID)
{

   HMQ  hmq;
   HWND hwndclient;
   QMSG qmsg;

   ULONG flCreateFlags =   FCF_BORDER     | FCF_SHELLPOSITION |
                           FCF_TASKLIST   | FCF_TITLEBAR      | FCF_SYSMENU |
                           FCF_SIZEBORDER | FCF_MINMAX ;

   hab = WinInitialize( (USHORT)NULL );

   hmq=WinCreateMsgQueue( hab,0 );


   WinRegisterClass( hab
                   , CLPPTH_CLASSNAME
                   , (PFNWP)ClientWndProc
                   , (ULONG)CS_SIZEREDRAW
                   , (USHORT)256 );

   hwndFrame = WinCreateStdWindow( HWND_DESKTOP
                                 ,  0UL
                                 ,  &flCreateFlags
                                 ,  CLPPTH_CLASSNAME
                                 ,  "Clip Path"
                                 ,  WS_VISIBLE
                                 ,  (HMODULE)0
                                 ,  0
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
// ------------------------------------------------------------------------
MRESULT EXPENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
     case WM_BUTTON1DOWN:
     {
       DrawClippedLines( hwnd );
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

VOID DrawClippedLines( HWND hwnd )
{
  HPS hps;
  RECTL rectl;
  LONG  i;
  POINTL pt1, pt2;
  POINTL aptlClip[4];

  hps = WinGetPS( hwnd );

  // Query our client window rectangle

  WinQueryWindowRect( hwnd, &rectl );

  // Define a simple triangle to be the area we clip to.

  aptlClip[0].x = 0;              aptlClip[0].y = 0;
  aptlClip[1].x = rectl.xRight;   aptlClip[1].y = 0;
  aptlClip[2].x = rectl.xRight/2; aptlClip[2].y = rectl.yTop;
  aptlClip[3].x = 0;              aptlClip[3].y = 0;


  // Create the path containing our triangle outline as a poly line

  GpiSetCurrentPosition( hps, aptlClip );

  GpiBeginPath( hps, 1 );

  GpiPolyLine( hps, 4, aptlClip );

  GpiEndPath( hps );

  // Establish our path as the clip path. Note the path is destroyed at
  // this point. Ie we can no longer use it.

  GpiSetClipPath( hps, 1, SCP_ALTERNATE | SCP_AND );

  // Draw some randomly colored lines all the way up the client window.
  // Note these lines will be clipped to our clip path.

  pt2.x = rectl.xRight;
  pt2.y = 0;
  pt1.x = 0;
  pt1.y = 0;

  for ( i=0; i < rectl.yTop; i++, pt1.y++, pt2.y++ )
  {
    GpiSetColor( hps, rand() % CLR_PALEGRAY );

    GpiSetCurrentPosition( hps, &pt1 );
    GpiLine( hps, &pt2 );
  }

  WinReleasePS( hps );
}

