
#define INCL_DOSMODULEMGR
#define INCL_DOSRESOURCES
#define INCL_GPIBITMAPS
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES
#define INCL_GPIREGIONS
#define INCL_GPITRANSFORMS
#define INCL_GPILOGCOLORTABLE
#define INCL_GPIPATHS
#define INCL_WINACCELERATORS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINMESSAGEMGR
#define INCL_WINPOINTERS
#define INCL_WINSTDDRAG
#define INCL_WINTRACKRECT
#define INCL_WINWINDOWMGR
#define INCL_DEV

#define INCL_ODAPI
#define INCL_ODARBITRATOR
#define INCL_ODCANVAS
#define INCL_ODDRAGANDDROP
#define INCL_ODDRAFT
#define INCL_ODERRORS
#define INCL_ODFACET
#define INCL_ODFOCUSSET
#define INCL_ODFRAME
#define INCL_ODLINK
#define INCL_ODLINKSOURCE
#define INCL_ODLINKSPEC
#define INCL_ODSESSION
#define INCL_ODSEMANTICINTERFACE
#define INCL_ODSHAPE
#define INCL_ODSTORAGEUNIT
#define INCL_ODSTORAGEUNITVIEW
#define INCL_ODTRANSFORM
#define INCL_ODUI
#define INCL_ODUNDO
#define INCL_ODWINDOW
#define INCL_ODWINDOWSTATE
#include <os2.h>

#include "gtkpart.xih"

#ifndef _ORDCOLL_
#include "OrdColl.h"
#endif

#ifndef _TEMPOBJ_
#include "TempObj.h"
#endif

#include "gtkbase.h"
#include "gtkpic.h"
#include "actions.h"

#include <math.h>


#define  FIXED1    (MAKEFIXED(1,0))

MATRIXLF mx0 = {65536,0,0,0,65536,0,0,0,1};


//-------------------------------------------------------------------------
// GtkObject
//-------------------------------------------------------------------------

GtkObject* ReadObject( Environment *ev, ODStorageUnit* su, ODDraftKey key,
                       GraphicsPart* part)
{
  GtkObject* obj = NULL;
  ULONG      objId, objType;
  BOOL       locked;

  POINTL ptl = {0,0};
  RECTL  rcl;

  StorageUnitGetValue(su, ev, sizeof(objType), (ODValue)&objType);
  StorageUnitGetValue(su, ev, sizeof(objId), (ODValue)&objId);
  StorageUnitGetValue(su, ev, sizeof(locked), (ODValue)&locked);

  switch (objType) {
    case GOT_LINE:
      obj = new GtkLine(ptl,ptl);
      break;
    case GOT_POLYLINE:
      obj = new GtkPolyline();
      break;
    case GOT_BOX:
      obj = new GtkBox(ptl,ptl);
      break;
    case GOT_ELLIPSE:
      obj = new GtkEllipse(ptl,ptl);
      break;
    case GOT_ARC:
      obj = new GtkArc(ptl,FIXED1,0,0);
      break;
    case GOT_EMBED:
      obj = new GtkEmbed(part,&rcl);               // dummy rcl
      break;
    case GOT_GROUP:
      obj = new GtkGroup();
      break;
    case GOT_LINK:
      obj = new GtkLink();
      break;
    case GOT_LINKSRC:
      obj = new GtkLinkSource( part->GetPicture(ev) );
      break;
    default: break;
  }

  if (obj) {
    obj->SetId(objId);
    obj->Lock(locked);

    obj->Read(ev, su, key, part);
  }

  return obj;
}

//-------------------------------------------------------------------------
void GtkObject::Move( LONG dx, LONG dy )
{
  PMATRIXLF pMx = this->QueryTransform();
  if (pMx) {
    pMx->lM31 += dx;
    pMx->lM32 += dy;
    this->UpdateBounds();
  }
}
//-------------------------------------------------------------------------
void GtkObject::Transform( MATRIXLF* mxNew )
{
  PMATRIXLF pMx = this->QueryTransform();
  if (pMx) {
    *pMx = multiplyXforms( pMx, mxNew );
    this->UpdateBounds();
  }
}

//-------------------------------------------------------------------------
BOOL GtkObject::Write(Environment *ev, ODStorageUnit* su, ODDraftKey key)
{
  StorageUnitSetValue(su, ev, sizeof(type), (ODValue)&type);
  StorageUnitSetValue(su, ev, sizeof(id), (ODValue)&id);
  StorageUnitSetValue(su, ev, sizeof(locked), (ODValue)&locked);
  return TRUE;
}

#if 0
//-------------------------------------------------------------------------
HRGN GtkObject::Region( HPS hps )
{
  GpiBeginPath(hps, 1);
  this->Draw( hps, xxx );
  GpiEndPath(hps);

  return GpiPathToRegion( hps, 1, FPATH_WINDING );
}
#endif

//-------------------------------------------------------------------------
BOOL SetLineAttribute(PLINESTUFF ls, ULONG aType, LONG aVal)
{
  if (ls) switch (aType) {
    case LS_COLOR: if (ls->color != aVal) { ls->color = aVal; return TRUE; }
      break;
    case LS_STYLE: if (ls->style != aVal) { ls->style = aVal; return TRUE; }
      break;
    case LS_WIDTH: if (ls->width != aVal) { ls->width = aVal; return TRUE; }
      break;
  }
  return FALSE;
}

//-------------------------------------------------------------------------
BOOL SetFillAttribute(PFILLSTUFF fs, ULONG aType, LONG aVal)
{
  if (fs) switch (aType) {
    case FS_COLOR: if (fs->color != aVal) { fs->color = aVal; return TRUE; }
      break;
    case FS_STYLE: if (fs->style != aVal) { fs->style = aVal; return TRUE; }
      break;
  }
  return FALSE;
}

//-------------------------------------------------------------------------
BOOL GtkObject::SetAttribute(ULONG aType, LONG aVal)
{
  BOOL handled = FALSE;
  switch (aType) {
    case LS_COLOR:
    case LS_STYLE:
    case LS_WIDTH:
      handled = SetLineAttribute( this->QueryLinestuff(), aType, aVal );
      break;
    case FS_COLOR:
    case FS_STYLE:
      handled = SetFillAttribute( this->QueryFillstuff(), aType, aVal );
      break;
    case TRANSLUCENCE:
      if (type==GOT_EMBED) {
        GtkEmbed* embed = (GtkEmbed*)this;
        if (BOOL_XOR( aVal, embed->IsTranslucent() )) {
          embed->SetTranslucent( aVal );
          handled = TRUE;
        }
      }
      break;
    case SHADOW_STYLE:
      shadow = aVal;
      handled = TRUE;
      break;
    case ARC_CLOSING:
      {
        PLONG close = this->QueryClosing();
        if (close) {
          *close = aVal;
          handled = TRUE;
        }
      }
      break;
    case BOX_ROUNDING:
      {
      }
      break;
    default:
      break;
  }
  if (handled && (aType==SHADOW_STYLE))
    this->UpdateBounds();
  return handled;
}

//-------------------------------------------------------------------------
BOOL GtkObject::QueryAttribute(ULONG aType, LONG *aVal)
{
  BOOL handled = FALSE;
  PLINESTUFF ls;
  PFILLSTUFF fs;

  switch (aType) {
    case LS_COLOR:
      ls = this->QueryLinestuff();
      if (ls) {
        *aVal = ls->color;
        handled = TRUE;
      }
      break;
    case LS_STYLE:
      ls = this->QueryLinestuff();
      if (ls) {
        *aVal = ls->style;
        handled = TRUE;
      }
      break;
    case LS_WIDTH:
      ls = this->QueryLinestuff();
      if (ls) {
        *aVal = ls->width;
        handled = TRUE;
      }
      break;
    case FS_COLOR:
      fs = this->QueryFillstuff();
      if (fs) {
        *aVal = fs->color;
        handled = TRUE;
      }
      break;
    case FS_STYLE:
      fs = this->QueryFillstuff();
      if (fs) {
        *aVal = fs->style;
        handled = TRUE;
      }
      break;
    case TRANSLUCENCE:
      if (type==GOT_EMBED) {
        GtkEmbed* embed = (GtkEmbed*)this;
        *aVal = embed->IsTranslucent();
        handled = TRUE;
      }
      break;
    case SHADOW_STYLE:
      *aVal = shadow;
      handled = TRUE;
      break;
    case ARC_CLOSING:
      {
        PLONG close = this->QueryClosing();
        if (close) {
          *aVal = *close;
          handled = TRUE;
        }
      }
      break;
    case BOX_ROUNDING:
      {
      }
      break;
    default:
      break;
  }
  if (handled && (aType==SHADOW_STYLE))
    this->UpdateBounds();
  return handled;
}

//-------------------------------------------------------------------------
// GtkLine
//-------------------------------------------------------------------------

GtkLine::GtkLine(POINTL ptl0, POINTL ptl1)
{
  id = 0;
  type = GOT_LINE;
  ls.color = RGB_CYAN;
  ls.style = LINETYPE_SOLID;
  ls.width = LINEWIDTH_THICK;
  shadow = PATSYM_NOSHADE;
  ptls = new POINTL[2];
  if (ptls) {
    ptls[0] = ptl0;
    ptls[1] = ptl1;
    this->UpdateBounds();
  }
}

//-------------------------------------------------------------------------
void GtkLine::CopyFrom( GtkObject* org, BOOL shallow )
{
  GtkLine* original = (GtkLine*)org;

  id = original->id;
  ls = original->ls;
  shadow = original->shadow;

  ptls = new POINTL[2];
  if (ptls) {
    memcpy( ptls, original->ptls, 2*sizeof(POINTL) );
    this->UpdateBounds();
  }
}

//-------------------------------------------------------------------------
void GtkLine::SetHandle( ULONG indx, POINTL pt, BOOL commit )
{
  ptls[ indx ? 1 : 0 ] = pt;
  this->UpdateBounds();
}

//-------------------------------------------------------------------------
void GtkLine::UpdateBounds()
{
  rclBounds.xLeft = rclBounds.xRight = ptls[0].x;
  rclBounds.yBottom = rclBounds.yTop = ptls[0].y;
  AddPt( &rclBounds, &ptls[1] );
  if (shadow!=PATSYM_NOSHADE) {
    rclBounds.xRight += SHADOW_DX;
    rclBounds.yBottom -= SHADOW_DY;
  }
}

//-------------------------------------------------------------------------
void GtkLine::DrawShadow( HPS hps )
{
  GpiMove( hps, ptls );
  GpiLine( hps, ptls+1 );
}

//-------------------------------------------------------------------------
void set_linestuff( HPS hps, PLINESTUFF ls )
{
  GpiSetColor( hps, ls->color );
  GpiSetLineType( hps, ls->style );
  GpiSetLineWidth( hps, ls->width );
}

//-------------------------------------------------------------------------
void set_fillstuff( HPS hps, PFILLSTUFF fs )
{
  GpiSetColor( hps, fs->color );
  GpiSetPattern( hps, fs->style );
  GpiSetLineType( hps, LINETYPE_INVISIBLE );
}

//-------------------------------------------------------------------------
void set_shadowstuff( HPS hps, LONG shadow, PFILLSTUFF fs, PLINESTUFF ls )
{
  if (fs->style!=PATSYM_NOSHADE) {
    GpiSetColor( hps, RGB_BLACK );
    GpiSetLineType( hps, LINETYPE_INVISIBLE );
    GpiSetPattern( hps, shadow );
  } else {
    set_linestuff( hps, ls );
    GpiSetColor( hps, (shadow==PATSYM_SOLID) ? RGB_BLACK : 0x00888888 );
  }
}

//#define CLIPPED_PATTERNS

//-------------------------------------------------------------------------
void GtkLine::Draw( HPS hps, ODFacet* facet )
{
  if (shadow!=PATSYM_NOSHADE) {

    HpsFocus f2(hps);

    set_linestuff( hps, &ls );
    GpiSetColor( hps, (shadow==PATSYM_SOLID) ? RGB_BLACK : 0x00888888 );

    GpiMove( hps, ptls );
    GpiLine( hps, ptls+1 );
  }

  set_linestuff( hps, &ls );
  GpiMove( hps, ptls );
  GpiLine( hps, ptls+1 );
}

//-------------------------------------------------------------------------
BOOL GtkLine::Read(Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part)
{
  StorageUnitGetValue(su, ev, 2*sizeof(POINTL), (ODValue)ptls);
  StorageUnitGetValue(su, ev, sizeof(ls), (ODValue)&ls);
  StorageUnitGetValue(su, ev, sizeof(shadow), (ODValue)&shadow);
  this->UpdateBounds();
  return TRUE;
}

//-------------------------------------------------------------------------
BOOL GtkLine::Write(Environment *ev, ODStorageUnit* su, ODDraftKey key)
{
  if ( !GtkObject::Write(ev, su, key) ) return FALSE;
  StorageUnitSetValue(su, ev, 2*sizeof(POINTL), (ODValue)ptls);
  StorageUnitSetValue(su, ev, sizeof(ls), (ODValue)&ls);
  StorageUnitSetValue(su, ev, sizeof(shadow), (ODValue)&shadow);
  return TRUE;
}

// ----------------------------------------------------------------------------
POINTL  segment_point( POINTL pt0, POINTL pt1, POINTL pt2 )
{
   LONG   dx1, dy1, dx2, dy2, dxseg, dyseg, b2;
   POINTL pt;

   dxseg = pt2.x - pt1.x ;
   dyseg = pt2.y - pt1.y ;

   if ( dxseg == 0 && dyseg == 0 )   return pt1;

   dx1 = pt0.x - pt1.x;
   dy1 = pt0.y - pt1.y;

   if ( (dx1*dxseg + dy1*dyseg) <= 0L )  return pt1;

   else {
      dx2 = pt0.x - pt2.x;
      dy2 = pt0.y - pt2.y;

      if ( (dx2*dxseg + dy2*dyseg) >= 0L )  return pt2;
      else {
         if ( dxseg == 0L ) {
            pt.x = pt1.x;
            pt.y = pt0.y;
            }
         else if ( dyseg == 0L ) {
            pt.y = pt1.y;
            pt.x = pt0.x;
            }
         else if ( labs( dxseg ) >= labs( dyseg ) ) {
            b2 = dy1*dyseg + dx1*dxseg;
            pt.x = pt1.x + b2 / ( (dyseg*dyseg)/dxseg + dxseg);
            pt.y = pt1.y + ( pt.x - pt1.x ) * dyseg / dxseg ;
            }
         else {
            b2 = dx1*dxseg + dy1*dyseg;
            pt.y = pt1.y + b2 / ( (dxseg*dxseg)/dyseg + dyseg);
            pt.x = pt1.x + ( pt.y - pt1.y ) * dxseg / dyseg ;
            }
         }
      }
   return  pt;
   }

//-------------------------------------------------------------------------
ULONG D2( PPOINTL pt1, PPOINTL pt2 )
{
  ULONG dx, dy;
  dx = abs( pt1->x - pt2->x );
  dy = abs( pt1->y - pt2->y );
  return dx*dx + dy*dy;
}

//-------------------------------------------------------------------------
BOOL GtkLine::Hit( HPS hps, PPOINTL ptl, ULONG dx )
{
  POINTL pt;
  ULONG  distance;
  pt = segment_point( *ptl, ptls[0], ptls[1] );
  distance = D2( &pt, ptl );
  return (distance <= dx);
}

//-------------------------------------------------------------------------
void BoundPoints( RECTL* rcl, PPOINTL ptls, LONG n )
{
  rcl->xLeft = rcl->xRight = ptls[0].x;
  rcl->yBottom = rcl->yTop = ptls[0].y;
  for (int i=1;i<n;i++) AddPt( rcl, &ptls[i] );
}

//-------------------------------------------------------------------------
BOOL  PointsHit( ULONG n, PPOINTL ptls, PPOINTL ptl, ULONG mode,
                    PULONG distance, PPOINTL ptlHit )
{
  POINTL pt;
  RECTL  rcl;
  ULONG  dist, olddist;
  int i;

  if (distance) olddist = *distance;

  switch (mode) {
    case NEAREST_PT:
      for (i=1; i<n; i++) {
        pt = segment_point( *ptl, ptls[i], ptls[i-1] );
        dist = D2( &pt, ptl );
        if (dist < *distance) {
          *distance= dist;
          *ptlHit = pt;
        }
      }
      break;
    case NEAREST_ENDPT:
      for (i=0; i<n; i++) {
        dist = D2( &ptls[i], ptl );
        if (dist < *distance) {
          *distance = dist;
          *ptlHit = ptls[i];
        }
      }
      break;
    case NEAREST_CENTER:
      {
        BoundPoints( &rcl, ptls, n );

        pt.x = (rcl.xLeft + rcl.xRight) / 2;
        pt.y = (rcl.yBottom + rcl.yTop) / 2;
        if (ptl) dist = D2( &pt, ptl );
        if (!distance || dist<*distance) {
          if (distance) *distance = dist;
          *ptlHit = pt;
          return TRUE;
        }
      }
      break;
    default:
      return FALSE;
  }
  return (olddist != *distance);
}

//-------------------------------------------------------------------------
BOOL  GtkLine::FindPoint( PPOINTL ptl, ULONG mode,
                          PULONG distance, PPOINTL ptlHit )
{ return PointsHit( 2, ptls, ptl, mode, distance, ptlHit ); }

//-------------------------------------------------------------------------
GtkObject* GtkLine::Copy(BOOL shallow)
{
  POINTL ptl = {0,0};                            // dummy!
  GtkObject* obj = new GtkLine(ptl,ptl);
  if (obj) obj->CopyFrom(this);
  return obj;
}

//-------------------------------------------------------------------------
void GtkLine::Move( LONG dx, LONG dy )
{
  ptls[0].x += dx;
  ptls[0].y += dy;
  ptls[1].x += dx;
  ptls[1].y += dy;
  this->UpdateBounds();
}

//-------------------------------------------------------------------------
void GtkLine::Transform( MATRIXLF* mx )
{
  transformPts( mx, ptls, 2 );
  this->UpdateBounds();
}


//-------------------------------------------------------------------------
// GtkPolyline
//-------------------------------------------------------------------------

GtkPolyline::GtkPolyline(ULONG npts, PPOINTL pts)
{
  id = 0;
  type = GOT_POLYLINE;
  ls.color = RGB_YELLOW;
  ls.style = LINETYPE_SOLID;
  ls.width = LINEWIDTH_THICK;
  fs.color = RGB_YELLOW;
  fs.style = PATSYM_NOSHADE;
  shadow = PATSYM_NOSHADE;
  ptls = 0;
  this->SetPts( npts, pts );
}

//-------------------------------------------------------------------------
void GtkPolyline::CopyFrom( GtkObject* org, BOOL shallow )
{
  GtkPolyline* original = (GtkPolyline*)org;
  id = original->id;
  ls = original->ls;
  fs = original->fs;
  shadow = original->shadow;
  this->SetPts( original->n, original->ptls );
}

//-------------------------------------------------------------------------
void GtkPolyline::SetPts(ULONG npts, PPOINTL pts )
{
  if (ptls) delete ptls;

  n = npts;
  if (n) {
    ptls = new POINTL[n];
    if (ptls) {
      memcpy( ptls, pts, n*sizeof(POINTL) );
      fSolid = (fs.style != PATSYM_NOSHADE) ||
               (ptls[0].x==ptls[n-1].x && ptls[0].y==ptls[n-1].y);
      }
  } else {
    ptls = 0;
  } /* endif */
  this->UpdateBounds();
}

//-------------------------------------------------------------------------
void GtkPolyline::SetHandle( ULONG indx, POINTL pt, BOOL commit )
{
  if ( indx>=0 && indx<n ) {
    ptls[indx] = pt;
    if (commit) {
      this->UpdateBounds();
    }
  }
}

//-------------------------------------------------------------------------
BOOL GtkPolyline::SetAttribute(ULONG aType, LONG aVal)
{
  BOOL fChanged = GtkObject::SetAttribute(aType, aVal);

  if ( fChanged && aType == FS_STYLE )
    fSolid = (fs.style != PATSYM_NOSHADE) ||
             (ptls[0].x==ptls[n-1].x && ptls[0].y==ptls[n-1].y);
  return fChanged;
}

//-------------------------------------------------------------------------
void GtkPolyline::UpdateBounds()
{
  if (n) {
    fSolid = n>2 && ( (fs.style != PATSYM_NOSHADE) ||
                      (ptls[0].x==ptls[n-1].x && ptls[0].y==ptls[n-1].y) );
    BoundPoints( &rclBounds, ptls, n );

//    if (fSolid && shadow!=PATSYM_NOSHADE && n>2) {
    if (shadow!=PATSYM_NOSHADE) {
      rclBounds.xRight += SHADOW_DX;
      rclBounds.yBottom -= SHADOW_DY;
    }
  } else {
    fSolid = FALSE;
    rclBounds.xLeft = rclBounds.xRight = 0;
    rclBounds.yBottom = rclBounds.yTop = 0;
  } /* endif */
}

//-------------------------------------------------------------------------
void GtkPolyline::DrawShadow( HPS hps )
{
  GpiMove( hps, ptls );
  if (n>1) GpiPolyLine( hps, n-1, ptls+1 );
  if (fSolid) GpiLine( hps, ptls );
}

//-------------------------------------------------------------------------
void GtkPolyline::Draw( HPS hps, ODFacet* facet )
{
  if (shadow!=PATSYM_NOSHADE) {

    HpsFocus f2(hps);

    set_shadowstuff( hps, shadow, &fs, &ls );

    if (fs.style!=PATSYM_NOSHADE) GpiBeginArea ( hps, BA_BOUNDARY );
    this->DrawShadow(hps);
    if (fs.style!=PATSYM_NOSHADE) GpiEndArea(hps);
  }

  if (fs.style!=PATSYM_NOSHADE) {
    set_fillstuff( hps, &fs );
    GpiBeginArea ( hps, BA_BOUNDARY );
    this->DrawShadow(hps);
    GpiEndArea(hps);
  }

  if (ls.style!=LINETYPE_INVISIBLE) {
    set_linestuff( hps, &ls );
    this->DrawShadow(hps);
  }
}

//-------------------------------------------------------------------------
BOOL GtkPolyline::Read(Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part)
{
  StorageUnitGetValue(su, ev, sizeof(n), (ODValue)&n);
  if (n) {
    ptls = new POINTL[n];
    if (!ptls) return FALSE;
    StorageUnitGetValue(su, ev, n*sizeof(POINTL), (ODValue)ptls);
  } else {
    ptls = 0;
  } /* endif */
  StorageUnitGetValue(su, ev, sizeof(ls), (ODValue)&ls);
  StorageUnitGetValue(su, ev, sizeof(fs), (ODValue)&fs);
  StorageUnitGetValue(su, ev, sizeof(shadow), (ODValue)&shadow);

  this->UpdateBounds();
  return TRUE;
}

//-------------------------------------------------------------------------
BOOL GtkPolyline::Write(Environment *ev, ODStorageUnit* su, ODDraftKey key)
{
  if ( !GtkObject::Write(ev, su, key) ) return FALSE;
  StorageUnitSetValue(su, ev, sizeof(n), (ODValue)&n);
  if (n) StorageUnitSetValue(su, ev, n*sizeof(POINTL), (ODValue)ptls);
  StorageUnitSetValue(su, ev, sizeof(ls), (ODValue)&ls);
  StorageUnitSetValue(su, ev, sizeof(fs), (ODValue)&fs);
  StorageUnitSetValue(su, ev, sizeof(shadow), (ODValue)&shadow);
  return TRUE;
}

// ---------------------------------------------------------------------------
int seg_cross( PPOINTL ptl, PPOINTL pt0, PPOINTL pt1 )
{
  LONG ya, yb, x;

  if (pt0->y < pt1->y) {
    ya = pt0->y;
    yb = pt1->y;
  } else if (pt0->y > pt1->y) {
    yb = pt0->y;
    ya = pt1->y;
  } else {
    // if it intersects a horizontal segment, lock it TRUE;
    return (pt0->y == ptl->y) &&
           ((pt0->x <= ptl->x && pt1->x >= ptl->x ) ||
            (pt1->x <= ptl->x && pt0->x >= ptl->x )   ) ? 10000 : 0;
  } /* endif */

  // y's straddle ptl?
  // FIX for hits of curves clicking outside and to the right...
//  if (ya <= ptl->y && yb >= ptl->y) {
  if (ya <= ptl->y && yb >= ptl->y && ptl->y != pt0->y) {
    // calc the intercept x
    x = pt0->x + (ptl->y - pt0->y) * (pt1->x-pt0->x) / (pt1->y-pt0->y);
    // intercept to the left of x??
    if ( x <= ptl->x ) {
      return pt0->y<pt1->y ? 1 : -1;
    }
  } /* endif */

  return 0;
  }

//-------------------------------------------------------------------------
BOOL GtkPolyline::Hit( HPS hps, PPOINTL ptl, ULONG dx )
{
  POINTL pt;
  ULONG  distance;

  if (fSolid) {
    int crossings = 0;
    for (int i=1; i<n; i++)
      crossings += seg_cross( ptl, &ptls[i-1], &ptls[i] );
    crossings += seg_cross( ptl, &ptls[n-1], &ptls[0] );
    return (crossings != 0);
  } else for (int i=1; i<n; i++) {
    pt = segment_point( *ptl, ptls[i], ptls[i-1] );
    distance = D2( &pt, ptl );
    if (distance <= dx) return TRUE;
  }
  return FALSE;
}

//-------------------------------------------------------------------------
BOOL  GtkPolyline::FindPoint( PPOINTL ptl, ULONG mode,
                              PULONG distance, PPOINTL ptlHit )
{
  BOOL hit = FALSE;
  // general distance checking
  if ( mode == NEAREST_CENTER )
       hit = PointsHit( 2, (PPOINTL)&rclBounds, ptl, mode, distance, ptlHit );
  else hit = PointsHit( n, ptls, ptl, mode, distance, ptlHit );

  // checking for nearest point on solid-closing line segment...
  if (mode == NEAREST_PT && fSolid) {
    ULONG dist;
    POINTL pt;
    pt = segment_point( *ptl, ptls[0], ptls[n-1] );
    dist = D2( &pt, ptl );
    if (dist < *distance) {
      *distance= dist;
      *ptlHit = pt;
      hit = TRUE;
    }
  }
  return hit;
}

//-------------------------------------------------------------------------
void GtkPolyline::Move( LONG dx, LONG dy )
{
  for (int i=0; i<n; i++) {
    ptls[i].x += dx;
    ptls[i].y += dy;
  }
  this->UpdateBounds();
}

//-------------------------------------------------------------------------
GtkObject* GtkPolyline::Copy(BOOL shallow)
{
  GtkObject* obj = new GtkPolyline();
  if (obj) obj->CopyFrom(this);
  return obj;
}

//-------------------------------------------------------------------------
void GtkPolyline::Transform( MATRIXLF* mx )
{
  transformPts( mx, ptls, n );
  this->UpdateBounds();
}


//-------------------------------------------------------------------------
// GtkEmbed
//-------------------------------------------------------------------------

GtkEmbed::GtkEmbed( GraphicsPart *gtkpart, PRECTL rcl, POINTL* offset)
{
  id = 0;
  type = GOT_EMBED;
  containingPart = gtkpart;

  ls.color = RGB_WHITE;
  ls.style = LINETYPE_INVISIBLE;
  ls.width = LINEWIDTH_NORMAL;
  fs.color = RGB_BLACK;
  fs.style = PATSYM_SOLID;
  shadow = PATSYM_NOSHADE;
  fTranslucent = 0;
  mx = mx0;

  if (rcl) {
    #if 0
      ptl0.x = ptl0.y = 0;
      ptl1.x = rcl->xRight - rcl->xLeft;
      ptl1.y = rcl->yTop   - rcl->yBottom;
      mx.lM31 = rcl->xLeft;
      mx.lM32 = rcl->yBottom;
    #else
      ptl0 = *((PPOINTL)&rcl->xLeft);
      ptl1 = *((PPOINTL)&rcl->xRight);
      if (offset) {
        mx.lM31 = offset->x;
        mx.lM32 = offset->y;
      }
    #endif
    this->UpdateBounds();
  } else {
    ptl0.x = ptl0.y = 0;
    ptl1 = ptl0;
    rclBounds.xLeft = rclBounds.xRight = 0;
  }
}

//-------------------------------------------------------------------------
GtkEmbed::~GtkEmbed()
{
  if (containingPart)
    containingPart->DeleteEmbed(0,id);
}

//-------------------------------------------------------------------------
void GtkEmbed::CopyFrom( GtkObject* org, BOOL shallow )
{
  GtkEmbed* original = (GtkEmbed*)org;

  ptl0 = original->ptl0;
  ptl1 = original->ptl1;
  mx = original->mx;
  rclBounds = original->rclBounds;

  ls = original->ls;
  fs = original->fs;
  shadow = original->shadow;
  fTranslucent = original->fTranslucent;

  if (!shallow) {
    containingPart = original->containingPart;
    id = 0xffffffff;                           // signal that id is NOT in synch
    if (containingPart)
      containingPart->CopyEmbed( 0, original->id, id );
  } else {
    containingPart = 0;
  }
}

//-------------------------------------------------------------------------
void  GtkEmbed::SetId( ULONG newId )
{
//  if (id == 0xffffffff && containingPart)
  if (containingPart)
    containingPart->SetEmbedId( 0, id, newId );

  id = newId;
}

//-------------------------------------------------------------------------
void  GtkEmbed::SetDragging( BOOL fDragging )
{
  if (!containingPart) return;

  Environment* ev = SOM_ENVIRONMENT;
  Proxy* proxy = containingPart->ProxyFromId(ev, id);
  if (!proxy) return;

  proxy->frame->SetDragging(ev, fDragging);
}

//-------------------------------------------------------------------------
void GtkEmbed::UpdateBounds()
{
  PPOINTL ptls = this->Pts();
  BoundPoints( &rclBounds, ptls, 5 );
  if (shadow!=PATSYM_NOSHADE) {                      // zzz this is BROKEN...
    rclBounds.xRight += SHADOW_DX;                   // (with mx)
    rclBounds.yBottom -= SHADOW_DY;
  }
  delete ptls;
}

//-------------------------------------------------------------------------
void GtkEmbed::Draw( HPS hps, ODFacet* facet )
{
  if (shadow!=PATSYM_NOSHADE) {

    HpsFocus f2(hps);

    set_shadowstuff( hps, shadow, &fs, &ls );

    HRGN embedRgn = this->Region( hps, facet );
    POINTL offset = { SHADOW_DX, -SHADOW_DY };
    GpiOffsetRegion( hps, embedRgn, &offset );
    GpiPaintRegion( hps, embedRgn );
    GpiDestroyRegion( hps, embedRgn );
  }

  containingPart->DrawEmbed( 0, id, facet );
}

//-------------------------------------------------------------------------
void GtkEmbed::DrawShadow( HPS hps )
{
  HpsFocus f(hps, &mx);

  // this should get replaced with a fill of the embed's used shape...
  GpiMove( hps, &ptl0 );
  GpiBox( hps, DRO_OUTLINE, &ptl1, 0, 0 );
}

//-------------------------------------------------------------------------
// key=0 means we are reading our own su, key!=0 means we are cloning in
//-------------------------------------------------------------------------
BOOL GtkEmbed::Read(Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part)
{
  StorageUnitGetValue(su, ev, sizeof(ptl0), (ODValue)&ptl0);
  StorageUnitGetValue(su, ev, sizeof(ptl1), (ODValue)&ptl1);
  StorageUnitGetValue(su, ev, sizeof(mx), (ODValue)&mx);

  StorageUnitGetValue(su, ev, sizeof(ls), (ODValue)&ls);
  StorageUnitGetValue(su, ev, sizeof(fs), (ODValue)&fs);
  StorageUnitGetValue(su, ev, sizeof(shadow), (ODValue)&shadow);
  StorageUnitGetValue(su, ev, sizeof(fTranslucent), (ODValue)&fTranslucent);

  ODStorageUnitRef aSURef;
  StorageUnitGetValue(su, ev, sizeof(ODStorageUnitRef), (ODValue)&aSURef);
  frameID = su->GetIDFromStorageUnitRef(ev, aSURef);

  // cloning in from another draft??  Need the id of the CLONE!
  if (key) {
    frameID = su->GetDraft(ev)->Clone(ev, key, frameID, 0, 0);

    // give it an id that won't match any in our existing proxy list!
    id = 0xffffffff;
  }

  // read the frame's transform
  StorageUnitGetValue(su, ev, sizeof(odmx), (ODValue)&odmx);

  // on a regular read we go ahead and init here...
  // NOW WE ARE REQUIRING AN EXPLICIT PRI CALL FROM OUTSIDE
//  if (!key)
//    this->PostReadInit(ev, part, FALSE );

  return TRUE;
}

//-------------------------------------------------------------------------
void GtkEmbed::PostReadInit( Environment *ev, GraphicsPart* part, BOOL cloned )
{
  if (frameID) {
    ODDraft* myDraft = part->GetStorageUnit(ev)->GetDraft(ev);
    ODFrame* frame = myDraft->AcquireFrame(ev, frameID);

    ODTransform* transform = frame->CreateTransform(ev);
    transform->SetMatrix( ev, &odmx );

    Proxy* p = new Proxy( frame, transform, id );
    containingPart->AddProxy(ev, p, cloned );

    if (locked)
      frame->SetFrozen(ev, kODTrue);

    if (fTranslucent)
      UpdateTranslucence();

    this->UpdateBounds();

    frameID = 0;
  }
}

//-------------------------------------------------------------------------
// key=0 means we are writing our own su, key!=0 means we are cloning out
//-------------------------------------------------------------------------
BOOL GtkEmbed::Write(Environment *ev, ODStorageUnit* su, ODDraftKey key)
{
  Proxy* proxy = containingPart->ProxyFromId(ev, id);
  if (!proxy) return FALSE;

  if ( !GtkObject::Write(ev, su,key) ) return FALSE;
  StorageUnitSetValue(su, ev, sizeof(ptl0), (ODValue)&ptl0);
  StorageUnitSetValue(su, ev, sizeof(ptl1), (ODValue)&ptl1);
  StorageUnitSetValue(su, ev, sizeof(mx), (ODValue)&mx);

  StorageUnitSetValue(su, ev, sizeof(ls), (ODValue)&ls);
  StorageUnitSetValue(su, ev, sizeof(fs), (ODValue)&fs);
  StorageUnitSetValue(su, ev, sizeof(shadow), (ODValue)&shadow);
  StorageUnitSetValue(su, ev, sizeof(fTranslucent), (ODValue)&fTranslucent);

  ODFrame* frame = proxy->frame;
  ODID frameID = frame->GetID(ev);

  // writing to our own su??
  if (!key) {
    frame->Externalize(ev);

  // or cloning out??
  } else {
    ODDraft* fromDraft = frame->GetStorageUnit(ev)->GetDraft(ev);
    frameID = fromDraft->Clone(ev, key, frameID, 0, 0);
  }

  ODStorageUnitRef aSURef;
  su->GetStrongStorageUnitRef(ev, frameID, aSURef);
  StorageUnitSetValue(su, ev, sizeof(ODStorageUnitRef), (ODValue)&aSURef);

  // write its external transform
  ODMatrix mx;
  proxy->transform->GetMatrix( ev, &mx );
  StorageUnitSetValue( su, ev, sizeof(mx), (ODValue)&mx);

  return TRUE;
}

//-------------------------------------------------------------------------
void GtkEmbed::UpdateTranslucence()
{
  Environment* ev = SOM_ENVIRONMENT;

  // get the embed's frame
  Proxy* proxy = containingPart->ProxyFromId(ev, id);
  if (!proxy) return;

  ODFrame* subFrame = proxy->frame;

  // create a storage unit for passing the property
  TempODStorageUnit suProps = subFrame->GetStorageUnit(ev)->GetDraft(ev)->CreateStorageUnit(ev);
//  ODStorageUnit* suProps;
//  suProps = subFrame->GetStorageUnit(ev)->GetDraft(ev)->CreateStorageUnit(ev);

  // create the bin and write it
  InitPropVal( ev, suProps, kODBackgroundTransparency, kODBoolean );
  StorageUnitSetValue( suProps, ev, sizeof(fTranslucent), (ODValue)&fTranslucent);

  // tell the embed
//  ODPart *embeddedPart = subFrame->AcquirePart(ev);
  TempODPart embeddedPart = subFrame->AcquirePart(ev);
  if (embeddedPart) {
    embeddedPart->ContainingPartPropertiesUpdated(ev, subFrame, suProps);
//    embeddedPart->Release(ev);
  }

  // free up the tmp su
//  suProps->Release(ev);
}

//-------------------------------------------------------------------------
BOOL GtkEmbed::SetAttribute(ULONG aType, LONG aVal)
{
  BOOL fChanged = GtkObject::SetAttribute(aType, aVal);

  // did the embed translucency change?  If so, we have to tell the embed!
  if ( fChanged && aType == TRANSLUCENCE )
    this->UpdateTranslucence();

  return fChanged;
}

//-------------------------------------------------------------------------
BOOL GtkEmbed::Hit( HPS hps, PPOINTL ptl, ULONG dx )
{
  if (ptl->x<rclBounds.xLeft   || ptl->x>rclBounds.xRight ||
      ptl->y<rclBounds.yBottom || ptl->y>rclBounds.yTop     ) {
    return FALSE;
  }

  POINTL pt = *ptl;
  GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 1, &pt );
  return containingPart && containingPart->EmbedHit( 0, id, &pt );
}

//-------------------------------------------------------------------------
POINTL GtkEmbed::QueryHandle( ULONG indx )
{
  POINTL pt;
  pt = (indx==0?ptl0:ptl1);
  transformPts( &mx, &pt, 1 );
  return pt;
}

//-------------------------------------------------------------------------
void GtkEmbed::SetHandle( ULONG indx, POINTL pt1, BOOL commit )
{
  POINTL pt2 = (indx==1) ? ptl0 : ptl1;

  {
  POINTL pts[2];
  pts[0] = ptl0;
  pts[1] = ptl1;
  transformPts( &mx, pts, 2 );
  invertPts( &mx, pts, 2 );
  pts[0].x = pts[1].x;
  }

  invertPts( &mx, &pt1, 1 );
  if (pt1.x == pt2.x || pt1.y == pt2.y ) return;        // exclude zero-extent

  if (indx) {
    ptl1 = pt1;
  } else {
    ptl0 = pt1;
  }
  this->UpdateBounds();

  // NOW, send msg to containingPart to move the part appropriately!
  if (commit && containingPart) {
    #if 0
      containingPart->MoveEmbed( 0, id, &rclBounds );
    #else
      RECTL rcl;
      *(PPOINTL)&rcl.xLeft = ptl0;
      *(PPOINTL)&rcl.xRight = ptl1;
      containingPart->ReFrameEmbed( 0, id, &rcl );
    #endif
  }
}

//-------------------------------------------------------------------------
GtkObject* GtkEmbed::Copy(BOOL shallow)
{
  GtkObject* obj = new GtkEmbed();

//  try{

    if (obj) obj->CopyFrom(this,shallow);

//  } catch (... ) {
//    containingPart = 0;
//    delete obj;
//    obj = 0;
//    DosBeep(100,100);
//  } /* end catch */

  return obj;
}

//-------------------------------------------------------------------------
void GtkEmbed::Move( LONG dx, LONG dy )
{
  #if 1
    mx.lM31 += dx;
    mx.lM32 += dy;
    OffsetRect( &rclBounds, dx, dy);
  #else
    GtkObject::Move( dx, dy );
  #endif

  // NOW, send msg to containingPart to move the part appropriately!
  if (containingPart)
    containingPart->TransformEmbed( 0, id, &mx );
}

//-------------------------------------------------------------------------
void GtkEmbed::Transform( MATRIXLF* mxNew )
{
  GtkObject::Transform( mxNew );
//  mx = multiplyXforms( &mx, mxNew );
//  this->UpdateBounds();

  // NOW, send msg to containingPart to move the part appropriately!
  if (containingPart)
    containingPart->TransformEmbed( 0, id, &mx );
}

//-------------------------------------------------------------------------
void GtkEmbed::Lock( BOOL fLock )
{
  GtkObject::Lock(fLock);

  if (containingPart)
    containingPart->LockEmbed(0, id, fLock);
}

//-------------------------------------------------------------------------
PPOINTL GtkEmbed::Pts()
{
  PPOINTL pts = new POINTL[5];
  pts[0] = ptl0;
  pts[1].x = ptl1.x;
  pts[1].y = ptl0.y;
  pts[2] = ptl1;
  pts[3].x = ptl0.x;
  pts[3].y = ptl1.y;
  pts[4] = ptl0;
  transformPts( &mx, pts, 5 );
  return pts;
}


//-------------------------------------------------------------------------
// GtkBox
//-------------------------------------------------------------------------

GtkBox::GtkBox(POINTL ptlLL, POINTL ptlUR, LONG rx)
{
  id = 0;
  type = GOT_BOX;
  ls.color = RGB_BLUE;
  ls.style = LINETYPE_SOLID;
  ls.width = LINEWIDTH_NORMAL;
  fs.color = RGB_YELLOW;
  fs.style = PATSYM_HALFTONE;
  shadow = PATSYM_NOSHADE;

  mx = mx0;
  #if 1
    ptl0 = ptlLL;
    ptl1 = ptlUR;
  #else
    ptl0.x = 0;
    ptl0.y = 0;
    ptl1.x = ptlUR.x - ptlLL.x;
    ptl1.y = ptlUR.y - ptlLL.y;
    mx.lM31 = ptlLL.x;
    mx.lM32 = ptlLL.y;
  #endif
  rounding.cx = rounding.cy = rx;
  this->UpdateBounds();
}

//-------------------------------------------------------------------------
void GtkBox::CopyFrom( GtkObject* org, BOOL shallow )
{
  GtkBox* original = (GtkBox*)org;
  id = original->id;

  ptl0 = original->ptl0;
  ptl1 = original->ptl1;
  mx = original->mx;

  rounding = original->rounding;
  ls = original->ls;
  fs = original->fs;
  shadow = original->shadow;

  this->UpdateBounds();
}

//-------------------------------------------------------------------------
void GtkBox::UpdateBounds()
{
  PPOINTL ptls = this->Pts();
  BoundPoints( &rclBounds, ptls, 5 );
  if (shadow!=PATSYM_NOSHADE) {                      // zzz this is BROKEN...
    rclBounds.xRight += SHADOW_DX;                   // (with mx)
    rclBounds.yBottom -= SHADOW_DY;
  }
  delete ptls;
}

//-------------------------------------------------------------------------
void GtkBox::Draw( HPS hps, ODFacet* facet )
{
  HpsFocus f(hps, &mx);

  LONG rx = labs(rounding.cx * (ptl1.x - ptl0.x)) / 100;
  LONG ry = labs(rounding.cy * (ptl1.y - ptl0.y)) / 100;

  if (shadow!=PATSYM_NOSHADE) {

    HpsFocus f2(hps);

    set_shadowstuff( hps, shadow, &fs, &ls );

    GpiMove( hps, &ptl0 );
    GpiBox( hps, fs.style!=PATSYM_NOSHADE ? DRO_FILL : DRO_OUTLINE, &ptl1, rx, ry);
  }

  GpiMove( hps, &ptl0 );

  if (fs.style!=PATSYM_NOSHADE) {
    set_fillstuff( hps, &fs );
    GpiBox( hps, DRO_FILL, &ptl1, rx, ry );
  }

  set_linestuff( hps, &ls );
  GpiBox( hps, DRO_OUTLINE, &ptl1, rx, ry );
}

//-------------------------------------------------------------------------
void GtkBox::DrawShadow( HPS hps )
{
  HpsFocus f(hps, &mx);

  LONG rx = labs(rounding.cx * (ptl1.x - ptl0.x)) / 100;
  LONG ry = labs(rounding.cy * (ptl1.y - ptl0.y)) / 100;

  GpiMove( hps, &ptl0 );
  GpiBox( hps, DRO_OUTLINE, &ptl1, rx, ry);
}

//-------------------------------------------------------------------------
BOOL GtkBox::Read(Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part)
{
  StorageUnitGetValue(su, ev, sizeof(ptl0), (ODValue)&ptl0);
  StorageUnitGetValue(su, ev, sizeof(ptl1), (ODValue)&ptl1);
  StorageUnitGetValue(su, ev, sizeof(mx), (ODValue)&mx);
  StorageUnitGetValue(su, ev, sizeof(rounding), (ODValue)&rounding);

  StorageUnitGetValue(su, ev, sizeof(ls), (ODValue)&ls);
  StorageUnitGetValue(su, ev, sizeof(fs), (ODValue)&fs);
  StorageUnitGetValue(su, ev, sizeof(shadow), (ODValue)&shadow);
  this->UpdateBounds();
  return TRUE;
}

//-------------------------------------------------------------------------
BOOL GtkBox::Write(Environment *ev, ODStorageUnit* su, ODDraftKey key)
{
  if ( !GtkObject::Write(ev, su, key) ) return FALSE;
  StorageUnitSetValue(su, ev, sizeof(ptl0), (ODValue)&ptl0);
  StorageUnitSetValue(su, ev, sizeof(ptl1), (ODValue)&ptl1);
  StorageUnitSetValue(su, ev, sizeof(mx), (ODValue)&mx);
  StorageUnitSetValue(su, ev, sizeof(rounding), (ODValue)&rounding);

  StorageUnitSetValue(su, ev, sizeof(ls), (ODValue)&ls);
  StorageUnitSetValue(su, ev, sizeof(fs), (ODValue)&fs);
  StorageUnitSetValue(su, ev, sizeof(shadow), (ODValue)&shadow);
  return TRUE;
}

//-------------------------------------------------------------------------
BOOL GtkBox::Hit( HPS hps, PPOINTL ptl, ULONG dx )
{
  int crossings = 0;
  PPOINTL pts = this->Pts();
  for (int i=1; i<5; i++)
    crossings += seg_cross( ptl, &pts[i-1], &pts[i] );
  delete pts;
  return (crossings != 0);
}

//-------------------------------------------------------------------------
BOOL  GtkBox::FindPoint( PPOINTL ptl, ULONG mode,
                         PULONG distance, PPOINTL ptlHit )
{
  BOOL hit = FALSE;
  if (mode==NEAREST_PT || mode ==NEAREST_ENDPT) {
    PPOINTL ptls = this->Pts();
    hit = PointsHit( 5, ptls, ptl, mode, distance, ptlHit );
    delete ptls;
  } else if (mode==NEAREST_CENTER) {
    POINTL pt;
    pt.x = (ptl0.x + ptl1.x) / 2;
    pt.y = (ptl0.y + ptl1.y) / 2;
    transformPts( &mx, &pt, 1 );
    hit = PointsHit( 1, &pt, ptl, mode, distance, ptlHit );
  }
  return hit;
}

//-------------------------------------------------------------------------
POINTL GtkBox::QueryHandle( ULONG indx )
{
  POINTL pt;
  pt = (indx==0?ptl0:ptl1);
  transformPts( &mx, &pt, 1 );
  return pt;
}

//-------------------------------------------------------------------------
void GtkBox::SetHandle( ULONG indx, POINTL pt, BOOL commit )
{
  invertPts( &mx, &pt, 1 );
  if (indx) {
    ptl1 = pt;
  } else {
    ptl0 = pt;
  }
  this->UpdateBounds();
}

//-------------------------------------------------------------------------
PPOINTL GtkBox::Pts()
{
  PPOINTL pts = new POINTL[5];
  pts[0] = ptl0;
  pts[1].x = ptl1.x;
  pts[1].y = ptl0.y;
  pts[2] = ptl1;
  pts[3].x = ptl0.x;
  pts[3].y = ptl1.y;
  pts[4] = ptl0;
  transformPts( &mx, pts, 5 );
  return pts;
}

//-------------------------------------------------------------------------
GtkObject* GtkBox::Copy(BOOL shallow)
{
  POINTL ptl = {0,0};                            // dummy!
  GtkObject* obj = new GtkBox(ptl,ptl);
  if (obj) obj->CopyFrom(this);
  return obj;
}


//-------------------------------------------------------------------------
// GtkEllipse
//-------------------------------------------------------------------------

GtkEllipse::GtkEllipse(POINTL ptlCenter, POINTL ptlCorner)
{
  id = 0;
  type = GOT_ELLIPSE;
  ls.color = RGB_RED;
  ls.style = LINETYPE_SOLID;
  ls.width = LINEWIDTH_THICK;
  fs.color = RGB_GREEN;
  fs.style = PATSYM_DIAG2;
  shadow = PATSYM_NOSHADE;
  ptl0 = ptlCenter;
  ptl1.x = ptlCenter.x + abs(ptlCorner.x - ptlCenter.x);
  ptl1.y = ptlCenter.y + abs(ptlCorner.y - ptlCenter.y);
  mx = mx0;
  this->UpdateBounds();
}

//-------------------------------------------------------------------------
void GtkEllipse::CopyFrom( GtkObject* org, BOOL shallow )
{
  GtkEllipse* original = (GtkEllipse*)org;

  id = original->id;
  ls = original->ls;
  fs = original->fs;
  shadow = original->shadow;

  ptl0 = original->ptl0;
  ptl1 = original->ptl1;
  mx = original->mx;

  this->UpdateBounds();
}

//-------------------------------------------------------------------------
void GtkEllipse::UpdateBounds()
{
  ULONG   n = 41;
  PPOINTL ptls = this->Pts(&n);
  BoundPoints( &rclBounds, ptls, n );
  if (shadow!=PATSYM_NOSHADE) {                        // zzz BROKEN (mx)
    rclBounds.xRight += SHADOW_DX;
    rclBounds.yBottom -= SHADOW_DY;
  }
  delete ptls;
}

//-----------------------------------------------------------------------------
void GtkEllipse::Draw( HPS hps, ODFacet* facet )
{
  POINTL ptl;
  ptl.x = 2*ptl0.x - ptl1.x;
  ptl.y = 2*ptl0.y - ptl1.y;

  HpsFocus f(hps, &mx);

  LONG rx = labs(ptl1.x - ptl.x);
  LONG ry = labs(ptl1.y - ptl.y);

  if (shadow!=PATSYM_NOSHADE) {

    HpsFocus f2(hps);

    set_shadowstuff( hps, shadow, &fs, &ls );

    GpiMove( hps, &ptl );
    GpiBox( hps, fs.style!=PATSYM_NOSHADE ? DRO_FILL : DRO_OUTLINE, &ptl1, rx, ry );
  }

  GpiMove( hps, &ptl );

  if (fs.style!=PATSYM_NOSHADE) {
    set_fillstuff( hps, &fs );
    GpiBox( hps, DRO_FILL, &ptl1, rx, ry );
  }

  set_linestuff( hps, &ls );
  GpiBox( hps, DRO_OUTLINE, &ptl1, rx, ry );
}

//-----------------------------------------------------------------------------
void GtkEllipse::DrawShadow( HPS hps )
{
  HpsFocus f(hps, &mx);

  POINTL ptl;
  ptl.x = 2*ptl0.x - ptl1.x;
  ptl.y = 2*ptl0.y - ptl1.y;

  LONG rx = labs(ptl1.x - ptl.x);
  LONG ry = labs(ptl1.y - ptl.y);

  GpiMove( hps, &ptl );
  GpiBox( hps, DRO_OUTLINE, &ptl1, rx, ry );
}

//-------------------------------------------------------------------------
BOOL GtkEllipse::Read(Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part)
{
  StorageUnitGetValue(su, ev, sizeof(ptl0), (ODValue)&ptl0);
  StorageUnitGetValue(su, ev, sizeof(ptl1), (ODValue)&ptl1);
  StorageUnitGetValue(su, ev, sizeof(mx), (ODValue)&mx);

  StorageUnitGetValue(su, ev, sizeof(ls), (ODValue)&ls);
  StorageUnitGetValue(su, ev, sizeof(fs), (ODValue)&fs);
  StorageUnitGetValue(su, ev, sizeof(shadow), (ODValue)&shadow);
  this->UpdateBounds();
  return TRUE;
}


//-------------------------------------------------------------------------
BOOL GtkEllipse::Write(Environment *ev, ODStorageUnit* su, ODDraftKey key)
{
  if ( !GtkObject::Write(ev, su, key) ) return FALSE;
  StorageUnitSetValue(su, ev, sizeof(ptl0), (ODValue)&ptl0);
  StorageUnitSetValue(su, ev, sizeof(ptl1), (ODValue)&ptl1);
  StorageUnitSetValue(su, ev, sizeof(mx), (ODValue)&mx);

  StorageUnitSetValue(su, ev, sizeof(ls), (ODValue)&ls);
  StorageUnitSetValue(su, ev, sizeof(fs), (ODValue)&fs);
  StorageUnitSetValue(su, ev, sizeof(shadow), (ODValue)&shadow);
  return TRUE;
}

//-------------------------------------------------------------------------
POINTL GtkEllipse::QueryHandle( ULONG indx )
{
  POINTL pt;
  pt = (indx==0?ptl0:ptl1);
  transformPts( &mx, &pt, 1 );
  return pt;
}

//-------------------------------------------------------------------------
void GtkEllipse::SetHandle( ULONG indx, POINTL pt, BOOL commit )
{
  invertPts( &mx, &pt, 1 );
  if (indx) {
    ptl1 = pt;
  } else {
    ptl0 = pt;
  }
  this->UpdateBounds();
}

// --------------------------------------------------------------------------
PPOINTL  GtkEllipse::Pts( PULONG count )
{
   PPOINTL ptls;
   LONG    i, a, b;
   FIXED   angle;

   if (!*count) *count = 61;
   ptls = new POINTL[*count];

   a = labs( this->ptl1.x - this->ptl0.x );
   b = labs( this->ptl1.y - this->ptl0.y );
   angle = 0;

   for ( i=0; i<*count; i++ ) {
      angle = MAKEFIXED( i*360/(*count-1), 0);
      ptls[i].x = ptl0.x + Rcos2( a, angle );
      ptls[i].y = ptl0.y + Rsin2( b, angle );
      }

   transformPts( &mx, ptls, *count );
   return ptls;
   }

//-----------------------------------------------------------------------------
BOOL GtkEllipse::Hit( HPS hps, PPOINTL ptl, ULONG dx )
{
  int     i, crossings = 0;
  ULONG   n = 41;
  PPOINTL ptls = this->Pts( &n );

  for (i=1; i<n; i++) crossings += seg_cross( ptl, &ptls[i-1], &ptls[i] );

  delete ptls;
  return (crossings != 0);
}

//-------------------------------------------------------------------------
BOOL  GtkEllipse::FindPoint( PPOINTL ptl, ULONG mode,
                             PULONG distance, PPOINTL ptlHit )
{
  BOOL hit = FALSE;
  if (mode==NEAREST_CENTER) {
    POINTL pt;
    pt = ptl0;
    transformPts( &mx, &pt, 1 );
    hit = PointsHit( 1, &pt, ptl, mode, distance, ptlHit );
  } else {
    ULONG   n = 41;
    PPOINTL ptls = this->Pts(&n);
    hit = PointsHit( n, ptls, ptl, mode, distance, ptlHit );
    delete ptls;
  }
  return hit;
}

//-------------------------------------------------------------------------
GtkObject* GtkEllipse::Copy(BOOL shallow)
{
  POINTL ptl = {0,0};                            // dummy!
  GtkObject* obj = new GtkEllipse(ptl,ptl);
  if (obj) obj->CopyFrom(this);
  return obj;
}


//-------------------------------------------------------------------------
// GtkArc
//-------------------------------------------------------------------------

GtkArc::GtkArc(POINTL ptl0, LONG r0, FIXED Angle0, FIXED Angle1)
{
  id = 0;
  type = GOT_ARC;
  ls.color = RGB_GREEN;
  ls.style = LINETYPE_SOLID;
  ls.width = LINEWIDTH_THICK;
  fs.color = RGB_WHITE;
  fs.style = PATSYM_HALFTONE;
  shadow = PATSYM_NOSHADE;
  closing = 0;

  iAngle = Angle0;
  fAngle = Angle1;

  ptls[0] = ptl0;
  radius  = r0;
  ptls[1] = angle_point( &ptl0, iAngle, radius );
  ptls[2] = angle_point( &ptl0, fAngle, radius );

  mx = mx0;

  this->UpdateBounds();
}

//-------------------------------------------------------------------------
void GtkArc::CopyFrom( GtkObject* org, BOOL shallow )
{
  GtkArc* original = (GtkArc*)org;

  id = original->id;
  ls = original->ls;
  fs = original->fs;
  shadow = original->shadow;
  closing = original->closing;

  iAngle = original->iAngle;
  fAngle = original->fAngle;
  radius = original->radius;
  memcpy( ptls, original->ptls, 3*sizeof(POINTL) );
  mx = original->mx;

  this->UpdateBounds();
}

//-------------------------------------------------------------------------
void GtkArc::UpdateBounds()
{
  ULONG   n = 0;
  PPOINTL pts = this->Pts(&n);
  BoundPoints( &rclBounds, pts, n-1 );
  delete pts;

  if (shadow!=PATSYM_NOSHADE) {
    rclBounds.xRight += SHADOW_DX;
    rclBounds.yBottom -= SHADOW_DY;
  }
}

#if 0
#define DRAWARC_ARC    0
#define DRAWARC_PIE    1
#define DRAWARC_CHORD  2

//-----------------------------------------------------------------------------
void drawArcparts( HPS hps, ULONG n, PPOINTL pts, LONG mode )
{
  GpiMove( hps, pts );

  GpiPolyLine( hps, mode==DRAWARC_PIE ? n-1 : n-2, pts+1 );

  if (mode!=DRAWARC_ARC) GpiLine( hps, pts );
}
#endif

//-----------------------------------------------------------------------------
void GtkArc::Draw( HPS hps, ODFacet* facet )
{
  ULONG   n = 0, count;
  PPOINTL pts = this->Pts(&n);

  // draw the arc part... and maybe the radial arm!
  if ( closing==CLOSING_PIE ||
       (closing!=CLOSING_CHORD && fs.style!=PATSYM_NOSHADE) ) count = n;
  else if (closing == CLOSING_CHORD) count = n-1;
  else count = n-2;

  // draw SHADOW?
  if (shadow!=PATSYM_NOSHADE) {

    HpsFocus f2(hps);

    set_shadowstuff( hps, shadow, &fs, &ls );

    if (fs.style!=PATSYM_NOSHADE)
      GpiBeginArea ( hps, BA_BOUNDARY );

    GpiMove( hps, pts );
    GpiPolyLine( hps, count-1, pts+1 );

    if (fs.style!=PATSYM_NOSHADE)
      GpiEndArea(hps);
  }

  // fill it?
  if (fs.style!=PATSYM_NOSHADE) {

    set_fillstuff( hps, &fs );
    GpiBeginArea ( hps, BA_BOUNDARY );

    GpiMove( hps, pts );
    GpiPolyLine( hps, count-1, pts+1 );

    GpiEndArea(hps);
  }

  // outline it?
  if (ls.style!=LINETYPE_INVISIBLE) {
    set_linestuff( hps, &ls );

    // draw the arc part... and maybe the radial arm!
    if ( closing==CLOSING_PIE) count = n;
    else if (closing == CLOSING_CHORD) count = n-1;
    else count = n-2;

    GpiMove( hps, pts );
    GpiPolyLine( hps, count-1, pts+1 );
  }
  delete pts;
}

//-----------------------------------------------------------------------------
void GtkArc::DrawShadow( HPS hps )
{
  ULONG   n = 0, count;
  PPOINTL pts = this->Pts(&n);

  if ( closing==CLOSING_PIE ||
       (closing!=CLOSING_CHORD && fs.style!=PATSYM_NOSHADE) ) count = n;
  else if (closing == CLOSING_CHORD) count = n-1;
  else count = n-2;

  // draw the arc part... and maybe the radial arm!
  GpiMove( hps, pts );
  GpiPolyLine( hps, count-1, pts+1 );

  delete pts;
}

//-------------------------------------------------------------------------
BOOL GtkArc::Read(Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part)
{
  StorageUnitGetValue(su, ev, sizeof(iAngle), (ODValue)&iAngle);
  StorageUnitGetValue(su, ev, sizeof(fAngle), (ODValue)&fAngle);
  StorageUnitGetValue(su, ev, sizeof(radius), (ODValue)&radius);
  StorageUnitGetValue(su, ev, 3*sizeof(POINTL), (ODValue)ptls);
  StorageUnitGetValue(su, ev, sizeof(mx), (ODValue)&mx);

  StorageUnitGetValue(su, ev, sizeof(ls), (ODValue)&ls);
  StorageUnitGetValue(su, ev, sizeof(fs), (ODValue)&fs);
  StorageUnitGetValue(su, ev, sizeof(shadow), (ODValue)&shadow);
  StorageUnitGetValue(su, ev, sizeof(closing), (ODValue)&closing);
  this->UpdateBounds();
  return TRUE;
}

//-------------------------------------------------------------------------
BOOL GtkArc::Write(Environment *ev, ODStorageUnit* su, ODDraftKey key)
{
  if ( !GtkObject::Write(ev, su, key) ) return FALSE;

  StorageUnitSetValue(su, ev, sizeof(iAngle), (ODValue)&iAngle);
  StorageUnitSetValue(su, ev, sizeof(fAngle), (ODValue)&fAngle);
  StorageUnitSetValue(su, ev, sizeof(radius), (ODValue)&radius);
  StorageUnitSetValue(su, ev, 3*sizeof(POINTL), (ODValue)ptls);
  StorageUnitSetValue(su, ev, sizeof(mx), (ODValue)&mx);

  StorageUnitSetValue(su, ev, sizeof(ls), (ODValue)&ls);
  StorageUnitSetValue(su, ev, sizeof(fs), (ODValue)&fs);
  StorageUnitSetValue(su, ev, sizeof(shadow), (ODValue)&shadow);
  StorageUnitSetValue(su, ev, sizeof(closing), (ODValue)&closing);
  return TRUE;
}

//-------------------------------------------------------------------------
POINTL GtkArc::QueryHandle( ULONG indx )
{
  if (indx>=0 && indx<=2) return scaledPt( &mx, &ptls[indx] );

  POINTL pt = {0,0};
  return pt;
}

//-------------------------------------------------------------------------
void GtkArc::SetHandle( ULONG indx, POINTL pt, BOOL commit )
{
  FSMARCDATA   ad;
  invertPts( &mx, &pt, 1 );

  if (indx==1 || indx==2) {
    ad.center = ptls[0];
    if (indx==1) {
      if (!get_angle( &ad, &pt, FALSE ) ) return;
//      if (!calc_angle( &ptls[0], &pt, &radius, &iAngle ) ) return;
      ptls[1] = pt;
      radius = ad.r;
      iAngle = ad.fxA1;
      fAngle -= MAKEFIXED(360,0);
      while (fAngle<iAngle) fAngle += MAKEFIXED(360,0);
      ptls[2] = angle_point( &ptls[0], fAngle, radius );
    } else {
      ad.fxA1 = iAngle;
      radius = ad.r = (LONG)sqrt( D2( &ptls[0], &pt ) );
      if (!calc_sweep( &ad, &pt, FALSE ) ) return;
      fAngle = ad.fxA2;
      ptls[1] = angle_point( &ptls[0], iAngle, radius );
      ptls[2] = angle_point( &ptls[0], fAngle, radius );
    }
  } else {
    ad.center = pt;
    ptls[0] = pt;
    if (!get_angle( &ad, &ptls[1], FALSE ) ) return;
    if (!calc_sweep( &ad, &ptls[2], FALSE ) ) return;
    iAngle = ad.fxA1;
    fAngle = ad.fxA2;
    radius = ad.r;
    ptls[2] = angle_point( &ptls[0], fAngle, radius );
  }
  this->UpdateBounds();
}

// --------------------------------------------------------------------------
PPOINTL  GtkArc::Pts( PULONG count )
{
  PPOINTL ptl;
  LONG    i, steps;
  FIXED   angle;

  angle = fAngle - iAngle;
  steps = FIXEDINT(angle)/5;
  if (MAKEFIXED(steps*5,0)<angle ) steps += 1;
  *count = steps + 3;                                  // steps+1 + 2
                                                       // pie uses all,
  ptl = new POINTL[*count];                            // chord uses n-1

  angle = iAngle;
  for ( i=0; i<steps; i++ ) {
    ptl[i] = angle_point( ptls, angle, radius );
    angle += MAKEFIXED( 5, 0);
  }
  ptl[steps] = angle_point( ptls, fAngle, radius );

  if (closing == CLOSING_CHORD)
    ptl[steps+1] = ptl[0];
  else {
    ptl[steps+1] = *ptls;
    ptl[steps+2] = ptl[0];
  }

  transformPts( &mx, ptl, *count );
  return ptl;
}

//-----------------------------------------------------------------------------
BOOL GtkArc::Hit( HPS hps, PPOINTL ptl, ULONG dx )
{
  ULONG   n = 0;
  PPOINTL pts = this->Pts(&n);
  BOOL    hit = FALSE;

  if (closing || fs.style!=PATSYM_NOSHADE) {                  // solid obj
    int crossings = 0;
    // check the arc-part first
    for (int i=1; i<n-1; i++)
      crossings += seg_cross( ptl, &pts[i-1], &pts[i] );

    if (closing == CLOSING_CHORD)
      crossings += seg_cross( ptl, &pts[n-2], pts );
    else {
      crossings += seg_cross( ptl, &pts[n-2], &pts[n-1] );
      crossings += seg_cross( ptl, &pts[n-1], pts );
    }
    hit = (crossings != 0);
  } else {                                                   // line obj
    POINTL pt;
    ULONG  distance;
    for (int i=1; i<n-1; i++) {
      pt = segment_point( *ptl, pts[i], pts[i-1] );
      distance = D2( &pt, ptl );
      if (distance <= dx) {
        hit = TRUE;
        break;
      }
    }
  }
  delete pts;
  return hit;
}

//-------------------------------------------------------------------------
BOOL  GtkArc::FindPoint( PPOINTL ptl, ULONG mode,
                             PULONG distance, PPOINTL ptlHit )
{
  BOOL hit = FALSE;
  if (mode==NEAREST_CENTER) {
    POINTL pt = scaledPt( &mx, ptls );
    hit = PointsHit( 1, &pt, ptl, mode, distance, ptlHit );
  } else if (mode==NEAREST_ENDPT) {
    POINTL pts[3];
    ULONG n;
    pts[0] = ptls[1];
    pts[1] = ptls[2];
    if (closing || fs.style!=PATSYM_NOSHADE) {                  // solid obj
      pts[2] = ptls[0];
      n = 3;
    } else n = 2;
    transformPts( &mx, pts, n );
    hit = PointsHit( n, pts, ptl, mode, distance, ptlHit );
  } else {
    ULONG   n = 0;
    PPOINTL pts = this->Pts(&n);
    hit = PointsHit( n-1, pts, ptl, mode, distance, ptlHit );
    delete pts;
  }
  return hit;
}

//-------------------------------------------------------------------------
GtkObject* GtkArc::Copy(BOOL shallow)
{
  POINTL ptl = {0,0};                            // dummy!
  GtkObject* obj = new GtkArc(ptl,FIXED1,0,0);
  if (obj) obj->CopyFrom(this);
  return obj;
}


//-------------------------------------------------------------------------
// GtkGroup
//-------------------------------------------------------------------------

GtkGroup::GtkGroup(ODxOrderedCollection* objsIn)
{
  id = 0;
  type = GOT_GROUP;

  mx = mx0;

  if (objsIn) {
    count = objsIn->Count();
    objs = new PGTKOBJECT[count];
    long n = 0;

    GtkObject* obj = (GtkObject*)objsIn->First();
    while (obj && n<count) {
      objs[n] = obj;
      n++;
      obj = (GtkObject*)objsIn->After(obj);
    }

    this->UpdateBounds();

  } else {
    count = 0;
    objs = 0;
    rclBounds.xLeft = rclBounds.xRight = rclBounds.yBottom = rclBounds.yTop = 0;
  }
}

//-------------------------------------------------------------------------
GtkGroup::~GtkGroup()
{
  this->Delete();
}

//-------------------------------------------------------------------------
GtkObject* GtkGroup::Obj(ODULong Id)
{
  // direct match?
  if (this->id==Id) {
    return this;

  // contained??
  } else {
    GtkObject* obj;
    for (int i=0; i<count; i++) {
      obj = objs[i]->Obj(Id);
      if (obj) {
        return obj;
      }
    }
  }
  return NULL;
}

//-------------------------------------------------------------------------
GtkLink* GtkGroup::LinkObj(ODLink* aLink)
{
  GtkObject* obj;
  for (int i=0; i<count; i++) {
    obj = objs[i]->LinkObj(aLink);
    if (obj) {
      return (GtkLink*)obj;
    }
  }
  return NULL;
}

//-------------------------------------------------------------------------
void GtkGroup::ShowLink(BOOL fShow)
{
  for (int i=0; i<count; i++) {
    objs[i]->ShowLink(fShow);
  }
}

//-------------------------------------------------------------------------
void GtkGroup::Clear()
{
  if (count) {
    count = 0;
    delete objs;
    objs = 0;
  }
}

//-------------------------------------------------------------------------
void GtkGroup::Delete()
{
  if (count) {
    for (int i=0; i<count; i++)
      delete objs[i];
    count = 0;
    delete objs;
    objs = 0;
  }
}

//-------------------------------------------------------------------------
void GtkGroup::CopyFrom( GtkObject* org, BOOL shallow )
{
  int i;
  GtkGroup* original = (GtkGroup*)org;
  id = original->id;

  mx = original->mx;

  // free up any old objects??
  this->Delete();

  objs = new PGTKOBJECT[original->count];
  for (i=0; i<original->count; i++)  {
    objs[count] = original->objs[i]->Copy();
    if (objs[count]) count++;
  }

  this->UpdateBounds();
}

//-------------------------------------------------------------------------
void GtkGroup::UpdateBounds()
{
  if (count && objs) {
    RECTL rcl;
    rclBounds = objs[0]->Bounds();
    for (int i=1; i<count; i++) {
      rcl = objs[i]->Bounds();
      AddRect( &rclBounds, &rcl );
    }

    // now transform that bounding rect into world coords
    POINTL pts[4];
    pts[0].x = pts[3].x = rclBounds.xLeft;
    pts[1].x = pts[2].x = rclBounds.xRight;
    pts[0].y = pts[1].y = rclBounds.yBottom;
    pts[2].y = pts[3].y = rclBounds.yTop;
    transformPts( &mx, pts, 4 );

    // re-bound
    BoundPoints( &rclBounds, pts, 4 );
  } else {
    rclBounds.xRight = rclBounds.xLeft;
    rclBounds.yTop = rclBounds.yBottom;
  }
}

//-------------------------------------------------------------------------
void GtkGroup::Draw( HPS hps, ODFacet* facet )
{
  HpsFocus f(hps, &mx);

  for (int i=0; i<count; i++) {
    objs[i]->Draw(hps, facet);
  }
}

//-------------------------------------------------------------------------
void GtkGroup::DrawShadow( HPS hps )
{
  HpsFocus f(hps, &mx);

  for (int i=0; i<count; i++) {
    objs[i]->DrawShadow(hps);
  }
}

//-------------------------------------------------------------------------
BOOL GtkGroup::Read(Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part)
{
  StorageUnitGetValue(su, ev, sizeof(count), (ODValue)&count);
  StorageUnitGetValue(su, ev, sizeof(mx), (ODValue)&mx);

  if (count) {
    objs = new PGTKOBJECT[count];
    for (int i=0; i<count; i++)  {
      objs[i] = ReadObject( ev, su, key, part );
    }
  } else {
    objs = 0;
  }

  this->UpdateBounds();
  return TRUE;
}

//-------------------------------------------------------------------------
BOOL GtkGroup::Write(Environment *ev, ODStorageUnit* su, ODDraftKey key)
{
  if ( !GtkObject::Write(ev, su, key) ) return FALSE;

  StorageUnitSetValue(su, ev, sizeof(count), (ODValue)&count);
  StorageUnitSetValue(su, ev, sizeof(mx), (ODValue)&mx);

  for (int i=0; i<count; i++) {
    objs[i]->Write(ev, su, key);
  }

  return TRUE;
}

//-------------------------------------------------------------------------
BOOL GtkGroup::Hit( HPS hps, PPOINTL ptl, ULONG dx )
{
  POINTL pt = *ptl;
  invertPts( &mx, &pt, 1 );

  for (int i=0; i<count; i++) {
    if (objs[i]->Hit(hps, &pt, dx)) return TRUE;
  }

  // no hit..
  return FALSE;
}

//-------------------------------------------------------------------------
BOOL  GtkGroup::FindPoint( PPOINTL ptl, ULONG mode,
                         PULONG distance, PPOINTL ptlHit )
{
  BOOL hit = FALSE;

  POINTL ptl2, ptlHit2;
  ULONG  distance2;

  // convert to model coords
  ptl2 = *ptl;
  invertPts( &mx, &ptl2, 1 );
  distance2 = 5000000;

  // search in model space
  for (int i=0; i<count; i++) {
    if ( objs[i]->FindPoint(&ptl2, mode, &distance2, &ptlHit2) ) hit = TRUE;
  }

  // if hit, convert to real coords and check distance there
  if (hit) {
    transformPts( &mx, &ptlHit2, 1 );
    distance2 = D2( ptl, &ptlHit2 );

    // nearer in real coords?
    if (distance2 < *distance) {
      *ptlHit = ptlHit2;
      *distance = distance2;
      return TRUE;
    }
  }

  return FALSE;
}

//-------------------------------------------------------------------------
POINTL GtkGroup::QueryHandle( ULONG indx )
{
  POINTL pt;

  pt = (indx==0 ? *(PPOINTL)&rclBounds.xLeft
                : *(PPOINTL)&rclBounds.xRight);

//  pt = (indx==0?ptl0:ptl1);
//  transformPts( &mx, &pt, 1 );
  return pt;
}

//-------------------------------------------------------------------------
void GtkGroup::SetHandle( ULONG indx, POINTL pt, BOOL commit )
{
#if 0
  invertPts( &mx, &pt, 1 );
  if (indx) {
    ptl1 = pt;
  } else {
    ptl0 = pt;
  }
  this->UpdateBounds();
#endif
}

//-------------------------------------------------------------------------
GtkObject* GtkGroup::Copy(BOOL shallow)
{
  GtkObject* obj = new GtkGroup();
  if (obj) obj->CopyFrom(this);
  return obj;
}

//-------------------------------------------------------------------------
void GtkGroup::Move( LONG dx, LONG dy )
{
#if 0
  GtkObject::Move(dx, dy);
#else
  for (int i=0; i<count; i++) {
    objs[i]->Move( dx, dy );
  }
  this->UpdateBounds();
#endif
}

//-------------------------------------------------------------------------
void GtkGroup::Transform( MATRIXLF* mx )
{
#if 0
  GtkObject::Transform(mx);
#else
  for (int i=0; i<count; i++) {
    objs[i]->Transform( mx );
  }
  this->UpdateBounds();
#endif
}

//-------------------------------------------------------------------------
BOOL GtkGroup::SetAttribute(ULONG aType, LONG aVal)
{
  BOOL fChanged = FALSE;

  for (int i=0; i<count; i++) {
    fChanged = objs[i]->SetAttribute( aType, aVal) || fChanged;
  }

  return fChanged;
}


//-------------------------------------------------------------------------
// GtkLinkSource
//-------------------------------------------------------------------------
ODLinkKey LockLinkSource( Environment* ev, ODLinkSource* linkSource );

//-------------------------------------------------------------------------
GtkLinkSource::GtkLinkSource( GtkPic* pic )
{
  type = GOT_LINKSRC;
  picture = pic;
  count = 0;
  linkSource = kODNULL;
  fHighlighted = FALSE;
}

//-------------------------------------------------------------------------
GtkLinkSource::~GtkLinkSource()
{
  if (count)
    delete ids;

  if (linkSource)
    linkSource->Release( SOM_ENVIRONMENT );
}

//-------------------------------------------------------------------------
BOOL GtkLinkSource::Init( Environment* ev, GraphicsPart* part, ODULong* pData )
{
  // obtain the list of sub-objects from the linkSpec data
  count = pData[0];

  if (count) {
    ids = new ODULong[count];

    for (int i=0; i<count; i++) {
      ids[i] = pData[i+1];
    }

//    this->UpdateBounds();

    // create the OD linkSource object in this draft
    linkSource = part->GetStorageUnit(ev)->GetDraft(ev)->CreateLinkSource(ev, part);

    // fill it
//    this->Export(ev, part);

    return kODTrue;
  }

  return kODFalse;
}

//-------------------------------------------------------------------------
void GtkLinkSource::Export( Environment* ev, GraphicsPart* part )
{
  // general stuff we need...
  ODStorageUnit* partSU = part->GetStorageUnit(ev);
  ODSession*  fSession = partSU->GetSession(ev);
  ODUpdateID  updateID = fSession->UniqueUpdateID(ev);
  ODDraft*    fromDraft = partSU->GetDraft(ev);

  // get a lock on it
  ODLinkKey linkKey = LockLinkSource( ev, linkSource );

  //Remove the current updateID from the link
  linkSource->Clear(ev, updateID, linkKey);

  // get the linkSource storage unit
  ODStorageUnit* linkContentSU = linkSource->GetContentStorageUnit(ev, linkKey);
  ODDraft*       toDraft = linkContentSU->GetDraft(ev);

  //Begin a transaction since objects may be cloned into the link
  ODDraftKey key = fromDraft->BeginClone(ev, toDraft, kODNULL, kODCloneToLink);

  // init the prop/val location for the contents
  InitPropVal( ev, linkContentSU, kODPropContents, kKindGraphicsPart );

  // now externalize in a way that a GtkGroup can read the data back in!!
  // (must include the dummy mx write, with fixup on the far end)

  // write the number of objects being written
  StorageUnitSetValue(linkContentSU, ev, sizeof(count), (ODValue)&count);
  StorageUnitSetValue(linkContentSU, ev, sizeof(mx0), (ODValue)&mx0);
  ODULong realcount = 0;

  // write the objects out
  GtkObject* obj;
  for (int i=0; i<count; i++) {
    obj = picture->Object(ids[i]);
    if (obj) {
      obj->Write(ev, linkContentSU, key);
      realcount++;
    }
  }

  // if objects have been deleted, the count will be less!!!!!
  if (realcount<count) {
    SetFocus( ev, linkContentSU, kODPropContents, kKindGraphicsPart );
    StorageUnitSetValue(linkContentSU, ev, sizeof(realcount), (ODValue)&realcount);

    // if all the objects have been deleted should we remove the linkSource???
    // if we had full undo, the deleted objects could be restored... should
    // probably remove the linksource at doc close if realcount is 0.
    if (!realcount) {
    }
  }

  // all done
  fromDraft->EndClone(ev, key);
  // unlock should trigger any live update links
  linkSource->Unlock(ev, linkKey);
}

//-------------------------------------------------------------------------
void GtkLinkSource::UpdateBounds()
{
  if (count) {
    RECTL rcl;
    GtkObject* obj = picture->Object(ids[0]);                // EXPOSURE!
    rclBounds = obj->Bounds();                              // no check for obj != 0 ...
    for (int i=1; i<count; i++) {
      obj = picture->Object(ids[i]);
      if (obj) {
        rcl = obj->Bounds();
        AddRect( &rclBounds, &rcl );
      }
    }
  } else {
    rclBounds.xLeft = rclBounds.xRight = 0;
  }
}

//-------------------------------------------------------------------------
void GtkLinkSource::Draw( HPS hps, ODFacet* facet )
{
  if (fHighlighted) {
    GpiSetColor( hps, RGB_RED );
    GpiSetLineType( hps, LINETYPE_SOLID );
    GpiSetLineWidth( hps, LINEWIDTH_NORMAL );
    GpiSetMix( hps, FM_DEFAULT );

    RECTL rcl = rclBounds;
    GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 2, (PPOINTL)&rcl.xLeft );
    InflateRect( &rcl, 2, 2 );
    GpiConvert( hps, CVTC_DEVICE, CVTC_WORLD, 2, (PPOINTL)&rcl.xLeft );
    GpiMove( hps, (PPOINTL)&rcl.xLeft );
    GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rcl.xRight, 0, 0 );

    GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 2, (PPOINTL)&rcl.xLeft );
    InflateRect( &rcl, 3, 3 );
    GpiConvert( hps, CVTC_DEVICE, CVTC_WORLD, 2, (PPOINTL)&rcl.xLeft );
    GpiMove( hps, (PPOINTL)&rcl.xLeft );
    GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rcl.xRight, 0, 0 );
  }
}

//-------------------------------------------------------------------------
BOOL GtkLinkSource::Read(Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part)
{
  BOOL rc = TRUE;
  StorageUnitGetValue(su, ev, sizeof(count), (ODValue)&count);

  if (count) {
    ids = new ODULong[count];
    StorageUnitGetValue(su, ev, count*sizeof(ODULong), (ODValue)&ids[0]);

    ODStorageUnitRef aSURef;
    StorageUnitGetValue(su, ev, sizeof(ODStorageUnitRef), &aSURef);

    if (su->IsValidStorageUnitRef(ev, aSURef)) {
      ODID linkSourceID = su->GetIDFromStorageUnitRef(ev, aSURef);
      linkSource = su->GetDraft(ev)->AcquireLinkSource(ev, linkSourceID);

      // Ensure the link source object references this part as the source
      // of the link
      linkSource->SetSourcePart(ev, part->GetStorageUnit(ev));

    } else {
      // Remove the link from this part's content. This is part specific.
      delete ids;
      ids = 0;
      count = 0;
      rc = FALSE;
    }
  } else {
    ids = 0;
  }

  this->UpdateBounds();
  return rc;
}

//-------------------------------------------------------------------------
BOOL GtkLinkSource::Write(Environment *ev, ODStorageUnit* su, ODDraftKey key)
{
  if ( !GtkObject::Write(ev, su, key) ) return FALSE;

  StorageUnitSetValue(su, ev, sizeof(count), (ODValue)&count);
  if (count) {
    StorageUnitSetValue(su, ev, count*sizeof(ODULong), (ODValue)&ids[0]);

    ODStorageUnitRef aSURef;
    su->GetStrongStorageUnitRef(ev, linkSource->GetStorageUnit(ev)->GetID(ev), aSURef);
    StorageUnitSetValue(su, ev, sizeof(ODStorageUnitRef), (ODValue)&aSURef);
  }

  return TRUE;
}

//-------------------------------------------------------------------------
void GtkLinkSource::UpdateIfNecessary( Environment* ev, GraphicsPart* part,
                                       ODxOrderedCollection* baseObjs )
{
  // for now, just DO it.  *should* check to see if any sub-objs have changed

  // FIRST, compare against passed in base set to correct for any deleted
  // objects AND to correct the z-order.
  int n=0;
  ODULong* tmpIds = new ODULong[count];

  // iterate through the set of picture objects
  GtkObject* obj = (GtkObject*)baseObjs->First();
  while (obj && n<count) {

    // for each one, look for a hit in the link source set
    for (int i=0; i<count; i++) {

      // if we have a match, lets add the id to our corrected, ordered set
      if (ids[i] && obj == picture->Object(ids[i])) {
        tmpIds[n++] = ids[i];
        ids[i] = 0;
        break;
      }
    }

    // next object
    obj = (GtkObject*)baseObjs->After(obj);
  }

  // so lets swap in the new data.  may not have changed, but its more expensive
  // to look than do the swap!
  delete ids;
  ids = tmpIds;
  count = n;

  // should we tell the links?  :)
  if (linkSource->IsAutoUpdate(ev)) {
    this->Export(ev, part);
  }

  // adjust bounds for ShowLinks function
  this->UpdateBounds();
}

//-------------------------------------------------------------------------
BOOL GtkLinkSource::SpecMatch( ODULong* pData )
{
  // are the object counts the same?
  if (pData[0] != count)
    return FALSE;

  // does every object in the spec match an id in our list??
  for (int i=1; i<=count; i++) {                    // pData is 1-based
    for (int j=0; j<count; j++) {                   // ids is 0-based
      // id match?
      if (pData[i]==ids[j]) break;
    }
    // no match before end of j loop...
    if (j==count)
      return FALSE;
  }
  // all had matches
  return TRUE;
}


//-------------------------------------------------------------------------
// GtkLink
//-------------------------------------------------------------------------
ODLinkKey LockLink( Environment* ev, ODLink* link );

BOOL GtkLink::Init( Environment* ev, GraphicsPart* part, ODLink* link )
{
  _link = link;
  _linkInfo.change = kODUnknownUpdate;
  _linkInfo.kind = kKindGraphicsPart;
  _linkInfo.autoUpdate = kODTrue;

  rclBounds.xLeft = rclBounds.xRight = 0;
  this->Update(ev, part);
  _link->RegisterDependent(ev, part, _linkInfo.change);
  return kODTrue;
}

//-------------------------------------------------------------------------
void GtkLink::Draw( HPS hps, ODFacet* facet )
{
  if (fHighlighted) {
    GpiSetColor( hps, RGB_BLUE );
    GpiSetLineType( hps, LINETYPE_SOLID );
    GpiSetLineWidth( hps, LINEWIDTH_NORMAL );
    GpiSetMix( hps, FM_DEFAULT );

    RECTL rcl = rclBounds;
    GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 2, (PPOINTL)&rcl.xLeft );
    InflateRect( &rcl, 2, 2 );
    GpiConvert( hps, CVTC_DEVICE, CVTC_WORLD, 2, (PPOINTL)&rcl.xLeft );
    GpiMove( hps, (PPOINTL)&rcl.xLeft );
    GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rcl.xRight, 0, 0 );

    GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 2, (PPOINTL)&rcl.xLeft );
    InflateRect( &rcl, 3, 3 );
    GpiConvert( hps, CVTC_DEVICE, CVTC_WORLD, 2, (PPOINTL)&rcl.xLeft );
    GpiMove( hps, (PPOINTL)&rcl.xLeft );
    GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rcl.xRight, 0, 0 );
  }

  GtkGroup::Draw(hps, facet);
}

//-------------------------------------------------------------------------
BOOL GtkLink::Read(Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part)
{
  GtkGroup::Read(ev, su, key, part);

  if (count) {

    ODStorageUnitRef aSURef;
    StorageUnitGetValue(su, ev, sizeof(ODStorageUnitRef), &aSURef);
    StorageUnitGetValue(su, ev, sizeof(ODLinkInfo), &_linkInfo);

    if (su->IsValidStorageUnitRef(ev, aSURef)) {
      _linkID = su->GetIDFromStorageUnitRef(ev, aSURef);
      _readSU = su;
     } else {
       _linkID = 0;
     }
  } else {
    _linkID = 0;
  }

  // on a regular read we go ahead and init here...
//  if (!key)
//    this->PostReadInit(ev, part, FALSE );

  if (_linkID) {
    _link = _readSU->GetDraft(ev)->AcquireLink(ev, _linkID, kODNULL);
  } else {
    _link = kODNULL;
  }

  if (_link) {

    // see if there has been any update since the last time we were running!
    if ( !key && _linkInfo.change != _link->GetUpdateID(ev) )
      this->Update(ev, part);

    // let the availability server know we want updates
    _link->RegisterDependent(ev, part, _linkInfo.change);
  }

  this->UpdateBounds();

  return kODTrue;
}

//-------------------------------------------------------------------------
void GtkLink::PostReadInit( Environment *ev, GraphicsPart* part, BOOL cloned )
{
}

//-------------------------------------------------------------------------
BOOL GtkLink::Write(Environment *ev, ODStorageUnit* su, ODDraftKey key)
{
  // for now, we kludge the write for a clone op since we haven't gotten
  // straight all the clone details.  This means that LinkTargets that
  // get cloned out into Drag/Drop, clipboard, SaveAs drafts, etc. will
  // lose their linkage and you will be left with a simple Group.
  if (key)
    type = GOT_GROUP;

  ODBoolean rc = GtkGroup::Write(ev, su, key);
  type = GOT_LINK;

  if ( key || !rc ) return rc;

  if (count) {

    ODStorageUnitRef aSURef;
    su->GetStrongStorageUnitRef(ev, _link->GetStorageUnit(ev)->GetID(ev), aSURef);
    StorageUnitSetValue(su, ev, sizeof(ODStorageUnitRef), (ODValue)&aSURef);

    StorageUnitSetValue(su, ev, sizeof(ODLinkInfo), &_linkInfo);
  }

  return TRUE;
}

//-------------------------------------------------------------------------
// invoke the GtkObject invocation which modifies mx rather than the
// individual objects!
//-------------------------------------------------------------------------
void GtkLink::Move( LONG dx, LONG dy )
{
#if 1
  GtkObject::Move(dx, dy);
#else
  for (int i=0; i<count; i++) {
    objs[i]->Move( dx, dy );
  }
  this->UpdateBounds();
#endif
}

//-------------------------------------------------------------------------
// invoke the GtkObject invocation which modifies mx rather than the
// individual objects!
//-------------------------------------------------------------------------
void GtkLink::Transform( MATRIXLF* mx )
{
#if 1
  GtkObject::Transform(mx);
#else
  for (int i=0; i<count; i++) {
    objs[i]->Transform( mx );
  }
  this->UpdateBounds();
#endif
}

//-------------------------------------------------------------------------
void GtkLink::Update(Environment* ev, GraphicsPart* part)
{
  BOOL adjustPosition = (rclBounds.xLeft != rclBounds.xRight);
  POINTL oldPos;

  if (adjustPosition) {
    oldPos.x = rclBounds.xLeft;
    oldPos.y = rclBounds.yBottom;

    // remove the old stuff
    this->Delete();
  }

  // general stuff we need...
  ODStorageUnit* partSU = part->GetStorageUnit(ev);
  ODSession*  fSession = partSU->GetSession(ev);
  ODDraft*    toDraft = partSU->GetDraft(ev);

  // get a lock on it
  ODLinkKey linkKey = LockLink( ev, _link );

  _linkInfo.change = _link->GetUpdateID(ev);
  _linkInfo.changeTime = _link->GetChangeTime(ev);

  // get the linkSource storage unit
  ODStorageUnit* linkContentSU = _link->GetContentStorageUnit(ev, linkKey);
  ODDraft*       fromDraft = linkContentSU->GetDraft(ev);

  //Begin a transaction since objects may be cloned from the link
  ODDraftKey key = fromDraft->BeginClone(ev, toDraft, kODNULL, kODCloneFromLink);

  // position the stream pointer
  if (SetFocus( ev, linkContentSU, kODPropContents, kKindGraphicsPart )) {
    // save the old group matrix
    MATRIXLF tmpMx = mx;

    // read 'em in
    GtkGroup::Read(ev, linkContentSU, key, part);

    // restore the group matrix
    mx = tmpMx;
  }

  // all done
  fromDraft->EndClone(ev, key);
  _link->Unlock(ev, linkKey);

  this->UpdateBounds();

  // crude attempt to keep things positioned correctly...
  if ( adjustPosition &&
       (oldPos.x != rclBounds.xLeft || oldPos.y != rclBounds.yBottom) ) {
    this->Move( oldPos.x - rclBounds.xLeft, oldPos.y - rclBounds.yBottom );
  }
}


