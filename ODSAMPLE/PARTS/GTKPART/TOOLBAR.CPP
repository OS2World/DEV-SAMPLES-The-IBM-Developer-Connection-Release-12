
#define INCL_BASE
#define INCL_PM
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <odmenuid.h>

#include "GtkRes.h"
#include "UCMenus.h"

#define  OWNERWND(hwnd)   WinQueryWindow(hwnd,QW_OWNER)
#define  PARENTWND(hwnd)   WinQueryWindow(hwnd,QW_PARENT)

// -------------------------------------------------------------------------
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
void hide_scrollbars( HWND hwnd );

// -------------------------------------------------------------------------
HWND CreateToolbar(HWND hwndParent, HMODULE hmod, ULONG menuid)
{
  UCMINFO UCMInfo;
  ULONG   ulMCx, ulMCy, flgs = 0;
  HWND    hwndClient, hwndMenu, hTmp;

  WinRegisterClass( (HAB)0, "TOOLBAR", ClientWndProc, CS_SIZEREDRAW, 0);

  hwndClient = WinCreateWindow( hwndParent, "TOOLBAR", "", flgs, 8, 8, 0, 0,
                                hwndParent, HWND_TOP, menuid, NULL, NULL );

  memset(&UCMInfo, 0, sizeof(UCMINFO));
  UCMInfo.cb = sizeof(UCMINFO);

  UCMInfo.NbOfCols = 1;
  UCMInfo.NbOfRows = 7;
  UCMInfo.Style    = UCS_FRAMED    |          // Draw a 3D frame around the items
                     UCS_FORCESIZE |       // Force the size of the items
                     UCS_CHNGBMPBG |       // Use one of the bitmap colors as a background color
                     UCS_NO_DM     |       // No drag&drop manip
                     UCS_NO_CM;            // No context menu

  HPS hps = WinGetPS( hwndClient );
  // --  Set the color of the bitmaps which has to be replaced by the items background color
   UCMInfo.BgBmp     = GpiQueryRGBColor( hps, 0, CLR_PALEGRAY );
   UCMInfo.BgColor   = GpiQueryRGBColor( hps, 0, CLR_DARKGRAY );
  // -- Set the color of the items background
  UCMInfo.ItemBgColor = 0x00b0b0b0;
  WinReleasePS( hps );

  UCMInfo.cx = 24;   // Width of the items   ( used because UCS_FORCESIZE is set )
  UCMInfo.cy = 24;   // Height of the items
  UCMInfo.hModule = hmod;

  hwndMenu = UCMenuCreateFromResource( (HAB)0, hwndClient, hwndClient,
                                       CMS_MATRIX, 0, 0, 0, 0, HWND_TOP,
                                       FID_MENU, hmod, ID_TOOLS, &UCMInfo, &hTmp );

//  hide_scrollbars( hwndMenu );
  WinSendMsg( hwndMenu, UCMENU_QUERYSIZE, MPFROMP( &ulMCx ), MPFROMP( &ulMCy ) );
  WinSetWindowPos( hwndMenu, HWND_TOP, 0, 0, ulMCx, ulMCy, SWP_SIZE );
  WinSetWindowPos( hwndClient, HWND_TOP, 0, 0, ulMCx, ulMCy+8, SWP_SIZE );

  return hwndClient;
}

// -------------------------------------------------------------------------
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  #define DB_RAISED    0x0400
  #define DB_DEPRESSED 0x0800

  switch (msg) {
    case WM_CREATE:
      break;
    case WM_CONTROL:
      if ( SHORT1FROMMP(mp1) == FID_MENU ){
        if ( SHORT2FROMMP(mp1) == UCN_ITEMSELECTED ){
          PUCMITEM pucmi = (PUCMITEM)mp2;
          if (pucmi) {
            HWND hwndOwner = OWNERWND(hwnd);
            WinSendMsg( hwndOwner, WM_COMMAND,
                        MPFROMSHORT(pucmi->usId), MPFROMSHORT(CMDSRC_MENU) );
          }
        }
      }
      break;

    case WM_PAINT:
      {
      RECTL rcl;
      HPS hPS = WinBeginPaint( hwnd, (HPS)NULL, (PRECTL)0 );
      WinQueryWindowRect( hwnd, &rcl );
      rcl.yBottom = rcl.yTop - 8;
      GpiErase( hPS );
      WinDrawBorder( hPS, &rcl, 2, 2, CLR_DARKGRAY, CLR_PALEGRAY, DB_RAISED );
      WinEndPaint( hPS );
      }
      break;

    case WM_BUTTON1DOWN :
    case WM_BUTTON2DOWN :
      {
      TRACKINFO track;
      SWP       swp;
      RECTL     rclOld;
      HWND      hwndMenu = WinWindowFromID(hwnd, ID_TOOLS);

      WinSetWindowPos( hwndMenu, HWND_TOP, 0, 0, 0, 0, SWP_MOVE );
      RECTL rcl;
      WinQueryWindowRect( hwnd, &rcl );
      rcl.yBottom = rcl.yTop - 8;
      HPS hps = WinGetPS( hwnd );
      WinDrawBorder( hps, &rcl, 2, 2, CLR_DARKGRAY, CLR_WHITE, DB_DEPRESSED );
      WinReleasePS( hps );

      #define BORDER 2
      track.cxBorder = track.cyBorder = BORDER;
      track.cxGrid = track.cyGrid = 1;
      track.cxKeyboard = track.cyKeyboard = 4;

      WinQueryWindowPos( hwnd, &swp );
      rclOld.xLeft = swp.x - BORDER;
      rclOld.yBottom = swp.y - BORDER;
      rclOld.xRight = swp.x + swp.cx + BORDER;
      rclOld.yTop = swp.y + swp.cy + BORDER;
      track.rclTrack = rclOld;

      WinQueryWindowRect( PARENTWND(hwnd), &track.rclBoundary );

      track.ptlMinTrackSize.x = track.ptlMinTrackSize.y = 1;  //2*BOXLET_SIZE;
      track.ptlMaxTrackSize   = *(PPOINTL)&track.rclBoundary.xRight;
      track.fs = TF_MOVE;

      if (  WinTrackRect( PARENTWND(hwnd), NULLHANDLE, &track) )
        WinSetWindowPos( hwnd, HWND_TOP,
                         track.rclTrack.xLeft+BORDER, track.rclTrack.yBottom+BORDER,
                         0, 0, SWP_MOVE );
      WinInvalidateRect(hwnd, &rcl, TRUE);
      }
      break;

    default:
      return(WinDefWindowProc(hwnd, msg, mp1, mp2));
  }
  return 0;
}

//---------------------------------------------------------------
void hide_scrollbars( HWND hwnd )
{
  HENUM henum;
  HWND  hwndNext;
  CHAR  szClass[8];

  henum = WinBeginEnumWindows(hwnd);
  while ((hwndNext = WinGetNextWindow(henum)) != NULLHANDLE) {
    WinQueryClassName( hwndNext, strlen(szClass), szClass );
    if (!strncmp(WC_SCROLLBAR, szClass, strlen(szClass) )) {
      WinShowWindow( hwndNext, FALSE );
      }
    else hide_scrollbars( hwndNext );          // recursive!
    }
  WinEndEnumWindows(henum);
  }

