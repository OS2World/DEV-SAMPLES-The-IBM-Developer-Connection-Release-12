#define INCL_WIN
#define INCL_GPI
#define INCL_GRE_PALETTE

#include <os2.h>
#include <pmddi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "inkps.h"

#define RETURN_ERROR    1               /* return value for exit        */
#define BEEP_WARN_FREQ      60          /* frequency of warning beep    */
#define BEEP_WARN_DUR      100          /* duration of warning beep     */

VOID  SetAttributes( ULONG ulCmd );
VOID DisplayMessage( PCH str );

HAB      hab;
HMQ      hmq;
HWND     hwndFrame;
HWND     hwndClient;
HWND     hwndMenu;

HPS    hpsPen;
HDC    hdcPen = 0L;
HDC    hdcPenDeskTop = 0L;
BOOL fDesktopLocked ;

LONG  cxScreen;
LONG  cyScreen;
LONG  cxClient;
LONG  cyClient;
LONG  ForeMix;
LONG  BackMix;
LONG  ForeColor;
LONG  BackColor;
LONG  ScreenColor;
LONG  LineWidth;
LONG  LineType;
LONG  LineEnd;
LONG  LineJoin;
LONG  FillPattern;
LONG  EndPath;
LONG  PSType;
BOOL  LockDesktop;
BOOL  fButton1 = FALSE;
POINTL newptlCur;
POINTL Origin;
CHAR  newstr[] = "New InkPS API's";
BOOL  fInPath = FALSE;

RECTL    rect;
POINTL   ptl[2], txtbox[TXTBOX_COUNT];
ULONG    ulWidth;
ULONG    ulLeft;
FONTMETRICS FontMetrics;


int main( VOID )
{
   QMSG  qmsg;
   BOOL success;
   PCHAR ClientClass =  "New InkPS Demo";
   ULONG FrameFlags  =  FCF_TITLEBAR      | FCF_SYSMENU  |
                        FCF_SIZEBORDER    | FCF_MINMAX   |
                        FCF_TASKLIST      | FCF_MENU     |
                        FCF_ICON ;


  //Spin loop for debug attachment
//  int i;
//  i = 4;
//  while (i == 4)
//  {
//     i = 4;
//  } /* endwhile */


   hab = WinInitialize(0L);
   if(!hab)
   {
     DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
     exit(RETURN_ERROR);
   }

   hmq = WinCreateMsgQueue( hab, 0 );
   if(!hmq)
   {
     DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
     WinTerminate(hab);
     exit(RETURN_ERROR);
   }

   success = WinRegisterClass( hab,
                               ClientClass,
                               (PFNWP)ClientWndProc,
                               CS_SIZEREDRAW | CS_MOVENOTIFY,
                               0 );
   if (!success)
   {
     DisplayMessage("Could Not Register Class");
     WinDestroyMsgQueue( hmq );
     WinTerminate( hab );
     exit(RETURN_ERROR);
   }

   hwndFrame = WinCreateStdWindow(  HWND_DESKTOP,
                                    WS_VISIBLE,
                                    &FrameFlags,
                                    ClientClass,
                                    "New InkPS Demo",
                                    WS_VISIBLE,
                                    0,
                                    WND_ID,
                                    &hwndClient );
   if (!hwndFrame)
   {
     DisplayMessage("Could Not Create Main Window");
     WinDestroyMsgQueue( hmq );
     WinTerminate( hab );
     exit(RETURN_ERROR);
   }

   cxScreen = WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
   cyScreen = WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );

   WinSetWindowPos(  hwndFrame,
                     HWND_TOP,
                     cxScreen / 5,
                     cyScreen / 5,
                     cxScreen * 3 / 5,
                     cyScreen * 3 / 5,
                     SWP_MOVE | SWP_SIZE | SWP_ACTIVATE );


   while ( WinGetMsg( hab, &qmsg, 0L, 0, 0 ) )
      WinDispatchMsg( hab, &qmsg );

   WinDestroyWindow( hwndFrame );
   WinDestroyMsgQueue( hmq );
   WinTerminate( hab );

   return 0;
}

MRESULT EXPENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   HPS      hps;
   USHORT   usCommand;
   SWP      swp;

   switch( msg )
   {
      case WM_SIZE:
         cxClient = SHORT1FROMMP( mp2 );
         cyClient = SHORT2FROMMP( mp2 );

         //------------------------------------------------------------
         // Fall through to the move case so that we can fetch the
         // window origin...
         //------------------------------------------------------------

      case WM_MOVE:
         WinQueryWindowPos( hwndFrame, &swp );
         Origin.x = swp.x;
         Origin.y = swp.y;

         WinQueryWindowPos( hwnd, &swp );
         Origin.x += swp.x;
         Origin.y += swp.y;
         break;

      case WM_BUTTON1DOWN:
         {
             if ( !fInPath )
                {
                   LONG lBeginOption = 0L;
                   SIZEL sizl ;
                   sizl.cx = sizl.cy = 0 ;

                   if (hdcPen == (HDC) 0L )
                       hdcPen = WinOpenWindowDC( hwnd );

                   if ( PSType == PPS_DESKTOP )
                   {
                      if (hdcPenDeskTop == (HDC) 0L )
                      {
        
                         hdcPenDeskTop =  DevOpenDC (hab, OD_SCREEN, "*", 
                                                     0, 0, 0) ;
                      }

                      if (hpsPen)
                         GpiDestroyPS (hpsPen) ;

                      hpsPen = GpiCreatePS( hab, hdcPenDeskTop, &sizl, GPIT_MICRO |
                                                                       GPIT_INK   |
                                                                       GPIA_ASSOC |
                                                                       PU_PELS) ;
                   }
                   else
                   {
                      if (hpsPen)
                         GpiDestroyPS (hpsPen) ;

                      hpsPen = GpiCreatePS( hab, hdcPen, &sizl, GPIT_MICRO |
                                                                GPIT_INK   |
                                                                GPIA_ASSOC |
                                                                PU_PELS) ;
                   }

                   GpiSetColor( hpsPen, ForeColor );
                   GpiSetMix( hpsPen, ForeMix );
                   GpiSetBackColor( hpsPen, BackColor );
                   GpiSetBackMix( hpsPen, BackMix );
                   GpiSetLineWidthGeom( hpsPen, LineWidth );
                   GpiSetLineType( hpsPen, LineType );
                   GpiSetLineEnd( hpsPen, LineEnd );
                   GpiSetLineJoin( hpsPen, LineJoin );
                   GpiSetPattern( hpsPen, FillPattern );

                   if (LockDesktop)
                   {
                      WinLockWindowUpdate( HWND_DESKTOP, HWND_DESKTOP );
                      fDesktopLocked = TRUE ;
                   }
                   GpiBeginInkPath( hpsPen, 1L, 0);
                   fInPath = TRUE;
                }

             fButton1 = TRUE;

             newptlCur.x = (LONG) (SHORT) mp1;

             newptlCur.y = (LONG) (SHORT) ((LONG) mp1 >> 16);

             if ( PSType == PPS_DESKTOP )
             {
                 newptlCur.x += Origin.x;
                 newptlCur.y += Origin.y;
             }

             GpiStrokeInkPath( hpsPen, 1L, 1L, &newptlCur, PPS_INKDOWN );

             if ( PSType == PPS_DESKTOP )
                 WinSetCapture( HWND_DESKTOP, hwnd );
         }
         break;


      case WM_BUTTON1UP:

         if ( fButton1 )
         {
             GpiStrokeInkPath( hpsPen, 1L, 1L, &newptlCur, PPS_INKUP );
             fButton1 = FALSE;
         }

         break;

      case WM_ENDINKPATH:
      case WM_BUTTON2DBLCLK:
         fInPath = FALSE;
         GpiEndInkPath( hpsPen, EndPath );

         if (LockDesktop)
         {
            WinLockWindowUpdate( HWND_DESKTOP, (ULONG) 0L );
            fDesktopLocked = FALSE;
         }
         GpiDestroyPS( hpsPen );

         if ( PSType == PPS_DESKTOP )
             WinSetCapture( HWND_DESKTOP, (HWND) 0L );
         break;


      case WM_MOUSEMOVE:
         if ( fButton1 && fInPath )
         {
             newptlCur.x = (LONG) (SHORT) mp1;

             newptlCur.y = (LONG) (SHORT) ((LONG) mp1 >> 16);

             if ( PSType == PPS_DESKTOP )
             {
                 newptlCur.x += Origin.x;
                 newptlCur.y += Origin.y;
             }

            GpiStrokeInkPath( hpsPen, 1L, 1L, &newptlCur, PPS_INKMOVE );
         }

         break; //return FALSE;
      
      
      case WM_COMMAND:
         usCommand = LOUSHORT( mp1 );
         switch ( usCommand )
         {
            case MI_OPT_CLEAR:

               WinInvalidateRect( hwnd, 0, 0 );
               return FALSE;

         }

         if ( usCommand > MI_ATTRS && usCommand < MI_MAX )
         {
            SetAttributes( (ULONG)usCommand );
            return (MRESULT)FALSE;
         }
         break;

      case WM_CREATE:
         hwndMenu = WinWindowFromID(   WinQueryWindow( hwnd, QW_PARENT ),
                                       FID_MENU );

         //----------------------------------------------------------------
         // Set default attributes.
         //----------------------------------------------------------------
         ForeMix     = FM_XOR;
         BackMix     = BM_OVERPAINT;
         ForeColor   = CLR_YELLOW;
         BackColor   = CLR_BLUE;
         ScreenColor = CLR_BLUE;
         LineWidth   = 1;
         LineType    = LINETYPE_SOLID;
         LineEnd     = LINEEND_FLAT;
         LineJoin    = LINEJOIN_ROUND;
         FillPattern = PATSYM_SOLID;
         EndPath     = PPE_ERASEPATH;
         PSType      = PPS_WINDOW;
         LockDesktop = FALSE;

         WinCheckMenuItem( hwndMenu, MI_FM + ForeMix,     TRUE );
         WinCheckMenuItem( hwndMenu, MI_BM + BackMix,     TRUE );
         WinCheckMenuItem( hwndMenu, MI_FC + ForeColor,   TRUE );
         WinCheckMenuItem( hwndMenu, MI_BC + BackColor,   TRUE );
         WinCheckMenuItem( hwndMenu, MI_SC + ScreenColor, TRUE );
         WinCheckMenuItem( hwndMenu, MI_LW + LineWidth,   TRUE );
         WinCheckMenuItem( hwndMenu, MI_LT + LineType,    TRUE );
         WinCheckMenuItem( hwndMenu, MI_LE + LineEnd,     TRUE );
         WinCheckMenuItem( hwndMenu, MI_LJ + LineJoin,    TRUE );
         WinCheckMenuItem( hwndMenu, MI_FP + FillPattern, TRUE );
         WinCheckMenuItem( hwndMenu, MI_IO + EndPath,     TRUE );
         WinCheckMenuItem( hwndMenu, MI_PS + PSType,      TRUE );

         WinInvalidateRect( hwnd, 0, 0 );

         break;

      case WM_ERASEBACKGROUND:
      case WM_PAINT:
         hps = WinBeginPaint( hwnd, 0, &rect );
         WinQueryWindowRect( hwnd, &rect );
         WinFillRect( hps, &rect, ScreenColor );
         GpiSetColor( hps, ForeColor );
         GpiSetBackColor( hps, BackColor );

         ulWidth = cxClient;
         ulLeft = 0;

         GpiQueryFontMetrics( hps, sizeof(FONTMETRICS), &FontMetrics );

         GpiQueryTextBox( hps, strlen(newstr), newstr, TXTBOX_COUNT, txtbox );
         rect.xLeft = ulLeft + ( ulWidth - ( txtbox[TXTBOX_BOTTOMRIGHT].x - txtbox[TXTBOX_BOTTOMLEFT].x ) ) / 2;
         rect.xRight = rect.xLeft + ( txtbox[TXTBOX_BOTTOMRIGHT].x - txtbox[TXTBOX_BOTTOMLEFT].x );
         rect.yTop = cyClient * 9 / 10;
         rect.yBottom = rect.yTop - ( txtbox[TXTBOX_TOPLEFT].y - txtbox[TXTBOX_BOTTOMLEFT].y );
         ptl[0].x = rect.xLeft;
         ptl[0].y = rect.yBottom + FontMetrics.lMaxDescender;
         GpiCharStringPosAt( hps, ptl, &rect, CHS_OPAQUE | CHS_CLIP, strlen(newstr), newstr, 0 );

         WinEndPaint( hps );
         return FALSE;


      case WM_CLOSE:

         if (hdcPenDeskTop)
            DevCloseDC (hdcPenDeskTop) ;
         break;
   }

   return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
}

VOID  SetAttributes( ULONG ulCmd )
{

   WinPostMsg( hwndClient, WM_ENDINKPATH, (MPARAM)0L, (MPARAM)0L );

   if ( ulCmd > MI_FM && ulCmd < MI_BM )
   {
      //-------------------------------------------------------------
      // Un-check old foreground mix.
      //-------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MI_FM + ForeMix, FALSE );
      ForeMix = ulCmd - MI_FM;
      WinCheckMenuItem( hwndMenu, MI_FM + ForeMix, TRUE );

      return;
   }

   if ( ulCmd > MI_BM && ulCmd < MI_FC )
   {
      //-------------------------------------------------------------
      // Un-check old background mix.
      //-------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MI_BM + BackMix, FALSE );
      BackMix = ulCmd - MI_BM;
      WinCheckMenuItem( hwndMenu, MI_BM + BackMix, TRUE );

      return;
   }

   if ( ulCmd > (MI_FC-4) && ulCmd < (MI_BC-4) )
   {
      //-------------------------------------------------------------
      // Un-check old foreground color.
      //-------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MI_FC + ForeColor, FALSE );
      ForeColor = ulCmd - MI_FC;
      WinCheckMenuItem( hwndMenu, MI_FC + ForeColor, TRUE );

      return;
   }

   if ( ulCmd > (MI_BC-4) && ulCmd < (MI_SC-4) )
   {
      //-------------------------------------------------------------
      // Un-check old background color.
      //-------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MI_BC + BackColor, FALSE );
      BackColor = ulCmd - MI_BC;
      WinCheckMenuItem( hwndMenu, MI_BC + BackColor, TRUE );

      return;
   }

   if ( ulCmd > (MI_SC-4) && ulCmd < MI_LW-4 )
   {
      //-------------------------------------------------------------
      // Un-check old screen color.
      //-------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MI_SC + ScreenColor, FALSE );
      ScreenColor = ulCmd - MI_SC;
      WinCheckMenuItem( hwndMenu, MI_SC + ScreenColor, TRUE );
      WinInvalidateRect( hwndClient, 0, 0 );

      return;
   }

   if ( ulCmd > MI_LW && ulCmd < MI_LT )
   {
      //-------------------------------------------------------------
      // Un-check old geometric line width color.
      //-------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MI_LW + LineWidth, FALSE );
      LineWidth = ulCmd - MI_LW;
      WinCheckMenuItem( hwndMenu, MI_LW + LineWidth, TRUE );

      return;
   }

   if ( ulCmd > MI_LT && ulCmd < MI_LE )
   {
      //-------------------------------------------------------------
      // Un-check old Line Type
      //-------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MI_LT + LineType, FALSE );
      LineType = ulCmd - MI_LT;
      WinCheckMenuItem( hwndMenu, MI_LT + LineType, TRUE );

      return;
   }

   if ( ulCmd > MI_LE && ulCmd < MI_LJ )
   {
      //-------------------------------------------------------------
      // Un-check old Line End
      //-------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MI_LE + LineEnd, FALSE );
      LineEnd = ulCmd - MI_LE;
      WinCheckMenuItem( hwndMenu, MI_LE + LineEnd, TRUE );

      return;
   }

   if ( ulCmd > MI_LJ && ulCmd < MI_FP )
   {
      //-------------------------------------------------------------
      // Un-check old Line Join
      //-------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MI_LJ + LineJoin, FALSE );
      LineJoin = ulCmd - MI_LJ;
      WinCheckMenuItem( hwndMenu, MI_LJ + LineJoin, TRUE );

      return;
   }

   if ( ulCmd > MI_FP && ulCmd < MI_IO )
   {
      //-------------------------------------------------------------
      // Un-check old Fill Pattern
      //-------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MI_FP + FillPattern, FALSE );
      FillPattern = ulCmd - MI_FP;
      WinCheckMenuItem( hwndMenu, MI_FP + FillPattern, TRUE );

      return;
   }

   if ( ulCmd > MI_IO && ulCmd < MI_PS )
   {
      //-------------------------------------------------------------
      // Un-check old XOR Inking option.
      //-------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MI_IO + EndPath, FALSE );
      EndPath = ulCmd - MI_IO;
      WinCheckMenuItem( hwndMenu, MI_IO + EndPath, TRUE );

      return;
   }

   if ( ulCmd > MI_PS && ulCmd < MI_LOCK )
   {
      //-------------------------------------------------------------
      // Un-check old Ink PS Type.
      //-------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MI_PS + PSType, FALSE );
      PSType = ulCmd - MI_PS;
      WinCheckMenuItem( hwndMenu, MI_PS + PSType, TRUE );

      return;
   }

   if ( ulCmd == MI_LOCK )
   {
      //-------------------------------------------------------------
      // Toggle the Lock setting.
      //-------------------------------------------------------------
      LockDesktop = !LockDesktop;
      WinCheckMenuItem( hwndMenu, MI_LOCK, LockDesktop );

      return;
   }

   return;
}


/****************************************************************
 *  Name:   DisplayMessage
 *
 *  Description : Displays error messages.
 *
 *  Concepts : Displays a message box containing an error
 *             message each time called.
 *
 *  API's : WinMessageBoxNon
 *
 *  Parameters   : str - error message to display
 *
 *  Returns:  Void
 *
 ****************************************************************/
VOID DisplayMessage(PCH str)
{
  /*
   * This routine calls WinMessageBox to dsplay error messages to the
   * user.
   */

  WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, (PCH) str,
     (PCH) "Pen Demo Error", 0,
     MB_OK | MB_APPLMODAL | MB_MOVEABLE | MB_ERROR);
}
