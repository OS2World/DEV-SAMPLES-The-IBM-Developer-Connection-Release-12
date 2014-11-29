
#define INCL_GPIBITMAPS
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES
#define INCL_GPIREGIONS
#define INCL_GPITRANSFORMS
#define INCL_GPILOGCOLORTABLE
#define INCL_GPIPATHS

#define INCL_WIN
#define INCL_WINPOINTERS

#define INCL_ODAPI
#define INCL_ODFACET
#define INCL_ODLINK
#define INCL_ODSTORAGEUNIT

#include <os2.h>

#ifndef _ORDCOLL_
#include "OrdColl.h"
#endif

#include "gtkpart.xih"
#include "gtkbase.h"
#include "gtkpic.h"
#include "mygpi.h"

extern MATRIXLF mx0;

// -----------------------------------------------------------------------
POINTL CursorPosition( Environment* ev, ODFacet* facet )
{
  POINTL ptl;
  WinQueryPointerPos( HWND_DESKTOP, &ptl );
  WinMapWindowPoints( HWND_DESKTOP, facet->GetFacetHWND(ev), &ptl, 1 );
  return ptl;
}

// -----------------------------------------------------------------------
HPS CreateHps( ULONG cx, ULONG cy, ULONG hType )
{
  SIZEL sizlPage = {cx, cy};
  LONG  ht = (hType==HPS_BITMAP ? OD_MEMORY : OD_METAFILE);

  HDC hdc = DevOpenDC( 1, ht, "*", 0, NULL, 0) ;
  HPS hps = GpiCreatePS( 1, hdc, &sizlPage, PU_PELS    | GPIF_DEFAULT |
                                            GPIT_MICRO | GPIA_ASSOC) ;

  if (hType==HPS_BITMAP) {
    // put this in 'cause WARP won't let us use hps without bitmap... :(
    long cColorPlanes, cColorBitcount;
    DevQueryCaps (hdc, CAPS_COLOR_PLANES, 1L, &cColorPlanes);
    DevQueryCaps (hdc, CAPS_COLOR_BITCOUNT, 1L, &cColorBitcount);

    BITMAPINFOHEADER bmp;
    bmp.cbFix = sizeof(BITMAPINFOHEADER);
    bmp.cx = cx;
    bmp.cy = cy;
    bmp.cPlanes = (SHORT)cColorPlanes;
    bmp.cBitCount = (SHORT)cColorBitcount;
    HBITMAP hbm = GpiCreateBitmap( hps, (PBITMAPINFOHEADER2)&bmp,
                                   0x0000, (PBYTE)NULL, (PBITMAPINFO2)NULL);
    GpiSetBitmap( hps, hbm);
  }

  return hps;
}

void ReleaseHps( HPS hps )
{
  if (hps) {
    // put this in 'cause WARP won't let us use hps without bitmap... :(
    HBITMAP hbm = GpiSetBitmap( hps, NULLHANDLE);
    if (hbm) GpiDeleteBitmap (hbm);

    GpiDestroyPS(hps);
//    if (_hdcMem) DevCloseDC(_hdcMem);
  }
}

// ---------------------------------------------------------
void FillRect( HPS hps, RECTL *rcl, RGBColor color )
{
  GpiSetColor( hps, color );
  GpiMove( hps, (PPOINTL)&rcl->xLeft);
  GpiBox( hps, DRO_FILL, (PPOINTL)&rcl->xRight, 0, 0);
}

// ---------------------------------------------------------
VOID SetScaling( HPS hps, PRECTL prclSource, PRECTL prclTarget, BOOL fStretch )
{
   #define VIEWTRANSFORM
   LONG lxPictRight, lxPictLeft;
   LONG lyPictTop, lyPictBottom;          /* picture boundary information */
   LONG lxPictCentre, lyPictCentre;       /* picture center               */
   LONG clxPict, clyPict;                 /* picture extents              */
   LONG clxBounds, clyBounds;             /* bounds area extent           */
   LONG lxScale, lyScale;                 /* scaling factors              */
   LONG lxTrans, lyTrans;                 /* final translation            */
   SHORT sxWork, syWork;                  /* work areas                   */
   LONG lxWork, lyWork;                   /* work areas                   */

   MATRIXLF matlfViewTransform;
   POINTL   ptl;
   ULONG    eID;

   /*********************************************************************/
   /* From the bounding rectangle of the picture, calculate its center. */
   /*********************************************************************/
   lxPictLeft   = prclSource->xLeft;
   lyPictBottom = prclSource->yBottom;
   lxPictRight  = prclSource->xRight;
   lyPictTop    = prclSource->yTop;

   ptl.x = lxPictCentre = (lxPictRight - lxPictLeft)/2 + lxPictLeft;
   ptl.y = lyPictCentre = (lyPictTop - lyPictBottom)/2 + lyPictBottom;

   #ifdef VIEWTRANSFORM
     GpiSetDefaultViewMatrix( hps, 0, NULL, TRANSFORM_REPLACE );
   #else
     GpiSetModelTransformMatrix( hps, 0, NULL, TRANSFORM_REPLACE );
   #endif

//   GpiSetModelTransformMatrix( hps, 0, NULL, TRANSFORM_REPLACE );
   GpiSetModelTransformMatrix( hps, 9, &mx0, TRANSFORM_REPLACE );  // 143738

   /*********************************************************************/
   /* Get the picture extents.                                          */
   /*********************************************************************/
   clxPict = lxPictRight - lxPictLeft;
   clyPict = lyPictTop - lyPictBottom;

   /*********************************************************************/
   /* Get the current bounding rectangle extent.                        */
   /*********************************************************************/
   clxBounds = prclTarget->xRight - prclTarget->xLeft;
   clyBounds = prclTarget->yTop - prclTarget->yBottom;

   if (fStretch) {
     sxWork = (SHORT)(clxBounds/clxPict);
     lxWork = clxBounds%clxPict;
     lxScale = MAKEULONG((USHORT)((lxWork * 0x10000)/clxPict), sxWork);

     syWork = (SHORT)(clyBounds/clyPict);
     lyWork = clyBounds%clyPict;
     lyScale = MAKEULONG((USHORT)((lyWork * 0x10000)/clyPict), syWork);
   } else {
     sxWork = (SHORT)(clyBounds/clxBounds);
     lxWork = clyBounds%clxBounds;
     lxScale = MAKEULONG((USHORT)((lxWork * 0x10000)/clxBounds), sxWork);

     syWork = (SHORT)(clyPict/clxPict);
     lyWork = clyPict%clxPict;
     lyScale = MAKEULONG((USHORT)((lyWork * 0x10000)/clxPict), syWork);

     /*****************************************************************/
     /* Compare the two ratios to determine the dimension along which */
     /* scaling is to be provided, so that the picture fits into the  */
     /* area and its aspect ratio is preserved.  The same scaling is  */
     /* then applied along both the x and y axes.                     */
     /*****************************************************************/

     if (lxScale > lyScale) {
       sxWork = (SHORT)(clxBounds/clxPict);
       lxWork = clxBounds%clxPict;
       lxScale = MAKEULONG((USHORT)((lxWork * 0x10000)/clxPict), sxWork);
       lyScale = lxScale;
     }
     else {
       syWork = (SHORT)(clyBounds/clyPict);
       lyWork = clyBounds%clyPict;
       lyScale = MAKEULONG((USHORT)((lyWork * 0x10000)/clyPict), syWork);
       lxScale = lyScale;
     }
   }

   matlfViewTransform.fxM11 = lxScale;
   matlfViewTransform.fxM12 = 0;
   matlfViewTransform.lM13  = 0;
   matlfViewTransform.fxM21 = 0;
   matlfViewTransform.fxM22 = lyScale;
   matlfViewTransform.lM23  = 0;
   matlfViewTransform.lM31  = 0;
   matlfViewTransform.lM32  = 0;
   matlfViewTransform.lM33  = 1;

   #ifdef VIEWTRANSFORM
     GpiSetDefaultViewMatrix( hps, 9, &matlfViewTransform, TRANSFORM_ADD );
   #else
     GpiSetModelTransformMatrix( hps, 9, &matlfViewTransform, TRANSFORM_ADD );
   #endif

   GpiConvert( hps, CVTC_WORLD, CVTC_PAGE, 1, &ptl );

   /*********************************************************************/
   /* Translate the center of the picture to the center of the          */
   /* bounding area, based on the page origin.                          */
   /*********************************************************************/
   lxTrans = (prclTarget->xRight + prclTarget->xLeft)/2 - ptl.x;
   lyTrans = (prclTarget->yTop + prclTarget->yBottom)/2 - ptl.y;

   matlfViewTransform.fxM11 = 65536;
   matlfViewTransform.fxM12 = 0;
   matlfViewTransform.lM13  = 0;
   matlfViewTransform.fxM21 = 0;
   matlfViewTransform.fxM22 = 65536;
   matlfViewTransform.lM23  = 0;
   matlfViewTransform.lM31  = lxTrans;
   matlfViewTransform.lM32  = lyTrans;
   matlfViewTransform.lM33  = 1;

   #ifdef VIEWTRANSFORM
     GpiSetDefaultViewMatrix( hps, 9, &matlfViewTransform, TRANSFORM_ADD );
   #else
     GpiSetModelTransformMatrix( hps, 9, &matlfViewTransform, TRANSFORM_ADD );
   #endif
   }

// -----------------------------------------------------------------------
ODBoolean GtkPointer::Init( HMODULE hmod, ULONG ptrId )
{
  if (_hptr)
    WinDestroyPointer( _hptr );

  _hptr = WinLoadPointer( HWND_DESKTOP, hmod, ptrId );
  _hbm = 0;

  return (_hptr != 0);
}

// -----------------------------------------------------------------------
void GtkPointer::Select( ULONG viewType )
{
  if (_hptr) {
    // select the bitmap
    POINTERINFO pi;
    WinQueryPointerInfo( _hptr, &pi );
    _hbm = (viewType == IT_LARGEICON) ? pi.hbmColor : pi.hbmMiniColor;

    // query its info
    BITMAPINFOHEADER2 bih2;
    bih2.cbFix = sizeof(BITMAPINFOHEADER2);
    GpiQueryBitmapInfoHeader( _hbm, &bih2 );

    // adjust the frame size
    _cx = bih2.cx;
    _cy = bih2.cy;
  }
}

// -----------------------------------------------------------------------
ODBoolean GtkPointer::SetCurrent()
{
  if (_hptr) {
    WinSetPointer( HWND_DESKTOP, _hptr );
    return kODTrue;
  }

  return kODFalse;
}

