+----------------------------------------------------------------------------+
|                                                                            |
| TITLEBAR    This is a DLL loaded by PM SYS_DLLS Load                        |
|                                                                            |
+-------------------------------------+--------------------------------------+
| Version: 1.0                        |   Marc Fiammante (FIAMMANT at LGEVM2)|
+-------------------------------------+--------------------------------------+
|                                                                            |
+-------------------------------------+--------------------------------------+
| History:                                                                   |
| --------                                                                   |
|                                                                            |
| created: Marc Fiammante Nov  1991                                          |
+---------------------------------------------------------------------------*/
/*----------------------------------------------------------*/
#define  INCL_WIN
#define  INCL_BASE
#define  INCL_DOS
#include <os2.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
MRESULT EXPENTRY WCTitleBarProc(HWND hwnd,USHORT  msg,MPARAM mp1,MPARAM mp2 );
CLASSINFO classInfo;
VOID EXPENTRY  GLOBALINIT() {
     HAB hab;
     static BOOL Initialized=FALSE;
     if ( !Initialized )        /* do the re-registeration only once! */
       {
         Initialized = TRUE ;
         hab = WinInitialize( NULL) ;
         if (hab==NULL) {
            hab=WinQueryAnchorBlock(HWND_DESKTOP);
         } /* endif */
         WinQueryClassInfo(NULL,
                          WC_TITLEBAR,
                          &classInfo);
         WinRegisterClass(                            /* Register window class       */
            hab,                                      /* Anchor block handle         */
            WC_TITLEBAR,                              /* Window class name           */
            WCTitleBarProc,                           /* Address of window procedure */
            classInfo.flClassStyle,
            classInfo.cbWindowData                    /* No extra window words       */
            );
       }
}
MRESULT EXPENTRY WCTitleBarProc (HWND hwnd,USHORT  msg,MPARAM mp1,MPARAM mp2 ) {
#define BUFLEN 256
#define DELTA  10
  USHORT rc;
  ULONG  SegSize;
  CHAR       Buffer[BUFLEN];
  switch (msg) {
    case WM_SETWINDOWPARAMS:
     /* Check access to WNDPARAMS structure */
     rc=DosSizeSeg(SELECTOROF(mp1),&SegSize);
     if (rc!=NO_ERROR) {
        DosBeep(400,30);
        return ((*classInfo.pfnWindowProc)(hwnd, msg, mp1, mp2 ));
     } /* endif */
     if (OFFSETOF(mp1)+sizeof(WNDPARAMS)>(USHORT)SegSize) {
        DosBeep(600,30);
        return ((*classInfo.pfnWindowProc)(hwnd, msg, mp1, mp2 ));
     } /* endif */
     if (  ((((PWNDPARAMS)mp1)->fsStatus)       &WPM_TEXT) &&
           ((((PWNDPARAMS)mp1)->cchText) +DELTA<BUFLEN) ) {
         rc=DosSizeSeg(SELECTOROF(((PWNDPARAMS)mp1)->pszText),&SegSize);
         if (rc!=NO_ERROR) {
            DosBeep(1400,30);
            return ((*classInfo.pfnWindowProc)(hwnd, msg, mp1, mp2 ));
         } /* endif */
         if (OFFSETOF(((PWNDPARAMS)mp1)->pszText)+
                     (((PWNDPARAMS)mp1)->cchText)>(USHORT)SegSize) {
            DosBeep(1600,30);
            return ((*classInfo.pfnWindowProc)(hwnd, msg, mp1, mp2 ));
         } /* endif */
         memset(Buffer,0x20,BUFLEN-1);
         memcpy(Buffer,"Marc's - ",9);
         memcpy(Buffer+DELTA,((PWNDPARAMS)mp1)->pszText,
                             ((PWNDPARAMS)mp1)->cchText);
         Buffer[DELTA+(((PWNDPARAMS)mp1)->cchText)]=0x00;
         ((PWNDPARAMS)mp1)->cchText+=DELTA;
         ((PWNDPARAMS)mp1)->pszText =Buffer;
     } /* endif */
     return ((*classInfo.pfnWindowProc)(hwnd, msg, mp1, mp2 ));
     break;
    case WM_CREATE:
     /* Check access to CREATESTRUCT structure */
     rc=DosSizeSeg(SELECTOROF(mp2),&SegSize);
     if (rc!=NO_ERROR) {
        DosBeep(400,30);
        return ((*classInfo.pfnWindowProc)(hwnd, msg, mp1, mp2 ));
     } /* endif */
     if (OFFSETOF(mp2)+sizeof(CREATESTRUCT)>(USHORT)SegSize) {
        DosBeep(600,30);
        return ((*classInfo.pfnWindowProc)(hwnd, msg, mp1, mp2 ));
     } /* endif */
     rc=DosSizeSeg(SELECTOROF(((PCREATESTRUCT)mp2)->pszText),&SegSize);
     if (rc!=NO_ERROR) {
        DosBeep(800,30);
        return ((*classInfo.pfnWindowProc)(hwnd, msg, mp1, mp2 ));
     } /* endif */
     if ( strlen(((PCREATESTRUCT)mp2)->pszText) +DELTA<BUFLEN  ) {
         memset(Buffer,0x20,BUFLEN-1);
         memcpy(Buffer,"Marc's - ",9);
         strcpy(Buffer+DELTA,((PCREATESTRUCT)mp2)->pszText);
         ((PCREATESTRUCT)mp2)->pszText =Buffer;
     } /* endif */
     return ((*classInfo.pfnWindowProc)(hwnd, msg, mp1, mp2 ));
       break;
    default:
     return ((*classInfo.pfnWindowProc)(hwnd, msg, mp1, mp2 ));
  } /* endswitch */
}
