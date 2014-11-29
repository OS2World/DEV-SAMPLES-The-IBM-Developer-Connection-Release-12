/************   OS/2 Application Primer Sample   **********************/
/*                                                                    */
/* Module Name : CUSTDLG.C                                            */
/* Version : 2.1 (12/24/93)                                           */
/*                                                                    */
/*    << This sample program is only for OS/2 V2.x. >>                */
/*                                                                    */
/* A sample program for "OS/2 DBCS Application Primer".               */
/* This source contains the dialog procedures related to the          */
/* manipulation of the customer information.                          */
/*                                                                    */
/**********************************************************************/
#define INCL_NOCOMMON
#define INCL_DOS
#define INCL_WIN
#define INCL_DOSFILEMGR

#define INCL_MAIN_WND
#define INCL_CUSTINFO_DLG
#define INCL_CUSTLIST_DLG
#define INCL_CUSTQRY_DLG
#define INCL_CUSTREG_DLG

#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ordent.h"
#include "ordhelp.h"
#include "dbcs.h"

extern HMODULE  hmodRsrc;
extern CHAR     szKeyword[ LENGTH_CUSTNAME_NULL ];

/******************* Dialog Procedure ***************************/
MRESULT EXPENTRY CustInfoProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  CHAR    szId[LENGTH_CUSTNUM_NULL],
          szListBoxRecord[LENGTH_CUSTNUM+5+LENGTH_CUSTNAME+1];
  struct  custRec custRecord;
  HWND    hwndLBox;
  USHORT  item;

  switch (msg) {
    case WM_INITDLG:
      HelpAssociate(hwnd);
      SetEntryFieldLength(hwnd, IDD_CUSTINFO_DLG);
      strncpy(szId, PVOIDFROMMP(mp2), LENGTH_CUSTNUM_NULL);
      strncpy(custRecord.custNo, szId, LENGTH_CUSTNUM_NULL);
      GetCustInfo(&custRecord);
      WinSetDlgItemText(hwnd, IDD_CUSTINFO_ITX_CUSTNUM,  custRecord.custNo);
      WinSetDlgItemText(hwnd, IDD_CUSTINFO_SLE_CUSTNAME, custRecord.custName);
      WinSetDlgItemText(hwnd, IDD_CUSTINFO_MLE_CUSTADDR, custRecord.custAddr);
      WinSetDlgItemText(hwnd, IDD_CUSTINFO_SLE_CUSTPHON, custRecord.custPhon);
      break;

    case WM_COMMAND:
      switch(SHORT1FROMMP(mp1)) {
        case IDD_CUSTINFO_BTN_CHANGE :
          /*--- If there is no text in the entries -----------*/
          if (WinQueryDlgItemTextLength(hwnd, IDD_CUSTINFO_SLE_CUSTNAME) &&
              WinQueryDlgItemTextLength(hwnd, IDD_CUSTINFO_MLE_CUSTADDR) &&
              WinQueryDlgItemTextLength(hwnd, IDD_CUSTINFO_SLE_CUSTPHON)) {

            BuildCustRec(hwnd, IDD_CUSTINFO_DLG, &custRecord);
            PutCustInfo(&custRecord);
            /*------------------------------------------------*/
            /*   Update the customer name in the list box of  */
            /*   the customer list dialog.                    */
            /*------------------------------------------------*/
            strcpy(szListBoxRecord, custRecord.custNo);
            strcat(szListBoxRecord, "     ");
            strcat(szListBoxRecord, custRecord.custName);
            hwndLBox = WinWindowFromID(HWND_DESKTOP, IDD_CUSTLIST_DLG);
            item = SHORT1FROMMR(
              WinSendDlgItemMsg(hwndLBox, IDD_CUSTLIST_LST_LISTBOX,
                                LM_SEARCHSTRING,
                                MPFROM2SHORT(LSS_CASESENSITIVE |
                                             LSS_SUBSTRING, LIT_FIRST),
                                MPFROMP(custRecord.custNo))) ;
            WinSendDlgItemMsg(hwndLBox, IDD_CUSTLIST_LST_LISTBOX,
                              LM_SETITEMTEXT,
                              MPFROMSHORT(item), szListBoxRecord);
            HelpDestroyAssociate(hwnd);
            WinDismissDlg(hwnd, TRUE);
            return;
          } else {
            /*------------------------------------------------*/
            /*   If name/address/phone number are not filled, */
            /*   give a caution to a user with beep.          */
            /*------------------------------------------------*/
            DosBeep(1380, 100);
          }
          break;

        case IDD_CUSTINFO_BTN_CANCEL:
          HelpDestroyAssociate(hwnd);
          WinDismissDlg(hwnd, TRUE);
          return;
        default:
          break;
      }
    default:
      break;
  }
  return WinDefDlgProc(hwnd, msg, mp1, mp2);
}


MRESULT EXPENTRY CustListProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  SHORT   indexItem;
  CHAR    szId[LENGTH_CUSTNUM_NULL];

  switch (msg) {
    case WM_INITDLG:
      HelpAssociate(hwnd);
      SetUpCustLBox(hwnd);
      indexItem =
        SHORT1FROMMR(WinSendDlgItemMsg(hwnd, IDD_CUSTLIST_LST_LISTBOX,
                                       LM_QUERYTOPINDEX, NULL, NULL));
      WinSendDlgItemMsg(hwnd, IDD_CUSTLIST_LST_LISTBOX, LM_SELECTITEM,
                        MPFROMSHORT(indexItem), MPFROMSHORT(TRUE));
      break;

    case WM_COMMAND:
      switch(SHORT1FROMMP(mp1)) {
        case IDD_CUSTLIST_BTN_DESC :
          indexItem =
            SHORT1FROMMR(WinSendDlgItemMsg(hwnd,
                                           IDD_CUSTLIST_LST_LISTBOX,
                                           LM_QUERYSELECTION,
                                           MPFROMSHORT(LIT_FIRST),
                                           NULL));
          WinSendDlgItemMsg(hwnd,
                            IDD_CUSTLIST_LST_LISTBOX,
                            LM_QUERYITEMTEXT,
                            MPFROM2SHORT(indexItem, LENGTH_CUSTNUM),
                            MPFROMP(szId));
          szId[LENGTH_CUSTNUM] = '\0';
          /*--- Call the customer information dialog -----------*/
          WinDlgBox(HWND_DESKTOP, hwnd, CustInfoProc,
                    hmodRsrc, IDD_CUSTINFO_DLG, szId);
          break;

        case IDD_CUSTLIST_BTN_CANCEL :
          HelpDestroyAssociate(hwnd);
          WinDismissDlg(hwnd, TRUE);
          return FALSE;
      }
      break;
    case WM_CONTROL:
      switch(SHORT2FROMMP(mp1)) {
        case LN_ENTER:
          WinPostMsg(hwnd, WM_COMMAND,
                     MPFROMSHORT(IDD_CUSTLIST_BTN_DESC),
                     MPFROM2SHORT(CMDSRC_PUSHBUTTON, TRUE));
          break;
      }
    default:
      return WinDefDlgProc( hwnd, msg, mp1, mp2 );
  }
  return FALSE;
}


MRESULT EXPENTRY CustQueryProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  SHORT KeyLen;

  switch (msg) {
    case WM_INITDLG:
      HelpAssociate(hwnd);
      WinSendDlgItemMsg(hwnd, IDD_CUSTQRY_SLE_KEY, EM_SETTEXTLIMIT,
                        MPFROMSHORT(LENGTH_CUSTNAME), NULL );
      WinSetFocus(hwnd, WinWindowFromID( hwnd, IDD_CUSTQRY_SLE_KEY));
      break;

    case WM_COMMAND:
      switch(SHORT1FROMMP(mp1)) {
        case IDD_CUSTQRY_BTN_QUERY:
          KeyLen = WinQueryDlgItemTextLength(hwnd, IDD_CUSTQRY_SLE_KEY);
          WinQueryDlgItemText(hwnd, IDD_CUSTQRY_SLE_KEY,
                              KeyLen+1, szKeyword );
          HelpDestroyAssociate(hwnd);
          WinDismissDlg(hwnd, CALL_CUSTLIST);
          return;

        case IDD_CUSTQRY_BTN_CANCEL:
          HelpDestroyAssociate(hwnd);
          WinDismissDlg(hwnd, SKIP_CUSTLIST);
          return;
      } break;
    default:
      break;
  }
  return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}


MRESULT EXPENTRY CustRegProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  CHAR    szCustNum[LENGTH_CUSTNUM];
  struct  custRec custRecord;

  switch (msg) {
    case WM_INITDLG:
      HelpAssociate(hwnd);
      SetEntryFieldLength(hwnd, IDD_CUSTREG_DLG);
      GetNewNumber(CUSTFILE, szCustNum);
      WinSetDlgItemText(hwnd, IDD_CUSTREG_ITX_CUSTNUM, szCustNum);
      WinSetFocus(hwnd, WinWindowFromID(hwnd, IDD_CUSTREG_SLE_CUSTNAME));
      break;

    case WM_COMMAND:
      switch(SHORT1FROMMP(mp1)) {
        case IDD_CUSTREG_BTN_REG:
          if (WinQueryDlgItemTextLength(hwnd, IDD_CUSTREG_SLE_CUSTNAME) &&
              WinQueryDlgItemTextLength(hwnd, IDD_CUSTREG_MLE_CUSTADDR) &&
              WinQueryDlgItemTextLength(hwnd, IDD_CUSTREG_SLE_CUSTPHON)) {

            BuildCustRec(hwnd, IDD_CUSTREG_DLG, &custRecord);
            PutCustInfo(&custRecord);

            HelpDestroyAssociate(hwnd);
            WinDismissDlg(hwnd, TRUE);
            return;
          } else {
            DosBeep(1380, 100);
          }
        break;
        case IDD_CUSTREG_BTN_CANCEL:
          HelpDestroyAssociate(hwnd);
          WinDismissDlg(hwnd, TRUE);
          return;
        default:
          break;
      }
    default:
      break;
  }
  return WinDefDlgProc(hwnd, msg, mp1, mp2);
}
