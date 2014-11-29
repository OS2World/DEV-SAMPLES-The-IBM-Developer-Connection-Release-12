
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

#ifndef _ORDCOLL_
#include "OrdColl.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <builtin.h>
#include <string.h>

#include "gtkpart.xih"

#include "gtkbase.h"
#include "gtkpic.h"
#include "gtkres.h"
#include "mygpi.h"
#include "actions.h"
#include "platform.h"

#define ODDebug2

VOID CalcBounds( HMF hmf, PRECTL prcl );
BOOL DrawMeta2( HPS hps, HMF hmf );

//-------------------------------------------------------------------------
// GtkPic class stuff
//-------------------------------------------------------------------------

GtkPic::GtkPic()
{
  ulViewOpts = VO_OPAQUE;

  ls.color = RGB_BLACK;
  ls.style = LINETYPE_SOLID;
  ls.width = LINEWIDTH_THICK;
  fs.color = RGB_BLUE;
  fs.style = PATSYM_NOSHADE;
  shadow = PATSYM_NOSHADE;

  rclPic.xLeft = rclPic.yBottom = 0;
  rclPic.xRight = 6000;
  rclPic.yTop = 4000;

  nextID = 1;
  fClipEmbeds = TRUE;
  fLinksVisible = FALSE;
  objs = new ODxOrderedCollection;
  selected = new ODxOrderedCollection;
  linksrcs = new ODxOrderedCollection;
  lastHit = (GtkObject*)0;
  lastSelected = (GtkObject*)0;
  rclBounds.xLeft = rclBounds.xRight = 0;
}

//-------------------------------------------------------------------------
GtkPic::~GtkPic()
{
  if (objs) delete objs;
  if (selected) delete selected;
  if (linksrcs) delete linksrcs;
}

//-------------------------------------------------------------------------
void GtkPic::AddObject(GtkObject* newObject, BOOL keepID)
{
  objs->AddLast( newObject );

  if (!keepID)
    newObject->SetId( nextID++ );
}

//-------------------------------------------------------------------------
HRGN MarksRegion( HPS hps, PPOINTL ptls )
{
  RECTL  rclSizers[8];
  for (int i=0; i<8; i++) {
    rclSizers[i].xLeft   = rclSizers[i].xRight  = ptls[i].x;
    rclSizers[i].yBottom = rclSizers[i].yTop    = ptls[i].y;
  }
  GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 2*8, (PPOINTL)&rclSizers[0].xLeft );

  for (i=0; i<8; i++) {
    rclSizers[i].xLeft   -= 3;
    rclSizers[i].xRight  += 4;
    rclSizers[i].yBottom -= 3;
    rclSizers[i].yTop    += 4;
  }

  return GpiCreateRegion( hps, 8, rclSizers );
}

//-------------------------------------------------------------------------
// optional rcl in WORLD coords... if prcl is NULL redo whole pic.
//-------------------------------------------------------------------------
void GtkPic::ClipEmbeds( HPS hps, ODFacet* facet, PRECTL prcl )
{
//  if (fClipEmbeds && (objs->Count() > 1) ) {
  if (objs->Count()) {
    HRGN frameRgn;
    GtkObject* obj;
    GtkObject* subobj;
    RECTL  rcl, subrcl;

// DosBeep(880,50);
// DosSleep(100);

    rcl = rclPic;
    GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 2, (PPOINTL)&rcl.xLeft );
    frameRgn = GpiCreateRegion( hps, 1, &rcl );

    if (selected->Count()) {
      HRGN marksRgn = MarksRegion( hps, ptlSizers );
      GpiCombineRegion( hps, frameRgn, frameRgn, marksRgn, CRGN_DIFF );
      GpiDestroyRegion( hps, marksRgn );
    }

    obj = (GtkObject*)objs->First();
    while (obj) {
      // is this an embedded part?
      if (obj->IsEmbed()) {
        rcl = obj->Bounds();
        // is the part within the update region?
        if (prcl && !RectsIntersect( prcl, &rcl ) ) {
          obj = (GtkObject*)objs->After(obj);
          continue;
        }

        HRGN clipRgn = GpiCreateRegion( hps, 0, NULL);

        // iterate through all objects ABOVE this Part
        subobj = (GtkObject*)objs->After(obj);

        while (subobj) {
          // only worry about embeds
          if (subobj->IsEmbed()) {
            // does the subobj overlap the Part in question?
            subrcl = subobj->Bounds();
            if (RectsIntersect( &subrcl, &rcl ) ) {
              // add its area to the clip region
              HRGN subRgn = subobj->Region( hps, facet );
              if (subRgn) {
                GpiCombineRegion( hps, clipRgn, clipRgn, subRgn, CRGN_OR );
                GpiDestroyRegion( hps, subRgn );
              }
            }
          }
          subobj = (GtkObject*)objs->After(subobj);
        }
        // combine, frame region MINUS the overlapping areas
        GpiCombineRegion( hps, clipRgn, frameRgn, clipRgn, CRGN_DIFF );
        obj->SetRegion( clipRgn, facet );

//        GpiDestroyRegion( hps, clipRgn );
      }
      obj = (GtkObject*)objs->After(obj);
    }
    GpiDestroyRegion( hps, frameRgn );
  }
}

//-------------------------------------------------------------------------
GtkObject* GtkPic::Object(ULONG id)
{
  GtkObject* theObj;
  GtkObject* obj = (GtkObject*)objs->First();
  while (obj) {
//    if (obj->Id()==id) {
    theObj = obj->Obj(id);
    if (theObj) {
      return theObj;
    }
    obj = (GtkObject*)objs->After(obj);
  }
  return NULL;
}

//-------------------------------------------------------------------------
VOID  GtkPic::ClassifySelection( ULONG* objFlags, ULONG* stateFlags )
{
  *objFlags = *stateFlags = 0;

  if (lastHit) {                                              // move object

    *objFlags = lastHit->Type();

    if ( lastHit->IsLocked() ) {
      *stateFlags |= GOS_ANYLOCKED;
    } else {
      *stateFlags |= GOS_ANYUNLOCKED;
    }

  } else if (selected->Count()) {

    GtkObject* obj = (GtkObject*)selected->First();
    while (obj) {

      *objFlags |= obj->Type();

      if ( obj->IsLocked() ) {
        *stateFlags |= GOS_ANYLOCKED;
      } else {
        *stateFlags |= GOS_ANYUNLOCKED;
      }

      if ( obj->IsEmbed() ) {
        LONG transparent;
        obj->QueryAttribute( TRANSLUCENCE, &transparent );
        if ( transparent ) {
          *stateFlags |= GOS_ANYTRANSPARENT;
        } else {
          *stateFlags |= GOS_ANYOPAQUE;
        }
      }

      obj = (GtkObject*)selected->After(obj);
    }
  }
}

//-------------------------------------------------------------------------
void GtkPic::Draw(HPS hps, PRECTL prclDest, ODFacet* facet)
{
  // do scaling and clipping here?
  if (prclDest) {
    SetScaling( hps, &rclPic, prclDest, kODFalse );

    // the following doesn't seem to work...
    GpiBeginPath(hps, 1);
    GpiMove( hps, (PPOINTL)&rclPic.xLeft );
    GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rclPic.xRight, 0, 0);
    GpiEndPath(hps);

    GpiSetClipPath( hps, 1, 0 );
  }

  // render all our content objects (including embeds)
  GtkObject* obj = (GtkObject*)objs->First();
  while (obj) {
    obj->Draw(hps, facet);
    obj = (GtkObject*)objs->After(obj);
  }

  // links highlighting?
  if (fLinksVisible) {
    obj = (GtkObject*)linksrcs->First();
    while (obj) {
      obj->Draw(hps, facet);
      obj = (GtkObject*)linksrcs->After(obj);
    }
  }

  // draw a bounding box for the thumbnail??
  if (prclDest) {
    GpiSetClipPath( hps, 0, 0 );

    GpiSetColor(hps, RGB_BLACK );
    GpiMove( hps, (PPOINTL)&rclPic.xLeft );
    GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rclPic.xRight, 0, 0);
  }
}

//-------------------------------------------------------------------------
void GtkPic::DrawShadows(HPS hps, GtkObject* obj)
{
  if (!obj && objs->Contains(lastHit)) obj = lastHit;

  if (obj) {                                              // move object
    if (!obj->IsLocked()) obj->DrawShadow(hps);
  } else if (selected->Count()) {                         // move selection
    obj = (GtkObject*)selected->First();
    while (obj) {
      if (!obj->IsLocked()) obj->DrawShadow(hps);
      obj = (GtkObject*)selected->After(obj);
    }
  }
}

//-------------------------------------------------------------------------
void GtkPic::DrawMarks(HPS hps, BOOL fAltMarked)
{
  if (selected->Count()) {
    RECTL rcl;
    ULONG  i, n;
    POINTL pt;
    LONG d2, d3, d4, d5, d7, d8;
    POINTL ds[4] = { {0,0}, {2,3}, {4,5}, {7,8} };
    GpiConvert( hps, CVTC_DEVICE, CVTC_WORLD, 4, ds );
    d2 = ds[1].x - ds[0].x;
    d3 = ds[1].y - ds[0].y;
    d4 = ds[2].x - ds[0].x;
    d5 = ds[2].y - ds[0].y;
    d7 = ds[3].x - ds[0].x;
    d8 = ds[3].y - ds[0].y;

    GpiSetMix( hps, FM_XOR );
    // this one assumes RGP hps...
//    GpiSetColor( hps, 0x00FFFFFF );                        // 0x00aaaaaa
    GpiSetColor( hps, 0x00c9c9c9 );                        // 0x00aaaaaa
    GpiSetLineType ( hps, LINETYPE_DEFAULT );
    GpiSetLineWidth( hps, LINEWIDTH_DEFAULT );
    GpiSetPattern( hps, PATSYM_HALFTONE );

    GtkObject* obj = (GtkObject*)selected->First();
    while (obj) {
      n = obj->HandleCount();
      for (i=0; i<n; i++) {
        pt = obj->QueryHandle(i);
        rcl.xLeft = pt.x-d3; rcl.yBottom = pt.y-d3;
        rcl.xRight = pt.x+d3; rcl.yTop = pt.y+d3;
        GpiMove( hps, (PPOINTL)&rcl.xLeft );
        GpiBox( hps, fAltMarked?DRO_OUTLINE:DRO_FILL, (PPOINTL)&rcl.xRight,0,0);
      } /* endfor */
      obj = (GtkObject*)selected->After(obj);
    }

    // draw bounding box
    GpiSetLineType ( hps, LINETYPE_SHORTDASH );
    GpiMove( hps, (PPOINTL)&rclBounds.xLeft );
    GpiBox( hps, DRO_OUTLINE, (PPOINTL)&rclBounds.xRight, d7*4, d7*4);

    // paint outer sizing marks
    GpiSetLineType ( hps, LINETYPE_DEFAULT );
    GpiSetPattern( hps, PATSYM_SOLID );
    HRGN marksRgn = MarksRegion( hps, ptlSizers );
    GpiPaintRegion(hps, marksRgn);

    SIZEL sizlThickness = {1, 1};
    GpiSetColor(hps, RGB_WHITE);
    GpiFrameRegion(hps, marksRgn, &sizlThickness);

    GpiDestroyRegion( hps, marksRgn );
  }
  GpiSetLineType( hps, LINETYPE_DEFAULT );
  GpiSetMix( hps, FM_DEFAULT );
}

//-------------------------------------------------------------------------
void  GtkPic::ShowLinks( BOOL fShow )
{
//  if (BOOL_XOR(fShow, fLinksVisible)) {
    fLinksVisible = fShow;

    GtkObject* obj = (GtkObject*)objs->First();
    while (obj) {
      obj->ShowLink(fShow);
      obj = (GtkObject*)objs->After(obj);
    }

    obj = (GtkObject*)linksrcs->First();
    while (obj) {
      obj->ShowLink(fShow);
      obj = (GtkObject*)linksrcs->After(obj);
    }

//  }
}

#define BOXLET_SIZE  8

//-------------------------------------------------------------------------
// Returns TRUE if *ptl is within a boxlet of a handle to some object.
// Modifies *ptl to be the actual handle location...
//-------------------------------------------------------------------------
BOOL GtkPic::HitHandle(HPS hps, PPOINTL ptl)
{
  if (selected->Count()) {
    GtkObject* obj;
    POINTL pt0;

    pt0 = *ptl;
    GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 1, &pt0 );

    obj = (GtkObject*)selected->Last();
    while (obj) {
      ULONG i, n;
      POINTL pt,ptA;
      n = obj->HandleCount();
      for (i=0; i<n; i++) {
        pt = obj->QueryHandle(i);
        ptA = pt;
        GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 1, &pt );
        if (abs(pt.x-pt0.x)<BOXLET_SIZE &&abs(pt.y-pt0.y)<BOXLET_SIZE) {
          *ptl = ptA;
          lastHit = obj;
          handleIndx = i;
          return TRUE;
        }
      }
      obj = (GtkObject*)selected->Before(obj);
    }
  }
  return FALSE;
}

//-------------------------------------------------------------------------
// Returns TRUE if *ptl is within a boxlet of a handle to some object.
// Modifies *ptl to be the actual handle location...
//-------------------------------------------------------------------------
BOOL GtkPic::HitSizers(HPS hps, PPOINTL ptl)
{
//  lastHit = (GtkObject*)NULL;

  if (selected->Count()) {
    ULONG i;
    POINTL pts[9];
    pts[8] = *ptl;
    memcpy( pts, ptlSizers, 8*sizeof(POINTL) );
    GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 9, pts );
    for (i=0; i<8; i++) {
      if (pts[8].x >= pts[i].x-3 && pts[8].x <= pts[i].x+3 &&
          pts[8].y >= pts[i].y-3 && pts[8].y <= pts[i].y+3    ) {
        *ptl = ptlSizers[i];
        handleIndx = i;
        return TRUE;
      }
    }
  }
  return FALSE;
}

// part of fix for paste/embed bug
//-------------------------------------------------------------------------
GtkObject* GtkPic::HitObject( PLONG handle )
{
  if (handle) *handle = handleIndx;

  if (lastHit) return lastHit;

  if (selected->Count()==1)
    return (GtkObject*)selected->First();

  return 0;
}

//-------------------------------------------------------------------------
void GtkPic::BoundSelected()
{
  RECTL rcl;
  GtkObject* obj;
  if (selected->Count()) {
    ODxOrderedCollectionIterator  iter(selected);
    obj = (GtkObject*)iter.First();
    rclBounds = obj->Bounds();

    obj = (GtkObject*)iter.Next();
    while (obj != kODNULL) {
      rcl = obj->Bounds();
      AddRect( &rclBounds, &rcl );
      obj = (GtkObject*)iter.Next();
    }

    // KLUDGE.  this needs to be some screen len converted to world coords...
    InflateRect( &rclBounds, BOUNDS_BORDER, BOUNDS_BORDER );
//    InflateRect( &rclBounds, 125, 125 );

    ptlSizers[0].x = ptlSizers[3].x = ptlSizers[5].x = rclBounds.xLeft;
    ptlSizers[2].x = ptlSizers[4].x = ptlSizers[7].x = rclBounds.xRight;
    ptlSizers[1].x = ptlSizers[6].x = (rclBounds.xLeft + rclBounds.xRight)/2;
    ptlSizers[0].y = ptlSizers[1].y = ptlSizers[2].y = rclBounds.yBottom;
    ptlSizers[5].y = ptlSizers[6].y = ptlSizers[7].y = rclBounds.yTop;
    ptlSizers[3].y = ptlSizers[4].y = (rclBounds.yBottom + rclBounds.yTop)/2;
  } else {
    rclBounds.xLeft = rclBounds.xRight = 0;
  }
}

//-------------------------------------------------------------------------
void GtkPic::Select(GtkObject* obj, ULONG mode)
{
  if (objs->Count()) {

    if (mode != SELECT_TOGGLEONE && selected->Count()) {
      ODxOrderedCollectionIterator  iter(selected);
      GtkObject* objX;

      for (objX = (GtkObject*)iter.First();
            iter.IsNotComplete(); objX = (GtkObject*)iter.Next())
        objX->SetSelected(FALSE);

      selected->RemoveAll();
    }

    if (obj) {
      if (mode == SELECT_TOGGLEONE &&
          selected->Count()        &&
          selected->Contains(obj) ) {
        selected->Remove(obj);
        obj->SetSelected(FALSE);
      } else {
        selected->AddLast(obj);
        obj->SetSelected(TRUE);
      }
    }
    // signal to deselect any locked objects
    else if (mode == SELECT_TOGGLEONE && selected->Count()) {
      // make a copy of the selection
      ODxOrderedCollection* oldobjs = new ODxOrderedCollection;
      ODxOrderedCollectionIterator  iter(selected);
      for (obj = (GtkObject*)iter.First();
            iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {
        oldobjs->AddLast( obj );
      }

      // iterate through the copy and remove locked ones from selected
      ODxOrderedCollectionIterator  iter2(oldobjs);

      for (obj = (GtkObject*)iter.First();
            iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {
        if (obj->IsLocked()) selected->Remove(obj);
      }
      delete oldobjs;
    }

    this->BoundSelected();
  }
}

//-------------------------------------------------------------------------
void GtkPic::SelectAll()
{
  if (objs->Count() > selected->Count()) {

    if (selected->Count())
      selected->RemoveAll();

    ODxOrderedCollectionIterator  iter(objs);
    GtkObject* obj;

    for (obj = (GtkObject*)iter.First();
          iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {
      selected->AddLast(obj);
      obj->SetSelected(TRUE);
    }

    this->BoundSelected();
  }
}

//-------------------------------------------------------------------------
BOOL GtkPic::Select(HPS hps, PPOINTL ptl, ULONG mode)
{
  if (objs->Count()) {
    GtkObject* obj;
    POINTL pt0;
    ULONG aperture;

    pt0 = *ptl;
    GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 1, &pt0 );
    pt0.x += 5;
    GpiConvert( hps, CVTC_DEVICE, CVTC_WORLD, 1, &pt0 );
    aperture = (pt0.x-ptl->x)*(pt0.x-ptl->x);

    // check for invalid CONTINUATION-selection
    if (mode == SELECT_NEXTONE && !selected->Contains(lastSelected) )
       mode = SELECT_ONLYONE;

    if (mode != SELECT_TOGGLEONE && selected->Count()) {
      GtkObject* objX = (GtkObject*)selected->First();
      while (objX) {
        objX->SetSelected(FALSE);
        objX = (GtkObject*)selected->After(objX);
      }
      selected->RemoveAll();
    }

    obj = (GtkObject*)objs->Last();

    // CONTINUING from last hit?  (done by user with Shift+Click)
    if (mode == SELECT_NEXTONE && lastSelected != (GtkObject*)NULL) {
      // FIRST, iterate to the last-hit object
      while (obj != kODNULL && obj != lastSelected)
        obj = (GtkObject*)objs->Before(obj);
      // continue with the next one (should ALWAYS) do this step...
      if (obj != kODNULL)
        obj = (GtkObject*)objs->Before(obj);
      // if last-hit was first in list, reset to the end
      if (obj == kODNULL)
        obj = (GtkObject*)objs->Last();
    }

    while (obj != kODNULL &&
           (mode != SELECT_NEXTONE || obj != lastSelected) ) {
      if ( obj->Hit(hps,ptl,aperture) ) {
        lastSelected = obj;

        if (mode == SELECT_TOGGLEONE &&
            selected->Count()        &&
            selected->Contains(obj) ) {
          selected->Remove(obj);
          obj->SetSelected(FALSE);
        } else {
          selected->AddLast(obj);
          obj->SetSelected(TRUE);
        }

        this->BoundSelected();
        return TRUE;
      }
      obj = (GtkObject*)objs->Before(obj);

      // got to 0 and need to loop for CONTINUATION-select
      if (obj == kODNULL && mode == SELECT_NEXTONE )
        obj = (GtkObject*)objs->Last();
    }
    // last possibility - went around and no other object hit.  Re-check lastSelected!
    if (obj != kODNULL && obj->Hit(hps,ptl,aperture) ) {
      lastSelected = obj;
      selected->AddLast(obj);
      obj->SetSelected(TRUE);
      this->BoundSelected();
      return TRUE;
    }
  }
  lastSelected = (GtkObject*)NULL;
  this->BoundSelected();
  return FALSE;
}

// -------------------------------------------------------------------------
BOOL  rcl_inside( PRECTL rclBounds, PRECTL rcl )
{
   return ( rcl->xLeft   >= rclBounds->xLeft    &&
            rcl->yBottom >= rclBounds->yBottom  &&
            rcl->xRight  <= rclBounds->xRight   &&
            rcl->yTop    <= rclBounds->yTop        );
   }

//-------------------------------------------------------------------------
BOOL GtkPic::Select(HPS hps, PPOINTL ptl0, PPOINTL ptl1, ULONG mode)
{
  if (objs->Count()) {
    ODxOrderedCollectionIterator  iter(objs);
    GtkObject* obj;
    RECTL rcl, rclObj;

    rcl.xLeft = min(ptl0->x,ptl1->x);
    rcl.yBottom = min(ptl0->y,ptl1->y);
    rcl.xRight = max(ptl0->x,ptl1->x);
    rcl.yTop = max(ptl0->y,ptl1->y);

    if (mode != SELECT_TOGGLEONE && selected->Count()) {
      GtkObject* objX = (GtkObject*)selected->First();
      while (objX) {
        objX->SetSelected(FALSE);
        objX = (GtkObject*)selected->After(objX);
      }
      selected->RemoveAll();
    }

    for (obj = (GtkObject*)iter.First();
          iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {
      rclObj = obj->Bounds();
      if ( rcl_inside( &rcl, &rclObj ) ) {
        if (mode == SELECT_TOGGLEONE &&
            selected->Count()        &&
            selected->Contains(obj) ) {
          selected->Remove(obj);
          obj->SetSelected(FALSE);
        } else {
          selected->AddLast(obj);
          obj->SetSelected(TRUE);
        }
      }
    }
    lastSelected = (GtkObject*)NULL;
    this->BoundSelected();
  }

  return (selected->Count() != 0);
}

//---------------------------------------------------------------------------
// Check to see if the point is a) within the selected-objects box, or b)
// ON an unselected or singly-selected object.  If a, lastHit=0 and return TRUE.
// If b, lastHit is the hit object and return TRUE.  If neither, return FALSE.
//
// Calling with ptl=NULL will reset lastHit so that ops HAVE to work on the
// full selection.
//---------------------------------------------------------------------------
BOOL GtkPic::Correlate(HPS hps, PPOINTL ptl, ULONG mode)
{
  #ifdef ODDebug2
    somPrintf("GtkPart: Picture->Correlate()\n");
  #endif

  lastHit = (GtkObject*)NULL;
  if (!ptl) return FALSE;

  if (!mode) {
    // check the selection first
    int i = selected->Count();
    if ( i && PointInRect(ptl, &rclBounds) ) {
//      if (i==1) lastHit = (GtkObject*)selected->First();
      return TRUE;
    }

    if (objs->Count() > selected->Count()) {
      POINTL pt0;
      ULONG aperture;

      pt0 = *ptl;
      GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 1, &pt0 );
      pt0.x += 5;
      GpiConvert( hps, CVTC_DEVICE, CVTC_WORLD, 1, &pt0 );
      aperture = (pt0.x-ptl->x)*(pt0.x-ptl->x);

      GtkObject* obj = (GtkObject*)objs->Last();
      while (obj) {
        if ( !selected->Contains(obj) &&
             obj->Hit( hps, ptl, aperture )            ) {
          lastHit = obj;
          return TRUE;
        }
        obj = (GtkObject*)objs->Before(obj);
      }
    }
  } else {                       // find first embed under pt
    if (objs->Count()) {
      GtkObject* obj = (GtkObject*)objs->Last();
      while (obj) {
        if ( obj->IsEmbed() && obj->Hit( hps, ptl, 0 ) ) {
          lastHit = obj;
          return TRUE;
        }
        obj = (GtkObject*)objs->Before(obj);
      }
    }
  }
  return FALSE;
}

//-------------------------------------------------------------------------
BOOL GtkPic::FindPoint(PPOINTL ptl, ULONG mode)
{
  if (objs->Count()) {
    POINTL ptlHit;

    distance = 5000000;

    GtkObject* obj = (GtkObject*)objs->Last();
    while (obj) {
      obj->FindPoint(ptl,mode, &distance,&ptlHit);
      obj = (GtkObject*)objs->Before(obj);
    }

    if (distance != 5000000) {
      *ptl = ptlHit;
      return TRUE;
    }
  }
  return FALSE;
}

//-------------------------------------------------------------------------
void GtkPic::Invalidate(HWND hwnd, HPS hps, GtkObject* obj)
{
  RECTL  rcl;
  ULONG  n;
  POINTL pt;

  if (!obj && objs->Contains(lastHit)) obj = lastHit;

  if (obj) {                                              // invalidate object
    // main bounds
    rcl = obj->Bounds();

    // add handles, since some may lay outside bounds...
    if (selected->Contains(obj)) {
      n = obj->HandleCount();
      for (int i=0; i<n; i++) {
        pt = obj->QueryHandle(i);
        AddPt( &rcl, &pt );
      }
    }
  } else if (selected->Count()) {                         // invalidate selection
    // main bounds
    rcl = rclBounds;

    // add handles, since some may lay outside bounds...
    GtkObject* obj = (GtkObject*)selected->Last();
    while (obj) {
      n = obj->HandleCount();
      for (int i=0; i<n; i++) {
        pt = obj->QueryHandle(i);
        AddPt( &rcl, &pt );
      }
      obj = (GtkObject*)selected->Before(obj);
    }
  } else return;

  GpiConvert( hps, CVTC_WORLD, CVTC_DEVICE, 2, (PPOINTL)&rcl.xLeft );
  InflateRect( &rcl, (BOUNDS_BORDER+5), (BOUNDS_BORDER+5) );

  // Don't WANT to have to invalidate children, but there seems to be a problem
  // in clipping the std hps obtained during part->Draw handling...
//  WinInvalidateRect( hwnd, &rcl, FALSE );
  WinInvalidateRect( hwnd, &rcl, TRUE );
}

//-------------------------------------------------------------------------
void GtkPic::SetDragging( BOOL fDragging, GtkObject* obj )
{
  if (!obj && objs->Contains(lastHit)) obj = lastHit;

  if (obj) {
    obj->SetDragging(fDragging);
  } else if (selected->Count()) {
    ODxOrderedCollectionIterator  iter(selected);
    for (obj = (GtkObject*)iter.First();
          iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {
      obj->SetDragging(fDragging);
    }
  }
}

//-------------------------------------------------------------------------
void GtkPic::Move(LONG dx, LONG dy, GtkObject* obj)
{
  if (!obj && objs->Contains(lastHit)) obj = lastHit;

  if (obj) {                                              // move object
    if (!obj->IsLocked()) {
      obj->Move(dx,dy);
      if (selected->Contains(obj)) this->BoundSelected();
    }
  } else if (selected->Count()) {                         // move selection
    ODxOrderedCollectionIterator  iter(selected);
    for (obj = (GtkObject*)iter.First();
          iter.IsNotComplete(); obj = (GtkObject*)iter.Next())
      if (!obj->IsLocked()) obj->Move(dx,dy);
    this->BoundSelected();
  }
}

//-------------------------------------------------------------------------
void GtkPic::Copy(GtkObject* obj)
{
  GtkObject* newobj;
  if (!obj && objs->Contains(lastHit)) obj = lastHit;

  if (obj) {                                              // move object
    if (!obj->IsLocked()) {
      newobj = obj->Copy();
      if (newobj) {
        this->AddObject( newobj );                   // this SETs the object id...
        if (lastHit)
          lastHit = newobj;            // (GtkObject*)0;
      }
    }
  } else if (selected->Count()) {                         // move selection
    // create a copy of the selected list
    ODxOrderedCollection* oldobjs = new ODxOrderedCollection;
    ODxOrderedCollectionIterator  iter(selected);
    for (obj = (GtkObject*)iter.First();
          iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {
      if (!obj->IsLocked())
        oldobjs->AddLast( obj );
      obj->SetSelected(FALSE);
    }

    // clear the real selected list
    selected->RemoveAll();

    // make copies and insert into pic AND new selected list
    ODxOrderedCollectionIterator  iter2(oldobjs);
    for (obj = (GtkObject*)iter2.First();
          iter2.IsNotComplete(); obj = (GtkObject*)iter2.Next()) {
      newobj = obj->Copy();
      if (newobj) {
        this->AddObject( newobj );                  // this SETs the object id...
        selected->AddLast( newobj );
        newobj->SetSelected(TRUE);
      }
    }

    delete oldobjs;
  }
}

//-------------------------------------------------------------------------
void GtkPic::Zorder(ULONG op, GtkObject* obj)
{
  if (!obj && objs->Contains(lastHit)) obj = lastHit;

  if (obj) {                                              // move object
    if (objs->Contains(obj) && !obj->IsLocked()) {
      objs->Remove( obj );
      if (op==Z_ORDER_TOP)
        objs->AddLast( obj );
      else
        objs->AddFirst( obj );
      if (selected->Contains(obj)) {
        selected->Remove( obj );
        if (op==Z_ORDER_TOP)
          selected->AddLast( obj );
        else
          selected->AddFirst( obj );
      }
    }
  } else if (selected->Count()) {                         // move selection
    if (op==Z_ORDER_TOP) {
      GtkObject* obj = (GtkObject*)selected->First();
      while (obj) {
        if (!obj->IsLocked()) {
          objs->Remove( obj );
          objs->AddLast( obj );
        }
        obj = (GtkObject*)selected->After(obj);
      }
    } else {
      GtkObject* obj = (GtkObject*)selected->Last();
      while (obj) {
        if (!obj->IsLocked()) {
          objs->Remove( obj );
          objs->AddFirst( obj );
        }
        obj = (GtkObject*)selected->Before(obj);
      }
    }
  }
}

//-------------------------------------------------------------------------
BOOL GtkPic::Delete(GtkObject* obj)
{
  BOOL handled = FALSE;
  if (!obj && objs->Contains(lastHit)) obj = lastHit;

  if (obj) {                                              // delete object
    if (!obj->IsLocked()) {
      if (selected->Contains(obj)) selected->Remove(obj);
      if (objs->Contains(obj)) objs->Remove(obj);
      delete obj;
      handled = TRUE;
    }
  } else if (selected->Count()) {                         // delete selection
    // create a copy of the un-locked, selected list
    ODxOrderedCollection* oldobjs = new ODxOrderedCollection;
    ODxOrderedCollectionIterator  iter(selected);
    for (obj = (GtkObject*)iter.First();
          iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {
      if (!obj->IsLocked())
        oldobjs->AddLast( obj );
    }

    if (oldobjs->Count()) {
      // delete everything in the new list
      ODxOrderedCollectionIterator  iter2(oldobjs);
      for (obj = (GtkObject*)iter2.First();
            iter2.IsNotComplete(); obj = (GtkObject*)iter2.Next()) {
        if (!obj->IsLocked()) {
          selected->Remove(obj);
          objs->Remove(obj);
          delete obj;
        }
      }
      this->BoundSelected();
      handled = TRUE;
    }
    delete oldobjs;
  }
  return handled;
}

//-------------------------------------------------------------------------
BOOL GtkPic::Lock( BOOL fLock, GtkObject* obj )
{
  BOOL handled = FALSE;
  if (!obj && objs->Contains(lastHit)) obj = lastHit;

  if (obj && BOOL_XOR(fLock,obj->IsLocked())) {
    obj->Lock(fLock);
    handled = TRUE;
  } else if (selected->Count()) {
    ODxOrderedCollectionIterator  iter(selected);
    for (obj = (GtkObject*)iter.First();
          iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {
      if (handled || BOOL_XOR(fLock,obj->IsLocked())) {
        obj->Lock(fLock);
        handled = TRUE;
      }
    }
  }
  return handled;
}

//-------------------------------------------------------------------------
void GtkPic::Dissolve( GtkGroup* group )
{
  BOOL selectem = selected->Contains(group);
  ODULong n = group->Count();
  GtkObject** newobjs = group->Objs();
  PMATRIXLF mxGroup = group->QueryTransform();
  for (int i=0; i<n; i++) {
    newobjs[i]->Transform( mxGroup );
    this->AddObject( newobjs[i], TRUE );           // this KEEPs the object id...
    if (selectem)
      selected->AddLast( newobjs[i] );
  }
  objs->Remove(group);
  if (selectem)
    selected->Remove(group);
  group->Clear();
  delete group;
}

//-------------------------------------------------------------------------
BOOL GtkPic::Group( BOOL fGroup, GtkObject* obj )
{
  BOOL handled = FALSE;
  if (!obj && objs->Contains(lastHit)) obj = lastHit;

  if (obj) {
    if (!fGroup && obj->Type()==GOT_GROUP && !obj->IsLocked()) {
      this->Dissolve( (GtkGroup*)obj );
      handled = TRUE;
    }
  } else if (selected->Count()) {
    ODxOrderedCollectionIterator  iter(selected);

    // screen for any locked objects
    for (obj = (GtkObject*)iter.First();
          iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {
      if (obj->IsLocked() && (fGroup || obj->Type()==GOT_GROUP)) return FALSE;
    }

    if (fGroup) {
      GtkGroup* group = new GtkGroup(selected);
      if (group) {
        for (obj = (GtkObject*)iter.First();
              iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {
          objs->Remove(obj);
        }
        selected->RemoveAll();
        this->AddObject( group );
        this->Select( group );
        handled = TRUE;
      }
    } else {
      for (obj = (GtkObject*)iter.First();
            iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {
        if (obj->Type()==GOT_GROUP) {
          this->Dissolve( (GtkGroup*)obj );
          handled = TRUE;
        }
      }
    }
  }
  return handled;
}

//-------------------------------------------------------------------------
void GtkPic::Transform(MATRIXLF *mx, GtkObject* obj)
{
  BOOL handled = FALSE;

  if (!obj && objs->Contains(lastHit)) obj = lastHit;
  else if (selected->Count()==1) obj = (GtkObject*)selected->First();

  // transform them buggers!
  if (obj) {                                              // delete object
    if (!obj->IsLocked()) {
      obj->Transform(mx);
      if (selected->Contains(obj))
        this->BoundSelected();
      handled = TRUE;
    }
  } else if (selected->Count()) {                         // delete selection
    ODxOrderedCollectionIterator  iter(selected);
    for (obj = (GtkObject*)iter.First();
          iter.IsNotComplete(); obj = (GtkObject*)iter.Next())
      if (!obj->IsLocked()) obj->Transform(mx);
    this->BoundSelected();
    handled = TRUE;
  }
//  return handled;
}

//-------------------------------------------------------------------------
BOOL GtkPic::Rotate(FIXED angle, PPOINTL ptlCenter, GtkObject* obj)
{
  MATRIXLF mx;
  POINTL ptl0;
  BOOL handled = FALSE;

  if (!obj && objs->Contains(lastHit)) obj = lastHit;
  else if (selected->Count()==1) obj = (GtkObject*)selected->First();

  if (ptlCenter) {
    ptl0 = *ptlCenter;
  } else {
    RECTL  rcl;
    if (obj)
      obj->FindPoint( NULL, NEAREST_CENTER, NULL, &ptl0 );
    else if (selected->Count()) {
      rcl = rclBounds;
      ptl0.x = (rcl.xLeft + rcl.xRight) / 2;
      ptl0.y = (rcl.yBottom + rcl.yTop) / 2;
    } else return FALSE;
  }

  // calculate rotation matrix
  #if 1
    calc_rotate( angle, &ptl0, &mx );
  #else
    FIXED fSin, fCos;
    mx = mx0;
    fSin = Rsin2( FIX1, angle );
    fCos = Rcos2( FIX1, angle );
    mx.fxM11 = fCos;
    mx.fxM12 = fSin;
    mx.fxM21 =-fSin;
    mx.fxM22 = fCos;
    mx.lM31  = FixLongMult(FIX1-fCos,ptl0.x) + FixLongMult(fSin,ptl0.y);
    mx.lM32  = FixLongMult(FIX1-fCos,ptl0.y) - FixLongMult(fSin,ptl0.x);
  #endif

  // transform them buggers!
  if (obj) {                                              // delete object
    if (!obj->IsLocked()) {
      obj->Transform(&mx);
      if (selected->Contains(obj))
        this->BoundSelected();
      handled = TRUE;
    }
  } else if (selected->Count()) {                         // delete selection
    ODxOrderedCollectionIterator  iter(selected);
    for (obj = (GtkObject*)iter.First();
          iter.IsNotComplete(); obj = (GtkObject*)iter.Next())
      if (!obj->IsLocked()) obj->Transform(&mx);
    this->BoundSelected();
    handled = TRUE;
  }
  return handled;
}

//-------------------------------------------------------------------------
LONG  GtkPic::QueryDefAttribute( ULONG aType )
{
  switch (aType) {
    case LS_COLOR:
      return ls.color;
    case LS_STYLE:
      return ls.style;
    case LS_WIDTH:
      return ls.width;
    case FS_COLOR:
      return fs.color;
    case FS_STYLE:
      return fs.style;
    case SHADOW_STYLE:
      return shadow;
//    case BOX_ROUNDING:
//      break;
    default:
      break;
  }
  return 0;
}

//-------------------------------------------------------------------------
void  GtkPic::SetDefAttribute( ULONG aType, LONG aVal )
{
  switch (aType) {
    case LS_COLOR:
      ls.color = aVal;
      break;
    case LS_STYLE:
      ls.style = aVal;
      break;
    case LS_WIDTH:
      ls.width = aVal;
      break;
    case FS_COLOR:
      fs.color = aVal;
      break;
    case FS_STYLE:
      fs.style = aVal;
      break;
    case SHADOW_STYLE:
      shadow = aVal;
      break;
//    case BOX_ROUNDING:
//      break;
    default:
      break;
  }
}

//-------------------------------------------------------------------------
BOOL GtkPic::SetAttribute(ULONG aType, LONG aVal, GtkObject* obj)
{
  BOOL handled = FALSE;
  if (!obj && objs->Contains(lastHit)) obj = lastHit;

  this->SetDefAttribute( aType, aVal );

  if (obj) {                                              // move object
    if (!obj->IsLocked())
      handled = obj->SetAttribute( aType, aVal);
  } else if (selected->Count()) {                         // move selection
    ODxOrderedCollectionIterator  iter(selected);
    for (obj = (GtkObject*)iter.First();
          iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {
      if (!obj->IsLocked())
        handled = obj->SetAttribute( aType, aVal) || handled;
    }
    if (handled) this->BoundSelected();
  }
  return handled;
}

//-------------------------------------------------------------------------
HMF GtkPic::GetMetafile()
{
  HMF hmf = NULLHANDLE;
#if 0
  switch (picType) {
    case PIC_GTKPIC:
      {
      static DEVOPENSTRUC  dop = {NULL, "DISPLAY", NULL, NULL };
      SIZEL  sizl;
      HDC    hdc = DevOpenDC( (HAB)0, OD_METAFILE, "*", 4L,
                                         (PDEVOPENDATA)&dop, (HDC)NULL );
      if (!hdc) break;

      sizl.cx = rclPic.xRight; sizl.cy = rclPic.yTop;
      HPS hps = GpiCreatePS( (HAB)0, hdc, &sizl, PU_HIENGLISH|GPIA_ASSOC|GPIT_NORMAL );

      // shouldn't need to do ANY scaling...
//      this->Draw( hps, &rclPic );
      GtkRenderPage2( hps, gtk_page(picHandle), 0, 0, NULL );

      GpiAssociate( hps, NULLHANDLE );
      GpiDestroyPS( hps );
      hmf = DevCloseDC(hdc);
      }
      break;
    case PIC_PMHMF:
      hmf = GpiCopyMetaFile( picHandle );
      break;
    default:
      break;
  } /* endswitch */
#endif
  return hmf;
}

//-------------------------------------------------------------------------
ODBoolean GtkPic::QueryBounds( PRECTL prclBounds )
{
  if (prclBounds) *prclBounds=rclPic;
  return TRUE;   //(picHandle != NULLHANDLE);
}

//-------------------------------------------------------------------------
ODBoolean GtkPic::SetBounds( PRECTL prclBounds )
{
  rclPic = *prclBounds;
  return TRUE;   //(picHandle != NULLHANDLE);
}

//-------------------------------------------------------------------------
void GtkPic::EnableMenuActions( ODPopup *fPopup )
{
//    fPopup->CheckMenuItem( ev, kODNULL, VIEW_ASFRAME, !isIcon );
//    fPopup->CheckMenuItem( ev, kODNULL, VIEW_ASICON, isIcon );
//    fPopup->EnableMenuItem( ev, kODNULL, VIEW_ASICON, !fRoot );
//    fPopup->EnableMenuItem( ev, kODNULL, VIEW_FRAMESHAPE, !fRoot );
}

//-------------------------------------------------------------------------
ODBoolean GtkPic::ReadFromStorage(Environment* ev, ODStorageUnit* su, ODDraftKey key,
                                  GraphicsPart* part)
{
  ULONG      ulCount;
  GtkObject* obj;
  BOOL       ok = TRUE, fAdding = (key != 0);
  BOOL       fLoading = !fAdding;
  int        i;

  StorageUnitGetValue(su, ev, sizeof(ulCount), (ODValue)&ulCount);

  for (i=0; i<ulCount; i++) {
    obj = ReadObject( ev, su, key, part );
    if (obj) {
      this->AddObject( obj, fLoading );           // this SETs the object id...

      if (fAdding) {
        selected->AddLast(obj);
        obj->SetSelected(TRUE);
      }
    } else {
      ok = FALSE;
      break;
    }
  }

  ODULong nID, uVO;
  RECTL rcl;

  StorageUnitGetValue(su, ev, sizeof(nextID), (ODValue)&nID);
  StorageUnitGetValue(su, ev, sizeof(rclPic), (ODValue)&rcl);
  StorageUnitGetValue(su, ev, sizeof(ulViewOpts), (ODValue)&uVO);

  if (fLoading) {
    nextID = nID;
    ulViewOpts = uVO;
    rclPic = rcl;
  }

  ulCount = 0;
  StorageUnitGetValue(su, ev, sizeof(ulCount), (ODValue)&ulCount);

  for (i=0; i<ulCount; i++) {
    // read a link source object
    obj = ReadObject( ev, su, key, part );

    if (obj) {

      // if LOADING incorporate link sources
      if (fLoading) {
        linksrcs->AddLast(obj);

      // if cloning, toss 'em?
      } else {
        delete obj;
      }

    } else {
      ok = FALSE;
      break;
    }
  }

  return ok;
}

//-------------------------------------------------------------------------
void GtkPic::PostReadInit( Environment *ev, GraphicsPart* part, ODBoolean cloned )
{
  // full set, or just the selection?!
  ODxOrderedCollectionIterator  iter( cloned ? selected : objs );

  for (GtkObject* obj = (GtkObject*)iter.First();
        iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {

    obj->PostReadInit(ev, part, cloned);

    // earlier SetSel didn't do anything to embeds 'cause the proxies weren't
    // connected yet...
    if (cloned)
      obj->SetSelected(TRUE);
  }

  if (cloned)
    this->BoundSelected();
}

//-------------------------------------------------------------------------
void GtkPic::WriteToStorage( Environment* ev, ODStorageUnit* su, ODDraftKey key,
                             ODBoolean fSelected)
{
  RECTL  rcl;
  ULONG  ulCount;

  if (fSelected && lastHit && objs->Contains(lastHit)) {

    ulCount = 1;
    StorageUnitSetValue(su, ev, sizeof(ulCount), (ODValue)&ulCount);

    lastHit->Write(ev, su, key);

    rcl = lastHit->Bounds();
    InflateRect( &rcl, BOUNDS_BORDER, BOUNDS_BORDER );

  } else {

    ulCount = fSelected ? selected->Count() : objs->Count();
    StorageUnitSetValue(su, ev, sizeof(ulCount), (ODValue)&ulCount);

    if (ulCount) {

      ODxOrderedCollectionIterator  iter(fSelected ? selected : objs);
      for (GtkObject* obj = (GtkObject*)iter.First();
            iter.IsNotComplete(); obj = (GtkObject*)iter.Next())
        obj->Write(ev, su, key);
    }

    rcl = (key==0) ? rclPic : rclBounds;
  }

  StorageUnitSetValue(su, ev, sizeof(nextID), (ODValue)&nextID);
  StorageUnitSetValue(su, ev, sizeof(RECTL), (ODValue)&rcl);
  StorageUnitSetValue(su, ev, sizeof(ulViewOpts), (ODValue)&ulViewOpts);

  // saving ourself?  write out link sources!
  if (!key) {
    ulCount = linksrcs->Count();
    StorageUnitSetValue(su, ev, sizeof(ulCount), (ODValue)&ulCount);

    if (ulCount) {
      ODxOrderedCollectionIterator  iter(linksrcs);
      for (GtkObject* obj = (GtkObject*)iter.First();
            iter.IsNotComplete(); obj = (GtkObject*)iter.Next())
        obj->Write(ev, su, key);
    }
  } else {
    ulCount = 0;
    StorageUnitSetValue(su, ev, sizeof(ulCount), (ODValue)&ulCount);
  }
}

// ---------------------------------------------------------
//      LINKING stuff!
// ---------------------------------------------------------

// ---------------------------------------------------------------------------
//     Return a linkSpec describing the current selection (or obj under cursor
// ---------------------------------------------------------------------------
ODByteArray* GtkPic::LinkSpec()
{
  ODULong*     pData = NULL;
  ODByteArray* bArray;

  if (lastHit && objs->Contains(lastHit)) {

    pData = new ODULong[2];
    pData[0] = 1;
    pData[1] = lastHit->Id();

  } else {

    ODULong ulCount = selected->Count();

    if (ulCount) {
      int i = 1;

      pData = new ODULong[ulCount+1];
      pData[0] = ulCount;

      ODxOrderedCollectionIterator  iter(selected);
      for (GtkObject* obj = (GtkObject*)iter.First();
            iter.IsNotComplete(); obj = (GtkObject*)iter.Next())
        pData[i++] = obj->Id();
    } else {
      return NULL;
    }
  }

  bArray = CreateByteArray(pData, (1+pData[0]) * sizeof(ODULong) );
  return bArray;
}

// ---------------------------------------------------------------------------
//     Create a link SOURCE object and add it to our linkSource set
// ---------------------------------------------------------------------------
ODLinkSource* GtkPic::CreateLinkSource( Environment* ev, GraphicsPart* part,
                                        ODULong* pData )
{
  GtkLinkSource* linkSrc;

  // first look to see if there is an existing link source for this object set
  ODxOrderedCollectionIterator  iter(linksrcs);
  for (linkSrc = (GtkLinkSource*)iter.First();
        iter.IsNotComplete(); linkSrc = (GtkLinkSource*)iter.Next()) {
    if (linkSrc->SpecMatch(pData))
      return linkSrc->LinkSourcePtr();
  }

  // none found, create a new one
  linkSrc = new GtkLinkSource(this);

  // try initializing it
  if (!linkSrc->Init(ev, part, pData)) {
    delete linkSrc;
    return NULL;
  }

  // successful, keep it!
  linkSrc->UpdateIfNecessary(ev, part, objs);
  linkSrc->ShowLink(fLinksVisible);
  linksrcs->AddLast( linkSrc );
  ODLinkSource* linkSource = linkSrc->LinkSourcePtr();

  {
   CHAR szBuffer[256];
   sprintf( szBuffer, "GtkPart link source %d", linksrcs->Count() );
   ODIText*  LScomment = CreateITextCString( 0, 0, szBuffer );
   linkSource->SetDescription( ev, LScomment );
   DisposeIText( LScomment );
  }

  return linkSource;
}

// ---------------------------------------------------------------------------
//     Create a new link target object and set it selected
// ---------------------------------------------------------------------------
GtkLink*  GtkPic::CreateLinkObj( Environment* ev, GraphicsPart* part, ODLink* link )
{
  GtkLink* linkObj = new GtkLink;
  if (linkObj->Init(ev, part, link)) {
    linkObj->ShowLink(fLinksVisible);
    this->AddObject( linkObj );
    this->Select( linkObj );
  } else {
    delete linkObj;
    linkObj = NULL;
  }
  return linkObj;
}

// ---------------------------------------------------------------------------
//     Return the GtkLink obj associated with a particular ODLink*
// ---------------------------------------------------------------------------
GtkLink*  GtkPic::LinkObj( ODLink* link )
{
  ODxOrderedCollectionIterator  iter(objs);
  GtkObject* obj;

  // screen for any locked objects
  for (obj = (GtkObject*)iter.First();
        iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {
    GtkLink* linkObj = obj->LinkObj(link);
    if (linkObj) return linkObj;
  }

  return NULL;
}

// ---------------------------------------------------------------------------
//     Tell all link sources to check update status and update if necessary
// ---------------------------------------------------------------------------
void  GtkPic::UpdateLinks( Environment* ev, GraphicsPart* part, ODLinkSource* linkSource )
{
  ODxOrderedCollectionIterator  iter(linksrcs);
  GtkLinkSource* linkSrc;

  // check 'em all?
  for (linkSrc = (GtkLinkSource*)iter.First();
        iter.IsNotComplete(); linkSrc = (GtkLinkSource*)iter.Next()) {

    if (!linkSource || linkSource == linkSrc->LinkSourcePtr()) {

      // this lets the linkSource update if there have been any changes
      linkSrc->UpdateIfNecessary(ev, part, objs);

      // this triggers a repaint for the linkSource boundary (if visible),
      // necessary 'cause the selection set is orthogonal to the link source sets!
      if (fLinksVisible)
        part->InvalidateObject(ev, linkSrc);
    }
  }
}

// ---------------------------------------------------------------------------
//     Break links and incorporate objects into picture
// ---------------------------------------------------------------------------
void  GtkPic::BreakLinks( Environment* ev, ODLink* link )
{
  GtkObject* obj;
  GtkLink*   linkObj;

  if (link) {

    ODxOrderedCollectionIterator  iter(objs);
    for (obj = (GtkObject*)iter.First();
          iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {

      linkObj = obj->LinkObj(link);

      if (linkObj) {
        this->Dissolve(linkObj);
        return;
      }

    }
  } else {

    ODxOrderedCollectionIterator  iter(selected);
    for (obj = (GtkObject*)iter.First();
          iter.IsNotComplete(); obj = (GtkObject*)iter.Next()) {

      if ( obj->Type()==GOT_LINK ) {
        this->Dissolve((GtkGroup*)obj);
      }
    }
  }

}

// ---------------------------------------------------------
void GtkPic::BreakLinkSource( Environment* ev, ODLinkSource* linkSource )
{
  GtkLinkSource* linkSrc;

  // first look to see if there is an existing link source for this object set
  ODxOrderedCollectionIterator  iter(linksrcs);
  for (linkSrc = (GtkLinkSource*)iter.First();
        iter.IsNotComplete(); linkSrc = (GtkLinkSource*)iter.Next()) {

    if (linkSrc->LinkSourcePtr() == linkSource) {
      linksrcs->Remove(linkSrc);
      linkSource->SetSourcePart(ev, kODNULL);
      delete linkSrc;                                // this does the Release()
    }
  }
}


// ---------------------------------------------------------
//      Undo/Redo stuff!
// ---------------------------------------------------------
ODULong GtkPic::CreateUndoStruct( ODULong uclass, PVOID *pBuf )
{
  GtkObject* obj;
  ODULong pSize = 0;

  switch (uclass) {
    case UNDOCLS_LOCK:
    case UNDOCLS_UNLOCK:
      {
        ODBoolean fLock = (uclass == UNDOCLS_LOCK);
        UndoLockstuff *uls = new UndoLockstuff(uclass);
        if (lastHit && objs->Contains(lastHit)) {
          if (BOOL_XOR(fLock,lastHit->IsLocked())) {
            uls->ids = new ODULong[1];
            uls->ids[0] = lastHit->Id();
            uls->count = 1;
          }
        } else if (selected->Count()) {
          uls->ids = new ODULong[selected->Count()];
          obj = (GtkObject*)selected->First();
          while (obj) {
            if (BOOL_XOR(fLock,obj->IsLocked())) {
              uls->ids[uls->count] = obj->Id();
              uls->count += 1;
            }
            obj = (GtkObject*)selected->After(obj);
          }
        }
        if (uls->count) {
          *pBuf = uls;
          pSize = sizeof(UndoLockstuff);
        } else {
          this->DestroyUndoStruct(uls);
        }
      }
      break;
    case UNDOCLS_ATTRIBS:
      {
        UndoAttributes *uas = (UndoAttributes *)*pBuf;
        LONG oldVal;
        if (lastHit && objs->Contains(lastHit)) {
          if ( !lastHit->IsLocked() &&
               lastHit->QueryAttribute( uas->aAttribType, &oldVal ) &&
               oldVal != uas->newAttrib ) {
            uas->ids = new ODULong[1];
            uas->ids[0] = lastHit->Id();
            uas->oldAttribs = new long[1];
            uas->oldAttribs[0] = oldVal;
            uas->count = 1;
          }
        } else if (selected->Count()) {
          uas->ids = new ODULong[selected->Count()];
          uas->oldAttribs = new long[selected->Count()];
          obj = (GtkObject*)selected->First();
          while (obj) {
            if ( !obj->IsLocked() &&
                 obj->QueryAttribute( uas->aAttribType, &oldVal ) &&
                 oldVal != uas->newAttrib ) {
              uas->ids[uas->count] = obj->Id();
              uas->oldAttribs[uas->count] = oldVal;
              uas->count += 1;
            }
            obj = (GtkObject*)selected->After(obj);
          }
        }
        if (uas->count) {
          *pBuf = uas;
          pSize = sizeof(UndoAttributes);
        } else {
          this->DestroyUndoStruct(uas);
        }
      }
      break;
    case UNDOCLS_MOVE:
      {
        UndoMove* um = (UndoMove *)*pBuf;
        um->count = 0;
        if (lastHit && objs->Contains(lastHit)) {
          if ( !lastHit->IsLocked() ) {
            um->ids = new ODULong[1];
            um->ids[0] = lastHit->Id();
            um->count = 1;
          }
        } else if (selected->Count()) {
          um->ids = new ODULong[selected->Count()];
          obj = (GtkObject*)selected->First();
          while (obj) {
            if ( !obj->IsLocked() ) {
              um->ids[um->count] = obj->Id();
              um->count += 1;
            }
            obj = (GtkObject*)selected->After(obj);
          }
        }
        if (um->count) {
          pSize = sizeof(UndoMove);
        } else {
//          this->DestroyUndoStruct(um);
        }
      }
      break;
    case UNDOCLS_CREATE:
      {
        ULONG n = (lastHit && objs->Contains(lastHit)) ? 1 : this->SelectedCount();
        if (n) {
          UndoCreate* uc = (UndoCreate *)*pBuf;
          uc->ids = new ODULong[n];
          uc->objs = new GtkObject*[n];
          uc->undone = new ODBoolean;
          *uc->undone = kODFalse;

          if (lastHit && objs->Contains(lastHit)) {
            uc->count = 1;
            uc->ids[0] = lastHit->Id();
          } else {
            uc->count = 0;

            obj = (GtkObject*)selected->First();
            while (obj) {
              uc->ids[uc->count] = obj->Id();
              uc->count += 1;
              obj = (GtkObject*)selected->After(obj);
            }
          }

          pSize = sizeof(UndoCreate);
        }
      }
      break;
    default:
      break;
  }

  return pSize;
}

// ---------------------------------------------------------
void  GtkPic::DestroyUndoStruct( PVOID pBuf )
{
  UndoBase*  undoRec = (UndoBase*)pBuf;

  switch (undoRec->uclass) {
    case UNDOCLS_LOCK:
    case UNDOCLS_UNLOCK:
      {
        UndoLockstuff *uls = (UndoLockstuff*)undoRec;
        delete uls->ids;
        delete uls;
      }
      break;
    case UNDOCLS_ATTRIBS:
      {
        UndoAttributes *uas = (UndoAttributes*)undoRec;
        delete uas->ids;
        delete uas->oldAttribs;
        delete uas;
      }
      break;
    case UNDOCLS_MOVE:
      {
        UndoMove *um = (UndoMove*)undoRec;
        delete um->ids;
        delete um;
      }
      break;
    case UNDOCLS_CREATE:
      {
        UndoCreate *uc = (UndoCreate*)undoRec;

        // does the struct "own" the objects and need to delete them?
        if (*uc->undone) {
          for (int i=0; i<uc->count; i++)
            delete uc->objs[i];
        }

        delete uc->ids;
        delete uc->objs;
        delete uc->undone;
        delete uc;
      }
      break;
    default:
      break;
  }
}

// ---------------------------------------------------------
void  GtkPic::DoUndo( PVOID pBuf )
{
  UndoBase*  undoRec = (UndoBase*)pBuf;

  switch (undoRec->uclass) {
    case UNDOCLS_LOCK:
    case UNDOCLS_UNLOCK:
      {
        UndoLockstuff *uls = (UndoLockstuff*)undoRec;
        ODBoolean fLock = undoRec->uclass == UNDOCLS_UNLOCK;
        for (int i=0; i<uls->count; i++) {
          this->Object(uls->ids[i])->Lock(fLock);
        }
      }
      break;
    case UNDOCLS_ATTRIBS:
      {
        UndoAttributes *uas = (UndoAttributes*)undoRec;
        for (int i=0; i<uas->count; i++) {
          this->Object(uas->ids[i])->SetAttribute( uas->aAttribType,
                                                   uas->oldAttribs[i]);
        }
      }
      break;
    case UNDOCLS_MOVE:
      {
        UndoMove *um = (UndoMove*)undoRec;
        for (int i=0; i<um->count; i++) {
          this->Object(um->ids[i])->Move( -um->dx, -um->dy );
        }
      }
      break;
    case UNDOCLS_CREATE:
      {
        UndoCreate *uc = (UndoCreate*)undoRec;
        if (!*uc->undone) {
          if (this->SelectedCount()) this->Select();

          for (int i=0; i<uc->count; i++) {
            uc->objs[i] = this->Object(uc->ids[i]);
            objs->Remove( uc->objs[i] );
          }

          *uc->undone = kODTrue;
        }
      }
      break;
    default:
      break;
  }
}

// ---------------------------------------------------------
void  GtkPic::DoRedo( PVOID pBuf )
{
  UndoBase*  undoRec = (UndoBase*)pBuf;

  switch (undoRec->uclass) {
    case UNDOCLS_LOCK:
    case UNDOCLS_UNLOCK:
      {
        UndoLockstuff *uls = (UndoLockstuff*)undoRec;
        ODBoolean fLock = undoRec->uclass == UNDOCLS_LOCK;
        for (int i=0; i<uls->count; i++) {
          this->Object(uls->ids[i])->Lock(fLock);
        }
      }
      break;
    case UNDOCLS_ATTRIBS:
      {
        UndoAttributes *uas = (UndoAttributes*)undoRec;
        for (int i=0; i<uas->count; i++) {
          this->Object(uas->ids[i])->SetAttribute( uas->aAttribType,
                                                   uas->newAttrib );
        }
      }
      break;
    case UNDOCLS_MOVE:
      {
        UndoMove *um = (UndoMove*)undoRec;
        for (int i=0; i<um->count; i++) {
          this->Object(um->ids[i])->Move( um->dx, um->dy );
        }
      }
      break;
    case UNDOCLS_CREATE:
      {
        UndoCreate *uc = (UndoCreate*)undoRec;

        if (*uc->undone) {
          if (this->SelectedCount()) this->Select();

          for (int i=0; i<uc->count; i++) {
            objs->AddLast( uc->objs[i] );
          }

          *uc->undone = kODFalse;
        }
      }
      break;
    default:
      break;
  }
}

