/***************************************************************************/
/* XPRMOS2.H     XPG4 Primer for OS/2 WARP - V1.0       11/15/95           */
/*               Defines macros except macros related to resource file and */
/*               data files.                                               */
/***************************************************************************/

/*******************************/
/* Error code used in file I/O */
/*******************************/
#define ERR_FILE_OPEN          0x0001L
#define ERR_FILE_INVALID       0x0002L
#define ERR_FILE_IO            0x0003L
#define ERR_LOAD_RESOURCE      0x0004L
#define ERR_CREATE_HELP        0x0005L

/*******************************/
/* Function-like macro         */
/*******************************/
#define showMessageBox( hwnd, ulMsg, ulCaption, flStyle ) \
   WinMessageBox( HWND_DESKTOP, hwnd, \
                  acMsgTable[ulMsg], acMsgTable[ulCaption], \
                  (USHORT) NULL, MB_OK | flStyle );

#define clearField( hwnd, ulItemId ) \
   WinSetFocus( HWND_DESKTOP, WinWindowFromID (hwnd, ulItemId )); \
   WinSetDlgItemText( hwnd, ulItemId, "" );

/* max() and min() macros in VAC++ are not a part of XPG4. */
/* define the alternative macros */
#define getMax(a,b)     ((a>b)?a:b)
#define getMin(a,b)     ((a<b)?a:b)

/*******************************/
/* Define constatts            */
/*******************************/
/* subscripts of lDevCaps/lWinFont array */
#define CHAR_X  0
#define CHAR_Y  1
#define DISP_X  2
#define DISP_Y  3

/***********************************/
/* Info. used in View Order window */
/***********************************/

#define MAX_TITLE_LEN   32

#define LEN_PRODNAME    24
#define LEN_PRICE        9
#define LEN_QTY          5
#define LEN_SUB         10

#define POS_PRODNAME     4
#define POS_PRICE       28
#define POS_QTY         39
#define POS_SUB         51

#define LEN_TOTALQTY     6
#define LEN_TOTAL       16

#define POS_TOTALQTY    38
#define POS_TOTAL       45

#define NUMLINES        12
#define NUMCHARS        80

/* user-defined message used in ViewOrder sub window */
#define WM_USER_FONT       WM_USER+1   /* for subWindow to change font     */
#define WM_USER_NEW        WM_USER+2   /* for subWindow to change font     */
#define WM_USER_SETSCROLL  WM_USER+3   /* set max & pos of scroll          */

/* Enumerators                 */
enum outType { outProd, outPrice, outQty, outTotal };

/****************************************************/
/* Structures to store view order unique infomation */
/****************************************************/
struct ViewOrderSubInfo
  {
      size_t    szMaxTitle;            /* the maximum length of title.     */
      SHORT     sHscrollMax;
      SHORT     sVscrollMax;
      SHORT     sHscrollPos;
      SHORT     sVscrollPos;
      SHORT     cxCaps;
      SHORT     cyClient;
  };                                            /* Set by WinSetWindowPtr()*/
typedef struct ViewOrderSubInfo SUBINFO;

struct ViewOrderInfo
{
   HPS  hps;
   FONTDLG fontDlg;
   SUBINFO subInfo;
   HWND hwndSubWindow;
   HWND hwndPBNext;
   HWND hwndPBPrev;
   HWND hwndPBCancel;
};                                                         /* End structure*/
typedef struct ViewOrderInfo VOINFO;

