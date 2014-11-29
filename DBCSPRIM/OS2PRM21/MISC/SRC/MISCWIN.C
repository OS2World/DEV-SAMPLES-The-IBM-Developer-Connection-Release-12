/*** OS/2 Application Primer "MISC Sample Secondary Window Procedures"*/
/*                                                                    */
/* Program name : MISCWIN.C                                           */
/* Version : 2.1 (12/24/93)                                           */
/*                                                                    */
/*    << This sample program is only for DBCS OS/2 V2.x. >>           */
/*                                                                    */
/*    Window/Dialog procedures for the ASCII-EBCDIC conversion and    */
/*    word break sample of the Sample Miscellenaeus program.          */
/*                                                                    */
/**********************************************************************/

#define INCL_WIN
#define INCL_DOS
#define INCL_GPI
#define INCL_NLS

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "miscrsc.h"
#include "miscprg.h"

extern HAB hab;
extern QMSG qmsg;
extern HMQ hmq;
extern MRESULT EXPENTRY cpConvDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT EXPENTRY editorWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern USHORT _Far16 _Pascal TrnsDt(struct TrnsDtParm *);

/*** Dialog Procedure for "ASCII-to-EBCDIC Code Conversion" ***********/
/*                                                                    */
/* function name : cpConvDlgProc                                      */
/*                                                                    */
/*    Converts an ASCII string to an EBCDIC string utilizing          */
/*    both WinCpTranslateString and TrnsDt OS/2 services, and         */
/*    displays its result in hexdecimal.                              */
/*                                                                    */
/*    Shows how to use WinCpTranslateString and TrnsDt.               */
/**********************************************************************/

#pragma seg16(Tparm)
struct TrnsDtParm Tparm;

MRESULT EXPENTRY cpConvDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    int i,
        compRC;
    CHAR szListBoxItem[5];
    static USHORT *pPcCodePage, idCtry;
    static UCHAR  szBufIn[SRC_LENGTH+1], szBufOut[SRC_LENGTH*2+1];
    static CHAR   szBufInx[SRC_LENGTH*2+1], szBufOutx[SRC_LENGTH*4+1];
    static CHAR   szCurrentHost[5];
    CHAR *rc;
    static HCODE *HCodeTop;
    static HCODE *p;
    static HCODE *pHost;
    FILE *CPfile;
    CHAR szStr[64],
         szTemp[3][32],
         szComp[3][10] = {"dbcs", "sbcs", "combined"},
         szfileName[16] = "cptable",
         szlangID[2];
    CPTYPE flag[3] = {dbcs, sbcs, combined};
    USHORT queryCtry(CHAR*);
    void ErrorMsg(HWND, ULONG, ULONG);
    void freeData(HCODE *);

    switch(msg)
    {
        case WM_COMMAND:
          switch (SHORT1FROMMP(mp1))
          {
            case PB_EXE:

              /*----- Query the source string ----------------------*/
              WinQueryWindowText(WinWindowFromID(hwnd, (ULONG)EF_STRING),
                                 sizeof(szBufIn), szBufIn);

              for (i=0; szBufIn[i] != '\0'; i++)
                 sprintf(szBufInx+i*2, "%02X", szBufIn[i]);

              WinSetWindowText(WinWindowFromID(hwnd, (ULONG)EF_PC), szBufInx);

              /*----- Translate with WinCpTranslateString ----------*/
              WinCpTranslateString(WinQueryAnchorBlock(hwnd),
                               (ULONG)*pPcCodePage,        /* source code page  */
                               szBufIn,                    /* address of source */
                               (ULONG)atol(szCurrentHost), /* target code page  */
                               (ULONG)sizeof(szBufOut),    /* length of target  */
                               szBufOut);                  /* address of target */

              for (i = 0; szBufOut[i] != '\0'; i++)
                  sprintf(szBufOutx+i*2, "%02X", szBufOut[i]);

              WinSetWindowText(WinWindowFromID(hwnd, (ULONG)EF_HOSTWIN), szBufOutx);

              /*----- Translate with TrnsDt ------------------------*/

              Tparm.Length    = sizeof(Tparm);
              Tparm.exit      = 0;
              Tparm.SourceLen = strlen(szBufIn);
              Tparm.pSource   = szBufIn;
              Tparm.TargetLen = sizeof(szBufOut);
              Tparm.pTarget   = szBufOut;
              Tparm.id = 0;
              Tparm.SourceCP = *pPcCodePage;
              Tparm.TargetCP = atoi(szCurrentHost);

              if ((*pHost).CPflag == combined)
                 /*--------------------------------------------------*/
                 /*  Options for a combined code page :              */
                 /*                                                  */
                 /*        1111 11                                   */
                 /*   bit  5432 1098 7654 3210                       */
                 /*        -------------------                       */
                 /*        0000 0001 0000 0100        0x0104         */
                 /*                                                  */
                 /*         Bit 8 (on) : SO/SI in a target string    */
                 /*         Bit 2 (on) : Use the IBM-defined table   */
                 /*         Bit 0 (off): No SO/SI in a source string */
                 /*--------------------------------------------------*/
                 Tparm.Options = 0x0104;
              else
                 /*--------------------------------------------------*/
                 /*  Options for a SBCS or a DBCS code page :        */
                 /*                                                  */
                 /*        1111 11                                   */
                 /*   bit  5432 1098 7654 3210                       */
                 /*        -------------------                       */
                 /*        0000 0000 0000 0100        0x0004         */
                 /*                                                  */
                 /*         Bit 8 (off): No SO/SI in a target string */
                 /*         Bit 2 (on) : Use the IBM-defined table   */
                 /*         Bit 0 (off): No SO/SI in a source string */
                 /*--------------------------------------------------*/
                 Tparm.Options = 0x0004;

              TrnsDt(&Tparm);

              for (i = 0; szBufOut[i] != '\0'; i++)
                  sprintf(szBufOutx+i*2, "%02X", szBufOut[i]);

              WinSetWindowText(WinWindowFromID(hwnd, (ULONG)EF_HOSTTRAN), szBufOutx );
              break;

            case PB_CAN:

              /* Dismiss the dialog box */
              WinDismissDlg (hwnd,PB_CAN);
              break;
          }
          break;

        case WM_CONTROL:

            switch (SHORT1FROMMP(mp1))
            {
              case CB_HCODE:
                switch(SHORT2FROMMP(mp1)) {
                  case CBN_EFCHANGE:
                    if (WinQueryDlgItemText(hwnd, (ULONG)CB_HCODE,
                           sizeof(szCurrentHost), (PSZ)szCurrentHost) == 0)
                      return;

                    HCodeTop = p;
                    while (HCodeTop != NULL){
                      if (strcmp(HCodeTop->usCP, szCurrentHost) == 0){
                        WinSetWindowText(
                               WinWindowFromID(hwnd, (ULONG)EF_HCTEXT),
                               HCodeTop->descr);
                        pHost = HCodeTop;
                        break;
                      }
                      HCodeTop = HCodeTop->next;
                    }
                    break;
                  default:
                    break;
                }
                break;
              default:
                break;
            }
            break;

        case WM_INITDLG:

            pPcCodePage = PVOIDFROMMP(mp2);

            if ((idCtry = queryCtry(szlangID)) > 2){
              /* If country != J/K/C, dismiss the dialog box */
              ErrorMsg(hwnd, ERR_TITLE, ERR_CTRYCODE);
              WinDismissDlg (hwnd,PB_CAN);
              return;
            }

            szlangID[1] = '\0';

            strcat(szfileName, szlangID);
            strcat(szfileName, ".dat");
            CPfile = fopen(szfileName,"r");
            if (CPfile == NULL){
              ErrorMsg(hwnd, ERR_TITLE, ERR_FILE);
              WinDismissDlg(hwnd, PB_CAN);
              return;
            }

            p = HCodeTop = malloc(sizeof(*HCodeTop));

            rc = fgets(szStr, sizeof(szStr), CPfile);
            while (rc != NULL){
              strcpy(szTemp[0], strtok(szStr,","));
              for (i = 1; i < 3; i++)
                strcpy(szTemp[i], strtok(NULL, ","));

              for (i = 0; i < 3; i++){
                if ( (compRC = strcmp(szTemp[2], szComp[i])) == 0){
                  strcpy(HCodeTop->usCP, szTemp[0]);
                  strcpy(HCodeTop->descr, szTemp[1]);
                  HCodeTop->CPflag = flag[i];
                  break;
                }
                if (i == 2){
                  ErrorMsg(hwnd, ERR_TITLE, ERR_CPFLAG);
                  WinDismissDlg(hwnd, PB_CAN);
                  fclose(CPfile);
                  freeData(p);
                  return;
                }
              } /* END of FOR loop */

              if ( (rc = fgets(szStr, sizeof(szStr), CPfile)) != NULL){
                HCodeTop->next = malloc(sizeof(*HCodeTop));
                HCodeTop = HCodeTop->next;
                HCodeTop->next = NULL;
              }
            }/* END of WHILE loop */

            HCodeTop = p;

            while(HCodeTop != NULL){
              strcpy(szListBoxItem, HCodeTop->usCP);
              WinSendMsg(WinWindowFromID(hwnd, CB_HCODE), LM_INSERTITEM,
                   MPFROM2SHORT(LIT_END, 0), MPFROMP(szListBoxItem));
              HCodeTop = HCodeTop->next;
            }

            WinSetDlgItemShort(hwnd, (ULONG)EF_PCCP, *pPcCodePage, FALSE);
            HCodeTop = p;
            WinSetDlgItemText(hwnd, CB_HCODE, HCodeTop->usCP);
            WinSendDlgItemMsg(hwnd, EF_STRING, EM_SETTEXTLIMIT,
                                  MPFROM2SHORT(SRC_LENGTH,0), 0L);

            break;

        case WM_PAINT:

            /* Execute default processing */
            return(WinDefDlgProc(hwnd, msg, mp1, mp2));
            break;
        default:
            return(WinDefDlgProc(hwnd,msg,mp1,mp2));
    }
    return(FALSE);
}

static USHORT queryCtry(CHAR szlangID[])
{
  COUNTRYCODE Country;
  COUNTRYINFO CtryBuffer;
  ULONG length;
  APIRET rc;

  Country.country = Country.codepage = 0;

  rc = DosQueryCtryInfo(sizeof(CtryBuffer), &Country, &CtryBuffer, &length);

  if (rc == 0)
    switch (CtryBuffer.country) {
      case 81:                            // Set Japanese env.
        szlangID[0] = 'J';
        return(0);
      case 82:                            // Set Korean env.
        szlangID[0] = 'H';
        return(1);
      case 88:                            // Set T-Chinese env.
        szlangID[0] = 'T';
        return(2);
      default:                             // No DBCS code page
        return(3);
   }
   else return(3);
}

void ErrorMsg(HWND hwnd, ULONG ulTitleID, ULONG ulMsgID)
{
  CHAR szTitle[32];
  CHAR szMsg[256];

  WinLoadString(hab, NULLHANDLE, ulTitleID, sizeof(szTitle), szTitle);
  WinLoadString(hab, NULLHANDLE, ulMsgID, sizeof(szMsg), szMsg);

  WinMessageBox(HWND_DESKTOP, hwnd, szMsg, szTitle, 0,
                MB_ICONHAND | MB_OK);
}

static void freeData(HCODE *data)
{
  HCODE *cur;
  HCODE *prev;

  cur = data;

  while (cur != NULL){
    prev = cur;
    cur = prev->next;
    free(prev);
  }
}

/*** Dialog Procedure for "Word-Break sample" *************************/
/*                                                                    */
/* function name : wordDlgProc                                        */
/*                                                                    */
/*    Load several strings from the resource file and display         */
/*    them on a dialog window with/without word-break option.         */
/*                                                                    */
/*    Shows how to use WinDrawString and DT_WORDBREAK flag.           */
/**********************************************************************/

void GetFontSize(HPS hps, LONG *lWidth, LONG *lHeight);
MRESULT EXPENTRY wordDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 HWND hwndSTX;
 static HWND hwndMenu;
 static HPS hps;
 static RECTL rcl;
 LONG lPresParam;

 LONG lStrLength,
      lTtlDrawn,
      lDrawn,
      lYcoord;
 ULONG stringID[NUM_EXMPL] = {IDS_TEXT1, IDS_TEXT2, IDS_TEXT3,
                              IDS_TEXT4, IDS_TEXT5};
 CHAR szExmplNo[NUM_EXMPL][10] = {"Example 1", "Example 2", "Example 3",
                                  "Example 4", "Example 5"};
 static CHAR szExmpl[NUM_EXMPL][256];
 static LONG lCharWidth,
             lCharHeight;
 static USHORT flWBreak = DT_WORDBREAK;

 USHORT i;

 switch(msg)
 {
   case WM_COMMAND:
     switch (SHORT1FROMMP(mp1))
     {
       case MID_BREAKON:
         /* Alternate ON-OFF status of flWBreak */
         flWBreak = (USHORT)(flWBreak == DT_WORDBREAK) ?
                    (USHORT)0 : DT_WORDBREAK;
         WinSendMsg(hwndMenu, MM_SETITEMATTR,MPFROM2SHORT(MID_BREAKON, TRUE),
                    MPFROM2SHORT(MIA_CHECKED, (flWBreak == DT_WORDBREAK) ?
                                               MIA_CHECKED : ~MIA_CHECKED));

         lYcoord = rcl.yTop;
         WinFillRect(hps, &rcl, CLR_WHITE);

         for (i = 0; i < NUM_EXMPL; i++){
           WinDrawText(hps, strlen(szExmplNo[i]), szExmplNo[i], &rcl,
                       0L, 0L, DT_TOP | DT_LEFT | DT_TEXTATTRS);
           rcl.yTop -= lCharHeight;

           lStrLength = (LONG)strlen(szExmpl[i]);

           /*
              WinDrawText until all the characters in the string are
              drawn.  (If flWBreak = OFF, the following for-loop is
              executed only once)
           */
           for (lTtlDrawn = 0; lTtlDrawn != lStrLength;
                                                  rcl.yTop -= lCharHeight){
             lDrawn = WinDrawText(hps, lStrLength - lTtlDrawn,
                           szExmpl[i] + lTtlDrawn, &rcl, 0L, 0L,
                           flWBreak | DT_TOP | DT_LEFT | DT_TEXTATTRS);
             if (lDrawn)
               lTtlDrawn += lDrawn;
             else
               break;
           }

         }

         rcl.yTop = lYcoord;
         break;  /* End of case MID_BREAKON */

       case MID_EXIT:    /* Exit */
         WinReleasePS(hps);
         WinDismissDlg(hwnd, FALSE);
         break;

     }
     break;

 case WM_INITDLG:
     /* Load Menu for WORDDLG */
     hwndMenu = WinLoadMenu(hwnd, NULLHANDLE, DID_WORD);
     WinSendMsg(hwnd, WM_UPDATEFRAME, MPFROMSHORT(FCF_MENU), 0);

     lPresParam = CLR_WHITE;
     WinSetPresParam(hwnd, PP_BACKGROUNDCOLOR, (ULONG) sizeof(LONG),
                     (PVOID) &lPresParam);

     hwndSTX = WinWindowFromID(hwnd, STX_SAMPLE);
     hps = WinGetPS(hwndSTX);

     GetFontSize(hps, &lCharWidth, &lCharHeight);

     /* Re-positioning the Dialog Box */
     WinSetWindowPos(hwnd, HWND_BOTTOM, DLGXPOS, DLGYPOS,
                 lCharWidth * NUM_CHAR + BORD_WIDTH * 2 + 1,
                 lCharHeight * (NUM_LINE + TITL_MENU),
                 SWP_MOVE | SWP_SIZE | SWP_ACTIVATE | SWP_SHOW);

     /* Re-positioning the Static Text Field */
     WinSetWindowPos(hwndSTX, HWND_BOTTOM, STXXPOS, STXYPOS,
                 lCharWidth * NUM_CHAR + 1, lCharHeight * NUM_LINE,
                 SWP_MOVE | SWP_SIZE | SWP_ACTIVATE | SWP_SHOW);

     WinQueryWindowRect(hwndSTX, &rcl);

     for (i = 0; i < NUM_EXMPL; i++)
       WinLoadString(hab, NULLHANDLE, stringID[i],
                     sizeof(szExmpl[i]), szExmpl[i]);

     WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(MID_BREAKON), 0L);

     break;

 case WM_PAINT:
     return(WinDefDlgProc(hwnd, msg, mp1, mp2));
     break;

 default:
     return(WinDefDlgProc(hwnd,msg,mp1,mp2));
 }
 return(FALSE);
}


/************************************************/
/* GetFontSize:                                 */
/* Get the following system value from PM APIs. */
/*   Average character width                    */
/*   Character height                           */
/************************************************/

void GetFontSize(HPS hps, LONG *lWidth, LONG *lHeight)
{
  FONTMETRICS fm;

  GpiQueryFontMetrics(hps, (LONG)sizeof(FONTMETRICS), &fm);
  *lWidth = fm.lAveCharWidth;
  /*--- Getting enough space to move to next line. ---*/
  *lHeight = fm.lMaxBaselineExt + fm.lExternalLeading;
}
