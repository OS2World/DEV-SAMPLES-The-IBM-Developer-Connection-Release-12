/***************************************************************************/
/* XPRMDATA.H                                           11/15/95           */
/*    XPG4 Primer data structure and variable definitions.                 */
/***************************************************************************/
#include <time.h>
typedef int Boolean;
typedef unsigned long ulong;

/* For creating each block of records chain */
typedef struct Record
  {
     struct Record *next;
  } RECORD;

/* Records chain information */
#define LOCALE_LEN     16
struct Records
  {
     char acLocale[LOCALE_LEN];
     void *pTopRecord;
  };                                                       /* End structure*/
typedef struct Records RECORDS;

/* Product record */
#define PROD_NUM_LEN    8
#define PROD_NAME_LEN  32
struct ProdRec
  {
     struct ProdRec* next;
     char acNum[PROD_NUM_LEN];
     wchar_t awcName[PROD_NAME_LEN];
     double dUnitPrice;
  };                                                       /* End structure*/
typedef struct ProdRec PRODREC;
#define PROD_REC_MAXLEN  128
enum proditem { prodall, prodnum, prodname };            /* search category*/

/* Customer record */
#define CUST_NUM_LEN    8
#define CUST_NAME_LEN  48
#define CUST_ADDR_LEN 128
#define CUST_PHONE_LEN 16
struct CustRec
  {
     struct CustRec* next;
     wchar_t awcNum[CUST_NUM_LEN];
     wchar_t awcName[CUST_NAME_LEN];
     wchar_t awcAddr[CUST_ADDR_LEN];
     wchar_t awcPhone[CUST_PHONE_LEN];
  };                                                       /* End structure*/
typedef struct CustRec CUSTREC;
#define CUST_ITEM_MAXLEN  CUST_ADDR_LEN*MB_LEN_MAX
enum custitem { custall, custnum, custname, phone };     /* search category*/

/* Ordered products; used in orderEntryDlgProc */
struct OrderProd
  {
     PRODREC* pProd;
     USHORT   sQty;
     USHORT   sItemIndex;
  };                                                       /* End structure*/
typedef struct OrderProd ORDERPROD;

/* Order record; used in orderEntryDlgProc and queryOrderDlgProc */
#define MAX_ORDER_PROD 3
struct OrderRec
  {
     struct OrderRec* next;            /* used in queryOrderDlgProc only   */
     struct OrderRec* prev;            /* used in queryOrderDlgProc only   */
     char acOrderNum[8];
     CUSTREC *pCustRec;
     ORDERPROD prods[MAX_ORDER_PROD];
     USHORT usTotalQty;
     double dTotalPrice;
     char acEmpNum[8];
     time_t orderDate;
     time_t shipDate;
  };                                                       /* End structure*/
typedef struct OrderRec ORDERREC;
enum orderitem { all, ordernum, orderdate };

/* Order record file format; data is stored with this format in a file */
struct OrderFileRec
  {
     char acOrderNum[8];
     char acCustNum[8];
     struct
       {
          char   acProdNum[8];
          USHORT usQty;
       } prods[MAX_ORDER_PROD];                            /* End structure*/
     USHORT usTotalQty;
     double dTotalPrice;
     char acEmpNum[8];
     time_t orderDate;
     time_t shipDate;
  };                                                       /* End structure*/
typedef struct OrderFileRec ANORDER;
