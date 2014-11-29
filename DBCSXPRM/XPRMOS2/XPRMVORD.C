/***************************************************************************/
/* XPRMVORD.C    XPG4 Primer for OS/2 WARP - V1.0       11/15/95           */
/*               Procedure for view orders window.                         */
/***************************************************************************/
#define INCL_WIN
#define INCL_DOS
#define INCL_GPI
#define INCL_WINERRORS
#include <os2.h>                       /* System Include File              */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#include "xprmos2.h"                   /* Application Include File         */
#include "xprmres.h"                   /* Application Include NLS File     */
#include "xprmdata.h"                  /* Application Data Structure file  */

/**************************************/
/* External Variables                 */
/**************************************/
extern HAB hab;
extern HMODULE hModRsrc;
extern HWND hwndHelpInstance;
extern char acMsgTable[][MAX_MSG_LEN];

/* device information */
extern long lDevCaps[4];

/**************************************/
/* Static Variables                   */
/**************************************/
static ORDERREC* pFoundOrders;
static SHORT sHRange, sHPos;
static RECTL rclViewed;
static long lWinFont[2];
static char acOutText[NUMLINES][NUMCHARS];

/**************************************/
/* function prototypes - external     */
/**************************************/
extern BOOL formatDate( time_t timeval, char* pszBuf, size_t len );
extern BOOL formatTime( time_t timeval, char* pszBuf, size_t len );
extern void formatPrice( double dPrice, char* pszBuf, size_t len, BOOL flCurrency );
extern BOOL isDigitString( char *pszString );
/* PM related functions */
extern void putWindowToCenter( HWND hwnd );
/* file I/O */
extern ORDERREC* getOrderRec( char *pszKey, enum orderitem e_category );

/**********************************************************************/
/* queryOrderdlgProc()                                                */
/*   Query order dialog's procedure.  queries the matched order and   */
/*   creates a secondary window if found.                             */
/**********************************************************************/
MRESULT EXPENTRY queryOrderDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
CHAR acOrderKey[33];
int iTemp;
static enum orderitem e_category;
/* To create viewOrder window */
ULONG flCreateViewOrder;
HWND hwndViewOrder;
HWND hwndClViewOrder;
enum orderitem procBNCLICKEDonQueryOrder( HWND hwnd, SHORT itemId );

    switch(msg)
    {
        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1))
            {
               case DID_OK:

                   WinQueryDlgItemText( hwnd, QO_EF_KEY, 32, acOrderKey );
                   if( strlen(acOrderKey) == 0 )
                   {
                      showMessageBox( hwnd, IDS_WARNING_3,
                                      IDS_WARNING_CAPTION, MB_WARNING );
                      clearField( hwnd, QO_EF_KEY );
                      break;
                   }

                   if( acOrderKey[0] == '*' )            /* get all records*/
                   {
                      pFoundOrders = getOrderRec( NULL, all );
                   }
                   else
                   {
                      if( e_category == ordernum )
                      {
                         if( isDigitString(acOrderKey) == FALSE )
                         {
                            showMessageBox( hwnd, IDS_WARNING_4,
                                            IDS_WARNING_CAPTION, MB_WARNING );
                            clearField( hwnd, QO_EF_KEY );
                            break;
                         }

                         iTemp = atoi( acOrderKey );
                         sprintf( acOrderKey, "%06d", iTemp );
                      }
                      pFoundOrders = getOrderRec( acOrderKey, e_category );
                   }

                   if( pFoundOrders == NULL )
                   {
                       WinMessageBox( HWND_DESKTOP, hwnd,
                                      acMsgTable[IDS_WARNING_1],
                                      acMsgTable[IDS_WARNING_CAPTION],
                                      (USHORT) NULL,
                                      MB_OK | MB_WARNING );
                       break;
                   }

                   /* Create a secondary window with handle hwndViewOrder */
                   /* and parent HWND_DESKTOP */
                   flCreateViewOrder= FCF_MAXBUTTON | FCF_MENU | FCF_MINBUTTON
                                    | FCF_SIZEBORDER | FCF_SYSMENU | FCF_TITLEBAR;
                   hwndViewOrder = WinCreateStdWindow( HWND_DESKTOP,
                                                       WS_VISIBLE,
                                                       &flCreateViewOrder,
                                                       "clXPRMViewOrderWindow",
                                                       "",
                                                       0L,
                                                       hModRsrc,
                                                       WID_VIEWORDER,
                                                       (PHWND) & hwndClViewOrder);

                   /* Owner is primary window */
                   WinSetOwner( hwndViewOrder, hwnd );
                   break;

               case DID_CANCEL:
                   /* Dismiss the dialog box */
                   WinDismissDlg (hwnd,DID_CANCEL);
                   break;
               }
               break;

        case WM_CONTROL:
            switch (SHORT1FROMMP(mp1))
            {
               case QO_PB_NUMBER:
               case QO_PB_DATE:
                 if( SHORT2FROMMP(mp1) == BN_CLICKED )
                    e_category = procBNCLICKEDonQueryOrder( hwnd, SHORT1FROMMP(mp1) );
                 break;
            }
            break;

        case WM_INITDLG:

            putWindowToCenter( hwnd );
            /* Max no. of chars for QO_EF_KEY is 32 */
            WinSendDlgItemMsg( hwnd, QO_EF_KEY, EM_SETTEXTLIMIT, MPFROMSHORT(13), 0L);
            /* Set button QO_PB_DATE to checked */
            WinSendDlgItemMsg( hwnd, QO_PB_DATE, BM_SETCHECK, MPFROMSHORT(1), 0L);
            e_category = orderdate;
            break;

      default:
            return(WinDefDlgProc(hwnd,msg,mp1,mp2));
            break;
    }
    return(FALSE);
}

/**********************************************************************/
/* procBNCLICKEDonQueryOrder()                                        */
/*   handles BN_CLICKED message on QO_PB_NUMBER and QO_PB_DATE push   */
/*   buttons.  Sets the QO_EF_KEY entry field's limit depending on    */
/*   the selected button.  Returns category of the key.               */
/**********************************************************************/
static enum orderitem procBNCLICKEDonQueryOrder( HWND hwnd, SHORT itemId )
{
enum orderitem e_category;

    switch (itemId)
    {
      case QO_PB_NUMBER:
        e_category = ordernum;
        WinSendDlgItemMsg( hwnd, QO_EF_KEY,
                           EM_SETTEXTLIMIT, MPFROMSHORT(6), 0L);
        break;

      case QO_PB_DATE:
        e_category = orderdate;
        WinSendDlgItemMsg( hwnd, QO_EF_KEY,
                           EM_SETTEXTLIMIT, MPFROMSHORT(13), 0L);
        break;
    }
    return e_category;
}

/***************************************************************************/
/*  viewOrderWinProc()                                                     */
/*     Window procedure for view orders window.                            */
/***************************************************************************/
MRESULT EXPENTRY viewOrderWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static HWND hwndFrame;
static ORDERREC *pCurrentOrder;
static size_t szMaxTitle;
RECTL rectl;
SWP swp;
VOINFO *pInfo;

BOOL initViewOrderWindow( HWND hwnd );
void locateControls( HWND hwnd, VOINFO *pInfo, SHORT scx, SHORT scy );
void createOutData( HWND hwnd, ORDERREC* pOrder, VOINFO* pInfo );
void callFontDlg( HPS hps, HWND hwnd, PFONTDLG pFontDlg );

    pInfo = WinQueryWindowPtr( hwnd, 0 );

    switch(msg)
    {
        case WM_COMMAND:
        switch( SHORT1FROMMP(mp1) )
        {
            case DID_CANCEL:
               WinDestroyWindow( hwndFrame );
               break;

            case VO_PB_NEXT:
               pCurrentOrder = pCurrentOrder->next;
               createOutData( hwnd, pCurrentOrder, pInfo );
               WinQueryWindowPos( pInfo->hwndSubWindow, &swp );
               rectl.yBottom = swp.y;
               rectl.yTop = swp.y+swp.cy;
               rectl.xLeft = swp.x;
               rectl.xRight = swp.x+swp.cx;
               /* send update message to pInfo->hwndSubWindow */
               WinSendMsg( pInfo->hwndSubWindow, WM_USER_NEW, 0L, 0L );
               WinInvalidateRect( hwnd, &rectl, TRUE );   /* cause WM_PAINT*/

               if( pCurrentOrder->next == NULL )      /* if the last record*/
                   WinEnableWindow( pInfo->hwndPBNext, FALSE );
               if( pCurrentOrder->prev != NULL )/* if the prev. record exists*/
                  WinEnableWindow( pInfo->hwndPBPrev, TRUE );
               break;

            case VO_PB_PREV:
               pCurrentOrder = pCurrentOrder->prev;
               createOutData( hwnd, pCurrentOrder, pInfo );
               WinQueryWindowPos( pInfo->hwndSubWindow, &swp );
               rectl.yBottom = swp.y;
               rectl.yTop = swp.y+swp.cy;
               rectl.xLeft = swp.x;
               rectl.xRight = swp.x+swp.cx;
               /* send update message to pInfo->hwndSubWindow */
               WinSendMsg( pInfo->hwndSubWindow, WM_USER_NEW, 0L, 0L );
               WinInvalidateRect( hwnd, &rectl, TRUE );     /* cause WM_PAINT*/

               if( pCurrentOrder->prev == NULL )     /* if the first record*/
                  WinEnableWindow( pInfo->hwndPBPrev, FALSE );
               if( pCurrentOrder->next != NULL )/* if the next record exists*/
                  WinEnableWindow( pInfo->hwndPBNext, TRUE );
               break;

            case MID_FONT:                                          /* Font*/
               /* call a font dialog */
               callFontDlg( pInfo->hps, hwnd, &pInfo->fontDlg );
               /* send setfont message to pInfo->hwndSubWindow */
               WinSendMsg( pInfo->hwndSubWindow, WM_USER_FONT,
                           MPFROMP((PVOID)&pInfo->fontDlg.fAttrs), 0L );
               break;
        }
        break;                                         /* end of WM_COMMAND*/

        case WM_PAINT:
        {
        char acBuf[80];
        HPS hps;

            sprintf( acBuf, "%s: %s",
                     acMsgTable[IDS_VO_TITLE], pCurrentOrder->acOrderNum );
            WinSetWindowText( WinWindowFromID( hwndFrame, FID_TITLEBAR ), acBuf );
            hps = WinBeginPaint( hwnd, NULLHANDLE, (PRECTL) &rectl );
            WinFillRect( hps, (PRECTL) &rectl, SYSCLR_WINDOW );/* paint the rectangle*/
            /* send update message to pInfo->hwndSubWindow */
            WinSendMsg( pInfo->hwndSubWindow, WM_PAINT, 0L, 0L );
            WinEndPaint( hps );
            break;
        }

        case WM_SIZE:
            WinInvalidateRect( hwnd, NULL, TRUE );        /* cause WM_PAINT*/
            locateControls( hwnd, pInfo, SHORT1FROMMP(mp2), SHORT2FROMMP(mp2) );
            break;

        case WM_CREATE:
        {
        long lWinCx,lWinCy, lWinX, lWinY;

            if( initViewOrderWindow( hwnd ) == FALSE )
               return MPFROMSHORT( TRUE );

            hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
            pInfo = WinQueryWindowPtr( hwnd, 0 );
            pCurrentOrder = pFoundOrders;
            createOutData( hwnd, pCurrentOrder, pInfo );
            if( pCurrentOrder->prev == NULL )        /* if the first record*/
               WinEnableWindow( pInfo->hwndPBPrev, FALSE );
            if( pCurrentOrder->next == NULL )         /* if the last record*/
               WinEnableWindow( pInfo->hwndPBNext, FALSE );

            /* set size and position */
            lWinCx = lDevCaps[CHAR_X]*75;
            lWinCy = lDevCaps[CHAR_Y]*20;
            lWinX = (lDevCaps[DISP_X]-lWinCx)/2;
            lWinY = (lDevCaps[DISP_Y]-lWinCy)/2;
            /* Set the position of the window */
            WinSetWindowPos( hwndFrame, HWND_BOTTOM,
                             lWinX,lWinY, lWinCx,lWinCy,
                             SWP_MOVE | SWP_SIZE | SWP_ACTIVATE );
            WinSendMsg( hwndHelpInstance, HM_SET_ACTIVE_WINDOW,
                        MPFROMLONG(hwndFrame), MPFROMLONG(hwndFrame) );

            return(MRESULT)(FALSE);
        }

        case WM_ACTIVATE:
            if( SHORT1FROMMP(mp1) == TRUE )                    /* activated*/
            {
                WinSendMsg( hwndHelpInstance, HM_SET_ACTIVE_WINDOW,
                            MPFROMLONG(hwndFrame), MPFROMLONG(hwndFrame) );
            }
            else
            {
                WinSendMsg( hwndHelpInstance, HM_SET_ACTIVE_WINDOW,
                            NULL, NULL );
            }
            break;

        case WM_CLOSE:
            GpiDestroyPS( pInfo->hps );
            if( pInfo != NULL )  free( pInfo );
            WinDestroyWindow( hwndFrame );
            break;

        default:
            return(WinDefWindowProc(hwnd,msg,mp1,mp2));
            break;
    }
return(FALSE);
}

/***************************************************************************/
/*  initViewOrderWindow()                                                  */
/*     Creates presentation space, child windows and queries default font. */
/*     Allocate FONTDLG object, initialize it and sets as a window pointer.*/
/*     Returns handle of the presentation space.                           */
/***************************************************************************/
BOOL initViewOrderWindow( HWND hwnd )
{
VOINFO*  pInfo;
HDC hdc;
SIZEL sizlPSPage;
ULONG flCreateSubWindow;
HWND hwndSubWindow, hwndClSubWindow;
FONTMETRICS fm;

    /* default value of window's font size */
    lWinFont[CHAR_X] = lDevCaps[CHAR_X];
    lWinFont[CHAR_Y] = lDevCaps[CHAR_Y];

    if( (pInfo = malloc(sizeof(VOINFO))) == NULL )
    {
       showMessageBox( hwnd, IDS_ERROR_5,
                       IDS_ERROR_CAPTION, MB_ERROR );
       return NULLHANDLE;
    }

    memset( pInfo, 0, sizeof(VOINFO) );

    /* create presentation space */
    if( (hdc=WinQueryWindowDC( hwnd )) == NULLHANDLE )
       hdc = WinOpenWindowDC( hwnd );
    sizlPSPage.cx = 0L;
    sizlPSPage.cy = 0L;
    pInfo->hps = GpiCreatePS( hab, hdc, &sizlPSPage,
                              PU_ARBITRARY | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC );

    /* create data window */
    flCreateSubWindow = FCF_SIZEBORDER | FCF_HORZSCROLL | FCF_VERTSCROLL ;
    pInfo->hwndSubWindow = WinCreateStdWindow( hwnd,
                                               WS_VISIBLE,
                                               &flCreateSubWindow,
                                               "clXPRMViewSubWindow",
                                               "",
                                               0L,
                                               hModRsrc,
                                               WID_SUBWINDOW,
                                               (PHWND) & hwndClSubWindow);
    /* Owner is primary window */
    WinSetOwner( pInfo->hwndSubWindow, hwnd );

    /* create buttons */
    pInfo->hwndPBNext = WinCreateWindow( hwnd, WC_BUTTON,
                                         acMsgTable[IDS_VO_NEXT],
                                         BS_PUSHBUTTON | WS_VISIBLE, 0,0,0,0,
                                         hwnd, HWND_TOP, VO_PB_NEXT,0L, NULL);

    pInfo->hwndPBPrev = WinCreateWindow( hwnd, WC_BUTTON,
                                         acMsgTable[IDS_VO_PREV],
                                         BS_PUSHBUTTON | WS_VISIBLE, 0,0,0,0,
                                         hwnd, HWND_TOP, VO_PB_PREV,0L, NULL);

    pInfo->hwndPBCancel=WinCreateWindow( hwnd, WC_BUTTON,
                                         acMsgTable[IDS_VO_CANCEL],
                                         BS_PUSHBUTTON | WS_VISIBLE, 0,0,0,0,
                                         hwnd, HWND_TOP, DID_CANCEL,0L, NULL);

    /* set up fontDlg structure */
    GpiQueryFontMetrics( pInfo->hps, sizeof(FONTMETRICS), &fm );
    pInfo->fontDlg.fAttrs.usRecordLength = sizeof(FATTRS);
    pInfo->fontDlg.fAttrs.fsSelection = fm.fsSelection;
    strcpy( pInfo->fontDlg.fAttrs.szFacename, fm.szFacename );

    /* set the initial value of sub window's sliders */
    pInfo->subInfo.sHscrollPos = pInfo->subInfo.sVscrollPos = 0;
    pInfo->subInfo.sHscrollMax = (NUMCHARS-1)*lWinFont[CHAR_X];
    pInfo->subInfo.sVscrollMax = NUMLINES-1;

    WinSetWindowPtr( hwnd, 0, (PVOID)pInfo );
    WinSetWindowPtr( hwndClSubWindow, 0, (PVOID)(&pInfo->subInfo) );

    return TRUE;
}

/***************************************************************************/
/*  locateControls()                                                       */
/*     Calculates and locates the child windows depending on the client    */
/*     window size.                                                        */
/***************************************************************************/
static void locateControls( HWND hwnd, VOINFO* pInfo, SHORT scx, SHORT scy )
{
int i;
LONG lWinCx[3];
LONG lWinCy,lWinX,lWinY, lDist;
POINTL aptl[TXTBOX_COUNT];
RECTL rectl;

    lWinCy = lDevCaps[CHAR_Y]*2;
    lWinY = lDevCaps[CHAR_Y];
    for( i=0; i<3; i++ )
    {
       GpiQueryTextBox( pInfo->hps, strlen(acMsgTable[IDS_VO_NEXT+i]),
                        acMsgTable[IDS_VO_NEXT+i],
                        TXTBOX_COUNT, aptl );
       lWinCx[i] = aptl[TXTBOX_TOPRIGHT].x-aptl[TXTBOX_TOPLEFT].x+lDevCaps[CHAR_X]*4;
    }

    lDist = (scx-lWinCx[0]-lWinCx[1]-lWinCx[2])/4;
    if( lDist < 0 ) lDist = 0;

/* Cancel button */
    lWinX = scx - lWinCx[2] - lDist;
    WinSetWindowPos( pInfo->hwndPBCancel, HWND_BOTTOM,
                     lWinX,lWinY, lWinCx[2],lWinCy,
                     SWP_MOVE | SWP_SIZE | SWP_SHOW );
/* PREV button */
    lWinX -= (lWinCx[1] + lDist);
    WinSetWindowPos( pInfo->hwndPBPrev, HWND_BOTTOM,
                     lWinX,lWinY, lWinCx[1],lWinCy,
                     SWP_MOVE | SWP_SIZE | SWP_SHOW );
/* NEXT button */
    lWinX -= (lWinCx[0] + lDist);
    WinSetWindowPos( pInfo->hwndPBNext, HWND_BOTTOM,
                     lWinX,lWinY, lWinCx[0],lWinCy,
                     SWP_MOVE | SWP_SIZE | SWP_SHOW );
/* SubWindow */
    WinQueryWindowRect( hwnd, &rectl );
    lDist = rectl.xRight-rectl.xLeft;
    lWinCy = rectl.yTop-lDevCaps[CHAR_Y]*4;                /* above buttons*/
    lWinX = 0;
    lWinY = lDevCaps[CHAR_Y]*4;
    /* Set the position of the window */
    WinSetWindowPos( pInfo->hwndSubWindow, HWND_BOTTOM,
                     lWinX,lWinY,lDist,lWinCy,
                     SWP_MOVE | SWP_SIZE | SWP_ACTIVATE );
    return;
}

/***************************************************************************/
/*  createOutData()                                                        */
/*     Formats the order record.                                           */
/***************************************************************************/
static void createOutData( HWND hwnd, ORDERREC* pOrder, VOINFO* pInfo )
{
int i;
size_t szTemp;
double dPrice;
char acBuf[80], acPrice[9], acDate[12], acTime[12];    /* work buffer      */

    /* set order number to the title */
    memset( acOutText, '\0', sizeof(acOutText) );

    /* Set up the out put data ...*/
    /* Titles of Date & Time, CustNum, CustName */
    sprintf( acOutText[1], "%-s", acMsgTable[IDS_VO_DATE] );
    sprintf( acOutText[2], "%-s", acMsgTable[IDS_VO_CUSTNUM] );
    sprintf( acOutText[3], "%-s", acMsgTable[IDS_VO_CUSTNAME] );
    /* get the longest title */
    szTemp = strlen( acOutText[1] );
    szTemp = getMax( szTemp, strlen(acOutText[2]) );
    szTemp = getMax( szTemp, strlen(acOutText[3]) );
    szTemp++;                                  /* for separater in a buffer*/

    /* Check if the title is not too long */
    if( szTemp>MAX_TITLE_LEN )  szTemp = MAX_TITLE_LEN;
    pInfo->subInfo.szMaxTitle = szTemp;

    /* put the date & time data */
    if( formatDate( pOrder->orderDate, acDate, 12 ) == FALSE
     || formatTime( pOrder->orderDate, acTime, 12 ) == FALSE )
    {
      showMessageBox( hwnd, IDS_ERROR_8, IDS_ERROR_CAPTION, MB_ERROR );
    }
    else  sprintf( &acOutText[1][szTemp], "%-12s  %-12s", acDate, acTime );

    /* put the customer info if available. */
    /* There could be the situation that the customer record is not */
    /* available due to version error of custmer record file */
    if( pOrder->pCustRec != NULL )
    {
       /* put the customer number */
       sprintf( &acOutText[2][szTemp], "%#.6ls", pOrder->pCustRec->awcNum );
       /* put the custmer name */
       sprintf( &acOutText[3][szTemp], "%-.40ls", pOrder->pCustRec->awcName );
    }

    /* skip one line for separator */
    /* product title */
    sprintf( acOutText[5],  "%-*s", LEN_PRODNAME-1, acMsgTable[IDS_VO_PRODNAME] );
    sprintf( &acOutText[5][LEN_PRODNAME], "%-*s",
             LEN_PRICE-1, acMsgTable[IDS_VO_UNITPRICE] );
    sprintf( &acOutText[5][LEN_PRODNAME+LEN_PRICE], "%-*s",
             LEN_QTY-1, acMsgTable[IDS_VO_QTY] );
    sprintf( &acOutText[5][LEN_PRODNAME+LEN_PRICE+LEN_QTY], "%-*s",
             LEN_SUB-1, acMsgTable[IDS_VO_SUB] );

    /* skip one line for separator */
    /* each product info. */
    for( i=0; i<MAX_ORDER_PROD && pOrder->prods[i].pProd!=NULL; i++ )
    {
       formatPrice( pOrder->prods[i].pProd->dUnitPrice, acPrice, LEN_PRICE, FALSE );
       dPrice = pOrder->prods[i].sQty * pOrder->prods[i].pProd->dUnitPrice;
       formatPrice( dPrice, acBuf, 16, FALSE );

       sprintf( acOutText[7+i], "%-*.*ls",
                LEN_PRODNAME-1, LEN_PRODNAME-1, pOrder->prods[i].pProd->awcName );
       sprintf( &acOutText[7+i][LEN_PRODNAME], "%*s",
                LEN_PRICE-1, acPrice );
       sprintf( &acOutText[7+i][LEN_PRODNAME+LEN_PRICE], "%*d",
                LEN_QTY-1, pOrder->prods[i].sQty );
       sprintf( &acOutText[7+i][LEN_PRODNAME+LEN_PRICE+LEN_QTY], "%*s",
                LEN_SUB-1, acBuf );
    }

    /* Put total to the last line */
    formatPrice( pOrder->dTotalPrice, acPrice, LEN_TOTAL, TRUE );
    sprintf( acOutText[NUMLINES-1], "%*d", LEN_TOTALQTY-1, pOrder->usTotalQty );
    sprintf( &acOutText[NUMLINES-1][LEN_TOTALQTY], "%*s", LEN_TOTAL-1, acPrice );

    return;
}

/***************************************************************************/
/*  viewSubWinProc()                                                       */
/*     Window procedure for view order's sub window.  This is an actual    */
/*     window which shows the specified order record.                      */
/***************************************************************************/
MRESULT EXPENTRY viewSubWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static HPS hps;
static HWND hwndFrame, hwndVscroll, hwndHscroll;
RECTL rectl;
SUBINFO *pSubInfo;
void showData( HPS hps, HWND hwnd, SUBINFO* pSubInfo );
void setFontToWindow( HPS hps, HWND hwnd, PFATTRS pAttrs );

    pSubInfo = WinQueryWindowPtr( hwnd, 0 );

    switch(msg)
    {
        case WM_CREATE:
        {
        HDC hdc;
        SIZEL sizlPSPage;

            /* create presentation space */
            if( (hdc=WinQueryWindowDC( hwnd )) == NULLHANDLE )
               hdc = WinOpenWindowDC( hwnd );
            sizlPSPage.cx = 0L;
            sizlPSPage.cy = 0L;
            hps = GpiCreatePS( hab, hdc, &sizlPSPage,
                               PU_ARBITRARY | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC );

            hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
            hwndVscroll = WinWindowFromID( hwndFrame, FID_VERTSCROLL );
            hwndHscroll = WinWindowFromID( hwndFrame, FID_HORZSCROLL );
            WinSendMsg( hwnd, WM_USER_NEW, 0L, 0L );
            WinSendMsg( hwndHelpInstance, HM_SET_ACTIVE_WINDOW,
                        MPFROMLONG(WinQueryWindow(hwndFrame, QW_PARENT)),
                        MPFROMLONG(WinQueryWindow(hwndFrame, QW_PARENT)) );
            return(MRESULT)(FALSE);
        }

        case WM_HSCROLL:
        {
        SHORT sHscrollInc, sHscrollPos, sHscrollMax;

            sHscrollPos = pSubInfo->sHscrollPos;
            sHscrollMax = pSubInfo->sHscrollMax;

            switch (SHORT2FROMMP (mp2))
            {
                 case SB_LINELEFT:
                      sHscrollInc = -(pSubInfo->cxCaps)/8 ;
                      break ;

                 case SB_LINERIGHT:
                      sHscrollInc = pSubInfo->cxCaps/8 ;
                      break ;

                 case SB_PAGELEFT:
                      sHscrollInc = -(pSubInfo->cxCaps) ;
                      break ;

                 case SB_PAGERIGHT:
                      sHscrollInc = pSubInfo->cxCaps ;
                      break ;

                 case SB_SLIDERPOSITION:
                      sHscrollInc = SHORT1FROMMP (mp2) - sHscrollPos;
                      break ;

                 default:
                      sHscrollInc = 0 ;
                      break ;
            }
            sHscrollInc = getMax (-sHscrollPos,
                          getMin (sHscrollInc, sHscrollMax - sHscrollPos)) ;

            if( sHscrollInc != 0 )
            {
                sHscrollPos += sHscrollInc ;
                WinScrollWindow (hwnd, -sHscrollInc,
                                 0L,NULL, NULL, 0L, &rectl, 0 );

                pSubInfo->sHscrollPos = sHscrollPos;
                WinSendMsg( hwndHscroll,SBM_SETPOS,
                            MPFROMSHORT( sHscrollPos ), NULL) ;
                WinInvalidateRect( hwnd, &rectl, FALSE );
            }
            break;
        }

        case WM_VSCROLL:
        {
        SHORT sVscrollInc, sVscrollPos, sVscrollMax;
        SHORT cyChar = lWinFont[CHAR_Y];

            sVscrollPos = pSubInfo->sVscrollPos;
            sVscrollMax = pSubInfo->sVscrollMax;

            switch (SHORT2FROMMP (mp2))
            {
                 case SB_LINEUP:
                      sVscrollInc = -1 ;
                      break ;

                 case SB_LINEDOWN:
                      sVscrollInc = 1 ;
                      break ;

                 case SB_PAGEUP:
                      sVscrollInc = min (-1, -(pSubInfo->cyClient)/cyChar) ;
                      break ;

                 case SB_PAGEDOWN:
                      sVscrollInc = max (1, (pSubInfo->cyClient)/ cyChar) ;
                      break ;

                 case SB_SLIDERTRACK:
                      sVscrollInc = SHORT1FROMMP (mp2) - sVscrollPos;
                      break ;

                 default:
                      sVscrollInc = 0 ;
                      break ;
            }

            /* Do we have to scroll vertically? */
            sVscrollInc = getMax( -sVscrollPos,
                                  getMin( sVscrollInc, sVscrollMax - sVscrollPos) );

            if( sVscrollInc != 0 )
            {
                sVscrollPos += sVscrollInc ;
                WinScrollWindow( hwnd,0,
                                 cyChar * sVscrollInc,
                                 NULL,NULL, NULLHANDLE, NULL, SW_INVALIDATERGN );
                pSubInfo->sVscrollPos = sVscrollPos;
                WinSendMsg( hwndVscroll,SBM_SETPOS,
                            MPFROMSHORT (sVscrollPos), NULL) ;
                WinUpdateWindow( hwnd );
            }

            break;
        }

        case WM_PAINT:
        {
            WinBeginPaint( hwnd, hps, (PRECTL) &rectl );
            WinFillRect( hps, (PRECTL) &rectl, SYSCLR_WINDOW );/* paint the rectangle*/
            showData( hps, hwnd, pSubInfo );
            WinEndPaint( hps );
            break;
        }

        case WM_SIZE:
          /* get the window rectangle */
          WinQueryWindowRect( hwnd, &rectl );
          pSubInfo->cxCaps = (SHORT)(rectl.xRight-rectl.xLeft);
          pSubInfo->cyClient = (SHORT)(rectl.yTop-rectl.yBottom);
          break;

        case WM_CLOSE:
            GpiDestroyPS( hps );
            WinDestroyWindow( hwndFrame );
            break;

        case WM_USER_FONT:
            setFontToWindow( hps, hwnd, PVOIDFROMMP(mp1) );
            pSubInfo->sHscrollMax = (NUMCHARS-1)*lWinFont[CHAR_X];
            WinSendMsg( hwnd, WM_USER_SETSCROLL,
                        MPFROMLONG(hwndHscroll),
                        MPFROM2SHORT(pSubInfo->sHscrollPos,
                                     pSubInfo->sHscrollMax) );
            break;

        case WM_USER_SETSCROLL:                  /* set max & pos of scroll*/
            /* mp1 = scroll bar window handle.*/
            /* mp2 = short1: position short2: max value */
            WinSendMsg( LONGFROMMP(mp1), SBM_SETSCROLLBAR,
                        MPFROMSHORT(SHORT1FROMMP(mp2)),
                        MPFROM2SHORT(0, SHORT2FROMMP(mp2)) );
            break;

        case WM_USER_NEW:                                /* show new record*/
        {

            if( pSubInfo == NULL )                            /* first path*/
            {
               WinSendMsg( hwnd, WM_USER_SETSCROLL,
                           MPFROMLONG(hwndVscroll),
                           MPFROM2SHORT(0, NUMLINES-1) );
               WinSendMsg( hwnd, WM_USER_SETSCROLL,
                           MPFROMLONG(hwndHscroll),
                           MPFROM2SHORT(0, (NUMCHARS-1)*lDevCaps[CHAR_X]) );
            }
            else
            {
               pSubInfo->sHscrollPos = pSubInfo->sVscrollPos = 0;
               pSubInfo->sHscrollMax = (NUMCHARS-1)*lWinFont[CHAR_X];
               pSubInfo->sVscrollMax = NUMLINES-1;

               WinSendMsg( hwnd, WM_USER_SETSCROLL,
                           MPFROMLONG(hwndVscroll),
                           MPFROM2SHORT(0, pSubInfo->sVscrollMax) );
               WinSendMsg( hwnd, WM_USER_SETSCROLL,
                           MPFROMLONG(hwndHscroll),
                           MPFROM2SHORT(0, pSubInfo->sHscrollMax) );
            }
            break;
        }
        default:
            return(WinDefWindowProc(hwnd,msg,mp1,mp2));
    }

return(FALSE);
}

/***************************************************************************/
/*  showData()                                                             */
/*     Write the order record to the specified presentation space.         */
/***************************************************************************/
static void showData( HPS hps, HWND hwnd, SUBINFO* pSubInfo )
{
int i, iTop, iVCount, iLeft;
char *p;
size_t szMax = pSubInfo->szMaxTitle;
enum outType eOutType;
POINTL ptl;
RECTL rclClient;
CHARBUNDLE cbnd;

    /* Write to the presentation space */
    WinQueryWindowRect( hwnd, &rclClient );
    ptl.y = rclClient.yTop;
    iVCount = ptl.y/lWinFont[CHAR_Y];    /* how many lines can be displayed*/

    iTop = pSubInfo->sVscrollPos;/* line number which should be shown at the top*/
    iLeft = -(pSubInfo->sHscrollPos);

    for( i=0; i<iVCount; i++ )
    {
       if( (iTop+i) > NUMLINES )     break;
       ptl.y -= lWinFont[CHAR_Y];

       switch( iTop+i )
       {
          case 0:                                            /* insert blan*/
          case 4:
            break;

          case 1:                                              /* date&time*/
          case 2:                                        /* customer number*/
          case 3:                                          /* customer name*/
            cbnd.lColor = CLR_BLUE;
            GpiSetAttrs( hps, PRIM_CHAR, CBB_COLOR, 0, &cbnd );/* Set the color*/

            ptl.x = iLeft+lWinFont[CHAR_X]*2;     /* two blenks at the left side*/
            GpiCharStringAt( hps, &ptl,
                             strlen(acOutText[iTop+i]),acOutText[iTop+i] );

            GpiSetAttrs( hps, PRIM_CHAR, CBB_COLOR, CBB_COLOR, &cbnd );/* Restore the color*/

            ptl.x = iLeft+lWinFont[CHAR_X]*(szMax+4);
            GpiCharStringAt( hps, &ptl, strlen(&acOutText[iTop+i][szMax]),
                                               &acOutText[iTop+i][szMax]);
            break;

          case 5:                                                  /* title*/
          case 7:                                              /* product 1*/
          case 8:                                              /* product 2*/
          case 9:                                              /* product 3*/
            eOutType = outProd;
            ptl.x = iLeft+lWinFont[CHAR_X]*POS_PRODNAME;
            p = acOutText[iTop+i];
            for( eOutType=outProd; eOutType<=outTotal; eOutType++ )
            {
               switch( eOutType)
               {
                 case outPrice:
                   ptl.x = iLeft+lWinFont[CHAR_X]*POS_PRICE;
                   break;

                 case outQty:
                   ptl.x = iLeft+lWinFont[CHAR_X]*POS_QTY;
                   break;

                 case outTotal:
                   ptl.x = iLeft+lWinFont[CHAR_X]*POS_SUB;
                   break;
               }
               GpiCharStringAt( hps, &ptl, strlen(p), p );
               p+= (strlen(p)+1);
            }
            break;

          case 6:                                              /* separater*/
          case 10:
            ptl.y += lWinFont[CHAR_Y]/2;
            if( (iTop+i) == 6 )  ptl.x = iLeft+lWinFont[CHAR_X]*POS_PRODNAME;
            else                 ptl.x = iLeft+lWinFont[CHAR_X]*POS_TOTALQTY;
            GpiMove( hps, &ptl );
            ptl.x = iLeft+lWinFont[CHAR_X]*(POS_SUB+LEN_SUB-1);
            GpiLine( hps, &ptl );
            ptl.y -= lWinFont[CHAR_Y]/2;
            break;

          case 11:                                                 /* total*/
            ptl.x = iLeft+lWinFont[CHAR_X]*POS_TOTALQTY;
            p = acOutText[iTop+i];
            GpiCharStringAt( hps, &ptl, strlen(p), p );
            p+= (strlen(p)+1);
            ptl.x = iLeft+lWinFont[CHAR_X]*POS_TOTAL;
            GpiCharStringAt( hps, &ptl, strlen(p), p );
            break;

          default:
            break;
       }
    }                                                            /* End for*/

    return;
}

/***************************************************************************/
/*  callFontDlg()                                                          */
/*     Initialize and call the font dialog.                                */
/***************************************************************************/
void callFontDlg( HPS hps, HWND hwnd, PFONTDLG pFontDlg )
{
FATTRS faAttrs = pFontDlg->fAttrs;
FIXED fxSzFont = pFontDlg->fxPointSize;
char szFamilyname[FACESIZE];
void setVectorFontSize(HPS hpsClient, FIXED fxPointSize, PFATTRS pfattrs);

    /* Setup FontDlg parameter */
    memset( pFontDlg, 0, sizeof(FONTDLG) );
    memset( szFamilyname, 0, sizeof(FACESIZE) );

    pFontDlg->cbSize = sizeof( FONTDLG );
    pFontDlg->hpsScreen = hps;
    pFontDlg->pszFamilyname = szFamilyname;
    pFontDlg->usFamilyBufLen = sizeof(szFamilyname);
    pFontDlg->fl = FNTS_CENTER | FNTS_INITFROMFATTRS;

    pFontDlg->clrFore = CLR_NEUTRAL;
    pFontDlg->clrBack = CLR_BACKGROUND;
    pFontDlg->pszPreview = acMsgTable[IDS_VO_PREVIEW];

    pFontDlg->fxPointSize = fxSzFont;
    pFontDlg->fAttrs = faAttrs;

    if( WinFontDlg (HWND_DESKTOP, hwnd, pFontDlg ) == FALSE )
    {
       WinAlarm(HWND_DESKTOP, WA_ERROR);
    }
    else
    {
       if( pFontDlg->lReturn == DID_OK )
       {
           if ( pFontDlg->fAttrs.fsFontUse == FATTR_FONTUSE_OUTLINE )
           {
              pFontDlg->fAttrs.fsFontUse |= FATTR_FONTUSE_TRANSFORMABLE;
              pFontDlg->fAttrs.lMaxBaselineExt = 0;
              pFontDlg->fAttrs.lAveCharWidth = 0;
              setVectorFontSize(hps, pFontDlg->fxPointSize, &pFontDlg->fAttrs);
           }
           else
           {
              lWinFont[CHAR_X] = pFontDlg->fAttrs.lAveCharWidth;
              lWinFont[CHAR_Y] = pFontDlg->fAttrs.lMaxBaselineExt
                               + pFontDlg->lExternalLeading;
           }
       }
    }
    return;
}

/***************************************************************************/
/*  setVectorFontSize()                                                    */
/*     Since a vector font itself does not have the specific font size in  */
/*     pel, caliculates the size from point and device resolution.         */
/***************************************************************************/
void setVectorFontSize(HPS hpsClient, FIXED fxPointSize, PFATTRS pfattrs)
{
HPS   hps;
HDC   hDC;
LONG  lxFontResolution;
LONG  lyFontResolution;
SIZEF sizef;
POINTL aptl[TXTBOX_COUNT];


  hps = WinGetScreenPS(HWND_DESKTOP);

  hDC = GpiQueryDevice(hps);
  DevQueryCaps( hDC, CAPS_HORIZONTAL_FONT_RES, (LONG)1, &lxFontResolution);
  DevQueryCaps( hDC, CAPS_VERTICAL_FONT_RES, (LONG)1, &lyFontResolution);

  sizef.cx = (FIXED)(((fxPointSize) / 72 ) * lxFontResolution );
  sizef.cy = (FIXED)(((fxPointSize) / 72 ) * lyFontResolution );
  lWinFont[CHAR_Y] = FIXEDINT(sizef.cy);                    /* get int part*/

  /* instead of average font width, use white-space width */
  GpiQueryTextBox( hps, strlen(" "), " ", TXTBOX_COUNT, aptl );
  lWinFont[CHAR_X] = aptl[TXTBOX_TOPRIGHT].x-aptl[TXTBOX_TOPLEFT].x;

  GpiSetCharBox( hpsClient, &sizef );
  WinReleasePS(hps);
}

/***************************************************************************/
/*  setFontToClient()                                                      */
/*     Sets the font specified by pAttrs to the presentation space.        */
/***************************************************************************/
#define ID_LCID            30L
void setFontToWindow( HPS hps, HWND hwnd, PFATTRS pAttrs )
{
 LONG rc;

    GpiSetCharSet( hps, 0L );
    GpiSetCharMode( hps, CM_MODE1 );
    GpiDeleteSetId( hps, ID_LCID );

    rc = GpiCreateLogFont( hps, NULL, ID_LCID, pAttrs );
    if( rc == FONT_MATCH )
    {
       GpiSetCharSet( hps, ID_LCID );                 /* set the font to PS*/
       WinInvalidateRect( hwnd, NULL, TRUE );             /* cause WM_PAINT*/
    }
    else if( rc == FONT_DEFAULT )
    {
       showMessageBox( hwnd, IDS_WARNING_6,
                       IDS_WARNING_CAPTION, MB_WARNING );
    }
    else
    {
       showMessageBox( hwnd, IDS_ERROR_6,
                       IDS_ERROR_CAPTION, MB_ERROR );
    }

}
