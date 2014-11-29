//---------------------------------------------------------------------------
//
//  Name     : star.c
//
//  Purpose  : Demonstrate alternate and winding mode fills.
//
//
//---------------------------------------------------------------------------

#define INCL_32
#define INCL_GPI
#define INCL_WIN
#include <os2.h>

HAB     hab;
HWND    hwndFrame;
PFNWP   pfnwpFrame;

#define STAR_CLASSNAME "STAR"

// Local function prototype declarations

MRESULT EXPENTRY ClientWndProc( HWND hwnd ,USHORT msg ,MPARAM mp1 ,MPARAM mp2 );

void DrawStar( HWND hwnd , ULONG lFlags );

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
   HWND hwndclient;
   QMSG qmsg;

   ULONG flCreateFlags =   FCF_BORDER     | FCF_SHELLPOSITION |
                           FCF_TASKLIST   | FCF_TITLEBAR      | FCF_SYSMENU |
                           FCF_SIZEBORDER | FCF_MINMAX ;

   hab = WinInitialize( (USHORT)NULL );

   hmq=WinCreateMsgQueue( hab,0 );

   WinRegisterClass( hab
                   , STAR_CLASSNAME
                   , (PFNWP)ClientWndProc
                   , (ULONG)CS_SIZEREDRAW
                   , (USHORT)256 );

   hwndFrame = WinCreateStdWindow( HWND_DESKTOP
                                 ,  0UL
                                 ,  &flCreateFlags
                                 ,  STAR_CLASSNAME
                                 ,  "Star- Alternate/winding mode fills"
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

//--------------------------------------------------------------------------
//
//  Client window procedure.
//
// --------------------------------------------------------------------------
MRESULT EXPENTRY ClientWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{

   switch (msg)
   {

     // When the mouse buttons are clicked, run the sample functions.

     case WM_BUTTON1DOWN:
     {
       DrawStar( hwnd ,  BA_WINDING );
     }
     break;

     case WM_BUTTON2DOWN:
     {
       DrawStar( hwnd , BA_ALTERNATE );
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

void DrawStar( HWND hwnd, ULONG lFlags )
{
  HPS hps;
  POINTL aptl1[3] = { 0,30,100,30,50,120 } ;
  POINTL aptl2[3] = { 50,0,100,90,0,90 };

  hps = WinGetPS( hwnd );

  GpiSetCurrentPosition( hps, aptl1 );
  GpiSetColor( hps, CLR_RED );
  GpiSetPattern( hps, PATSYM_DENSE4 );

  GpiBeginArea( hps, BA_BOUNDARY | lFlags );

  GpiPolyLine( hps, 3, aptl1 );

  GpiSetCurrentPosition( hps, aptl2 );

  GpiPolyLine( hps, 3, aptl2 );

  GpiEndArea( hps );

  WinReleasePS( hps );
}
