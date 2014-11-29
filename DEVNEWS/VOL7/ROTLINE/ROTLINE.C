//---------------------------------------------------------------------------
//
//  Name     : rotline.c
//
//  Author   : Kelvin R. Lawrence
//
//  Purpose  : Demonstrate simple use of two threads in a drawing application
//
//  Last update : 1-Nov-1994
//
//---------------------------------------------------------------------------

#define INCL_DOSPROCESS
#define INCL_GPI
#define INCL_WIN
#define INCL_32

#include <os2.h>
#include "rotline.h"
#include <stdlib.h>
#include <string.h#include <stdio.h >

// Define our user messages that we can post to our second thread.

#define UM_AUTO   WM_USER
#define UM_EXIT   WM_USER+1
#define UM_STOP   WM_USER+2
#define UM_PAINT  WM_USER+3

#define MAX_LOOP          999999L
#define REPAINT_THRESHOLD 1000L

// Key global variables

HAB    hab;        // Anchor block for the main thread
HAB    hab2;       // Anchor block for the second thread
HMQ    hmq;        // Message queue handle for the main thread
HMQ    hmq2;       // Message queue handle for the second thread
TID    tid;        // Thread id of the second thread
LONG   lShape;     // Type of shape to draw

#define ROTLINE_CLASSNAME "ROTLINE"

MRESULT EXPENTRY ClientWndProc( HWND hwnd ,USHORT msg ,MPARAM mp1 ,MPARAM mp2 );
VOID DrawLines( HWND hwnd, PPOINTL ppointlStart, PRECTL prectl );
VOID DrawBoxes( HWND hwnd, PPOINTL ppointlStart, PRECTL prectl );
VOID DrawShape( HWND hwnd );
VOID ThreadProc( LONG arg );
VOID DoPaint( HWND hwnd );
VOID DoCube( HWND hwnd, PRECTL prectl );
VOID DoPolygons( HPS hps );
VOID DoMultipleCubes( HWND hwnd, PRECTL prectl );
VOID DoTextString( HWND hwnd );

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

void main(VOID)
{
   HWND hwndFrame;    // Frame window handle
   HWND hwndclient;    // Client window handle
   QMSG qmsg;


   ULONG flCreateFlags =   FCF_BORDER     | FCF_SHELLPOSITION |
                           FCF_TASKLIST   | FCF_TITLEBAR      | FCF_SYSMENU |
                           FCF_SIZEBORDER | FCF_MINMAX        | FCF_MENU ;


   // Create our worker thread. Once created the thread will sit waiting
   // for messages to be posted to it. Create the second thread before
   // we create our windows so that it is ready to start servicing paint
   // requests when the windows are created.
   
   DosCreateThread( &tid
                  , (PFNTHREAD)ThreadProc
                  , 0L
                  , 0L
                  , 8192L
                  );

   // Give the thread a timeslice before we proceed so it can get
   // established. This is dirty, normally better to use a semaphore
   // or similar for this.

   while( !hmq2 )
   {
     // Wait for the second thread to initialize
     DosSleep( 0 );
   };

   // Initialize the primary thread

   hab = WinInitialize( (USHORT)NULL );

   hmq=WinCreateMsgQueue( hab,0 );

   WinRegisterClass( hab
                   , ROTLINE_CLASSNAME
                   , (PFNWP)ClientWndProc
                   , (ULONG)CS_SIZEREDRAW
                   , (USHORT)256 );

   hwndFrame = WinCreateStdWindow( HWND_DESKTOP
                                 ,  0UL
                                 ,  &flCreateFlags
                                 ,  ROTLINE_CLASSNAME
                                 ,  "Rotline - Line rotation"
                                 ,  WS_VISIBLE
                                 ,  (HMODULE)0
                                 ,  ID_ROTLINE
                                 ,  &hwndclient );


   WinShowWindow( hwndFrame, TRUE );


   // Start processing messages on our main thread and dispatching them
   // to our window procedure.

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

   static HWND hwndMenu;

   switch (msg)
   {
     case WM_COMMAND:
     {
       switch( SHORT1FROMMP( mp1 ) )
       {
         case IDM_EXIT:
         {
           WinPostMsg( hwnd, WM_CLOSE, (MPARAM)0L, (MPARAM)0L );
         }
         break;

         case IDM_AUTOLINES:
         {
           WinEnableMenuItem( hwndMenu, IDM_AUTOLINES, FALSE );
           WinPostQueueMsg( hmq2, UM_AUTO, (MPARAM)hwnd, (MPARAM)0L );
           WinEnableMenuItem( hwndMenu, IDM_STOPDRAW, TRUE );
         }
         break;

         case IDM_STOPDRAW:
         {
           WinEnableMenuItem( hwndMenu, IDM_AUTOLINES, TRUE );
           WinPostQueueMsg( hmq2, UM_STOP, (MPARAM)0L, (MPARAM)0L );
           WinEnableMenuItem( hwndMenu, IDM_STOPDRAW, FALSE );
         }
         break;

         case IDM_REFRESH:
         {
           WinInvalidateRect( hwnd, NULL, FALSE );
         }
         break;

         case IDM_SPIRO:
         case IDM_EXPCUBE:
         case IDM_ROTCUBE:
         case IDM_CUBES:
         case IDM_TEXT:
         {
           WinCheckMenuItem( hwndMenu, lShape, FALSE );
           lShape = SHORT1FROMMP( mp1 );
           WinCheckMenuItem( hwndMenu, lShape, TRUE );
           WinInvalidateRect( hwnd, NULL, FALSE );
         }
         break;

       }
     }
     break;

     case WM_PAINT:
     {
       HPS   hpsPaint ;
       RECTL rectl;

       hpsPaint = WinBeginPaint( hwnd, (HPS)0, &rectl );

       WinPostQueueMsg( hmq2, UM_PAINT, (MPARAM)hwnd, (MPARAM)0L );

       WinEndPaint( hpsPaint );
     }
     break;

     case WM_CREATE:
     {
       lShape = IDM_SPIRO;
       
       hwndMenu = WinWindowFromID( WinQueryWindow( hwnd, QW_PARENT )
                                 , FID_MENU );

       WinCheckMenuItem( hwndMenu, lShape, TRUE );
       return( WinDefWindowProc( hwnd, msg, mp1, mp2 ));
     }
     break;

     case WM_CLOSE:
     {
       WinPostQueueMsg( hmq2, UM_EXIT, (MPARAM)0L, (MPARAM)0L );
       DosWaitThread( &tid, DCWW_WAIT );
       WinPostMsg( hwnd, WM_QUIT, (MPARAM)0L, (MPARAM)0L );
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
// DrawShape()
//
// --------------------------------------------------------------------------
VOID DrawShape( HWND hwnd )
{
 POINTL pointlStart;
 LONG   lRandom;
 RECTL  rectl;

 WinQueryWindowRect( hwnd , &rectl );

 switch( lShape )
 {
   case IDM_SPIRO:
   {
     pointlStart.x = rand() % rectl.xRight ;
     pointlStart.y = rand() % rectl.yTop;

     lRandom = rand() %2;

     if ( lRandom == 1 )
     {
       DrawLines( hwnd, &pointlStart, &rectl );
     }
     else
     {
       DrawBoxes( hwnd, &pointlStart, &rectl );
     }
   }
   break;

   case IDM_EXPCUBE:
   case IDM_ROTCUBE:
   {
     DoCube( hwnd, &rectl );
   }
   break;

   case IDM_CUBES:
   {
    DoMultipleCubes( hwnd, &rectl );
   }
   break;

   case IDM_TEXT:
   {
     DoTextString( hwnd );
   }
   break;
 }
}

//--------------------------------------------------------------------------
//
//  DrawLines()
//
// --------------------------------------------------------------------------
VOID DrawLines( HWND hwnd, PPOINTL ppointlStart, PRECTL prectl )
{
  HPS hps;
  POINTL pointlLine;
  MATRIXLF m;
  LONG i, xlen, ylen;

  hps = WinGetPS( hwnd );

  GpiSetColor( hps, rand() % CLR_PALEGRAY );

  GpiQueryModelTransformMatrix( hps, 9L, &m );

  xlen =   ( rand() % prectl->xRight ) /10;
  ylen =   ( rand() % prectl->yTop ) /10;

  for ( i=0; i<360; i+=10 )
  {
    GpiRotate( hps,  &m, TRANSFORM_REPLACE, MAKEFIXED(i,0), ppointlStart );
                 
    GpiSetModelTransformMatrix( hps, 9L, &m, TRANSFORM_REPLACE );

    GpiSetCurrentPosition( hps,  ppointlStart);

    pointlLine.y = ppointlStart->y + ylen ;
    pointlLine.x = ppointlStart->x + xlen;

    GpiLine( hps, &pointlLine );
  }

  WinReleasePS( hps );
}
//--------------------------------------------------------------------------
//
// DrawBoxes()
//
// --------------------------------------------------------------------------
VOID DrawBoxes( HWND hwnd, PPOINTL ppointlStart, PRECTL prectl)
{
  HPS hps;
  POINTL pointlBox;
  MATRIXLF m;
  LONG i, xlen, ylen;

  // Get a cached PS for the window

  hps = WinGetPS( hwnd );

  GpiSetColor( hps, rand() % CLR_PALEGRAY );

  // Query the current contents of the model transform

  GpiQueryModelTransformMatrix( hps, 9L, &m );

  // Setup our box coordinates to be 100 by 100 from wherever
  // the start position is.

  xlen =   ( rand() % prectl->xRight ) /10;
  ylen =   ( rand() % prectl->yTop )   /10;

  pointlBox.y = ppointlStart->y + ylen;
  pointlBox.x = ppointlStart->x + xlen;

  // Draw a series of boxes, each time around the loop we'll
  // rotate through an extra 10 degrees, replacing the transform
  // with our newly calculated one.

  for ( i=0; i<360; i+=10 )
  {
    GpiRotate( hps
             , &m
             , TRANSFORM_REPLACE
             , MAKEFIXED(i,0)
             , ppointlStart );

    GpiSetModelTransformMatrix( hps, 9L, &m, TRANSFORM_REPLACE );

    GpiSetCurrentPosition( hps, ppointlStart );

    GpiBox( hps, DRO_OUTLINE, &pointlBox,0,0 );
  }

  // Free the cached PS

  WinReleasePS( hps );

}

//--------------------------------------------------------------------------
//
//  ThreadProc()
//
// --------------------------------------------------------------------------
VOID ThreadProc( LONG arg )
{
   LONG  i;
   QMSG  qmsg2;
   HWND  hwnd2;

   hab2 = WinInitialize( 0UL );
   hmq2 = WinCreateMsgQueue( hab2, 0UL );

   WinCancelShutdown( hmq2, TRUE );

   qmsg2.msg = 0;

   while( qmsg2.msg != UM_EXIT )
   {
     switch( qmsg2.msg )
     {
       case UM_PAINT:
       {
         hwnd2 = (HWND)qmsg2.mp1;

         DoPaint( hwnd2 );
        
       }
       break;

       case UM_AUTO:
       {
         hwnd2 = (HWND)qmsg2.mp1;
  
         WinSetWindowText( WinQueryWindow( hwnd2, QW_PARENT), "Thread busy" );

         for( i=1; i < MAX_LOOP; i++ )
         {
           if ( i % REPAINT_THRESHOLD == 0 )
           {
             WinInvalidateRect( hwnd2, NULL, FALSE );
           }

           DrawShape( hwnd2 );

           qmsg2.msg = 0;

           WinPeekMsg( hab2, &qmsg2, 0L, 0L, 0L, PM_NOREMOVE );

           if ( qmsg2.msg == UM_STOP || qmsg2.msg == UM_EXIT )
           {
             i = MAX_LOOP ;
           }
           else if ( qmsg2.msg == UM_PAINT )
           {
             WinPeekMsg( hab2, &qmsg2, 0L, 0L, 0L, PM_REMOVE );
             DoPaint( (HWND)qmsg2.mp1 );
           }
         }

         if ( qmsg2.msg == UM_STOP )
         {
           WinSetWindowText( WinQueryWindow( hwnd2, QW_PARENT), "Thread waiting" );
         }
       }
       break;
     }                                    
     WinGetMsg( hab2, &qmsg2, 0L, 0L, 0L );
   }
                                                        
   if ( hmq2 )
   {
     WinDestroyMsgQueue( hmq2 );
   }

   if ( hab2 )
   {
     WinTerminate( hab2 );
   }

   DosExit( EXIT_THREAD, 0L );
}

//--------------------------------------------------------------------------
//
//  DoPaint()
//
// --------------------------------------------------------------------------
VOID DoPaint( HWND hwnd )
{
  RECTL rectl;
  HPS   hps;

  WinQueryWindowRect( hwnd, &rectl );

  hps = WinGetPS( hwnd );

  WinFillRect( hps, &rectl, CLR_BLACK );

  WinReleasePS( hps );
}

//--------------------------------------------------------------------------
//
//  DoCube()
//
// --------------------------------------------------------------------------
VOID DoCube( HWND hwnd, PRECTL prectl )
{
  HPS hps;
  LONG i,j;
  MATRIXLF m;
  FIXED fxScale[2];
  POINTL ptlTrans;

  POINTL cube[24] = { 0,0,0,20,0,20,20,20,20,20,20,0,20,0,0,0,0,0,10,10,
                     10,10,30,10,30,10,20,0,0,20,10,30,10,30,30,30,30,30,
                     30,10,20,20,30,30,10,10,10,30 };

  hps = WinGetPS( hwnd );

  GpiSetMix( hps, FM_XOR );

  GpiQueryModelTransformMatrix( hps, 9L, &m );

  GpiSetColor( hps, CLR_CYAN );

  if ( lShape == IDM_EXPCUBE  )
  {
    for ( i=0; i<40; i++ )
    {
      j = ( i <20 ) ? i : 40-i ;

      fxScale[0] = fxScale[1] = MAKEFIXED(j,0);
      GpiScale( hps, &m, TRANSFORM_REPLACE, fxScale, cube );
  
      ptlTrans.x = ptlTrans.y = 10;
      GpiTranslate( hps, &m, TRANSFORM_ADD, &ptlTrans ); 

      GpiSetModelTransformMatrix( hps, 9L, &m, TRANSFORM_REPLACE );

      if ( lShape == IDM_EXPCUBE )
      {
        GpiPolyLineDisjoint( hps, 24L, cube );
        DosSleep( 50 );
        GpiPolyLineDisjoint( hps, 24L, cube );
      }
    }
  }
  else  //IDM_ROTCUBE
  {
    for ( i=0; i<360; i+=10 )
    {
      GpiRotate( hps, &m, TRANSFORM_REPLACE, MAKEFIXED( i,0), cube );

      fxScale[0] = fxScale[1] = MAKEFIXED(5,0);
      GpiScale( hps, &m, TRANSFORM_ADD, fxScale, cube );

      ptlTrans.x = prectl->xRight/2;
      ptlTrans.y = prectl->yTop/2;
      GpiTranslate( hps, &m, TRANSFORM_ADD, &ptlTrans ); 

      GpiSetModelTransformMatrix( hps, 9L, &m, TRANSFORM_REPLACE );

      GpiPolyLineDisjoint( hps, 24L, cube );
      DosSleep( 50 );
      GpiPolyLineDisjoint( hps, 24L, cube );
    }
  }

  WinReleasePS( hps );
}


//--------------------------------------------------------------------------
//
//  DoPolygons()
//
// --------------------------------------------------------------------------
VOID DoPolygons( HPS hps )
{
  LONG i;
  POLYGON  aplygn[3];
  POINTL   aptlFront[4] = { 0,0,20,0,20,20,0,20 };
  POINTL   aptlTop[5]   = { 0,20,20,20,30,30,10,30,0,20 };
  POINTL   aptlSide[5]  = { 20,0,30,10,30,30,20,20,20,0 };

  GpiSetBackMix( hps, BM_OVERPAINT );
  GpiSetMix( hps, FM_OVERPAINT );
  
  aplygn[0].ulPoints = 4;
  aplygn[0].aPointl  = aptlFront;

  aplygn[1].ulPoints = 5;
  aplygn[1].aPointl  = aptlTop;

  aplygn[2].ulPoints = 5;
  aplygn[2].aPointl  = aptlSide;

  GpiSetCurrentPosition( hps, &aptlFront[0] );

  for ( i=0; i<3; i++ )
  {
    //GpiSetColor( hps, CLR_BLUE+i );
    GpiSetColor( hps, rand() % CLR_PALEGRAY );

    GpiPolygons( hps
                , 1L
                , &aplygn[i]
                , POLYGON_ALTERNATE
                , POLYGON_INCL
                );
  }
}

//--------------------------------------------------------------------------
//
//  DoMultipleCubes()
//
// --------------------------------------------------------------------------
VOID DoMultipleCubes( HWND hwnd, PRECTL prectl )
{
  HPS hps;
  MATRIXLF m;
  LONG i;

  hps = WinGetPS( hwnd );

  GpiQueryModelTransformMatrix( hps, 9L, &m );

  for( i=0; i<10; i++ )
  {
    m.lM31 = rand() % prectl->xRight;
    m.lM32 = rand() % prectl->yTop;

    GpiSetModelTransformMatrix( hps, 9L, &m, TRANSFORM_REPLACE );
    DoPolygons( hps );
  }
  WinReleasePS( hps );
}

//--------------------------------------------------------------------------
//
//  DoTextString()
//
// --------------------------------------------------------------------------
VOID DoTextString( HWND hwnd )
{
  HPS hps;
  GRADIENTL g;
  SIZEF box;
  FATTRS fattrs;
  LONG i;
  POINTL pointl;
  POINTL  ptlDef[16]  = { 10,0, 10,5, 10,10, 5,10, 0,10, -5,10, -10,10,
                         -10,5, -10,0, -10,-5,-10,-10, -5,-10, 0,-10, 5,
                         -10, 10,-10, 10,-5 };

  hps = WinGetPS( hwnd );

  fattrs.usRecordLength = sizeof(FATTRS) ;
  fattrs.fsSelection    = 0 ;
  fattrs.lMatch         = 0 ;
  strcpy(fattrs.szFacename, "Times New Roman" ) ;
  fattrs.idRegistry     = 0L ;
  fattrs.usCodePage     = 850L ;
  fattrs.lMaxBaselineExt = 0L;
  fattrs.lAveCharWidth   = 0L;
  fattrs.fsType          = 0L ;
  fattrs.fsFontUse       = FATTR_FONTUSE_OUTLINE ;


  GpiSetMix( hps, FM_XOR );
  GpiSetColor( hps, CLR_BLUE );

  GpiCreateLogFont(hps, NULL, 99L, &fattrs )  ;

  GpiSetCharSet( hps, 99L) ;

  box.cx = MAKEFIXED( 100, 0);
  box.cy = MAKEFIXED( 100,0 );
  GpiSetCharBox( hps, &box );
  pointl.x = 200;
  pointl.y = 200;

  for ( i=15; i>=0; i-- )
  {
    GpiSetCurrentPosition( hps, &pointl );
    g.x = ptlDef[i].x ; g.y= ptlDef[i].y ;
    GpiSetCharAngle( hps, &g);
    GpiCharString( hps, 3L, "GPI" );
    GpiSetCurrentPosition( hps, &pointl );
    DosSleep( 50 );
    GpiCharString( hps, 3L, "GPI" );
  }

  WinReleasePS( hps );
}
