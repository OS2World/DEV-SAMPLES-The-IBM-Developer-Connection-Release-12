//---------------------------------------------------------------------------
//
//  Name     : animate2.c
//
//  Purpose  : Show how a series of animation frames contained in a single
//             (wide and narrow) bitmap , can be used to create the effect
//             of animation. This program loads the bitmap and walks through
//             it diplaying each frame with a time delay between each one.
//
//             GPI functions used:
//
//             GpiLoadBitmap
//             GpiWCBitBlt
//
//             Author: Kelvin R. Lawrence
//             29th-September
//
//---------------------------------------------------------------------------

#define INCL_32
#define INCL_GPI
#define INCL_WIN
#include <os2.h>
#include <memory.h>

#include "animate2.h"

#define TITLEBARTEXT "animate2 - Using bitmaps and timers"
#define CLASSNAME    "animate2"

//---------------
// Global handles
//---------------

HAB   hab ;
HWND  hwndFrame ;

//----------------------------
// Global variables and arrays
//----------------------------

#define NUM_FRAMES   16
#define FRAME_WIDTH  50
#define FRAME_HEIGHT 50

HBITMAP hbmpFrames ;
LONG    lCurrentFrame = 1 ;

//--------------------------------------
// Local function prototype declarations
//--------------------------------------

MRESULT EXPENTRY ClientWndProc( HWND hwnd ,USHORT msg ,MPARAM mp1 ,MPARAM mp2 );
HBITMAP LoadBitmap( HWND hwnd, LONG idBitmap );
VOID DisplayNextFrame( HWND hwnd, LONG lFrameNumber, HBITMAP hbmpFrames  );

//--------------------------------------------------------------------------
//
//  main() procedure.
//   --> Initialize PM for this process
//   --> Create our message queue
//   --> Create frame and client windows
//   --> Show the window
//   --> Enter our message dispatching loop
//
//
// --------------------------------------------------------------------------
int cdecl main(VOID)
{
   HMQ hmq;
   HWND hwndClient;
   QMSG qmsg;

   ULONG flCreateFlags =   FCF_BORDER     | FCF_SHELLPOSITION |
                           FCF_TASKLIST   | FCF_TITLEBAR      | FCF_SYSMENU |
                           FCF_SIZEBORDER | FCF_MINMAX        | FCF_MENU ;

   hab = WinInitialize( (USHORT)NULL );

   hmq=WinCreateMsgQueue( hab,0 );

   WinRegisterClass( hab
                   , CLASSNAME
                   , (PFNWP)ClientWndProc
                   , (ULONG)CS_SIZEREDRAW
                   , (USHORT)256 );

   hwndFrame = WinCreateStdWindow( HWND_DESKTOP
                                 ,  0UL
                                 ,  &flCreateFlags
                                 ,  CLASSNAME
                                 ,  TITLEBARTEXT
                                 ,  WS_VISIBLE
                                 ,  (HMODULE)0
                                 ,  ID_PROG
                                 ,  &hwndClient );


   WinShowWindow( hwndFrame, TRUE );

   WinStartTimer( hab, hwndClient, ID_TIMER, 10 );

   hbmpFrames = LoadBitmap( hwndClient, ID_FRAMES );

   //-------------------
   // Start message loop
   //-------------------

   while ( WinGetMsg( hab,&qmsg, (HWND)0, 0, 0 ) )
   {
     WinDispatchMsg( hab,&qmsg );
   }

   //----------------------------------------
   // Clean up resources before we terminate.
   //----------------------------------------

   WinDestroyWindow(hwndFrame);
   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);

   return 0;
}

//--------------------------------------------------------------------------
//
//  Client window procedure.
//
// --------------------------------------------------------------------------
MRESULT EXPENTRY ClientWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{

   switch (msg)
   {

     //---------------------------------------------------------------------
     //
     // WM_ACTIVATE
     //
     // Force a complete repaint when we get the focus
     //
     //---------------------------------------------------------------------
     case WM_ACTIVATE:
     {
       if ( SHORT1FROMMP( mp1 ) )
       {
         WinInvalidateRect( hwnd, 0L, FALSE );
       }
     }
     break ;


     //---------------------------------------------------------------------
     //
     // WM_COMMAND
     //
     // Process the users menu choices
     //
     //---------------------------------------------------------------------
     case WM_COMMAND:
     {
       switch( SHORT1FROMMP( mp1 ) )
       {
         case IDM_EXIT:
         {
           WinPostMsg( hwnd, WM_CLOSE, (MPARAM)0L, (MPARAM)0L );
         }
         break;
       }
     }
     break;

     //---------------------------------------------------------------------
     //
     // WM_PAINT
     //
     // Process paint requests
     //
     //---------------------------------------------------------------------
     case WM_PAINT:
     {
       HPS   hpsPaint ;
       RECTL rectl;

       hpsPaint = WinBeginPaint( hwnd, (HPS)0, &rectl );

       WinQueryWindowRect( hwnd, &rectl );

       WinFillRect( hpsPaint, &rectl, CLR_BLACK );

       WinEndPaint( hpsPaint );
     }
     break;

     //---------------------------------------------------------------------
     //
     // WM_TIMER
     //
     // Display the next bitmap in the sequence
     //
     //---------------------------------------------------------------------
     case WM_TIMER:
     {
       DisplayNextFrame( hwnd, lCurrentFrame, hbmpFrames );

       lCurrentFrame++ ;

       if ( lCurrentFrame >= NUM_FRAMES )
       {
         lCurrentFrame = 0 ;
       }
     }
     break ;

     //---------------------------------------------------------------------
     //
     // default case
     //
     // Let PM handle messages that we are not interested in
     //
     //---------------------------------------------------------------------
     default:
     {
       return( WinDefWindowProc( hwnd, msg, mp1, mp2 )) ;
     }
   }
}

//--------------------------------------------------------------------------
//
// LoadBitmap()
//
// Load the bitmap containing the animation frames.
//
//
// --------------------------------------------------------------------------
HBITMAP LoadBitmap( HWND hwnd, LONG idBitmap )
{
 HBITMAP hbmp ;

 HPS hps = WinGetPS( hwnd ) ;

 hbmp = GpiLoadBitmap( hps         // PS for load
                     , (HMODULE) 0 // Module containing bitmap (0=.EXE)
                     , idBitmap    // Id of the bitmap to load
                     , 0           // Load at original width
                     , 0           // Load at original heaight
                     );

 WinReleasePS( hps ) ;

 return( hbmp ) ;
}

//--------------------------------------------------------------------------
//
// DisplayNextFrame()
//
// Display the next frame in the series.
//
//
// --------------------------------------------------------------------------
VOID DisplayNextFrame( HWND hwnd, LONG lFrameNumber, HBITMAP hbmpFrames  )
{
  POINTL aptl[4]   ;         // Points used to perform blt
  LONG   x1,y1,x2,y2 ;
  HPS    hps ;

  x1 = lFrameNumber * FRAME_WIDTH  ;
  y1 = 0 ;

  x2 = x1 + FRAME_WIDTH  -1 ;
  y2 = y1 + FRAME_HEIGHT -1 ;

  //----------------------------------------------------------------
  // Set up the initial coordinates for our blt. We have to provide
  // both coordinates of the target rectangle and the source as we
  // are going to double the size of the source bitmap when we do
  // the BLT.
  //----------------------------------------------------------------

  aptl[0].x = 0  ;            // Lower left x of target bitmap (the screen)
  aptl[0].y = 0  ;            // Lower left y of target bitmap (the screen)
  aptl[1].x = 2 * FRAME_WIDTH-1 ; // Upper right x of target bitmap (the screen)
  aptl[1].y = 2 * FRAME_HEIGHT-1; // Upper right y of target bitmap (the screen)
  aptl[2].x = x1 ;            // Lower left of x  source (the memory bitmap)
  aptl[2].y = 0  ;            // Lower left of y  source (the memory bitmap)
  aptl[3].x = x2 ;          // Upper right x of source
  aptl[3].y = y2 ;          // Upper right y of source

  //---------------------------------
  // Now blt the bitmap to the screen
  //---------------------------------

  hps = WinGetPS( hwnd );

  GpiWCBitBlt( hps          // Target PS
             , hbmpFrames   // Source PS
             , 4            // Number of points in point array
             , aptl         // Source and target coordinates for the BLT
             , ROP_SRCCOPY  // Mix mode for BLT
             , 0            // Options bits
             );

  WinReleasePS( hps );


}
