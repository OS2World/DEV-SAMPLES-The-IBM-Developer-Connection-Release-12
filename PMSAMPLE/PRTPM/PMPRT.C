/*---------------------------------------------------------------------------+
+----------------------------------------------------------------------------+
|                                                                            |
| PMPRT                                                                      |
|                                                                            |
|*  Description:  This routine is responsible for popping up a window        |
|*                when it receives notification of a print screen key by     |
|*                The input hook.                                            |
|                 This routines tries to respect the contrasts when          |
|                 Converting from colors to Black and White                  |
+-------------------------------------+--------------------------------------+
| Version: 1.0                        |   Marc Fiammante (FIAMMANT at LGEVM2)|
+-------------------------------------+--------------------------------------+
| Inspired from OS2PM PROCS FROM OS2TOOLS                                    |
+-------------------------------------+--------------------------------------+
| History:                                                                   |
| --------                                                                   |
|                                                                            |
| created: Marc Fiammante March  1992                                        |
+---------------------------------------------------------------------------*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
//
//
// Print a bitmap function
//
// 
//
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define INCL_BASE
#define INCL_DOS
#define INCL_WIN
#define INCL_GPIBITMAPS
#define INCL_GPILOGCOLORTABLE
#define INCL_GPIPRIMITIVES
#define INCL_DEV
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define SHR4(a)    ((UCHAR)(((a)>>4)&0x0F))
#define SHL4(a)    ((UCHAR)(((a)<<4)&0xF0))
#include <pmbitmap.h>
#include "pmprt.h"
typedef struct _COLORDEF {
                           LONG  SystemColor;
                           LONG  Index;
                           UCHAR Foreground;
                           } COLORDEF;
COLORDEF  PMColors[]= {
     {  SYSCLR_BUTTONLIGHT             ,  0L ,0},
     {  SYSCLR_BUTTONMIDDLE            ,  0L ,0},
     {  SYSCLR_BUTTONDARK              ,  0L ,15},
     {  SYSCLR_BUTTONDEFAULT           ,  0L ,0},
     {  SYSCLR_SHADOW                  ,  0L ,0},
     {  SYSCLR_SCROLLBAR               ,  0L ,0},
     {  SYSCLR_ACTIVETITLE             ,  0L ,0},
     {  SYSCLR_INACTIVETITLE           ,  0L ,0},
     {  SYSCLR_MENU                    ,  0L ,0},
     {  SYSCLR_WINDOW                  ,  0L ,0},
     {  SYSCLR_WINDOWFRAME             ,  0L ,15},
     {  SYSCLR_ACTIVEBORDER            ,  0L ,0},
     {  SYSCLR_INACTIVEBORDER          ,  0L ,0},
     {  SYSCLR_APPWORKSPACE            ,  0L ,0},
     {  SYSCLR_HELPBACKGROUND          ,  0L ,0},
     {  SYSCLR_HELPHILITE              ,  0L ,0},
     {  SYSCLR_DIALOGBACKGROUND        ,  0L ,0},
     {  SYSCLR_INACTIVETITLETEXTBGND   ,  0L ,0},
     {  SYSCLR_ACTIVETITLETEXTBGND     ,  0L ,0},
     {  SYSCLR_HILITEBACKGROUND        ,  0L ,0},
     {  SYSCLR_BACKGROUND              ,  0L ,0},
     {  SYSCLR_TITLEBOTTOM             ,  0L ,0},
     {  SYSCLR_HELPTEXT                ,  0L ,15},
     {  SYSCLR_ICONTEXT                ,  0L ,15},
     {  SYSCLR_HILITEFOREGROUND        ,  0L ,15},
     {  SYSCLR_INACTIVETITLETEXT       ,  0L ,15},
     {  SYSCLR_ACTIVETITLETEXT         ,  0L ,15},
     {  SYSCLR_OUTPUTTEXT              ,  0L ,15},
     {  SYSCLR_MENUTEXT                ,  0L ,15},
     {  SYSCLR_WINDOWSTATICTEXT        ,  0L ,15},
     {  SYSCLR_WINDOWTEXT              ,  0L ,15},
     {  SYSCLR_TITLETEXT               ,  0L ,15}
     };
#define NUM_PMCOLORS (sizeof(PMColors)/sizeof(COLORDEF))

USHORT           PrintBitmap (HAB hab, PRECTL prcl,  BOOL fSpooled);
USHORT           ConvertBitmap (HAB hab, HPS hpsSource,HBITMAP Old);
USHORT           FileImageBmp(HAB hab,PCHAR pFileName,PRECTL pRectl);
USHORT           SaveImageBmp(HAB hab);
MRESULT EXPENTRY PrtDlgProc( HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2 );
BOOL    EXPENTRY InputHookProc(HAB habSpy, PQMSG pQmsg, BOOL bRemove);
MRESULT EXPENTRY PrintProc(HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2 );
VOID             WindowDestroy(void);
VOID             WindowInitialization(void);

HAB     hab;                             /* PM anchor block handle       */
HMODULE hDll;                            /* Spy module handle            */
extern  HWND pascal   PrtWindow;         /*                              */
static  HWND ActiveWindow;               /*                              */
HWND    hwndModeless;                    /* Modeless dialog window  */

HDC     hdcMem;                          /* Memory device context handle  */
HPS     hpsMem;                          /* Presentation space handle     */
HBITMAP hbmSimpic=NULL;                  /* Handle of screen bitmap       */
UCHAR   bmBits[8196];
UCHAR   bmHdrBuf[sizeof(BITMAPINFOHEADER)+sizeof(RGB)*256];
PBITMAPINFOHEADER pBmih=(PBITMAPINFOHEADER)bmHdrBuf;
PBITMAPINFO pBmi=(PBITMAPINFO)bmHdrBuf;
BITMAPINFO        BmiTemp;
BITMAPARRAYFILEHEADER bmafh;
LONG    numRealClrs;
RGB     rgbValues[256];
LONG    realClrs[256];
UCHAR   ForBack[256];
UCHAR   LookTbl[256];
BOOL    Contrast[256];
UCHAR   LookTbl2[256];
SIZEL   size;
POINTL  pointl[4];
ULONG   Index;
ULONG   Color;
ULONG   hResIn, vResIn;
ULONG   hResOut,vResOut;
void    (*CvtFunct)(void);
void    Cvt16(void);       /* Convert  for  16 colors */
void    Cvt256(void);      /* Conevert for 256 colors */
BOOL    DialogShowing=FALSE;
/*---------------------------------------------------------------------------+
| Main                                                                       |
+---------------------------------------------------------------------------*/

VOID cdecl main()
{
  HMQ  hmq;                             /* Message queue handle         */
  QMSG qmsg;                            /* Message from message queue   */

  hab = WinInitialize( NULL );          /* Initialize PM                */
  hmq = WinCreateMsgQueue( hab, 100 );    /* Create a message queue       */



  DosGetModHandle( PRTHOOK , (PHMODULE)&hDll);
  WinSetHook(hab, NULL, HK_INPUT  , (PFN)InputHookProc, hDll);
  /* create the modeless dialog window                                */
  WinRegisterClass(                           /* Register window class       */
     hab,                                     /* Anchor block handle         */
     "Print",                                 /* Window class name           */
     PrintProc,                               /* Address of window procedure */
     0L,                                      /* Class Style                 */
     0                                        /* No extra window words       */
     );
  PrtWindow          =WinCreateWindow(        /* Create an object window     */
                        HWND_OBJECT,          /* Parent window               */
                        "Print",              /* Class of window             */
                        "Print",              /* Name of window              */
                        0L ,                  /* No particular style         */
                        0, 0, 0, 0,           /* Size and position           */
                        HWND_DESKTOP,         /* Window owner                */
                        HWND_BOTTOM,          /* Where she is in the screen  */
                        ID_PMPRT,             /* Identifier                  */
                        NULL,                 /* No particular datas and no  */
                        NULL);                /* parameters                  */
  WindowInitialization();
/************************************************************************/
/* Get and dispatch messages from the application message queue         */
/* until WinGetMsg returns FALSE, indicating a WM_QUIT message.         */
/************************************************************************/
  while( WinGetMsg( hab, &qmsg, NULL, 0, 0 ) ) {
      WinDispatchMsg( hab, &qmsg );
  }
  WinReleaseHook(hab , NULL, HK_INPUT  , (PFN)InputHookProc, hDll);

  WindowDestroy();
  WinDestroyWindow( PrtWindow );        /* Tidy up...                  */
  WinDestroyMsgQueue( hmq );            /* and                          */
  WinTerminate( hab );                  /* terminate the application    */
}
USHORT Input =ID_ACTIVE;
USHORT Output=ID_PRINT;
RECTL  Rectl;
CHAR   Filename[NAMELENGTH+4+1];
/*********************  Start of window procedure  **********************/
MRESULT EXPENTRY PrintProc(HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2 ) {
     BOOL Process;
  switch (msg)
  {
     case WM_USER+WM_CHAR:
         if ((SHORT2FROMMP(mp2)==VK_PRINTSCRN)        &&
             ((SHORT1FROMMP(mp1)&KC_PREVDOWN)==FALSE) &&
             ((SHORT1FROMMP(mp1)&KC_KEYUP)   ==FALSE)    )
         {
           if (DialogShowing==FALSE) {
              DialogShowing=TRUE;
              DosBeep(200,50);
              SaveImageBmp(hab);
              DosBeep(1200,50);
              DosBeep(1000,50);
              DosBeep(800,50);
              ActiveWindow=WinQueryActiveWindow(HWND_DESKTOP,FALSE);
              Process= WinDlgBox( HWND_DESKTOP,
                                  HWND_DESKTOP,
                                  PrtDlgProc,
                                  NULL,
                                  ID_PMPRT,
                                  NULL );

              WinSetActiveWindow(HWND_DESKTOP,ActiveWindow);
              WinInvalidateRegion(ActiveWindow, NULL, TRUE );
              if (Process) {
                 if (Output==ID_PRINT) {
                     PrintBitmap (hab,&Rectl,TRUE);
                 } else {
                    FileImageBmp(hab,Filename,&Rectl);
                 }
              } /* endif */
              DialogShowing=FALSE;
              DosBeep(500,50);
           }
         } /* endif */
       break;
    default:  /* Pass all other messages to the default dialog proc  */
      return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  return FALSE;
}
/*********************  Start of dialog procedure  **********************/
MRESULT EXPENTRY PrtDlgProc( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
  switch (msg)
  {
     case WM_CLOSE:
       WinDismissDlg( hwndDlg, FALSE); /* Finished with dialog box    */
       break;
     case WM_INITDLG:
       {
          WinPostMsg( WinWindowFromID( hwndDlg, ID_ACTIVE ),
                    BM_SETCHECK,
                    MPFROM2SHORT( TRUE,0 ),
                    0L );
          WinPostMsg( WinWindowFromID( hwndDlg, ID_PRINT ),
                    BM_SETCHECK,
                    MPFROM2SHORT( TRUE,0 ),
                    0L );
          WinPostMsg( WinWindowFromID( hwndDlg, ID_FILENAME ),
                    EM_SETTEXTLIMIT,
                    MPFROMSHORT(NAMELENGTH),
                    0L );
       }
      break;
    case WM_CONTROL:
      if( SHORT2FROMMP( mp1 ) == BN_CLICKED )
        /* Determine Preview window new color from the selected      */
        /* auto radio button                                         */
        switch( SHORT1FROMMP( mp1 ) )
        {
          case ID_ACTIVE:
            Input =ID_ACTIVE;
            break;
          case ID_DESKTOP:
            Input =ID_DESKTOP;
            break;
          case ID_PRINT:
            Output=ID_PRINT;
            break;
          case ID_FILE:
            Output=ID_FILE;
            break;
          default:
           return FALSE;
        }
        break;
    case WM_COMMAND:
      switch( SHORT1FROMMP( mp1 ) )
      {
    case DID_OK:    /* Enter key pressed or pushbutton selected  */
          {
             POINTL WinPt[2];         /* Coordinates of window          */
             BOOL   Success;          /* Boolean return value           */
             HWND   HwndCatch;
             if (Input==ID_ACTIVE) {
                HwndCatch=ActiveWindow;
             } else {
                HwndCatch=HWND_DESKTOP;
             }
             WinPt[0].x=0;
             WinPt[0].y=0;
             WinQueryWindowRect(HwndCatch,&Rectl);
             Success=WinMapWindowPoints(HwndCatch,HWND_DESKTOP,&WinPt[0],1);
             Rectl.xLeft  +=WinPt[0].x;
             Rectl.yBottom+=WinPt[0].y;
             Rectl.xRight +=WinPt[0].x;
             Rectl.yTop   +=WinPt[0].y;
              if (Output==ID_FILE) {
                 WinQueryWindowText( WinWindowFromID( hwndDlg, ID_FILENAME ),
                                 NAMELENGTH+1,
                                 Filename );
                 strcat(Filename,".BMP");
             }
          }
          WinDismissDlg( hwndDlg, TRUE); /* Finished with dialog box    */
          break;
        case DID_CANCEL:    /* Enter key pressed or pushbutton selected  */
          WinDismissDlg( hwndDlg, FALSE); /* Finished with dialog box    */
          break;
        default:
          return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
      }
      break;

    default:  /* Pass all other messages to the default dialog proc  */
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
  }
  return FALSE;
}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
USHORT PrintBitmap (HAB hab, PRECTL pRectl,  BOOL fSpooled)
{
    CHAR szPrinter [33];
    CHAR szDetails [256];
    CHAR *szPort;
    CHAR *szPrintDriver;
    CHAR *szLogicalPort;

    HPS  hpsMemPrinter;
    HPS  hpsPrinter;
    HDC  hdcPrinter;
    HDC  hdcMemPrinter;
    USHORT cb;
    DEVOPENSTRUC dopPrinter;
    POINTL aptl[4];
    USHORT usRc;
    ULONG  ulRc;
    SIZEL  sizl;
    ULONG  lHeight;
    ULONG  lWidth;
    ULONG  lMin;
    ULONG  Margin;
    ULONG  lWidthIn;
    ULONG  lHeightIn;
    HBITMAP hbmPrinter,Old;                  /* Handle of screen bitmap       */
    ULONG pdata;
    PDRIVDATA pdrivedata;
//    static HCINFO hcInfo;
//    LONG NumForms,Form;
    cb = WinQueryProfileString (hab, "PM_SPOOLER", "PRINTER", "", szPrinter,32);
    szPrinter[cb-2]=0;   // remove extra ;

    cb = WinQueryProfileString (hab, "PM_SPOOLER_PRINTER", szPrinter, "", szDetails, 256);
    szPort = strtok (szDetails, ";.,");
    szPrintDriver = strtok (NULL,  ";.,");
    szLogicalPort = strtok (NULL,  ";.,");
    pdata=DevPostDeviceModes(hab, NULL, szPrintDriver, szPrinter, szLogicalPort,
                        DPDM_POSTJOBPROP);
    pdrivedata=malloc((USHORT)pdata);

    DevPostDeviceModes(hab, pdrivedata, szPrintDriver, szPrinter,
                        szLogicalPort, DPDM_CHANGEPROP);
    dopPrinter.pszDriverName = szPrintDriver;
    dopPrinter.pdriv = pdrivedata;
    dopPrinter.pszDataType = "PM_Q_STD";

    if (fSpooled) {
         dopPrinter.pszLogAddress = szLogicalPort;
         hdcPrinter = DevOpenDC (hab, OD_QUEUED, "*", 4L,
                             (PDEVOPENDATA)&dopPrinter,  (HDC)NULL);
    } else {
         dopPrinter.pszLogAddress = szPort;
         hdcPrinter = DevOpenDC (hab, OD_DIRECT, "*", 4L,
                             (PDEVOPENDATA)&dopPrinter,  (HDC)NULL);
    } /* endif */
    free(pdrivedata);

    if (DEV_ERROR == hdcPrinter) {
         usRc = ERRORIDERROR(WinGetLastError (hab));
         WinMessageBox (HWND_DESKTOP,    // parent
                        HWND_DESKTOP,       // owner
                       "hdcPrinter = DEV_ERROR",
                       "MOSS/E Print: Error", 0, MB_OK);
         return 1;
    }
//  NumForms=DevQueryHardcopyCaps(hdcPrinter,0L,0L,&hcInfo);
//  for (Form=0L;Form<NumForms;Form++) {
//     if (DevQueryHardcopyCaps(hdcPrinter,Form,1L,&hcInfo)==DQHC_ERROR) break;
//  } /* endfor */
    ConvertBitmap (hab, hpsMem,hbmSimpic);

    sizl.cx = 0L;
    sizl.cy = 0L;

    hpsPrinter = GpiCreatePS (hab, hdcPrinter, &sizl,
                    PU_PELS | GPIF_DEFAULT | GPIT_NORMAL | GPIA_ASSOC);



    hdcMemPrinter = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, hdcPrinter);
    if (DEV_ERROR == hdcMemPrinter){
         usRc = ERRORIDERROR(WinGetLastError (hab));
         WinMessageBox (HWND_DESKTOP,    // parent
                        HWND_DESKTOP,       // owner
                       "hdcMemPrinter = DEV_ERROR",
                       "MOSS/E Print: Error", 0, MB_OK);
    }

    hpsMemPrinter = GpiCreatePS (hab, hdcMemPrinter, &sizl,
                    PU_PELS | GPIF_DEFAULT | GPIT_NORMAL | GPIA_ASSOC);


    DevEscape(hdcPrinter, DEVESC_STARTDOC, 15L,
                 (PSZ)"MOSS/E Spooled Picture",
                 NULL, NULL);

    DevQueryCaps(hdcPrinter,CAPS_HORIZONTAL_RESOLUTION,1L,(PLONG)&hResOut);
    DevQueryCaps(hdcPrinter,CAPS_VERTICAL_RESOLUTION  ,1L,(PLONG)&vResOut);
    DevQueryCaps (hdcPrinter, CAPS_HEIGHT, 1L, (PLONG) &lHeight);
    DevQueryCaps (hdcPrinter, CAPS_WIDTH , 1L, (PLONG) &lWidth );
    if (lWidth<lHeight) {
       lMin=lWidth;
    } else {
       lMin=lHeight;
    } /* endif */


    Margin=(lMin*5L)/100L;

    lWidthIn =(hResOut*((LONG)(pRectl->xRight-pRectl->xLeft)))/hResIn;
    lHeightIn=(vResOut*((LONG)(pRectl->yTop-pRectl->yBottom)))/vResIn;
    Old=GpiSetBitmap (hpsMemPrinter, hbmSimpic);
    if (Old==HBM_ERROR) {
        char Buffer[80];
        usRc = ERRORIDERROR(WinGetLastError (hab));
        sprintf(Buffer,"MOSS/E Print 1 SetBitmap: Err l=%p s=%4.4X",ulRc,usRc);
        WinAlarm(HWND_DESKTOP,WA_ERROR);
        WinMessageBox (HWND_DESKTOP,    // parent
                       HWND_DESKTOP,       // owner
                       Buffer,
                       "MOSS/E Print: Info", 0, MB_OK);
    } /* endif */

    aptl[0].x=Margin;
    aptl[0].y=(lHeight-Margin)-lHeightIn;
    aptl[1].x=lWidthIn+Margin;
    aptl[1].y=lHeight-Margin;
    aptl[2].x=pRectl->xLeft;
    aptl[2].y=pRectl->yBottom;
    aptl[3].x=pRectl->xRight;
    aptl[3].y=pRectl->yTop;

    if ((lHeight-2*Margin)<lHeightIn) {
       aptl[0].y=Margin;
    } /* endif */
    if ((lWidth-2*Margin)<lWidthIn) {
       aptl[1].x=lWidth-Margin;
    } /* endif */

    ulRc = GpiBitBlt (hpsPrinter, hpsMemPrinter, 4L, aptl,ROP_SRCCOPY, BBO_OR);

    if (GPI_OK!=ulRc) {
         char Buffer[80];
         usRc = ERRORIDERROR(WinGetLastError (hab));
         sprintf(Buffer,"MOSS/E Print: Err l=%p s=%4.4X",ulRc,usRc);
         WinMessageBox (HWND_DESKTOP,    // parent
                        HWND_DESKTOP,       // owner
                       "GpiBitBlt hpsMem to hpsPrinter failed.",
                       Buffer, 0, MB_OK);
         return 1;
    }

    GpiSetBitmap (hpsMemPrinter, NULL );

    // end of doc
    GpiAssociate (hpsPrinter, NULL);

    ulRc = DevEscape(hdcPrinter, DEVESC_ENDDOC, 0L, NULL, 0L, NULL);
    DevCloseDC (hdcPrinter);

    DevCloseDC (hdcMemPrinter);
    GpiDestroyPS (hpsPrinter);
    GpiDeleteBitmap( hbmPrinter);
    GpiDestroyPS (hpsMemPrinter);
}
/***********************************************************************/
/* This function creates a screen-compatible device context and        */
/* defines a normal presentation space. The presentation page is the   */
/* same size as the client area of the window, and is defined in pels. */
/* The presentation space and the device context are associated.       */
/* The value of the global variable defining initial window size is    */
/* set.                                                                */
/***********************************************************************/
PSZ         dcdatablk[9] = {(PSZ)0
                           ,(PSZ)"DISPLAY"
                           ,(PSZ)0
                           ,(PSZ)0
                           ,(PSZ)0
                           ,(PSZ)0
                           ,(PSZ)0
                           ,(PSZ)0
                           ,(PSZ)0
                           };

VOID WindowInitialization()
{




//    size.cx = WinQuerySysValue(HWND_DESKTOP, SV_CXFULLSCREEN);
//    size.cy = WinQuerySysValue(HWND_DESKTOP, SV_CYFULLSCREEN);


    hdcMem = DevOpenDC( hab
                      , OD_MEMORY
                      , (PSZ)"*"
                      , 8L
                      , (PDEVOPENDATA)dcdatablk
                      , (HDC)NULL
                      );
    DevQueryCaps (hdcMem, CAPS_HEIGHT, 1L, (PLONG) &size.cy);
    DevQueryCaps (hdcMem, CAPS_WIDTH , 1L, (PLONG) &size.cx );
    DevQueryCaps(hdcMem,CAPS_HORIZONTAL_RESOLUTION,1L,(PLONG)&hResIn);
    DevQueryCaps(hdcMem,CAPS_VERTICAL_RESOLUTION  ,1L,(PLONG)&vResIn);


    hpsMem = GpiCreatePS( hab
                        , hdcMem
                        , (PSIZEL)&size
                        , (LONG)PU_PELS | GPIT_NORMAL | GPIA_ASSOC
                        );


}
/************ End of WINDOWINITIALIZATION Private Function *************/
/*************** Start of WINDOWDESTROY Private Function ***************/
/***********************************************************************/
/* This function deletes the presentation space, and closes the memory */
/* device context.                                                     */
/***********************************************************************/

VOID WindowDestroy()
{
    if ( hbmSimpic != NULL ) { GpiDeleteBitmap( hbmSimpic ); }
    GpiAssociate( hpsMem, (HDC)NULL );
    GpiDestroyPS( hpsMem );
    DevCloseDC( hdcMem );
}
/**************** End of WINDOWDESTROY Private Function ****************/
#define BUFSIZE  0xFFFF
#define BUFSIZEL 0x0000FFFFL
CHAR Image[BUFSIZE];
USHORT SaveImageBmp(HAB hab)
{

   USHORT usRc;
   ULONG  ulRc;
   HPS     hpsSrc;
   HBITMAP Old;                  /* Handle of screen bitmap       */

   usRc = ERRORIDERROR(WinGetLastError (hab)); // Clear error buffer
   if ( hbmSimpic != NULL ) { GpiDeleteBitmap( hbmSimpic ); }
   hpsSrc=WinGetScreenPS(HWND_DESKTOP);
   if (hpsSrc==NULL) {
         char Buffer[80];
         usRc = ERRORIDERROR(WinGetLastError (hab));
         sprintf(Buffer,"MOSS/E Save: Err l=%p s=%4.4X",ulRc,usRc);
         WinMessageBox (HWND_DESKTOP,    // parent
                        HWND_DESKTOP,       // owner
                       "hpsSrc failed.",
                       Buffer, 0, MB_OK);
   }
   // Query the number of colors in the hpsSrc and create
   // a logical color table in the temp HPS
   numRealClrs=GpiQueryRealColors(hpsSrc,0L,0L,256L,realClrs);
   GpiCreateLogColorTable(hpsMem,
                          NULL,
                          LCOLF_CONSECRGB,
                          0L,
                          numRealClrs,
                          realClrs);
   // Create a bitmap, and set it in the memory HPS, so
   // that we can copy the rectangle into it
   pBmih->cbFix=sizeof(BITMAPINFOHEADER);
   pBmih->cx=(USHORT)size.cx;
   pBmih->cy=(USHORT)size.cy;
   DevQueryCaps(hdcMem,CAPS_COLOR_PLANES,1L,(PLONG)&pBmih->cPlanes);
   DevQueryCaps(hdcMem,CAPS_COLOR_BITCOUNT,1L,(PLONG)&pBmih->cBitCount);
   hbmSimpic =GpiCreateBitmap(hpsMem,pBmih,0L,NULL,NULL);

   Old=GpiSetBitmap(hpsMem,hbmSimpic);
   if (Old==HBM_ERROR)
   {
        char Buffer[80];
        usRc = ERRORIDERROR(WinGetLastError (hab));
        sprintf(Buffer,"MOSS/E Save SetBitmap: Err l=%p s=%4.4X",ulRc,usRc);
        WinAlarm(HWND_DESKTOP,WA_ERROR);
        WinMessageBox (HWND_DESKTOP,    // parent
                       HWND_DESKTOP,       // owner
                       Buffer,
                       "info ", 0, MB_OK);
   } /* endif */
   pointl[0].x=0;pointl[2].x=0;
   pointl[0].y=0;pointl[2].y=0;
   pointl[1].x=size.cx;
   pointl[1].y=size.cy;
   ulRc=GpiBitBlt(hpsMem,hpsSrc,3L,pointl,ROP_SRCCOPY,BBO_IGNORE);
   if (GPI_OK!=ulRc) {
         char Buffer[80];
         usRc = ERRORIDERROR(WinGetLastError (hab));
         sprintf(Buffer,"MOSS/E Save: Err l=%p s=%4.4X Index=%ul",ulRc,usRc,Index);
         WinMessageBox (HWND_DESKTOP,    // parent
                        HWND_DESKTOP,       // owner
                       "GpiBitBlt hpsSrc to hpsMem failed.",
                       Buffer, 0, MB_OK);
   }

   GpiSetBitmap (hpsMem, NULL );
   WinReleasePS(hpsSrc);
   return 0;
}
USHORT FileImageBmp(HAB hab,PCHAR pFileName,PRECTL pRectl)
//-------------------------------------------------------------------------
// This routine saves an rectangle in a PS as a 1.2 .BMP file.
// Unfortunately, it has only been tested on an 8514 and a VGA.  It
// *should*  8)  work on an EGA also...
//
// Input:  pFileName - points to the name to save the bitmap as
//         hpsSrc - the source hps to save from
//         pRectl - points to the rectangle within hpsSrc to save
//
// Written by:  Larry Salomon, Jr.
//
// Many thanks to Jason Crawford for his routine to save a 1.1 file.
//-------------------------------------------------------------------------
{
   USHORT ExitError;
   HDC hdcTemp;
   HPS hpsTemp;
   HBITMAP hbmTemp,Old;
   BITMAPARRAYFILEHEADER bmafh;
   USHORT index1;
   ULONG index2;
   ULONG bmBitsLineSize;
   USHORT dosRc;
   HFILE hFile=NULL;
   USHORT bytesToWrite;
   USHORT bytesWritten;
   USHORT action;
   POINTL pointl[4];
   SIZEL sizeBmp;
   ULONG  ulRc;
   USHORT usRc;

   ConvertBitmap (hab, hpsMem,hbmSimpic);
   ExitError=NO_ERROR;
   sizeBmp.cx=pRectl->xRight-pRectl->xLeft;
   sizeBmp.cy=pRectl->yTop-pRectl->yBottom;

   // Create a memory device context and hps to copy the rectangle into.
   // Note that we could have probably used the existing HDC, but that's
   // one more parameter to pass...Ugh!
   hdcTemp=DevOpenDC(hab,OD_MEMORY,"*" , 8L , (PDEVOPENDATA)dcdatablk,NULL);

   hpsTemp=GpiCreatePS(hab,
                       hdcTemp,
                       &sizeBmp,
                       PU_PELS|GPIF_DEFAULT|GPIT_MICRO|GPIA_ASSOC);

   // Query the number of colors in the hpsSrc and create
   // a logical color table in the temp HPS

   GpiCreateLogColorTable(hpsTemp,
                          NULL,
                          LCOLF_CONSECRGB,
                          0L,
                          numRealClrs,
                          realClrs);
   // Create a bitmap, and set it in the temp HPS, so
   // that we can copy the rectangle into it
   pBmih->cbFix=sizeof(BITMAPINFOHEADER);
   pBmih->cx=(USHORT)sizeBmp.cx;
   pBmih->cy=(USHORT)sizeBmp.cy;
   DevQueryCaps(hdcTemp,CAPS_COLOR_PLANES,1L,(PLONG)&pBmih->cPlanes);
   DevQueryCaps(hdcTemp,CAPS_COLOR_BITCOUNT,1L,(PLONG)&pBmih->cBitCount);

   hbmTemp=GpiCreateBitmap(hpsTemp,pBmih,0L,0L,NULL);
   if (hbmTemp==NULL) {
      WinAlarm(HWND_DESKTOP,WA_ERROR);
      ExitError=9050;
      goto EXIT_PROC;
   } /* endif */

   Old =GpiSetBitmap(hpsMem,hbmSimpic);
   if (Old==HBM_ERROR) {
      char Buffer[80];
      usRc = ERRORIDERROR(WinGetLastError (hab));
      sprintf(Buffer,"MOSS/E Print SetBitmap: Err l=%p s=%4.4X",ulRc,usRc);
      WinAlarm(HWND_DESKTOP,WA_ERROR);
      WinMessageBox (HWND_DESKTOP,    // parent
                     HWND_DESKTOP,       // owner
                     Buffer,
                     "MOSS/E Print: Info", 0, MB_OK);
   } /* endif */
   GpiSetBitmap(hpsTemp,hbmTemp);

   pointl[0].x=0;pointl[2].x=pRectl->xLeft;
   pointl[0].y=0;pointl[2].y=pRectl->yBottom;
   pointl[1].x=sizeBmp.cx;
   pointl[1].y=sizeBmp.cy;
   bmBitsLineSize=(((pBmih->cBitCount*sizeBmp.cx+31)/32)*4*pBmih->cPlanes);
   ulRc =GpiBitBlt(hpsTemp,hpsMem,3L,pointl,ROP_SRCCOPY,BBO_IGNORE);
   if (GPI_OK!=ulRc) {
      char Buffer[80];
      usRc = ERRORIDERROR(WinGetLastError (hab));
      sprintf(Buffer,"MOSS/E Print BitBlt: Err l=%p s=%4.4X",ulRc,usRc);
      WinAlarm(HWND_DESKTOP,WA_ERROR);
      WinMessageBox (HWND_DESKTOP,    // parent
                     HWND_DESKTOP,       // owner
                     Buffer,
                     "MOSS/E Print: Info", 0, MB_OK);
   } /* endif */
   GpiSetBitmap (hpsMem, NULL );

   // bmBitsLineSize is the size of one line of the bitmap
   // The formula was taken from the 1.2 Programming Reference
   bmBitsLineSize=(((pBmih->cBitCount*sizeBmp.cx+31)/32)*4*pBmih->cPlanes);

   dosRc=DosOpen(pFileName,
                 &hFile,
                 &action,
                 0L,
                 FILE_ARCHIVED,
                 0x11,
                 0x92,
                 0L);
   if (dosRc!=0) {
      WinAlarm(HWND_DESKTOP,WA_ERROR);
      ExitError=10000+dosRc;
      goto EXIT_PROC;
   } /* endif */

   // In 1.2, you must create the file as an array (actually, it is closer
   // to being a linked list), with each element equal to one of the atomic
   // types (icon, pointer, or bitmap).  All elements must be of the same
   // type.
   bmafh.usType=BFT_BITMAPARRAY;
   bmafh.cbSize=sizeof(BITMAPARRAYFILEHEADER);
   bmafh.offNext=0;                             // Points to next array hdr,
                                                // or 0 if this is the last
//   Removed the cx cy for device independancy
//   DevQueryCaps(hdcTemp,CAPS_WIDTH,1L,(PLONG)&bmafh.cxDisplay);
//   DevQueryCaps(hdcTemp,CAPS_HEIGHT,1L,(PLONG)&bmafh.cyDisplay);
   bmafh.cxDisplay=0;
   bmafh.cyDisplay=0;
   bmafh.bfh.usType=BFT_BMAP;                   // Array element - bitmap
   bmafh.bfh.cbSize=sizeof(BITMAPFILEHEADER);
   bmafh.bfh.xHotspot=0;
   bmafh.bfh.yHotspot=0;
   bmafh.bfh.offBits=sizeof(BITMAPARRAYFILEHEADER)+sizeof(RGB)*numRealClrs;
   bmafh.bfh.bmp.cbFix=sizeof(BITMAPINFOHEADER);
   bmafh.bfh.bmp.cx=(USHORT)sizeBmp.cx;
   bmafh.bfh.bmp.cy=(USHORT)sizeBmp.cy;
   bmafh.bfh.bmp.cPlanes=pBmih->cPlanes;
   bmafh.bfh.bmp.cBitCount=pBmih->cBitCount;

   bytesToWrite=sizeof(bmafh);
   dosRc=DosWrite(hFile,
                  &bmafh,
                  bytesToWrite,
                  &bytesWritten);
   if ((dosRc!=0) || (bytesToWrite!=bytesWritten)) {
      WinAlarm(HWND_DESKTOP,WA_ERROR);
      ExitError=10000+dosRc;
      goto EXIT_PROC;
   } /* endif */

   // Query all of the logical colors, to get their
   // RGB values, so that we can write them to a file
   for (index1=0; (ULONG)index1<numRealClrs; index1++) {
     index2=GpiQueryColorIndex(hpsTemp,0L,realClrs[index1]);
     rgbValues[index2].bBlue=(UCHAR)(realClrs[index1]);
     rgbValues[index2].bGreen=(UCHAR)(realClrs[index1]>>8);
     rgbValues[index2].bRed=(UCHAR)(realClrs[index1]>>16);
   } /* endfor */

   bytesToWrite=sizeof(RGB)*(SHORT)numRealClrs;
   dosRc=DosWrite(hFile,
                  rgbValues,
                  bytesToWrite,
                  &bytesWritten);
   if ((dosRc!=0) || (bytesToWrite!=bytesWritten)) {
      WinAlarm(HWND_DESKTOP,WA_ERROR);
      ExitError=10000+dosRc;
      goto EXIT_PROC;
   } /* endif */

   // Query and write the bitmap bits
   for (index2=0; index2<sizeBmp.cy; index2++) {
      GpiQueryBitmapBits(hpsTemp,index2,1L,bmBits,pBmi);
      dosRc=DosWrite(hFile,
                     bmBits,
                     (USHORT)bmBitsLineSize,
                     &bytesWritten);
      if ((dosRc!=0) || (bmBitsLineSize!=bytesWritten)) {
         WinAlarm(HWND_DESKTOP,WA_ERROR);
         ExitError=10000+dosRc;
         goto EXIT_PROC;
      } /* endif */
   } /* endfor */

   EXIT_PROC:

   if (hFile!=NULL) {
      dosRc=DosClose(hFile);
      if (dosRc!=0) ExitError=10000+dosRc;
   } /* endif */

   if (hbmTemp!=NULL) {
      GpiDeleteBitmap(hbmTemp);
   } /* endif */

   if (hpsTemp!=NULL) {
      GpiDestroyPS(hpsTemp);
      DevCloseDC(hdcTemp);
   } /* endif */

   WinAlarm(HWND_DESKTOP,WA_NOTE);
   return(ExitError);
}
#define BUFSIZE  0xFFFF
#define BUFSIZEL 0x0000FFFFL
/* Converts a bitmap using the given convertion table ***/
PUCHAR  LineDeb;
BOOL    New,Change;
UCHAR   OldChar,LastColor;
LONG    Line,TotalSize;
ULONG   bmBitsLineSize;
ULONG   Lines;

SEL     HugeSel,CurSel;
PCHAR   HugeBuff;
USHORT  NumSeg,Size,HugeShift;
UCHAR   Backg,Foreg;
UCHAR   DbleForeg,FirstBack,FirstFore,DbleBackg;
PUCHAR  CheckLine,TargetLine,SaveLine;
LONG    Read,Wrote;
USHORT  Curline,Curcol;
/********** Convert Bitmap to black and white with contrasts function ***/
USHORT  ConvertBitmap (HAB hab, HPS hpsSource,HBITMAP Old)
{
   USHORT usRc;
   USHORT i,j;
//   ULONG  ulRc;
   numRealClrs=GpiQueryRealColors(hpsSource,0L,0L,256L,realClrs);
   if (numRealClrs>256L) {
       numRealClrs=256L;
   } /* endif */
   GpiCreateLogColorTable(hpsSource,
                          NULL,
                          LCOLF_CONSECRGB,
                          0L,
                          numRealClrs,
                          realClrs);
   Backg=(UCHAR)CLR_BACKGROUND;
   Foreg=(UCHAR)GpiQueryColorIndex(hpsMem,LCOLOPT_REALIZED,0x00FFFFFF);
   if (Foreg==Backg) {
       Foreg=(UCHAR)CLR_NEUTRAL;
   } /* endif */
   /* Use reverse video fore becomes back  */
   /* Use luminosity to compute conversion */
   for (i=0;i<(USHORT)numRealClrs ;i++ ) {
      int Avg;
      Avg=( LOUCHAR(HIUSHORT(realClrs[i]))+
            HIUCHAR(LOUSHORT(realClrs[i]))+
            LOUCHAR(LOUSHORT(realClrs[i])) )/3 ;
      if (Avg>0x7F) {
           ForBack[(USHORT)i]=Backg;
      } else {
           ForBack[(USHORT)i]=Foreg;
      } /* endif */
   }  /* End for */
   /* Use reverse video fore becomes back  */
   /* Correct for text outputs             */
   for (i=0;i<NUM_PMCOLORS ;i++ ) {
      Color=WinQuerySysColor(HWND_DESKTOP,PMColors[i].SystemColor,0L);
      Index=GpiQueryColorIndex(hpsMem,LCOLOPT_REALIZED,Color);
      PMColors[i].Index=Index;
      if (Index <256L) {
         if (PMColors[i].Foreground)
             ForBack[(USHORT)Index]=Backg;
         else
             ForBack[(USHORT)Index]=Foreg;
      } else {
             DosBeep(2450,20);
      } /* endif */
   } /* endfor */
   CvtFunct=NULL;
   /*--------------------------------*/
   /* Build look ahead table in case */
   for (j=0;j<256;j++ ) { Contrast[j]=FALSE; } /* reset contrasts */
   if (numRealClrs==256) {
      CvtFunct=Cvt256;
      for (j=0;j<256;j++ ) {
          LookTbl[j]=ForBack[j];
      }
   }
   if (numRealClrs==16) {
      CvtFunct=Cvt16;
      FirstBack=(UCHAR) ((0x0F & Foreg) | SHL4(Backg));
      FirstFore=(UCHAR) (SHL4(Foreg) | (0x0F & Backg));
      DbleForeg=(UCHAR) ((0x0F&Foreg) |SHL4(Foreg)) ;
      DbleBackg=(UCHAR) ((0x0F&Backg) |SHL4(Backg)) ;
      for (j=0;j<256;j++ ) {
          UCHAR jj,Bas,Haut,ForBas,ForHaut;
          jj=(UCHAR)j;
          Bas=(UCHAR)(0x0F&jj);
          Haut=(UCHAR)SHR4((0xF0&jj));
          ForBas =ForBack[Bas];
          ForHaut=ForBack[Haut];
          LookTbl[j]=(UCHAR)((0x0F& ForBas) + (SHL4(ForHaut)));
          if (Bas!=Haut) {
             if (LookTbl[j]==DbleForeg) {
                 Contrast[j]=TRUE;             /* is  contrasting */
             } /* endif */
             if (LookTbl[j]==DbleBackg) {
                 Contrast[j]=TRUE;             /* is  contrasting */
             } /* endif */
          } /* endif */
//          fprintf(Out,"j = %2.2X LookTbl= %2.2X  Contrast= %d\n",j,
//                 (USHORT)LookTbl[j],Contrast[j]);
      }  /* End for */
   } /* endif */
   if (CvtFunct==NULL) {
      return 0;
   } /* endif */
   /* Select the bitmap */
   GpiSetBitmap (hpsMem, Old );
   /*----------------------------------------------*/
   Line=GpiQueryBitmapParameters(Old,pBmih);
   bmBitsLineSize=(((pBmih->cBitCount*pBmih->cx+31)/32)*4*pBmih->cPlanes);
   Lines=0x00010000L/((ULONG)bmBitsLineSize); /* How many lines in 64K */
   TotalSize=((LONG)bmBitsLineSize)*((LONG)BmiTemp.cy);
   /* Allocate 3 more lines for contrast checking */
   NumSeg=(pBmih->cy+3)/((USHORT)Lines);
   Size=(USHORT)(((pBmih->cy+3)-NumSeg*Lines)*bmBitsLineSize);
   if (NumSeg>0) { /* Exceeds 64 K of  DATA */
       usRc=DosAllocHuge(NumSeg,Size,&HugeSel,0,0);
   } else {
       usRc=DosAllocSeg(Size,&HugeSel,0);
   } /* endif */
   if (usRc!=NO_ERROR)
   {
        char Buffer[80];
        sprintf(Buffer,"MOSS/E Alloc: Error=%4.4X",usRc);
        WinAlarm(HWND_DESKTOP,WA_ERROR);
        WinMessageBox (HWND_DESKTOP,    // parent
                       HWND_DESKTOP,       // owner
                       Buffer,
                       "Error ", 0, MB_OK);
        return 1;
   } /* endif */
   DosGetHugeShift(&HugeShift);
   CurSel=HugeSel;
   HugeBuff=MAKEP(CurSel,0);
   for (Line=0L;Line<(LONG)pBmih->cy;Line+=Read) {
      if (Line+Lines>=(LONG)pBmih->cy) Read=(LONG)pBmih->cy-Line;
      else Read=Lines;
      if (Line==0L) { /* Leave room for contrast checking */
         CheckLine=HugeBuff;
         TargetLine=CheckLine +bmBitsLineSize;
         SaveLine  =TargetLine+bmBitsLineSize;
         HugeBuff  =SaveLine  +bmBitsLineSize;
         Read-=3;
      } /* endif */
      Read=GpiQueryBitmapBits(hpsSource,Line,Read,HugeBuff,pBmi);
      if (Read==GPI_ALTERROR)
      {
           char Buffer[80];
           usRc = ERRORIDERROR(WinGetLastError (hab));
           sprintf(Buffer,"MOSS/E Query: Err s=%4.4X",usRc);
           WinAlarm(HWND_DESKTOP,WA_ERROR);
           WinMessageBox (HWND_DESKTOP,    // parent
                          HWND_DESKTOP,       // owner
                          Buffer,
                          "Error ", 0, MB_OK);
      } /* endif */
      for (Curline=0;Curline<(USHORT)Read;Curline++) {
         LineDeb=HugeBuff+(Curline*bmBitsLineSize );
         memcpy(SaveLine,LineDeb,(USHORT)bmBitsLineSize);
         OldChar=(UCHAR) SHR4((*LineDeb));
         for (Curcol=0;Curcol<(USHORT)bmBitsLineSize;Curcol++) {
            (*CvtFunct)(); /* Call convert function */
         } /* endfor */
         memcpy(CheckLine ,SaveLine,(USHORT)bmBitsLineSize);
         memcpy(TargetLine,LineDeb ,(USHORT)bmBitsLineSize);
      } /* endfor */
      Wrote=GpiSetBitmapBits(hpsSource,Line,Read,HugeBuff,pBmi);
      if (Wrote==GPI_ALTERROR)
      {
           char Buffer[80];
           usRc = ERRORIDERROR(WinGetLastError (hab));
           sprintf(Buffer,"MOSS/E Set  : Err s=%4.4X",usRc);
           WinAlarm(HWND_DESKTOP,WA_ERROR);
           WinMessageBox (HWND_DESKTOP,    // parent
                          HWND_DESKTOP,       // owner
                          Buffer,
                          "Error ", 0, MB_OK);
      } /* endif */
      if (NumSeg>0) {
         CurSel+=(0x0001<<HugeShift);
         HugeBuff=MAKEP(CurSel,0);
      } else {
         break; /* One segment always leave loop */
      } /* endif */
   } /* endfor */
   DosFreeSeg(HugeSel);
   GpiSetBitmap (hpsMem, NULL );
}
void    Cvt16()
{       /* Convert  for  16 colors */
   register UCHAR Temp1,Temp2;
   static   PUCHAR pCh;
   static   UCHAR TheChar;
   pCh=LineDeb+Curcol;
   Temp1=(UCHAR)(*pCh);
   Temp2=(UCHAR)(Temp1&0xF0);
   Temp2+=OldChar;
   TheChar=LookTbl[(USHORT)Temp1];
   New    =Contrast[(USHORT)Temp1];
   Change =Contrast[(USHORT)Temp2];
   if (Change) {
      if (TheChar==DbleForeg) {
          if (LastColor==Foreg) TheChar=FirstFore;
          else                  TheChar=FirstBack;
      } else {
          if (LastColor==Foreg) TheChar=FirstFore;
          else                  TheChar=FirstBack;
      } /* endif */
      if (Line+Curline>0L) {
           UCHAR LastIn  ,LastOut;
           LastIn =*(CheckLine +Curcol);
           LastOut=*(TargetLine+Curcol);
           if ( ((UCHAR)(0x0F & LastIn))==((UCHAR)(0x0F&Temp1))) {
               TheChar=(TheChar&0xF0)|(LastOut&0x0F);
           }
      }
   } else {
      if (New) {
         if (TheChar==DbleForeg) {
          if (LastColor==Foreg) TheChar=FirstFore;
          else                  TheChar=FirstBack;
         } else {
          if (LastColor==Foreg) TheChar=FirstFore;
          else                  TheChar=FirstBack;
         } /* endif */
           if (Line+Curline>0L) {
              UCHAR LastIn  ,LastOut;
              LastIn =*(CheckLine +Curcol);
              LastOut=*(TargetLine+Curcol);
              if ( ((UCHAR)(0xF0 & LastIn))==((UCHAR)(0xF0&Temp1))) {
                  TheChar=(TheChar&0x0F)|(LastOut&0xF0);
              }
           }
      } else { /* no horizontal contrast check for vertical contrast */
         if (Line+Curline>0L) {
            UCHAR LastIn  ,LastOut;
            UCHAR InVert1 ,InVert2;
            UCHAR OutVert1,OutVert2;
            LastIn =*(CheckLine +Curcol);
            LastOut=*(TargetLine+Curcol);
            InVert1 =(UCHAR)(((UCHAR)(0xF0 & LastIn ))+SHR4(0xF0&Temp1  ));
            OutVert1=(UCHAR)(((UCHAR)(0xF0 & LastOut))+SHR4(0xF0&TheChar));
            InVert2 =(UCHAR)(((UCHAR)(0x0F & LastIn ))+SHL4(0x0F&Temp1  ));
            OutVert2=(UCHAR)(((UCHAR)(0x0F & LastOut))+SHL4(0x0F&TheChar));
            if (Contrast[MAKEUSHORT(InVert1,0)]) {
               if (OutVert1==DbleForeg) {
                   TheChar=((UCHAR)(TheChar&0x0F)) + SHL4(Backg);
               } else {
                  if (OutVert1==DbleBackg) {
                     TheChar=((UCHAR)(TheChar&0x0F)) + SHL4(Foreg);
                  } /* endif */
               } /* endif */
            }
            if (Contrast[MAKEUSHORT(InVert2,0)]) {
               if (OutVert2==DbleForeg) {
                   TheChar=((UCHAR)(TheChar&0xF0)) + Backg;
               } else {
                  if (OutVert2==DbleBackg) {
                     TheChar=((UCHAR)(TheChar&0xF0)) + Foreg;
                  } /* endif */
               } /* endif */
            }
         } /* endif */
      } /* endif */
   } /* endif */
   OldChar  =(UCHAR) (0x0F&Temp1  );
   LastColor=(UCHAR) (0x0F&TheChar);
   *pCh=TheChar;
}
void    Cvt256()
{     /* Convert for 256 colors */
   register UCHAR Temp1,Temp2;
   static PUCHAR pCh;
   static UCHAR TheChar;
   pCh=LineDeb+Curcol;
   Temp1=(UCHAR)(*pCh);
   Temp2=(UCHAR)OldChar;
   TheChar=LookTbl[(USHORT)Temp1];
   /* Check if must contrast Colors */
   if ((TheChar==LastColor) &&
       (Temp1!=Temp2) ) {
      if (TheChar==Foreg) TheChar=Backg;
      else                TheChar=Foreg;
      if (Line+Curline>0L) { /* correct vertical contrasts */
           UCHAR LastIn  ,LastOut;
           LastIn =*(CheckLine +Curcol);
           LastOut=*(TargetLine+Curcol);
           if (  ((UCHAR)LastIn) ==((UCHAR)Temp1) ) {
               TheChar=LastOut;
           }
      }
   } else {
      if (Line+Curline>0L) {
          UCHAR LastIn  ,LastOut;
          LastIn =*(CheckLine +Curcol);
          LastOut=*(TargetLine+Curcol);
          if ((TheChar==LastOut) && /* if need for vertical contrast */
              (Temp1!=LastIn) ) {
              if (TheChar==Foreg) TheChar=Backg;
              else                TheChar=Foreg;
          }
      } /* endif */
   } /* endif */
   OldChar  =(UCHAR) Temp1;
   LastColor=(UCHAR) TheChar;
   *pCh=TheChar;
}
