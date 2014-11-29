/*****************************************************************************/
/*                             VERSION 04/17/90                              */
/*                                                                           */
/*  The base code for reading in the bitmap (i.e. the LoadBitmapFile()       */
/*  routine) is a combination of my code and Rick Chapman's, John Clegg's,   */
/*  Ralph Yozzo's, and Richard Redpath's.  The rest of the code is my own.   */
/*                                                                           */
/*  I have tried to keep this stuff fairly bug-free, but this program was    */
/*  written in my spare time, mainly to demonstrate some things that can be  */
/*  done, so therefore certain error checking and other "product level"      */
/*  coding styles may not be fully adhered to.                               */
/*                                                                           */
/*                       -  Keith Bernstein                                  */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
#define INCL_PM
#define INCL_GPIBITMAPS
#define INCL_DOSFILEMGR
#define INCL_DOSINFOSEG
#define INCL_DOSMEMMGR
#define INCL_DOSMISC
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES

#include <os2.h>                        /* PM header file               */
#include <stdlib.h>                     /* "max()" definition.          */
#include <stdio.h>                      /* "fopen()" definition.        */
#include <string.h>                     /* C/2 string functions         */
#include <process.h>                    /* _beginthread prototype.      */
#include <ctype.h>                      /* toupper prototype            */
#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>
#include "standard.h"                   /* Standard definitions for GBM     */
#include "gbm.h"                        /* For GBM interfaces               */
#include "bitmap.h"                     /* Contants, typedefs, & prototypes */
#include "bitmaprc.h"                   /* Resource symbolic identifiers    */

#define FRAME_LIST (FCF_TITLEBAR | FCF_SYSMENU | FCF_MINMAX | FCF_MENU)

CHAR szString [STRINGLENGTH];
CHAR *szTitleBar = "Bitmap File Displayer";
CHAR *pOptions="errok";
UCHAR PathBuf[160];
UCHAR SlideBuf[160];
UCHAR ErrorBuf[MAXPATH + 100];
BOOL bScaleWhileDrawing = FALSE;
BOOL bStopSlide;
BOOL bPaletteManager = TRUE;
BOOL bPaletteSupport;
BOOL bIgnoreErrors = FALSE;
BOOL bNewPalette = FALSE;
BOOL bBW;
BOOL bKeepProportion = TRUE;
BOOL bFrameEmpty = FALSE;
ULONG backcolor;
ULONG forecolor;
HWND hClient;
HWND hFrame;
HWND hClient;
HWND hwndTitleBar;
HWND hwndSysMenu;
HWND hwndMinMax;
HWND hwndAppMenu;
QMSG qmsg;
HDC hdcMem;
HDC hdcScreen;
HPS hpsWindow;
HPS hpsMem;                                  \
HPAL hsyspal = 0;
USHORT GlobalBitCount;
ULONG  Globalx;
ULONG  Globaly;
HBITMAP hbmMem;
UCHAR *pFname;
BOOL bNewImage;
HMTX hUpdateSem;
HEV  hStopWaitSem;
HEV  hPauseSem;
ULONG posts;
BOOL Paused = FALSE;
PFNWP pFrameWndProc;
USHORT Direction;
HPS    hpsCurrent;          /* Used for "in-memory" transformations*/
HDC    hdcCurrent;          /* Used for "in-memory" transformations*/
HBITMAP hbmCurrent;         /* Used for "in-memory" transformations*/
PSZ dcdatablk[9] = {0,
                    "MEMORY", /* display driver      */
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0
                   };


/*****************************************************************************/
/*  Abstract for function: SlideThread()                                       */
/*    This function will keep on reading in the .BMP files listed in the     */
/*    .SLD file until bStopSlide is TRUE.                                      */
/*****************************************************************************/
void SlideThread(PVOID pFile)
{
   HAB hAB = WinInitialize(0);
   HMQ hmq = WinCreateMsgQueue(hAB, 0);    /* Create a message queue       */
   UCHAR *pTime;
   UCHAR *pErrorString;
   LONG TimeOutAmount;
   LONG DefaultDelay = SEM_INDEFINITE_WAIT;
   FILE *hSlideFile;
   PLINKNODE pHeadNode;
   PLINKNODE pCurrNode;
   HWND hMenu;
   HWND hUpdateWnd;
   ULONG CurrChecked;
   ULONG TempShort;
   SHORT ErrorCode = 0;
   PSZ   pFileName = &SlideBuf[0];

   if (!(hSlideFile = fopen(SlideBuf, "r"))) {
      ErrorCode = LBF_COULDNT_OPEN_FILE_RC;
      strcpy((PSZ)ErrorBuf, "Error loading file:  <");
      strcat((PSZ)ErrorBuf, pFileName);
      strcat((PSZ)ErrorBuf, ">");
      pErrorString = ErrorBuf;
   } else if (pHeadNode = PutFileInLList(hSlideFile)) {
      pCurrNode = pHeadNode;
      fclose(hSlideFile);
      Direction = FORWARD;
      /* Enable menu items which may now be used, and disable ones which */
      /* are now invalid... */
      hMenu = FixupMenus(hFrame, TRUE);
      /* Check the "Forward" choice... */
      WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(ID_FORWARD, TRUE),
                 MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
      CurrChecked = ID_FORWARD;
      while (!bStopSlide) {
         strcpy(PathBuf, pCurrNode->pString);
         /* See if they want to terminate or change defaults... */
         if (PathBuf[0] == '/') {
            if (!PathBuf[1]) {
               /* Then terminate... */
               bStopSlide = TRUE;
           } else {
              if ( (PathBuf[1] == 'I') || (PathBuf[1] == 'i') ) {
                 DefaultDelay = SEM_INDEFINITE_WAIT;
              } else {
                 DefaultDelay = atol(&PathBuf[1]) * 1000;
              }
           }
         } else {
            /* Now see if they've specified a display time parameter... */
            pTime = strrchr(PathBuf, '/');
            if (pTime) {
               if ( (pTime[1] == 'I') || (pTime[1] == 'i') ) {
                  TimeOutAmount = SEM_INDEFINITE_WAIT;
               } else {
                  TimeOutAmount = atol(pTime + 1) * 1000;
               }
               /* Now remove this from filename string... */
               *pTime = '\0';
            } else {
               TimeOutAmount = DefaultDelay;
            }
            /* Remove white space or open may fail! */
            RemoveWhiteSpace(PathBuf);

            /* Wait 'till we're done drawing the old one before loading a */
            /* new one!! */
            DosRequestMutexSem(hUpdateSem, SEM_INDEFINITE_WAIT);
            TempShort = LoadBitmapFile(PathBuf, (PHAB)&hAB,
                                       (PHDC)&hdcMem, (PHPS)&hpsMem,
                                       (PHBITMAP)&hbmMem, FALSE);
            if (!TempShort) {
               /* Let main thread do this so we for WinXXX and "C" calls! */
               bNewImage = TRUE;
               /* Now get window to update, see if we are iconic or not... */
               if (WinQueryWindowULong(hFrame, QWL_STYLE) & WS_MINIMIZED) {
                  hUpdateWnd = hFrame;
               } else {
                  hUpdateWnd = hClient;
               }
               WinInvalidateRect (hUpdateWnd, (PRECTL) NULL, FALSE);
               DosReleaseMutexSem(hUpdateSem);
               WinUpdateWindow(hUpdateWnd);
               /* Don't display the title until the window is up!! */
               DosRequestMutexSem(hUpdateSem, SEM_INDEFINITE_WAIT);
               DosReleaseMutexSem(hUpdateSem);
               ResetWindowText(hFrame, PathBuf, GlobalBitCount, Globalx, Globaly);
               /* Let's show the picture for the length of time they've */
               /* specified, *unless* this semaphore gets cleared by a key */
               /* or button press, in which case we'll stop waiting. */
               DosResetEventSem(hStopWaitSem, &posts);
               DosWaitEventSem(hStopWaitSem, TimeOutAmount);
               DosResetEventSem(hStopWaitSem, &posts);
               /* Now let's see if they've paused it!! */
               DosWaitEventSem(hPauseSem, SEM_INDEFINITE_WAIT);
               /* Do this in two steps to make sure hStopWaitSem is still */
               /* currently set (since FORWARD, etc. get out of pause and */
               /* reset it!). */
               if (Paused) {
                  Paused = FALSE;
                  /* Uncheck the option... */
                  WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(ID_PAUSE, TRUE),
                             MPFROM2SHORT(MIA_CHECKED, NULL));
               }
            } else {
               DosReleaseMutexSem(hUpdateSem);
               ErrorCode = TempShort;
               if (TempShort == LBF_COULDNT_OPEN_FILE_RC) {
                  strcpy(ErrorBuf, "Error loading file:  <");
                  strupr(PathBuf);
                  strcat(ErrorBuf, PathBuf);
                  strcat(ErrorBuf, ">");
                  pErrorString = ErrorBuf;
               } else {
                  pErrorString = (UCHAR *)NULL;
               }
            }
         }
         switch (Direction) {
            case FORWARD:
               pCurrNode = pCurrNode->pNext;
               if (CurrChecked == ID_REVERSE) {
                  CurrChecked = ID_FORWARD;
                  WinSendMsg(hMenu, MM_SETITEMATTR,
                             MPFROM2SHORT(ID_FORWARD, TRUE),
                             MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
                  WinSendMsg(hMenu, MM_SETITEMATTR,
                             MPFROM2SHORT(ID_REVERSE, TRUE),
                             MPFROM2SHORT(MIA_CHECKED, NULL));
               }
               break;
            case REVERSE:
               pCurrNode = pCurrNode->pPrev;
               if (CurrChecked == ID_FORWARD) {
                  CurrChecked = ID_REVERSE;
                  WinSendMsg(hMenu, MM_SETITEMATTR,
                             MPFROM2SHORT(ID_REVERSE, TRUE),
                             MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
                  WinSendMsg(hMenu, MM_SETITEMATTR,
                             MPFROM2SHORT(ID_FORWARD, TRUE),
                             MPFROM2SHORT(MIA_CHECKED, NULL));
               }
               break;
            case RESTART:
               pCurrNode = pHeadNode;
               Direction = FORWARD;
               break;
         }
      }
      WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(CurrChecked, TRUE),
                 MPFROM2SHORT(MIA_CHECKED, NULL));
      FixupMenus(hFrame, FALSE);
      FreeLList(pHeadNode);
   }
   if (ErrorCode) {
      ShowErrors(ErrorCode, pErrorString);
   }
   bStopSlide = TRUE;
   WinCancelShutdown(hmq, 0);
   WinDestroyMsgQueue(hmq);
   WinPostMsg(hClient, WM_COMMAND, MPFROMSHORT(ID_CLEAR_BITMAP), (MPARAM)NULL);
}



HAB  hAB;
HMQ  hmq;

int main(int argc, char *argv[])
{
   SHORT TempShort;
   ULONG ParamBits;
   RECTL InitialSize;
   ULONG flCreate;                       /* Window creation control flags*/

   hAB = WinInitialize( 0 );             /* Initialize PM                */
   hmq = WinCreateMsgQueue( hAB, 0 );    /* Create a message queue       */

   WinRegisterClass(                     /* Register window class        */
      hAB,                               /* Anchor block handle          */
      "MyWindow",                        /* Window class name            */
      (PFNWP)MyWindowProc,               /* Address of window procedure  */
      CS_SIZEREDRAW,                     /* Class Style                  */
      0                                  /* No extra window words        */
      );

   flCreate = FCF_STANDARD & (~FCF_TASKLIST);


   hFrame = WinCreateStdWindow(
               HWND_DESKTOP,            /* Desktop window is parent     */
               0,                       /* Set frame styles to standard */
               (PVOID)&flCreate,        /* Frame control flag           */
               "MyWindow",              /* Client window class name     */
               szTitleBar,              /* Fill in after creation for TM*/
               0L,                      /* No special class style       */
               (HMODULE)NULL,           /* Resource is in .EXE file     */
               ID_WINDOW,               /* Frame window identifier      */
               &hClient                 /* Client window handle         */
               );

   /* This subclassing is necessary so we can get paint messages when iconic. */
/*   pFrameWndProc = WinSubclassWindow(hFrame, FilterProc); */

   /* This is sort of a PM oddity, but if we don't set the style to */
   /* WS_VISIBLE, then PM won't give us our default coords., HOWEVER, if we */
   /* don't specify WS_VISIBLE, and make it iconic, PM *will* set our  */
   /* restore coordinates to SHELLPOSITION!! So Let's initially show the  */
   /* window as iconic, but with NOREDRAW so it doesn't actually get */
   /* shown (if we don't make it iconic, the client area gets shown!), then */
   /* we will have valid restore coords, in case the user doesn't specify */
   /* all coords on the command line, then we'll show the window!! */
   WinSetWindowPos(hFrame, (HWND)NULL, 0, 0, 0, 0,
                   SWP_MINIMIZE | SWP_SHOW | SWP_NOREDRAW | SWP_DEACTIVATE);
   WinSetWindowPos(hFrame, (HWND)NULL, 0, 0, 0, 0,
                   SWP_HIDE);
   WinSetWindowPos(hFrame, (HWND)NULL, 0, 0, 0, 0,
                   SWP_RESTORE);
   /* Now we are where we want to be, an invisible window at FCF_SHELLPOSITION!*/

   InitialSize.xLeft = WinQueryWindowUShort(hFrame, QWS_XRESTORE);
   InitialSize.xRight = WinQueryWindowUShort(hFrame, QWS_CXRESTORE);
   InitialSize.yBottom = WinQueryWindowUShort(hFrame, QWS_YRESTORE);
   InitialSize.yTop = WinQueryWindowUShort(hFrame, QWS_CYRESTORE);

   ParamBits =  ParseArgs(argc, argv, &pFname, &InitialSize);

   DosCreateMutexSem(NULL,&hUpdateSem,0,0);
   DosCreateEventSem(NULL,&hPauseSem,0,0);
   DosPostEventSem(hPauseSem);
   DosCreateEventSem(NULL,&hStopWaitSem,0,1);

   /* We do this explicitly rather than in our style bits so that the title */
   /* will ALWAYS be szTitleBar, and not what someone may have started */
   /* us with! */
   AddToSwitchList(hFrame, (HWND)NULL, szTitleBar);
   gbm_init();

   if (ParamBits & BITMAP_FILE) {
      TempShort = LoadBitmapFile(pFname, (PHAB)&hAB, (PHDC)&hdcMem,
                                 (PHPS)&hpsMem, (PHBITMAP)&hbmMem, TRUE);
      if (!TempShort) {
         bNewImage = TRUE;
         /* Remember, we haven't assigned the value of 'hFrame' yet!! */
         /* so use the client handle to get the frame's handle... */
         ResetWindowText(hFrame, pFname, GlobalBitCount, Globalx, Globaly);
      } else if (TempShort == LBF_COULDNT_OPEN_FILE_RC) {
         strcpy(ErrorBuf, "Error loading file:  <");
         strcat(ErrorBuf, pFname);
         strcat(ErrorBuf, ">");
         ShowErrors(TempShort, ErrorBuf);
      } else {
         ShowErrors(TempShort, NULL);
      }
   } else if (ParamBits & SLIDE_FILE) {
      bStopSlide = FALSE;
      strcpy(SlideBuf,pFname);
      if (_beginthread(SlideThread, NULL, THREAD_STACKSIZE, (PVOID)pFname)
                       == -1) {
         ShowErrors(NO_SLIDE_THREAD_RC, NULL);
      }
   }

   /* Make sure we have the focus when we come up!! */
   TempShort = (SWP_SHOW | SWP_ACTIVATE);
   if (ParamBits & SWP_SIZE) {
      TempShort |= (SWP_MOVE | SWP_SIZE);
   }

   if (ParamBits & SWP_MINIMIZE) {
      /* This must be done before any possible moves!! */
      WinSetWindowPos(hFrame, (HWND)NULL, 0, 0, 0, 0, SWP_MINIMIZE);
      TempShort &= (~(SWP_SIZE));
   } else if (ParamBits & SWP_MAXIMIZE) {
      TempShort &= (~(SWP_SIZE));
      WinSetWindowPos(hFrame, (HWND)NULL, 0, 0, 0, 0, SWP_MAXIMIZE);
   }

   WinSetWindowPos(hFrame, (HWND)NULL, (SHORT)InitialSize.xLeft,
                   (SHORT)InitialSize.yBottom, (SHORT)InitialSize.xRight,
                   (SHORT)InitialSize.yTop, TempShort);

   WinSendMsg(WinWindowFromID(hFrame, FID_MENU), MM_SETITEMATTR,
              MPFROM2SHORT(ID_SCALE_WHILE_DRAW, TRUE),
              MPFROM2SHORT(MIA_CHECKED, bScaleWhileDrawing ? 0 : MIA_CHECKED));
   WinSendMsg(WinWindowFromID(hFrame, FID_MENU), MM_SETITEMATTR,
              MPFROM2SHORT(ID_PALETTE_MANAGEMENT, TRUE),
              MPFROM2SHORT(MIA_CHECKED, bPaletteManager ? MIA_CHECKED : 0));
   WinSendMsg(WinWindowFromID(hFrame, FID_MENU), MM_SETITEMATTR,
              MPFROM2SHORT(ID_KEEP_PROPORTION, TRUE),
              MPFROM2SHORT(MIA_CHECKED, bKeepProportion ? MIA_CHECKED : 0));


   hwndTitleBar =  WinWindowFromID( hFrame, (USHORT)FID_TITLEBAR);
   hwndSysMenu  =  WinWindowFromID( hFrame, (USHORT)FID_SYSMENU );
   hwndMinMax   =  WinWindowFromID( hFrame, (USHORT)FID_MINMAX  );
   hwndAppMenu  =  WinWindowFromID( hFrame, (USHORT)FID_MENU    );
   if (bFrameEmpty) {
      bFrameEmpty = !bFrameEmpty;
      WinSendMsg(hClient, WM_COMMAND, MPFROMSHORT(ID_FRAME_EMPTY), NULL);
   } /* endif */

   while( WinGetMsg( hAB, (PQMSG)&qmsg, (HWND)NULL, 0, 0 ) ) {
           WinDispatchMsg( hAB, (PQMSG)&qmsg );
   }

   /* Our resources should get freed automatically, but let's just be sure! */
   RemoveBitmapFromMem((PHDC)&hdcMem, (PHPS)&hpsMem, (PHBITMAP)&hbmMem);
   RemoveBitmapFromMem((PHDC)&hdcCurrent, (PHPS)&hpsCurrent,
                       (PHBITMAP)&hbmCurrent);
   WinDestroyWindow(hFrame);           /* Tidy up...                   */
   WinDestroyMsgQueue(hmq);            /* and                          */
   return(WinTerminate(hAB));          /* terminate the application    */
}

/*#if 0*/
PVOID MakeRLEBitmap(HBITMAP hbmMem, HPS hpsMem, HAB hab)
{
   BITMAPINFOHEADER2 bmih;
   PBITMAPINFO2 pbmih = NULL;
   ULONG scansize, tsize;
   LONG bmisize;
   PVOID  data;
   LONG   scans;

   bmih.cbFix = sizeof(bmih);
   GpiQueryBitmapInfoHeader(hbmMem, &bmih);
   scansize = ((bmih.cBitCount*bmih.cx+31)/32)*bmih.cPlanes*4;
   bmisize = (bmih.cbFix+(sizeof(RGB2))*
                 (bmih.cclrUsed ? bmih.cclrUsed :
                                  2<<(bmih.cBitCount*bmih.cPlanes) ) );
   tsize = bmisize+scansize*bmih.cy;
   if ( DosAllocMem((PVOID)&pbmih, tsize, PAG_COMMIT+OBJ_TILE+PAG_WRITE)) {
       return FALSE;
   } /* endif */
   data = (PVOID)pbmih;
   memcpy(data, (PVOID)&bmih, bmih.cbFix);
   pbmih->cBitCount = bmih.cBitCount*bmih.cPlanes;
   pbmih->cPlanes = 1;
   pbmih->cbImage = scansize*bmih.cy;
   pbmih->ulCompression = (pbmih->cBitCount == 4 ? BCA_RLE4 :
                           pbmih->cBitCount == 8 ? BCA_RLE8 : BCA_UNCOMP);
   scans = GpiQueryBitmapBits(hpsMem, 0, bmih.cy, ((PSZ)data)+bmisize, pbmih);
   if (GPI_ALTERROR == scans) {
      data = NULL;
      tsize = pbmih->cbImage*3/2;
      while (  tsize < 2*scansize*bmih.cy) {
         DosFreeMem(pbmih);
         if ( DosAllocMem((PVOID)&pbmih, tsize+bmisize,
                          PAG_COMMIT+OBJ_TILE+PAG_WRITE)) {
             return FALSE;
         } /* endif */
         memcpy((PVOID)pbmih, (PVOID)&bmih, bmih.cbFix);
         pbmih->cBitCount = bmih.cBitCount*bmih.cPlanes;
         pbmih->cPlanes = 1;
         pbmih->cbImage = tsize;
         pbmih->ulCompression = (pbmih->cBitCount == 4 ? BCA_RLE4 :
                                 pbmih->cBitCount == 8 ? BCA_RLE8 : BCA_UNCOMP);
         scans = GpiQueryBitmapBits(hpsMem, 0, bmih.cy, ((PSZ)pbmih)+bmisize, pbmih);
         if (GPI_ALTERROR != scans) {
            data = (PVOID)pbmih;
            break;
         } /* endif */
      } /* endwhile */
   } /* endif */
   return data;
}

ULONG WriteBitmap(PVOID bm, PSZ filename, USHORT type, USHORT xhot, USHORT yhot)
{
   PBITMAPINFO2 pbmih = (PBITMAPINFO2)bm;
   PBITMAPFILEHEADER2 pbmfh = NULL;
   ULONG  bmihsize, bmisize, bmfhsize, ctabsize;
   PVOID   bits;
   PSZ     tptr;
   ULONG   result = FALSE;
   HFILE   hfile;
   ULONG   action, written;

   bmisize = (pbmih->cbFix+(sizeof(RGB2))*
               (pbmih->cclrUsed ? pbmih->cclrUsed :
                                  2<<(pbmih->cBitCount*pbmih->cPlanes) ) );
   bits = (PVOID)((PSZ)pbmih + bmisize);
   if (pbmih->ulIdentifier) {
      bmihsize = sizeof(*pbmih);
   } else if (pbmih->ulColorEncoding) {
      bmihsize = FIELDOFFSET(BITMAPINFOHEADER2, ulIdentifier);
   } else if (pbmih->cSize2) {
      bmihsize = FIELDOFFSET(BITMAPINFOHEADER2, ulColorEncoding);
   } else if (pbmih->cSize1) {
      bmihsize = FIELDOFFSET(BITMAPINFOHEADER2, cSize2);
   } else if (pbmih->usRendering) {
      bmihsize = FIELDOFFSET(BITMAPINFOHEADER2, cSize1);
   } else if (pbmih->usRecording) {
      bmihsize = FIELDOFFSET(BITMAPINFOHEADER2, usRendering);
   } else if (pbmih->usUnits) {
      bmihsize = FIELDOFFSET(BITMAPINFOHEADER2, usRecording);
   } else if (pbmih->cclrImportant) {
      bmihsize = FIELDOFFSET(BITMAPINFOHEADER2, usUnits);
   } else {
      bmihsize = FIELDOFFSET(BITMAPINFOHEADER2, cclrImportant);
   } /* endif */
   ctabsize = ((sizeof(RGB2))* (pbmih->cclrUsed ? pbmih->cclrUsed :
                               1<<(pbmih->cBitCount*pbmih->cPlanes) ) );
   bmfhsize = FIELDOFFSET(BITMAPFILEHEADER2, bmp2) + bmihsize + ctabsize;
   if (DosAllocMem((PVOID)&pbmfh, bmfhsize, PAG_COMMIT+OBJ_TILE+PAG_WRITE)) {
      return FALSE;
   } /* endif */
   pbmfh->usType   = (USHORT)(type ? type : BFT_BMAP);
/*   pbmfh->cbSize   = bmfhsize+pbmfh->bmp2.cbImage;*/
   pbmfh->cbSize   = FIELDOFFSET(BITMAPFILEHEADER2, bmp2) + bmihsize;
   pbmfh->xHotspot = xhot;
   pbmfh->yHotspot = yhot;
   pbmfh->offBits  = bmfhsize;
   tptr = (PSZ)&(pbmfh->bmp2);
   memcpy(tptr, (PVOID)bm, bmihsize);
   (pbmfh->bmp2).cbFix = bmihsize;
   tptr += bmihsize;
   memcpy(tptr, (PVOID)&(pbmih->argbColor), ctabsize);
   if (!DosOpen(filename, &hfile, &action, bmfhsize+pbmfh->bmp2.cbImage,
               FILE_NORMAL, FILE_TRUNCATE | FILE_CREATE,
               OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYREADWRITE |
                 OPEN_FLAGS_NOINHERIT,
               0 )) {
      if (DosWrite(hfile, pbmfh, bmfhsize, &written) ||
                 written != bmfhsize) {
         DosClose(hfile);
         DosDelete(filename);
      } else if (DosWrite(hfile, bits, pbmfh->bmp2.cbImage, &written) ||
                 written != pbmfh->bmp2.cbImage) {
         DosClose(hfile);
         DosDelete(filename);
      } else {
         result = TRUE;
         DosClose(hfile);
      } /* endif */
   } /* endif */
   DosFreeMem(pbmfh);
   return result;
}
/*#endif*/

PSZ Editname(PSZ source)
{
static ULONG maxfile;
   PSZ       tptr, outfile = NULL;
   ULONG     rc;

   if (!maxfile) {
      if (rc = DosQuerySysInfo(QSV_MAX_PATH_LENGTH, QSV_MAX_PATH_LENGTH,
                                       &maxfile, sizeof(maxfile)) ) {
         exit(rc);
      } /* endif */
   } /* endif */
   if (!outfile && !(outfile = malloc(maxfile))) {
      exit(25);
   } /* endif */
   strcpy(outfile, source);
   tptr = outfile + strlen(outfile) - 1;
   while (tptr>outfile) {
      if (*tptr == '\\' || *tptr == ':' ) {
         break;
      } /* endif */
      tptr--;
   } /* endwhile */
   ++tptr;
   if (rc = DosEditName(1, source+(tptr-outfile), "*.RLE",
                                tptr, maxfile-(tptr-outfile)) ) {
      exit(rc);
   } /* endif */
   if (':' == *(outfile+1)) {
      DosSetDefaultDisk(toupper(*(outfile+1))-'A');
      if (tptr>outfile+2) {
         *--tptr = '\0';
         DosSetCurrentDir(outfile+2);
         *tptr = '\\';
      } /* endif */
   } else {
      if (tptr>outfile) {
         *--tptr = '\0';
         DosSetCurrentDir(outfile);
         *tptr = '\\';
      } /* endif */
   } /* endif */

   return (PSZ)strdup(outfile);
}

PSZ copyext(PSZ exts)      /* Copy an extension from a blank separated list   */
{
   UCHAR temp[30];
   int   i = 0;

   while (exts[i] && exts[i] != ' ') {
      temp[i] = exts[i];
      i++;
   } /* endwhile */
   temp[i] = 0;
   return strdup(temp);
}


/* This is the file filter to allow only files with the supported bitmap/image   */
/*    extensions into the file dialog.                                           */
MRESULT EXPENTRY FileDlgFilter(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static PSZ types[100];                 /* Keep the supported extensions here.    */
static BOOL firsttime = TRUE;          /* We only want to obtain the extension   */
                                       /*    list one time.                      */
static ULONG typecount = 0;            /* Number of extensions we support.       */
   ULONG i;
   int count;
   PSZ tptr;

   if (firsttime) {                    /* First time through we query GBM for    */
      firsttime = FALSE;               /*    supported file types and build up   */
      for (i=0; i<100; i++) {          /*    our table.                          */
         types[i] = NULL;
      } /* endfor */
      if (!gbm_query_n_filetypes(&count)) {
         for (i=0; i<count; i++) {
            GBMFT gbmft;

            if (!gbm_query_filetype(i, &gbmft)) {
               PSZ ptr = gbmft.extensions;

               while (ptr && *ptr && *ptr != ' ') {
                  types[typecount++] = copyext(ptr);
                  while (*ptr && *ptr != ' ')ptr++;
                  if (*ptr) ptr++;
               } /* endwhile */
            } /* endif */
         } /* endfor */
      } /* endif */
   } /* endif */

   if (FDM_FILTER == msg) {            /* Process the FDM_FILTER message to      */
      tptr = (PSZ)mp1;                 /*    filter out the unwanted file types. */
      tptr += strlen(tptr) - 1;
      while (tptr>(PSZ)mp1 && *tptr != '.') {     /* Locate an extension! Reject if   */
                                             /*    can't find one.               */
         if (*tptr == '\\' || *tptr == ':') return (MRESULT)FALSE;
         tptr--;
      } /* endwhile */
      if (*tptr != '.') {
         return (MRESULT)FALSE;
      } /* endif */
      tptr++;
      for (i=0; i<typecount; i++) {          /* Now compare the extension to our */
         if (!stricmp(tptr,types[i])) {      /*    supported extension list.     */
            return (MRESULT)TRUE;            /* Return TRUE if it is supported!  */
         } /* endif */
      } /* endfor */
      return (MRESULT)FALSE;
   } else {
      return WinDefFileDlgProc(hwnd, msg, mp1, mp2);
   } /* endif */
}

typedef struct {
   HWND         hwnd;
   PSZ          name;
   BOOL         bRLE;
} BITINFO;
typedef BITINFO * PBITINFO;

void LoadBitmapThread(PVOID pData)
{
   HAB          hABt = WinInitialize(0);
   HMQ          hmqt = WinCreateMsgQueue(hABt, 0);    /* Create a message queue       */
   PBITINFO     pb = (PBITINFO)pData;
   SHORT        TempShort;

   /* Wait 'till we're done drawing the old one before loading a */
   /* new one!! */
   DosRequestMutexSem(hUpdateSem, SEM_INDEFINITE_WAIT);
   TempShort = LoadBitmapFile(pb->name, (PHAB)&hABt,
                              (PHDC)&hdcMem, (PHPS)&hpsMem,
                              (PHBITMAP)&hbmMem, TRUE);
   DosReleaseMutexSem(hUpdateSem);
   if (!TempShort) {
     bNewImage = TRUE;
     /* Now get window to update... */
     WinInvalidateRect (pb->hwnd, (PRECTL) NULL, FALSE);
     WinUpdateWindow(pb->hwnd);
     ResetWindowText(hFrame, pb->name, GlobalBitCount, Globalx, Globaly);
   } else if (TempShort == LBF_COULDNT_OPEN_FILE_RC) {
     strcpy(ErrorBuf, "Error loading file:  <");
     strcat(ErrorBuf, pb->name);
     strcat(ErrorBuf, ">");
     ShowErrors(TempShort, ErrorBuf);
   } else {
     ShowErrors(TempShort, NULL);
   }
   free(pb->name);
   free(pb);
   WinDestroyMsgQueue(hmqt);
   WinTerminate(hABt);
}


ULONG SaveBitmapFile(PSZ lpFileName, PHAB phAB, PHDC phdcMem,
                      PHPS phpsMem, PHBITMAP phbmMem,
                      BOOL bShowWaitPointer)
{
  ULONG             ReturnVal   = 0  ;
  BITMAPINFOHEADER  bih              ;
  int               ft               ;
  int               fd               ;
  GBM               gbm              ;
  GBMFT             gbmft            ;
  GBMRGB            gbmrgb [0x100]   ;
  BYTE             *pbData = NULL    ;
  PSZ               opts             ;
  struct {
     BITMAPINFOHEADER2 bmp2;
     RGB2              rgb[0x100];
  } bm;

#define GBM_WRITE (GBM_FT_W1 || GBM_FT_W4 || GBM_FT_W8 || GBM_FT_W24)

static ULONG errtab[9] = {
   0, SBF_NO_MEMORY, SBF_ERROR_WRITE_FORMAT, SBF_ERROR_WRITE_FORMAT,
   SBF_ERROR_WRITE_FORMAT, SBF_ERROR_WRITE_FORMAT, SBF_ERROR_WRITE_FORMAT,
   SBF_ERROR_WRITE_FORMAT, SBF_ERROR_WRITE_FORMAT
};


  if ((opts = strchr(lpFileName, ',')) != NULL ) {
     *opts++ = '\0';
  } else {
     opts = "";
  } /* endif */
  if (bShowWaitPointer) {
    WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP,
                                                   SPTR_WAIT, FALSE));
  }

  /* First remove all leading and trailing white space, or this call */
  /* will fail!! */
  RemoveWhiteSpace(lpFileName);
  bih.cbFix = sizeof(bih);
  /* Now check the filetype! */
  if (gbm_guess_filetype(lpFileName,&ft) || gbm_query_filetype(ft, &gbmft) ) {
     ReturnVal = SBF_UNKNOWN_FORMAT_RC;
  } else {
     bm.bmp2.cbFix = sizeof(BITMAPINFOHEADER2);
     if (!(gbmft.flags && GBM_WRITE)) {
        ReturnVal = SBF_FORMAT_NO_WRITE;
     } else if (!GpiQueryBitmapParameters(*phbmMem, &bih)) {
        ReturnVal = SBF_ERROR_GETTING_BITMAP_INFO;
     } else {
#ifdef DEBUG
        printf("Checking for bits - %u.\n",bih.cPlanes*bih.cBitCount);
#endif
        switch (bih.cPlanes * bih.cBitCount) {
        case 1:
           ReturnVal = !(gbmft.flags & GBM_FT_W1);
           break;
        case 4:
           ReturnVal = !(gbmft.flags & GBM_FT_W4);
           break;
        case 8:
           ReturnVal = !(gbmft.flags & GBM_FT_W8);
           break;
        case 24:
           ReturnVal = !(gbmft.flags & GBM_FT_W24);
           break;
        default:
           ReturnVal = TRUE;
          break;
        } /* endswitch */
        if (ReturnVal) {
           ReturnVal = SBF_FORMAT_MISMATCH;
        } else if (!GpiQueryBitmapInfoHeader(*phbmMem, (PBITMAPINFOHEADER2)&bm)) {
           ReturnVal = SBF_ERROR_GETTING_BITMAP_INFO;
        } else {
           bm.bmp2.ulCompression   = 0;
           bm.bmp2.usReserved      = 0;
           bm.bmp2.usRecording     = 0;
           bm.bmp2.usRendering     = 0;
           bm.bmp2.ulColorEncoding = 0;
           if (bm.bmp2.cBitCount == 1 && bm.bmp2.cPlanes == 4) {
              bm.bmp2.cBitCount = 4;
              bm.bmp2.cPlanes   = 1;
           } /* endif */
           gbm.w = bm.bmp2.cx;
           gbm.h = bm.bmp2.cy;
           gbm.bpp = bm.bmp2.cBitCount;
#ifdef DEBUG
           printf("Bitcount = %u.\n",bm.bmp2.cBitCount);
#endif
           if (!(pbData = malloc(((bm.bmp2.cBitCount*gbm.w+31)/32)*4))  ) {
              ReturnVal = SBF_NO_MEMORY;
           } else if (!GpiQueryBitmapBits(*phpsMem, 0, 1, pbData, (PBITMAPINFO2)&bm) ) {
              ReturnVal = SBF_NO_COLOR_TABLE;
           } else if ((free(pbData), (pbData = malloc(((bm.bmp2.cBitCount*gbm.w+31)/32)*4*gbm.h))) ) {
              if (bm.bmp2.cBitCount != 24) {
                 int ulCol;

                 for (ulCol=0; ulCol<(1<<gbm.bpp); ulCol++) {
                    gbmrgb [ulCol].r = bm.rgb[ulCol].bRed  ;
                    gbmrgb [ulCol].g = bm.rgb[ulCol].bGreen;
                    gbmrgb [ulCol].b = bm.rgb[ulCol].bBlue ;
                 } /* endfor */
              } /* endif */
              if (GpiQueryBitmapBits(*phpsMem, 0, gbm.h, pbData, (PBITMAPINFO2)&bm) ) {
                 if (-1 == (fd = open( lpFileName, O_CREAT | O_TRUNC | O_RDWR | O_BINARY
                                     , S_IREAD | S_IWRITE)) ) {
                    ReturnVal = SBF_ERROR_OPEN_FILE;
                 } else {
                    ReturnVal = errtab[gbm_write( lpFileName, fd, ft, &gbm, (GBMRGB *)gbmrgb
                                                , pbData, opts)];
                 } /* endif */
                 close(fd);
              } else {
                 ReturnVal = SBF_ERROR_GETBITS;
              } /* endif */
           } else {
              ReturnVal = SBF_NO_MEMORY;
           } /* endif */
           if (pbData) {
              free(pbData);
           } /* endif */
        } /* endif */
     } /* endif */
  } /* endif */

  return(ReturnVal);
}


void SaveBitmapThread(PVOID pData)
{
   HAB          hABt = WinInitialize(0);
   HMQ          hmqt = WinCreateMsgQueue(hABt, 0);    /* Create a message queue       */
   PBITINFO     pb = (PBITINFO)pData;
   SHORT        TempShort;

   /* Wait 'till we're done drawing the old one before loading a */
   /* new one!! */
   DosRequestMutexSem(hUpdateSem, SEM_INDEFINITE_WAIT);
   TempShort = SaveBitmapFile(pb->name, (PHAB)&hABt,
                              (PHDC)&hdcMem, (PHPS)&hpsMem,
                              (PHBITMAP)&hbmMem, TRUE);
   DosReleaseMutexSem(hUpdateSem);
   if (TempShort == LBF_COULDNT_OPEN_FILE_RC) {
     strcpy(ErrorBuf, "Error saving bitmap file:  <");
     strcat(ErrorBuf, pb->name);
     strcat(ErrorBuf, ">");
     ShowErrors(TempShort, ErrorBuf);
   } else if (TempShort) {
     ShowErrors(TempShort, NULL);
   }
   free(pb->name);
   free(pb);
   WinDestroyMsgQueue(hmqt);
   WinTerminate(hABt);
}

/*****************************************************************************/
/*  Abstract for function: MyWindowProc()                                    */
/*    This function is the main window function.                             */
/*****************************************************************************/
MRESULT EXPENTRY MyWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  USHORT           command;             /* WM_COMMAND command value     */
  HPS              hps;                 /* Presentation Space handle    */
  RECTL            rc;                  /* Rectangle coordinates.       */
  POINTL           PointStruct;         /* For GpiCharStringAt().       */
  FONTMETRICS      FontBuf;             /* For GpiCharStringAt().       */
  USHORT           CharHeight;          /* For GpiCharStringAt().       */
  HPS              hpsForDrawing;       /* For final BitBlt.            */
  SHORT            TempShort;
  BOOL             bError = FALSE;
  UCHAR            MsgBuf[100];
  POINTL           bmarray [5];
  BITMAPINFOHEADER bmapinfo;
  BOOL             bReturnVal = FALSE;
  FILEDLG          filedlg;
  HWND             hwndDest;
  PVOID            prlebm;

  /* Make this static, so we can check if we really have to do any */
  /* transformations. */
  static SIZEL  NewImageSize;
  /* Make this static since we can only do this once per window invocation. */
  PSZ outfile;
  ULONG cclr;
  SIZEL empty = {0,0};

  switch(msg) {
    case WM_CREATE:
      hClient = hwnd;
      bReturnVal = FALSE;
      hdcScreen = WinOpenWindowDC(hwnd);
      DevQueryCaps(hdcScreen, CAPS_ADDITIONAL_GRAPHICS, 1, (PLONG)&cclr);
      bPaletteSupport = (cclr&CAPS_PALETTE_MANAGER) == CAPS_PALETTE_MANAGER;
      hpsWindow = GpiCreatePS(hAB, hdcScreen, &empty, PU_PELS | GPIT_MICRO | GPIA_ASSOC );
      GpiAssociate(hpsWindow, hdcScreen);
      break;

    case WM_BUTTON1DBLCLK:
      Direction = REVERSE;
      DosPostEventSem(hStopWaitSem);
      DosPostEventSem(hPauseSem);
      break;

    case WM_BUTTON2DBLCLK:
      Direction = FORWARD;
      DosPostEventSem(hStopWaitSem);
      DosPostEventSem(hPauseSem);
      break;

    case WM_BUTTON1DOWN:
    case WM_BUTTON2DOWN:
      if (bFrameEmpty) {
         if (   (WinGetKeyState(HWND_DESKTOP, VK_BUTTON1) & 0x8000)
            && (WinGetKeyState(HWND_DESKTOP, VK_BUTTON2) & 0x8000) ) {
            WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(ID_FRAME_EMPTY), NULL);
         } /* endif */
      } /* endif */
      break;

    case WM_COMMAND:
      command = SHORT1FROMMP(mp1);      /* Extract the command value    */
      switch (command) {
        case ID_LOADFILE:
         memset(&filedlg, 0, sizeof(filedlg));
         filedlg.cbSize          = sizeof(filedlg);
         filedlg.fl              = FDS_CENTER |FDS_OPEN_DIALOG;
         filedlg.pszTitle        = "Source Bitmap Selection";
         filedlg.pfnDlgProc      = FileDlgFilter;
         strcpy(filedlg.szFullFile,"*");
         if (WinFileDlg(HWND_DESKTOP, hwnd, &filedlg) && filedlg.lReturn == DID_OK) {
            PBITINFO pbitinfo = malloc(sizeof(BITINFO));
            /* So we have a filespec... */
            strcpy(PathBuf, filedlg.szFullFile);
            pbitinfo->hwnd = hwnd;
            pbitinfo->name = strdup(PathBuf);
            _beginthread(LoadBitmapThread, NULL
                        , THREAD_STACKSIZE, (PVOID)pbitinfo);
          }
          /* else they cancelled out, so do nothing!! */
          bReturnVal = TRUE;
          break;

       case ID_SAVEFILE:
         memset(&filedlg, 0, sizeof(filedlg));
         filedlg.cbSize          = sizeof(filedlg);
         filedlg.fl              = FDS_CENTER |FDS_OPEN_DIALOG;
         filedlg.pszTitle        = "Save Bitmap Selection";
         filedlg.pfnDlgProc      = FileDlgFilter;
         strcpy(filedlg.szFullFile,"*");
         if (WinFileDlg(HWND_DESKTOP, hwnd, &filedlg) && filedlg.lReturn == DID_OK) {
            PBITINFO pbitinfo = malloc(sizeof(BITINFO));
            strcpy(PathBuf, filedlg.szFullFile);
            pbitinfo->hwnd = hwnd;
            pbitinfo->name = strdup(PathBuf);
            pbitinfo->bRLE = FALSE;
            _beginthread(SaveBitmapThread, NULL
                        , THREAD_STACKSIZE, (PVOID)pbitinfo);
          }
          bReturnVal = TRUE;
          break;

       case ID_SAVERLEFILE:
          if (prlebm = MakeRLEBitmap(hbmMem, hpsMem, hAB)) {
             outfile = Editname(PathBuf);
             if (!(TempShort = WriteBitmap(prlebm, outfile, BFT_BMAP, 0, 0) )) {
                strcpy(ErrorBuf, "Error saving file:  <");
                strcat(ErrorBuf, outfile);
                strcat(ErrorBuf, ">");
                ShowErrors(TempShort, ErrorBuf);
             }
             DosFreeMem(prlebm);
             free(outfile);
          } else {
             strcpy(ErrorBuf, "Error creating RLE format bitmap.");
             ShowErrors(TempShort, ErrorBuf);
          }
          bReturnVal = TRUE;
          break;

        case ID_LOADSLD:
          /* List all .SLD files by default!! */
          memset(&filedlg, 0, sizeof(filedlg));
          filedlg.cbSize          = sizeof(filedlg);
          filedlg.fl              = FDS_CENTER |FDS_OPEN_DIALOG;
          filedlg.pszTitle        = "Load a .SLD (Slide) file for a slide show";
          strcpy(filedlg.szFullFile,SLIDE_TEMPLATE);
         if (WinFileDlg(HWND_DESKTOP, hwnd, &filedlg) && filedlg.lReturn == DID_OK) {
            /* So we have a filespec... */
            strcpy(SlideBuf, filedlg.szFullFile);
            bStopSlide = FALSE;
            DosReleaseMutexSem(hUpdateSem);
            DosPostEventSem(hPauseSem);
            DosPostEventSem(hStopWaitSem);
            if (_beginthread(SlideThread, NULL,
                             THREAD_STACKSIZE, (PVOID)SlideBuf)
                             == -1) {
              ShowErrors(NO_SLIDE_THREAD_RC, NULL);
            }
          }
          bReturnVal = TRUE;
          break;

        case ID_ABOUT:
          WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)StaticBox,
                    (HMODULE)NULL, ABOUTBOX, (PVOID)ABOUTBOX);
          bReturnVal = TRUE;
          break;

        case ID_FRAME_EMPTY:
          if (bFrameEmpty) {
             hwndDest = hFrame;
          } else {
             hwndDest = HWND_OBJECT;
          } /* endif */
          WinSetParent(hwndTitleBar, hwndDest, FALSE);
          WinSetParent(hwndSysMenu , hwndDest, FALSE);
          WinSetParent(hwndMinMax  , hwndDest, FALSE);
          WinSetParent(hwndAppMenu , hwndDest, FALSE);
          WinSendMsg(hFrame, WM_UPDATEFRAME, (MPARAM)FRAME_LIST, NULL);
          bFrameEmpty = !bFrameEmpty;
          bReturnVal = TRUE;
          break;

        case ID_SCALE_WHILE_DRAW:
          bScaleWhileDrawing = !(bScaleWhileDrawing);
          WinSendMsg(WinWindowFromID(hFrame, FID_MENU), MM_SETITEMATTR,
                     MPFROM2SHORT(ID_SCALE_WHILE_DRAW, TRUE),
                     MPFROM2SHORT(MIA_CHECKED, bScaleWhileDrawing ? 0 : MIA_CHECKED));
          /* Let's reflect this change... */
          /* First reset this so if we are sure we load the bitmap into */
          /* memory first if we are in that mode!! */
          bNewImage = TRUE;
          /* Now blank out the screen... */
          WinInvalidateRect(hFrame, (PRECTL)NULL, FALSE);
          /* Now invalidate the client area so we get a WM_PAINT message... */
          WinInvalidateRect(hwnd, (PRECTL)NULL, FALSE);
          bReturnVal = TRUE;
          break;

        case ID_PALETTE_MANAGEMENT:
          bPaletteManager = !(bPaletteManager);
          WinSendMsg(WinWindowFromID(hFrame, FID_MENU), MM_SETITEMATTR,
                     MPFROM2SHORT(ID_PALETTE_MANAGEMENT, TRUE),
                     MPFROM2SHORT(MIA_CHECKED, bPaletteManager ? MIA_CHECKED : 0));
          WinInvalidateRect(hwnd, (PRECTL)NULL, FALSE);
          GpiSelectPalette(hpsWindow, bPaletteManager ? hsyspal : 0);
          WinPostMsg( hClient, WM_REALIZEPALETTE, 0L, 0L );
          WinBroadcastMsg( HWND_DESKTOP, WM_REALIZEPALETTE, 0L, 0L, BMSG_POST | BMSG_DESCENDANTS);
          bReturnVal = TRUE;
          break;

        case ID_KEEP_PROPORTION:
          bKeepProportion = !(bKeepProportion);
          WinSendMsg(WinWindowFromID(hFrame, FID_MENU), MM_SETITEMATTR,
                     MPFROM2SHORT(ID_KEEP_PROPORTION, TRUE),
                     MPFROM2SHORT(MIA_CHECKED, bKeepProportion ? MIA_CHECKED : 0));
          WinInvalidateRect(hFrame, (PRECTL)NULL, FALSE);
          WinInvalidateRect(hwnd, (PRECTL)NULL, FALSE);
          bNewImage = TRUE;
          bReturnVal = TRUE;
          break;


        case ID_CLEAR_BITMAP:
          if (hsyspal) {
             GpiSelectPalette(hpsCurrent, 0);
             GpiSelectPalette(hpsMem, 0);
             GpiSelectPalette(hpsWindow, 0);
             GpiDeletePalette(hsyspal);
             WinRealizePalette(hClient, hpsWindow, &cclr);
             WinRealizePalette(hClient, hpsWindow, &cclr);
             WinRealizePalette(hClient, hpsWindow, &cclr);
          } /* endif */
          RemoveBitmapFromMem((PHDC)&hdcMem, (PHPS)&hpsMem, (PHBITMAP)&hbmMem);
          RemoveBitmapFromMem((PHDC)&hdcCurrent, (PHPS)&hpsCurrent,
                              (PHBITMAP)&hbmCurrent);
          /* Reset the window title text... */
          WinSetWindowText(hFrame, "Bitmap File Displayer");
          /* Clear window... */
          WinInvalidateRect(hFrame, (PRECTL)NULL, FALSE);
          bReturnVal = TRUE;
          break;

        case ID_RESTART:
          Direction = RESTART;
          DosPostEventSem(hStopWaitSem);
          DosPostEventSem(hPauseSem);
          break;

        case ID_PAUSE:
          if (Paused) {
            /* Stop pausing... */
            DosPostEventSem(hPauseSem);
          } else {
            /* Start pausing... */
            DosResetEventSem(hPauseSem,&posts);
            Paused = TRUE;
            /* Check the option... */
            WinSendMsg(WinWindowFromID(hFrame, FID_MENU),
                       MM_SETITEMATTR, MPFROM2SHORT(ID_PAUSE, TRUE),
                       MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
          }
          break;

        case ID_TERMINATE:
          if (bStopSlide == FALSE) {
            bStopSlide = TRUE;
            DosPostEventSem(hStopWaitSem);
            DosPostEventSem(hPauseSem);
          }
          break;

        case ID_REVERSE:
          Direction = REVERSE;
          DosPostEventSem(hStopWaitSem);
          DosPostEventSem(hPauseSem);
          break;

        case ID_FORWARD:
          Direction = FORWARD;
          DosPostEventSem(hStopWaitSem);
          DosPostEventSem(hPauseSem);
          break;

        case ID_EXIT:
          WinPostMsg( hwnd, WM_CLOSE, 0L, 0L );
          bReturnVal = TRUE;
          break;

        case ID_RESUME:
          bReturnVal = TRUE;
          break;

        case ID_HELP:
          WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)StaticBox,
                    (HMODULE)NULL, HELPBOX, (PVOID)HELPBOX);
          bReturnVal = TRUE;
          break;

        default:
          bReturnVal = FALSE;
          break;
      }
      break;

    case WM_HELP:
      WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)StaticBox,
                (HMODULE)NULL, HELPBOX, (PVOID)HELPBOX);
      bReturnVal = TRUE;
      break;

    case WM_ERASEBACKGROUND:
      bReturnVal = TRUE;
      break;

    case WM_REALIZEPALETTE:
      if (bPaletteManager && hwnd == hClient && hsyspal) {
         if (WinRealizePalette(hwnd, hpsWindow, &cclr) > 0) {
              WinQueryWindowRect(hwnd, (PRECTL)&rc);
              WinInvalidateRect(hwnd, (PRECTL)&rc, 0);
         } /* endif */
         bReturnVal = TRUE;
      } else {
         bReturnVal = FALSE;
      } /* endif */
      break;

    case WM_PAINT:
      if (hbmMem) {
        DosRequestMutexSem(hUpdateSem, SEM_INDEFINITE_WAIT);
        WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP,
                                                       SPTR_WAIT, FALSE));
        /* Create a presentation space  */
        /* If we are iconic, the PS returned from BeginPaint is no good */
        /* so we will do this special processing... */
        if (WinQueryWindowULong(hFrame, QWL_STYLE) & WS_MINIMIZED) {
          bError = TRUE;
        } else {
/*          ULONG errorpos;
*/
          hps = WinBeginPaint(hwnd, 0, &rc);
          WinQueryWindowRect(hwnd, (PRECTL)&rc);
          if (!GpiSetBackMix(hpsWindow, BM_OVERPAINT)
            || !GpiQueryBitmapParameters(hbmMem, (PBITMAPINFOHEADER)&bmapinfo)) {
            bError = TRUE;
#ifdef DEBUG
            printf("Error at GpiSetBackMix or GpiQueryBitmapParameters.\n");
            printf("Errorid = %x.\n",WinGetLastError(hAB));
#endif
          } else {
            /*******************************************************************/
            /* **** SOURCE **********                                          */
            /*******************************************************************/
            bmarray[2].x = 0;
            bmarray[2].y = 0;
            bmarray[3].x = bmapinfo.cx;
            bmarray[3].y = bmapinfo.cy;

            /*******************************************************************/
            /* **** TARGET **********                                          */
            /*******************************************************************/

            bmarray[0].x = rc.xLeft;
            bmarray[0].y = rc.yBottom;
            bmarray[1].x = rc.xRight;
            bmarray[1].y = rc.yTop;
#ifdef DEBUG
            printf("cx,cy=%d,%d.\n",bmapinfo.cx,bmapinfo.cy);
            printf("xLeft,xRight,yTop,yBottom=%d,%d,%d,%d.\n",rc.xLeft,rc.xRight,rc.yTop,rc.yBottom);
#endif
            if (bKeepProportion) {
               ULONG dx  = bmapinfo.cx;
               ULONG dy  = bmapinfo.cy;
               ULONG dxw = rc.xRight - rc.xLeft;
               ULONG dyw = rc.yTop - rc.yBottom;
#ifdef DEBUG
               printf("dx,dy,dxw,dyw=%d,%d,%d,%d.\n",dx,dy,dxw,dyw);
#endif
               if (dxw*dy>dx*dyw) {
                  ULONG deltax = (dxw) - (dx*dyw)/(dy);
#ifdef DEBUG
                  printf("deltax=%d.\n",deltax);
#endif
                  bmarray[0].x += deltax/2;
                  bmarray[1].x -= deltax/2;
               } else {
                  ULONG deltay = (dyw) - (dy*dxw)/(dx);
#ifdef DEBUG
                  printf("deltay=%d.\n",deltay);
#endif
                  bmarray[0].y += deltay/2;
                  bmarray[1].y -= deltay/2;
               } /* endif */
            } /* endif */

            if (bScaleWhileDrawing) {
              hpsForDrawing = hpsMem;

              /* Make sure we free any bitmaps we had in memory, in case */
              /* user just switched modes on us!!  Also, reset statics, so */
              /* if they switch modes back again, we'll be O.K.!! */
              if (hsyspal) {
                 GpiSelectPalette(hpsCurrent, 0);
              } /* endif */
              RemoveBitmapFromMem((PHDC)&hdcCurrent, (PHPS)&hpsCurrent,
                                  (PHBITMAP)&hbmCurrent);
              NewImageSize.cx = 0L;
              NewImageSize.cy = 0L;
            /* Let's adjust the bitmap's size in the memory PS, so user doesn't */
            /* see it get drawn... */
            } else if ( (NewImageSize.cx != rc.xRight)
                        || (NewImageSize.cy != rc.yTop)
                        || bNewImage ) {
              if (!bNewImage) {
                strcpy(MsgBuf, "Re-scaling bitmap, one moment please...");

                GpiQueryFontMetrics(hpsWindow, (LONG)sizeof(FONTMETRICS),
                                    (PFONTMETRICS)&FontBuf);

                CharHeight = LOUSHORT(FontBuf.lMaxBaselineExt);
                PointStruct.x = 0L;
                PointStruct.y = rc.yTop - CharHeight;
                GpiCharStringAt(hpsWindow, (PPOINTL)&PointStruct,
                                (LONG)strlen(MsgBuf), (PCH)MsgBuf);
              }
              NewImageSize.cx = rc.xRight;
              NewImageSize.cy = rc.yTop;

              if (!hdcCurrent) {
                hdcCurrent = DevOpenDC(hAB, OD_MEMORY, "*", 8L,
                                    (PDEVOPENDATA) dcdatablk, hdcScreen);
              }

              if (!hpsCurrent) {
                hpsCurrent = GpiCreatePS(hAB, hdcCurrent, (PSIZEL)&NewImageSize,
                                      (LONG) PU_PELS | GPIT_NORMAL | GPIA_ASSOC);
              }

              bmapinfo.cx = (USHORT)NewImageSize.cx;
              bmapinfo.cy = (USHORT)NewImageSize.cy;

              if (!hdcCurrent || !hpsCurrent) {
                bError = TRUE;
#ifdef DEBUG
                printf("Error, hdcCurrent or hpsCurrent is NULL!.\n");
#endif
              } else {
                if (hsyspal) {
                   GpiSelectPalette(hpsCurrent, hsyspal);
                } /* endif */
                /* Remove the old bitmap, so we can create a new one with the */
                /* proper dimensions... */
                /* NOTE:  Don't use our bitmap remover function, because we */
                /*        want to keep the hps!! */
                GpiSetBitmap (hpsCurrent, (HBITMAP)NULL);
                GpiDeleteBitmap(hbmCurrent);
                hbmCurrent = GpiCreateBitmap(hpsCurrent, (PBITMAPINFOHEADER2)&bmapinfo,
                                          (ULONG)0, (PBYTE)NULL, (PBITMAPINFO2)NULL);
                if ( (GpiSetBitmap(hpsCurrent, hbmCurrent) == HBM_ERROR)
                     || (GpiBitBlt(hpsCurrent, hpsMem, 4L, (PPOINTL)bmarray,
                                   (LONG) ROP_SRCCOPY,
                                   (LONG) BBO_IGNORE) == GPI_ERROR) ) {
                  bError = TRUE;
#ifdef DEBUG
                  printf("Error from GpiSetBitmap or GpiBitBlt.\n");
                  printf("Errorid = %x.\n",WinGetLastError(hAB));
#endif
                } else {
                  /* Now reset the array... */
                     bmarray[0].x = rc.xLeft;
                     bmarray[0].y = rc.yBottom;
                     bmarray[1].x = rc.xRight;
                     bmarray[1].y = rc.yTop;
                  bmarray[3].x = rc.xRight;
                  bmarray[3].y = rc.yTop;

                  hpsForDrawing = hpsCurrent;
                }
              }
            } else {
              hpsForDrawing = hpsCurrent;
              /* Reset the array... */
                 bmarray[0].x = rc.xLeft;
                 bmarray[0].y = rc.yBottom;
                 bmarray[1].x = rc.xRight;
                 bmarray[1].y = rc.yTop;
              bmarray[3].x = rc.xRight;
              bmarray[3].y = rc.yTop;
            }
            if (bPaletteManager && bNewPalette) {
               WinPostMsg(hClient, WM_REALIZEPALETTE, 0, 0);
               WinBroadcastMsg( HWND_DESKTOP, WM_REALIZEPALETTE, 0L, 0L, BMSG_POST | BMSG_DESCENDANTS);
               WinRealizePalette(hClient, hpsWindow, &cclr);
               bNewPalette = FALSE;
            } /* endif */

            /* Now put it to screen... */
            if (bBW) {
               GpiSetBackColor(hpsWindow, 0);
               GpiSetColor(hpsWindow, 1);
            } /* endif */
            if (bKeepProportion && bScaleWhileDrawing) {
               if (!bPaletteManager) {
                  GpiCreateLogColorTable(hpsWindow, LCOL_RESET, LCOLF_RGB, 0, 0, NULL);
               } /* endif */
               WinFillRect(hpsWindow, &rc, bPaletteManager ? 0 : backcolor);
            } /* endif */
            if (GpiBitBlt(hpsWindow, hpsForDrawing, 4L, (PPOINTL)bmarray,
                          (LONG) ROP_SRCCOPY, (LONG) BBO_IGNORE) == GPI_ERROR) {
              bError = TRUE;
#ifdef DEBUG
              printf("Error from GpiBitBlt.\n");
              printf("Errorid = %x.\n",WinGetLastError(hAB));
#endif
            }
            if (bBW) {
               GpiSetColor(hpsWindow, CLR_BLACK);
               GpiSetBackColor(hpsWindow, CLR_WHITE);
            } /* endif */

            bNewImage = FALSE;

          }
          if (bError) {
            WinAlarm(HWND_DESKTOP, WA_WARNING);

            strcpy(MsgBuf, "An error occured while trying to draw bitmap (probably out of memory).");
            GpiQueryFontMetrics(hpsWindow, (LONG)sizeof(FONTMETRICS),
                                (PFONTMETRICS)&FontBuf);

            CharHeight = LOUSHORT(FontBuf.lMaxBaselineExt);
            PointStruct.x = 0L;
            PointStruct.y = rc.yTop - CharHeight;
            GpiCharStringAt(hpsWindow, (PPOINTL)&PointStruct,
                            (LONG)strlen(MsgBuf), (PCH)MsgBuf);
            if (!bScaleWhileDrawing) {
              strcpy(MsgBuf, "Try not keeping the scaled image in memory (\"Drawing\" pulldown).");
              PointStruct.x = 0L;
              PointStruct.y = rc.yTop - (2 * CharHeight);
              GpiCharStringAt(hpsWindow, (PPOINTL)&PointStruct,
                              (LONG)strlen(MsgBuf), (PCH)MsgBuf);
            }
          }
          WinEndPaint(hps);                     /* Drawing is complete   */
          bReturnVal = TRUE;
        }
        DosReleaseMutexSem(hUpdateSem);
        /* Restore the pointer... */
        WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP,
                                                       SPTR_ARROW, FALSE));
      } else {
        bReturnVal = FALSE;
      }
      break;

case WM_CLOSE:
      if (bPaletteManager) {
          GpiSelectPalette(hpsWindow, 0);
          WinRealizePalette(hClient, hpsWindow, &cclr);
          WinRealizePalette(hClient, hpsWindow, &cclr);
          WinRealizePalette(hClient, hpsWindow, &cclr);
      } /* endif */
      WinPostMsg( hwnd, WM_QUIT, 0L, 0L );  /* Cause termination        */
      bReturnVal = TRUE;
      break;

    default:
      bReturnVal = FALSE;
  }
  if (bReturnVal) {
    return((MRESULT)bReturnVal);
  } else {
    return(WinDefWindowProc(hwnd, msg, mp1, mp2));
  }
}



#if 0
/*****************************************************************************/
/*  Abstract for function: FilterProc()                                      */
/*    This function is needed so we get PAINT messages when iconic so we     */
/*    can repaint ourselves properly when iconic.                            */
/*****************************************************************************/
MRESULT EXPENTRY FilterProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  if ( (msg == WM_PAINT)
       && (WinQueryWindowULong(hwnd, QWL_STYLE) & WS_MINIMIZED) ) {
    /* If we don't paint it, let the frame paint it! */
    if (WinSendMsg(hClient, WM_PAINT, (MPARAM)NULL, (MPARAM)NULL)) {
      /* If we don't return this, we go into an endless WM_PAINT loop!! */
      return(WinDefWindowProc(hwnd, msg, mp1, mp2));
    }
  }
  return (*pFrameWndProc)(hwnd, msg, mp1, mp2);
}
#endif


/* These flags are used for the "OpenMode" parameter of the DosOpen() call. */
#define OM_DASD              0x8000
#define OM_WRITETHROUGH      0x4000
#define OM_FAILERRORS        0x2000
#define OM_INHERITFLAG       0x0080
#define OM_SHARE_DENYRW      0x0010   /* Deny read/write access.             */
#define OM_SHARE_DENYWR      0x0020   /* Deny write access.                  */
#define OM_SHARE_DENYRD      0x0030   /* Deny read access.                   */
#define OM_SHARE_DENYNO      0x0040   /* Deny none.                          */
#define OM_ACCESS_RD         0x0000   /* Read only access.                   */
#define OM_ACCESS_WR         0x0001   /* Write only access.                  */
#define OM_ACCESS_RW         0x0002   /* Read/write access.                  */


/*...sLoadBitmap:0:*/
ULONG LoadBitmap(CHAR *szFn, CHAR *szOpt, GBM *pgbm, GBMRGB *pgbmrgb, BYTE **ppbData)
{
   GBM_ERR rc;
   int fd, ft;
   ULONG cb;
   USHORT usStride;
   PSZ pszFn = malloc(strlen(szFn)+10);
   PSZ pOpts;

   if (!pszFn) {
      return (ULONG)GBM_ERR_MEM;
   } /* endif */
   strcpy(pszFn, szFn);
   if (pOpts = strchr(pszFn, ',')) {
      *pOpts++ = '\0';
   } else {
      pOpts = pOptions;
   } /* endif */
   if ( (rc = gbm_guess_filetype(pszFn, &ft)) != GBM_ERR_OK ) {
#ifdef DEBUG
      printf("Error %u from gbm_guess_filetype. Filename is %s\n", rc, pszFn);
#endif
      rc = GBM_ERR_NOT_SUPP;
   } else if ( (fd = open(pszFn, O_RDONLY | O_BINARY)) == -1 ) {
      rc = GBM_ERR_NOT_FOUND;
#ifdef DEBUG
      printf("GBM_ERR_NOT_FOUND\n");
#endif
   } else {
      if ( (rc = gbm_read_header(pszFn, fd, ft, pgbm, pOpts)) != GBM_ERR_OK ) {
#ifdef DEBUG
         printf("Error %u from gbm_read_header.\n", rc);
#endif
         rc = GBM_ERR_READ;
      } else if ( (rc = gbm_read_palette(fd, ft, pgbm, pgbmrgb)) != GBM_ERR_OK ) {
#ifdef DEBUG
         printf("Error %u from gbm_read_palette.\n", rc);
#endif
         rc = GBM_ERR_READ;
      } else {
         usStride = ((pgbm -> w * pgbm -> bpp + 31)/32) * 4;
         cb = pgbm -> h * usStride;
         if ( (*ppbData = malloc((int) cb)) == NULL ) {
            rc = GBM_ERR_MEM;
#ifdef DEBUG
            printf("Error allocating memory, size = %u.\n", cb);
#endif
         } else if ( (rc = gbm_read_data(fd, ft, pgbm, *ppbData)) != GBM_ERR_OK ) {
#ifdef DEBUG
            printf("Error %u from gbm_read_data.\n", rc);
#endif
            free(*ppbData);
            rc = GBM_ERR_READ;
         } /* endif */
      } /* endif */
      close(fd);
   } /* endif */
   free(pszFn);
   return (ULONG)rc;
}



ULONG LoadBitmapFile(PSZ lpFileName, PHAB phAB, PHDC phdcMem,
                      PHPS phpsMem, PHBITMAP phbmMem,
                      BOOL bShowWaitPointer)
{
  SIZEL             ImageSize        ;
  ULONG             ReturnVal   = 0  ;
  HPAL              hpal             ;
  UCHAR             path[256]        ;
  PUCHAR            pdir             ;
  GBM gbm;
  GBMRGB gbmrgb [0x100];
  BYTE *pbData;

static ULONG errtab[9] = {
   0, LBF_ERROR_ALLOC_MEM_RC, LBF_ERROR_CREATING_BMP_RC, LBF_ERROR_CREATING_BMP_RC,
   LBF_COULDNT_OPEN_FILE_RC, LBF_ERROR_CREATING_BMP_RC, LBF_ERROR_CREATING_BMP_RC,
   LBF_ERROR_CREATING_BMP_RC, LBF_ERROR_CREATING_BMP_RC
};

  if (bShowWaitPointer) {
    WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP,
                                                   SPTR_WAIT, FALSE));
  }

  /* First remove all leading and trailing white space, or this call */
  /* will fail!! */
  RemoveWhiteSpace(lpFileName);

  if (!(ReturnVal = errtab[LoadBitmap(lpFileName, "", &gbm, gbmrgb, &pbData)])) {
     struct {
        BITMAPINFOHEADER2 bmp2;
        RGB2              rgb[0x100];
     } bm;
     ULONG cRGB;
     ULONG ulCol;

     memset(&bm, 0, sizeof(bm));
     bm.bmp2.cbFix     = sizeof(BITMAPINFOHEADER2);
     bm.bmp2.cx        = gbm.w;
     bm.bmp2.cy        = gbm.h;
     bm.bmp2.cBitCount = gbm.bpp;
     bm.bmp2.cPlanes   = 1;
     cRGB = ( (1 << gbm.bpp) & 0x1ff );
             /* 1 -> 2, 4 -> 16, 8 -> 256, 24 -> 0 */

     if (cRGB && cRGB<257) {
        for ( ulCol = 0; ulCol < cRGB; ulCol++ ) {
           bm.rgb[ulCol].bRed   = gbmrgb [ulCol].r;
           bm.rgb[ulCol].bGreen = gbmrgb [ulCol].g;
           bm.rgb[ulCol].bBlue  = gbmrgb [ulCol].b;
        } /* endfor */
        backcolor = *(PULONG)&bm.rgb[0];
        forecolor = *(PULONG)&bm.rgb[1];
     } else {
        backcolor = 0;
     } /* endif */
     bBW = (bm.bmp2.cBitCount == 1);
     if (bPaletteSupport && cRGB && cRGB<257) {
        hpal = GpiCreatePalette(hAB, 0, LCOLF_CONSECRGB, cRGB, (PULONG)&(bm.rgb));
     } else {
        hpal = 0;
     } /* endif */
     GpiSelectPalette(hpsCurrent, hpal);
     if (hsyspal) {
        GpiSelectPalette(*phpsMem, 0);
        GpiSelectPalette(hpsWindow, 0);
        GpiDeletePalette(hsyspal);
        hsyspal = 0;
     } /* endif */
     hsyspal = hpal;
     /* Make sure no old bitmaps are laying around!! */
     RemoveBitmapFromMem(phdcMem, phpsMem, phbmMem);

     *phdcMem  = DevOpenDC(*phAB, OD_MEMORY, "*", 8L,
                           (PDEVOPENDATA)dcdatablk, hdcScreen);
/*
     if (!*phdcMem) {
       ERRORID error = WinGetLastError(*phAB);
     }
*/
     ImageSize.cx = 0;
     ImageSize.cy = 0;
     *phpsMem  = GpiCreatePS(*phAB, *phdcMem, (PSIZEL)&ImageSize,
                             (LONG) PU_PELS | GPIT_NORMAL | GPIA_ASSOC);

     if (hpal) {
        GpiSelectPalette(*phpsMem, hpal);
     } /* endif */

     *phbmMem  = GpiCreateBitmap(*phpsMem, (PBITMAPINFOHEADER2)&(bm.bmp2),
                                 (ULONG)CBM_INIT, (PSZ)pbData,
                                 (PBITMAPINFO2)&(bm.bmp2));

     if (!(*phdcMem && *phpsMem && *phbmMem)) {
       ReturnVal = LBF_ERROR_CREATING_BMP_RC;
#ifdef DEBUG
       printf("Error creating bitmap, hdc=%u, hps=%u, hbm=%u.\n",*phdcMem, *phpsMem, *phbmMem);
#endif
       RemoveBitmapFromMem(phdcMem, phpsMem, phbmMem);
     } else {
       BITMAPINFOHEADER2 bminfo;

       GpiSetBitmap(*phpsMem, *phbmMem);
       bminfo.cbFix = sizeof(bminfo);
       GpiQueryBitmapInfoHeader(*phbmMem, &bminfo);
       GlobalBitCount = bminfo.cBitCount;
       Globalx = bminfo.cx;
       Globaly = bminfo.cy;
     }
     GpiSelectPalette(hpsWindow, bPaletteManager ? hpal : 0);
     bNewPalette = TRUE;

     free(pbData);
  } /* endif */

  if (bShowWaitPointer) {
    WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP,
                                                   SPTR_ARROW, FALSE));
  }
  if (ReturnVal) {
     ShowErrors(ReturnVal,NULL);
  } else {
     if (lpFileName[1] == ':') {
        DosSetDefaultDisk(1+toupper(lpFileName[0])-'A');
        lpFileName += 2;
     } /* endif */
     strcpy(path,lpFileName);
     if (pdir = strrchr(path,'\\')) {
        *pdir = '\0';
        DosSetCurrentDir(path);
     } /* endif */
  } /* endif */
  return(ReturnVal);
}



/*****************************************************************************/
/*  Abstract for function: RemoveWhiteSpace()                                */
/*    This function will strip all leading and trailing whitespace from a    */
/*    string.  It uses no "C" routines so that it can be used without        */
/*    regard to memory model or multi-threaded/DLL problems.                 */
/*****************************************************************************/
VOID RemoveWhiteSpace(PSZ lpString)
{
  register ULONG i;
  register ULONG j;
  ULONG NumToShift;

  /* Don't use "C" routines in case someone wants to use this with small */
  /* or medium-model runtime libraries... */
  /* First remove trailing whitespace... */
  for (i = 0; lpString[i]; ) i++;
  for(;(i && (lpString[i - 1] == ' ')); i--) {
    lpString[i - 1] = '\0';
  }

  /* Now remove leading whitespace... */
  for (j = 0; lpString[j] && (lpString[j] == ' '); ) j++;
  /* Now just "shift" the string over by the appropriate amount... */
  if (NumToShift = i - j) {
    for (i = 0; i < NumToShift; i++, j++) {
      lpString[i] = lpString[j];
    }
    /* Now null-terminate */
    lpString[i] = '\0';
  }
}


/*****************************************************************************/
/*  Abstract: RemoveBitmapFromMem()                                          */
/*    This function totally frees the bitmap, presentation space, and        */
/*    device context, and then sets all of the pointers to these items to    */
/*    NULL.                                                                  */
/*****************************************************************************/
void RemoveBitmapFromMem(PHDC phdc, PHPS phps, PHBITMAP phbm)
{
  if (phbm) {
    if (phps && *phps) {
        /* Remove old bitmap... */
        GpiSetBitmap (*phps, (HBITMAP)NULL);
    }
    if (*phbm) {
      GpiDeleteBitmap(*phbm);
    }

    /* Free up the PS... */
    if (phps) {
      if (*phps) {
        GpiAssociate(*phps, (HDC)NULL);
        GpiDestroyPS(*phps);
      }
      *phps = (HPS)NULL;
    }
    *phbm = (HBITMAP)NULL;
  } else if (phps) {
    if (*phps) {
      /* Remove old bitmap... */
      GpiSetBitmap (*phps, (HBITMAP)NULL);
      /* Free up the PS... */
      GpiAssociate(*phps, (HDC)NULL);
      GpiDestroyPS(*phps);
    }
    *phps = (HPS)NULL;
  }

  if (phdc) {
    if (*phdc) {
      /* Remove old DC... */
      DevCloseDC(*phdc);
    }
    *phdc = (HDC)NULL;
  }
}


/*****************************************************************************/
/*  Abstract: AddToSwitchList()                                              */
/*    This function adds a program entry of text "pEntryText into the        */
/*    Task Manger's "switch list"                                            */
/*****************************************************************************/
HSWITCH AddToSwitchList(HWND hSomeWnd, HWND hIcon, UCHAR *pEntryText)
{
  SWCNTRL SwitchStruct;
  PID SomeWndPID;
  TID SomeWndTID;

  WinQueryWindowProcess(hSomeWnd, &SomeWndPID, &SomeWndTID);

  SwitchStruct.hwnd = hSomeWnd;
  SwitchStruct.hwndIcon = hIcon;
  SwitchStruct.hprog = (HPROGRAM)0L;
  SwitchStruct.idProcess = SomeWndPID;
  SwitchStruct.idSession = 0;
  SwitchStruct.uchVisibility = SWL_VISIBLE;
  SwitchStruct.fbJump = SWL_JUMPABLE;
/*  SwitchStruct.fReserved = 0;*/
  strncpy(SwitchStruct.szSwtitle, pEntryText,
         sizeof(SwitchStruct.szSwtitle) - 1);
  return(WinAddSwitchEntry((PSWCNTRL)&SwitchStruct));
}


      #define max(a,b) (((a) > (b)) ? (a) : (b))
/*****************************************************************************/
/*  Abstract: ResetWindowText()                                              */
/*    This function will extract the filename from the file/path spec,       */
/*    convert it to uppercase, and put it in hWnd's title bar.               */
/*****************************************************************************/
void ResetWindowText(HWND hWnd, UCHAR *pTextString, ULONG usNumberOfColors, ULONG x, ULONG y)
{
  UCHAR *pTemp;
  UCHAR TitleBuf[150];

#ifdef DEBUG
  printf("Entering ResetWindowText with hwnd=%x.\n", hWnd);
#endif
  pTemp = (PUCHAR)max(strrchr(pTextString, ':'), strrchr(pTextString, '\\'));
  if (pTemp) {
    pTemp++;
  } else {
    pTemp = pTextString;
  }
  strcpy(TitleBuf, pTemp);
  strupr(TitleBuf);
  strcat(TitleBuf, " - ");
  itoa(x, &TitleBuf[strlen(TitleBuf)], 10);
  strcat(TitleBuf, "x");
  itoa(y, &TitleBuf[strlen(TitleBuf)], 10);
  strcat(TitleBuf, "x");
  /* Convert number of bits to number of colors... */
/*  colors = 1 << usNumberOfColors;*/
  if (1 == usNumberOfColors) {
     strcat(TitleBuf,"2 colors");
  } else if (4 == usNumberOfColors) {
     strcat(TitleBuf,"16 colors");
  } else if (8 == usNumberOfColors) {
     strcat(TitleBuf,"256 colors");
  } else if (24 == usNumberOfColors) {
     strcat(TitleBuf," 24 bit color");
  } else {
     strcat(TitleBuf,"unknown colors");
  } /* endif */
/*  strcat(TitleBuf, " colors"); */
/*  itoa(colors, &TitleBuf[strlen(TitleBuf)], 10);*/
  WinSetWindowText(hWnd, TitleBuf);
}


void ShowErrors(ULONG ErrorCode, PSZ lpString)
{
  UCHAR MsgBuf[100];
  PSZ lpTemp;

  if (!lpString && !WinLoadMessage((HAB)NULL, (HMODULE)NULL, ErrorCode,
                                   sizeof(MsgBuf), MsgBuf)) {
    strcpy(MsgBuf, "Message not available.");
  } else if (lpString) {
    lpTemp = lpString;
  } else {
    lpTemp = MsgBuf;
  }

  WinAlarm(HWND_DESKTOP, WA_WARNING);
  if (!bIgnoreErrors && MBID_ERROR ==
           WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, lpTemp, "Bitmap Displayer Error!", 0,
                         MB_OK | MB_CUAWARNING | MB_MOVEABLE) ) {
     WinGetLastError(hAB);
  } /* endif */
}


/*****************************************************************************/
/*  Abstract: StaticBox()                                                    */
/*    This function is used to control DlgBoxes that only display static     */
/*    text and have just an "Ok" button.                                     */
/*                                                                           */
/*    Currently its associated DlgBoxes have the resource IDs of:            */
/*    ABOUTBOX and VALIDPARAMS.                                              */
/*****************************************************************************/
BOOL EXPENTRY StaticBox(HWND hDlg, ULONG Message, MPARAM mp1, MPARAM mp2)
{
  HWND hTempWnd;

  if (Message == WM_COMMAND) {
    WinDismissDlg(hDlg, TRUE);
    return(TRUE);
  } else if (Message == WM_CLOSE) {
    /* Tell ourselves to leave... */
    WinPostMsg(hDlg, WM_COMMAND, (MPARAM)DID_CANCEL, (MPARAM)0);
    return(TRUE);
  } else if (Message == WM_INITDLG) {
    if (mp2 == MPFROMSHORT(HELPBOX)) {
      /* Adjust the system pulldown to be more suitable for this DialogBox... */
      hTempWnd = WinWindowFromID(hDlg, FID_SYSMENU);
      WinSendMsg(hTempWnd, MM_REMOVEITEM, MPFROM2SHORT(SC_RESTORE, TRUE),
                 (MPARAM)NULL);
      WinSendMsg(hTempWnd, MM_REMOVEITEM, MPFROM2SHORT(SC_RESTORE, TRUE),
                 (MPARAM)NULL);
      WinSendMsg(hTempWnd, MM_REMOVEITEM, MPFROM2SHORT(SC_SIZE, TRUE),
                 (MPARAM)NULL);
      WinSendMsg(hTempWnd, MM_REMOVEITEM, MPFROM2SHORT(SC_MINIMIZE, TRUE),
                 (MPARAM)NULL);
      WinSendMsg(hTempWnd, MM_REMOVEITEM, MPFROM2SHORT(SC_MAXIMIZE, TRUE),
                 (MPARAM)NULL);
      /* Now change the "Close" item to be "Cancel"... */
      WinSendMsg(hTempWnd, MM_SETITEMTEXT, (MPARAM)SC_CLOSE,
                 MPFROMP("~Cancel\tAlt+F4"));
    }
    /* Make sure that the button we want starts off with the focus! */
    /* This isn't too bad, since under PM we have to give the dialogbox */
    /* the focus explicitly anyway, so just give the focus to control */
    /* instead! */
    WinSetFocus(HWND_DESKTOP, WinWindowFromID(hDlg, DID_OK));
    return(TRUE);
  } else {
    return((BOOL)SHORT1FROMMR(WinDefDlgProc(hDlg, Message, mp1, mp2)));
  }
}



/*****************************************************************************/
/*  Abstract for function: PutFileInLList()                                  */
/*    This function will put all *pertinent* lines from the .SLD (slide)     */
/*    file into a circularly linked list, and return a pointer to the        */
/*    head node of the list.  If there are no items or an error occurs,      */
/*    the return value will be NULL.                                         */
/*                                                                           */
/*    Use the FreeLList() function to delete the entire list.                */
/*****************************************************************************/
PLINKNODE PutFileInLList(FILE *hFile)
{
  register int TempShort;
  UCHAR LineBuf[260];
  PLINKNODE pHeadNode = (PLINKNODE)NULL;
  PLINKNODE pCurrNode;

  while (fgets(LineBuf, sizeof(LineBuf), hFile)) {
    if (LineBuf[0] != ';')  /* If comment line, skip it! */ {
      /* So we must have a valid path returned to us in */
      /* LineBuf!! */
      /* Remove trailing CR... */
      TempShort = strlen(LineBuf);
      if (TempShort) {
        if ( (LineBuf[TempShort - 1] == 0x0D)
          || (LineBuf[TempShort - 1] == 0x0A) ) {
          LineBuf[TempShort - 1] = '\0';
        }
      }
      /* Do this in case the line consists of all whitespace!! */
      RemoveWhiteSpace(LineBuf);
      if (LineBuf[0])  /* If blank line, skip it! */ {
        pCurrNode = calloc(1, sizeof(LINKNODE));
        pCurrNode->pString = malloc((strlen(LineBuf) + 1));
        strcpy(pCurrNode->pString, LineBuf);
        if (pHeadNode) {
          pCurrNode->pNext = pHeadNode;
          pCurrNode->pPrev = pHeadNode->pPrev;
          pCurrNode->pPrev->pNext = pCurrNode;
          pHeadNode->pPrev = pCurrNode;
          if (!pHeadNode->pNext) {
            pHeadNode->pNext = pCurrNode;
          }
        } else {
          pHeadNode = pCurrNode;
          pHeadNode->pPrev = pHeadNode;
        }
      }
    }
  }
  return(pHeadNode);
}


/*****************************************************************************/
/*  Abstract for function: FreeLList()                                       */
/*    This function will free the memory associated with this entire linked  */
/*    list structure.                                                        */
/*****************************************************************************/
VOID FreeLList(PLINKNODE pHeadNode)
{
  PLINKNODE pFreeNode;

  while (pHeadNode) {
    if (pHeadNode->pNext == pHeadNode) {
      free(pHeadNode->pString);
      free(pHeadNode);
      pHeadNode = (PLINKNODE)NULL;
    } else {
      pFreeNode = pHeadNode->pNext;  /* This is the one we'll be freeing.    */
      pHeadNode->pNext = pFreeNode->pNext;
      free(pFreeNode->pString);
      free(pFreeNode);
    }
  }
}


/*****************************************************************************/
/*  Abstract for function: FixupMenus()                                      */
/*    This function will set up the pulldown menus for the slide show, by    */
/*    disabling menus, etc., and then will restore them once the slide show  */
/*    is over.                                                               */
/*                                                                           */
/*    This function will return a handle to the specified window's menu.     */
/*****************************************************************************/
HWND FixupMenus(HWND hMenuOwner, BOOL bStartShow)
{
  HWND hMenu;
  USHORT FileAttrib;
  USHORT SlideAttrib;

  hMenu = WinWindowFromID(hMenuOwner, FID_MENU);
  if (bStartShow) {
    FileAttrib = MIA_DISABLED;
    SlideAttrib = 0;
  } else {
    FileAttrib = 0;
    SlideAttrib = MIA_DISABLED;
  }

  WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(ID_LOADFILE, TRUE),
             MPFROM2SHORT(MIA_DISABLED, FileAttrib));
  WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(ID_LOADSLD, TRUE),
             MPFROM2SHORT(MIA_DISABLED, FileAttrib));
  WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(ID_RESTART,  TRUE),
             MPFROM2SHORT(MIA_DISABLED, SlideAttrib));
  WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(ID_PAUSE,  TRUE),
             MPFROM2SHORT(MIA_DISABLED, SlideAttrib));
  WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(ID_TERMINATE, TRUE),
             MPFROM2SHORT(MIA_DISABLED, SlideAttrib));
  WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(ID_FORWARD, TRUE),
             MPFROM2SHORT(MIA_DISABLED, SlideAttrib));
  WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(ID_REVERSE, TRUE),
             MPFROM2SHORT(MIA_DISABLED, SlideAttrib));
  return(hMenu);
}


/*****************************************************************************/
/*  Abstract for function: ParseArgs()                                       */
/*    This function parses out the command line arguments, filling up the    */
/*    "InitialSize" structure if ncecessary, and pointing the filename       */
/*    pointer at the appropriate string.                                     */
/*                                                                           */
/*    This function will return a bit value indicating if sizing args where  */
/*    specified, if the window is to be initially minimized or maximized,    */
/*    and if a filespec was given, whether or not it is for a .BMP file or a */
/*    .SLD file.                                                             */
/*****************************************************************************/
ULONG ParseArgs(int argc, char *argv[], UCHAR **pFname, PRECTL pInitialSize)
{
  UCHAR *pCurrArg;
  ULONG ReturnVal = 0;

  *pFname = NULL;
  while (--argc) {
    /* Use this to save time! */
    pCurrArg = argv[argc];
    strupr(pCurrArg);
    if (pCurrArg[0] != '/') {
      *pFname = pCurrArg;
    } else {
      switch (pCurrArg[1]) {
        case 'E':
          bIgnoreErrors = !bIgnoreErrors;
          break;
        case 'F':
          bFrameEmpty = !bFrameEmpty;
          break;
        case 'H':
          /* Height... */
          pInitialSize->yTop = atoi(&pCurrArg[3]);
          ReturnVal |= SWP_SIZE;
          break;
        case 'I':
          ReturnVal |= SWP_MINIMIZE;
          break;
        case 'K':
          bScaleWhileDrawing = !(bScaleWhileDrawing);
          break;
        case 'O':
          pOptions = strdup(&pCurrArg[3]);
          break;
        case 'P':
          bPaletteManager = !bPaletteManager;
          break;
       case 'R':
          bKeepProportion = !bKeepProportion;
          break;
        case 'S':
          ReturnVal |= SLIDE_FILE;
          break;
        case 'W':
          /* Width... */
          pInitialSize->xRight = atoi(&pCurrArg[3]);
          ReturnVal |= SWP_SIZE;
          break;
        case 'X':
          pInitialSize->xLeft = atoi(&pCurrArg[3]);
          ReturnVal |= SWP_SIZE;
          break;
        case 'Y':
          pInitialSize->yBottom = atoi(&pCurrArg[3]);
          ReturnVal |= SWP_SIZE;
          break;
        case 'Z':
          ReturnVal |= SWP_MAXIMIZE;
          break;
      }
    }
  }
  if (*pFname) {
    if (!(ReturnVal & SLIDE_FILE)) {  /* If they didn't use /S, default is .BMP file! */
      ReturnVal |= BITMAP_FILE;
    }
  } else if (ReturnVal & SLIDE_FILE) { /* Can't specify /S with no filename!!          */
    ReturnVal &= (~SLIDE_FILE);     /* So remove /S option.                         */
  }

  return(ReturnVal);
}
