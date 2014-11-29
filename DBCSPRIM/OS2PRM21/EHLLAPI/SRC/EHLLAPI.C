/**** OS/2 Sample Program - Using EHLLAPI interface *******************/
/*                                                                    */
/* Program Name: EHLLAPI.C                                            */
/* Version : 1.0                                                      */
/*                                                                    */
/*   This sample program shows an example of EHLLAPI interface at     */
/*   either SBCS or DBCS OS/2. It reads the 3270 emulator session 'A' */
/*   screen via EHLLAPI interface and displays the 3270 screen at a   */
/*   PM window. By changing the window size, the PM client window is  */
/*   re-drawn and the fittest size of a font is selected from avalable*/
/*   fonts at OS/2.                                                   */
/*                                                                    */
/*   This sample program requires, at least, a 3270 emulator          */
/*   session, which should be text mode of 3279 model 2(80x25).       */
/*                                                                    */
/**********************************************************************/

#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WINFRAMEMGR
#define INCL_WINHEAP
#define INCL_WINPOINTERS
#define INCL_WINERRORS
#define INCL_WINTRACKRECT
#define INCL_DOSMEMMGR
#define INCL_DOSNLS
#define INCL_GPILCIDS
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES
#define INCL_NLS

#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dbcs.h"

/*--- Structure to store information on available fonts ------*/
typedef struct _CONDENSED_FM {
    CHAR    szFacename[FACESIZE];
    LONG    lAveCharWidth;
    LONG    lMaxBaselineExt;
    LONG    lMatch;
} CONDENSED_FM;

typedef CONDENSED_FM far *PCONDENSED_FM;

#define LCID_CRFONT 1L               // Local ID for the selected font
#define WIDTH 80                     // 3270 screen width
#define HEIGHT 24                    // 3270 screen height
#define SCREENSIZE (WIDTH * HEIGHT)  // Total characters on the 3270 screen

/*--- Service macros -----------------------------------------*/
#define METACW(cs) (pFM[pLID[c] - 1].lAveCharWidth)
#define METMBE(c) (pFM[pLID[c] - 1].lMaxBaselineExt)
#define SW(c) (pptlScrs[c].x)
#define SH(c) (pptlScrs[c].y)

/*--- Service macros for 3270 screen attributes --------------*/
#define PS_CHR(x, y) buffer[WIDTH * (y) + (x)]                 

/*--- Function prototypes ------------------------------------*/
USHORT ListFonts(HPS);
void   CreateFont(HPS, PCONDENSED_FM);
void   GetPS(CHAR *);
MRESULT APIENTRY ClientWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT APIENTRY SizeWndProc(HWND, USHORT, MPARAM, MPARAM);

/*--- Function prototype for EHLLAPI -------------------------*/
extern void APIENTRY hllapi(INT *, CHAR *, INT *, INT *);

HAB           hab;                    // Anchor block handle
SHORT         cxFrame, cyFrame;       // Size of the child frame window
HHEAP         hHeap;                  // Handle of 64K bytes heap space
SEL           sel;                    // Selector of PM heap space 
PUSHORT       pLID;                   // Pointer to list of LIDs sorted in 
                                      // the order of lMazBaselineExt
PCONDENSED_FM pFM;                    // Pointer to structures that store
                                      // information on available fixed pitch
                                      // raster fonts in the specified
                                      // code page
USHORT        cpProc;                 // The current process code page
USHORT        cMaxAvailFonts;         // The number of available fonts
SHORT         idCFont;                // The ID of current selected font
PFNWP         oldwndproc;             // The old win proc of the subcalssed 
                                      // frame
PPOINTL       pptlScrs;               // The pointer to POINTL structures for
                                      // screens for various sizes of fonts
UCHAR         buffer[SCREENSIZE];     // Buffer of 3270 presentation space
USHORT        _dbcs_cp;               // A combined code page indicator

/**********************************************************************/
/*    EHLLAPI sample - Main program                                   */
/**********************************************************************/
INT main(void) {

   HWND         hwndFrame, hwndClient;
   static CHAR  *szClientClass = "EHLLAPI";
   static ULONG flFrameFlags = FCF_TITLEBAR   | FCF_SYSMENU  |
                               FCF_SIZEBORDER | FCF_MINMAX   |
                               FCF_TASKLIST;
   HMQ          hmq;
   QMSG         qmsg;
   USHORT       rc;
   RECTL        rcl;
   USHORT       sizereq;
   NPBYTE       offset;
   INT          i;

   /*--- Initialize DBCS 1 byte table ------------------------*/
   _dbcs_cp = InitDBCSTable();

   /*--- Create 64K bytes heap -------------------------------*/
   rc = DosAllocSeg(0, &sel, 0x0000);
   hHeap = WinCreateHeap(sel, 0, 0, 0, 0, 0);

   /*--- Get the current process code page -------------------*/
   DosGetCp(2, &cpProc, &rc);

   hab = WinInitialize(0);
   hmq = WinCreateMsgQueue(hab, 0);
   WinRegisterClass(hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0);
   hwndFrame = WinCreateStdWindow(HWND_DESKTOP, WS_VISIBLE,
                                  &flFrameFlags, szClientClass,
                                  NULL, 0L, NULL, 0,
                                  &hwndClient);
   /*--- Sub Classing for Re-sizing window -------------------*/
   oldwndproc = WinSubclassWindow(hwndFrame, (PFNWP)SizeWndProc);

   sizereq = (USHORT)sizeof(POINTL) * (USHORT)cMaxAvailFonts;
   offset = WinAllocMem(hHeap, sizereq);
   pptlScrs = MAKEP(sel, offset);

   for (i = 0; i < cMaxAvailFonts; ++i) {
      rcl.xLeft = 0;
      rcl.xRight = WIDTH * pFM[pLID[i] - 1].lAveCharWidth;
      rcl.yBottom = 0;
      rcl.yTop = HEIGHT * pFM[pLID[i] - 1].lMaxBaselineExt;
      WinCalcFrameRect(hwndFrame, &rcl, FALSE);
      pptlScrs[i].x = rcl.xRight - rcl.xLeft;
      pptlScrs[i].y = rcl.yTop - rcl.yBottom;
   }

   idCFont = 0;
   cxFrame = (SHORT)pptlScrs[idCFont].x;
   cyFrame = (SHORT)pptlScrs[idCFont].y;

   WinSetWindowPos(hwndFrame, NULL, 50, 50,
                   (SHORT)pptlScrs[idCFont].x,
                   (SHORT)pptlScrs[idCFont].y,
                   SWP_SIZE | SWP_MOVE | SWP_ACTIVATE);
   WinSendMsg(hwndFrame, WM_SETICON,
              WinQuerySysPointer(HWND_DESKTOP, SPTR_APPICON, FALSE),
              NULL);

   while (WinGetMsg(hab, &qmsg, NULL, 0, 0))
      WinDispatchMsg(hab, &qmsg);
   WinDestroyWindow(hwndFrame);
   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);
   return 0;
}

/**********************************************************************/
/*    Client Window Procedure                                         */
/**********************************************************************/
MRESULT APIENTRY ClientWndProc(HWND hwnd, USHORT msg,
                               MPARAM mp1, MPARAM mp2) {

   static SHORT  cxClient, cyClient;
   HPS           hps;
   POINTL        ptlCharPos;
   INT           i;
   RECTL         rcl;

   switch(msg) {

      case WM_CREATE:
         hps = WinGetPS(hwnd);
         cMaxAvailFonts = ListFonts(hps);
         WinReleasePS(hwnd);
         return 0;

      case WM_SIZE:
         cxClient = SHORT1FROMMP(mp2);
         cyClient = SHORT2FROMMP(mp2);
         return 0;

      case WM_PAINT:
         GetPS("A");                     // Connect to 3270 session 'A'.
         hps = WinBeginPaint(hwnd, NULL, NULL);
         GpiSetColor(hps, CLR_DARKGREEN);
         WinQueryWindowRect(hwnd, &rcl);
         WinFillRect(hps, &rcl, CLR_BLACK);
         CreateFont(hps, &pFM[pLID[idCFont] - 1]);
         GpiSetCharSet(hps, LCID_CRFONT);
         ptlCharPos.x = 0;
         ptlCharPos.y = cyClient - METMBE(idCFont);
         for (i = 0; i < HEIGHT; ++i) {
            GpiCharStringAt(hps, &ptlCharPos, (LONG)80, &PS_CHR(0, i));
            ptlCharPos.y -= METMBE(idCFont);
         }
         WinEndPaint(hps);
         return 0;
   }
   return (WinDefWindowProc(hwnd, msg, mp1, mp2));
}

/**********************************************************************/
/*    Sub-classing Window Procedure for re-sizing a client window     */
/**********************************************************************/
MRESULT APIENTRY SizeWndProc(HWND hwnd, USHORT msg,
                             MPARAM mp1, MPARAM mp2) {

   PTRACKINFO pTrackInfo;
   LONG  x, y;
   PSWP  pswp;
   INT i;

   switch (msg) {

      case WM_QUERYTRACKINFO:
         (*oldwndproc)(hwnd, msg, mp1, mp2);
         pTrackInfo = mp2;
         pTrackInfo->ptlMaxTrackSize = pptlScrs[cMaxAvailFonts - 1];
         pTrackInfo->ptlMinTrackSize = pptlScrs[0];
         return (MRESULT)TRUE;

      case WM_ADJUSTWINDOWPOS:
         pswp = mp1;
         if (pswp->fs & SWP_SIZE) {

            x = pswp->cx - cxFrame;
            y = pswp->cy - cyFrame;

            if (x * y < 0) {
               pswp->cx = cxFrame;
               pswp->cy = cyFrame;
            } else {
               if ((x < 0) || (y < 0)) {
                  i = idCFont - 1;
                  while(SW(i) > pswp->cx) --i;
                  while(SH(i) > pswp->cy) --i;
                  pswp->cx = cxFrame = (SHORT)SW(i);
                  pswp->cy = cyFrame = (SHORT)SH(i);
                  idCFont = i;
               } else {
                  if ((x > 0) || (y > 0)) {
                     i = idCFont + 1;
                     while(SW(i) < pswp->cx) ++i;
                     while(SH(i) < pswp->cy) ++i;
                     pswp->cx = cxFrame = (SHORT)SW(i);
                     pswp->cy = cyFrame = (SHORT)SH(i);
                     idCFont = i;
                  }
               }
            }
         }
         return((*oldwndproc)(hwnd, msg, mp1, mp2));
   }
   return((*oldwndproc)(hwnd, msg, mp1, mp2));
}

/**********************************************************************/
/*    A subroutine for Getting Presentation Space                     */
/**********************************************************************/
void GetPS(CHAR *ssprm) {

   INT  rc, fcode, dlen, i;
   CHAR *pcSprm;
   CHAR tempbuf[SCREENSIZE];

   /*--- Reset System ----------------------------------------*/
   fcode = 21;
   hllapi(&fcode, (char far *)0L, (int far *)0L, &rc);

   /*--- Connect Presentation Space --------------------------*/
   fcode = 1;
   hllapi(&fcode, ssprm, (int far *)0L, &rc);

   /*--- Set Session Parameters ------------------------------*/
   fcode = 9;
   pcSprm = "SO";
   dlen = strlen(pcSprm);
   hllapi(&fcode, pcSprm, &dlen, &rc);

   /*--- Copy Presentation Space -----------------------------*/
   fcode = 5;
   dlen = 0;
   hllapi(&fcode, tempbuf, &dlen, &rc);
   for (i = 0; i < SCREENSIZE; ++i) {
      PS_CHR(i, 0) = tempbuf[i];
   }
   /*--- Change SO/SI to pseudo SO/SI ------------------------*/
   for (i = 0; i < SCREENSIZE; ++i) {
      switch (PS_CHR(i, 0)) {
         case 0x0e:
            /*--- Translate SO(0x0E) into 0x1E(Right Arrow) --*/
            PS_CHR(i, 0) = 0x1e;
            break;
         case 0x0f:
            /*--- Translate SI(0x0F) into 0x1F(Left Arrow) ---*/
            PS_CHR(i, 0) = 0x1f;
            break;
      }
   }
   /*--- Disconnect Presentation Space -----------------------*/
   fcode = 2;
   hllapi(&fcode, (char far *)0L, (int far *)0L, &rc);
}

/**********************************************************************/
/*   Listing available fonts in the order of the font size            */
/**********************************************************************/
USHORT ListFonts(HPS hps) {

   SHORT         i, j;
   USHORT        temp, cFonts;
   LONG          NoOfFontsReq = 0;
   LONG          ActualNoOfFonts;
   USHORT        sizereq;
   NPBYTE        offset;
   PFONTMETRICS  pMetrics;

   ActualNoOfFonts = GpiQueryFonts(hps,
                                   QF_PUBLIC | QF_PRIVATE,
                                   (PSZ)NULL,
                                   &NoOfFontsReq,
                                   0L,
                                   (PFONTMETRICS)NULL);

   sizereq = (USHORT)sizeof(FONTMETRICS) * (USHORT)ActualNoOfFonts;
   offset = WinAllocMem(hHeap, sizereq);
   pMetrics = MAKEP(sel, offset);

   GpiQueryFonts(hps,
                 QF_PUBLIC | QF_PRIVATE,
                 (PSZ)NULL,
                 &ActualNoOfFonts,
                 (LONG)sizeof(FONTMETRICS),
                 pMetrics);
   cFonts = 0;

   if (_dbcs_cp == NON_DBCS_CP) {
      for (i = 0; i < (USHORT)ActualNoOfFonts; ++i) {
         if (!(pMetrics[i].fsDefn & FM_DEFN_OUTLINE) &&
              (pMetrics[i].fsType & FM_TYPE_FIXED)) {
            ++cFonts;
         }
      }
      sizereq = (USHORT)sizeof(CONDENSED_FM) * (USHORT)cFonts;
      offset = WinAllocMem(hHeap, sizereq);
      pFM = MAKEP(sel, offset);
      cFonts = 0;
      for (i = 0; i < (USHORT)ActualNoOfFonts; ++i) {
         if (!(pMetrics[i].fsDefn & FM_DEFN_OUTLINE) &&
              (pMetrics[i].fsType & FM_TYPE_FIXED)) {
            strcpy(pFM[cFonts].szFacename, pMetrics[i].szFacename);
            pFM[cFonts].lMaxBaselineExt = pMetrics[i].lMaxBaselineExt;
            pFM[cFonts].lAveCharWidth   = pMetrics[i].lAveCharWidth;
            pFM[cFonts].lMatch          = pMetrics[i].lMatch;
            ++cFonts;
         }
      }
   } else {
      for (i = 0; i < (USHORT)ActualNoOfFonts; ++i) {
         if (!(pMetrics[i].fsDefn & FM_DEFN_OUTLINE) &&
              (pMetrics[i].fsType & FM_TYPE_FIXED) &&
              (pMetrics[i].fsType & FM_TYPE_MBCS)) {
            ++cFonts;
         }
      }
      sizereq = (USHORT)sizeof(CONDENSED_FM) * (USHORT)cFonts;
      offset = WinAllocMem(hHeap, sizereq);
      pFM = MAKEP(sel, offset);
      cFonts = 0;
      for (i = 0; i < (USHORT)ActualNoOfFonts; ++i) {
         if (!(pMetrics[i].fsDefn & FM_DEFN_OUTLINE) &&
              (pMetrics[i].fsType & FM_TYPE_FIXED) &&
              (pMetrics[i].fsType & FM_TYPE_MBCS)) {
            strcpy(pFM[cFonts].szFacename, pMetrics[i].szFacename);
            pFM[cFonts].lMaxBaselineExt = pMetrics[i].lMaxBaselineExt;
            pFM[cFonts].lAveCharWidth   = pMetrics[i].lAveCharWidth;
            pFM[cFonts].lMatch          = pMetrics[i].lMatch;
            ++cFonts;
         }
      }
   }

   offset = WinFreeMem(hHeap, (NPBYTE)OFFSETOF(pMetrics), sizereq);

   sizereq = (USHORT)sizeof(USHORT) * (USHORT)cFonts;
   offset = WinAllocMem(hHeap, sizereq);
   pLID = MAKEP(sel, offset);

   for (i = 0; i < cFonts; ++i) {
      pLID[i] = (USHORT)(i + 1);
   }

   /*--- Sorting LIDs in the order of lMaxBaselineExt --------*/
   for (i = 0; i < cFonts - 1; ++i) {
      for (j = cFonts - 2; j >= i; --j) {
         if (pFM[pLID[j + 1] - 1].lMaxBaselineExt <
             pFM[pLID[j] - 1].lMaxBaselineExt) {
            temp = pLID[j];
            pLID[j] = pLID[j + 1];
            pLID[j + 1] = temp;
         }
      }
   }
   return (USHORT)cFonts;
}

/**********************************************************************/
/*    A subroutine for creating the selected font                     */
/**********************************************************************/
void CreateFont(HPS hps, PCONDENSED_FM pFM) {

   FATTRS  SelectedFont;

   SelectedFont.usRecordLength  = sizeof(FATTRS);
   SelectedFont.fsSelection     = 0;
   SelectedFont.lMatch          = pFM->lMatch;
   strcpy(SelectedFont.szFacename, pFM->szFacename);
   SelectedFont.idRegistry      = 0;
   SelectedFont.usCodePage      = cpProc;
   SelectedFont.lMaxBaselineExt = pFM->lMaxBaselineExt;
   SelectedFont.lAveCharWidth   = pFM->lAveCharWidth;
   SelectedFont.fsType          = 0;
   SelectedFont.fsFontUse       = FATTR_FONTUSE_NOMIX;
   GpiCreateLogFont(hps,
                    NULL,
                    LCID_CRFONT,
                    &SelectedFont );
}
