/*
 *      File:           Common.hpp
 *
 *   Contains: In-line helper functions, macros and classes for
 *             sample parts.
 *
 *   Written by:        Jason Crawford
 *
 *   Copyright: (c) 1994 by IBM Corp., all rights reserved.
 *
 *   Change History (most recent first):
 *
 *    124384      6/05/95       map  moved function definition to common.cpp file
 *    119548      4/25/95       jso  removed utils for byte-arrays
 *                                   as they were made public in opendoc.lib
 *    120017      4/22/95       ced  Added utils for dealing with byte-arrays
 *                                   and ref-counted objects.
 *       <2>      4/01/95       ced  add overloaded variations on InflateRect
 *       <1>      7/6/94        jlc  first checked in
 *
 *
 *   Copyright: (c) 1993-1994 by IBM Corp., all rights reserved.
 *
 */
#ifndef _COMMON_
#define _COMMON_

#if ODDebug
   #define NOTDONE() beepprintf("not done in %s", __FILE__)
   #define NOTDONE2(a) beepprintf("%s not done in %s", a, __FILE__)
#else
   #define NOTDONE()
   #define NOTDONE2(a)
#endif

   void inline InflateRect( PRECTL prectl, long dx, long dy )
   {
      prectl->xLeft   -= dx;
      prectl->xRight  += dx;
      prectl->yBottom -= dy;
      prectl->yTop    += dy;
   }

   void inline InflateRect( RECTL &rectl, long dx, long dy )
   {
      rectl.xLeft   -= dx;
      rectl.xRight  += dx;
      rectl.yBottom -= dy;
      rectl.yTop    += dy;
   }

   void inline InflateRect( ODRect &rect, ODCoordinate dx, ODCoordinate dy)
   {
      rect.left   -= dx;
      rect.right  += dx;
      rect.bottom -= dy;
      rect.top    += dy;
   }

   void inline OffsetRect( PRECTL prectl, long dx, long dy)
   {
      prectl->xLeft   += dx;
      prectl->xRight  += dx;
      prectl->yBottom += dy;
      prectl->yTop    += dy;
   }

   /*
    * The following classes aid in moving between ODPoint and ODRect
    * coordinates which are expressed using 16.16 fixed values to POINTL
    * and RECTL coordinates which are expressed using long ints.
    */

   #define FIXED2LONG(f) (((f) + 0x8000) >> 16)
   class ODPOINTL : public POINTL
   {
      public:
      operator ODPoint() const { ODPoint pt = {MAKEFIXED(x,0),MAKEFIXED(y,0)}; return pt; } ;
      ODPOINTL( ODPoint pt) {x = FIXED2LONG(pt.x); y = FIXED2LONG(pt.y); };
      ODPOINTL( int x1, int y1) { x = x1; y = y1; };
      ODPOINTL( ) { x = y = 0; };
      //ODPOINTL( POINTL & pt) { } ;
   };

   inline ODPOINTL & downcastToODPOINTL( POINTL & pt ) { return  (*(ODPOINTL*)&pt); }
   class ODRECTL : public RECTL
   {
      public:
      operator ODRect() const { ODRect rct = {
                                    MAKEFIXED(xLeft,0),
                                    MAKEFIXED(yTop,0),
                                    MAKEFIXED(xRight,0),
                                    MAKEFIXED(yBottom,0)
                                             }; return rct; } ;
      ODRECTL( ODRect rct) {
              xLeft   = FIXED2LONG(rct.left);
              yBottom = FIXED2LONG(rct.bottom);
              xRight  = FIXED2LONG(rct.right);
              yTop    = FIXED2LONG(rct.top);
                           };
      ODRECTL( ODPoint botLeft, ODPoint topRight) {
              xLeft   = FIXED2LONG(botLeft.x);
              yBottom = FIXED2LONG(botLeft.y);
              xRight  = FIXED2LONG(topRight.x);
              yTop    = FIXED2LONG(topRight.y);
                           };
      ODRECTL( ) { xLeft = yBottom = xRight = yTop = 0; };
      ODRECTL( RECTL & rct)  { *this = *(ODRECTL *)&rct ;}
   };
   inline ODRECTL & downcastToODRECTL( RECTL & rct ) { return  *(ODRECTL*)&rct; }



//----------------------------------------------------------------------------------------
// Handy functions and macros for dealing with ref-counted objectes
//----------------------------------------------------------------------------------------

#define  ODDeleteObject(object)  \
  do{              \
    if (object!=kODNULL) {  \
      delete object;    \
      object = kODNULL;  \
    }            \
  }while(0)

#ifndef INCL_ODDTS // include non-DTS C++ headers

#define  ODReleaseObject(ev, object) \
  do{                \
    if (object!=kODNULL) {    \
      object->Release(ev);  \
      object = kODNULL;    \
    }              \
  }while(0)

#define  ODIncrementRefCountObject(ev, object)  \
  do{                      \
    if (object!=kODNULL) {          \
      object->IncrementRefCount(ev);    \
    }                    \
  }while(0)

inline ODShape*
ODCopyAndRelease( Environment *ev, ODShape *s )
{
  if (s) {
    if( s->GetRefCount(ev) == 1 )
      return s;
    ODShape *copy = s->Copy(ev);
    s->Release(ev);
    return copy;
  }
}

#else // include DTS C++ headers

#define  ODReleaseObject(object) \
  do{                \
    if (object!=kODNULL) {    \
      object->Release();  \
      object = kODNULL;    \
    }              \
  }while(0)

#define  ODIncrementRefCountObject(object)  \
  do{                      \
    if (object!=kODNULL) {          \
      object->IncrementRefCount();    \
    }                    \
  }while(0)

inline ODShape*
ODCopyAndRelease(ODShape *s )
{
  if (s) {
    if( s->GetRefCount() == 1 )
      return s;
    ODShape *copy = s->Copy();
    s->Release();
    return copy;
  }
  return s;
}

#endif // ! INCL_ODDTS

ODTransform*
ODCopyAndRelease( Environment *ev, ODTransform *t );
#endif
