/************   OS/2 Application Primer Sample   **********************/
/*                                                                    */
/* Module Name : ORDENT.H                                             */
/* Version : 2.1 (12/24/93)                                           */
/*                                                                    */
/*  A sample program for "OS/2 DBCS Application Primer".              */
/*  The header file.                                                  */
/*                                                                    */
/**********************************************************************/

#define ID_MAIN          256

#define ID_MAIN_BMP      276
#define ID_CLR_CLNT      277

#ifdef INCL_NLS_INFO

#define MMDDYY           0
#define DDMMYY           1
#define YYMMDD           2

#define CUR_PLACE        0x01
#define CUR_SPACE        0x02
#define CUR_BEFORE       0
#define CUR_SP_YES       0x02

#endif


#ifdef INCL_MAIN_WND

#define CP_USE           437
#define CP_INTNL         850
#define CP_JPN           932
#define CP_JPNSAA        942

#define CP_OPT_USE       u
#define CP_OPT_JPN       j

#define ID_FILE          257
#define ID_FILE_NEW      258
#define ID_FILE_END      259
#define ID_CUST          260
#define ID_CUST_QRY      261
#define ID_CUST_REG      262
#define ID_RPRT          263
#define ID_RPRT_PRT      264
#define ID_HELP          265
#define ID_HELP_HLP      266
#define ID_HELP_EXT      267
#define ID_HELP_KEY      268
#define ID_HELP_ABT      269
#define ID_HELP_KEYS_HLP 270

#define IDH_MAIN                   280
#define IDH_MAIN_SUBTABLE          281
#define IDH_ORDR_DLG_SUBTABLE      282
#define IDH_LGON_DLG_SUBTABLE      283
#define IDH_CUSTREG_DLG_SUBTABLE   284
#define IDH_CUSTQRY_DLG_SUBTABLE   285
#define IDH_CUSTLIST_DLG_SUBTABLE  286
#define IDH_CUSTINFO_DLG_SUBTABLE  287
#define IDH_PRNT_DLG_SUBTABLE      288

#define ID_EXTENDED_HELP           ID_MAIN

#define IDD_MAIN_DLG     300
#define IDD_MAIN_WND     301
#define IDD_MAIN_LS      302


/* Strings in the string table */
#define ID_FRAM_TTL      0
#define IDD_MAIN_TTL_ON  1
#define IDD_MAIN_TTL_CN  2
#define IDD_MAIN_TTL_PN  3
#define IDD_MAIN_TTL_QT  4
#define IDD_MAIN_TTL_OD  5
#define IDD_MAIN_TTL_DD  6
#define IDD_MAIN_TTL_NM  7
#define IDD_MAIN_TTL_OQ  8
#define IDD_MAIN_TTL_OP  9
#define IDD_MAIN_TTL_TL  10
#define IDD_MAIN_TTL_RT  11
#define IDD_MAIN_TTL_CD  12
#define IDD_MAIN_TTL_TD  13
#define IDD_MAIN_ABT_H1  14
#define IDD_MAIN_ABT_T1  15
#define IDD_ORDR_MSG_CN  16
#define IDD_ORDR_MSG_UD  17
#define IDD_ORDR_MSG_ER  18
#define IDD_ORDR_MSG_MQ  19
#define IDD_ORDR_MSG_DT  20
#define IDD_CUST_MSG_RG  21
#define IDD_CUST_MSG_UD  22
#define IDD_CUST_MSG_ER  23
#define IDD_RPRT_MSG_TD  24
#define IDD_RPRT_MSG_PG  25
#define IDS_HELP_TTL     26
#define IDS_HELP_ABT_TTL 27
#define IDS_HELP_ABT_MSG 28

#define LS_1ST_COL       1
#define LS_2ND_COL       10
#define LS_3RD_COL       23
#define LS_4TH_COL       35
#define LS_5TH_COL       44
#define LS_6TH_COL       53
#define LS_END           60

#define IDB_BMP1         80

#endif

#define IDD_LGON_DLG     320

#ifdef INCL_LGON_DLG

#define IDD_LGON_TXT_MSG 321
#define IDD_LGON_TXT_ENO 322
#define IDD_LGON_BTN_OK  323
#define IDD_LGON_BTN_CN  324
#define IDD_LGON_SLE     325

#endif


#define IDD_TEST_DLG     350

#ifdef INCL_TEST_DLG

#define IDD_TEST_TXT     351
#define IDD_TEST_DLG_OK  352

#endif


#define IDD_ORDR_DLG     400

#ifdef INCL_ORDR_DLG

#define READY            0x00
#define CUST_ENT_NULL    0x01
#define CUST_NOT_FOUND   0x02
#define PROD_ENT_NULL    0x04
#define PROD_NOT_FOUND   0x08
#define PRCE_FMT_ERROR   0x10
#define QTY_FMT_ERROR    0x20
#define QTY_TOO_MUCH     0x40
#define DATE_FMT_ERROR   0x80

#define IDD_ORDR_TXT_EN  412
#define IDD_ORDR_ITX_EN  413
#define IDD_ORDR_TXT_CR  414
#define IDD_ORDR_ITX_CR  415
#define IDD_ORDR_TXT_CN  418
#define IDD_ORDR_LBX_CN  419
#define IDD_ORDR_TXT_CM  420
#define IDD_ORDR_ITX_CM  421

#define IDD_ORDR_TXT_ON  422
#define IDD_ORDR_ITX_ON  423
#define IDD_ORDR_TXT_VL  424
#define IDD_ORDR_ITX_VL  424
#define IDD_ORDR_TXT_AM  425
#define IDD_ORDR_TXT_PN  426
#define IDD_ORDR_TXT_LB  427

#define IDD_ORDR_ITX_PR1 436
#define IDD_ORDR_ITX_PR2 437
#define IDD_ORDR_ITX_PR3 438
#define IDD_ORDR_SLE_QT1 446
#define IDD_ORDR_SLE_QT2 447
#define IDD_ORDR_SLE_QT3 448

#define IDD_ORDR_LBX_PM  467

#define IDD_ORDR_BTN_CAN 470
#define IDD_ORDR_BTN_CNF 471
#define IDD_ORDR_BTN_ENT 472
#define IDD_ORDR_BTN_HLP 473


#define LCID_FIXED_FONT  29L

#endif

#define IDD_PRNT_DLG     700

#ifdef INCL_PRTR_DLG

#define IDD_PRNT_TXT_TTL  701
#define IDD_PRNT_TXT_FRM  702
#define IDD_PRNT_SLE_FRM  703
#define IDD_PRNT_TXT_TO   704
#define IDD_PRNT_SLE_TO   705
#define IDD_PRNT_BTN_ENT  706
#define IDD_PRNT_BTN_CAN  707
#define IDD_PRNT_BTN_HELP 708

#endif

#define IDD_CUSTQRY_DLG       1000

#ifdef INCL_CUSTQRY_DLG

#define IDD_CUSTQRY_SLE_KEY       1001
#define IDD_CUSTQRY_TXT_KEY       1002
#define IDD_CUSTQRY_BTN_QUERY     1003
#define IDD_CUSTQRY_BTN_CANCEL    1004
#define IDD_CUSTQRY_BTN_HELP      1005

#endif

#define IDD_CUSTLIST_DLG      1100

#ifdef INCL_CUSTLIST_DLG

#define IDD_CUSTLIST_LST_LISTBOX  1101
#define IDD_CUSTLIST_BTN_DESC     1102
#define IDD_CUSTLIST_BTN_CANCEL   1103
#define IDD_CUSTLIST_BTN_HELP     1104
#define IDD_CUSTLIST_TXT_CUSTNUM  1105
#define IDD_CUSTLIST_TXT_CUSTNAME 1106

#endif

#define IDD_CUSTINFO_DLG        1200

#ifdef INCL_CUSTINFO_DLG

#define IDD_CUSTINFO_TXT_CUSTNUM    1201
#define IDD_CUSTINFO_TXT_CUSTNAME   1202
#define IDD_CUSTINFO_TXT_CUSTADDR   1203
#define IDD_CUSTINFO_TXT_CUSTPHON   1204
#define IDD_CUSTINFO_ITX_CUSTNUM    1205
#define IDD_CUSTINFO_SLE_CUSTNAME   1206
#define IDD_CUSTINFO_MLE_CUSTADDR   1207
#define IDD_CUSTINFO_SLE_CUSTPHON   1208
#define IDD_CUSTINFO_BTN_CHANGE     1209
#define IDD_CUSTINFO_BTN_CANCEL     1210
#define IDD_CUSTINFO_BTN_HELP       1211

#endif

#define IDD_CUSTREG_DLG         1300

#ifdef INCL_CUSTREG_DLG

#define IDD_CUSTREG_TXT_CUSTNUM     1301
#define IDD_CUSTREG_TXT_CUSTNAME    1302
#define IDD_CUSTREG_TXT_CUSTADDR    1303
#define IDD_CUSTREG_TXT_CUSTPHON    1304
#define IDD_CUSTREG_ITX_CUSTNUM     1305
#define IDD_CUSTREG_SLE_CUSTNAME    1306
#define IDD_CUSTREG_MLE_CUSTADDR    1307
#define IDD_CUSTREG_SLE_CUSTPHON    1308
#define IDD_CUSTREG_BTN_REG         1309
#define IDD_CUSTREG_BTN_CANCEL      1310
#define IDD_CUSTREG_BTN_HELP        1311

#endif

#define LENGTH_QUANTITY         5
#define LENGTH_EMPNO            6
#define LENGTH_PRODNO          10
#define LENGTH_CUSTNUM          6
#define LENGTH_CUSTNUM_NULL   LENGTH_CUSTNUM+1
#define LENGTH_CUSTNAME        40
#define LENGTH_CUSTNAME_NULL  LENGTH_CUSTNAME+1
#define LENGTH_CUSTADDR       120
#define LENGTH_CUSTADDR_NULL  LENGTH_CUSTADDR+1
#define LENGTH_CUSTPHON        12
#define LENGTH_CUSTPHON_NULL  LENGTH_CUSTPHON+1

#define SUCCESS          1
#define FAIL             0

#define CALL_CUSTLIST    0x0000
#define SKIP_CUSTLIST    0x0001

#define FILE_ATTR_RO     0x0001
#define OPEN_FLAG_EXIST  0x0001

#define READ_ONLY        1
#define READ_WRITE       2

#define CUSTFILE  "CUSTFILE.DAT"
#define PRODFILE  "PRODFILE.DAT"
#define ORDRFILE  "ORDRFILE.DAT"

struct  custRec {
  CHAR    custNo[7];
  CHAR    custName[41];
  CHAR    custAddr[121];
  CHAR    custPhon[13];
};

struct  prodRec {
  CHAR    prodNo[7];
  CHAR    prodName[31];
  ULONG   unitPrice;
};

struct ordrProd {
  CHAR    prodNum[7];
  USHORT  qty;
};

struct  ordrRec {
  CHAR    ordrNo[7];
  CHAR    ordrDate[9];
  CHAR    custNo[7];
  struct  ordrProd  prod[3];
  CHAR    empNo[7];
  CHAR    shipDate[9];
};

MRESULT EXPENTRY LogonDlgProc(  HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY OrdrEntDlgProc(HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY ReptPrtDlgProc(HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY ClientWndProc( HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY CustQueryProc( HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY CustListProc(  HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY CustRegProc(   HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY CustInfoProc(  HWND, ULONG, MPARAM, MPARAM);

/*------------*/
/* ORDENT.C   */
/*------------*/
SHORT  PutErrorToLog(CHAR *, CHAR *);

/*------------*/
/* CUSTLIST.C */
/*------------*/
SHORT  MatchCustName(CHAR *, CHAR *);
SHORT  SetUpCustLBox(HWND);

/*------------*/
/* CUSTSUBR.C */
/*------------*/
  //Retrieve customer records with customer numbers in the structure
SHORT  GetCustInfo(struct custRec *);
  //Add new record to the end of customer file.
SHORT  PutCustInfo(struct custRec *);
  //Open a specified file with READ_ONLY/READ_ERITE state.
HFILE  OpenDataFile(CHAR *, USHORT);

/*------------*/
/* CUSTSUBW.C */
/*------------*/
SHORT   SetEntryFieldLength(HWND, USHORT);
SHORT   SetMEntryFieldLength(HWND, USHORT);
SHORT   BuildCustRec(HWND, USHORT, struct custRec *);

/*------------*/
/* MAINSUBR.C */
/*------------*/
  // Return character width as the 1st param and display width as the 2nd
  // param.
VOID    QueryDevCapability(HWND, LONG*, LONG *);
  // Check an employe number.
SHORT   CheckEmpNumber(CHAR *);
  // Draw bitmap to client area of the main window
SHORT   DrawBitMap(HPS);
  // Delete bitmap from the memory.
SHORT   DeleteBitmap(void);

/*------------*/
/* NLSFUNCS.C */
/*------------*/
  //Initialize structures to get country information
SHORT   InitCtryInfo(void);
  //Returns a formatted date string if it is shorter than the 2nd parameter.
SHORT   GetDate(CHAR *, SHORT);
  //Returns a date string in format YYMMDD
 // time_t  GetDateYYYYMMDD(CHAR *, SHORT);
 //Returns a currency character of the country information
SHORT   GetCurrency(CHAR *, SHORT);
 //Format a price with the country information
SHORT   FormatPrice(CHAR *);
 //Format a date with the country information
SHORT   FormatDate(CHAR *, SHORT);
 //Format a numeric string with the country information
SHORT   FormatInteger(CHAR *);
 //Modify a character string by padding characters.
SHORT PadCharacter(CHAR *, CHAR, SHORT);
 //Get the country code
USHORT  GetCtryCode(void);
 //Get a default language from the country info.
USHORT  GetDefLang(CHAR *);
 //Check a language ID with the process code page
USHORT  CheckCodepage(CHAR *);
 //Get a supported code pages from the Language ID
USHORT  GetCpList(CHAR *, USHORT *);
 //Reverse a given string
USHORT  StrReverse(CHAR *);

/*------------*/
/* ORDRSUBR.C */
/*------------*/
//Initialize each type of records
SHORT   InitCustRecord(struct custRec *);
SHORT   InitProdRecord(struct prodRec *);
SHORT   InitOrdrRecord(struct ordrRec *);
  //Format the order number
SHORT   FormatOrdrNo(CHAR *);
  //Retrieve a product record from the product number
SHORT   GetProdInfo(struct prodRec *);
  //Retrieve an order record from the product number
SHORT   GetOrdrInfo(struct ordrRec *);
  //Get a new record number
  //(= a number of records in product/customer/order files + 1)
SHORT   GetNewNumber(CHAR *, CHAR *);
SHORT   SetUpProductLBox(HWND);
SHORT   SetUpCustNoLBox(HWND);
SHORT   HandleCustInp(HWND, struct custRec *);
SHORT   HandleQtyInp(HWND, SHORT);
SHORT   HandleTotal(HWND, ULONG);
SHORT   IdentSelect(HWND, USHORT *);
SHORT   IdentNewItem(USHORT *, USHORT *);
SHORT   ReflectLBSelect(HWND, USHORT *, struct prodRec *);
SHORT   BuildOrdrRec(CHAR *, USHORT *,
                struct ordrRec *, struct custRec *, struct prodRec *);
SHORT   PutOrdrFile(struct ordrRec *);
SHORT   AlertUser(HWND, SHORT, CHAR *);

/*------------*/
/* PRINTER.C  */
/*------------*/
  //Generate data stream and print sales report of the specfied period.
SHORT   PrintReport(CHAR *, CHAR *);

/*------------*/
/* LGONDLG.C  */
/*------------*/
extern LONG devDispCaps[2], devCharCaps[2], devFontCaps[2];
extern CHAR szEmpNumber[20];

/*------------*/
/* MAINDLG.C  */
/*------------*/
extern HWND     hwndFrame;
extern HMODULE  hmodRsrc;
extern HAB      hab;

/*------------*/
/* ORDENT.C   */
/*------------*/
extern CHAR     LangID[];

/*------------*/
/* DBCS.C     */
/*------------*/
extern USHORT flDbcsCp;
