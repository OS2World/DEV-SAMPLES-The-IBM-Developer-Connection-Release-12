// Demonstrate use of partial arcs to draw pie slices and pie charts.
//

#define INCL_32
#define INCL_GPI
#define INCL_WIN
#include <os2.h>

MRESULT EXPENTRY ClientWndProc( HWND hwnd ,USHORT msg ,MPARAM mp1 ,MPARAM mp2 );
VOID DrawSlice( HPS hps, PPOINTL pptlCenter );
VOID DrawPie( HPS hps, LONG lSlices, PPOINTL pptlCenter );

HAB     hab;
HWND    hwndFrame;

#define PIE_CLASSNAME "PIE"

int cdecl main(VOID)
{

   HMQ hmq;
   HWND hwndclient;
   QMSG qmsg;

   ULONG flCreateFlags =   FCF_BORDER     | FCF_SHELLPOSITION |
                           FCF_TASKLIST   | FCF_TITLEBAR      | FCF_SYSMENU |
                           FCF_SIZEBORDER | FCF_MINMAX ;

   hab = WinInitialize( (USHORT)NULL );

   hmq=WinCreateMsgQueue( hab,0 );


   WinRegisterClass( hab
                   , PIE_CLASSNAME
                   , (PFNWP)ClientWndProc
                   , (ULONG)CS_SIZEREDRAW
                   , (USHORT)256 );

   hwndFrame = WinCreateStdWindow( HWND_DESKTOP
                                 ,  0UL
                                 ,  &flCreateFlags
                                 ,  PIE_CLASSNAME
                                 ,  "Pie - Partial arc example"
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
MRESULT EXPENTRY ClientWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
     case WM_BUTTON1DOWN:
     {
       HPS hps;
       POINTL pointl;

       pointl.x = LOUSHORT(mp1);
       pointl.y = HIUSHORT(mp1);

       hps = WinGetPS( hwnd );

       DrawSlice( hps, &pointl );

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

       DrawPie( hps, 12L, &pointl );

       WinReleasePS( hps );

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

// Draw a whole pie chart

VOID DrawPie( HPS hps, LONG lSlices, PPOINTL pptlCenter )
{
  // Loop counter and size of each pie slice

  LONG i;
  LONG lSegSize = 360/lSlices ;

  // Set the linetype and mix mode we want to use

  GpiSetLineType( hps, LINETYPE_SOLID );
  GpiSetBackMix( hps, BM_OVERPAINT );

  // Loop around drawing each slice one at a time

  for ( i=0; i<lSlices; i++ )
  {
    // Set the current position to be the center point of the pie

    GpiSetCurrentPosition( hps, pptlCenter );

    // Set our colors. Choose a different background color for each
    // slice.

    GpiSetColor( hps, CLR_BLACK );
    GpiSetBackColor( hps, CLR_BLUE+i );

    // Choose a different pattern for each slice

    GpiSetPattern( hps,PATSYM_DENSE1+i );

    // Start the area definition for this slice

    GpiBeginArea( hps, BA_BOUNDARY );

    // Draw a partial arc with the start angle and sweep angle
    // calculated above.

    GpiPartialArc( hps
                 , pptlCenter
                 , MAKEFIXED(150,0) 
                 , MAKEFIXED(lSegSize*i,0)
                 , MAKEFIXED(lSegSize, 0)
               );

    // End our area definition and draw the area.

    GpiEndArea( hps );
  }
}

// Simple procedure to draw a 90 degree pie slice 

VOID DrawSlice( HPS hps, PPOINTL pptlCenter )
{

  // Set the linetype and mix mode we want to use

  GpiSetLineType( hps, LINETYPE_SOLID );
  GpiSetBackMix( hps, BM_OVERPAINT );

  // Set the foreground and background colors

  GpiSetColor( hps, CLR_BLACK );
  GpiSetBackColor( hps, CLR_BLUE );
  
  // Choose a pattern to use for filling the area

  GpiSetPattern( hps,PATSYM_DIAG1  );

  //Set the current position to our center point

  GpiSetCurrentPosition( hps, pptlCenter );

  // Start defining our area. The BA_BOUNDARY flag says we want the
  // area boundary to be drawn.

  GpiBeginArea( hps, BA_BOUNDARY );

  // Draw a partial arc with a 90 degree start angle , a 90 degree
  // angle of sweep and a size multiplier of 150.

  GpiPartialArc( hps
               , pptlCenter
               , MAKEFIXED(150,0) 
               , MAKEFIXED(90,0)
               , MAKEFIXED(90,0)
             );

  // End our area definition and draw the area.

  GpiEndArea( hps );
}
