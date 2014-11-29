/***************************************************************************/
/* XPRMCUST.C    XPG4 Primer for OS/2 WARP - V1.0       11/15/95           */
/*               Window procedures and related functions for customer      */
/*               records manipulation.                                     */
/***************************************************************************/
#define INCL_WIN
#define INCL_DOS
#define INCL_GPI
#include <os2.h>                       /* System Include File              */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <limits.h>

#include "xprmos2.h"                   /* Application Include File         */
#include "xprmres.h"                   /* Application Include NLS File     */
#include "xprmdata.h"

/**************************************/
/* function prototypes - global       */
/**************************************/
MRESULT EXPENTRY custListDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

/**************************************/
/* function prototypes - external     */
/**************************************/
/* PM related functions */
extern void putWindowToCenter( HWND hwnd );
extern void setDlgItemWcs( HWND hwnd, ULONG ulItemID, wchar_t* wcsText);
extern size_t queryDlgItemWcs( HWND hwnd, ULONG ulItemId, wchar_t* wcsBuf, size_t szLen );
extern void associateHelp( HWND hwnd );
/* functions related to customer records handling */
extern CUSTREC* getCustRec( char* pszKey, enum custitem e_category );
extern void getNewCustNum( wchar_t* wcsNewNumber );
extern Boolean addNewCust( CUSTREC* pCust );
extern BOOL isDigitString( char *pszString );

/**************************************/
/* function prototypes - static       */
/**************************************/
static void setCustNum( HWND hwnd, wchar_t *wcsCustNum );

/**************************************/
/* External Variables                 */
/**************************************/
extern HAB hab;
extern HMODULE hModRsrc;
extern HWND hwndHelpInstance;
extern char acMsgTable[][MAX_MSG_LEN];

/***************************************************************************/
/*  custInfoDlgProc()                                                      */
/*     This procedure is called in case of either                          */
/*       adding new customer (mp2 = NULL)                                  */
/*     or                                                                  */
/*       showing query result (mp2 = pointer to the CUSTREC data           */
/***************************************************************************/
MRESULT EXPENTRY custInfoDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 char *pszCustNumber;
 static CUSTREC* pCust;
 static CUSTREC aCust;
 ULONG  ulInvalidField;
 ULONG  procOKonCustInfo( HWND hwnd, CUSTREC* pCust );
 CUSTREC* initCustInfoDlg( HWND hwnd, MPARAM mp2 );

    switch(msg)
    {
        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1))
            {
                case DID_OK:
                    if( (ulInvalidField=procOKonCustInfo( hwnd, pCust )) != 0 )
                    {
                        showMessageBox( hwnd, IDS_WARNING_3,
                                        IDS_WARNING_CAPTION, MB_WARNING );
                        clearField( hwnd, ulInvalidField );
                        break;
                    }
                    /* if the contents are OK, write to the file */
                    if( addNewCust( pCust ) == TRUE )
                    {
                        showMessageBox( hwnd, IDS_INFO_1,
                                        IDS_INFO_CAPTION, MB_INFORMATION );
                    }
                    else                                  /* file I/O error*/
                    {
                        showMessageBox( hwnd, IDS_ERROR_1,
                                        IDS_ERROR_CAPTION, MB_ERROR );
                    }
                    WinDismissDlg (hwnd,DID_OK);
                    break;

                 case DID_CANCEL:
                    WinDismissDlg (hwnd,DID_CANCEL);
                    break;
             }
             break;

        case WM_INITDLG:
            associateHelp( hwnd );
            pCust = initCustInfoDlg( hwnd, mp2 );
            if( pCust == NULL )                      /* add customer record*/
            {
               pCust = &aCust;
               /* set new customer number */
               getNewCustNum( aCust.awcNum );
               setCustNum( hwnd, aCust.awcNum );
            }
            break;

      default:
            return(WinDefDlgProc(hwnd,msg,mp1,mp2));
            break;
    }
    return(FALSE);
}

/***************************************************************************/
/* initCustInfoDlg()                                                       */
/*     Procedure for WM_INITDLG message.                                   */
/*     Sets the text limit for each entry fields, and sets the customer    */
/*     records if mp2 is not NULL.                                         */
/***************************************************************************/
static CUSTREC* initCustInfoDlg( HWND hwnd, MPARAM mp2 )
{
char acBuf[48];
CUSTREC* pCust;

   putWindowToCenter( hwnd );
   pCust = (CUSTREC*)PVOIDFROMMP(mp2);
   /* Max no. of chars for CI_EF_NAME is 46 wide character. */
   WinSendDlgItemMsg( hwnd, CI_EF_NAME,
                      EM_SETTEXTLIMIT,
                      MPFROMSHORT(CUST_NAME_LEN*MB_LEN_MAX-1), 0L );
   /* Max no. of chars for CI_EF_CUSTTEL is 14 SBCS characters */
   WinSendDlgItemMsg( hwnd, CI_EF_CUSTTEL,
                      EM_SETTEXTLIMIT, MPFROMSHORT(CUST_PHONE_LEN-1), 0L );

   /* Show the customer record if pCUst is set */
   if( pCust != NULL )
   {
       setCustNum( hwnd, pCust->awcNum );
       setDlgItemWcs( hwnd, CI_EF_NAME, pCust->awcName );
       setDlgItemWcs( hwnd, CI_ML_CUSTADDR, pCust->awcAddr );
       setDlgItemWcs( hwnd, CI_EF_CUSTTEL, pCust->awcPhone );
       /* Disable OK button */
       WinEnableWindow( WinWindowFromID(hwnd, DID_OK ), FALSE );
   }
   return pCust;
}

/***************************************************************************/
/* setCustNum()                                                            */
/*     Set customer number of the specified customer record to the title   */
/*     bar.                                                                */
/***************************************************************************/
static void setCustNum( HWND hwnd, wchar_t* wcsCustNum )
{
char *p;

    p = malloc( strlen(acMsgTable[IDS_VO_CUSTNUM]) +8 );
    sprintf( p, "%s:%0.6ls", acMsgTable[IDS_VO_CUSTNUM], wcsCustNum );
    WinSetDlgItemText( hwnd, FID_TITLEBAR, p );
    free( p );
    return;
}

/***************************************************************************/
/* procOKonCustInfo()                                                     */
/*     Procedure for DID_OK message on Customer Information dialog.        */
/*     Queries the contents of the entry fields and returns the field ID   */
/*     if no data is entered.  Otherwise, returns 0.                       */
/***************************************************************************/
static ULONG procOKonCustInfo( HWND hwnd, CUSTREC* pCust )
{

    if( queryDlgItemWcs( hwnd, CI_EF_NAME, pCust->awcName, CUST_NAME_LEN-1 ) == 0 )
        return CI_EF_NAME;
    if( queryDlgItemWcs( hwnd, CI_ML_CUSTADDR, pCust->awcAddr, CUST_ADDR_LEN-1 ) == 0 )
        return CI_ML_CUSTADDR;
    if( queryDlgItemWcs( hwnd, CI_EF_CUSTTEL, pCust->awcPhone, CUST_PHONE_LEN-1 ) == 0 )
        return CI_EF_CUSTTEL;

    return 0;
}

/****************************************************************************/
/* queryCustDlgProc()                                                       */
/*   This procedure finds the specified customer record and calls either    */
/*     custListDlg (category = custnum or phone, exact match)               */
/*   or                                                                     */
/*     custInfoDlg (category = name)                                        */
/****************************************************************************/
MRESULT EXPENTRY queryCustDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 CHAR acCustKey[48];
 CUSTREC* pCust;
 int iTemp;
 static enum custitem e_category;
 enum custitem procBNCLICKEDonQueryCust( HWND hwnd, SHORT itemId );

    switch(msg)
    {
        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1))
            {
                case DID_OK:

                   WinQueryDlgItemText( hwnd, QC_EF_CUSTKEY, 47, acCustKey );
                   if( strlen(acCustKey) == 0 )
                   {
                      showMessageBox( hwnd, IDS_WARNING_3,
                                      IDS_WARNING_CAPTION, MB_WARNING );
                      clearField( hwnd, QC_EF_CUSTKEY );
                      break;
                   }

                   if( acCustKey[0] == '*' )                 /* all records*/
                   {
                      e_category = custall;
                      pCust = getCustRec( NULL, custall );
                   }
                   else
                   {
                      if( e_category == custnum )
                      {
                         if( isDigitString(acCustKey) == FALSE )
                         {
                            showMessageBox( hwnd, IDS_WARNING_4,
                                            IDS_WARNING_CAPTION, MB_WARNING );
                            clearField( hwnd, QC_EF_CUSTKEY );
                            break;
                         }

                         iTemp = atoi( acCustKey );
                         sprintf( acCustKey, "%06d", iTemp );
                      }
                      pCust = getCustRec( acCustKey, e_category );
                   }

                   if( pCust == NULL )
                   {
                       showMessageBox( hwnd, IDS_WARNING_1, IDS_WARNING_CAPTION, MB_WARNING );
                   }
                   else
                   {
                     if( e_category == custname ||
                         e_category == custall )
                     {
                       /* multiple records, show the list of matching data */
                       WinDlgBox ( HWND_DESKTOP, hwnd,
                                   (PFNWP) custListDlgProc, hModRsrc,
                                   DID_CUSTLIST, pCust );
                     }
                     else
                     {
                       /* exact match, show the customer record */
                       WinDlgBox( HWND_DESKTOP, hwnd,
                                  (PFNWP) custInfoDlgProc, hModRsrc,
                                  DID_CUSTINFO, pCust );
                     }
                   }
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
               case QC_PB_CUSTNUM:
               case QC_PB_CUSTNAME:
               case QC_PB_TELNUM:
                 if( SHORT2FROMMP(mp1) == BN_CLICKED )
                    e_category = procBNCLICKEDonQueryCust( hwnd, SHORT1FROMMP(mp1) );
                 break;
            }
            break;

        case WM_INITDLG:

            associateHelp( hwnd );
            putWindowToCenter( hwnd );
            /* Max no. of chars for QC_EF_CUSTKEY is 6 */
            WinSendDlgItemMsg( hwnd, QC_EF_CUSTKEY,
                               EM_SETTEXTLIMIT, MPFROMSHORT(6), 0L);
            /* Set button QC_PB_CUSTNUM to checked */
            WinSendDlgItemMsg( hwnd,QC_PB_CUSTNUM,
                               BM_SETCHECK, MPFROMSHORT(TRUE), 0L );
            e_category = custnum;
            break;

        default:
            return(WinDefDlgProc(hwnd,msg,mp1,mp2));
            break;
    }
    return(FALSE);
}

/***************************************************************************/
/* procBNCLICKEDonQueryCust()                                              */
/*     Procedure for BN_CLICKED message on the Query Customer dialog.      */
/*     Sets the entry field's limit length according to the selected push  */
/*     button and returns the search category.                             */
/***************************************************************************/
static enum custitem procBNCLICKEDonQueryCust( HWND hwnd, SHORT itemId )
{
enum custitem e_category;

    switch( itemId )
    {
      case QC_PB_CUSTNUM:
        e_category = custnum;
        WinSendDlgItemMsg( hwnd, QC_EF_CUSTKEY,
                           EM_SETTEXTLIMIT, MPFROMSHORT(6), 0L);
        break;
      case QC_PB_CUSTNAME:
       /* must be either custnameNL or custnameEng */
        e_category = custname;
        WinSendDlgItemMsg( hwnd, QC_EF_CUSTKEY,
                           EM_SETTEXTLIMIT, MPFROMSHORT(46), 0L);
        break;
      case QC_PB_TELNUM:
        e_category = phone;
        WinSendDlgItemMsg( hwnd, QC_EF_CUSTKEY,
                           EM_SETTEXTLIMIT, MPFROMSHORT(14), 0L);
        break;
    }
    return e_category;
}

/****************************************************************************/
/* custListDlgProc()                                                        */
/*   If the multiple records are found after query customer records dialog, */
/*   this dialog is shown.                                                  */
/****************************************************************************/
MRESULT EXPENTRY custListDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
CUSTREC* pCust;
CUSTREC* selectedCust( HWND hwnd );

    switch(msg)
    {
        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1))
            {
                case DID_OK:
                   pCust = selectedCust( hwnd );
                   if( pCust == NULL )
                   {
                       showMessageBox( hwnd, IDS_ERROR_2, IDS_ERROR_CAPTION, MB_ERROR );
                   }
                   else
                   {
                       WinDlgBox( HWND_DESKTOP, hwnd,
                                  (PFNWP) custInfoDlgProc, hModRsrc,
                                  DID_CUSTINFO, pCust );
                   }
                   break;
                case DID_CANCEL:
                   /* Dismiss the dialog box */
                   WinDismissDlg (hwnd,DID_CANCEL);
                   break;
            }
            break;

        case WM_INITDLG:
        {
        SHORT sIndex;
        char acCustName[CUST_NAME_LEN];

            associateHelp( hwnd );
            putWindowToCenter( hwnd );
            pCust = (CUSTREC*)PVOIDFROMMP(mp2);
            while( pCust != NULL )
            {
               /*Only first 48 bytes are shown.*/
               sprintf( acCustName, "%.*ls", CUST_NAME_LEN, pCust->awcName );
               sIndex = SHORT1FROMMR( WinSendDlgItemMsg( hwnd, LI_CUSTLIST,
                                                         LM_INSERTITEM,
                                                         MPFROMSHORT(LIT_END),
                                                         MPFROMP(acCustName) ) );
               /* Set CUSTREC as the index handle */
               WinSendDlgItemMsg( hwnd, LI_CUSTLIST,
                                  LM_SETITEMHANDLE,
                                  MPFROMSHORT(sIndex),
                                  MPFROMLONG((ULONG)pCust) );
               pCust = pCust->next;
            }
            /* select the first item */
            WinSendDlgItemMsg( hwnd, LI_CUSTLIST,
                               LM_SELECTITEM,
                               MPFROMSHORT(0),
                               MPFROMSHORT(TRUE) );

            break;
        }
        default:
            return(WinDefDlgProc(hwnd,msg,mp1,mp2));
     break;
    }
return(FALSE);
}

/****************************************************************************/
/* selectedCust()                                                           */
/*   Gets the customer records information bound to the selected list item. */
/*   Return the pointer or NULL if no item is selected or the cound record  */
/*   is not found.                                                          */
/****************************************************************************/
static CUSTREC* selectedCust( HWND hwnd )
{
SHORT sItemIndex;
ULONG ulItemHandle;

   sItemIndex = SHORT1FROMMR( WinSendDlgItemMsg( hwnd, LI_CUSTLIST,
                                                LM_QUERYSELECTION,
                                                (MPARAM) LIT_FIRST,
                                                (MPARAM) 0L) );
   if( sItemIndex == LIT_NONE ) return NULL;

   ulItemHandle = LONGFROMMR( WinSendDlgItemMsg( hwnd, LI_CUSTLIST,
                                                 LM_QUERYITEMHANDLE,
                                                 MPFROMSHORT(sItemIndex),
                                                 (MPARAM) 0L ) );
   if( ulItemHandle == 0L ) return NULL;
   else  return (CUSTREC*)(PVOID)ulItemHandle;
}
