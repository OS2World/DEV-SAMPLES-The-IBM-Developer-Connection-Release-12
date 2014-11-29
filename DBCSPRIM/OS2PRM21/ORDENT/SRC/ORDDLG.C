/************   OS/2 Application Primer Sample   **********************/
/*                                                                    */
/* Module Name : ORDDLG.C                                             */
/* Version : 2.1 (12/24/93)                                           */
/*                                                                    */
/*    << This sample program is only for OS/2 V2.x. >>                */
/*                                                                    */
/*   A sample program for "OS/2 DBCS Application Primer".             */
/*   Order-entry dialog procedure and its subroutines. Accept orders  */
/*   and write them to the order file, ORDRFILE.DAT.                  */
/*                                                                    */
/**********************************************************************/
#define    INCL_NOCOMMON
#define    INCL_WIN
#define    INCL_DOSFILEMGR
#define    INCL_DOSPROCESS

#define    INCL_ORDR_DLG
#define    INCL_MAIN_WND

#include   <os2.h>
#include   <stdlib.h>
#include   <stdio.h>
#include   <string.h>
#include   <ctype.h>
#include   <time.h>

#include   "ordent.h"
#include   "ordhelp.h"

extern HAB     hab;
extern HMODULE hmodRsrc;
extern CHAR    szEmpNumber[];

CHAR   szCustFileName[13] = "CUSTFILE.DAT";
CHAR   szProdFileName[13] = "PRODFILE.DAT";
CHAR   szOrdrFileName[13] = "ORDRFILE.DAT";


MRESULT EXPENTRY OrdrEntDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  static CHAR    szDate[20], szOrdrNum[10];
  static struct  custRec  cRec;
  static struct  prodRec  pRec[3];
  static struct  ordrRec  oRec;
  static USHORT  usCurItem[3], usQty[3];
  static ULONG   ulTotalPrice;

  USHORT  usSelect[4];
  SHORT   i;
  ULONG   ulPrice[3];

  switch(msg) {
    case WM_INITDLG:
      HelpAssociate(hwnd);
      ulTotalPrice = 0L;
      InitCustRecord(&cRec);

      for (i = 0; i < 3; i++)
        InitProdRecord(&pRec[i]);

      InitOrdrRecord(&oRec);
      WinSetWindowText(WinWindowFromID(hwnd, IDD_ORDR_ITX_EN), szEmpNumber);
      GetDate(szDate, sizeof(szDate));
      WinSetWindowText(WinWindowFromID(hwnd, IDD_ORDR_ITX_CR), szDate);
      GetNewNumber("ORDRFILE.DAT", szOrdrNum);
      WinSetWindowText(WinWindowFromID(hwnd, IDD_ORDR_ITX_ON), szOrdrNum);
      WinSendMsg(WinWindowFromID(hwnd, IDD_ORDR_SLE_QT1), EM_SETTEXTLIMIT,
                 MPFROMSHORT(LENGTH_QUANTITY), NULL);
      WinSendMsg(WinWindowFromID(hwnd, IDD_ORDR_SLE_QT2), EM_SETTEXTLIMIT,
                 MPFROMSHORT(LENGTH_QUANTITY), NULL);
      WinSendMsg(WinWindowFromID(hwnd, IDD_ORDR_SLE_QT3), EM_SETTEXTLIMIT,
                 MPFROMSHORT(LENGTH_QUANTITY), NULL);
      WinSendMsg(WinWindowFromID(hwnd, IDD_ORDR_LBX_CN), EM_SETTEXTLIMIT,
                 MPFROMSHORT(LENGTH_CUSTNUM), NULL);
      SetUpProductLBox(WinWindowFromID(hwnd, IDD_ORDR_LBX_PM));
      SetUpCustNoLBox(WinWindowFromID(hwnd, IDD_ORDR_LBX_CN));
      for (i = 0; i < 3; i++)
        usCurItem[i] = 0xFFFF;
      for (i = 0; i < 4; i++)
        usSelect[i] = 0xFFFF;
      break;
    case WM_COMMAND:
      switch(SHORT1FROMMP(mp2)) {
        case CMDSRC_PUSHBUTTON:
          switch(SHORT1FROMMP(mp1)) {
            case IDD_ORDR_BTN_CNF:
            case IDD_ORDR_BTN_ENT:
              ulTotalPrice = 0L;
              if (HandleCustInp(hwnd, &cRec) != SUCCESS){
                DosBeep(1380, 100);
                return FAIL;
              }
              for (i = 0; i < 3; i++){
                if (usCurItem[i] == 0xFFFF)
                  ulPrice[i] = 0L;
                else {
                  usQty[i] = HandleQtyInp(hwnd, i);
                  if (usQty[i] == 0xFFFF)
                    ulPrice[i] = 0L;
                  else
                    ulPrice[i] = pRec[i].unitPrice * usQty[i];
                }
              }
              for (i = 0; i < 3; i++)
                ulTotalPrice += ulPrice[i];
              HandleTotal(hwnd, ulTotalPrice);
              if (SHORT1FROMMP(mp1) == IDD_ORDR_BTN_CNF)
                break;
              BuildOrdrRec(szOrdrNum, usQty, &oRec, &cRec, pRec);
              if (PutOrdrFile(&oRec) == SUCCESS)
                AlertUser(HWND_DESKTOP, IDD_ORDR_MSG_UD, "\0");
              else
                AlertUser(HWND_DESKTOP, IDD_ORDR_MSG_ER, "\0");
            case IDD_ORDR_BTN_CAN:
              HelpDestroyAssociate(hwnd);
              WinDismissDlg(hwnd, TRUE);
              return FALSE;
            default:
              break;
          } break;
      } break;
    case WM_CONTROL:

      switch(SHORT1FROMMP(mp1)) {
        case IDD_ORDR_LBX_PM:

          switch(SHORT2FROMMP(mp1)) {
            case LN_SELECT:

              if (3 < IdentSelect(WinWindowFromID(hwnd, IDD_ORDR_LBX_PM),
                                  usSelect)){
                DosBeep(1380, 100);
                WinSendMsg(WinWindowFromID(hwnd, IDD_ORDR_LBX_PM),
                           LM_SELECTITEM,
                           MPFROMSHORT(IdentNewItem(usCurItem, usSelect)),
                           MPFROMSHORT(FALSE));
              } else {
                for (i = 0; i < 3; i++)
                  usCurItem[i] = usSelect[i];
                ReflectLBSelect(WinWindowFromID(hwnd, IDD_ORDR_LBX_PM),
                                usCurItem, pRec);
                for (i = 0; i < 3; i++){
                  if (usCurItem[i] == (USHORT)0xFFFF)
                    WinSetWindowText(WinWindowFromID(hwnd, IDD_ORDR_ITX_PR1+i),
                                     "" );
                  else
                    WinSetWindowText(WinWindowFromID(hwnd, IDD_ORDR_ITX_PR1+i),
                                     pRec[i].prodName );
                }
              }
            default:
              break;
          } break;

        case IDD_ORDR_LBX_CN:
          switch(SHORT2FROMMP(mp1)) {
            case CBN_EFCHANGE:
              HandleCustInp(hwnd, &cRec);
           default:
              break;
          } break;
        default:
          break;
      } break;

    default:
      return WinDefDlgProc(hwnd, msg, mp1, mp2);
  }
  return FALSE;
}


/**********************************************************************/
/*    Initialize a customer record                                    */
/**********************************************************************/
SHORT InitCustRecord(struct custRec *pCRec)
{
  pCRec->custNo[0] = '\0';
  pCRec->custName[0] = '\0';
  pCRec->custAddr[0] = '\0';
  pCRec->custPhon[0] = '\0';
  return SUCCESS;
}


/**********************************************************************/
/*    Initialize a product record                                     */
/**********************************************************************/
SHORT InitProdRecord(struct prodRec *pPRec)
{
  pPRec->prodNo[0] = '\0';
  pPRec->prodName[0] = '\0';
  pPRec->unitPrice = 0L;
  return SUCCESS;
}


/**********************************************************************/
/*    Initialize a product order                                      */
/**********************************************************************/
static SHORT InitOrdrProd(struct ordrProd *pOProd)
{
  pOProd->prodNum[0] = '\0';
  pOProd->qty = 0;
  return SUCCESS;
}


/**********************************************************************/
/*    Initialize an order record                                      */
/**********************************************************************/
SHORT InitOrdrRecord(struct ordrRec *pORec)
{
  USHORT i;

  pORec->ordrNo[0] = '\0';
  pORec->ordrDate[0] = '\0';
  pORec->custNo[0] = '\0';
  for (i = 0; i < 5; i++)
    InitOrdrProd(&(pORec->prod[i]));
  pORec->empNo[0] = '\0';
  pORec->shipDate[0] = '\0';
  return SUCCESS;
}


/**********************************************************************/
/*    Format the order number                                         */
/**********************************************************************/
SHORT FormatOrdrNo(CHAR strBuf[])
{
  CHAR  szBuf[6], *ptrBuf;
  SHORT i;

  ptrBuf = szBuf;
  for (i=0; i < (6-strlen(strBuf)); i++)
    *ptrBuf++ = '0';
  strcpy(ptrBuf, strBuf);
  memcpy(strBuf, szBuf, 6);
  return SUCCESS;

}


/**********************************************************************/
/*    Set-up a product list box                                       */
/**********************************************************************/
SHORT SetUpProductLBox(HWND hwndLBox)
{
  CHAR    szListBoxItem[40];
  ULONG   cntr, byteRead;
  HFILE   hProdFile;
  struct  prodRec prodRecord[10];

  hProdFile = OpenDataFile(szProdFileName, READ_ONLY);
  do {
    DosRead(hProdFile, (PVOID)prodRecord, sizeof(prodRecord), &byteRead);
    for (cntr = 0; (cntr * sizeof(struct prodRec)) < byteRead; cntr++) {
      memset(szListBoxItem, ' ', sizeof(szListBoxItem));
      strncpy(szListBoxItem, prodRecord[cntr].prodNo, 6);
      szListBoxItem[6] = ' ';
      strcpy(&szListBoxItem[7], prodRecord[cntr].prodName);
      WinSendMsg(hwndLBox, LM_INSERTITEM,
                 MPFROM2SHORT(LIT_END, 0),
                 MPFROMP(szListBoxItem));
    }
  } while (byteRead == sizeof(prodRecord));
  DosClose(hProdFile);
  return SUCCESS;
}


/**********************************************************************/
/*    Set-up a customer number list box                               */
/**********************************************************************/
SHORT SetUpCustNoLBox(HWND hwndLBox)
{
  CHAR    szListBoxItem[7];
  ULONG   cntr, byteRead;
  HFILE   hCustFile;
  struct  custRec custRecord[10];

  hCustFile = OpenDataFile(szCustFileName, READ_ONLY);
  do {
    DosRead(hCustFile, (PVOID)custRecord, sizeof(custRecord), &byteRead);
    for (cntr = 0; (cntr * sizeof(struct custRec)) < byteRead; cntr++) {
      memset(szListBoxItem, ' ', sizeof(szListBoxItem));
      strncpy(szListBoxItem, custRecord[cntr].custNo, 6);
      szListBoxItem[6] = '\0';
      WinSendMsg(hwndLBox, LM_INSERTITEM,
                 MPFROM2SHORT(LIT_END, 0),
                 MPFROMP(szListBoxItem));
    }
  } while (byteRead == sizeof(custRecord));
  DosClose(hCustFile);
  return SUCCESS;
}


/**********************************************************************/
/*   Set a total price into the order entry dialog                    */
/**********************************************************************/
SHORT HandleTotal(HWND hwnd, ULONG ulTotal)
{
  CHAR szTotal[20];

  _ultoa(ulTotal, szTotal, 10);
  FormatPrice(szTotal);
  WinSetWindowText(WinWindowFromID(hwnd, IDD_ORDR_ITX_VL), szTotal);
  return SUCCESS;
}


/**********************************************************************/
/*    Set a customer number and name into the order entry dialog      */
/**********************************************************************/
SHORT HandleCustInp(HWND hwndDlg, struct custRec *pCRec)
{
  CHAR szBuf[7];
  SHORT rc, i;

  WinQueryWindowText(WinWindowFromID(hwndDlg, IDD_ORDR_LBX_CN),
                     sizeof(szBuf), szBuf);

  for (i = 0; i < strlen(szBuf); i++)
    if ((szBuf[i] < '0') || ('9' < szBuf[i])) return FAIL;

  PadCharacter(szBuf, '0', LENGTH_CUSTNUM);
  strcpy(pCRec->custNo, szBuf);

  if ((rc = GetCustInfo(pCRec)) == SUCCESS)
    WinSetWindowText(WinWindowFromID(hwndDlg, IDD_ORDR_ITX_CM),
                     pCRec->custName);

  return rc;
}


/***********************************************************************/
/*    Query quantity of an ordered product from the order entry dialog */
/***********************************************************************/
SHORT HandleQtyInp(HWND hwndDlg, SHORT num)
{
  CHAR szBuf[10];
  SHORT i;

  WinQueryWindowText(WinWindowFromID(hwndDlg, IDD_ORDR_SLE_QT1 + num),
                     sizeof(szBuf), szBuf);
  for (i = 0; i < strlen(szBuf); i++)
    if ((szBuf[i] < '0') || ('9' < szBuf[i])) return 0xFFFF;
  return atoi(szBuf);
}


/**********************************************************************/
/*    Pick up four selected items from the product list box           */
/**********************************************************************/
SHORT IdentSelect(HWND hwndLBox, USHORT usItems[])
{
  MRESULT mr;
  SHORT i, sSelNum;
  SHORT sSelect;

  sSelect = LIT_FIRST;
  for (i = 0; i < 4; i++){
    mr = WinSendMsg(hwndLBox, LM_QUERYSELECTION,
                    MPFROMSHORT(sSelect),
                    MPFROMLONG(0L));
    usItems[i] = (USHORT)(sSelect = SHORT1FROMMR(mr));
    if (sSelect == LIT_NONE) break;
  }
  sSelNum = i;
  for (; i < 4; i++)
    usItems[i] = 0xFFFF;
  return sSelNum;
}


/**********************************************************************/
/*                                                                    */
/**********************************************************************/
SHORT IdentNewItem(USHORT usCur[], USHORT usNew[])
{
  SHORT i, j;

  for (i = 0; i < 4; i++){
    for (j = 0; j < 3; j++){
      if (usNew[i] == usCur[j]) break;
    }
    if (j == 3) return usNew[i];
  }
  return (USHORT)0xFFFF;
}


/**********************************************************************/
/*                                                                    */
/**********************************************************************/
SHORT ReflectLBSelect(HWND hwndLBox, USHORT usLBItems[], struct prodRec pRec[])
{
  CHAR lBItemText[80];
  SHORT i;

  for (i = 0; i < 3; i++){
    if (usLBItems[i] != (USHORT)0xFFFF){
      WinSendMsg(hwndLBox, LM_QUERYITEMTEXT,
                 MPFROM2SHORT(usLBItems[i], 80),
                 MPFROMP(lBItemText));
      memcpy(pRec[i].prodNo, lBItemText, 6);
      pRec[i].prodNo[6] = '\0';
      GetProdInfo(&pRec[i]);
    }
  }
  return SUCCESS;
}


/**********************************************************************/
/*    Get a new number of customer, product or order numbers          */
/**********************************************************************/
SHORT GetNewNumber(CHAR *szFileName, CHAR *szNumber)
{
  ULONG   ulStructSize, moveType;
  ULONG   newPtr;
  LONG    lRecNum, distance;
  HFILE   hFile;

  if (strcmp(szFileName, CUSTFILE) == 0)
    ulStructSize = sizeof(struct custRec);
  else if (strcmp(szFileName, PRODFILE) == 0)
    ulStructSize = sizeof(struct prodRec);
  else if (strcmp(szFileName, ORDRFILE) == 0)
    ulStructSize = sizeof(struct ordrRec);
  else return FAIL;
  hFile = OpenDataFile(szFileName, READ_ONLY);
  distance = (LONG)0;
  moveType = 2;
  DosSetFilePtr(hFile, distance, moveType, &newPtr);
  lRecNum = (newPtr/ulStructSize) + 1;
  sprintf(szNumber, "%06ld", lRecNum);
  DosClose(hFile);
  return SUCCESS;
}


/**********************************************************************/
/*    Build an order record                                           */
/**********************************************************************/
SHORT BuildOrdrRec(CHAR szOrdr[], USHORT qty[], struct ordrRec *pORec,
                   struct custRec *pCRec, struct prodRec pRec[])
{
  SHORT i;

  strcpy(pORec->ordrNo, szOrdr);
  strcpy(pORec->custNo, pCRec->custNo);
  GetDateYYYYMMDD(pORec->ordrDate, sizeof(pORec->ordrDate));
  for (i = 0; i < 3; i++){
    strcpy(pORec->prod[i].prodNum, pRec[i].prodNo);
    pORec->prod[i].qty = qty[i];
  }
  strcpy(pORec->empNo, szEmpNumber);
  return SUCCESS;
}


/**********************************************************************/
/*    Put an order reccord into the order file                        */
/**********************************************************************/
SHORT PutOrdrFile(struct ordrRec *pORec)
{
  HFILE   hOrdrFile;
  ULONG   moveType, bytesWritten;
  LONG    distance;
  ULONG   newPtr;

  hOrdrFile = OpenDataFile(ORDRFILE, READ_WRITE);
  distance = (LONG)0;
  moveType = 2;
  DosSetFilePtr(hOrdrFile, distance, moveType, &newPtr);
  DosWrite(hOrdrFile, pORec, sizeof(struct ordrRec), &bytesWritten);
  DosClose(hOrdrFile);
  return SUCCESS;
}


/**********************************************************************/
/*                                                                    */
/**********************************************************************/
SHORT AlertUser(HWND hwnd, SHORT rsrcId, CHAR *szBuf)
{
  CHAR szMsgBoxText[80];

  WinLoadString(hab, hmodRsrc, rsrcId, sizeof(szMsgBoxText), szMsgBoxText);
  if (szBuf != '\0') strcat(szMsgBoxText, szBuf);
  WinMessageBox(HWND_DESKTOP, hwnd, szMsgBoxText, "", 0,
                MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
  return SUCCESS;
}


/**********************************************************************/
/*    Get a product record matched with the given product number      */
/**********************************************************************/
SHORT  GetProdInfo(struct prodRec *pProdRec)
{
  SHORT   i;
  ULONG   byteRead, prodRecMatch;
  HFILE   hProdFile;
  struct  prodRec  prodRecord[10];

  hProdFile = OpenDataFile(PRODFILE, READ_ONLY);
  prodRecMatch = 1;
  do {
    DosRead(hProdFile, (char *)prodRecord, sizeof(prodRecord), &byteRead );
    for (i=0; i<10; i++) {
      if (strcmp(prodRecord[i].prodNo, pProdRec->prodNo) == 0) {
        *pProdRec = prodRecord[i];
        prodRecMatch = 0;
        break;
      }
    }
    if (prodRecMatch == 0) break;
  } while (byteRead == sizeof(prodRecord));
  DosClose(hProdFile);
  return prodRecMatch;
}


/**********************************************************************/
/*    Get an order record matched with the given order number         */
/**********************************************************************/
SHORT  GetOrdrInfo(struct ordrRec *pOrdrRec)
{
  SHORT   i;
  ULONG   byteRead, ordrRecMatch;
  HFILE   hOrdrFile;
  struct  ordrRec  ordrRecord[10];

  hOrdrFile = OpenDataFile( ORDRFILE, READ_ONLY );
  ordrRecMatch = FAIL;
  do {
    DosRead(hOrdrFile, (char *)ordrRecord, sizeof(ordrRecord), &byteRead );
    for (i=0; i<10; i++) {
      if (strcmp(ordrRecord[i].ordrNo, pOrdrRec->ordrNo) == 0) {
        *pOrdrRec = ordrRecord[i];
        ordrRecMatch = SUCCESS;
        break;
      }
    }
    if (ordrRecMatch == SUCCESS) break;
  } while (byteRead == sizeof(ordrRecord));
  DosClose(hOrdrFile);
  return ordrRecMatch;
}
