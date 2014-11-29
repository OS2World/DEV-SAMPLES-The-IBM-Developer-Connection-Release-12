/*************** loadicon C Sample Program Source Code File (.C) ****************/
/*                                                                            */
/* PROGRAM NAME: loadicon                                                       */
/* -------------                                                              */
/*  Presentation Manager Standard Window C Sample Program                     */
/*                                                                            */
/* COPYRIGHT:                                                                 */
/* ----------                                                                 */
/*  (C) Copyright International Business Machines Corporation 1988, 1989      */
/*  Copyright (c) 1988, 1989 Microsoft Corporation                            */
/*                                                                            */
/* REVISION LEVEL: 1.2                                                        */
/* ---------------                                                            */
/*                                                                            */
/* WHAT THIS PROGRAM DOES:                                                    */
/* -----------------------                                                    */
/*  This program displays a standard window containing the text "Hello".      */
/*  The action bar contains a single choice, Options.                         */
/*  The Options pull-down contains three choices that each                    */
/*  paint a different string in the window.                                   */
/*  It also contains an Exit choice.                                          */
/*                                                                            */
/* WHAT THIS PROGRAM DEMONSTRATES:                                            */
/* -------------------------------                                            */
/*  This program demonstrates how to create and display an icon from          */
/*  a selected file                                                           */
/*                                                                            */
/* WHAT YOU NEED TO COMPILE THIS PROGRAM:                                     */
/* --------------------------------------                                     */
/*                                                                            */
/*  REQUIRED FILES:                                                           */
/*  ---------------                                                           */
/*                                                                            */
/*    **NOTE** : See Readme.C for a list and description of required          */
/*               SYSTEM files.                                                */
/*                                                                            */
/*    loadicon.C       - Source code                                            */
/*    loadicon.CMD     - Command file to build this program                     */
/*    loadicon.MAK     - Make file for this program                             */
/*    loadicon.DEF     - Module definition file                                 */
/*    loadicon.H       - Application header file                                */
/*    loadicon.ICO     - Icon file                                              */
/*    loadicon.L       - Linker automatic response file                         */
/*    loadicon.RC      - Resource file                                          */
/*                                                                            */
/*    OS2.H          - Presentation Manager include file                      */
/*    STRING.H       - String handling function declarations                  */
/*                                                                            */
/*  REQUIRED LIBRARIES:                                                       */
/*  -------------------                                                       */
/*                                                                            */
/*    OS2.LIB        - Presentation Manager/OS2 library                       */
/*    SLIBCE.LIB     - Protect mode/standard combined small model C library   */
/*                                                                            */
/*  REQUIRED PROGRAMS:                                                        */
/*  ------------------                                                        */
/*                                                                            */
/*    IBM C Compiler                                                          */
/*    IBM Linker                                                              */
/*    Resource Compiler                                                       */
/*                                                                            */
/* EXPECTED INPUT:                                                            */
/* ---------------                                                            */
/*                                                                            */
/* EXPECTED OUTPUT:                                                           */
/* ----------------                                                           */
/*                                                                            */
/* CALLS USED (Dynamic Link References):                                      */
/* -------------------------------------                                      */
/*                                                                            */
/*    GpiCharStringAt     WinDefWindowProc    WinInvalidateRegion             */
/*    GpiSetBackColor     WinDestroyMsgQueue  WinLoadString                   */
/*    GpiSetBackMix       WinDestroyWindow    WinPostMsg                      */
/*    GpiSetColor         WinDispatchMsg      WinRegisterClass                */
/*    WinBeginPaint       WinEndPaint         WinSetWindowPos                 */
/*    WinCreateMsgQueue   WinGetMsg           WinTerminate                    */
/*    WinCreateStdWindow  WinInitialize                                       */
/*                                                                            */
/******************************************************************************/

#define INCL_BASE                       /* Selectively include          */
#define INCL_DOS                        /* Selectively include          */
#define INCL_GPI                        /* Selectively include          */
#define INCL_WIN                        /* the PM header file           */

#include <os2.h>                        /* PM header file               */
#include <string.h>                     /* C/2 string functions         */
#include <stdio.h>                      /* C/2 string functions         */
#include <string.h>                     /* C/2 string functions         */
#include "loadicon.h"                   /* Resource symbolic identifiers*/
#include <pmbitmap.h>

CHAR          szFileName[FILEDLG_DRIVE_LENGTH +
                         FILEDLG_PATH_LENGTH +
                         FILEDLG_FILE_LENGTH];
HFILE     IcoFile;
UCHAR                 bmHdrBufBW[sizeof(BITMAPINFOHEADER)+sizeof(RGB)*2];
UCHAR                 bmHdrBufCOL[sizeof(BITMAPINFOHEADER)+sizeof(RGB)*256];
PBITMAPINFOHEADER     pBmihBW=(PBITMAPINFOHEADER)bmHdrBufBW;
PBITMAPINFOHEADER     pBmihCOL=(PBITMAPINFOHEADER)bmHdrBufCOL;
PBITMAPINFO           pBmiBW=(PBITMAPINFO)bmHdrBufBW;
PBITMAPINFO           pBmiCOL=(PBITMAPINFO)bmHdrBufCOL;
SEL                   InfoSel;
BITMAPARRAYFILEHEADER bmafh;
BITMAPFILEHEADER      bmfh;
LONG                  numRealClrs;
RGB                   rgbValues[256];
ERRORID ErrorId;
ERRORID BitmapbitsBW;
ERRORID BitmapbitsCOL;
LONG realClrs[256];

HDC     hdcMem;                  /* Memory device context handle       */
HPS     hpsMem;                  /* Presentation space handle          */

SIZEL sizeBmp;
POINTERINFO iPtr;
HAB   hab;                             /* PM anchor block handle       */
SEL   BitSel;
PCH   BitBuffer;
ULONG FilePtr;
HPOINTER  hptrIcon;
/************************************************************************/
/* Function prototypes                                                  */
/************************************************************************/
MRESULT EXPENTRY MyWindowProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 );
VOID WindowInitialization(void);
VOID WindowDestroy(void);
void cdecl main( void );

                                        /* Define parameters by type    */
CHAR   Buffer[255];
HWND   hwndClient;                      /* Client area window handle    */
HWND   hwndFrame;                       /* Frame window handle          */
HWND   hIcon=NULL;                      /* Icon  window handle          */
USHORT Action;
USHORT bytesToRead;
USHORT bytesRead;
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

/**********************  Start of main procedure  ***********************/
void cdecl main(  )
{
  HMQ  hmq;                             /* Message queue handle         */
  QMSG qmsg;                            /* Message from message queue   */
  ULONG flCreate;                       /* Window creation control flags*/

  hab = WinInitialize( NULL );          /* Initialize PM                */
  hmq = WinCreateMsgQueue( hab, 0 );    /* Create a message queue       */

  WinRegisterClass(                     /* Register window class        */
     hab,                               /* Anchor block handle          */
     "MyWindow",                        /* Window class name            */
     MyWindowProc,                      /* Address of window procedure  */
     CS_SIZEREDRAW,                     /* Class style                  */
     0                                  /* No extra window words        */
     );

  flCreate = FCF_STANDARD &             /* Set frame control flags to   */
             ~FCF_SHELLPOSITION;        /* standard except for shell    */
                                        /* positioning.                 */

   hwndFrame = WinCreateStdWindow(
               HWND_DESKTOP,            /* Desktop window is parent     */
               0L,                      /* No frame styles              */
               &flCreate,               /* Frame control flag           */
               "MyWindow",              /* Client window class name     */
               "",                      /* No window text               */
               0L,                      /* No special class style       */
               NULL,                    /* Resource is in .EXE file     */
               ID_WINDOW,               /* Frame window identifier      */
               &hwndClient              /* Client window handle         */
               );

  if ( hwndFrame!=NULL) {           /* Shows and activates frame    */
  WinSetWindowPos( hwndFrame,           /* Shows and activates frame    */
                   HWND_TOP,            /* window at position 100, 100, */
                   100, 100, 200, 200,  /* and size 200, 200.           */
                   SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW
                 );


/************************************************************************/
/* Get and dispatch messages from the application message queue         */
/* until WinGetMsg returns FALSE, indicating a WM_QUIT message.         */
/************************************************************************/
  while( WinGetMsg( hab, &qmsg, NULL, 0, 0 ) ) {
    WinDispatchMsg( hab, &qmsg );
    }

  WinDestroyWindow( hwndFrame );        /* Tidy up...                   */
  }
  WinDestroyMsgQueue( hmq );            /* and                          */
  WinTerminate( hab );                  /* terminate the application    */
}
/***********************  End of main procedure  ************************/

/*********************  Start of window procedure  **********************/
MRESULT EXPENTRY MyWindowProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
  USHORT command;                       /* WM_COMMAND command value     */
  HPS    hps;                           /* Presentation Space handle    */
  RECTL  rc;                            /* Rectangle coordinates        */
  POINTL pt;                            /* String screen coordinates    */
  USHORT RetCode;
  switch( msg )
  {
    case WM_CREATE:
      WindowInitialization();
      hIcon=WinCreateWindow(         /* Create ListBox              */
               hwnd,                   /* Handle parent               */
               WC_STATIC,              /* Window Style                */
               ID_WINDOW_S,            /* Icon handle                 */
               WS_VISIBLE   |          /* Frame control flag          */
               SS_ICON ,
               10, 10, 100,100,             /* Size and position           */
               hwnd,
               HWND_TOP,
               2222,               /* Window identifier           */
               NULL,
               NULL);
      break;
    case WM_DESTROY:
            /***********************************************************/
            /* The window is to be destroyed.                          */
            /***********************************************************/
            WindowDestroy();
            return( FALSE );

    case WM_COMMAND:
      /******************************************************************/
      /* When the user chooses option 1, 2, or 3 from the Options pull- */
      /* down, the text string is set to 1, 2, or 3, and                */
      /* WinInvalidateRegion sends a WM_PAINT message.                  */
      /* When Exit is chosen, the application posts itself a WM_CLOSE   */
      /* message.                                                       */
      /******************************************************************/
      command = SHORT1FROMMP(mp1);      /* Extract the command value    */
      switch (command)
      {
        case ID_OPEN:
          RetCode = GetFileSelection(hwnd, FILEDLG_OPEN, szIcoPattern,
                             szIcoDrive, szIcoPath, szIcoFile);
          if (RetCode >= 0) {
             strcpy(szFileName, szIcoDrive);
             if (szIcoPath[0] != '\0') {
                strcat(szFileName, "\\");
                strcat(szFileName, szIcoPath);
             } else {
             } /* endif */
             strcat(szFileName, "\\");
             strcat(szFileName, szIcoFile);
             if (RetCode == 1) {
                 RetCode = WinMessageBox(HWND_DESKTOP, hwndFrame,
                              (PSZ)"Icon not found\n",
                              (PSZ)"Open .ICO",
                              0,
                              MB_CANCEL | MB_MOVEABLE );
                 break;
             } else {
                 RetCode=DosOpen2(szFileName,
                                 (PHFILE)&IcoFile,
                                 (PUSHORT)&Action,
                                 0L,
                                 FILE_NORMAL | FILE_ARCHIVED,
                                 OPEN_ACTION_OPEN_IF_EXISTS |
                                 OPEN_ACTION_FAIL_IF_NEW ,
                                 (ULONG)OPEN_ACCESS_READONLY |
                                        OPEN_SHARE_DENYNONE,
                                 (PEAOP)NULL,
                                 0L);
                if (RetCode!=0) {
                   sprintf(Buffer,"Open File >%s< \nFail rc %d",szFileName,RetCode);
                   DosClose(IcoFile);
                   RetCode = WinMessageBox(HWND_DESKTOP, hwndFrame,
                              Buffer,
                              (PSZ)"Open .ICO",
                              0,
                              MB_CANCEL | MB_MOVEABLE );
                   break;
                }
                bytesToRead=sizeof(bmafh);
                RetCode=DosRead(IcoFile,
                      &bmafh,
                      bytesToRead,
                      &bytesRead);
                if (RetCode!=0) {
                   sprintf(Buffer,"Read >%s<\nFail rc %d",szFileName,RetCode);
                   DosClose(IcoFile);
                   RetCode = WinMessageBox(HWND_DESKTOP, hwndFrame,
                              Buffer,
                              (PSZ)"Read .ICO",
                              0,
                              MB_CANCEL | MB_MOVEABLE );
                   break;
                }
                if (bmafh.bfh.usType!=BFT_COLORICON) {   // Array element - icon
                   DosClose(IcoFile);
                   sprintf(Buffer,"File Not 1.2 format Icon: %c%c",
                              *((CHAR *)&bmafh.bfh.usType),
                              *(((CHAR *)&bmafh.bfh.usType)+1));
                   RetCode = WinMessageBox(HWND_DESKTOP, hwndFrame,
                              Buffer,
                              (PSZ)"Analyse .ICO",
                              0,
                              MB_CANCEL | MB_MOVEABLE );
                   break;
                }
                bytesToRead=sizeof(RGB)*2;
                RetCode=DosRead(IcoFile,
                      bmHdrBufBW+sizeof(BITMAPINFOHEADER),
                      bytesToRead,
                      &bytesRead);
                if (RetCode!=0) {
                   sprintf(Buffer,"Mask read Fail rc %d",szFileName,RetCode);
                   DosClose(IcoFile);
                   RetCode = WinMessageBox(HWND_DESKTOP, hwndFrame,
                              Buffer,
                              (PSZ)"Read .ICO",
                              0,
                              MB_CANCEL | MB_MOVEABLE );
                   break;
                }
                bytesToRead=sizeof(bmfh);
                /******** Read color bitmap header */
                RetCode=DosRead(IcoFile,
                      &bmfh,
                      bytesToRead,
                      &bytesRead);
                if (RetCode!=0) {
                   sprintf(Buffer,"Color header read Fail rc %d",szFileName,RetCode);
                   DosClose(IcoFile);
                   RetCode = WinMessageBox(HWND_DESKTOP, hwndFrame,
                              Buffer,
                              (PSZ)"Read .ICO",
                              0,
                              MB_CANCEL | MB_MOVEABLE );
                   break;
                }
                bytesToRead=sizeof(RGB)*256;
                /******** Read color bitmap color table */
                RetCode=DosRead(IcoFile,
                      bmHdrBufCOL+sizeof(BITMAPINFOHEADER),
                      bytesToRead,
                      &bytesRead);
                if (RetCode!=0) {
                   sprintf(Buffer,"Color table read Fail rc %d",szFileName,RetCode);
                   DosClose(IcoFile);
                   RetCode = WinMessageBox(HWND_DESKTOP, hwndFrame,
                              Buffer,
                              (PSZ)"Read .ICO",
                              0,
                              MB_CANCEL | MB_MOVEABLE );
                   break;
                }
                //------------------- BW Bit map creation
                // Create a bitmap, and set it in the temp HPS, so
                // that we can copy the rectangle into it
                // Create a memory device context and hps
                pBmihBW->cbFix=sizeof(BITMAPINFOHEADER);
                pBmihBW->cx=(USHORT)bmafh.bfh.bmp.cx;
                pBmihBW->cy=(USHORT)bmafh.bfh.bmp.cy;
                pBmihBW->cPlanes=1;
                pBmihBW->cBitCount=1;
                iPtr.hbmPointer=GpiCreateBitmap(hpsMem,pBmihBW,0L,0L,NULL);
                GpiSetBitmap( hpsMem,iPtr.hbmPointer );

                DosAllocSeg( ((pBmihBW->cx+31)/32)*4*pBmihBW->cy*pBmihBW->cPlanes*pBmihBW->cBitCount,
                             &BitSel, SEG_NONSHARED);
                BitBuffer=MAKEP(BitSel,0);
                DosChgFilePtr(IcoFile, bmafh.bfh.offBits,0,&FilePtr );
                bytesToRead=((pBmihBW->cx+31)/32)*4*pBmihBW->cy*pBmihBW->cPlanes*pBmiBW->cBitCount;
                RetCode=DosRead(IcoFile,
                      BitBuffer,
                      bytesToRead,
                      &bytesRead);
                if (RetCode!=0) {
                   sprintf(Buffer,"BW bits  read Fail rc %d",szFileName,RetCode);
                   DosClose(IcoFile);
                   RetCode = WinMessageBox(HWND_DESKTOP, hwndFrame,
                              Buffer,
                              (PSZ)"Read .ICO",
                              0,
                              MB_CANCEL | MB_MOVEABLE );
                   break;
                }
                BitmapbitsBW=WinGetLastError(hab);
                GpiSetBitmapBits(hpsMem,0L,(LONG)bmafh.bfh.bmp.cy,BitBuffer,pBmiBW);
                BitmapbitsBW=WinGetLastError(hab);
                DosFreeSeg(BitSel);
                //------------------- Color Bit map creation
                pBmihCOL->cbFix=sizeof(BITMAPINFOHEADER);
                pBmihCOL->cx=(USHORT)bmfh.bmp.cx;
                pBmihCOL->cy=(USHORT)bmfh.bmp.cy;
                pBmihCOL->cPlanes  =bmfh.bmp.cPlanes;
                pBmihCOL->cBitCount=bmfh.bmp.cBitCount;
                iPtr.hbmColor=GpiCreateBitmap(hpsMem,pBmihCOL,0L,0L,NULL);
                GpiSetBitmap( hpsMem,iPtr.hbmColor );

                DosAllocSeg( ((pBmihCOL->cx+31)/32)*4*pBmihCOL->cy*pBmihCOL->cPlanes*pBmihCOL->cBitCount,
                             &BitSel, SEG_NONSHARED);
                BitBuffer=MAKEP(BitSel,0);
                DosChgFilePtr(IcoFile, bmfh.offBits,0,&FilePtr );
                bytesToRead=((pBmihCOL->cx+31)/32)*4*pBmihCOL->cy*pBmihCOL->cPlanes*pBmihCOL->cBitCount;
                RetCode=DosRead(IcoFile,
                      BitBuffer,
                      bytesToRead,
                      &bytesRead);
                if (RetCode!=0) {
                   sprintf(Buffer,"Color bits  read Fail rc %d",szFileName,RetCode);
                   DosClose(IcoFile);
                   RetCode = WinMessageBox(HWND_DESKTOP, hwndFrame,
                              Buffer,
                              (PSZ)"Read .ICO",
                              0,
                              MB_CANCEL | MB_MOVEABLE );
                   break;
                }
                BitmapbitsCOL=WinGetLastError(hab);
                GpiSetBitmapBits(hpsMem,0L,(LONG)bmfh.bmp.cy,BitBuffer,pBmiCOL);
                BitmapbitsCOL=WinGetLastError(hab);
                DosFreeSeg(BitSel);
                GpiSetBitmap( hpsMem, (HBITMAP)NULL );
                DosClose(IcoFile);
                iPtr.fPointer=FALSE; /* Icon */
                iPtr.xHotspot=0;
                iPtr.yHotspot=0;
                ErrorId=WinGetLastError(hab);
                hptrIcon=WinCreatePointerIndirect(HWND_DESKTOP,&iPtr);
                ErrorId=WinGetLastError(hab);
                WinSendMsg(hIcon,SM_SETHANDLE,(MPARAM)hptrIcon,NULL);
                WinSendMsg(hwndFrame,WM_SETICON,(MPARAM)hptrIcon,NULL);
                WinSendMsg(hwndFrame,WM_UPDATEFRAME,(MPARAM)FCF_ICON,NULL);
             } /* endif */
          } else {
          } /* endif */
          WinInvalidateRegion( hwnd, NULL, FALSE );
          WinInvalidateRegion( hIcon, NULL, FALSE );
          break;
        case ID_EXITPROG:
          WinPostMsg( hwnd, WM_CLOSE, 0L, 0L );
          break;
        default:
          return WinDefWindowProc( hwnd, msg, mp1, mp2 );
      }
      break;
    case WM_ERASEBACKGROUND:
      /******************************************************************/
      /* Return TRUE to request PM to paint the window background       */
      /* in SYSCLR_WINDOW.                                              */
      /******************************************************************/
      return (MRESULT)( TRUE );
    case WM_PAINT:
      /******************************************************************/
      /* Window contents are drawn here in WM_PAINT processing.         */
      /******************************************************************/
                                        /* Create a presentation space  */
      hps = WinBeginPaint( hwnd, NULL, &rc );
      WinFillRect( hps, &rc,CLR_BACKGROUND  );/* Fill invalid rectangle       */
      GpiSetColor( hps, CLR_NEUTRAL );         /* colour of the text,   */
      GpiSetBackColor( hps, CLR_BACKGROUND );  /* its background and    */
      GpiSetBackMix( hps, BM_OVERPAINT );      /* how it mixes,         */
                                               /* and draw the string...*/
      sprintf(Buffer,"errbitBW %p,errbitCOL %p,error %p " ,
                     BitmapbitsBW,BitmapbitsCOL,ErrorId);
      pt.x=10;
      pt.y=70;
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer );
      sprintf(Buffer,"hPtr %p , hBmap %p , hColor %p" ,
                     hptrIcon,iPtr.hbmPointer,iPtr.hbmColor);

      pt.x=10;
      pt.y=50;
      GpiCharStringAt( hps, &pt, (LONG)strlen( Buffer ), Buffer );
      WinEndPaint( hps );                      /* Drawing is complete   */
      break;
    case WM_CHAR:
      /******************************************************************/
      /* Character input is processed here.                             */
      /* The first two bytes of message parameter 2 contain             */
      /* the character code.                                            */
      /******************************************************************/
       WinInvalidateRegion( hwnd, NULL, FALSE );
      break;                                /* end the application.     */
    case WM_CLOSE:
      /******************************************************************/
      /* This is the place to put your termination routines             */
      /******************************************************************/
      WinPostMsg( hwnd, WM_QUIT, 0L, 0L );  /* Cause termination        */
      break;
    default:
      /******************************************************************/
      /* Everything else comes here.  This call MUST exist              */
      /* in your window procedure.                                      */
      /******************************************************************/

      return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  return FALSE;
}
/***********************************************************************/
/* This function deletes the presentation space, and closes the memory */
/* device context.                                                     */
/***********************************************************************/

VOID WindowDestroy()
{
    GpiAssociate( hpsMem, (HDC)NULL );
    GpiDestroyPS( hpsMem );
    DevCloseDC( hdcMem );
}
/**************** End of WINDOWDESTROY Private Function ****************/

/*********** Start of WINDOWINITIALIZATION Private Function ************/
/***********************************************************************/
/* This function creates a screen-compatible device context and        */
/* defines a normal presentation space. The presentation page is the   */
/* same size as the client area of the window, and is defined in pels. */
/* The presentation space and the device context are associated.       */
/* The value of the global variable defining initial window size is    */
/* set.                                                                */
/***********************************************************************/

VOID WindowInitialization()
{
    SIZEL   size;

    size.cx = WinQuerySysValue(HWND_DESKTOP, SV_CXFULLSCREEN);
    size.cy = WinQuerySysValue(HWND_DESKTOP, SV_CYFULLSCREEN);


    hdcMem = DevOpenDC( hab
                      , OD_MEMORY
                      , (PSZ)"*"
                      , 8L
                      , (PDEVOPENDATA)dcdatablk
                      , (HDC)NULL
                      );

    hpsMem = GpiCreatePS( hab
                        , hdcMem
                        , (PSIZEL)&size
                        , (LONG)PU_PELS | GPIT_NORMAL | GPIA_ASSOC
                        );
   DosBeep(400,100);
}
/************ End of WINDOWINITIALIZATION Private Function *************/
