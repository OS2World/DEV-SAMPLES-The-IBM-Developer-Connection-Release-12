
#ifndef _GTKUTIL_
#define _GTKUTIL_

typedef  LHANDLE  GTKDEV;                      // I/O device (file, buffer,...)

extern MATRIXLF mx0;
#define  FIX1    (MAKEFIXED(1,0))

#define  BOOL_XOR(a,b) ( ((a) && !(b)) || (!(a) && (b)) )

#define  PointInRect(a,b)                                                     \
              (BOOL)( (a)->x >= (b)->xLeft   && (a)->x <= (b)->xRight &&      \
                      (a)->y >= (b)->yBottom && (a)->y <= (b)->yTop      )

#define SetStatusTextFromResource(itemID,resID)                               \
                       WinLoadString( (HAB)0, _hmod, resID, 255, str255 );    \
                       _fMenuBar->SetMenuItemStatusText(ev, itemID, str255)

#define SetPopupStatusTextFromResource(itemID,resID)                          \
                       WinLoadString( (HAB)0, _hmod, resID, 255, str255 );    \
                       fPopup->SetMenuItemStatusText(ev, itemID, str255)

VOID    put_pointer( HWND hwnd, HPS hps, PPOINTL ptlMW );
VOID    null_mousemove();
HBITMAP read_bitmap( HPS hps, char* szFilename );
BOOL    key_shift( SHORT vk_key );

LONG   FixLongMult( FIXED f, LONG l );
FIXED  FixFixMult( FIXED f, FIXED l );
LONG   LongFixDiv( LONG l, FIXED f );
FIXED  FixFixDiv( FIXED l, FIXED f );

POINTL scaledPt( MATRIXLF *mx, PPOINTL pt0 );
VOID transformPts( MATRIXLF *mx, PPOINTL ptl, LONG npts );
MATRIXLF multiplyXforms( PMATRIXLF pre, PMATRIXLF post );
VOID invertPts( MATRIXLF *mx, PPOINTL ptl, LONG npts );
BOOL invertXform( PMATRIXLF mxIn, PMATRIXLF mxOut );

LONG  Rsin2 ( LONG radius, FIXED angl );
LONG  Rcos2 ( LONG radius, FIXED angl );

//---------------------------------------------------------------------------
typedef struct {
        POINTL   center, firstpt;    //, lastpt;
        BOOL     fDrawn;
        BOOL     fMode;
        LONG     n;
        POINTL   ptls[74];                        // 360/5 + 2
        FIXED    fxA1, fxA2;
        LONG     r;
        } FSMARCDATA, *PFSMARCDATA;

#define PI                  3.1415926536
#define TWO_PI              (PI*2.0)
#define RADTODEG            (360.0/TWO_PI)
#define DEGTORAD            (TWO_PI/360.0)

#define D2FIXED(d)          ((FIXED)(d*65536.0))

POINTL angle_point( PPOINTL ptl0, FIXED angle, FIXED radius );
BOOL calc_angle( PPOINTL p0, PPOINTL p1, PFIXED angle, PLONG radius );
BOOL get_angle( PFSMARCDATA pAD, PPOINTL p1, BOOL discrete );
BOOL calc_sweep( PFSMARCDATA pAD, PPOINTL p2, BOOL discrete );
BOOL calc_matrix( PRECTL prclTarget, PRECTL prclSource, MATRIXLF* mx );
BOOL calc_rotate( FIXED angle, PPOINTL ptl, MATRIXLF* mx );

void AddPt( PRECTL rcl, PPOINTL ptl );
void AddRect( PRECTL rclBounds, PRECTL rcl );
void InflateRect( PRECTL rcl, long cx, long cy );
void OffsetRect( PRECTL rcl, long dx, long dy);
BOOL RectsIntersect( PRECTL rcl1, PRECTL rcl2 );

#endif
