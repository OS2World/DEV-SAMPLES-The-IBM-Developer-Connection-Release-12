/************   OS/2 Application Primer Sample   **********************/
/*                                                                    */
/* Module Name : CUSTSUB.C                                            */
/* Version : 2.1 (12/24/93)                                           */
/*                                                                    */
/*    << This sample program is only for OS/2 V2.x. >>                */
/*                                                                    */
/* A sample program for "OS/2 DBCS Application Primer".               */
/* This source contains subroutines related to the manipulations of   */
/* the customer information.                                          */
/**********************************************************************/
#define    INCL_NOCOMMON
#define    INCL_DOS
#define    INCL_DOSFILEMGR
#define    INCL_WIN

#define    INCL_CUSTREG_DLG
#define    INCL_CUSTINFO_DLG
#define    INCL_CUSTLIST_DLG

#include   <os2.h>
#include   <string.h>
#include   <stdlib.h>
#include   <stdio.h>
#include   "ordent.h"
#include   "dbcs.h"

#define MATCH         1
#define UN_MATCH      0

extern CHAR     szKeyword[ LENGTH_CUSTNAME_NULL ];


/**********************************************************************/
/*    Get a customer record with a given customer number              */
/**********************************************************************/
SHORT  GetCustInfo(struct custRec *pCustRec)
{
  HFILE   hCustFile;
  LONG    lCustNum, lMoveByte;
  ULONG   newPtr;
  ULONG   ByteRead, rc;
  CHAR    *stopPos;

  lCustNum = strtol(pCustRec->custNo, &stopPos, 10U);
  lMoveByte = (lCustNum - 1) * sizeof(struct custRec);

  hCustFile = OpenDataFile(CUSTFILE, READ_ONLY);
  DosSetFilePtr(hCustFile, lMoveByte, 0, &newPtr);
  rc = DosRead(hCustFile, pCustRec, sizeof(struct custRec), &ByteRead);
  DosClose(hCustFile);
  return (rc ? FAIL : SUCCESS);
}


/**********************************************************************/
/*    Put a customer record into the customer file                    */
/**********************************************************************/
SHORT  PutCustInfo(struct custRec *pCustRec)
{
  HFILE   hCustFile;
  LONG    lCustNum, lMoveByte;
  ULONG   newPtr;
  ULONG   ByteWritten;
  CHAR    *stopPos;

  lCustNum = strtol(pCustRec->custNo, &stopPos, 10U);
  lMoveByte = (lCustNum - 1) * sizeof(struct custRec);
  hCustFile = OpenDataFile(CUSTFILE, READ_WRITE);
  DosSetFilePtr(hCustFile, lMoveByte, 0, &newPtr);
  DosWrite(hCustFile, pCustRec, sizeof(struct custRec), &ByteWritten);
  DosClose(hCustFile);
  return SUCCESS;
}


/**********************************************************************/
/*    Oppen a data file with a given file name and an open mode       */
/**********************************************************************/
HFILE OpenDataFile(CHAR szFileName[13], USHORT fMode)
{
  HFILE  hFile;
  ULONG  actnTaken, fileAttr, openFlag, openMode;

  switch (fMode) {
    case READ_ONLY:
      fileAttr = 0;
      openFlag = 0x0001;
      openMode = 0x0040;

    case READ_WRITE:
    default:
      fileAttr = 0;
      openFlag = 0x0011;
      openMode = 0x0042;
  }
  DosOpen(szFileName, &hFile, &actnTaken, (ULONG)0, fileAttr,
               openFlag, openMode, 0L);
  return hFile;
}


/**********************************************************************/
/*    Set-up a customer list box                                      */
/**********************************************************************/
SHORT SetUpCustLBox(HWND hwnd)
{
  HFILE   hCustFile;
  ULONG   ByteRead;
  struct  custRec custRecord;
  BOOL    fAllRecord, fPutRecord;
  CHAR    szListBoxRecord[LENGTH_CUSTNUM+5+LENGTH_CUSTNAME+1];

  if (strlen(szKeyword) > 0) {
    /*--- Initialize flags in a case which a keyword is given  --------*/
    fAllRecord = FALSE;   fPutRecord = FALSE;
  } else {
    /*--- Initialize flags in a case which a keyword is not given  ----*/
    fAllRecord = TRUE;    fPutRecord = TRUE;
  }
  hCustFile = OpenDataFile(CUSTFILE, READ_ONLY);
  /*--- Read customer records until the end ----------------------*/
  while (!DosRead(hCustFile, &custRecord, sizeof(struct custRec), &ByteRead)
         && ByteRead) {
    /*--------------------------------------------------------*/
    /*   If a keyword is given, compare customer names with   */
    /*   a given keyword.                                     */
    /*--------------------------------------------------------*/
    if (fAllRecord == FALSE) {
      if (MatchCustName(custRecord.custName, szKeyword) == MATCH)
         fPutRecord = TRUE;
      else
         fPutRecord = FALSE;
    }
    /*--------------------------------------------------------*/
    /*   If a customer name matches with a keyword or         */
    /*   a keyword is not given, put a customer number/name   */
    /*   into the List box.                                   */
    /*--------------------------------------------------------*/
    if (fPutRecord == TRUE) {
      strcpy(szListBoxRecord, custRecord.custNo);
      strcat(szListBoxRecord, "     ");
      strcat(szListBoxRecord, custRecord.custName);
      WinSendDlgItemMsg(hwnd, IDD_CUSTLIST_LST_LISTBOX, LM_INSERTITEM,
                        MPFROMSHORT(LIT_END), szListBoxRecord);
    }
  }
  DosClose(hCustFile);
  return SUCCESS;
}


/**********************************************************************/
/*    Compare a customer name with a given keyword using a simple     */
/*    string-matching algorithm.                                      */
/**********************************************************************/
SHORT MatchCustName(CHAR szText[], CHAR szKey[]) {

  #define DBCS_PLUS  2
  #define SBCS_PLUS  1
  USHORT  sMoveStart, i, j;

  if (isDBCS1st(szText[0]) == DBCS_1ST)  sMoveStart = DBCS_PLUS;
  else                                   sMoveStart = SBCS_PLUS;
  i=j=0;
  while((szText[i] != '\0') && (szKey[j] != '\0')) {
    if (szText[i] == szKey[j]) {
      i++; j++;
    } else {
      /*------------------------------------------------------------*/
      /*   If a value of the i-th byte of the target string is not  */
      /*   the same as a value of the j-th byte of the keyword,     */
      /*   move the pointer of the target string by 1 byte or       */
      /*   2 byte unit (sMoveStart) so that it points the next      */
      /*   character.                                               */
      /*------------------------------------------------------------*/
      i = i - j + sMoveStart;  j=0;
      if (isDBCS1st(szText[i]) ==  DBCS_1ST)  sMoveStart = DBCS_PLUS;
      else                                    sMoveStart = SBCS_PLUS;
    }
  }
  if (szKey[j] == '\0') return MATCH;

  return UN_MATCH;
}


/**********************************************************************/
/*    Set a byte length of entry fields                               */
/**********************************************************************/
SHORT SetEntryFieldLength(HWND hwnd, USHORT dlgId)
{
  ULONG idCustName, idCustPhon, idCustAddr;

  switch ( dlgId ) {
    case IDD_CUSTREG_DLG:
      idCustName = IDD_CUSTREG_SLE_CUSTNAME;
      idCustPhon = IDD_CUSTREG_SLE_CUSTPHON;
      idCustAddr = IDD_CUSTREG_MLE_CUSTADDR;
      break;
    case IDD_CUSTINFO_DLG:
      idCustName = IDD_CUSTINFO_SLE_CUSTNAME;
      idCustPhon = IDD_CUSTINFO_SLE_CUSTPHON;
      idCustAddr = IDD_CUSTINFO_MLE_CUSTADDR;
      break;
  }
  WinSendDlgItemMsg(hwnd, idCustName, EM_SETTEXTLIMIT,
                    MPFROMSHORT(LENGTH_CUSTNAME), 0L) ;
  WinSendDlgItemMsg(hwnd, idCustPhon, EM_SETTEXTLIMIT,
                    MPFROMSHORT(LENGTH_CUSTPHON), 0L) ;
  WinSendDlgItemMsg(hwnd, idCustAddr, MLM_SETTEXTLIMIT,
                    MPFROMLONG(LENGTH_CUSTADDR), 0L);
  return SUCCESS;
}


/**********************************************************************/
/*    Build a customer record from customer registration/update       */
/**********************************************************************/
SHORT BuildCustRec(HWND hwnd, USHORT dlgId, struct custRec *pCustRec)
{
  ULONG  idCustNum, idCustName, idCustPhon, idCustAddr;
  LONG   LenNum, LenName, LenPhon, LenAddr;
  CHAR   szNum[LENGTH_CUSTNUM_NULL],   szName[LENGTH_CUSTNAME_NULL],
         szAddr[LENGTH_CUSTADDR_NULL], szPhon[LENGTH_CUSTPHON_NULL];

  switch (dlgId) {
    case IDD_CUSTREG_DLG:
      idCustNum  = IDD_CUSTREG_ITX_CUSTNUM;
      idCustName = IDD_CUSTREG_SLE_CUSTNAME;
      idCustPhon = IDD_CUSTREG_SLE_CUSTPHON;
      idCustAddr = IDD_CUSTREG_MLE_CUSTADDR;
      break;
    case IDD_CUSTINFO_DLG:
      idCustNum  = IDD_CUSTINFO_ITX_CUSTNUM;
      idCustName = IDD_CUSTINFO_SLE_CUSTNAME;
      idCustPhon = IDD_CUSTINFO_SLE_CUSTPHON;
      idCustAddr = IDD_CUSTINFO_MLE_CUSTADDR;
      break;
  }
  LenNum  = WinQueryDlgItemTextLength(hwnd, idCustNum);
  LenName = WinQueryDlgItemTextLength(hwnd, idCustName);
  LenAddr = WinQueryDlgItemTextLength(hwnd, idCustAddr);
  LenPhon = WinQueryDlgItemTextLength(hwnd, idCustPhon);
  WinQueryDlgItemText(hwnd, idCustNum , LenNum+1, szNum);
  WinQueryDlgItemText(hwnd, idCustName, LenName+1, szName);
  WinQueryDlgItemText(hwnd, idCustAddr, LenAddr+1, szAddr);
  WinQueryDlgItemText(hwnd, idCustPhon, LenPhon+1, szPhon);
  strncpy(pCustRec->custNo,   szNum,  LENGTH_CUSTNUM_NULL);
  strncpy(pCustRec->custName, szName, LENGTH_CUSTNAME_NULL);
  strncpy(pCustRec->custAddr, szAddr, LENGTH_CUSTADDR_NULL);
  strncpy(pCustRec->custPhon, szPhon, LENGTH_CUSTPHON_NULL);
  return SUCCESS;
}
