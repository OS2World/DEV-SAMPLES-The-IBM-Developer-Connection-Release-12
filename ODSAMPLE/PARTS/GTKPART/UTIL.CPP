
#define  INCL_BASE
#define  INCL_WIN
#define  INCL_GPI

#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Util.h"

//---------------------------------------------------------------------------
VOID  put_pointer( HWND hwnd, HPS hps, PPOINTL ptlMW )
{
  POINTL ptlLoc;
  ptlLoc = *ptlMW;

  GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 1L, &ptlLoc );
  WinMapWindowPoints( hwnd, HWND_DESKTOP, &ptlLoc, 1L);
  WinSetPointerPos( HWND_DESKTOP, (SHORT) ptlLoc.x, (SHORT) ptlLoc.y);
}

//----------------------------------------------------------------------------------------
VOID null_mousemove()
{
  POINTL ptl;
  WinQueryPointerPos( HWND_DESKTOP, &ptl );
  WinSetPointerPos( HWND_DESKTOP, ptl.x, ptl.y );
}

//---------------------------------------------------------------------------
HBITMAP read_bitmap( HPS hps, char* szFilename )
{
  PBYTE        Bitmap;
  HFILE        FileHandle;
  ULONG        Action;
  FILESTATUS3  FileInfoBuf;
  ULONG        BytesRead;
  HBITMAP      hbm;

  if (0 == DosOpen(szFilename, &FileHandle, &Action, 0L, FILE_NORMAL, 1L, 0x92, 0)) {
    DosQueryFileInfo(FileHandle, 1L, &FileInfoBuf, sizeof( FILESTATUS3 ));

    DosAllocMem( (PPVOID)&Bitmap, FileInfoBuf.cbFile, PAG_COMMIT | PAG_READ | PAG_WRITE);
    DosRead( FileHandle, Bitmap, FileInfoBuf.cbFile, &BytesRead );
    DosClose(FileHandle);

    USHORT* pUS = (USHORT*)Bitmap;
    PBITMAPFILEHEADER2   pbfh2 = 0;

    if (*pUS==BFT_BITMAPARRAY) {
      ULONG cx, cy, offset = 0;

      cx = WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
      cy = WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );

      PBITMAPARRAYFILEHEADER2   pbafh2 = (PBITMAPARRAYFILEHEADER2)Bitmap;
      while (pbafh2 && pbafh2->cxDisplay != cx) {
        offset += pbafh2->offNext;
        if (!pbafh2->offNext || offset >= BytesRead) {
          pbafh2 = 0;
        } else {
          pbafh2 = (PBITMAPARRAYFILEHEADER2)(Bitmap + offset);
        }
      }

      if (!pbafh2)
        pbafh2 = (PBITMAPARRAYFILEHEADER2)Bitmap;

      pbfh2 = &pbafh2->bfh2;

    } else if (*pUS==BFT_BMAP) {

      pbfh2 = (PBITMAPFILEHEADER2)Bitmap;
    }
    else return NULL;

    hbm = GpiCreateBitmap( hps, &pbfh2->bmp2, CBM_INIT,
                           Bitmap + pbfh2->offBits, (PBITMAPINFO2)&pbfh2->bmp2);

    DosFreeMem( Bitmap );
  }

  return hbm;
}

//---------------------------------------------------------------------------
void CenterWindow (HWND hwnd)
{
  HWND hwndParent;
  RECTL rcl;
  RECTL rclParent;

  WinQueryWindowRect( hwnd, &rcl );

  hwndParent = WinQueryWindow( hwnd, QW_PARENT );
  WinQueryWindowRect( hwndParent, &rclParent );

  rcl.xRight/=2;
  rcl.yTop/=2;
  rclParent.xRight/=2;
  rclParent.yTop/=2;

  WinSetWindowPos( hwnd, NULL, (rclParent.xRight - rcl.xRight),
                               (rclParent.yTop - rcl.yTop), 0, 0, SWP_MOVE );
}

//---------------------------------------------------------------------------
MRESULT EXPENTRY PartInfoDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  switch (msg) {
    case WM_INITDLG:
      CenterWindow (hwnd);
      break;
    case WM_COMMAND:
      {
        short buttonID = SHORT1FROMMP(mp1);
        if (buttonID==DID_OK || buttonID==DID_CANCEL)
          WinDismissDlg(hwnd, buttonID);
      }
      break;
    default:
      return(WinDefDlgProc(hwnd, msg, mp1, mp2));
      break;
  }
  return 0;
}


//---------------------------------------------------------------------------
// VK_SHIFT  VK_CTRL  VK_ALT  VK_CAPSLOCK  VK_NUMLOCK  VK_SCRLLOCK VK_ALTGRAF
//---------------------------------------------------------------------------
BOOL  key_shift( SHORT vk_key )
{
  SHORT key_state = WinGetKeyState( HWND_DESKTOP, vk_key );
  return (BOOL)( key_state & 0x8000 );
}

FIXED FIXEDABS( FIXED f )
{ return abs(f); }
//{ return MAKEFIXED( abs(FIXEDINT(f)), FIXEDFRAC(f) ); }

//---------------------------------------------------------------------------
LONG FixLongMult( FIXED f, LONG l )
{
  LONG result;
  BOOL fNeg = (l<0 && f>=0) || (l>=0 && f<0);
  l = labs(l);
  f = FIXEDABS(f);
//  result = l*FIXEDINT(f) + ((LONG)HIUSHORT(l))*FIXEDFRAC(f) + (((LONG)LOUSHORT(l))*FIXEDFRAC(f))>>16;
  result = l*FIXEDINT(f);
  result += ((ULONG)HIUSHORT(l))*FIXEDFRAC(f);
  result += (((ULONG)LOUSHORT(l))*FIXEDFRAC(f))>>16;
  return fNeg ? -result : result;
}

//---------------------------------------------------------------------------
FIXED FixFixMult( FIXED f, FIXED l )
{
  LONG result;
  BOOL fNeg = (l<0 && f>=0) || (l>=0 && f<0);
  l = FIXEDABS(l);
  f = FIXEDABS(f);
//  result = l*HIUSHORT(f) + ((LONG)HIUSHORT(l))*LOUSHORT(f) + (((LONG)LOUSHORT(l))*LOUSHORT(f))>>16;
//  result = l*FIXEDINT(f) + ((LONG)HIUSHORT(l))*FIXEDFRAC(f) + (((LONG)LOUSHORT(l))*FIXEDFRAC(f))>>16;
  result = l*FIXEDINT(f);
  result += ((ULONG)HIUSHORT(l))*FIXEDFRAC(f);
  result += (((ULONG)LOUSHORT(l))*FIXEDFRAC(f))>>16;
  return fNeg ? -result : result;
}

//---------------------------------------------------------------------------
FIXED FixFixDiv( FIXED l, FIXED f )
{
  #if 1
    return (LONG)( (float)l*65536.0/(float)f );
  #else
    FIXED result;
    BOOL fNeg = (l<0 && f>=0) || (l>=0 && f<0);
    l = FIXEDABS(l);
    f = FIXEDABS(f);
    result = ((l&0xffff0000)/f)<<16;
    result += ( ((ULONG)(l&0x0000ffff))<<16 ) / f;
  //  return fNeg ? MAKEFIXED(-FIXEDINT(result),FIXEDFRAC(result)) : result;
    return fNeg ? -result : result;
  #endif
}

//---------------------------------------------------------------------------
LONG LongFixDiv( LONG l, FIXED f )
{
  #if 1
    return (LONG)( (float)l*65536.0/(float)f );
  #else
    LONG result;
    BOOL fNeg = (l<0 && f>=0) || (l>=0 && f<0);
    l = labs(l);
    f = FIXEDABS(f);
    result = ((l&0xffff0000)/f)<<16;
    result += ( ((ULONG)(l&0x0000ffff))<<16 ) / f;
    return fNeg ? -result : result;
  #endif
}


//---------------------------------------------------------------------------
FIXED LongLongDiv( LONG l1, LONG l2 )
{
  LONG result;
  BOOL fNeg = (l1<0 && l2>=0) || (l1>=0 && l2<0);
  result = MAKEFIXED( (int)(labs(l1)/labs(l2)), (labs(l1)*0x10000)/labs(l2) );
  return fNeg ? -result : result;
}

// ---------------------------------------------------------------------------
POINTL scaledPt( MATRIXLF *mx, PPOINTL pt0 )
{
  POINTL pt;
  pt.x = FixLongMult(mx->fxM11,pt0->x) + FixLongMult(mx->fxM21,pt0->y) + mx->lM31;
  pt.y = FixLongMult(mx->fxM12,pt0->x) + FixLongMult(mx->fxM22,pt0->y) + mx->lM32;
  return pt;
}

// ---------------------------------------------------------------------------
VOID transformPts( MATRIXLF *mx, PPOINTL ptls, LONG npts )
{
  POINTL pt;
  for (int i=0; i<npts; i++ ) {
    pt.x = FixLongMult(mx->fxM11,ptls[i].x) +
           FixLongMult(mx->fxM21,ptls[i].y) + mx->lM31;
    pt.y = FixLongMult(mx->fxM12,ptls[i].x) +
           FixLongMult(mx->fxM22,ptls[i].y) + mx->lM32;
    ptls[i] = pt;
  }
}

// --------------------------------------------------------------------------
MATRIXLF multiplyXforms( PMATRIXLF pre, PMATRIXLF post )
{
  MATRIXLF result;

  result = mx0;
  result.fxM11 = FixFixMult(pre->fxM11,post->fxM11) + FixFixMult(pre->fxM12,post->fxM21);
  result.fxM12 = FixFixMult(pre->fxM11,post->fxM12) + FixFixMult(pre->fxM12,post->fxM22);
  result.fxM21 = FixFixMult(pre->fxM21,post->fxM11) + FixFixMult(pre->fxM22,post->fxM21);
  result.fxM22 = FixFixMult(pre->fxM21,post->fxM12) + FixFixMult(pre->fxM22,post->fxM22);
  result.lM31  = FixLongMult(post->fxM11,pre->lM31) +
                 FixLongMult(post->fxM21,pre->lM32) + post->lM31;
  result.lM32  = FixLongMult(post->fxM12,pre->lM31) +
                 FixLongMult(post->fxM22,pre->lM32) + post->lM32;

  return result;
}

//---------------------------------------------------------------------------
BOOL invertXform( PMATRIXLF mxIn, PMATRIXLF mxOut )
{
  LONG d;
  LONG a,b;

  d = FixFixMult(mxIn->fxM11,mxIn->fxM22) - FixFixMult(mxIn->fxM12,mxIn->fxM21);

  *mxOut = mx0;
  if (!d)  return FALSE;                        // can't be inverted, return I

  float d0 = (float)d;

  #if 1
    mxOut->fxM11 =  (FIXED)( (float)mxIn->fxM22*65536.0/d0 );
    mxOut->fxM21 = -(FIXED)( (float)mxIn->fxM21*65536.0/d0 );
    mxOut->fxM12 = -(FIXED)( (float)mxIn->fxM12*65536.0/d0 );
    mxOut->fxM22 =  (FIXED)( (float)mxIn->fxM11*65536.0/d0 );
  #else
    mxOut->fxM11 =  FixFixDiv( mxIn->fxM22, d );
    mxOut->fxM21 = -FixFixDiv( mxIn->fxM21, d );
    mxOut->fxM12 = -FixFixDiv( mxIn->fxM12, d );
    mxOut->fxM22 =  FixFixDiv( mxIn->fxM11, d );
  #endif

  a = FixLongMult(mxIn->fxM21,mxIn->lM32);
  b = FixLongMult(mxIn->fxM22,mxIn->lM31);
  mxOut->lM31 = LongFixDiv( a - b, d );
  a = FixLongMult(mxIn->fxM11,mxIn->lM32);
  b = FixLongMult(mxIn->fxM12,mxIn->lM31);
  mxOut->lM32 = LongFixDiv( b - a, d );
  return TRUE;
}

// ---------------------------------------------------------------------------
VOID invertPts( MATRIXLF *mx, PPOINTL ptl, LONG npts )
{
  MATRIXLF inverse;
  if (invertXform(mx, &inverse))
    transformPts( &inverse, ptl, npts );
}

//--------------------------------------------------------------------------------
//  calculate r * sin ( angle )
//--------------------------------------------------------------------------------
LONG  Rsin2 ( LONG radius, FIXED angl )
{
   // old fashion table look up method; 91 entries covering 0 - 90 degree inclusive
   // the value should be divided by 65536L
   static LONG SinTabl[] = {     0L,  1144L,  2287L,  3430L,  4572L,  5712L,
                              6850L,  7987L,  9121L, 10252L, 11380L, 12505L,
                             13626L, 14742L, 15855L, 16962L, 18064L, 19161L,
                             20252L, 21336L, 22415L, 23486L, 24550L, 25607L,
                             26656L, 27697L, 28729L, 29753L, 30767L, 31772L,
                             32768L, 33754L, 34729L, 35693L, 36647L, 37590L,
                             38521L, 39441L, 40348L, 41243L, 42126L, 42995L,
                             43852L, 44695L, 45525L, 46341L, 47143L, 47930L,
                             48703L, 49461L, 50203L, 50931L, 51643L, 52339L,
                             53020L, 53684L, 54332L, 54963L, 55578L, 56175L,
                             56756L, 57319L, 57865L, 58393L, 58903L, 59396L,
                             59870L, 60326L, 60764L, 61183L, 61584L, 61966L,
                             62328L, 62672L, 62997L, 63303L, 63589L, 63856L,
                             64104L, 64332L, 64540L, 64729L, 64898L, 65048L,
                             65177L, 65287L, 65376L, 65446L, 65496L, 65526L,
                             65536L };
   LONG   lVal, rVal;
   int    anglInt;
   LONG   lAnglFrac;
   BOOL   negative;

   if (radius<0) {
      radius = -radius;
      angl -= MAKEFIXED(180,0);
      }

   while( angl <  0L               ) angl += MAKEFIXED(360,0);
   while( angl >= MAKEFIXED(360,0) ) angl -= MAKEFIXED(360,0);

   if ( angl > MAKEFIXED(180,0) ) {
      negative = TRUE ;
      angl -= MAKEFIXED(180,0);
      }
   else negative = FALSE;

   if (angl > MAKEFIXED(90,0) )  angl = MAKEFIXED(180,0) - angl;

   anglInt = FIXEDINT( angl );
   lAnglFrac = MAKEFIXED( 0, FIXEDFRAC(angl) );
//   lAnglFrac = (LONG)FIXEDFRAC( angl );

   lVal = SinTabl[anglInt] ;

   if ( lAnglFrac )
      lVal += FixFixMult( lAnglFrac, SinTabl[anglInt+1] - SinTabl[anglInt] );
//      lVal += ( (lAnglFrac*( SinTabl[anglInt+1] - SinTabl[anglInt] )) >> 16 );

   rVal = FixLongMult( lVal, radius );
//   rVal = ( ( radius * lVal ) >> 16 );

   return ( negative ? -rVal : rVal );
   }

//-------------------------------------------------------------------------
//  r * cos ( angle ) = r * sin ( 90 degree - angle )
//-------------------------------------------------------------------------

LONG  Rcos2 ( LONG  radius, FIXED angl )
{ return Rsin2( radius, MAKEFIXED(90,0) - angl ); }

//---------------------------------------------------------------------------
POINTL angle_point( PPOINTL ptl0, FIXED angle, FIXED radius )
{
  POINTL pt;
  pt.x = ptl0->x + Rcos2( radius, angle );
  pt.y = ptl0->y + Rsin2( radius, angle );
  return pt;
}

//---------------------------------------------------------------------------
BOOL calc_angle( PPOINTL p0, PPOINTL p1, PFIXED angle, PLONG radius )
{
  if ( p0->x == p1->x && p0->y == p1->y ) return FALSE;

  LONG   dx, dy;
  dx = p1->x - p0->x;
  dy = p1->y - p0->y;

  double a0 = atan2( dy, dx);
  *angle =  D2FIXED(a0 * RADTODEG);
  if ( *angle<0 ) *angle += MAKEFIXED(360,0);

  if (radius) *radius = (LONG)sqrt( dx*dx + dy*dy );
  return TRUE;
}

//---------------------------------------------------------------------------
BOOL get_angle( PFSMARCDATA pAD, PPOINTL p1, BOOL discrete )
{
  PPOINTL p0 = &pAD->center;
  if (!calc_angle( p0, p1, &pAD->fxA1, &pAD->r ) ) return FALSE;

  if (discrete && MAKEFIXED( (FIXEDINT(pAD->fxA1)/15)*15, 0 )!=pAD->fxA1 ) {
    pAD->fxA1 += MAKEFIXED(7,32768);
    pAD->fxA1 = MAKEFIXED( (FIXEDINT(pAD->fxA1)/15)*15, 0 );
    *p1 = angle_point( p0, pAD->fxA1, pAD->r );
  }
  return TRUE;
}

//---------------------------------------------------------------------------
BOOL calc_sweep( PFSMARCDATA pAD, PPOINTL p2, BOOL discrete )
{
  if (!calc_angle( &pAD->center, p2, &pAD->fxA2, NULL ) ) return FALSE;

  if ( pAD->fxA2<pAD->fxA1 ) pAD->fxA2 += MAKEFIXED(360,0);

  if (discrete) {
    FIXED fxA = pAD->fxA2 - pAD->fxA1;
    if (fxA != MAKEFIXED( (FIXEDINT(fxA)/15)*15, 0 ) ) {
      fxA += MAKEFIXED(7,32768);
      fxA = MAKEFIXED( (FIXEDINT(fxA)/15)*15, 0 );
      pAD->fxA2 = pAD->fxA1 + fxA;
    }
  }
  return TRUE;
}

//---------------------------------------------------------------------------
BOOL  calc_matrix( PRECTL prclTarget, PRECTL prclSource, MATRIXLF* mx )
{
  LONG     sdx, sdy, tdx, tdy;

  tdx = (LONG) ( prclTarget->xRight - prclTarget->xLeft );
  tdy = (LONG) ( prclTarget->yTop - prclTarget->yBottom );
  sdx = (LONG) ( prclSource->xRight - prclSource->xLeft );
  sdy = (LONG) ( prclSource->yTop - prclSource->yBottom );

  if (tdx == 0L || tdy ==0L || sdx == 0L || sdy == 0L ) {
    *mx = mx0;
    return FALSE;
  }

  mx->fxM11 = LongLongDiv( tdx, sdx );
  mx->fxM22 = LongLongDiv( tdy, sdy );
  mx->fxM12 = mx->fxM21 = 0;

  mx->lM31 = prclTarget->xLeft - FixLongMult( mx->fxM11, prclSource->xLeft );
  mx->lM32 = prclTarget->yBottom - FixLongMult( mx->fxM22, prclSource->yBottom );

  return TRUE;
}

//-------------------------------------------------------------------------
BOOL calc_rotate( FIXED angle, PPOINTL ptl, MATRIXLF* mx)
{
  FIXED fSin, fCos;

  *mx = mx0;
  fSin = Rsin2( FIX1, angle );
  fCos = Rcos2( FIX1, angle );
  mx->fxM11 = fCos;
  mx->fxM12 = fSin;
  mx->fxM21 =-fSin;
  mx->fxM22 = fCos;
  mx->lM31  = FixLongMult( FIX1-fCos, ptl->x ) + FixLongMult( fSin, ptl->y );
  mx->lM32  = FixLongMult( FIX1-fCos, ptl->y ) - FixLongMult( fSin, ptl->x );
  return TRUE;
}

//-------------------------------------------------------------------------
void AddPt( PRECTL rcl, PPOINTL ptl )
{
  if (ptl->x < rcl->xLeft) rcl->xLeft = ptl->x;
  if (ptl->x > rcl->xRight) rcl->xRight = ptl->x;
  if (ptl->y < rcl->yBottom) rcl->yBottom = ptl->y;
  if (ptl->y > rcl->yTop) rcl->yTop = ptl->y;
}

//-------------------------------------------------------------------------
void AddRect( PRECTL rclBounds, PRECTL rcl )
{
  if (rcl->xLeft<rclBounds->xLeft) rclBounds->xLeft = rcl->xLeft;
  if (rcl->xRight>rclBounds->xRight) rclBounds->xRight = rcl->xRight;
  if (rcl->yBottom<rclBounds->yBottom) rclBounds->yBottom = rcl->yBottom;
  if (rcl->yTop>rclBounds->yTop) rclBounds->yTop = rcl->yTop;
}

// -------------------------------------------------------------------------
void   InflateRect( PRECTL rcl, LONG cx, LONG cy )
{
  rcl->xRight  += cx;
  rcl->xLeft   -= cx;
  rcl->yTop    += cy;
  rcl->yBottom -= cy;
}

void OffsetRect( PRECTL rcl, long dx, long dy)
{
   rcl->xLeft   += dx;
   rcl->xRight  += dx;
   rcl->yBottom += dy;
   rcl->yTop    += dy;
}

//-------------------------------------------------------------------------
BOOL RectsIntersect( PRECTL rcl1, PRECTL rcl2 )
{
  return (rcl1->xLeft <= rcl2->xRight && rcl2->xLeft <= rcl1->xRight &&
          rcl1->yBottom <= rcl2->yTop && rcl2->yBottom <= rcl1->yTop     );
}

