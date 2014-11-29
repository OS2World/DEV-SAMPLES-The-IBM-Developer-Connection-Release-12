/*

GBMV.C  General Bitmap Viewer

*/

/*...sincludes:0:*/
#define	INCL_DOS
#define	INCL_WIN
#define	INCL_GPI
#define	INCL_DEV
#define	INCL_BITMAPFILEFORMAT
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gbm.h"
#include "gbmerr.h"
#include "gbmht.h"
#include "gbmv.h"

/*...vgbmv\46\h:0:*/
/*...e*/
/*...spm gaps:0:*/
/*
Things you would expect PM to have, but find that it doesn't.
*/

/*...sError:0:*/
static VOID Error(HWND hwnd, const CHAR *szFmt, ...)
	{
	va_list vars;
	CHAR sz [256+1];

	va_start(vars, szFmt);
	vsprintf(sz, szFmt, vars);
	va_end(vars);
	WinMessageBox(HWND_DESKTOP, hwnd, sz, NULL, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
	}
/*...e*/
/*...sFatal:0:*/
static VOID Fatal(HWND hwnd, const CHAR *sz)
	{
	Error(hwnd, sz);
	exit(1);
	}
/*...e*/
/*...sUsage:0:*/
static VOID Usage(HWND hwnd)
	{
	Fatal(hwnd, "Usage: gbmv [-e] [-h] filename.ext{,opt}");
	}
/*...e*/
/*...sWarning:0:*/
static VOID Warning(HWND hwnd, const CHAR *szFmt, ...)
	{
	va_list vars;
	CHAR sz [256+1];

	va_start(vars, szFmt);
	vsprintf(sz, szFmt, vars);
	va_end(vars);
	WinMessageBox(HWND_DESKTOP, hwnd, sz, NULL, 0, MB_OK | MB_WARNING | MB_MOVEABLE);
	}
/*...e*/
/*...e*/
/*...smain:0:*/
#define	WC_GBMV		"GbmViewerClass"

static HWND hwndFrame, hwndClient;

static HBITMAP hbmBmp;
static LONG lColorBg, lColorFg;

/*...sLoadBitmap:0:*/
static BOOL LoadBitmap(
	HWND hwnd,
	const CHAR *szFn, const CHAR *szOpt,
	GBM *gbm, GBMRGB *gbmrgb, BYTE **ppbData
	)
	{
	GBM_ERR rc;
	int fd, ft;
	ULONG cb;
	USHORT usStride;

	if ( gbm_guess_filetype(szFn, &ft) != GBM_ERR_OK )
		{
		Warning(hwnd, "Can't deduce bitmap format from file extension: %s", szFn);
		return ( FALSE );
		}

	if ( (fd = open(szFn, O_RDONLY | O_BINARY)) == -1 )
		{
		Warning(hwnd, "Can't open file: %s", szFn);
		return ( FALSE );
		}

	if ( (rc = gbm_read_header(szFn, fd, ft, gbm, szOpt)) != GBM_ERR_OK )
		{
		close(fd);
		Warning(hwnd, "Can't read file header of %s: %s", szFn, gbm_err(rc));
		return ( FALSE );
		}

	if ( (rc = gbm_read_palette(fd, ft, gbm, gbmrgb)) != GBM_ERR_OK )
		{
		close(fd);
		Warning(hwnd, "Can't read file palette of %s: %s", szFn, gbm_err(rc));
		return ( FALSE );
		}

	usStride = ((gbm -> w * gbm -> bpp + 31)/32) * 4;
	cb = gbm -> h * usStride;
	if ( (*ppbData = malloc((int) cb)) == NULL )
		{
		close(fd);
		Warning(hwnd, "Out of memory requesting %ld bytes", cb);
		return ( FALSE );
		}

	if ( (rc = gbm_read_data(fd, ft, gbm, *ppbData)) != GBM_ERR_OK )
		{
		free(*ppbData);
		close(fd);
		Warning(hwnd, "Can't read file data of %s: %s", szFn, gbm_err(rc));
		return ( FALSE );
		}

	close(fd);

	return ( TRUE );
	}
/*...e*/
/*...sTo24Bit:0:*/
static BOOL To24Bit(GBM *gbm, GBMRGB *gbmrgb, BYTE **ppbData)
	{
	int	stride = ((gbm -> w * gbm -> bpp + 31)/32) * 4;
	int	new_stride = ((gbm -> w * 3 + 3) & ~3);
	int	bytes, y;
	byte	*pbDataNew;

	if ( gbm -> bpp == 24 )
		return ( TRUE );

	bytes = new_stride * gbm -> h;
	if ( (pbDataNew = malloc(bytes)) == NULL )
		return ( FALSE );

	for ( y = 0; y < gbm -> h; y++ )
		{
		byte	*src = *ppbData + y * stride;
		byte	*dest = pbDataNew + y * new_stride;
		int	x;

		switch ( gbm -> bpp )
			{
/*...s1:24:*/
case 1:
	{
	byte	c;

	for ( x = 0; x < gbm -> w; x++ )
		{
		if ( (x & 7) == 0 )
			c = *src++;
		else
			c <<= 1;

		*dest++ = gbmrgb [(c & 0x80) != 0].b;
		*dest++ = gbmrgb [(c & 0x80) != 0].g;
		*dest++ = gbmrgb [(c & 0x80) != 0].r;
		}
	}
	break;
/*...e*/
/*...s4:24:*/
case 4:
	for ( x = 0; x + 1 < gbm -> w; x += 2 )
		{
		byte	c = *src++;

		*dest++ = gbmrgb [c >> 4].b;
		*dest++ = gbmrgb [c >> 4].g;
		*dest++ = gbmrgb [c >> 4].r;
		*dest++ = gbmrgb [c & 15].b;
		*dest++ = gbmrgb [c & 15].g;
		*dest++ = gbmrgb [c & 15].r;
		}

	if ( x < gbm -> w )
		{
		byte	c = *src;

		*dest++ = gbmrgb [c >> 4].b;
		*dest++ = gbmrgb [c >> 4].g;
		*dest++ = gbmrgb [c >> 4].r;
		}
	break;
/*...e*/
/*...s8:24:*/
case 8:
	for ( x = 0; x < gbm -> w; x++ )
		{
		byte	c = *src++;

		*dest++ = gbmrgb [c].b;
		*dest++ = gbmrgb [c].g;
		*dest++ = gbmrgb [c].r;
		}
	break;
/*...e*/
			}
		}
	free(*ppbData);
	*ppbData = pbDataNew;
	gbm -> bpp = 24;

	return ( TRUE );
	}
/*...e*/
/*...sMakeBitmap:0:*/
static BOOL MakeBitmap(
	HWND hwnd,
	const GBM *gbm, const GBMRGB *gbmrgb, const BYTE *pbData,
	HBITMAP *phbm
	)
	{
	HAB hab = WinQueryAnchorBlock(hwnd);
	USHORT cRGB, usCol;
	SIZEL sizl;
	HDC hdc;
	HPS hps;
	struct
		{
		BITMAPINFOHEADER2 bmp2;
		RGB2 argb2Color [0x100];
		} bm;

	/* Got the data in memory, now make bitmap */

	memset(&bm, 0, sizeof(bm));

	bm.bmp2.cbFix     = sizeof(BITMAPINFOHEADER2);
	bm.bmp2.cx        = gbm -> w;
	bm.bmp2.cy        = gbm -> h;
	bm.bmp2.cBitCount = gbm -> bpp;
	bm.bmp2.cPlanes   = 1;

	cRGB = ( (1 << gbm -> bpp) & 0x1ff );
		/* 1 -> 2, 4 -> 16, 8 -> 256, 24 -> 0 */

	for ( usCol = 0; usCol < cRGB; usCol++ )
		{
		bm.argb2Color [usCol].bRed   = gbmrgb [usCol].r;
		bm.argb2Color [usCol].bGreen = gbmrgb [usCol].g;
		bm.argb2Color [usCol].bBlue  = gbmrgb [usCol].b;
		}

	if ( (hdc = DevOpenDC(hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, (HDC) NULL)) == (HDC) NULL )
		{
		Warning(hwnd, "DevOpenDC failure");
		return ( FALSE );
		}

	sizl.cx = bm.bmp2.cx;
	sizl.cy = bm.bmp2.cy;
	if ( (hps = GpiCreatePS(hab, hdc, &sizl, PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC)) == (HPS) NULL )
		{
		Warning(hwnd, "GpiCreatePS failure");
		DevCloseDC(hdc);
		return ( FALSE );
		}


	if ( cRGB == 2 )
/*...shandle 1bpp case:16:*/
/*
1bpp presentation spaces have a reset or background colour.
They also have a contrast or foreground colour.
When data is mapped into a 1bpp presentation space :-
Data which is the reset colour, remains reset, and is stored as 0's.
All other data becomes contrast, and is stored as 1's.
The reset colour for 1bpp screen HPSs is white.
I want 1's in the source data to become 1's in the HPS.
We seem to have to reverse the ordering here to get the desired effect.
*/

{
static RGB2 argb2Black = { 0x00, 0x00, 0x00 };
static RGB2 argb2White = { 0xff, 0xff, 0xff };
bm.argb2Color [0] = argb2Black; /* Contrast */
bm.argb2Color [1] = argb2White; /* Reset */
}
/*...e*/

	if ( (*phbm = GpiCreateBitmap(hps, &(bm.bmp2), CBM_INIT, (BYTE *) pbData, (BITMAPINFO2 *) &(bm.bmp2))) == (HBITMAP) NULL )
		{
		Warning(hwnd, "GpiCreateBitmap failure");
		GpiDestroyPS(hps);
		DevCloseDC(hdc);
		return ( FALSE );
		}

	GpiSetBitmap(hps, (HBITMAP) NULL);
	GpiDestroyPS(hps);
	DevCloseDC(hdc);

	return ( TRUE );
	}
/*...e*/

/*...sGbmVWndProc:0:*/
MRESULT _System GbmVWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
	{
	switch ( (int) msg )
		{
/*...sWM_COMMAND \45\ menu command:16:*/
case WM_COMMAND:
	switch ( COMMANDMSG(&msg) -> cmd )
		{
/*...sMID_EXIT \45\ initiate shutdown of this app:32:*/
case MID_EXIT:
	WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
	break;
/*...e*/
		}
	return ( (MRESULT) 0 );
/*...e*/
/*...sWM_PAINT   \45\ repaint client area:16:*/
case WM_PAINT:
	{
	static SIZEL sizl = { 0, 0 };
	HPS hps = WinBeginPaint(hwnd, (HPS) NULL, NULL);
	HAB hab = WinQueryAnchorBlock(hwnd);
	HDC hdcBmp = DevOpenDC(hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, (HDC) NULL);
	HPS hpsBmp = GpiCreatePS(hab, hdcBmp, &sizl, PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);
	POINTL aptl [4];
	RECTL rcl;
	BITMAPINFOHEADER bmp;

	GpiQueryBitmapParameters(hbmBmp, &bmp);

	GpiSetBitmap(hpsBmp, hbmBmp);

	GpiSetBackColor(hps, GpiQueryColorIndex(hps, 0, lColorBg));
	GpiSetColor    (hps, GpiQueryColorIndex(hps, 0, lColorFg));

	WinQueryWindowRect(hwnd, &rcl);
	aptl [0].x = rcl.xLeft;
	aptl [0].y = rcl.yBottom;
	aptl [1].x = rcl.xRight;
	aptl [1].y = rcl.yTop;
	aptl [2].x = 0;
	aptl [2].y = 0;
	aptl [3].x = bmp.cx;
	aptl [3].y = bmp.cy;
	GpiBitBlt(hps, hpsBmp, 4L, aptl, ROP_SRCCOPY, BBO_IGNORE);

	GpiSetBitmap(hpsBmp, (HBITMAP) NULL);
	GpiDestroyPS(hpsBmp);
	DevCloseDC(hdcBmp);

	WinEndPaint(hps);
	}
	return ( (MRESULT) 0 );
/*...e*/
		}
	return ( WinDefWindowProc(hwnd, msg, mp1, mp2) );
	}
/*...e*/

int main(int argc, CHAR *argv [])
	{
	HAB hab = WinInitialize(0);
	HMQ hmq = WinCreateMsgQueue(hab, 0);/* 0 => default size for message Q   */
	static ULONG flFrameFlags = FCF_TITLEBAR  | FCF_SYSMENU    |
				    FCF_MINBUTTON | FCF_TASKLIST   |
				    FCF_BORDER    | FCF_ACCELTABLE ;
	QMSG qMsg;
	CHAR sz [255+1], *szOpt;
	GBM gbm;
	GBMRGB gbmrgb [0x100];
	BYTE *pbData;
	BOOL fHalftone = FALSE, fErrdiff = FALSE, fOk;
	int i;

/*...sprocess command line options:8:*/
for ( i = 1; i < argc; i++ )
	{
	if ( argv [i][0] != '-' )
		break;
	switch ( argv [i][1] )
		{
		case 'e':	fErrdiff = TRUE;
				break;
		case 'h':	fHalftone = TRUE;
				break;
		default:	Usage(HWND_DESKTOP);
				break;
		}
	}

if ( fErrdiff && fHalftone )
	Fatal(HWND_DESKTOP, "error-diffusion and halftoning can't both be done at once");

if ( i == argc )
	Usage(HWND_DESKTOP);
/*...e*/

	gbm_init();

	if ( (szOpt = strchr(argv [i], ',')) != NULL )
		*szOpt++ = '\0';
	else
		szOpt = "";

	if ( LoadBitmap(HWND_DESKTOP, argv [i], szOpt, &gbm, gbmrgb, &pbData) )
		{
		HPS hps = WinGetPS(HWND_DESKTOP);
		HDC hdc = GpiQueryDevice(hps);
		LONG lPlanes, lBitCount;

		DevQueryCaps(hdc, CAPS_COLOR_PLANES  , 1L, &lPlanes  );
		DevQueryCaps(hdc, CAPS_COLOR_BITCOUNT, 1L, &lBitCount);
		WinReleasePS(hps);

		if ( fHalftone || fErrdiff )
			fOk = To24Bit(&gbm, gbmrgb, &pbData);
		else
			fOk = TRUE;

		if ( fOk )
/*...scan now go on to errordiffuse\47\halftone and display:24:*/
{
if ( fHalftone )
	switch ( (int) lBitCount )
		{
		case 4:
			gbm_ht_pal_VGA(gbmrgb);
			gbm_ht_VGA_3x3(&gbm, pbData, pbData);
			gbm.bpp = 4;
			break;
		case 8:
			gbm_ht_pal_7R8G4B(gbmrgb);
			gbm_ht_7R8G4B_2x2(&gbm, pbData, pbData);
			gbm.bpp = 8;
			break;
		case 16:
			gbm_ht_24_2x2(&gbm, pbData, pbData, 0xf8, 0xfc, 0xf8);
			gbm.bpp = 24;
			break;
		}
else if ( fErrdiff )
	switch ( (int) lBitCount )
		{
		case 4:
			gbm_errdiff_pal_VGA(gbmrgb);
			gbm_errdiff_VGA(&gbm, pbData, pbData);
			gbm.bpp = 4;
			break;
		case 8:
			gbm_errdiff_pal_7R8G4B(gbmrgb);
			gbm_errdiff_7R8G4B(&gbm, pbData, pbData);
			gbm.bpp = 8;
			break;
		case 16:
			if ( !gbm_errdiff_24(&gbm, pbData, pbData, 0xf8, 0xfc, 0xf8) )
				break;
			gbm.bpp = 24;
			break;
		}
fOk = MakeBitmap(HWND_DESKTOP, &gbm, gbmrgb, pbData, &hbmBmp);
if ( gbm.bpp == 1 )
/*...sremember Bg and Fg colours:32:*/
{
lColorBg = (gbmrgb [0].r << 16) + (gbmrgb [0].g << 8) + gbmrgb [0].b;
lColorFg = (gbmrgb [1].r << 16) + (gbmrgb [1].g << 8) + gbmrgb [1].b;
}
/*...e*/

free(pbData);
if ( fOk )
/*...sall ok\44\ do PM windows etc\46\:32:*/
{
RECTL rcl;
SHORT cxScreen, cyScreen, cxWindow, cyWindow;
BITMAPINFOHEADER bmp;

WinRegisterClass(
	hab,			/* Anchor block handle               */
	WC_GBMV,		/* Window class name                 */
	GbmVWndProc,		/* Responding procedure for class    */
	CS_SIZEREDRAW,		/* Class style                       */
	0);			/* Extra bytes to reserve for class  */

hwndFrame = WinCreateStdWindow(
	HWND_DESKTOP,		/* Parent window handle              */
	0L,			/* Style of frame window             */
	&flFrameFlags,		/* Pointer to control data           */
	WC_GBMV,		/* Client window class name          */
	NULL,			/* Title bar text                    */
	0L,			/* Style of client window            */
	(HMODULE) NULL,		/* Module handle for resources       */
	RID_STANDARD,		/* ID of resources                   */
	&hwndClient);		/* Pointer to client window handle   */

strcpy(sz, argv [i]);
if ( szOpt [0] )
	{
	strcat(sz, ",");
	strcat(sz, szOpt);
	}
if ( fErrdiff )
	strcat(sz, " -e");
else if ( fHalftone )
	strcat(sz, " -h");
WinSetWindowText(hwndFrame, sz);

GpiQueryBitmapParameters(hbmBmp, &bmp);

rcl.xLeft   = 0L;
rcl.xRight  = bmp.cx;
rcl.yBottom = 0L;
rcl.yTop    = bmp.cy;
WinCalcFrameRect(hwndFrame, &rcl, FALSE);

cxWindow = (SHORT) (rcl.xRight - rcl.xLeft),
cyWindow = (SHORT) (rcl.yTop - rcl.yBottom),
cxScreen = (SHORT) WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
cyScreen = (SHORT) WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);

WinSetWindowPos(hwndFrame,
		HWND_TOP,
		(cxScreen - cxWindow) / 2,
		(cyScreen - cyWindow) / 2,
		cxWindow,
		cyWindow,
		SWP_MOVE | SWP_SIZE | SWP_ACTIVATE | SWP_SHOW);

while ( WinGetMsg(hab, &qMsg, (HWND) NULL, 0, 0) )
	WinDispatchMsg(hab, &qMsg);

WinDestroyWindow(hwndFrame);
}
/*...e*/
}
/*...e*/
		else
			{
			free(pbData);
			Error(HWND_DESKTOP, "Out of memory");
			}
		}

	WinDestroyMsgQueue(hmq);
	WinTerminate(hab);

	gbm_deinit();

	return ( 0 );
	}
/*...e*/
