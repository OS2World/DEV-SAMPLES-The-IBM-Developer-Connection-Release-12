/*
 * SYSVALS.C -- System Values Display Program using AVIO
 */

#define INCL_WIN
#define INCL_GPI
#define INCL_BASE
#define INCL_AVIO
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMLINES (sizeof sysvals / sizeof sysvals [0])

struct
     {
     SHORT sIndex ;
     CHAR  *szIdentifier ;
     CHAR  *szDescription ;
     }
     sysvals [] =
     {
     SV_SWAPBUTTON,     "SV_SWAPBUTTON",     "Mouse buttons swapped flag",
     SV_DBLCLKTIME,     "SV_DBLCLKTIME",     "Mouse double click time in msec",
     SV_CXDBLCLK,       "SV_CXDBLCLK",       "Mouse double click area width",
     SV_CYDBLCLK,       "SV_CYDBLCLK",       "Mouse double click area height",
     SV_CXSIZEBORDER,   "SV_CXSIZEBORDER",   "Sizing border width",
     SV_CYSIZEBORDER,   "SV_CYSIZEBORDER",   "Sizing border height",
     SV_ALARM,          "SV_ALARM",          "Alarm enabled flag",
     SV_CURSORRATE,     "SV_CURSORRATE",     "Cursor blink rate",
     SV_FIRSTSCROLLRATE,"SV_FIRSTSCROLLRATE","Scroll bar time until repeats",
     SV_SCROLLRATE,     "SV_SCROLLRATE",     "Scroll bar scroll rate",
     SV_NUMBEREDLISTS,  "SV_NUMBEREDLISTS",  "Flag for numbering of lists",
     SV_WARNINGFREQ,    "SV_WARNINGFREQ",    "Alarm frequency for warning",
     SV_NOTEFREQ,       "SV_NOTEFREQ",       "Alarm frequency for note",
     SV_ERRORFREQ,      "SV_ERRORFREQ",      "Alarm frequency for error",
     SV_WARNINGDURATION,"SV_WARNINGDURATION","Alarm duration for warning",
     SV_NOTEDURATION,   "SV_NOTEDURATION",   "Alarm duration for note",
     SV_ERRORDURATION,  "SV_ERRORDURATION",  "Alarm duration for error",
     SV_CXSCREEN,       "SV_CXSCREEN",       "Screen width in pixels",
     SV_CYSCREEN,       "SV_CYSCREEN",       "Screen height in pixels",
     SV_CXVSCROLL,      "SV_CXVSCROLL",      "Vertical scroll bar width",
     SV_CYHSCROLL,      "SV_CYHSCROLL",      "Horizontal scroll bar height",
     SV_CYVSCROLLARROW, "SV_CYVSCROLLARROW", "Vertical scroll arrow height",
     SV_CXHSCROLLARROW, "SV_CXHSCROLLARROW", "Horizontal scroll arrow width",
     SV_CXBORDER,       "SV_CXBORDER",       "Border width",
     SV_CYBORDER,       "SV_CYBORDER",       "Border height",
     SV_CXDLGFRAME,     "SV_CXDLGFRAME",     "Dialog window frame width",
     SV_CYDLGFRAME,     "SV_CYDLGFRAME",     "Dialog window frame height",
     SV_CYTITLEBAR,     "SV_CYTITLEBAR",     "Title bar height",
     SV_CYVSLIDER,      "SV_CYVSLIDER",      "Vertical scroll slider height",
     SV_CXHSLIDER,      "SV_CXHSLIDER",      "Horizontal scroll slider width",
     SV_CXMINMAXBUTTON, "SV_CXMINMAXBUTTON", "Minimize/Maximize button width",
     SV_CYMINMAXBUTTON, "SV_CYMINMAXBUTTON", "Minimize/Maximize button height",
     SV_CYMENU,         "SV_CYMENU",         "Menu bar height",
     SV_CXFULLSCREEN,   "SV_CXFULLSCREEN",   "Full screen client window width",
     SV_CYFULLSCREEN,   "SV_CYFULLSCREEN",   "Full screen client window height",
     SV_CXICON,         "SV_CXICON",         "Icon width",
     SV_CYICON,         "SV_CYICON",         "Icon height",
     SV_CXPOINTER,      "SV_CXPOINTER",      "Pointer width",
     SV_CYPOINTER,      "SV_CYPOINTER",      "Pointer height",
     SV_DEBUG,          "SV_DEBUG",          "Debug version flag",
     SV_CMOUSEBUTTONS,  "SV_CMOUSEBUTTONS",  "Number of mouse buttons",
     SV_POINTERLEVEL,   "SV_POINTERLEVEL",   "Pointer display count",
     SV_CURSORLEVEL,    "SV_CURSORLEVEL",    "Cursor display count",
     SV_TRACKRECTLEVEL, "SV_TRACKRECTLEVEL", "Tracking rectangle display count",
     SV_CTIMERS,        "SV_CTIMERS",        "Number of available timers",
     SV_MOUSEPRESENT,   "SV_MOUSEPRESENT",   "Mouse present flag",
     SV_CXBYTEALIGN,    "SV_CXBYTEALIGN",    "Horizontal pixel alignment value",
     SV_CYBYTEALIGN,    "SV_CYBYTEALIGN",    "Vertical pixel alignment value"
     } ;

#define MAXWIDTH 60

MRESULT EXPENTRY ClientWndProc (HWND, ULONG, MPARAM, MPARAM) ;

HAB  hab ;

static CHAR  szClientClass [] = "SysVals" ;
static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_VERTSCROLL    | FCF_HORZSCROLL ;

int main (void) {
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, 0L, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, NULL,
                                     0L, 0, 0, &hwndClient) ;

     WinSendMsg (hwndFrame, WM_SETICON,
                 (MPARAM)WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON,
                                             FALSE),
                 NULL) ;

     while (WinGetMsg (hab, &qmsg, 0, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
}

BYTE RgbToVioColor (COLOR clrRgb) {
     BYTE bIrgb = 0;
     RGB  rgb ;

     rgb = MAKETYPE (clrRgb, RGB) ;

     if (rgb.bBlue  >= 0x80) bIrgb |= '\x01' ;
     if (rgb.bGreen >= 0x80) bIrgb |= '\x02' ;
     if (rgb.bRed   >= 0x80) bIrgb |= '\x04' ;

     if (rgb.bBlue >= 0xC0 || rgb.bGreen >= 0xC0 || rgb.bRed >= 0xC0)
          bIrgb |= 8 ;

     if (bIrgb == 0 && rgb.bBlue >= 0x40 && rgb.bGreen >= 0x40 &&
                       rgb.bRed  >= 0x40)
          bIrgb = 8 ;

     return bIrgb ;
}

BYTE ConstructDefaultAttribute (VOID) {
     return RgbToVioColor (
                 WinQuerySysColor (HWND_DESKTOP, SYSCLR_WINDOW, 0L)) << 4 |
            RgbToVioColor (
                 WinQuerySysColor (HWND_DESKTOP, SYSCLR_WINDOWTEXT, 0L)) ;
}

MRESULT EXPENTRY ClientWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
     static BYTE   bBlankCell [2] = " " ;
     static HVIO   hvps ;
     static HWND   hwndHscroll, hwndVscroll ;
     static HPS    hps ;
     static SHORT  sHscrollPos, sVscrollPos,
                   cxClient, cyClient ;
     static ULONG  cxChar, cyChar;
     CHAR          szBuffer [80] ;
     HDC           hdc ;
     SIZEL         sizl ;
     USHORT        usRow ;
     int           i;

     switch (msg)
          {
          case WM_CREATE:
               hdc = WinOpenWindowDC (hwnd) ;

               sizl.cx = sizl.cy = 0 ;
               hps = GpiCreatePS (hab, hdc, &sizl, PU_PELS    | GPIF_DEFAULT |
                                                   GPIT_MICRO | GPIA_ASSOC) ;

               VioCreatePS (&hvps, NUMLINES, MAXWIDTH, 0, 1, 0) ;
               VioAssociate (hdc, hvps) ;

               VioGetDeviceCellSize (&cyChar, &cxChar, hvps) ;

               bBlankCell[1] = ConstructDefaultAttribute () ;
               VioScrollUp (0, 0, -1, -1, -1, bBlankCell, hvps) ;

               for (usRow = 0 ; usRow < NUMLINES ; usRow++)
                    VioWrtCharStr (szBuffer,
                                   sprintf (szBuffer, "%-20s%-35s%5ld",
                                            sysvals[usRow].szIdentifier,
                                            sysvals[usRow].szDescription,
                                            WinQuerySysValue (HWND_DESKTOP,
                                            sysvals[usRow].sIndex)),
                                   usRow, 0, hvps) ;

               hwndHscroll = WinWindowFromID (
                                   WinQueryWindow (hwnd, QW_PARENT),
                                   FID_HORZSCROLL) ;

               WinSendMsg (hwndHscroll, SBM_SETSCROLLBAR,
                                        MPFROM2SHORT (sHscrollPos, 0),
                                        MPFROM2SHORT (0, MAXWIDTH - 1)) ;

               hwndVscroll = WinWindowFromID (
                                   WinQueryWindow (hwnd, QW_PARENT),
                                   FID_VERTSCROLL) ;

               WinSendMsg (hwndVscroll, SBM_SETSCROLLBAR,
                                        MPFROM2SHORT (sVscrollPos, 0),
                                        MPFROM2SHORT (0, NUMLINES - 1)) ;
               return 0 ;

          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;

               WinDefAVioWindowProc ((HWND)hwnd, msg, mp1, mp2) ;
               return 0 ;

          case WM_HSCROLL:
               switch (SHORT2FROMMP (mp2))
                    {
                    case SB_LINELEFT:
                         sHscrollPos -= 1 ;
                         break ;

                    case SB_LINERIGHT:
                         sHscrollPos += 1 ;
                         break ;

                    case SB_PAGELEFT:
                         sHscrollPos -= 8 ;
                         break ;

                    case SB_PAGERIGHT:
                         sHscrollPos += 8 ;
                         break ;

                    case SB_SLIDERPOSITION:
                         sHscrollPos = SHORT1FROMMP (mp2) ;
                         break ;
                    }
               sHscrollPos = max (0, min (sHscrollPos, MAXWIDTH - 1)) ;

               if (sHscrollPos != SHORT1FROMMR (WinSendMsg (hwndHscroll,
                                       SBM_QUERYPOS, NULL, NULL)))
                    {
                    VioSetOrg (sVscrollPos, sHscrollPos, hvps) ;

                    WinSendMsg (hwndHscroll, SBM_SETPOS,
                                MPFROM2SHORT (sHscrollPos, 0), NULL) ;
                    }
               return 0 ;

          case WM_VSCROLL:
               switch (SHORT2FROMMP (mp2))
                    {
                    case SB_LINEUP:
                         sVscrollPos -= 1 ;
                         break ;

                    case SB_LINEDOWN:
                         sVscrollPos += 1 ;
                         break ;

                    case SB_PAGEUP:
                         sVscrollPos -= cyClient / cyChar ;
                         break ;

                    case SB_PAGEDOWN:
                         sVscrollPos += cyClient / cyChar ;
                         break ;

                    case SB_SLIDERPOSITION:
                         sVscrollPos = SHORT1FROMMP (mp2) ;
                         break ;
                    }
               sVscrollPos = max (0, min (sVscrollPos, NUMLINES - 1)) ;

               if (sVscrollPos != SHORT1FROMMR (WinSendMsg (hwndVscroll,
                                       SBM_QUERYPOS, NULL, NULL)))
                    {
                    VioSetOrg (sVscrollPos, sHscrollPos, hvps) ;

                    WinSendMsg (hwndVscroll, SBM_SETPOS,
                                MPFROM2SHORT (sVscrollPos, 0), NULL) ;
                    }
               return 0 ;

          case WM_CHAR:
               switch (CHARMSG(&msg)->vkey)
                    {
                    case VK_LEFT:
                    case VK_RIGHT:
                         return WinSendMsg (hwndHscroll, msg, mp1, mp2) ;
                    case VK_UP:
                    case VK_DOWN:
                    case VK_PAGEUP:
                    case VK_PAGEDOWN:
                         return WinSendMsg (hwndVscroll, msg, mp1, mp2) ;
                    }
               break ;

          case WM_PAINT:
               WinBeginPaint (hwnd, hps, NULL) ;
               GpiErase (hps) ;

               VioShowBuf (0, MAXWIDTH * NUMLINES * 2, hvps) ;

               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               VioAssociate (0, hvps) ;
               VioDestroyPS (hvps) ;
               GpiDestroyPS (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
}
