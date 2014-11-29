/*
   File:    GtkPic.h

   Contains:   definition for class GtkPic

   Written by: Alan C. Warren

   Copyright:  © 1994 by IBM Corp. all rights reserved.

   Change History (most recent first):

      <01>    5/26/94 acw   Initial split off from GtkPart class
*/

#ifndef _GTKPIC_
#define _GTKPIC_

#include  <popup.xh>
#include  "GtkObj.h"

#define VO_OPAQUE     0x0001L
#define VO_STRETCH    0x0002L

#define  RCL_WIDTH(rcl)  ((rcl)->xRight - (rcl)->xLeft + 1)
#define  RCL_HEIGHT(rcl) ((rcl)->yTop - (rcl)->yBottom + 1)

//==============================================================================
// Classes defined in this interface
//==============================================================================

class GtkPic;

//==============================================================================
// Classes used by this interface
//==============================================================================

//class GraphicsPart;
class GtkObject;

//==============================================================================
// GtkPic
//==============================================================================

class GtkPic
{
public:

  // -constructor/destructor-
  GtkPic();
  virtual ~GtkPic();

  ODBoolean ReadFromStorage( Environment *ev, ODStorageUnit *su, ODDraftKey key,
                             GraphicsPart* part );
  void      PostReadInit( Environment *ev, GraphicsPart* part, ODBoolean cloned );
  void      WriteToStorage( Environment *ev, ODStorageUnit *su, ODDraftKey key,
                            ODBoolean fSelected = FALSE );

  ODByteArray*  LinkSpec();
  ODLinkSource* CreateLinkSource( Environment* ev, GraphicsPart* part, ODULong* pData );
  GtkLink*      CreateLinkObj( Environment* ev, GraphicsPart* part, ODLink* link );
  GtkLink*      LinkObj( ODLink* link );
  void          UpdateLinks( Environment* ev, GraphicsPart* part,
                             ODLinkSource* linkSource = 0 );
  void          BreakLinks( Environment* ev, ODLink* link = 0 );
  void          BreakLinkSource( Environment* ev, ODLinkSource* linkSource );

  void  Draw( HPS hps, PRECTL rclDest, ODFacet* facet );
  void  DrawShadows( HPS hps, GtkObject* obj = 0 );
  void  DrawMarks( HPS hps, BOOL fAltMarked = FALSE );
  void  ShowLinks( BOOL fShow );

  void  Invalidate( HWND hwnd, HPS hps, GtkObject* obj = 0 );
  void  ClipEmbeds( HPS hps, ODFacet* facet, PRECTL prcl = 0 );

  ULONG ObjectCount() { return objs->Count(); }
  ULONG SelectedCount() { return selected->Count(); }
  PRECTL SelectedBounds() { return &rclBounds; }
  GtkObject* Object( ULONG id );
  BOOL  IsSelected( GtkObject* obj ) { return selected->Contains(obj); }
  VOID  ClassifySelection( ULONG* objFlags, ULONG* stateFlags );

  BOOL  HitHandle( HPS hps, PPOINTL ptl );
  BOOL  HitSizers( HPS hps, PPOINTL ptl );
  GtkObject* HitObject( PLONG handle = 0 );
  void  SelectAll();
  void  Select( GtkObject* obj = 0, ULONG mode = 0 );
  void  Select( ULONG id, ULONG mode = 0 ) { this->Select(this->Object(id),mode); }
  BOOL  Select( HPS hps, PPOINTL ptl, ULONG mode = 0 );
  BOOL  Select( HPS hps, PPOINTL ptl0, PPOINTL ptl1, ULONG mode = 0 );
  void  BoundSelected();
  void  SetDragging( BOOL fDragging, GtkObject* obj = 0 );

  BOOL  Correlate( HPS hps, PPOINTL ptl = 0, ULONG mode = 0 );
  BOOL  FindPoint(PPOINTL ptl, ULONG mode );

  void  AddObject( GtkObject* newObject, BOOL keepID = FALSE );
  void  SetHandle( POINTL pt) { if (lastHit) lastHit->SetHandle(handleIndx,pt); }
  void  Move( LONG dx, LONG dy, GtkObject* obj = 0 );
  void  Transform( MATRIXLF* mx, GtkObject* obj = 0 );
  BOOL  Rotate( FIXED angle, PPOINTL ptlCenter = 0, GtkObject* obj = 0 );
  void  Zorder( ULONG op, GtkObject* obj = 0 );
  void  Copy( GtkObject* obj = 0 );
  BOOL  Delete( GtkObject* obj = 0 );
  BOOL  Lock( BOOL fLock, GtkObject* obj = 0 );
  BOOL  Group( BOOL fGroup, GtkObject* obj = 0 );
  void  Dissolve( GtkGroup* group );
  BOOL  SetAttribute( ULONG aType, LONG aVal, GtkObject* obj = 0 );

  ODULong CreateUndoStruct( ODULong uclass, PVOID *pBuf );
  void  DestroyUndoStruct( PVOID pBuf );
  void  DoUndo( PVOID pBuf );
  void  DoRedo( PVOID pBuf );

  LONG  QueryDefAttribute( ULONG aType );
  void  SetDefAttribute( ULONG aType, LONG aVal );

  HMF GetMetafile();
  ODBoolean QueryBounds( PRECTL prclBounds );
  ODBoolean SetBounds( PRECTL prclBounds );
  void EnableMenuActions( ODPopup *fPopup );

protected:

  RECTL        rclPic;
  ULONG        ulViewOpts;
  BOOL         fClipEmbeds;
  BOOL         fLinksVisible;

  ULONG        nextID;                         // counter for unique id's

  ODxOrderedCollection* objs;                   // content object set
  ODxOrderedCollection* selected;               // selected objects
  ODxOrderedCollection* linksrcs;               // link SOURCE objects

  LINESTUFF    ls;
  FILLSTUFF    fs;
  long         shadow;

  GtkObject*   lastHit;                        // last selected object
  GtkObject*   lastSelected;                   // last selected object
  LONG         handleIndx;
  RECTL        rclBounds;                      // of selected objects
  POINTL       ptlSizers[8];                   // sizing handles, pre-calc'd

  POINTL       ptlNear;
  GtkObject*   nearest;
  ULONG        distance;
};

#endif  // _GTKPIC_
