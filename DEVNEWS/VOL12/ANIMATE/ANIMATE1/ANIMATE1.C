//---------------------------------------------------------------------------
//
//  Name     : animate1.c
//
//  Purpose  : Show how a series of similar bitmaps can be used to create
//             the illusion of animation when combined with a timer. This
//             program loads a series of bitmaps and starts a timer. Each
//             time the timer "ticks" a new bitmap is drawn in the same
//             location as the previous one giving the impresson that
//             objects are moving.
//
//
//             GPI functions used:
//
//             GpiLoadBitmap
//             GpiSetBitmap
//             GpiBitBlt
//             GpiQueryBitmapInfoHeader
//             GpiDeleteBitmap
//             GpiCreatePS
//             GpiDestroyPS
//             DevOpenDC
//             DevCloseDC
//
//             Related functions used:
//
//             WinQueryWindowDC
//             WinOpenWindowDC
//             WinQueryAnchorBlock
//
//             Author: Kelvin R. Lawrence
//             29th-September-1996
//
//---------------------------------------------------------------------------

#define INCL_32
#define INCL_GPI
#define INCL_WIN
#include <os2.h>
#include <memory.h>

#include "animate1.h"

#define TITLEBARTEXT "animate1 - Using bitmaps and timers"
#define CLASSNAME    "animate1"

// Global handles

HAB   hab ;
HWND  hwndFrame ;
HWND  hwndMenu  ;
HPS   hpsMemory ;      // Memory presentation space
HDC   hdcMemory ;      // Memory device context
HDC   hdcScreen ;      // Screen device context for our window

// Global variables and arrays

#define NUM_BITMAPS  16

HBITMAP abmph[NUM_BITMAPS] ;
LONG    idBitmap = 0 ;


// Local function prototype declarations

MRESULT EXPENTRY ClientWndProc( HWND hwnd ,USHORT msg ,MPARAM mp1 ,MPARAM mp2 );
VOID LoadBitmaps( HWND hwnd );
VOID DisplayNextBitmap( HWND hwnd, LONG idBitmap );

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

   hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );

   WinShowWindow( hwndFrame, TRUE );

   WinStartTimer( hab, hwndClient, ID_TIMER, 10 );

   LoadBitmaps( hwndClient );

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

   if ( hpsMemory )
   {
     LONG i;

     GpiSetBitmap( hpsMemory, 0 );

     for ( i=0; i < NUM_BITMAPS; i++ )
     {
       GpiDeleteBitmap( abmph[i] );
     }

     GpiDestroyPS( hpsMemory );
   }

   if ( hdcMemory )
   {
     DevCloseDC( hdcMemory );
   }

   if ( hwndFrame )
   {
     WinDestroyWindow(hwndFrame);
     WinDestroyMsgQueue(hmq);
     WinTerminate(hab);
   }
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
       DisplayNextBitmap( hwnd, idBitmap );

       idBitmap++ ;

       if ( idBitmap >= NUM_BITMAPS )
       {
         idBitmap = 0 ;
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
// LoadBitmaps()
//
// Load a series of bitmaps from our resource table.
//
//
// --------------------------------------------------------------------------
VOID LoadBitmaps( HWND hwnd )
{
  SIZEL        sizl  ;     // Size of PS when created (zero = default)
  HAB          hab   ;     // Application anchor bock
  LONG         i     ;     // Loop counter
  DEVOPENSTRUC dev   ;     // DevOpenDC parameters

  //----------------------------------------------------------
  // Query the anchor block for our window and get a DC for it
  //----------------------------------------------------------

  hab = WinQueryAnchorBlock( hwnd );

  if ( !(hdcScreen = WinQueryWindowDC( hwnd )) )
  {
    hdcScreen = WinOpenWindowDC( hwnd ) ;
  }

  //---------------------------------------------------------
  // Create a memory DC that is compatible with the window DC
  //---------------------------------------------------------

  memset( &dev, 0, sizeof(DEVOPENSTRUC) );
  dev.pszDriverName = "DISPLAY" ;

  hdcMemory = DevOpenDC( hab
                       , OD_MEMORY
                       , "*"
                       , 2L
                       , (PDEVOPENDATA)&dev
                       , hdcScreen );

  //---------------------------------------------
  // Create micro PS and associate with memory dc
  //---------------------------------------------

  sizl.cx = sizl.cy = 0;

  hpsMemory = GpiCreatePS( hab
                         , hdcMemory
                         , &sizl
                         , PU_PELS | GPIA_ASSOC | GPIT_MICRO );

  //----------------------------------------------------------
  // Load the bitmaps we intend to use from our resource file.
  //----------------------------------------------------------

  for ( i = ID_BITMAP01; i <= ID_BITMAP16; i++ )
  {
    abmph[i-ID_BITMAP01] = GpiLoadBitmap
                             ( hpsMemory   // PS for load
                             , (HMODULE) 0 // Module containing bitmap (0=.EXE)
                             , i           // Id of the bitmap to load
                             , 0           // Load at original width
                             , 0           // Load at original heaight
                             );

  }

}


//--------------------------------------------------------------------------
//
// DiaplayNextBitmap()
//
// Display the next bitmap in the series.
//
//
// --------------------------------------------------------------------------
VOID DisplayNextBitmap( HWND hwnd, LONG idBitmap )
{
  POINTL aptl[5] ;         // Points used to perform blt
  BITMAPINFOHEADER2 bmp2 ; // Describes our bitmap
  LONG i,j,xDelta,yDelta ; // Loop counters and increments
  LONG lWidth,lHeight ;    // Width and height of loaded bitmap
  HPS  hpsWindow ;

  //-------------------------------------------------------------
  // Query information about the bitmap so that we can obtain its
  // width and height.
  //-------------------------------------------------------------

  memset( &bmp2, 0, sizeof( BITMAPINFOHEADER2 ) );

  bmp2.cbFix = sizeof( BITMAPINFOHEADER2 ) ;

  GpiQueryBitmapInfoHeader( abmph[idBitmap], &bmp2 );

  lWidth  = bmp2.cx ;
  lHeight = bmp2.cy ;

  //-----------------------------
  // Select bitmap into memory PS
  //-----------------------------

  GpiSetBitmap( hpsMemory, abmph[idBitmap] );

  //----------------------------------------------------------------
  // Set up the initial coordinates for our blt. We have to provide
  // both coordinates of the target rectangle and the source as we
  // are going to double the size of the source bitmap when we do
  // the BLT.
  //----------------------------------------------------------------

  aptl[0].x =  0 ;       // Lower left x of target bitmap (the screen)
  aptl[0].y =  0 ;       // Lower left y of target bitmap (the screen)
  aptl[1].x = lWidth*2 ; // Upper right x of target bitmap (the screen)
  aptl[1].y = lHeight*2; // Upper right y of target bitmap (the screen)
  aptl[2].x =  0 ;       // Lower left of x  source (the memory bitmap)
  aptl[2].y =  0 ;       // Lower left of y  source (the memory bitmap)
  aptl[3].x = lWidth ;   // Upper right x of source
  aptl[3].y = lHeight ;  // Upper right y of source

  //---------------------------------
  // Now blt the bitmap to the screen
  //---------------------------------

  hpsWindow = WinGetPS( hwnd );

  GpiBitBlt( hpsWindow    // Target PS
           , hpsMemory    // Source PS
           , 4            // Number of points in point array
           , aptl         // Source and target coordinates for the BLT
           , ROP_SRCCOPY  // Mix mode for BLT
           , 0            // Options bits
           );

  WinReleasePS( hpsWindow );


}
