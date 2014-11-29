/***************************************************************************/
/* XPRMMAIN.C    XPG4 Primer for OS/2 WARP - V1.0       11/15/95           */
/*               main function, main window procedure and common routines. */
/***************************************************************************/
#define INCL_WIN
#define INCL_DOS
#define INCL_GPI
#define INCL_WINERRORS
#include <os2.h>                       /* System Include File              */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <nl_types.h>
#include <langinfo.h>
#include <time.h>
#include <monetary.h>
#include <ctype.h>
#include <limits.h>

#include "xprmos2.h"                   /* Application Include File         */
#include "xprmdata.h"                  /* Application Data Structure file  */
#include "xprmres.h"                   /* MRI resource definitions         */
#include "xprminfo.h"                  /* About dialog resource definitions*/
#include "xprmhelp.h"                  /* ipf resource definition          */

/**************************************/
/* function prototypes - global       */
/**************************************/
void putWindowToCenter( HWND hwnd );
/* function to set the wide string to PM controls */
void setDlgItemWcs( HWND hwnd, ULONG ulItemID, wchar_t* wcsText);
size_t queryDlgItemWcs( HWND hwnd, ULONG ulItemId, wchar_t* wcsBuf , size_t szLen );
char *stringLower( char *pszString );
BOOL isDigitString( char *pszString );
void logError( ulong ulCode, MPARAM mp );
void associateHelp( HWND hwnd );

/**************************************/
/* function prototypes - static       */
/**************************************/
static MRESULT EXPENTRY mainWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
static MRESULT EXPENTRY logInDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
static MRESULT EXPENTRY aboutDlgProc(HWND hwnd,ULONG msg, MPARAM mp1, MPARAM mp2);
static void queryDevCapabilities( HWND hwndFrame );

/**************************************/
/* function prototypes - external     */
/**************************************/
/* PM window/dialog procedures */
extern MRESULT EXPENTRY viewOrderWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT EXPENTRY viewSubWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT EXPENTRY queryOrderDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT EXPENTRY orderEntryDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT EXPENTRY custInfoDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT EXPENTRY queryCustDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
/* file I/O */
extern char* readProdFile( void );
extern char* readCustFile( void );
extern ORDERREC* getOrderRec( char *pszKey, enum orderitem e_category );

/**************************************/
/* Global Variables                   */
/**************************************/
HAB hab;
HMODULE hModRsrc;
HWND hwndHelpInstance;
char acMsgTable[MAX_MSG_NUM][MAX_MSG_LEN];

/* Locale information */
BOOL flProdLocale, flCustLocale;       /* if true, use national language   */
BOOL flDBCScapability;          /* if true, the environment is DBCS capable*/

/* device information */
long lDevCaps[4];

/**************************************/
/* Static Variables                   */
/**************************************/
static const char* pszLogFile = "xprmos2.log";
/*static char* pszHelpName = "xprmres.hlp"; */
static wchar_t* wcsHelpExtension = L".hlp";
static char* pszResName = "\\xprmres.dll";
static char* pszDefRes = "\\en_us\\xprmres.dll";
static char pszEmpNumber[8];           /* Employee number                  */
static char *prodLocale, *custLocale, *curLocale;     /* locale information*/
static HBITMAP hbm = NULLHANDLE;       /* bitmap handle                    */

/*****************************************************************************/
/* main()                                                                    */
/*      Sets the process locale, loads data files and MRI dll, and loads     */
/*      message string table.  Then create the main standard window and      */
/*      start dispatching the PM messages.                                   */
/*****************************************************************************/
main(int argc, char* argv[], char* envp[])
{
int i;
QMSG qmsg;
HMQ hmq;
ULONG flCreateMain;
HWND hwndMain;
HWND hwndClMain;
char acCodeSet[16];
char acResName[256];
BOOL loadResource( char* pszResFile );
void initHelp( char* pszResFile );
BOOL loadFiles( void );
BOOL queryEnvironment( void );

    curLocale = setlocale( LC_ALL, "" );
    if( curLocale == NULL )  curLocale = setlocale( LC_ALL, "C" );

    /* load resource module */
    if( loadResource( acResName ) == FALSE ) return -1;

    /* load data files */
    if( loadFiles() == FALSE )  return -1;
    if( flProdLocale==FALSE )   setlocale( LC_MONETARY, prodLocale );

    /* initialize PM */
    hab = WinInitialize((USHORT) NULL);
    hmq = WinCreateMsgQueue(hab,0);

    for( i=0; i<MAX_MSG_NUM; i++ )                     /* load string table*/
    {
        if( WinLoadString( hab, hModRsrc, (ULONG)i,
                           MAX_MSG_LEN, acMsgTable[i] ) == 0 )  break;
    }

    /* initialize the help facility */
    initHelp( acResName );

    flDBCScapability = queryEnvironment();

    /* Register standard classes */
    WinRegisterClass( hab, "clXPRMMainWindow", (PFNWP) mainWinProc, CS_SIZEREDRAW, 0);
    WinRegisterClass( hab, "clXPRMViewOrderWindow", (PFNWP) viewOrderWinProc, CS_SIZEREDRAW, sizeof(PVOID) );
    WinRegisterClass( hab, "clXPRMViewSubWindow", (PFNWP) viewSubWinProc, CS_SIZEREDRAW, sizeof(PVOID) );

    /* Create the main standard window */
    flCreateMain= FCF_MAXBUTTON | FCF_MENU | FCF_MINBUTTON | FCF_SIZEBORDER
                | FCF_SYSMENU | FCF_TITLEBAR | FCF_TASKLIST | FCF_ACCELTABLE;/* 110995*/
    hwndMain = WinCreateStdWindow( HWND_DESKTOP,
                                   WS_VISIBLE,
                                   &flCreateMain,
                                   "clXPRMMainWindow",
                                   acMsgTable[IDS_MAIN_TITLE],
                                   0L,
                                   hModRsrc,
                                   WID_MAIN,
                                   (PHWND) & hwndClMain);
    /* associate help instance */
    associateHelp( hwndMain );

    while( WinGetMsg(hab,(PQMSG) &qmsg, (HWND) NULL, 0, 0) )
        WinDispatchMsg( hab,(PQMSG) &qmsg );

    WinDestroyWindow( hwndMain );
    if( hwndHelpInstance != NULLHANDLE )
       WinDestroyHelpInstance( hwndHelpInstance );
    WinDestroyMsgQueue( hmq );
    WinTerminate( hab );

    return(0);
}

/*****************************************************************************/
/* loadResource()                                                            */
/*      Query the current disk & path name and generate the qualified file   */
/*      name of the resource file.  If loading the resource is failed, it    */
/*      loads the default resource.                                          */
/*****************************************************************************/
/* 110195 */
static char acDriveName[26] = {'A','B','C','D','E','F','G','H','I','J','K','L','M',
                               'N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };
static BOOL loadResource( char* pszResFile )
{
int i;
APIRET ret;
ULONG ulDriveNum=0;
ULONG ulDriveMap=0;
ULONG ulLength = 253;
char acCurDir[256];
char *pszTempLocale;                                              /* 110695*/

    /* Use language and territory only */
    pszTempLocale = malloc( strlen(curLocale)+1 );                /* 110695*/
    strcpy( pszTempLocale, curLocale );                           /* 110695*/
    pszTempLocale = strtok( pszTempLocale, "." );                 /* 110695*/

    /* query the current drive */
    DosQueryCurrentDisk( &ulDriveNum, &ulDriveMap );
    for( i=0; i<26; i++ )                                         /* 110195*/
    {
       if( (i+1) == ulDriveNum )   acCurDir[0] = acDriveName[i];  /* 110185*/
    }
    acCurDir[1] = ':';
    acCurDir[2] = '\\';

    DosQueryCurrentDir( 0L, &acCurDir[3], &ulLength );
    strcpy( pszResFile, acCurDir );
    strcat( pszResFile, "\\" );
    strcat( pszResFile, pszTempLocale );                          /* 110695*/
    strcat( pszResFile, pszResName );
    free( pszTempLocale );                                        /* 110695*/

    if( DosLoadModule( NULL, 0, pszResFile, &hModRsrc ) != 0 )
    {
       strcpy( pszResFile, acCurDir );
       strcat( pszResFile, pszDefRes );
       if( (ret=DosLoadModule( NULL, 0, pszResFile, &hModRsrc )) != 0 )
       {
          logError( ERR_LOAD_RESOURCE, MPFROMLONG(ret) );
          return FALSE;
       }
    }
    return TRUE;
}

/*****************************************************************************/
/* initHelp()                                                                */
/*      Initializes help facility and creates the help instance.             */
/*****************************************************************************/
static void initHelp( char* pszResFile )
{
HELPINIT hini;
wchar_t *wcsTemp, *wcp;
size_t szNumChar, szBufLen;

    /* strchr() may misinterpret either byte of a DBCS as an SBCS, */
    /* thus use wcschr() instead.                                  */

    szBufLen = strlen( pszResFile );
    szNumChar = mbstowcs( NULL, pszResFile, 0 );
    wcsTemp = (wchar_t *)malloc( (szNumChar+1)*sizeof(wchar_t) );
    mbstowcs( wcsTemp, pszResFile, szNumChar+1 );
    /* search the extension of the file name */
    if( (wcp=wcsrchr( wcsTemp, L'.' )) == NULL )  return;
    *wcp = L'\0';
    wcscat( wcsTemp, wcsHelpExtension );
    wcstombs( pszResFile, wcsTemp, szBufLen+1 );

    hini.cb = sizeof(HELPINIT);
    hini.ulReturnCode = 0L;

    hini.pszTutorialName = NULL;

    hini.phtHelpTable = (PHELPTABLE)(MAKELONG(HID_MAIN, 0xffff) );
    hini.hmodHelpTableModule = hModRsrc;
    hini.hmodAccelActionBarModule = NULLHANDLE;
    hini.idAccelTable = 0;
    hini.idActionBar = 0;
    hini.pszHelpWindowTitle = acMsgTable[IDS_HELP_TITLE];
    hini.pszHelpLibraryName = pszResFile;

    hwndHelpInstance = WinCreateHelpInstance( hab, &hini );

    if( hwndHelpInstance == NULLHANDLE || hini.ulReturnCode != 0 )
    {
       logError( ERR_CREATE_HELP, 0 );
    }

    free( wcsTemp );
    return;
}

/*****************************************************************************/
/* loadFiles()                                                               */
/*      Calls readProfile() and readCustFile() to laod data files.  Sets     */
/*      the flProdLocale and flCustLocale flag.                              */
/*      Returns TRUE if the files are loaded without error.  Otherwise,      */
/*      returns FALSE.
/*****************************************************************************/
static BOOL loadFiles( void )
{
char *p, *pszTempLocale;                                          /* 110795*/

    /* Copy the locale name so that the original string is not changed */
    pszTempLocale = malloc( strlen(curLocale)+1 );                /* 110795*/
    strcpy( pszTempLocale, curLocale );                           /* 110795*/

    prodLocale = readProdFile();
    if( prodLocale == NULL )  return FALSE;
    if( strstr( stringLower(prodLocale),
                stringLower(pszTempLocale) ) == NULL )  flProdLocale = FALSE;
    else                                                flProdLocale = TRUE;

    strcpy( pszTempLocale, curLocale );                           /* 110795*/
    custLocale = readCustFile();
    if( custLocale == NULL )  return FALSE;
    if( strstr( stringLower(custLocale),
                stringLower(pszTempLocale) ) == NULL )   flCustLocale = FALSE;
    else                                                 flCustLocale = TRUE;

    return TRUE;
}

/*****************************************************************************/
/* loadFiles()                                                               */
/*      Lower-cases the contents of a string pointed to by pszString,        */
/*      Returns the pszString.                                               */
/*****************************************************************************/
char* stringLower( char* pszString )
{
int i;
char ch;
size_t len;
char* pTemp;

    len = strlen( pszString );
    pTemp = (char*)malloc( len+1 );
    if( pTemp == NULL )   return pszString;

    for( i=0; i<len; i++ )
    {
       ch = *(pszString+i);
       *(pTemp+i) = tolower( (int)ch );/* due to towlower error, use tolower*/
    }
    strncpy( pszString, pTemp, len );
    free( pTemp );
    return pszString;
}

/*****************************************************************************/
/* mainWinProc()                                                             */
/*      Main window's window procedure.                                      */
/*****************************************************************************/
MRESULT EXPENTRY mainWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    HPS hps;                           /* Handle to Presentation Space     */
    RECTL rectl;                       /* Dimensions of window             */
    HPOINTER hptrOld, hptrWait, hptrCurrent;
    void drawBitMap( HPS hps, HWND hwnd );

    switch(msg)
    {
    case WM_COMMAND:

        switch (SHORT1FROMMP(mp1))
        {
            case MID_NEWORDER:                                    /* New...*/
                /* orderEntryDlgProc handles messages for dialog DID_ORDERENTRY after loading it */
                WinDlgBox( HWND_DESKTOP, hwnd,
                           (PFNWP) orderEntryDlgProc,
                           hModRsrc, DID_ORDERENTRY,
                           pszEmpNumber );
                break;

            case MID_FIND:                                       /* Find...*/
                /* queryOrderDlgProc handles messages for dialog DID_QRYORDR after loading it */
                WinDlgBox (HWND_DESKTOP, hwnd,
                           (PFNWP) queryOrderDlgProc,
                           hModRsrc, DID_QRYORDR,
                           NULL);
                break;

            case MID_NEWCUST:                                     /* New...*/
                /* custInfoDlgProc handles messages for dialog DID_CUSTINFO after loading it */
                WinDlgBox( HWND_DESKTOP, hwnd,
                           (PFNWP) custInfoDlgProc,
                           hModRsrc, DID_CUSTINFO,
                           NULL );
                break;

            case MID_FINDCUST:                                   /* Find...*/
                /* queryCustDlgProc handles messages for dialog DID_QRYCUST after loading it */
                WinDlgBox (HWND_DESKTOP, hwnd,
                           (PFNWP) queryCustDlgProc,
                           hModRsrc, DID_QRYCUST,
                           NULL );
                break;

            case MID_HELPABOUT:                            /* product info.*/
                WinDlgBox( HWND_DESKTOP,hwnd,
                           (PFNWP) aboutDlgProc,
                           NULLHANDLE, DID_ABOUT,      /* dialog is in .EXE*/
                           NULL );
                break;

            case MID_HELPFORHELP:                           /* general help*/
                if( hwndHelpInstance != NULLHANDLE )
                   WinSendMsg(hwndHelpInstance, HM_DISPLAY_HELP, 0L, 0L );
                break;

            case MID_HELPKEYS:                                 /* keys help*/
                if( hwndHelpInstance != NULLHANDLE )
                   WinSendMsg(hwndHelpInstance, HM_KEYS_HELP, 0L, 0L );
                break;

            case MID_HELPINDEX:                               /* help index*/
                if( hwndHelpInstance != NULLHANDLE )
                   WinSendMsg(hwndHelpInstance, HM_HELP_INDEX, 0L, 0L );
                break;

            case VID_QUITPROG:                                    /* 110995*/
                WinPostMsg( hwnd, WM_CLOSE, mp1, mp2 );           /* 110995*/
                break;                                            /* 110995*/

        }
        break;                                         /* end of WM_COMMAND*/

    case WM_CREATE:
    {
    /* size and position of the main window*/
    long lMainWinCx,lMainWinCy, lMainWinX, lMainWinY;

        queryDevCapabilities( WinQueryWindow( hwnd, QW_PARENT ) );

        lMainWinCx = (SHORT)lDevCaps[CHAR_X]*60;
        lMainWinCy = lDevCaps[CHAR_Y]*25;
        lMainWinX = (lDevCaps[DISP_X]-lMainWinCx)/2;
        lMainWinY = (lDevCaps[DISP_Y]-lMainWinCy)/2;
        if( WinDlgBox( HWND_DESKTOP, hwnd,
                       (PFNWP) logInDlgProc,
                       hModRsrc, DID_LOGIN, NULL) == DID_OK )
        {
             /* Set the window position */
             WinSetWindowPos( WinQueryWindow( hwnd, QW_PARENT ), HWND_BOTTOM,
                              lMainWinX, lMainWinY,
                              lMainWinCx,lMainWinCy,
                              SWP_MOVE | SWP_SIZE | SWP_ACTIVATE );
        }
        else  WinPostMsg( hwnd, WM_CLOSE, mp1, mp2 );
        break;
    }

    case WM_PAINT:

        hps = WinBeginPaint(hwnd, 0, NULL);
        GpiErase(hps);
        hptrOld = WinQueryPointer(HWND_DESKTOP);
        hptrWait = WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE);
        hptrCurrent = hptrWait;
        WinSetPointer(HWND_DESKTOP, hptrWait);
        drawBitMap( hps, hwnd );
        WinSetPointer(HWND_DESKTOP, hptrOld);
        WinEndPaint(hps);
        break;

    case WM_DESTROY:
        if( hbm != NULLHANDLE )  GpiDeleteBitmap(hbm);
        break;

    case WM_CLOSE:
        WinPostMsg( hwnd, WM_QUIT, NULL, NULL );
        break;

    case HM_QUERY_KEYS_HELP:
        return( MRFROMLONG(HELP_HELPKEYSPANEL) );

    default:
        return(WinDefWindowProc(hwnd,msg,mp1,mp2));
        break;
    }
    return(FALSE);
}

/** 111495 *******************************************************************/
/* aboutDlgProc()                                                            */
/*      Dialog procedure for product information dialog.                     */
/*****************************************************************************/
MRESULT EXPENTRY aboutDlgProc(HWND hwnd,ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
   case WM_COMMAND:
        switch( SHORT1FROMMP(mp1) )
           case DID_OK:
              WinDismissDlg( hwnd, DID_OK );
              break;
        break;

   case WM_INITDLG:

        /* Set title and copyright notice */
        WinSetDlgItemText( hwnd, FID_TITLEBAR, acMsgTable[IDS_ABOUT_TITLE] );
        WinSetDlgItemText( hwnd, AB_STX_COPY1, acMsgTable[IDS_ABOUT_COPY1] );
        WinSetDlgItemText( hwnd, AB_STX_COPY2, acMsgTable[IDS_ABOUT_COPY2] );
        WinSetDlgItemText( hwnd, AB_STX_COPY3, acMsgTable[IDS_ABOUT_COPY3] );
        WinSetDlgItemText( hwnd, DID_OK, acMsgTable[IDS_ABOUT_OK] );
        /* Put the window to the center of the desktop */
        putWindowToCenter( hwnd );
        break;

   default:
      return WinDefDlgProc(hwnd, msg, mp1, mp2);
   }
 return (MRESULT)NULL;
}

/**********************************************************************/
/* drawBitMap()
/*   Draws bitmap at client area of the main window.                  */
/**********************************************************************/
static void drawBitMap( HPS hps, HWND hwnd )
{
  RECTL   rect;
  static  LONG    lClientColor;

  if( hbm == NULLHANDLE )                                 /* the first path*/
  {
    hbm = GpiLoadBitmap( hps, hModRsrc, BID_MAIN, 0L, 0L );
  }
  WinQueryWindowRect( hwnd, &rect );
  WinDrawBitmap( hps, hbm, NULL, (PPOINTL)&rect, (LONG)CLR_NEUTRAL,
                (LONG)CLR_BACKGROUND, DBM_STRETCH );
  return;
}

/**********************************************************************/
/* logInDlgProc()                                                     */
/*   Login dialog's procedure.                                        */
/**********************************************************************/
MRESULT EXPENTRY logInDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

    switch(msg)
    {
        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1))
            {
                case DID_OK:
                    /* Read 6 characters from the entry field LI_EF_EMPNUM into variable */
                    WinQueryDlgItemText( hwnd, LI_EF_EMPNUM, 6, pszEmpNumber);
                    /* Check if the contents is not empty */
                    if( strlen( pszEmpNumber ) == 0 )
                    {
                       showMessageBox( hwnd, IDS_WARNING_3, IDS_WARNING_CAPTION, MB_WARNING );
                       clearField( hwnd, LI_EF_EMPNUM );
                       break;
                    }
                    /* Check if the contents consists from numeric only. */
                    if( isDigitString( pszEmpNumber ) == FALSE )
                    {
                       showMessageBox( hwnd, IDS_WARNING_4, IDS_WARNING_CAPTION, MB_WARNING );
                       clearField( hwnd, LI_EF_EMPNUM );
                       break;
                    }
                    WinDismissDlg( hwnd,DID_OK );
                    break;

                case DID_CANCEL:
                    WinDismissDlg( hwnd,DID_CANCEL );
                    break;
            }
            break;

        case WM_INITDLG:
            /* Associates help instance */
            associateHelp( hwnd );
            /* Put the window to the center of the desktop */
            putWindowToCenter( hwnd );
            /* Max no. of chars for LI_EF_EMPNUM is 6 */
            WinSendDlgItemMsg( hwnd, LI_EF_EMPNUM,
                               EM_SETTEXTLIMIT, MPFROMSHORT(6), 0L );
            WinSetFocus( HWND_DESKTOP, WinWindowFromID (hwnd, LI_EF_EMPNUM) );
            break;

        default:
            return( WinDefDlgProc(hwnd,msg,mp1,mp2) );
            break;
    }
return(FALSE);
}

/**********************************************************************/
/* queryEnvironment()                                                 */
/*   Checks if the current code set is DBCS-capable or not.  The      */
/*   information is stored in the string table of the resource file.  */
/*   Returns TRUE if the code set is for DBCS.  Otherwise, returns    */
/*   FALSE.                                                           */
/**********************************************************************/
static BOOL queryEnvironment()
{
int i;
char *p, *pszCodeSet;

    pszCodeSet = nl_langinfo( CODESET );
    for( i=0; i<NUM_OF_CODESET; i++ )
    {
       p=strtok( acMsgTable[i], "," );
       if( strcmp( pszCodeSet, p ) == 0 )
       {
         p=strtok( NULL, "," );
         return( strcmp( "DBCS", p )==0? TRUE: FALSE );
       }
    }
    return FALSE;
}

/**********************************************************************/
/* queryDevCapabilities()                                             */
/*   Queries desk top resolution and system default font's size in    */
/*   pels.  The informations are stored in lDevCaps[].                */
/**********************************************************************/
void queryDevCapabilities( HWND hwndFrame )
{
  HPS hps;
  FONTMETRICS fm;

  /* Query desktop window size in pel */
  lDevCaps[DISP_X] = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
  lDevCaps[DISP_Y] = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
  /* Query main window's font size */
  hps = WinGetPS(hwndFrame);
  GpiQueryFontMetrics(hps, (LONG)sizeof(fm), &fm);
  WinReleasePS(hps);
  lDevCaps[CHAR_X] = fm.lAveCharWidth;
  lDevCaps[CHAR_Y] = fm.lMaxBaselineExt+fm.lExternalLeading;
}

/**********************************************************************/
/* isDigitString()                                                    */
/*   Tests if the string pointed to by pszString consists from digit  */
/*   numbers only.  Returns TRUE if so, otherwise returns FALSE.      */
/**********************************************************************/
BOOL isDigitString( char *pszString )
{
 int i, ch;
 size_t len;

   len = strlen( pszString );
   for( i=0; i<len; i++ )
   {
      ch = *pszString++;
      if( isdigit( ch ) == FALSE )      return FALSE;
   }
   return TRUE;
}

/**********************************************************************/
/* associateHelp()                                                    */
/*   Associate help to the helpinstance if available.                 */
/**********************************************************************/
void associateHelp( HWND hwnd )
{
    if ( hwndHelpInstance == NULLHANDLE )  return;

    if( WinAssociateHelpInstance( hwndHelpInstance, hwnd ) == 0 )
    {
       showMessageBox( hwnd, IDS_WARNING_7, IDS_WARNING_CAPTION, MB_WARNING );
    }
    return;
}

/**********************************************************************/
/* putWindowToCenter()                                                */
/*   Places the window to the center of the desk top window.          */
/**********************************************************************/
void putWindowToCenter( HWND hwnd )
{
SWP swpPos;
LONG lPosX, lPosY;

   WinQueryWindowPos( hwnd, &swpPos );
   lPosX = (lDevCaps[DISP_X] - swpPos.cx)/2;
   lPosY = (lDevCaps[DISP_Y] - swpPos.cy)/2;
   WinSetWindowPos( hwnd, HWND_DESKTOP,
                    lPosX, lPosY, swpPos.cx, swpPos.cy,
                    SWP_SHOW | SWP_ACTIVATE | SWP_MOVE );
}

/**********************************************************************/
/* setDlgItemWcs()                                                    */
/*   Wide string version of setDlgItemText().                         */
/**********************************************************************/
void setDlgItemWcs( HWND hwnd, ULONG ulItemId, wchar_t* wcsText )
{
char *pszBuf;
int  sz;

    sz = wcslen(wcsText) * MB_CUR_MAX;                            /* 110695*/
    pszBuf = (char *)malloc(sz+1);                                /* 110695*/
    wcstombs( pszBuf, wcsText, sz+1 );
    WinSetDlgItemText( hwnd, ulItemId, pszBuf );
    free( pszBuf );                                               /* 110695*/
}

/**********************************************************************/
/* queryDlgItemWcs()                                                  */
/*   Wide string version of queryDlgItemText().                       */
/**********************************************************************/
size_t queryDlgItemWcs( HWND hwnd, ULONG ulItemId, wchar_t* wcsBuf, size_t szLen )
{
 char *pszBuf;
 LONG lLength;
 size_t szNumChar;

    lLength = WinQueryDlgItemTextLength( hwnd, ulItemId );        /* 110695*/
    pszBuf = malloc( (size_t)lLength+1 );                         /* 110695*/
    WinQueryDlgItemText( hwnd, ulItemId, lLength+1, pszBuf );     /* 110695*/
    szNumChar = mbstowcs( NULL, pszBuf, 0 );                      /* 110695*/
    szNumChar = (szNumChar > szLen)? szLen : szNumChar;           /* 110695*/

    szNumChar = mbstowcs( wcsBuf, pszBuf, szNumChar+1 );
    free( pszBuf );                                               /* 110695*/
    return( szNumChar );
}

/**********************************************************************/
/* formatDate()                                                       */
/*   Format date value of timeval to the format specified in the      */
/*   current locale.  The formatted string is stored in the buffer    */
/*   pointed to by pszBuf.  The format varies depending on the buffer */
/*   size specified by len.                                           */
/**********************************************************************/
BOOL formatDate( time_t timeval, char* pszBuf, size_t len )
{
  struct tm* timeptr;

  timeptr = localtime( &timeval );
  if( strftime( pszBuf, --len, "%x", timeptr ) == 0 )/* locale representation*/
  {
     if( strftime( pszBuf, len, "%Ex", timeptr ) == 0 )/* alternate representation*/
     {
        if( strftime( pszBuf, len, "%D", timeptr ) == 0 )/* force mm/yy/dd format*/
           return FALSE;
     }
  }
  return TRUE;
}

/**********************************************************************/
/* formatTime()                                                       */
/*   Format time value of timeval to the format specified in the      */
/*   current locale.  The formatted string is stored in the buffer    */
/*   pointed to by pszBuf.  The format varies depending on the buffer */
/*   size specified by len.                                           */
/**********************************************************************/
BOOL formatTime( time_t timeval, char* pszBuf, size_t len )
{
  struct tm* timeptr;

  timeptr = localtime( &timeval );
  if( strftime( pszBuf, --len, "%r", timeptr ) == 0 )/* locale representation*/
  {
     if( strftime( pszBuf, len, "%EX", timeptr ) == 0 )/* alternate representation*/
     {
        if( strftime( pszBuf, len, "%T", timeptr ) == 0 )/* force hh:mm:ss format*/
           return FALSE;
     }
  }
  return TRUE;
}

/**********************************************************************/
/* formatPrice()                                                      */
/*   Format price value of dPrice to the format specified in the      */
/*   current locale.  The formatted string is stored in the buffer    */
/*   pointed to by pszBuf.  National currency format is used when the */
/*   product record file is the same with the current locale.  Other- */
/*   wise, use the international currency format.                     */
/**********************************************************************/
void formatPrice( double dPrice, char* pszBuf, size_t len, BOOL flCurrency )
{
char *pszFormat;

   memset( pszBuf, '\0', len );                   /* ensure null termintate*/
   if( flProdLocale == TRUE )                   /* national currency format*/
   {
      /* The following code is to skip the strfmon's bug */
      if( dPrice == 0 )  strcpy( pszBuf, "\\0" );
      else
      {
         if( flCurrency==TRUE ) pszFormat = "%n";
         else                   pszFormat = "%!n";
         strfmon( pszBuf, --len, pszFormat, dPrice );/* national currency format*/
      }
   }
   else                                    /* international currency format*/
   {
      /* The following code is to skip the strfmon's bug */
      if( dPrice == 0 )  strcpy( pszBuf, "JPY 0" );
      else
      {
         if( flCurrency==TRUE ) pszFormat = "%i";
         else                   pszFormat = "%!i";
         strfmon( pszBuf, --len, pszFormat, dPrice );/* international currency format*/
      }
   }
}

/*****************************************************************************/
/* logError()                                                                */
/*      Log the file I/O error to the XPRMOS2.LOG file.                      */
/*****************************************************************************/
void logError( ulong ulCode, MPARAM mp )
{
FILE* fh;
char acLogMsg[BUFSIZ];

   if( (fh=fopen( pszLogFile, "a" )) == NULL )  return;

   switch( ulCode )
   {
      case ERR_CREATE_HELP:
        fprintf( fh, "Error in creating help instance.\n" );
        break;
      case ERR_LOAD_RESOURCE:
        fprintf( fh, "Error in loading resource file %d.\n", LONGFROMMP(mp) );
        break;

      case ERR_FILE_OPEN:
         fprintf( fh, "Error in opening file %s.\n", PVOIDFROMMP(mp) );
         break;

      case ERR_FILE_INVALID :
         fprintf( fh, "The file(%s) is invalid.\n", PVOIDFROMMP(mp) );
         break;

      case ERR_FILE_IO:
         fprintf( fh, "I/O error: %s.\n", PVOIDFROMMP(mp) );
         break;

      default:
         fprintf( fh, "Unknown error:%ld", ulCode );
         break;
   }
   fclose( fh );
}
