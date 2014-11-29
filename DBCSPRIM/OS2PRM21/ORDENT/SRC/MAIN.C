/************   OS/2 Application Primer Sample   **********************/
/*                                                                    */
/* Module Name : MAIN.C                                               */
/* Version : 2.1 (12/24/93)                                           */
/*                                                                    */
/*    << This sample program is only for OS/2 V2.x. >>                */
/*                                                                    */
/* A sample program for "OS/2 DBCS Application Primer".               */
/* Window procedures for the log-on window and the main window, and   */
/* the subroutines related to them.                                   */
/*                                                                    */
/**********************************************************************/
#define    INCL_NOCOMMON
#define    INCL_DOS
#define    INCL_WIN
#define    INCL_GPICONTROL
#define    INCL_DOSFILEMGR
#define    INCL_GPILCIDS
#define    INCL_GPIPRIMITIVES
#define    INCL_GPIBITMAPS
#define    INCL_NLS

#define    INCL_MAIN_WND
#define    INCL_NLS_INFO
#define    INCL_LGON_DLG
#define    INCL_TEST_DLG
#define    INCL_ORDR_DLG
#define    INCL_CINF_DLG
#define    INCL_PINF_DLG
#define    INCL_PRTR_DLG

#include   <os2.h>
#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   "ordent.h"
#include   "ordhelp.h"
#include   "dbcs.h"

HELPINIT   hmiHelpData;
HWND       hwndHelpInstance;

HMODULE  hmodRsrc;
HAB      hab;
HMQ      hmq;
QMSG     qmsg;
HWND     hwndFrame, hwndClient;
CHAR     szClientWndClass[] = "Order Entry Program",
         szWindowTitle[40], szEmpNumber[20], NameRsrcDll[9],
         LangID[4];   // LangID(3 bytes) ENG, JPN, KOR, CHT, CHS ....
LONG     lInitPosX, lInitPosY, lInitPosW, lInitPosH;
ULONG    flCreate  = FCF_TITLEBAR | FCF_MINMAX | FCF_SYSMENU | FCF_MENU |
                     FCF_TASKLIST | FCF_ACCELTABLE | FCF_SIZEBORDER |
                     FCF_ICON,
         frameStyle = 0;
LONG     devDispCaps[2], devCharCaps[2];
HELPINIT hmiHelpData;
HWND     hwndHelpInstance;

CHAR       szKeyword[LENGTH_CUSTNUM_NULL];

/*----------------------------------------------------------------------*/
/*  Dialog procedure for the main dialog. Show a prompt for handling    */
/*  orders, inquirying customer information and printing sales report.  */
/*----------------------------------------------------------------------*/
MRESULT EXPENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  ULONG   command, rc;
  CHAR     szAboutTitle[41], szAboutMsg[80];
  HPS      hps;
  HPOINTER hptrOld, hptrWait, hptrCurrent;

  switch(msg) {
    case WM_COMMAND:
      command = SHORT1FROMMP(mp1);
      switch(command) {
        case ID_FILE_NEW:
          WinSendDlgItemMsg(hwnd, (ULONG)IDD_MAIN_LS, (ULONG)LM_SELECTITEM,
                            MPFROMSHORT(LIT_NONE), MPFROMSHORT(FALSE));
          WinDlgBox(HWND_DESKTOP, hwnd, OrdrEntDlgProc,
                    hmodRsrc, IDD_ORDR_DLG, NULL );
          break;

        case ID_CUST_QRY :
          rc = WinDlgBox(HWND_DESKTOP, hwnd, CustQueryProc,
                         hmodRsrc, IDD_CUSTQRY_DLG, NULL );
          if (rc == CALL_CUSTLIST)
            WinDlgBox(HWND_DESKTOP, hwnd, CustListProc,
                      hmodRsrc, IDD_CUSTLIST_DLG, NULL );
          break;

        case ID_CUST_REG:
          WinDlgBox(HWND_DESKTOP, hwnd, CustRegProc,
                    hmodRsrc, IDD_CUSTREG_DLG, NULL );
          break;

        case ID_HELP_ABT:
          WinLoadString(hab, hmodRsrc, IDS_HELP_ABT_TTL,
                        sizeof(szAboutTitle), szAboutTitle);
          WinLoadString(hab, hmodRsrc, IDS_HELP_ABT_MSG,
                        sizeof(szAboutMsg), szAboutMsg);
          WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szAboutMsg,
                        szAboutTitle, 1, MB_OK);
          break;

        case ID_HELP_HLP:
          if (hwndHelpInstance)
            WinSendMsg(hwndHelpInstance, HM_DISPLAY_HELP, 0L, 0L);
          break;

        case ID_RPRT_PRT:
          WinDlgBox(HWND_DESKTOP, hwnd, ReptPrtDlgProc,
                    hmodRsrc, IDD_PRNT_DLG, NULL );
          break;

        default:
          WinDefWindowProc(hwnd, msg, mp1, mp2);
      } break;

    case HM_QUERY_KEYS_HELP:
      return (MRESULT) ID_HELP_KEYS_HLP;
      break;

    case WM_PAINT:
      hps = WinBeginPaint(hwnd, 0, NULL);
      GpiErase(hps);
      hptrOld = WinQueryPointer(HWND_DESKTOP);
      hptrWait = WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE);
      hptrCurrent = hptrWait;
      WinSetPointer(HWND_DESKTOP, hptrWait);
      DrawBitMap(hps);
      WinSetPointer(HWND_DESKTOP, hptrOld);
      WinEndPaint(hps);
      break;

    case WM_DESTROY:
      DeleteBitmap();
      break;

    case WM_CLOSE:
      WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
      break;

  }
  return WinDefWindowProc(hwnd, msg, mp1, mp2);
}


/*--------------------------------------------------------------------*/
/* Dialog procedure for the logon dialog. Ask the operator to input   */
/* his employee number. When the employee number is entered, pass the */
/* number to the CheckEmpNumber function.                             */
/*--------------------------------------------------------------------*/
MRESULT EXPENTRY LogonDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  SWP   dlgPos;
  LONG  lDlgPosX, lDlgPosY;

  switch(msg) {

    case WM_INITDLG:
      HelpAssociate(hwnd);
      /*--- Set dialog position in the center of a screen -------*/
      WinQueryWindowPos(hwnd, &dlgPos);
      lDlgPosX = (devDispCaps[0] - dlgPos.cx)/2;
      lDlgPosY = (devDispCaps[1] - dlgPos.cy)/2;
      WinSetWindowPos(hwnd, HWND_TOP,
                      lDlgPosX, lDlgPosY, dlgPos.cx, dlgPos.cy,
                      SWP_SHOW | SWP_ACTIVATE | SWP_MOVE );
      WinSetFocus(hwnd, WinWindowFromID(hwnd, IDD_LGON_SLE));
      break;

    case WM_COMMAND:
      switch(COMMANDMSG(&msg)->cmd) {

        case IDD_LGON_BTN_OK:
          WinQueryWindowText(WinWindowFromID(hwnd, IDD_LGON_SLE),
                             sizeof(szEmpNumber)-1, szEmpNumber);
          if (CheckEmpNumber(szEmpNumber) == SUCCESS) {
            HelpDestroyAssociate(hwnd);
            WinDismissDlg(hwnd, (ULONG)TRUE);
          } else DosBeep(1380, 100);
            break;

         case IDD_LGON_BTN_CN:
           HelpDestroyAssociate(hwnd);
           szEmpNumber[0] = '\0';
           WinDismissDlg(hwnd, TRUE);

         default:
           return WinDefDlgProc(hwnd, msg, mp1, mp2);
       } break;

    default:
      return WinDefDlgProc(hwnd, msg, mp1, mp2);
  }
  return FALSE;
}


/**********************************************************************/
/*   Query device information for display and character size to       */
/*   achieve code device independent.                                 */
/**********************************************************************/
VOID QueryDevCapability(HWND hwndFram, LONG *dispCaps, LONG *charCaps)
{
  HPS hps;
  FONTMETRICS fm;

  dispCaps[0] = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
  dispCaps[1] = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
  hps = WinGetPS(hwndFram);
  GpiQueryFontMetrics(hps, (LONG)sizeof(fm), &fm);
  WinReleasePS(hps);
  charCaps[0] = fm.lAveCharWidth;
  charCaps[1] = fm.lMaxBaselineExt+fm.lExternalLeading;
}


/**********************************************************************/
/*   Check an employee number only by its length.                     */
/**********************************************************************/
SHORT CheckEmpNumber(CHAR szENum[])
{
  if (strlen(szENum) == 6) return SUCCESS;
  else                     return FAIL;
}


/**********************************************************************/
/*   Retrieve colors for drawing a bitmap                             */
/**********************************************************************/
static LONG GetClientColor(void)
{
  CHAR    szClientColor[20];

  WinLoadString(hab, hmodRsrc, ID_CLR_CLNT, sizeof(szClientColor),
                szClientColor);
  if (strcmp(szClientColor, "CLR_BLUE") == 0)
    return (LONG)CLR_BLUE;
  else if (strcmp(szClientColor, "CLR_DARKRED") == 0)
    return (LONG)CLR_DARKRED;
  else return (LONG)CLR_ERROR;
}


/**********************************************************************/
/*   Draws bitmap at client area of the main window.                  */
/**********************************************************************/

static HBITMAP hbm =  0L;

SHORT DrawBitMap(HPS hps)
{
  RECTL   rect;
  static  LONG    lClientColor;

  if (hbm == 0){
    hbm = GpiLoadBitmap(hps, hmodRsrc, ID_MAIN_BMP, 0L, 0L);
    lClientColor = GetClientColor();
  }
  WinQueryWindowRect(hwndClient, &rect);
  WinDrawBitmap(hps, hbm, NULL, (PPOINTL)&rect, (LONG)lClientColor,
                (LONG)CLR_BACKGROUND, DBM_STRETCH);
  return SUCCESS;
}


/**********************************************************************/
/*   Delete bitmap from memory.                                       */
/**********************************************************************/
SHORT DeleteBitmap(void)
{
  if (hbm) GpiDeleteBitmap(hbm);
  return SUCCESS;
}


/**********************************************************************/
/*   Write error messages to ORDENT.ERR when parameters are invalid.  */
/**********************************************************************/
SHORT PutErrorToLog(CHAR szErrorNum[], CHAR szParm[])
{
  FILE *stream;
  CHAR szErrorMsg[80], TempNum[3];
  USHORT  c, i;

  stream = fopen("orderr.msg", "r");
  while (fscanf(stream, "%3s", TempNum) != EOF) {
    if (strcmp(szErrorNum, TempNum) == 0) {
      while (c = fgetc(stream) != '\n') {
        memcpy(szErrorMsg+i, &c, 1);
        i++;
      }
      szErrorMsg[i]   = '\\';
      szErrorMsg[i+1] = 'n';
      szErrorMsg[i+2] = '\0';
      break;
    } else {
      while (c = fgetc(stream) != '\n');
    }
  }
  fclose(stream);
  stream = fopen("orderr.log", "a");
  fprintf(stream, szErrorMsg, szParm);
  fprintf(stream, "The syntax is ORDENT [ /L JPN|ENG ].\n");
  fclose(stream);
  return SUCCESS;
}


VOID HelpInit(CHAR LangID[])
{
  extern   HAB      hab;
  extern   HMODULE  hmodRsrc;
  CHAR     szHelpTitle[31], szHelpName[13], *temp;

  hmiHelpData.cb = (ULONG)sizeof(HELPINIT);
  hmiHelpData.ulReturnCode = 0L;
  hmiHelpData.pszTutorialName = NULL;
  hmiHelpData.phtHelpTable = (PHELPTABLE) (0xffff0000 | IDH_MAIN);
  hmiHelpData.hmodHelpTableModule = hmodRsrc;
  hmiHelpData.hmodAccelActionBarModule = 0L;
  hmiHelpData.idAccelTable = 0L;
  hmiHelpData.idActionBar = 0L;
  hmiHelpData.fShowPanelId = CMIC_HIDE_PANEL_ID;
  /*--- Loading a HELP TITLE from a resource DDL file ------------*/
  WinLoadString(hab, hmodRsrc, IDS_HELP_TTL, sizeof(szHelpTitle),
                szHelpTitle);
  hmiHelpData.pszHelpWindowTitle = szHelpTitle;
  /*--- Set the HLP filename ORDRxxx.HLP ---------------------*/
  temp = strcpy(szHelpName, "ORDR");
  temp = strcat(szHelpName, LangID);
  temp = strcat(szHelpName, ".HLP");
  hmiHelpData.pszHelpLibraryName = szHelpName;
  hwndHelpInstance = WinCreateHelpInstance(hab, &hmiHelpData);

  if (hmiHelpData.ulReturnCode)
    WinDestroyHelpInstance(hwndHelpInstance);
  return;
}


VOID HelpAssociate(HWND hwndAssociateWindow)
{
  if (hwndHelpInstance)
    WinAssociateHelpInstance(hwndHelpInstance, hwndAssociateWindow);
  return;
}


VOID HelpDestroyAssociate(HWND hwndAssociateWindow)
{
  WinAssociateHelpInstance(0L, hwndAssociateWindow);
  return;
}


void main(int argc, char *argv[])
{
  InitDBCSTable();
  InitCtryInfo();
  /*----------------------------------------------------------*/
  /*    Load a resource DLL file and link to a help file      */
  /*----------------------------------------------------------*/
  if (argc < 2) {
    /*--- Determin a language ID if no parameters are given ---*/
    GetDefLang(LangID);
  } else {
    /*--- Validate parameters given to a command line ------*/
    switch (argv[1][0]) {
      case '/':
        switch (argv[1][1]) {
          case 'l':
          case 'L':
            if (argc > 2) {
              strupr(argv[2]);
              strcpy(LangID, argv[2]);
            } else {
              PutErrorToLog("01", "");
              exit(EXIT_FAILURE);
            }
            break;
          default :
            PutErrorToLog("02", argv[1]);
            exit(EXIT_FAILURE);
            break;
        }
        break;
      default:
        PutErrorToLog("02", argv[1]);
        exit(EXIT_FAILURE);
        break;
    }
  }
  /*--- Checking a language parameter with the current code page ---*/
  if (CheckCodepage(LangID) != SUCCESS) {
    PutErrorToLog("03", "");
    exit(EXIT_FAILURE);
  } else {
    strncpy(NameRsrcDll, "RSRC", 9);
    strcat(NameRsrcDll, LangID);
    DosLoadModule(NULL, 0, NameRsrcDll, &hmodRsrc);
  }

  hab = WinInitialize(0);
  hmq = WinCreateMsgQueue(hab, 0);
  WinRegisterClass(hab, szClientWndClass, ClientWndProc, CS_SIZEREDRAW, 0);
  hwndFrame = WinCreateStdWindow(HWND_DESKTOP, frameStyle, &flCreate,
                                 szClientWndClass, "", 0L, hmodRsrc,
                                 ID_MAIN, &hwndClient);
  HelpInit(LangID);
  HelpAssociate(hwndFrame);
  /*--- Set the window title retrieved from the resource DLL file ----*/
  WinLoadString(hab, hmodRsrc, ID_FRAM_TTL, sizeof(szWindowTitle),
                szWindowTitle);
  WinSetWindowText(hwndFrame, szWindowTitle);
  /*--- Put a frame window in the center of a display screen -----*/
  QueryDevCapability(hwndFrame, devDispCaps, devCharCaps);
  lInitPosW = devCharCaps[0]*60;
  lInitPosH = devCharCaps[1]*20;
  lInitPosX = (devDispCaps[0]-lInitPosW)/2;
  lInitPosY = (devDispCaps[1]-lInitPosH)/2;

  /*--- Call the Log-On dialog ----------------------------------*/
  WinDlgBox(HWND_DESKTOP, hwndClient, LogonDlgProc, hmodRsrc,
            IDD_LGON_DLG, NULL);
  if (CheckEmpNumber(szEmpNumber) == FAIL) {
    if (hwndHelpInstance)
      WinDestroyHelpInstance(hwndHelpInstance);
    WinDestroyWindow(hwndFrame);
    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
    DosFreeModule(hmodRsrc);
    return;
  }

  WinSetWindowPos(hwndFrame, HWND_TOP,
                  lInitPosX, lInitPosY, lInitPosW, lInitPosH,
                  SWP_SHOW | SWP_ACTIVATE | SWP_SIZE | SWP_MOVE );

  while(WinGetMsg(hab, &qmsg, 0, 0, 0))
    WinDispatchMsg(hab, &qmsg);
  if (hwndHelpInstance)
      WinDestroyHelpInstance(hwndHelpInstance);
  WinDestroyWindow(hwndFrame);
  WinDestroyMsgQueue(hmq);
  WinTerminate(hab);
  DosFreeModule(hmodRsrc);
}
