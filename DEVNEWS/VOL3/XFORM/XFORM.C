//---------------------------------------------------------------------------
//
//  Name     : xform.c
//
//  Purpose  : Demonstrate simple but powerful uses of the model transform.
//
//
//---------------------------------------------------------------------------

#define INCL_32
#define INCL_GPI
#define INCL_WIN
#include <os2.h>

#include "xform.h"

HAB     hab;
HWND    hwndFrame;
PFNWP   pfnwpFrame;

#define XFORM_CLASSNAME "XFORM"

// Local function prototype declarations

MRESULT EXPENTRY ClientWndProc( HWND hwnd ,USHORT msg ,MPARAM mp1 ,MPARAM mp2 );
MRESULT EXPENTRY FrameSubclassProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);

void DrawBox( HWND hwnd );
void DrawTransformedBox( HWND hwnd );
void DrawShearedBox( HWND hwnd );
void RotateBox( HWND hwnd );

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
void cdecl main(VOID)
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
                   , XFORM_CLASSNAME
                   , (PFNWP)ClientWndProc
                   , (ULONG)CS_SIZEREDRAW
                   , (USHORT)256 );

   hwndFrame = WinCreateStdWindow( HWND_DESKTOP
                                 ,  0UL
                                 ,  &flCreateFlags
                                 ,  XFORM_CLASSNAME
                                 ,  "Xform - Model transform examples"
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

     // When the left mouse button is clicked, run the sample functions.

     case WM_BUTTON1DOWN:
     {
       DrawBox( hwnd );
       DrawTransformedBox( hwnd );
       RotateBox( hwnd );
       DrawShearedBox( hwnd );
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

//--------------------------------------------------------------------------
//
//  DrawBox()
//
// --------------------------------------------------------------------------

void DrawBox( HWND hwnd )
{
  HPS hps;
  POINTL pointl;

  // Get a cached PS for the window

  hps = WinGetPS( hwnd );

  pointl.x = pointl.y = 0;

  // Set the current position to (0,0)

  GpiSetCurrentPosition( hps, &pointl );

  pointl.x = pointl.y = 10;

  // Draw a 10 by 10 box from the current position in the
  // current color.

  GpiBox( hps, DRO_OUTLINE, &pointl,0,0 );

  // Free the cached PS

  WinReleasePS( hps );

 }

//--------------------------------------------------------------------------
//
// DrawTransformedBox
//
// --------------------------------------------------------------------------
void DrawTransformedBox( HWND hwnd )
{
  HPS hps;
  POINTL pointl;
  MATRIXLF m;

  // Get a cached PS for the window

  hps = WinGetPS( hwnd );

  // Query the current contents of the model transform

  GpiQueryModelTransformMatrix( hps, 9L, &m );

  m.lM31  = 100;                 // Translate the x coordinates
  m.lM32  = 100;                 // Translate the y coordinates
  m.fxM11 = MAKEFIXED(10,0);     // Scale up the x coordinates
  m.fxM22 = MAKEFIXED(10,0);     // Scale up the y coordinates

  // Replace the model transform with our modified one

  GpiSetModelTransformMatrix( hps, 9L, &m, TRANSFORM_REPLACE );


  // Set the current position to (0,0)

  pointl.x = pointl.y = 0;

  GpiSetCurrentPosition( hps, &pointl );

  // Draw a 10 by 10 box from the current position in the
  // current color.

  pointl.x = pointl.y = 10;

  GpiBox( hps, DRO_OUTLINE, &pointl,0,0 );

  // Free the cached PS

  WinReleasePS( hps );
}

//--------------------------------------------------------------------------
//
// RotateBox
//
// --------------------------------------------------------------------------
void RotateBox( HWND hwnd )
{
  HPS hps;
  POINTL pointlBox, pointlStart;
  MATRIXLF m;
  LONG i;

  // Get a cached PS for the window

  hps = WinGetPS( hwnd );

  // This time let's draw the boxes in blue

  GpiSetColor( hps, CLR_BLUE );

  // For this simple example, we will choose an arbitary position
  // as the anchor point for each box of (400,400). This will be
  // the point about which each box is rotated. A nice
  // alternative, as a small enhancement, would be to make the
  // start position be wherever the mouse is clicked in the
  // window.

  pointlStart.x = 400;
  pointlStart.y = 400;

  // Query the current contents of the model transform

  GpiQueryModelTransformMatrix( hps, 9L, &m );

  // Setup our box coordinates to be 100 by 100 from wherever
  // the start position is.

  pointlBox.y = pointlStart.y + 100;
  pointlBox.x = pointlStart.x + 100;

  // Draw a series of boxes, each time around the loop we'll
  // rotate through an extra 10 degrees, replacing the transform
  // with our newly calculated one.

  for ( i=0; i<360; i+=10 )
  {
    GpiRotate( hps
             , &m
             , TRANSFORM_REPLACE
             , MAKEFIXED(i,0)
             , &pointlStart );

    GpiSetModelTransformMatrix( hps, 9L, &m, TRANSFORM_REPLACE );

    GpiSetCurrentPosition( hps, &pointlStart );

    // Draw a 100 by 100 box. Note that we issue a normal box
    // drawing request. Blissfully unaware that the transform
    // we setup will cause our box to be rotated.

    GpiBox( hps, DRO_OUTLINE, &pointlBox,0,0 );
  }

  // Free the cached PS

  WinReleasePS( hps );

}

//--------------------------------------------------------------------------
//
// DrawShearedBox
//
// --------------------------------------------------------------------------
void DrawShearedBox( HWND hwnd )
{
  HPS hps;
  POINTL pointl;
  MATRIXLF m;

  // Get a cached PS for the window

  hps = WinGetPS( hwnd );

  // Query the current contents of the model transform

  GpiQueryModelTransformMatrix( hps, 9L, &m );

  m.fxM21 = MAKEFIXED(1,0); // tan(45)
 // m.fxM22 = MAKEFIXED(1,0); // identity

  // Replace the model transform with our modified one

  GpiSetModelTransformMatrix( hps, 9L, &m, TRANSFORM_REPLACE );


  // Set the current position to (0,100)

  pointl.x = 400; pointl.y = 200;

  GpiSetCurrentPosition( hps, &pointl );

  // Draw a 100 by 100 box from the current position in the
  // current color.

  pointl.x = pointl.y = 100;

  GpiSetColor( hps, CLR_BLUE );
  GpiSetPattern( hps, PATSYM_HALFTONE );

  GpiBox( hps, DRO_OUTLINEFILL, &pointl,0,0 );

  // Free the cached PS

  WinReleasePS( hps );
}
