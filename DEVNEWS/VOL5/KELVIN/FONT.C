/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *   Demonstrate some simple font related functions.                         *
 *                                                                           *
 *****************************************************************************/


#define INCL_32
#define INCL_GPI
#define INCL_WIN
#include <os2.h>

#include "font.h"
#include <string.h>
#include <stdlib.h>
#include <memory.h>

HWND    hwndFrame;
PFNWP   pfnwpFrame;
HAB     hab;
       
VOID DisplayFontDlg( HWND hwnd );
VOID CreateFontAndDisplayIt( HWND hwnd, PFATTRS pfAttrs );
MRESULT EXPENTRY ClientWndProc( HWND hwnd ,ULONG msg ,MPARAM mp1 ,MPARAM mp2 );
MRESULT EXPENTRY FrameSubclassProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

#define CLASSNAME "FONT_TEST"

CHAR    FaceName[32] = "";
CHAR    pszText[100] = "Hello !";
LONG    lMatch      = 0;    
FATTRS  fattrs;
HPS     hpsClient;
FONTDLG fd ;
BOOL    fFont = FALSE;

void cdecl main(VOID)
{

   HMQ hmq;
   HWND hwndclient;
   QMSG qmsg;

   ULONG flCreateFlags =   FCF_BORDER     | FCF_SHELLPOSITION |
                           FCF_TASKLIST   | FCF_TITLEBAR      | FCF_SYSMENU |
                           FCF_SIZEBORDER | FCF_MINMAX        | FCF_MENU ;

   hab = WinInitialize( (USHORT)NULL );

   hmq=WinCreateMsgQueue( hab,0 );


   WinRegisterClass( hab
                   , CLASSNAME
                   , (PFNWP)ClientWndProc
                   , (ULONG)CS_SIZEREDRAW
                   , (USHORT)256 );

   hwndFrame = WinCreateStdWindow( HWND_DESKTOP
                                 ,  0UL
                                 ,  &flCreateFlags
                                 ,  CLASSNAME
                                 ,  "Fonts"
                                 ,  WS_VISIBLE
                                 ,  (HMODULE)0
                                 ,  ID_FONT
                                 ,  &hwndclient );


   WinShowWindow( hwndFrame, TRUE );

   while ( WinGetMsg( hab,&qmsg, (HWND)0, 0, 0 ) )
   {
     WinDispatchMsg( hab,&qmsg );
   }

   if ( hwndFrame )
   {
     WinDestroyWindow(hwndFrame);
     WinDestroyMsgQueue(hmq);
     WinTerminate(hab);
   }
}

/**************************************************************************
 *                                                                        *
 * Client window procedure.                                               *
 *                                                                        *
 **************************************************************************/
MRESULT EXPENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

   switch (msg)
   {
     case  WM_COMMAND :
     {
        switch (SHORT1FROMMP(mp1))
        {
            case  IDM_SELFONT: 
            {
              DisplayFontDlg( hwnd );
              WinInvalidateRect( hwnd, NULL, FALSE );
            }
            break;

            case IDM_EXIT:
            {
              WinPostMsg( hwndFrame, WM_CLOSE, 0L, 0L );
            }
            break;
        }
     }
     break;


     case WM_CREATE:
     {
       hpsClient = WinGetPS( hwnd );
       return( WinDefWindowProc( hwnd, msg, mp1, mp2 )) ;
     }
     break;

     case WM_PAINT:
     {
       HPS   hpsPaint ;
       RECTL rectl;

       hpsPaint = WinBeginPaint( hwnd, (HPS)0, &rectl );

       WinFillRect( hpsPaint, &rectl, SYSCLR_WINDOW );

       if ( fFont )
       {
         CreateFontAndDisplayIt( hwnd, &fd.fAttrs );
       }

       WinEndPaint( hpsPaint );
     }
     break;

     default:
     {
       return( WinDefWindowProc( hwnd, msg, mp1, mp2 )) ;
     }
   }
}

VOID DisplayFontDlg( HWND hwnd )
{
  HPS hps;

  CHAR pszTitle[17] = "Font Dialog Test";

  // Reserve storage for the fonts name

  CHAR pszFamilyName[FACESIZE] = "\0";

  // Initialize the font dialog structure.

  memset( &fd, 0, sizeof( FONTDLG ));

  hps = WinGetPS( hwnd );

  fd.hpsScreen = hps;
  fd.cbSize    = sizeof( FONTDLG );
  fd.pszFamilyname =  pszFamilyName ;
  fd.pszTitle = pszTitle;
  fd.usFamilyBufLen = FACESIZE;
  fd.fxPointSize = (FIXED)0;
  fd.clrFore = CLR_BLACK;
  fd.clrBack = CLR_WHITE;
  fd.pszPreview = "Sample Text";
  fd.fl = FNTS_CENTER;

  WinFontDlg( HWND_DESKTOP, hwndFrame, &fd );

  WinReleasePS( hps );

  if ( fd.lReturn == DID_OK )
  {
    fFont = TRUE;
  }
}


VOID CreateFontAndDisplayIt( HWND hwnd, PFATTRS pfAttrs )
{
 #define MYCHARSET 99L

 HPS hps = WinGetPS( hwnd );
 SIZEF box;

 // Create a logical font for the FATTRS passed in. Use MYCHARSET as
 // the character set number for this logical font.

 GpiCreateLogFont( hps, NULL, MYCHARSET, pfAttrs );

 // Select MYCHARSET as the current one for thhis Presentation Space

 GpiSetCharSet( hps, MYCHARSET );

 // If the user had slected an outline font then, as a rough approximation
 // set the character box to the maximum baseline extent so that the text
 // will be roughly in the size requested. Note that bitmap fonts are of 
 // size and we don't have to explicitly size them.

 if ( pfAttrs->fsFontUse & FATTR_FONTUSE_OUTLINE )
 {
   box.cx = box.cy = pfAttrs->lMaxBaselineExt << 16;
   GpiSetCharBox( hps, &box );
 }

 // Draw a text string at the current position, using our new logical font

 GpiCharString( hps, 6L,  "Hello!" );

 WinReleasePS( hps );
}

VOID MyQueryFonts( HWND hwnd )
{
   LONG lNum;  
   LONG lRem;
   LONG lReq=0;
   PFONTMETRICS pfm;
   HPS hps;

   // Get a presentation space for the window provided.

   hps = WinGetPS( hwnd );

   // Query how many fonts are available to us at this time

   lNum = GpiQueryFonts( hps
                       , QF_PUBLIC | QF_PRIVATE
                       , NULL
                       , &lReq
                       , 0L
                       , NULL
                       );


   // Allocate space to store the fontmetrics structures

   pfm = (PFONTMETRICS) malloc( sizeof(FONTMETRICS) *lNum );

   // Now query the metrics structures

   lRem = GpiQueryFonts( hps
                       , QF_PUBLIC | QF_PRIVATE
                       , NULL
                       , &lNum
                       , sizeof(FONTMETRICS)
                       , pfm
                       );

   // Free resources

   free( pfm );
   WinReleasePS( hps );
}



