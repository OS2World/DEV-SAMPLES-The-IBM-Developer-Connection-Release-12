/*** OS/2 Application Primer "MISC Sample Main Routine" ***************/
/*                                                                    */
/* Program name : MISCMAIN.C                                          */
/* Version : 2.1 (12/24/93)                                           */
/*                                                                    */
/*    << This sample program is only for DBCS OS/2 V2.1. >>           */
/*                                                                    */
/*    Main routine for the Sample Miscellenaeus program.              */
/*                                                                    */
/**********************************************************************/
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#define INCL_DOSNLS
#define INCL_NLS

#include <os2.h>                       /* System Include File      */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <limits.h>
#include <malloc.h>

#include "miscrsc.h"
#include "miscprg.h"

MRESULT EXPENTRY miscMainWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT EXPENTRY editorWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT EXPENTRY cpConvDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT EXPENTRY wordDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

HAB hab;
QMSG qmsg;
HMQ hmq;

void main( void )
{
    ULONG flCreate;
    HWND hwndFrame;
    HWND hwndClient;

    hab = WinInitialize((USHORT) NULL);

    hmq = WinCreateMsgQueue(hab,0);

    WinRegisterClass(hab,
                     "miscMainWindow",
                     (PFNWP) miscMainWinProc,
                     CS_SIZEREDRAW,
                     0);

    WinRegisterClass(hab,
                     "editorWindow",
                     (PFNWP) editorWinProc,
                     CS_SIZEREDRAW,
                     0);

    flCreate= FCF_MAXBUTTON | FCF_MENU | FCF_MINBUTTON | FCF_SIZEBORDER | FCF_SYSMENU |
              FCF_TITLEBAR | FCF_TASKLIST ;

    hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                       WS_VISIBLE,
                                       &flCreate,
                                       "miscMainWindow",
                                       "DBCS Misc Samples",
                                       CS_SIZEREDRAW,
                                       NULLHANDLE,
                                       WID_MAIN,
                                       (PHWND) & hwndClient);

    while ( WinGetMsg(hab, (PQMSG) &qmsg, (HWND) NULL, 0, 0))
       WinDispatchMsg(hab, (PQMSG) &qmsg );

    WinDestroyWindow(hwndFrame);
    WinDestroyMsgQueue( hmq );
    WinTerminate( hab );

  return;
}

/*** Window Procedure for "Misc Main Window" **************************/
/*                                                                    */
/* function name : miscMainWinProc                                    */
/*                                                                    */
/**********************************************************************/

extern void GetFontSize(HPS hps, LONG *lWidth, LONG *lHeight);
#define NUM_MAINTEXT 3
MRESULT EXPENTRY miscMainWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static HPS hpsMain;
static ULONG lCodePage;
static LONG lFontHeight, lAveWidth;
static LONG desk_cx, desk_cy;
    RECTL rectl;
    HWND hwndEditorWin, hwndEditorClient;
    ULONG flCreate;
    FONTDLG fontDlg;
    char szFamilyname[FACESIZE];
    char szExmpl[128];
    ULONG stringID[NUM_MAINTEXT] = {IDS_MAINTEXT1, IDS_MAINTEXT2,
                                    IDS_MAINTEXT3 };
    static char* sText=NULL;
    static char* sTextEnjoy;
    void setFont( HWND hwnd, HPS hps, PFATTRS pAttrs );

    switch(msg)
    {
        case WM_CREATE:
        {
        int i;
        SIZEL sizl;
        HDC hdc;
        HWND hwndFrame;
        ULONG dataLength;
        char buf[BUFSIZ];
        LONG x,y,cx,cy;

            /* Read message text */
            for (i = 0; i < NUM_MAINTEXT; i++)
            {
              WinLoadString( hab, NULLHANDLE,
                             stringID[i], sizeof(buf), buf );
              if( sText == NULL )
              {
                 sText = malloc(strlen(buf)+1);
                 *sText = '\0';
              }
              else
                 sText = realloc( sText, strlen(sText)+strlen(buf)+1 );

              strcat( sText, buf );
            }

            WinLoadString( hab, NULLHANDLE,
                           IDS_TEXTENJOY, sizeof(buf), buf );

            sTextEnjoy = malloc( strlen(buf)+1 );
            strcpy( sTextEnjoy, buf );

            sizl.cx = 0L;
            sizl.cy = 0L;

            hdc = WinOpenWindowDC( hwnd );
            hpsMain = GpiCreatePS( hab,
                                   hdc,
                                   (PSIZEL)&sizl,
                                   (ULONG)PU_PELS | GPIT_MICRO | GPIA_ASSOC
                                  );
            /* Query the environment */
            DosQueryCp( sizeof(lCodePage), &lCodePage, &dataLength );

            GetFontSize( hpsMain, &lAveWidth, &lFontHeight );

            hwndFrame = WinQueryWindow(hwnd, QW_PARENT);

            desk_cx = WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
            desk_cy = WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );

            /* set window width to show maximum 40 SBCS characters */
            cx = (lAveWidth*MAIN_WIN_WIDTH>desk_cx) ? desk_cx : lAveWidth*MAIN_WIN_WIDTH;
            /* set window height large enough to show a string pointed to by sText.*/
            cy = (((strlen(sText)/MAIN_WIN_WIDTH)+10)*lFontHeight>desk_cy) ?
                  desk_cy : ((strlen(sText)/40) + 10)*lFontHeight;

            x = (cx<desk_cx) ? (desk_cx-cx)/2 : 0;
            y = (cy<desk_cy) ? (desk_cy-cy)/2 : 0;
            WinSetWindowPos(hwndFrame,
                            HWND_BOTTOM,
                            x, y, cx, cy,
                            SWP_MOVE | SWP_SIZE | SWP_ACTIVATE);

            return(MRESULT)(FALSE);
        }

        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1))
            {
                case MID_CONV:    /* CPCONV */
                {
                  WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP) cpConvDlgProc,
                            NULLHANDLE, DID_CONV, &lCodePage);
                  break;
                }

                case MID_EDITOR:  /* Simple Editor */
                  flCreate= FCF_SIZEBORDER | FCF_MENU | FCF_MAXBUTTON | FCF_MINBUTTON
                          | FCF_SYSMENU | FCF_TITLEBAR | FCF_DBE_APPSTAT;
                  hwndEditorWin = WinCreateStdWindow(HWND_DESKTOP,
                                                    WS_VISIBLE,
                                                    &flCreate,
                                                    "editorWindow",
                                                    "Simple Editor",
                                                    0L,
                                                    NULLHANDLE,
                                                    WID_EDITOR,
                                                    (PHWND) & hwndEditorClient);

                  WinSetWindowPos(hwndEditorWin,
                                  HWND_BOTTOM,
                                  190, 130, 500, 300,
                                  SWP_MOVE | SWP_SIZE | SWP_ACTIVATE);
                  break;

                case MID_WORD:    /* Word Break */
                {
                  WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP) wordDlgProc,
                            NULLHANDLE, DID_WORD, &lCodePage);
                  break;
                }

                case MID_EXIT:    /* Exit */
                  WinSendMsg (hwnd, WM_CLOSE,mp1,mp2);
                  break;
            }
            break;

        case WM_PAINT:
        {
         int i;
         LONG lTotLen, lWrittenLen, lDrawn;
         SWP swp;

            WinBeginPaint( hwnd, hpsMain, (PRECTL)&rectl );
            /* Always update whole window - CS_SIZEREDRAW? */
            WinQueryWindowPos( hwnd, &swp );
            rectl.xLeft = rectl.yBottom = 0;
            rectl.xRight = swp.cx;
            rectl.yTop = swp.cy;

            WinFillRect( hpsMain, (PRECTL) &rectl, CLR_BACKGROUND );

            lTotLen = (LONG)strlen(sText);

            /* make some space between the text and the frame window */
            rectl.xLeft+=lAveWidth;
            rectl.xRight-=lAveWidth;
            rectl.yTop-=lFontHeight;

            for (lWrittenLen = 0; lWrittenLen != lTotLen; rectl.yTop -= lFontHeight)
            {
              lDrawn = WinDrawText( hpsMain, lTotLen - lWrittenLen,
                                    sText+lWrittenLen, &rectl, 0L, 0L,
                                    DT_WORDBREAK | DT_TOP | DT_LEFT | DT_TEXTATTRS);

              if( lDrawn != 0 )
                lWrittenLen  += lDrawn;
              else
                break;
            }

            rectl.yTop -= lFontHeight;
            WinDrawText( hpsMain, strlen(sTextEnjoy), sTextEnjoy, &rectl,
                         CLR_RED, CLR_BACKGROUND,
                         DT_TOP | DT_CENTER );
            WinEndPaint( hpsMain );
            break;
        }

        case WM_DESTROY:
            GpiDestroyPS( hpsMain );
            break;

        default:
            return(WinDefWindowProc(hwnd,msg,mp1,mp2));
    }
  return(MRFROMLONG(NULL));
}
