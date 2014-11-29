/*----------------------------------------------*/
/* プログラム名: NLS32                                                        */
/* -------------                                                              */
/*  プレゼンテーション・マネージャー・タイピング ＤＢＣＳ  Ｃ                 */
/*  サンプル・プログラム   32ビット対応                                       */
/*                                                                            */
/* コピーライト:                                                              */
/* ----------                                                                 */
/*  (C) Copyright International Business Machines Corporation 1988, 1994      */
/*  Copyright (c) 1988 Microsoft Corporation                                  */
/*                                                                            */
/* 改訂レベル: 1.1                                                            */
/* ---------------                                                            */
/*                                                                            */
/* プログラムの概略:                                                          */
/* -----------------------                                                    */
/*  このプログラムは，ユーザーが80文字のテキスト行を書き込むことができる      */
/*  ウィンドウを表示します。書き込んだテキストを最終的に表示する際，          */
/*  禁則処理を適用するかどうかを指定できます。                                */
/*                                                                            */
/*  また, フォント・ドライバーをロード/アンロードしたり，使用可能なフォントの */
/*  一覧表示からフォントを選択し，そのフォントを使ってテキスト行を表示する    */
/*  ことができます。                                                          */
/*                                                                            */
/*  DBCS モード状況についても，メニュー操作によりかな漢字変換の状況(たとえば  */
/*  スポット変換のオン/オフなど)を変更することができます。                    */
/*                                                                            */
/* プログラムの役割:                                                          */
/* -----------------                                                          */
/*  このプログラムは，フォント・ドライバーのロード/アンロード, DBCS モード    */
/*  状況の変更など，DBCS 固有の機能についてその使い方を示すものです           */
/*                                                                            */
/* コンパイルに必要なもの:                                                    */
/* -----------------------                                                    */
/*                                                                            */
/*  必要なファイル:                                                           */
/*  ---------------                                                           */
/*                                                                            */
/*    NLS32.C         - このCサンプル・プログラムのソース・コード             */
/*    NLS32.DEF       - 定義ファイル                                          */
/*    NLS32.DLG       - ダイアログ・リソース・ファイル                        */
/*    NLS32.H         - 適用業務ヘッダー・ファイル                            */
/*    NLS32.ICO       - アイコン・ファイル                                    */
/*    NLS32.L         - リンカー自動応答ファイル                              */
/*    NLS32.RC        - リソース・ファイル                                    */
/*    MAKEFILE        - メイク用ファイル                                      */
/*                                                                            */
/*    OS2.H          - プレゼンテーション・マネージャー・インクルード・       */
/*                     ファイル                                               */
/*    STRING.H       - 種々の機能宣言                                         */
/*                                                                            */
/*  必要なライブラリー:                                                       */
/*  -------------------                                                       */
/*                                                                            */
/*    OS2386.LIB     - プレゼンテーション・マネージャー/OS2ライブラリー       */
/*    DDE4MBS.LIB    - 標準 C ライブラリー                                    */
/*    PMNLS386.LIB   - PM/OS2 NLS サポート・ライブラリー                      */
/*                                                                            */
/*  必要なプログラム:                                                         */
/*  -----------------                                                         */
/*                                                                            */
/*    IBM C/C++ コンパイラー                                                  */
/*    IBM 32ビット・リンカー                                                  */
/*    リソース・コンパイラー                                                  */
/*                                                                            */
/* 予定される入力:                                                            */
/* ---------------                                                            */
/*                                                                            */
/* 予定される出力:                                                            */
/* ---------------                                                            */
/*                                                                            */
/* 使用される呼び出し（ダイナミック・リンク参照）:                            */
/* -----------------------------------------------                            */
/* GpiAssociate            WinAlarm                                           */
/* GpiCharString           WinAllocMem                                        */
/* GpiCreateLogFont        WinBeginPaint                                      */
/* GpiCreatePS             WinCreateCursor                                    */
/* GpiDeleteSetId          WinCreateMsgQueue                                  */
/* GpiDestroyPS            WinCreateStdWindow                                 */
/* GpiLine                 WinDefWindowProc                                   */
/* GpiQueryColorIndex      WinDestroyCursor                                   */
/* GpiQueryCp              WinDestroyHeap                                     */
/* GpiQueryFontMetrics     WinDestroyMsgQueue                                 */
/* GpiQueryTextBox         WinDestroyWindow                                   */
/* GpiSetAttrs             WinDismissDlg                                      */
/* GpiSetBackColor         WinDispatchMsg                                     */
/* GpiSetBackMix           WinDlgBox                                          */
/* GpiSetCharSet           WinDrawText                                        */
/* GpiSetColor             WinEndPaint                                        */
/*                         WinFillRect                                        */
/*                         WinFreeMem                                         */
/*                         WinGetMsg                                          */
/*                         WinInitialize                                      */
/*                         WinInvalidateRect                                  */
/*                         WinLoadString                                      */
/*                         WinMessageBox                                      */
/*                         WinOpenWindowDC                                    */
/*                         WinPostMsg                                         */
/*                         WinPrevChar                                        */
/*                         WinQueryCp                                         */
/*                         WinQuerySysColor                                   */
/*                         WinQueryWindowRect                                 */
/*                         WinQueryWindowText                                 */
/*                         WinQueryWindowULong                                */
/*                         WinRegisterClass                                   */
/*                         WinSendDlgItemMsg                                  */
/*                         WinSendMsg                                         */
/*                         WinShowCursor                                      */
/*                         WinSubclassWindow                                  */
/*                         WinTerminate                                       */
/*                         WinWindowFromID                                    */
/*                                                                            */
/*                         WinDBCSIMEControl                                  */
/*                         WinDBCSLoadFontDriver                              */
/*                         WinDBCSModeControl                                 */
/*                         WinDBCSUnloadFontDriver                            */
/*                         WinDBCSQueryFDDescription                          */
/*                                                                            */
/******************************************************************************/

/***********************************************************************/
/*         PM ヘッダー・ファイルから必要な部分をインクルード           */
/***********************************************************************/

#define INCL_WIN
#define INCL_GPI
#define INCL_DOSPROCESS
#define INCL_DOSMODULEMGR
#define INCL_DOSMEMMGR
#define INCL_WINSTDFONT
#define INCL_NLS
/***********************************************************************/
/*               必要なヘッダー・ファイルをインクルード                */
/***********************************************************************/

#include <os2.h>
#include <os2nls.h>
#include <string.h>
#include <stdlib.h>
#include "nls32.h"

/*----------------------------- 一般的な定義 ------------------------------*/

HAB     hab;                  /* アンカー･ブロック･ハンドル                */
HMQ     hmq;                  /* 適用業務待ち行列ハンドル                  */
HWND    hwndClient;           /* クライアント域ハンドル                    */
HWND    hwndFrame;            /* フレーム制御ハンドル                      */
HDC     hdcNls;               /* ウィンドウ･デバイス･コンテキスト･ハンドル */
HPS     hpsNls;               /* NLS 表示空間ハンドル                      */

CHAR szClassName[CCHMAXSTRING];
CHAR szWindowTitle[CCHMAXSTRING];
CHAR szDlgHelp[CCHMAXSTRING];
CHAR szFontDriver[CCHMAXSTRING];
CHAR szIMEName[CCHMAXSTRING];
CHAR szGeneral[CCHMAXSTRING];
CHAR szPecicText[CCHMAXSTRING];
CHAR szlastText[CCHMAXSTRING];
CHAR szPortd[CCHMAXSTRING];
CHAR szPortdkey[CCHMAXSTRING];

CHAR    Linebuf[BUFFERLENGTH];
POINTS  CursorPos = {0, 0};            /* PS座標空間での現在位置 */
ULONG   Origin;
ULONG   CharHeight;
ULONG   Curpos = 0;
ULONG   clStart = 0;
ULONG   codepage;
ULONG   sizereq;

NPBYTE  npMem;

LONG   lastSelect = 0;

PFNWP   pfnWndProc;

FATTRS  SelectedFont;

CHARBUNDLE cb;

/*----------------------------- DBCS 機能標識 -----------------------------*/
static CHAR  Appltext[] = "適用業務はこの部分に任意の文字列を表示可能";

unsigned char Applattr[48]={ 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                             4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                             4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

FD_DESC FDdesc;
IMEMODE IMEmode;

WPMCTLPARAMS Wpmctl = { DBE_KSF_TEXT,
                        0,
                        Appltext,
                        Applattr  };

WNDPARAMS Applparam = { WPM_CTLDATA,
                        0,
                        0L,
                        0,
                        0L,
                        sizeof(WPMCTLPARAMS),
                        &Wpmctl };

ULONG  Priority = DBE_AS_APPLBOTTOM;

WNDPARAMS Visibility = { DBE_WPM_APPLSTAT_VISIBILITY,
                         0,
                         0L,
                         0,
                         0L,
                         sizeof(ULONG),
                         &Priority };

MPARAM  mpDBCSmode = MPFROM2SHORT( IDCMD_ALPHA, TRUE );

/*---------------------------- 内部標識フラグ -----------------------------*/
ULONG   fIsOntheSpot = FALSE;
ULONG   fIsInline = FALSE;
ULONG   fIsEnabled = TRUE;
ULONG   fIsActivated = FALSE;
ULONG   fIsWordwrap = FALSE;
ULONG   fIsApplstat = FALSE;
ULONG   fPriority = 0;
ULONG   fDBCSmode;

/*********************** メイン・プロシージャー開始  ***********************/

VOID main(VOID)
{
    QMSG qMsg;
    ULONG ctlData;

    hab = WinInitialize(0);
    hmq = WinCreateMsgQueue( hab, 0 );

    /*******************************************************************/
    /* 待ち行列ハンドルがNULLかどうかのチェック                        */
    /*******************************************************************/
    if (!hmq == 0L)
    {
        WinLoadString( hab, 0, IDSNAME, CCHMAXSTRING, (PSZ)szClassName );
        WinLoadString( hab, 0, IDSDLGHELP, CCHMAXSTRING, (PSZ)szDlgHelp );

        WinRegisterClass( hab,
                          (PSZ)szClassName,
                          (PFNWP)MyWndProc,
                          CS_SIZEREDRAW,
                          0 );

        ctlData = FCF_STANDARD | FCF_DBE_APPSTAT;

        hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                        WS_VISIBLE,
                                        &ctlData,
                                        (PSZ)szClassName,
                                        (PSZ)NULL,
                                        0L,
                                        (HMODULE)NULL,
                                        ID_NLSSAMP,
                                        (HWND FAR *)&hwndClient );

        /********************************************************************/
        /* システムからウィンドウ・タイトルを得る。                         */
        /********************************************************************/
        WinQueryWindowText( hwndFrame, CCHMAXSTRING, (PSZ)szWindowTitle );

        /********************************************************************/
        /* 事象待ち行列からのメッセージをポーリングする。                   */
        /********************************************************************/
        while ( WinGetMsg( hab, (PQMSG)&qMsg, (HWND)NULL, 0, 0 ) )
            WinDispatchMsg( hab, (PQMSG)&qMsg );

        WinDestroyWindow( hwndFrame );
        WinDestroyMsgQueue( hmq );
    }

    WinTerminate( hab );
    DosExit( EXIT_PROCESS, 0 );
}

MRESULT EXPENTRY MyWndProc(hwnd, msg, mp1, mp2)
HWND   hwnd;
ULONG  msg;
MPARAM mp1;
MPARAM mp2;
{
    RECTL  PaintArea, rect;
    POINTL points[TXTBOX_COUNT];
    ULONG  i;
    ULONG  usLen;

    switch (msg)
    {
    case WM_CREATE:
        /********************************************************************/
        /* 待ち行列コード・ページを得る。                                   */
        /********************************************************************/
        codepage = WinQueryCp( (HMQ)WinQueryWindowULong(hwnd, QWL_HMQ) );
        NlssampCreate( hwnd );
        return(MRESULT)(FALSE);

    case WM_COMMAND:
        NlssampCommand( hwnd, mp1 );
        break;

    case WM_ERASEBACKGROUND:
        return(MRESULT)(TRUE);

    case WM_CLOSE:
        WinPostMsg( hwnd, WM_QUIT, 0L, 0L );
        break;

    case WM_DESTROY:
        WinSendMsg( hwnd, WM_COMMAND,
                    MPFROM2SHORT( IDCMD_UNLOAD, TRUE ), 0L );
        GpiDestroyPS( hpsNls );
        break;

    case WM_SETFOCUS:
        WinQueryWindowRect( hwnd, &rect );
        Origin = (ULONG)rect.yTop - CharHeight;

        /********************************************************************/
        /* ウィンドウがフォーカスを持っていれば明滅カレットを表示           */
        /* 持っていなければカレットを消去                                   */
        /********************************************************************/
        if (!SHORT1FROMMP(mp2))
        {
            WinShowCursor( hwnd, FALSE );
            WinDestroyCursor( hwnd );
        }
        else
        {
            WinCreateCursor( hwnd, CursorPos.x, Origin-CursorPos.y, 0,
                             CharHeight, CURSOR_SOLID|CURSOR_FLASH,
                             (PRECTL)NULL );
            WinShowCursor( hwnd, TRUE );
        }
        break;

    case WM_INITMENU:
        /********************************************************************/
        /* IME モード，キ－ボ－ド状況域の初期設定                           */
        /********************************************************************/
        WinDBCSIMEControl( hab, hwndFrame, DBE_IMCTL_QUERY,
                           (PIMEMODE)&IMEmode );
        if ( fIsEnabled )
        {
            WinSendMsg( WinWindowFromID( hwndFrame, FID_MENU ),
                        MM_SETITEMATTR,
                        MPFROM2SHORT( IDCMD_ENABLE, TRUE ),
                        MPFROM2SHORT( MIA_CHECKED, MIA_CHECKED ) );
        }
        else
        {
            WinSendMsg( WinWindowFromID( hwndFrame, FID_MENU ),
                        MM_SETITEMATTR,
                        MPFROM2SHORT( IDCMD_ENABLE, TRUE ),
                        MPFROM2SHORT( MIA_CHECKED, 0 ) );
        }

        WinDBCSModeControl( hab, hwndFrame, DBE_MCTL_QUERY,
                            DBE_MCTL_JAPANREQ, (PULONG)&fDBCSmode );

        WinSendMsg( WinWindowFromID( hwndFrame, FID_MENU ),
                    MM_SETITEMATTR,
                    mpDBCSmode,
                    MPFROM2SHORT( MIA_CHECKED, 0 ) );

        if ( fDBCSmode & DBE_MCTL_ALPHANUMERIC )
        {
            mpDBCSmode = MPFROM2SHORT( IDCMD_ALPHA, TRUE );
        }
        else if ( fDBCSmode & DBE_MCTL_KATAKANA )
        {
            mpDBCSmode = MPFROM2SHORT( IDCMD_KATAKANA, TRUE );
        }
        else if ( fDBCSmode & DBE_MCTL_HIRAGANA )
        {
            mpDBCSmode = MPFROM2SHORT( IDCMD_HIRAGANA, TRUE );
        }

        WinSendMsg( WinWindowFromID( hwndFrame, FID_MENU ),
                    MM_SETITEMATTR,
                    mpDBCSmode,
                    MPFROM2SHORT( MIA_CHECKED, MIA_CHECKED ) );
        break;

    case WM_PAINT:
        WinBeginPaint( hwnd, hpsNls, (PRECTL)&PaintArea );
        WinShowCursor( hwnd, FALSE );

        WinFillRect( hpsNls, (PRECTL)&PaintArea, SYSCLR_WINDOW );

        WinQueryWindowRect( hwnd, &rect );
        Origin = (ULONG)rect.yTop - CharHeight;
        /********************************************************************/
        /* WinDrawTextを使ってバッファー内の全ストリングを表示              */
        /* "折り返し"フラグがオンならDT_WORDBREAKフラグがWinDrawTextを渡す｡ */
        /* １行の文字数はWinDrawTextをDT_QUERYEXTENTを指定して呼び出すこと  */
        /* により指定する。                                                 */
        /********************************************************************/
        i = usLen = CursorPos.y = 0;
        if (fIsWordwrap)
        {
             while ( i < Curpos )
             {
                 rect.yBottom = rect.yTop - CharHeight;
                 usLen = WinDrawText( hpsNls, -1, Linebuf+i, &rect, 0L, 0L,
                              DT_QUERYEXTENT | DT_WORDBREAK | DT_TEXTATTRS );

                 WinDrawText( hpsNls, usLen, Linebuf+i, &rect, 0L, 0L,
                              DT_WORDBREAK | DT_TEXTATTRS );
                 clStart = i;

                 if ( (i += usLen) < Curpos )
                 {
                     CursorPos.y += CharHeight;
                     rect.yTop    -= CharHeight;
                 }
             }
        }
        else
        {
            while ( i < Curpos )
            {
                rect.yBottom = rect.yTop - CharHeight;
                if ( (usLen=strlen(Linebuf+i)) > LINELENGTH )
                {
                    usLen = 0;
                    while ( ( usLen = ( WinNextChar( hab, codepage, 0,
                              Linebuf+i+usLen )-(Linebuf+i)) ) < LINELENGTH );
                    if (usLen > LINELENGTH )
                        usLen -= DBCSCHAR;
                }

                WinDrawText( hpsNls, usLen, Linebuf+i, &rect, 0L, 0L,
                             DT_TEXTATTRS );

                rect.yTop  -= CharHeight;
                clStart = i;

                if ( (i += usLen) < Curpos )
                    CursorPos.y += CharHeight;
            }
        }

        GpiQueryTextBox( hpsNls,
                         (LONG)usLen,
                         Linebuf+clStart,
                         TXTBOX_COUNT,
                         points );

        CursorPos.x = (ULONG)points[TXTBOX_CONCAT].x;
        WinCreateCursor( hwnd, CursorPos.x, Origin-CursorPos.y, 0,
                         CharHeight, CURSOR_SOLID|CURSOR_FLASH,
                         (PRECTL)NULL );
        WinShowCursor( hwnd, TRUE );
        PecicPaint( );

        WinEndPaint( hpsNls );
        break;

    case WM_QUERYCONVERTPOS:
        /********************************************************************/
        /* "スポット変換"フラグがオンなら, QCP_CONVERT にカーソル位置を返す */
        /********************************************************************/
        if (fIsOntheSpot)
        {
            ((PRECTL)mp1)->xLeft = CursorPos.x;
            ((PRECTL)mp1)->yBottom = Origin - CursorPos.y;
            ((PRECTL)mp1)->xRight = 0;
            ((PRECTL)mp1)->yTop = 0;
            return(MRFROMLONG(TRUE));
        }
        else
        {
            return ( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
        }
        break;

    case WM_CHAR:
        if ( !(LONGFROMMP(mp1) & KC_KEYUP) )
        {
            WinShowCursor( hwnd, FALSE );
            NlssampCharInput( hwnd, mp1, mp2 );
            WinCreateCursor( hwnd, CursorPos.x, Origin-CursorPos.y, 0, 0,
                             CURSOR_SETPOS, (PRECTL)NULL );
            WinShowCursor( hwnd, TRUE );
        }
        break;

    case WM_HELP:
        WinLoadString( hab,
                       0,
                       IDSHELP,
                       CCHMAXSTRING,
                       (PSZ)szGeneral );
        WinMessageBox( HWND_DESKTOP,
                       hwndFrame,
                       (PSZ)szGeneral,
                       (PSZ)szWindowTitle,
                       (ULONG)NULL,
                        MB_OK | MB_CUANOTIFICATION |
                        MB_APPLMODAL );
        break;

    default:
        return ( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
    }
  return(MRFROMLONG(NULL));
}

/***********************************************************************/
/* この関数によりウィンドウ・デバイス・コンテキストが得られる。        */
/* さらにNLS32表示空間を定義して両者の関連づけを行う。                 */
/* 前景色および背景色をセットし，内部値の初期化も行う。                */
/***********************************************************************/
VOID NlssampCreate(hwnd)
HWND hwnd;
{
    FONTMETRICS FontBuf;
    SIZEL  sizl;
    LONG  WindowTextColor;
    LONG  WindowColor;

    sizl.cx = 0L;
    sizl.cy = 0L;

    hdcNls = WinOpenWindowDC( hwnd );
    hpsNls = GpiCreatePS( hab,
                          hdcNls,
                          (PSIZEL)&sizl,
                          (ULONG)PU_PELS | GPIT_MICRO | GPIA_ASSOC
                        );

    GpiQueryFontMetrics( hpsNls,
                         (LONG)sizeof(FONTMETRICS),
                         (PFONTMETRICS)&FontBuf );

    CharHeight = (FontBuf.lMaxBaselineExt);
    Linebuf[Curpos] = '\0';

    WindowColor = WinQuerySysColor( HWND_DESKTOP,
                                    SYSCLR_WINDOW,
                                    (ULONG)NULL );

    WindowTextColor = WinQuerySysColor( HWND_DESKTOP,
                                        SYSCLR_WINDOWTEXT,
                                        (ULONG)NULL );
    cb.usBackMixMode = BM_OVERPAINT;
    cb.lBackColor = GpiQueryColorIndex( hpsNls,
                                        (ULONG)NULL,
                                        WindowColor );
    cb.lColor = GpiQueryColorIndex( hpsNls,
                                    (ULONG)NULL,
                                    WindowTextColor );

    GpiSetAttrs( hpsNls,
                 PRIM_CHAR,
                 CBB_COLOR | CBB_BACK_COLOR | CBB_BACK_MIX_MODE,
                 0L,
                 (PBUNDLE)&cb );
}

/***********************************************************************/
/* この関数はWM_COMMANDメッセージの全コマンドを処理                    */
/***********************************************************************/
VOID NlssampCommand( hwnd, mp )
HWND   hwnd;
MPARAM mp;
{
    FONTMETRICS FontBuf;
    FONTDLG     fdFontdlg;
    SIZEF   sizef;
    CHAR    szBootDrive;
    ULONG   ulBootDriveLen;

    switch (LOUSHORT( mp )) {
    case IDCMD_ACTIVATE:
        /********************************************************************/
        /* WinDBCSIMEcontrolを呼び出すことによりIMEを活動化/非活動化する。  */
        /********************************************************************/
        fIsActivated = !fIsActivated;
        if (fIsActivated)
        {
            IMEmode.fIMEMode &= ~DBE_IMCTL_NOTIMEMODE;
            IMEmode.fIMEMode |= DBE_IMCTL_IMEMODE;
        }
        else
        {
            IMEmode.fIMEMode &= ~DBE_IMCTL_IMEMODE;
            IMEmode.fIMEMode |= DBE_IMCTL_NOTIMEMODE;
        }
        WinDBCSIMEControl( hab, hwndFrame, DBE_IMCTL_SET,
                           (PIMEMODE)&IMEmode );
        break;
    case IDCMD_ENABLE:
        /********************************************************************/
        /* WinDBCSIMEcontrolを呼び出すことによりIMEを使用可/不可状態にする。*/
        /********************************************************************/
        fIsEnabled = !fIsEnabled;
        if (fIsEnabled)
        {
            IMEmode.fIMEMode &= ~DBE_IMCTL_IMEDISABLE;
            IMEmode.fIMEMode |= DBE_IMCTL_IMEENABLE;
        }
        else
        {
            IMEmode.fIMEMode &= ~DBE_IMCTL_IMEENABLE;
            IMEmode.fIMEMode |= DBE_IMCTL_IMEDISABLE;
        }
        WinDBCSIMEControl( hab, hwndFrame, DBE_IMCTL_SET,
                           (PIMEMODE)&IMEmode );
        break;
    case IDCMD_ONTHESPOT:
        fIsOntheSpot = !fIsOntheSpot;
        break;
    case IDCMD_INLINE:
        /********************************************************************/
        /* PECICウィンドウ･プロシージャーをサブクラス化/復元する            */
        /* (結果: MyPecicWndProc)                                           */
        /********************************************************************/
        fIsInline = !fIsInline;
        if (fIsInline)
        {
            pfnWndProc = WinSubclassWindow(
                               WinWindowFromID( hwndFrame, FID_DBE_PECIC ),
                               MyPecicWndProc );
            szlastText[0] = '\0';
        }
        else
        {
            WinSubclassWindow( WinWindowFromID( hwndFrame, FID_DBE_PECIC ),
                               pfnWndProc );
        }
        break;
    case IDCMD_IMENAME:
        /********************************************************************/
        /* WinMessageBox関数によりIMEモジュール名を表示する                 */
        /********************************************************************/
        WinDBCSIMEControl( hab, hwndFrame, DBE_IMCTL_QUERY,
                           (PIMEMODE)&IMEmode );
        DosQueryModuleName( IMEmode.hModIME, CCHMAXSTRING, szIMEName );

        WinMessageBox( HWND_DESKTOP,
                       hwnd,
                       (PSZ)szIMEName,
                       (PSZ)szWindowTitle,
                       0,
                       MB_CUANOTIFICATION | MB_OK );
        break;
    case IDCMD_ALPHA:
    case IDCMD_KATAKANA:
    case IDCMD_HIRAGANA:
    case IDCMD_SBCSDBCS:
    case IDCMD_ROMAN:
        /********************************************************************/
        /* WinDBCSModeControl 関数によりキーボード状況域を変更する。        */
        /********************************************************************/
        NlssampModeControl( mp );
        break;
    case IDCMD_APPLSTAT:
        /********************************************************************/
        /* 適用業務状況域の文字列を表示する                                 */
        /********************************************************************/
        fIsApplstat = !fIsApplstat;
        if (fIsApplstat) {
            Wpmctl.textlength = sizeof(Appltext);
            WinSendMsg( WinWindowFromID( hwndFrame, FID_DBE_APPSTAT ),
                        WM_SETWINDOWPARAMS,
                        (MPARAM)&Applparam,
                        (MPARAM)NULL );
        } else {
            Wpmctl.textlength = 0;
            WinSendMsg( WinWindowFromID( hwndFrame, FID_DBE_APPSTAT ),
                        WM_SETWINDOWPARAMS,
                        (MPARAM)&Applparam,
                        (MPARAM)NULL );
        }
        break;
    case IDCMD_PRIORITY:
        /********************************************************************/
        /* 適用業務状況制御ウィンドウでの可視優先度を変更する               */
        /********************************************************************/
        switch ( ++fPriority % 4 ) {
        case 0:
           Priority = DBE_AS_APPLBOTTOM;
           break;
        case 1:
           Priority = DBE_AS_PECICTOP;
           break;
        case 2:
           Priority = DBE_AS_KBDTOP;
           break;
        case 3:
           Priority = DBE_AS_APPLTOP;
           break;
        }
        WinSendMsg( WinWindowFromID( hwndFrame, FID_DBE_APPSTAT ),
                    WM_DBE_SETAPPLSTAT,
                    (MPARAM)&Visibility,
                    (MPARAM)NULL );

        break;
    case IDCMD_SELECTFONT:
        /********************************************************************/
        /* '使用可能なフォント' 項目が選択された。                          */
        /* フォント選択の処理は，ダイアログ・プロシージャー                 */
        /* Nls32DlglistFonts が行う。                                       */
        /********************************************************************/
//      WinDlgBox( HWND_DESKTOP,
//                 hwndFrame,
//                 (PFNWP)NlssampDlgListFonts,
//                 0,
//                 IDD_FONTLIST,
//                 (PCH)NULL );
        {

        HWND   hwndFontDlg;
        char   szFamilyname[FACESIZE];

        memset( &fdFontdlg, 0x00, sizeof(fdFontdlg) );
        szFamilyname[0]          = 0x00;
        fdFontdlg.cbSize         = sizeof(fdFontdlg);
        fdFontdlg.hpsScreen      = hpsNls;
        fdFontdlg.pszFamilyname  = szFamilyname;
        fdFontdlg.usFamilyBufLen = sizeof(szFamilyname);
        hwndFontDlg = WinFontDlg( HWND_DESKTOP, hwndFrame, &fdFontdlg );
        if( !hwndFontDlg || ( fdFontdlg.lReturn != DID_OK ) ) {
        }
        memcpy( &SelectedFont, &fdFontdlg.fAttrs, sizeof(SelectedFont) );

        GpiSetCharSet( hpsNls, 0L );
        GpiSetCharMode( hpsNls, CM_MODE1 );
        GpiDeleteSetId( hpsNls, ID_LCID );

        GpiCreateLogFont( hpsNls,
                           (PSTR8)szClassName,
                           ID_LCID,
                           &SelectedFont );
        GpiSetCharSet( hpsNls, ID_LCID );

        GpiQueryFontMetrics( hpsNls,
                             (LONG)sizeof(FONTMETRICS),
                             (PFONTMETRICS)&FontBuf );
        CharHeight = (FontBuf.lMaxBaselineExt);
        WinInvalidateRect( hwnd, (PRECTL)NULL, TRUE );

        if (FontBuf.fsDefn & OUTLINE_FONT)
        {
            GpiSetCharMode(hpsNls,CM_MODE2);
            GpiQueryCharBox( hpsNls, &sizef );
            sizef.cx = sizef.cy;
            GpiSetCharBox ( hpsNls, &sizef );
        }

        }
        break;
    case IDCMD_LOADFD:
        /********************************************************************/
        /* 任意選択のフォント・ドライバー (PMNLSFD2.FDR) をロードする       */
        /********************************************************************/
        WinLoadString( hab, 0, IDSDEFAULTFD, CCHMAXSTRING,
                       (PSZ)szFontDriver );

        WinLoadString( hab, 0, IDSPORTD,    CCHMAXSTRING, (PSZ)szPortd );
        WinLoadString( hab, 0, IDSPORTDKEY, CCHMAXSTRING, (PSZ)szPortdkey );

        if( PrfQueryProfileData( INI_PORTD,
                                 szPortd,
                                 szPortdkey,
                                 &szBootDrive,
                                 &ulBootDriveLen ) )
        {
            szFontDriver[0] = szBootDrive;
        }

        if( !WinDBCSQueryFDDescription( hab, szFontDriver, &FDdesc ) )
        {
            WinLoadString( hab, 0, IDSNONEFDR, CCHMAXSTRING,
                           (PSZ)szGeneral );

            WinMessageBox( HWND_DESKTOP,
                           hwndFrame,
                           (PSZ)szGeneral,
                           (PSZ)szWindowTitle,
                           0,
                           MB_CUAWARNING | MB_OK );
            break;
        }

        if ( !WinDBCSLoadFontDriver( hab, szFontDriver ) )
        {
            WinLoadString( hab, 0, IDSERRORFD, CCHMAXSTRING,
                           (PSZ)szGeneral );

            WinMessageBox( HWND_DESKTOP,
                           hwndFrame,
                           (PSZ)szGeneral,
                           (PSZ)szWindowTitle,
                           0,
                           MB_CUAWARNING | MB_OK );
            break;
        }

        WinDBCSQueryFDDescription( hab, szFontDriver, &FDdesc );
        WinMessageBox( HWND_DESKTOP,
                       hwnd,
                       (PSZ)FDdesc.str64Desc,
                       (PSZ)szWindowTitle,
                       0,
                       MB_CUANOTIFICATION | MB_OK );
        break;
    case IDCMD_UNLOAD:
        /********************************************************************/
        /* 任意選択のフォント・ドライバー (PMNLSFD2.FDR) をアンロードする。 */
        /********************************************************************/
        GpiSetCharSet( hpsNls, 0L );
        GpiSetCharMode( hpsNls, CM_MODE1 );
        GpiDeleteSetId( hpsNls, ID_LCID );

        WinDBCSUnloadFontDriver( hab, szFontDriver );
        szFontDriver[0] = 0;
        SelectedFont.usRecordLength = 0;

        GpiQueryFontMetrics( hpsNls,
                             (LONG)sizeof(FONTMETRICS),
                             (PFONTMETRICS)&FontBuf );

        CharHeight = (FontBuf.lMaxBaselineExt);
        WinInvalidateRect( hwnd, (PRECTL)NULL, TRUE );
        break;
    case IDCMD_WORDWRAP:
        fIsWordwrap = !fIsWordwrap;
        break;
    case IDCMD_HELPEXT:
    case IDCMD_HELPIND:
    case IDCMD_HELPKEY:
        /********************************************************************/
        /* 'ヘルプ'メニュー項目のいずれかが選択された場合，                 */
        /* IDSHELP の文字列をメッセージ・ボックス内に表示する。             */
        /********************************************************************/
        WinLoadString( hab,
                       0,
                       IDSHELP,
                       CCHMAXSTRING,
                       (PSZ)szGeneral );
        WinMessageBox( HWND_DESKTOP,
                       hwndFrame,
                       (PSZ)szGeneral,
                       (PSZ)szWindowTitle,
                       (ULONG)0,
                       MB_OK | MB_CUANOTIFICATION |
                       MB_APPLMODAL );
        break;
    case IDCMD_EXIT:
        /********************************************************************/
        /* '終了' メニュー項目が選択された場合，IDSEND文字列内のテキストを  */
        /* 表示して選択の確認を行う。その後WM_QUITメッセージが通知される。  */
        /********************************************************************/
        WinLoadString( hab,
                       0,
                       IDSEND,
                       CCHMAXSTRING,
                       (PSZ)szGeneral );
        if( WinMessageBox( HWND_DESKTOP,
                           hwndFrame,
                           (PSZ)szGeneral,
                           (PSZ)szWindowTitle,
                           (ULONG)0,
                           MB_YESNO | MB_CUAWARNING |
                           MB_APPLMODAL ) == MBID_YES )
              WinPostMsg( hwnd, WM_QUIT, MPFROMSHORT(0), MPFROMSHORT(0) );
        break;
    default:
        break;
    }
}

/*************************************************************************/
/* pecic-ウィンドウ・サブクラスの目的は，適用業務自身が PECIC ウィンドウ */
/* に文字列を表示することにある。WM_PAINT と WM_SETWNDPARAMS メッセージが*/
/* 登録される。                                                          */
/*************************************************************************/
MRESULT EXPENTRY MyPecicWndProc( hwnd, msg, mp1, mp2 )
HWND   hwnd;
ULONG  msg;
MPARAM mp1;
MPARAM mp2;
{
    PPECICDATA pPecic;

    switch (msg)
    {
    case WM_SETWINDOWPARAMS:
        pPecic = ((PWNDPARAMS)mp1)->pCtlData;
        if ( pPecic->wpmctlflag != DBE_KSF_TEXT )
            break;

        if (pPecic->textlength < CCHMAXSTRING)
        {
            szPecicText[0] = '\0';
            strncpy( szPecicText, pPecic->pTextString,
                     pPecic->textlength );
        }
        else
        {
            strncpy( szPecicText, pPecic->pTextString,
                     CCHMAXSTRING-1 );
            szPecicText[CCHMAXSTRING-1] = '\0';
        }

        PecicPaint( );

        return(MRFROMLONG(TRUE));
        break;
    default:
        break;
    }
    return ( (*pfnWndProc)( hwnd, msg, mp1, mp2 ) );
}

/************************************************************************/
/* この関数により PECIC 領域に'赤'の背景で'黒'の文字列が描かれる。      */
/************************************************************************/
VOID PecicPaint( VOID )
{
    RECTL  rect;
    POINTL points[TXTBOX_COUNT];
    POINTL pt;

    pt.x = (ULONG)CursorPos.x;
    pt.y = (ULONG)Origin - CursorPos.y;

    GpiQueryTextBox( hpsNls,
                     (LONG)strlen( szlastText ),
                     szlastText,
                     TXTBOX_COUNT,
                     points );
    rect.xLeft = pt.x;
    rect.yBottom = pt.y;
    rect.xRight = pt.x + points[TXTBOX_TOPRIGHT].x + 1;
    rect.yTop = pt.y + points[TXTBOX_TOPRIGHT].y + 1;

    WinFillRect( hpsNls, &rect, CLR_BACKGROUND );

    if ( strlen( szPecicText ) )
    {
        GpiSetColor( hpsNls, CLR_BLACK );
        GpiSetBackColor( hpsNls, CLR_RED );
        GpiSetBackMix( hpsNls, BM_OVERPAINT );

        GpiCharStringAt( hpsNls, &pt,
                         (LONG)strlen(szPecicText), szPecicText );
        strcpy( szlastText, szPecicText );

        GpiSetAttrs( hpsNls,
                     PRIM_CHAR,
                     CBB_COLOR | CBB_BACK_COLOR | CBB_BACK_MIX_MODE,
                     0L,
                     (PBUNDLE)&cb );
    }
}

/***********************************************************************/
/* この関数は, 現在の状況を取り出し，WinDBCSModeControlを使って        */
/* メニュー操作で設定された新しい状態に変更する。                      */
/***********************************************************************/
VOID NlssampModeControl( mp )
MPARAM mp;
{
    WinDBCSModeControl( hab, hwndFrame, DBE_MCTL_QUERY,
                        DBE_MCTL_JAPANREQ, &fDBCSmode );

    switch (LOUSHORT(mp))
    {
        case IDCMD_ALPHA:
           fDBCSmode &= ~( DBE_MCTL_KATAKANA | DBE_MCTL_HIRAGANA );
           fDBCSmode |= DBE_MCTL_ALPHANUMERIC;
           break;
        case IDCMD_KATAKANA:
           fDBCSmode &= ~( DBE_MCTL_ALPHANUMERIC | DBE_MCTL_HIRAGANA );
           fDBCSmode |= DBE_MCTL_KATAKANA;
           break;
        case IDCMD_HIRAGANA:
           fDBCSmode &= ~( DBE_MCTL_ALPHANUMERIC | DBE_MCTL_KATAKANA );
           fDBCSmode |= DBE_MCTL_HIRAGANA;
           break;
        case IDCMD_SBCSDBCS:
           if ( fDBCSmode & DBE_MCTL_SBCSCHAR )
           {
               fDBCSmode &= ~DBE_MCTL_SBCSCHAR;
               fDBCSmode |= DBE_MCTL_DBCSCHAR;
           }
           else
           {
               fDBCSmode &= ~DBE_MCTL_DBCSCHAR;
               fDBCSmode |= DBE_MCTL_SBCSCHAR;
           }
           break;
        case IDCMD_ROMAN:
           if ( fDBCSmode & DBE_MCTL_ROMAN )
           {
               fDBCSmode &= ~DBE_MCTL_ROMAN;
               fDBCSmode |= DBE_MCTL_NOROMAN;
           }
           else
           {
               fDBCSmode &= ~DBE_MCTL_NOROMAN;
               fDBCSmode |= DBE_MCTL_ROMAN;
           }
           break;
        default:
           break;
    }

    WinDBCSModeControl( hab, hwndFrame, DBE_MCTL_SET,
                        DBE_MCTL_JAPANREQ, &fDBCSmode );
}

/***********************************************************************/
/* この関数は入力文字の処理を行う。仮想キーの場合，Escキーか後退キー   */
/* またはスペース･キーであるかどうかをチェックし，それ以外は無視する。 */
/***********************************************************************/
BOOL NlssampCharInput( hwnd, mp1, mp2)
HWND hwnd;
MPARAM mp1;
MPARAM mp2;
{
    POINTL pt;
    POINTL points[TXTBOX_COUNT];

    if (LONGFROMMP(mp1) & KC_VIRTUALKEY)
    {
        if ((CHAR3FROMMP(mp2) == VK_BACKSPACE)
                && (Curpos > 0))
        {
            Curpos = WinPrevChar( hab, codepage, 0,
                                  Linebuf, Linebuf+Curpos )
                     - (PSZ)Linebuf;

            Linebuf[Curpos] = '\0';

            if ( Curpos < clStart )
            {
                WinInvalidateRect( hwnd, (PRECTL)0, TRUE );
            }
            GpiQueryTextBox( hpsNls,
                             (LONG)Curpos-clStart,
                             Linebuf+clStart,
                             TXTBOX_COUNT,
                             points );
            CursorPos.x = (ULONG)points[TXTBOX_CONCAT].x;

            pt.x = (ULONG)CursorPos.x;
            pt.y = (ULONG)Origin - CursorPos.y;
            GpiCharStringAt( hpsNls,
                             (PPOINTL)&pt,
                             5L,
                             "     " );
        }
        else if (CHAR3FROMMP(mp2) == VK_SPACE)
        {
            NlssampDisplayChar( hwnd, ' ' );
        }
        else if (CHAR3FROMMP(mp2) == VK_ESC)
        {
            Curpos = 0;
            clStart = 0;
            CursorPos.x = 0;
            CursorPos.y = 0;
            WinInvalidateRect( hwnd, (PRECTL)NULL, TRUE );
        }
        else
        {
            return(FALSE);
        }
    }
    else if (LONGFROMMP(mp1) & KC_CHAR)
    {
         NlssampDisplayChar( hwnd, SHORT1FROMMP(mp2) );
    }
    else
    {
         return(FALSE);
     }

    return(TRUE);
}

/***********************************************************************/
/* この関数は SBCS/DBCS 文字を表示する。有効な文字であり BUFFERLENGTH  */
/* の限界を越えていなければ, その文字を表示する。                      */
/***********************************************************************/
VOID NlssampDisplayChar( hwnd, ch )
HWND hwnd;
ULONG  ch;
{
    POINTL points[TXTBOX_COUNT];
    ULONG  i;
    RECTL  rect;

    if( HIUCHAR(  LOUSHORT(LONGFROMMP(ch))  )  != 0         )
        i = DBCSCHAR;
    else
        i = SBCSCHAR;

    if (Curpos < BUFFERLENGTH - i)
    {
        if ( Curpos-clStart+i > LINELENGTH )
        {
            CursorPos.x = 0;
            CursorPos.y += CharHeight;
            clStart = Curpos;
        }

        switch( i ) {
          case SBCSCHAR:
            Linebuf[Curpos++] = (char)( ch );
            Linebuf[Curpos] = '\0';
            break;
          case DBCSCHAR:
            Linebuf[Curpos++] = (char)( ch );
            Linebuf[Curpos++] = HIUCHAR( ch );
            Linebuf[Curpos] = '\0';
            break;
        }

        if (fIsWordwrap)
        {
            WinInvalidateRect( hwnd, (PRECTL)NULL, TRUE );
        }
        else
        {
            rect.xLeft = CursorPos.x;
            rect.yTop = (ULONG)Origin - CursorPos.y + CharHeight;
            rect.xRight = rect.xLeft + 100;
            rect.yBottom = rect.yTop - CharHeight;

            WinDrawText( hpsNls, i, (PSZ)&ch, &rect, 0L, 0L, DT_TEXTATTRS );
        }

        GpiQueryTextBox( hpsNls,
                         (LONG)Curpos-clStart,
                         Linebuf+clStart,
                         TXTBOX_COUNT,
                         points );

        CursorPos.x = (ULONG)points[TXTBOX_CONCAT].x;
    }
    else
    {
        WinAlarm(HWND_DESKTOP, WA_WARNING);
    }
}

/***************************************************************************/
/* NlssampDlgListFonts は '使用可能なフォント' 項目の処理を行うダイアログ･ */
/* プロシージャーである。                                                  */
/***************************************************************************/
MRESULT EXPENTRY NlssampDlgListFonts( hwndDlg, msg, mp1, mp2 )
HWND   hwndDlg;
ULONG  msg;
MPARAM mp1;
MPARAM mp2;
{
    switch(msg)
    {
        case WM_INITDLG:
            return(MRESULT)(NlssampInitListDlg(hwndDlg));
        case WM_COMMAND:
            switch (LOUSHORT(mp1))
            {
                 case DID_OK:
                     NlssampEndListDlg( hwndDlg );
                     WinDismissDlg( hwndDlg, TRUE );
                     WinInvalidateRect( hwndFrame, 0, TRUE );
                 break;
                 case DID_CANCEL:
                     return(MRESULT)( WinDismissDlg(hwndDlg, TRUE) );
            }
        case WM_CONTROL:
            switch (HIUSHORT(mp1))
            {
                 case LN_ENTER:
                     NlssampEndListDlg( hwndDlg );
                     WinDismissDlg( hwndDlg, TRUE );
                     WinInvalidateRect( hwndFrame, NULL, TRUE );
                 break;
            }
        break;
        case WM_HELP:
            WinMessageBox( HWND_DESKTOP,
                           hwndDlg,
                           (PSZ)szDlgHelp,
                           (PSZ)szWindowTitle,
                           0,
                           MB_CUANOTIFICATION | MB_ENTER );
        break;
        default:
            return( WinDefDlgProc(hwndDlg, msg, mp1, mp2) );
    }
  return(MRFROMLONG(NULL));
}

/***********************************************************************/
/* NlssampInitListDlg は '使用可能なフォント'が選ばれたときに          */
/* リスト・ボックスの初期化を行う。                                    */
/***********************************************************************/
BOOL NlssampInitListDlg(hwndDlg)
HWND hwndDlg;
{
    LONG         i;
    LONG         NoOfFontsReq = 0;
    LONG         ActualNoOfFonts;
    FONTMETRICS  *npMetrics;
    CHAR         Buffer[20];
    CHAR         *psz;
    CHAR         szTempText[CCHMAXSTRING];
    APIRET       rcAlloc;

    ActualNoOfFonts = GpiQueryFonts( hpsNls,
                                     QF_PUBLIC | QF_PRIVATE,
                                     (PSZ)NULL,
                                     &NoOfFontsReq,
                                     (LONG)0,
                                     (PFONTMETRICS)0 );

    sizereq = (ULONG)sizeof(FONTMETRICS) * (ULONG)ActualNoOfFonts;

    rcAlloc = DosAllocMem( (PPVOID)&npMem,
                           sizereq,
                           PAG_COMMIT | PAG_READ | PAG_WRITE );
    if( rcAlloc ) return( FALSE );

    npMetrics = (FONTMETRICS *)npMem;

    GpiQueryFonts( hpsNls,
                   QF_PUBLIC | QF_PRIVATE,
                   (PSZ)0,
                   &ActualNoOfFonts,
                   (LONG)sizeof(FONTMETRICS),
                   npMetrics );

    for (i = 0; i < ActualNoOfFonts; i++)
    {
        strcpy(szTempText, npMetrics->szFacename);

        if (!(npMetrics->fsDefn & OUTLINE_FONT))

        {
            psz = itoa ( (LONG)npMetrics -> lAveCharWidth,
                         Buffer,
                         RADIX );

            strcat( szTempText, " - " );
            strcat( szTempText, psz );
            strcat( szTempText, " X " );

            psz = itoa ( (LONG)npMetrics -> lMaxBaselineExt,
                         Buffer,
                         RADIX );

            strcat( szTempText, psz );

        }

        WinSendDlgItemMsg( hwndDlg,
                           IDDLISTBOX,
                           LM_INSERTITEM,
                           (MPARAM)LIT_END,
                           (MPARAM)(PSZ)szTempText );
        npMetrics++;
    }

    WinSendDlgItemMsg( hwndDlg,
                       IDDLISTBOX,
                       LM_SELECTITEM,
                       (MPARAM)lastSelect,
                       (MPARAM)TRUE );

    WinSendDlgItemMsg( hwndDlg,
                       IDDLISTBOX,
                       LM_SETTOPINDEX,
                       (MPARAM)lastSelect,
                       (MPARAM)NULL );

    return( FALSE );
}

/***********************************************************************/
/* NlssampEndListDlg は, 使用可能なフォントの一部が選ばれたときに      */
/* 呼び出され, そのフォントに関するフォント・メトリックス情報を        */
/* 保管する。                                                          */
/***********************************************************************/
VOID NlssampEndListDlg(hwndDlg)
HWND hwndDlg;{
    FONTMETRICS   *npMetrics;

    lastSelect = (LONG)WinSendDlgItemMsg( hwndDlg,
                                           IDDLISTBOX,
                                           LM_QUERYSELECTION,
                                           (MPARAM)0,                                          (MPARAM)NULL );

    npMetrics = (FONTMETRICS  *)npMem;
    npMetrics = npMetrics + lastSelect;

    SelectedFont.usRecordLength  = sizeof(FATTRS);
    SelectedFont.fsSelection     = npMetrics->fsSelection;
    SelectedFont.lMatch          = npMetrics->lMatch;
    strcpy(SelectedFont.szFacename, npMetrics->szFacename);
    SelectedFont.idRegistry      = npMetrics->idRegistry;
    SelectedFont.usCodePage      = npMetrics->usCodePage;
    SelectedFont.lMaxBaselineExt = npMetrics->lMaxBaselineExt;
    SelectedFont.lAveCharWidth   = npMetrics->lAveCharWidth;
    SelectedFont.fsType          = 0;
    SelectedFont.fsFontUse       = FATTR_FONTUSE_NOMIX;
}

