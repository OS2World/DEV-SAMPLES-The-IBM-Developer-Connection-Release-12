/***************************************************************************/
/* XPRMORDR.C    XPG4 Primer for OS/2 WARP - V1.0       11/15/95           */
/*               Window procedures and related functions for order records */
/*               manipulation.                                             */
/***************************************************************************/
#define INCL_WIN
#define INCL_DOS
#define INCL_GPI
#define INCL_NLS
#include <os2.h>                       /* System Include File              */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <monetary.h>
#include <time.h>
#include <limits.h>

#include "xprmos2.h"                   /* Application Include File         */
#include "xprmres.h"                   /* Application Include NLS File     */
#include "xprmdata.h"                  /* Application Data Structure file  */

/**************************************/
/* function prototypes - external     */
/**************************************/
/* PM related functions */
extern void putWindowToCenter( HWND hwnd );
extern void associateHelp( HWND hwnd );

/* functions related to records handling */
extern PRODREC* getProdRec( char* pszKey, enum proditem e_category );
extern CUSTREC* getCustRec( char* pszKey, enum custitem e_category );
extern BOOL writeAnOrder( ORDERREC* pOrder );
extern BOOL getNewOrderNum( char* pszNewNumber );

/* Common routines */
extern BOOL isDigitString( char* pszString );
extern void setDlgItemWcs( HWND hwnd, ULONG ulItemId, wchar_t* wcsText);
extern BOOL formatDate( time_t timeval, char* pszBuf, size_t len );
extern BOOL formatTime( time_t timeval, char* pszBuf, size_t len );
extern void formatPrice( double dPrice, char* pszBuf, size_t len, BOOL flCurrency );

/**************************************/
/* External Variables                 */
/**************************************/
extern HAB hab;
extern HMODULE hModRsrc;
extern HWND hwndHelpInstance;
extern char acMsgTable[][MAX_MSG_LEN];
extern BOOL flDBCScapability;
/* device information */
#define CHAR_X  0
#define CHAR_Y  1
#define DISP_X  2
#define DISP_Y  3
extern long lDevCaps[4];

/***************************************************************************/
/*  orderEntryDlgProc()                                                    */
/*     Order entry dialog's procedure.                                     */
/***************************************************************************/
MRESULT EXPENTRY orderEntryDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 ULONG ulInvalidField = OE_EF_QTY1;
 static ORDERREC order;
 void initOrderEntryDlg( HWND hwnd, ORDERREC* pOrder, char* pszEmpNumber );
 void procLNSELECTonLIPRODUCTS( HWND hwnd, ORDERREC* pOrder );
 void procCBNLBSELECTonCBCUSTNAME( HWND hwnd, ORDERREC* pOrder );
 void procENCHANGEonEFQTY( HWND hwnd, ORDERREC* pOrder, ULONG ulFieldId );
 void setSum( HWND hwnd, ORDERREC* pOrder );
 ULONG checkQty( HWND hwnd, ORDERREC* pOrder );

    switch(msg)
    {
        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1))
            {
                case DID_OK:
                    if( order.prods[0].pProd == NULL )
                    {
                       showMessageBox( hwnd, IDS_WARNING_3,
                                       IDS_WARNING_CAPTION, MB_WARNING );
                       WinSetFocus(HWND_DESKTOP, WinWindowFromID (hwnd, OE_LI_PRODUCTS)  );
                       break;
                    }

                    if( (ulInvalidField=checkQty( hwnd, &order )) != 0 )
                    {
                       showMessageBox( hwnd, IDS_WARNING_3,
                                       IDS_WARNING_CAPTION, MB_WARNING );
                       clearField( hwnd, ulInvalidField );
                       break;
                    }

                    if( writeAnOrder( &order ) == TRUE )
                    {
                       showMessageBox( hwnd, IDS_INFO_1, IDS_INFO_CAPTION, MB_INFORMATION );
                    }
                    else
                    {
                       showMessageBox( hwnd, IDS_ERROR_1, IDS_ERROR_CAPTION, MB_ERROR );
                    }
                    WinDismissDlg (hwnd,DID_OK);
                    break;

                case DID_CANCEL:
                    WinDismissDlg (hwnd,DID_CANCEL);
                    break;
            }
            break;

        case WM_CONTROL:

            switch (SHORT1FROMMP(mp1))
            {
                case OE_LI_PRODUCTS:
                    if( SHORT2FROMMP(mp1) == LN_SELECT )
                    {
                       procLNSELECTonLIPRODUCTS( hwnd, &order );
                    }
                    break;

                case OE_CB_CUSTNAME:
                    if( SHORT2FROMMP(mp1) == CBN_LBSELECT )
                    {
                       procCBNLBSELECTonCBCUSTNAME( hwnd, &order );
                    }
                    break;

                case OE_EF_QTY1:
                case OE_EF_QTY2:
                case OE_EF_QTY3:
                    if( SHORT2FROMMP(mp1) == EN_CHANGE )
                    {
                       procENCHANGEonEFQTY( hwnd, &order, SHORT1FROMMP(mp1) );
                       setSum( hwnd, &order );
                    }
                    break;
            }
            break;

        case WM_INITDLG:
            /* Associates help instance */
            associateHelp( hwnd );
            initOrderEntryDlg( hwnd, &order, (char*)PVOIDFROMMP(mp2) );
            break;

        default:
            return(WinDefDlgProc(hwnd,msg,mp1,mp2));
            break;
    }
    return(FALSE);
}

/***************************************************************************/
/*  checkQty()                                                             */
/*     Checks if the Qty field is empty thoug ha product was selected.     */
/*     Returns the field id if such an empty field is found.  Otherwise,   */
/*     returns 0.                                                          */
/***************************************************************************/
static ULONG checkQty( HWND hwnd, ORDERREC* pOrder )
{
int i;

    for( i=0; i<MAX_ORDER_PROD; i++ )
    {
       if( pOrder->prods[i].pProd != NULL
        && pOrder->prods[i].sQty == 0 )
       {
          return (ULONG)(OE_EF_QTY1+i);
       }
    }
    return 0;
}

/***************************************************************************/
/*  procLNSELECTonLIPRODUCTS()                                             */
/*     Procedure for LN_SELECT on the products list box.                   */
/*     If selected item exists, the bound product record is queried.       */
/*     If the maximum number of selection have made, the next selection    */
/*     is deselected.                                                      */
/***************************************************************************/
static void procLNSELECTonLIPRODUCTS( HWND hwnd, ORDERREC* pOrder )
{
int iNumItems, i;
ULONG ulProdNameId = OE_EF_ORDER1;
SHORT sCurIndex;
ULONG itemHandle;
char acBuf[32];
int numSelectedItems( HWND hwnd, ORDERREC* pOrder );
void setSum( HWND hwnd, ORDERREC* pOrder );

   iNumItems = numSelectedItems( hwnd, pOrder );

   if( iNumItems > MAX_ORDER_PROD )
   {
       /* get the index of item that cursor is placed */
       sCurIndex = SHORT1FROMMR( WinSendDlgItemMsg( hwnd, OE_LI_PRODUCTS,
                                                    LM_QUERYSELECTION,
                                                    (MPARAM) LIT_CURSOR,
                                                    (MPARAM) 0L) );
       /* deselect it */
       WinSendDlgItemMsg( hwnd, OE_LI_PRODUCTS,
                          LM_SELECTITEM,
                          MPFROMSHORT(sCurIndex),
                          MPFROMSHORT(FALSE) );

       showMessageBox( hwnd, IDS_WARNING_5, IDS_WARNING_CAPTION, MB_WARNING );
       return;
   }

   for( i=0; i<iNumItems; i++ )
   {
      /* get the prodrec handle */
      if( pOrder->prods[i].pProd == NULL )
      {
         itemHandle = LONGFROMMR( WinSendDlgItemMsg( hwnd, OE_LI_PRODUCTS,
                                                     LM_QUERYITEMHANDLE,
                                                     MPFROMSHORT(pOrder->prods[i].sItemIndex),
                                                     (MPARAM) 0L ) );
         if( itemHandle == 0L )
         {
             showMessageBox( hwnd, IDS_ERROR_2, IDS_ERROR_CAPTION, MB_ERROR );
             break;
         }
         else
             pOrder->prods[i].pProd = (PRODREC*)(PVOID)itemHandle;
      }

      /* set product name */
      setDlgItemWcs( hwnd, ulProdNameId, pOrder->prods[i].pProd->awcName );
      /* set Qty if available */
      sprintf( acBuf, "%d", pOrder->prods[i].sQty );
      WinSetDlgItemText( hwnd, OE_EF_QTY1+i, acBuf );
      ulProdNameId++;
   }                                                     /* end of for loop*/

   for( i=iNumItems; i<MAX_ORDER_PROD; i++ )
   {
      /* clear the entry field */
      WinSetDlgItemText( hwnd, ulProdNameId, "" );
      WinSetDlgItemText( hwnd, OE_EF_QTY1+i, "0" );           /* reset Qty.*/
      ulProdNameId++;
   }
   return;
}

/***************************************************************************/
/*  numSelectedItems()                                                     */
/*     Queries the selected items index and returns the number of selected */
/*     items.  If the maximum number of selections have been made,  the    */
/*     last selection is ignored.                                          */
/***************************************************************************/
static int numSelectedItems( HWND hwnd, ORDERREC* pOrder )
{
 ORDERPROD newOrders[MAX_ORDER_PROD+1];
 SHORT sItemIndex, sNewComer, indexList[MAX_ORDER_PROD+1];
 int iCnt=0;
 int i, j, k;

   memset( &newOrders, '\0', sizeof(newOrders) );
   sItemIndex = SHORT1FROMMR( WinSendDlgItemMsg( hwnd, OE_LI_PRODUCTS,
                                                 LM_QUERYSELECTION,
                                                 (MPARAM) LIT_FIRST,
                                                 (MPARAM) 0L) );

   sNewComer = LIT_NONE;
   while( sItemIndex != LIT_NONE && iCnt <= MAX_ORDER_PROD )
   {
      indexList[iCnt] = sItemIndex;

      for( i=0; i<MAX_ORDER_PROD; i++ )
      {
        if( pOrder->prods[i].sItemIndex == sItemIndex ) break;
      }

      if( i==MAX_ORDER_PROD ) sNewComer = sItemIndex;
      iCnt++;
      /* get next data */
      sItemIndex = SHORT1FROMMR( WinSendDlgItemMsg( hwnd, OE_LI_PRODUCTS,
                                                    LM_QUERYSELECTION,
                                                    MPFROMSHORT(sItemIndex),
                                                    (MPARAM) 0L) );
   }

   for( i=0, k=0; i<iCnt; i++)
   {
      /* If too many items are selected, the newcomer is deleted */
      if( indexList[i] == sNewComer && iCnt > MAX_ORDER_PROD )  continue;
      else
      {
        newOrders[k].sItemIndex = indexList[i];
        for( j=0; j<MAX_ORDER_PROD; j++ )
        {
          if( indexList[i] == pOrder->prods[j].sItemIndex )
          {
             newOrders[k].sQty = pOrder->prods[j].sQty;
             newOrders[k].pProd = pOrder->prods[j].pProd;
             break;
          }
        }
        k++;
      }
   }

   memcpy( pOrder->prods, &newOrders, sizeof(ORDERPROD)*MAX_ORDER_PROD );
   return iCnt;
}

/***************************************************************************/
/*  procENCHANGEonEFQTY()                                                  */
/*     Procedure for EN_CHANGE message on Qty. entry fields.  If the field */
/*     is empty, the qty is assumed 0.  If the contents of the field does  */
/*     not consists of digit characters, the qty is assumed 0.  Otherwise, */
/*     the price is calculated and set to the price field after formatting */
/*     with the strfmon().                                                 */
/***************************************************************************/
static void procENCHANGEonEFQTY( HWND hwnd, ORDERREC* pOrder, ULONG ulFieldId )
{
int id;
CHAR buf[8], acSum[32];                /* work buffer                      */
double dSum = 0;
ORDERPROD *pOrderProd;

    id = ulFieldId - OE_EF_QTY1;
    pOrderProd = &pOrder->prods[id];
    if( pOrderProd->pProd != NULL )
    {
       WinQueryDlgItemText( hwnd, ulFieldId, 7, buf );
       if( strlen(buf) == 0 )
       {
          WinSetDlgItemText( hwnd, ulFieldId, "0" );
       }
       else if( isDigitString(buf) == FALSE )
       {
          showMessageBox( hwnd, IDS_WARNING_4, IDS_WARNING_CAPTION, MB_WARNING );
          WinSetDlgItemText( hwnd, ulFieldId, "0" );
       }
       else
       {
          pOrderProd->sQty = atoi( buf );
          dSum = pOrderProd->pProd->dUnitPrice * pOrderProd->sQty;
       }
    }
    /* The following code is to skip the strfmon's bug */
    if( dSum == 0 )  strcpy( acSum, "0" );
    else             formatPrice( dSum, acSum, 32, FALSE );
    WinSetDlgItemText( hwnd, OE_EF_SUBSUM1+id, acSum);
}

/***************************************************************************/
/*  setSum()                                                               */
/*     Calculate total qty. and price of the order.                        */
/***************************************************************************/
static void setSum( HWND hwnd, ORDERREC* pOrder )
{
int i;
CHAR  acBuf[32];                       /* work buffer                      */
USHORT usTotalQty=0;
double dSum, dTotalPrice=0;
ORDERPROD *pOrderProd;

   /*!! Assuming Control values are continuos !!*/
   pOrderProd = &pOrder->prods[0];
   for( i=0; i<MAX_ORDER_PROD; i++, pOrderProd++ )
   {
      if( pOrderProd->pProd == NULL )  continue;

      usTotalQty += pOrderProd->sQty;
      dSum = pOrderProd->pProd->dUnitPrice * pOrderProd->sQty;
      dTotalPrice += dSum;
   }
   pOrder->dTotalPrice = dTotalPrice;
   pOrder->usTotalQty = usTotalQty;

   sprintf( acBuf, "%d", usTotalQty );
   WinSetDlgItemText( hwnd, OE_EF_TOTAL_QTY, acBuf );

   formatPrice( dTotalPrice, acBuf, sizeof(acBuf), TRUE );
   WinSetDlgItemText( hwnd, OE_EF_SUM, acBuf );

   return;
}

/***************************************************************************/
/*  procCBNLBSELECTonCBCUSTNAME()                                          */
/*     Procedure for CBN_LBSELECT message on customer name combo box.      */
/*     Queries the item and finds the bound customer record.  Then set the */
/*     customer number to the customer number field.                       */
/***************************************************************************/
static void procCBNLBSELECTonCBCUSTNAME( HWND hwnd, ORDERREC* pOrder )
{
SHORT sItemIndex;                      /* Ordinal of item within list      */
CHAR szlbContents[256];                /* Temporary var for list item text */
ULONG itemHandle;
CUSTREC *pCust;
char acCustNum[8];

   /* Get index of first selected item in listbox OE_CB_CUSTNAME  */
   sItemIndex = SHORT1FROMMR( WinSendDlgItemMsg( hwnd, OE_CB_CUSTNAME,
                                                 LM_QUERYSELECTION,
                                                 (MPARAM) LIT_FIRST,
                                                 (MPARAM) 0L) );
   /* Process error if none was selected */
   if(sItemIndex == LIT_NONE)
   {
       showMessageBox( hwnd, IDS_WARNING_2, IDS_WARNING_CAPTION, MB_WARNING );
   }
   else
   {
      /* Get text into szlbContents, if its length > 0 */
      if( WinSendDlgItemMsg( hwnd, OE_CB_CUSTNAME,
                             LM_QUERYITEMTEXT,
                             MPFROM2SHORT(sItemIndex,255),
                             (MPARAM) szlbContents ) == 0 )
      {
          showMessageBox( hwnd, IDS_WARNING_3, IDS_WARNING_CAPTION, MB_WARNING );
      }
      else
      {
         /* get the custrec handle */
         itemHandle = LONGFROMMR( WinSendDlgItemMsg( hwnd, OE_CB_CUSTNAME,
                                                     LM_QUERYITEMHANDLE,
                                                     MPFROMSHORT(sItemIndex),
                                                     (MPARAM) 0L ) );
         /* Set the text of the entry field OE_EF_CUSTNUM */
         if( itemHandle == 0L )
         {
             showMessageBox( hwnd, IDS_ERROR_2, IDS_ERROR_CAPTION, MB_ERROR );
         }
         else
         {
            pCust = (CUSTREC*)(PVOID)itemHandle;
            setDlgItemWcs( hwnd, OE_EF_CUSTNUM, pCust->awcNum );
            pOrder->pCustRec = pCust;
         }
      }
   }
}

/***************************************************************************/
/* initOrderEntryDlg()                                                     */
/*     Procedure for WM_INITDLG message.                                   */
/*     Sets the text limit for each entry fields, sets the customer names  */
/*     and products information to each list (combo) box. Sets the fixed   */
/*     pitch font to the products list box so that the column looks        */
/*     straihgt.                                                           */
/***************************************************************************/
static void initOrderEntryDlg( HWND hwnd, ORDERREC* pOrder, char* pszEmpNumber )
{
PRODREC *pProd;
CUSTREC *pCust;
time_t aTime;
struct tm* pTime;
char acTime[32], acDate[32];
int i, ch;
void setFixedFont( HWND hwnd, ULONG ulTargetItemId );

    /* null clear ORDERREC ... */
    memset( pOrder, '\0', sizeof(ORDERREC) );
    /* ... except item index of products */
    for( i=0; i<MAX_ORDER_PROD; i++ )
    {
       pOrder->prods[i].sItemIndex = LIT_NONE;
    }

    if( getNewOrderNum( pOrder->acOrderNum ) != TRUE )
    {
       strcpy( pOrder->acOrderNum, "" );              /* ensure null string*/
    }
    WinSetDlgItemText( hwnd, OE_EF_ORDERNUM, pOrder->acOrderNum );

    strcpy( pOrder->acEmpNum, pszEmpNumber );

    /* set the text limits of the entry fields which accept user input */
    WinSendDlgItemMsg( hwnd, OE_EF_QTY1,
                       EM_SETTEXTLIMIT, MPFROMSHORT(4), 0L );
    WinSendDlgItemMsg( hwnd,OE_EF_QTY2,
                       EM_SETTEXTLIMIT, MPFROMSHORT(4), 0L );
    WinSendDlgItemMsg( hwnd, OE_EF_QTY3,
                       EM_SETTEXTLIMIT, MPFROMSHORT(4), 0L );

    /* expand the text limits of product records to 80 byte */
    WinSendDlgItemMsg( hwnd, OE_EF_ORDER1,
                       EM_SETTEXTLIMIT, MPFROMSHORT(80), 0L );
    WinSendDlgItemMsg( hwnd, OE_EF_ORDER2,
                       EM_SETTEXTLIMIT, MPFROMSHORT(80), 0L );
    WinSendDlgItemMsg( hwnd, OE_EF_ORDER3,
                       EM_SETTEXTLIMIT, MPFROMSHORT(80), 0L );

    /* set date/time */
    aTime = time( NULL );
    if( formatDate( aTime, acDate, sizeof(acDate) ) == FALSE )
    {
       showMessageBox( hwnd, IDS_ERROR_8, IDS_ERROR_CAPTION, MB_ERROR );
    }
    else  WinSetDlgItemText( hwnd, OE_EF_ORDERDATE, acDate );

    if( formatTime( aTime, acTime, sizeof(acTime) ) == FALSE )
    {
       showMessageBox( hwnd, IDS_ERROR_8, IDS_ERROR_CAPTION, MB_ERROR );
    }
    else  WinSetDlgItemText( hwnd, OE_EF_ORDERTIME, acTime );

    memcpy( &pOrder->orderDate, &aTime, sizeof(time_t) );

    /* set Qty. and total/subtotals 0 */
    WinSetDlgItemText( hwnd, OE_EF_QTY1, "0" );
    WinSetDlgItemText( hwnd, OE_EF_QTY2, "0" );
    WinSetDlgItemText( hwnd, OE_EF_QTY3, "0" );
    WinSetDlgItemText( hwnd, OE_EF_SUBSUM1, "0" );
    WinSetDlgItemText( hwnd, OE_EF_SUBSUM2, "0" );
    WinSetDlgItemText( hwnd, OE_EF_SUBSUM3, "0" );
    WinSetDlgItemText( hwnd, OE_EF_TOTAL_QTY, "0" );
    WinSetDlgItemText( hwnd, OE_EF_SUM, "0" );

    pProd = getProdRec( NULL, 0 );
    if( pProd == NULL )
    {
       showMessageBox( hwnd, IDS_ERROR_3, IDS_ERROR_CAPTION, MB_ERROR );
    }
    else
    {
     SHORT sIndex;
     double dPrice;
     char acProdString[PROD_NUM_LEN+PROD_NUM_LEN*MB_LEN_MAX+8];
     char acPrice[16];

       /* Insert products into listbox OE_LI_PRODUCTS */
       while( pProd != NULL )
       {
          dPrice = pProd->dUnitPrice;
          formatPrice( dPrice, acPrice, sizeof(acPrice), TRUE );
          sprintf( acProdString, "%s %-#*.*ls %s",
                   pProd->acNum,
                   PROD_NAME_LEN, PROD_NAME_LEN, pProd->awcName,
                   acPrice );

          /* Set pProd as the index handle */
          sIndex = SHORT1FROMMR( WinSendDlgItemMsg( hwnd, OE_LI_PRODUCTS,
                                                    LM_INSERTITEM,
                                                    MPFROMSHORT(LIT_END),
                                                    MPFROMP(acProdString) ) );
          WinSendDlgItemMsg( hwnd, OE_LI_PRODUCTS,
                             LM_SETITEMHANDLE,
                             MPFROMSHORT(sIndex),
                             MPFROMLONG((ULONG)pProd) );
          pProd=pProd->next;
       }
    }                                                             /* End if*/

    pCust = getCustRec( NULL, 0 );
    if( pCust == NULL )
    {
       showMessageBox( hwnd, IDS_ERROR_4, IDS_ERROR_CAPTION, MB_ERROR );
    }
    else
    {
    char acCustName[CUST_NAME_LEN*MB_LEN_MAX];
    SHORT sIndex;
       /* Insert customer name to listbox OE_CB_CUSTNAME */
       while( pCust != NULL )
       {
          sprintf( acCustName, "%-#*ls",
                   CUST_NAME_LEN, pCust->awcName );
          /* Set pCust as the index handle */
          sIndex = SHORT1FROMMR( WinSendDlgItemMsg( hwnd, OE_CB_CUSTNAME,
                                                    LM_INSERTITEM,
                                                    MPFROMSHORT(LIT_END),
                                                    MPFROMP(acCustName) ) );
          WinSendDlgItemMsg( hwnd, OE_CB_CUSTNAME,
                             LM_SETITEMHANDLE,
                             MPFROMSHORT(sIndex),
                             MPFROMLONG((ULONG)pCust) );
          pCust=pCust->next;
       }
    }
    /* select the first customer name */
    WinSendDlgItemMsg( hwnd, OE_CB_CUSTNAME,
                       LM_SELECTITEM,
                       MPFROMSHORT(0),
                       MPFROMSHORT(TRUE) );

    /* set a non-proportional font to the products list box */
    setFixedFont( hwnd, OE_LI_PRODUCTS );

    /* set the focus on the customer name combo box */
    putWindowToCenter( hwnd );
    WinSetFocus(HWND_DESKTOP, WinWindowFromID (hwnd, OE_CB_CUSTNAME)  );
}

/***************************************************************************/
/* setFixedFont()                                                          */
/*     Queries all available fonts and selects the fixed-pitch font.       */
/*     Sets the fotnts to the specified window.                            */
/***************************************************************************/
void setFixedFont( HWND hwnd, ULONG ulTargetItemId )
{
 HPS psHandle,hps;
 HDC hdc;
 PFONTMETRICS pFontStructure, pBack;
 long remainder,numberFonts,zero,xres,yres;
 short i,divide;
 char lbline[100];

    zero = 0L;
                                               /* get device resolution */
    hps = WinGetScreenPS(HWND_DESKTOP);
    hdc = GpiQueryDevice(hps);
    DevQueryCaps(hdc,CAPS_HORIZONTAL_FONT_RES,1L,&xres);
    DevQueryCaps(hdc,CAPS_VERTICAL_FONT_RES,1L,&yres);

    hps = WinGetPS(hwnd);
                                  /* determine the number of fonts avail */
    numberFonts = GpiQueryFonts( hps, QF_PUBLIC|QF_PRIVATE, NULL, &zero,
                                 (ULONG)sizeof(FONTMETRICS) ,NULL );

                                  /* Allocate space for structures */
    pBack = pFontStructure = (PFONTMETRICS)malloc( sizeof(FONTMETRICS)*numberFonts );

                                  /* Get all of the fonts */
    remainder = GpiQueryFonts( hps, QF_PUBLIC|QF_PRIVATE, NULL, &numberFonts,
                               (ULONG)sizeof(FONTMETRICS), pFontStructure );

    for( i=1; i<=numberFonts; i++, pFontStructure++ )
    {
       if( xres == pFontStructure->sXDeviceRes
        && yres == pFontStructure->sYDeviceRes
        && (pFontStructure->fsType & FM_TYPE_FIXED) != 0 )
       {                                             /* Insert into listbox*/
           if( flDBCScapability == TRUE
            && (pFontStructure->fsType & FM_TYPE_MBCS) == 0 )  continue;

           divide = (short) pFontStructure->sNominalPointSize/10;
           sprintf( lbline, "%.3d", divide );
           strcat( lbline,"." );
           strcat( lbline,pFontStructure->szFacename );
           WinSetPresParam( WinWindowFromID(hwnd,ulTargetItemId),
                            PP_FONTNAMESIZE,
                            (ULONG)strlen(lbline)+1,
                            (PVOID) lbline);
           break;                                                 /* 110895*/
        }
        else ;                                  /* In case of outline font.*/
                                   /* Move onto next structure */
    }

    free( pBack );
    WinReleasePS( hps );
}
