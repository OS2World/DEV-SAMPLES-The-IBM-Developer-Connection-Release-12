
#ifndef _GTKOBJ_
#define _GTKOBJ_

#include "util.h"

#define GOT_LINE       0x00000001
#define GOT_BOX        0x00000002
#define GOT_ELLIPSE    0x00000004
#define GOT_POLYLINE   0x00000008
#define GOT_ARC        0x00000010
#define GOT_EMBED      0x00000020
#define GOT_GROUP      0x00000040
#define GOT_LINKSRC    0x00000080
#define GOT_LINK       0x00000100

#define FILL_OBJECTS    (GOT_BOX|GOT_ELLIPSE|GOT_POLYLINE|GOT_ARC|GOT_GROUP)
#define LINE_OBJECTS    (FILL_OBJECTS|GOT_LINE)
#define SHADOW_OBJECTS  (LINE_OBJECTS|GOT_EMBED)

#define INCLUDES_TYPE(flags,aType) ( (flags & (1<<(aType-1))) != 0 )

#define GOS_ANYLOCKED        0x00000001
#define GOS_ANYUNLOCKED      0x00000002
#define GOS_ANYTRANSPARENT   0x00000004
#define GOS_ANYOPAQUE        0x00000008
#define GOS_ANYLINKSOURCE    0x00000010

#define DRAWMODE_NORMAL  0
#define DRAWMODE_PATH    1

class GtkLink;

// --------------------------------------------------------------------------
GtkObject* ReadObject( Environment *ev, ODStorageUnit* su, ODDraftKey key,
                       GraphicsPart* part );

// --------------------------------------------------------------------------
typedef struct {
        long   color;
        long   style;
        long   width;
        } LINESTUFF, *PLINESTUFF;

typedef struct {
        long   color;
        long   style;
        } FILLSTUFF, *PFILLSTUFF;

// --------------------------------------------------------------------------
class GtkObject
{
public:

   GtkObject() { id = 0; changeId = 0; locked = FALSE; }
   virtual ~GtkObject() { return; }

   ODULong Id() { return this->id; }
   virtual GtkObject* Obj(ODULong Id) { return this->id==Id ? this : NULL; }
   virtual GtkLink* LinkObj(ODLink* aLink) { return NULL; }
   virtual void  SetId( ODULong newId ) { id = newId; }
   ODULong Type() { return this->type; }
   RECTL Bounds() { return this->rclBounds; }
   BOOL  IsEmbed() { return type==GOT_EMBED; }
   virtual void  SetSelected( BOOL fSelected ) { return; }
   virtual void  SetDragging( BOOL fDragging ) { return; }
   virtual void  Lock( BOOL fLock ) { locked = fLock; }
   virtual void  ShowLink( BOOL fShow ) { return; }
   BOOL  IsLocked() { return locked; }

   virtual void Draw( HPS hps, ODFacet* facet ) { return; }
   virtual void DrawShadow( HPS hps ) { return; }
   virtual void SetRegion( HRGN rgn, ODFacet* facet ) { return; }

   virtual BOOL Read( Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part ) = 0;
   virtual void PostReadInit( Environment *ev, GraphicsPart* part, BOOL cloned ) { return; }
   virtual BOOL Write( Environment *ev, ODStorageUnit* su, ODDraftKey key ) = 0;
   virtual GtkObject* Copy(BOOL shallow = FALSE) { return (GtkObject*)NULL; }
   virtual void CopyFrom( GtkObject* original, BOOL shallow = FALSE) { return; }

   virtual ODULong  HandleCount() { return 0; }
   virtual POINTL QueryHandle( ODULong indx ) { POINTL ptl = {0,0}; return ptl; }
   virtual void   SetHandle( ODULong indx, POINTL pt, BOOL commit = FALSE ) { return; }

   virtual BOOL   Hit( HPS hps, PPOINTL ptl, ODULong dx) { return FALSE; }
   virtual HRGN   Region(HPS hps, ODFacet* facet ) { return 0; }
   virtual BOOL   FindPoint( PPOINTL ptl, ODULong mode,
                             PULONG distance, PPOINTL ptlHit ) { return FALSE; }

   virtual void   Move( long dx, long dy );
   virtual void   Transform( MATRIXLF* mx );
   virtual PMATRIXLF QueryTransform() { return 0; }

   virtual BOOL SetAttribute(ULONG aType, long aVal);
   virtual BOOL QueryAttribute(ULONG aType, long *aVal);
   virtual PLINESTUFF QueryLinestuff() { return 0; }
   virtual PFILLSTUFF QueryFillstuff() { return 0; }
   virtual LONG QueryShadow() { return shadow; }
   virtual PLONG QueryClosing() { return 0; }

   virtual void SetTranslucent( BOOL translucent ) { return; }
   virtual BOOL IsTranslucent() { return FALSE; }

   virtual void UpdateBounds() = 0;

protected:

   friend GtkObject* ReadObject(Environment *ev, ODStorageUnit* su, GraphicsPart* part);

   ODULong  id;
   ODULong  type;
   ODULong  changeId;
   BOOL     locked;
   RECTL    rclBounds;

   long     shadow;
};

typedef GtkObject* PGTKOBJECT;

// --------------------------------------------------------------------------
class GtkLine : public GtkObject
{
public:

   GtkLine(POINTL ptl0, POINTL ptl1);
   ~GtkLine() { if (this->ptls) delete this->ptls; }

   void   Draw( HPS hps, ODFacet* facet );
   void   DrawShadow( HPS hps );
   BOOL   Read( Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part );
   BOOL   Write( Environment *ev, ODStorageUnit* su, ODDraftKey key );

   BOOL   Hit(HPS hps, PPOINTL ptl, ODULong dx);
   BOOL   FindPoint( PPOINTL ptl, ODULong mode, PULONG distance, PPOINTL ptlHit );
   ODULong  HandleCount() { return 2; }
   POINTL QueryHandle( ODULong indx ) { return (indx==0 ? ptls[0] : ptls[1]); }
   void   SetHandle( ODULong indx, POINTL pt, BOOL commit = FALSE );

   GtkObject* Copy(BOOL shallow = FALSE);
   void   Move( long dx, long dy );
   void   Transform( MATRIXLF* mx );
   void   CopyFrom( GtkObject* original, BOOL shallow = FALSE );
   PLINESTUFF QueryLinestuff() { return &ls; }
   void   UpdateBounds();

protected:

   PPOINTL   ptls;
   LINESTUFF ls;

}; /* end GtkLine */

// --------------------------------------------------------------------------
class GtkPolyline : public GtkObject
{
public:

   GtkPolyline(ULONG npts = 0, PPOINTL pts = 0);
   ~GtkPolyline() { if (ptls) delete ptls; }

   void   Draw( HPS hps, ODFacet* facet );
   void   DrawShadow( HPS hps );
   BOOL   Read( Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part );
   BOOL   Write( Environment *ev, ODStorageUnit* su, ODDraftKey key );

   BOOL   Hit(HPS hps, PPOINTL ptl, ODULong dx);
   BOOL   FindPoint( PPOINTL ptl, ODULong mode, PULONG distance, PPOINTL ptlHit );
   ODULong  HandleCount() { return n; }
   POINTL QueryHandle( ODULong indx ) { return ptls[indx]; }
   void   SetHandle( ODULong indx, POINTL pt, BOOL commit = FALSE );

   GtkObject* Copy(BOOL shallow = FALSE);
   void   Move( long dx, long dy );
   void   Transform( MATRIXLF* mx );
   void   CopyFrom( GtkObject* original, BOOL shallow = FALSE );
   PLINESTUFF QueryLinestuff() { return &ls; }
   PFILLSTUFF QueryFillstuff() { return &fs; }
   void   SetPts(ULONG npts, PPOINTL pts );
   BOOL   SetAttribute(ULONG aType, long aVal);
   void   UpdateBounds();

protected:

   BOOL fSolid;

   ODULong     n;
   PPOINTL   ptls;
   LINESTUFF ls;
   FILLSTUFF fs;

}; /* end GtkPolyline */


// --------------------------------------------------------------------------
class GtkEmbed : public GtkObject
{
public:

   GtkEmbed( GraphicsPart* gtkpart = 0, PRECTL rcl = 0, POINTL* offset = 0);
   ~GtkEmbed();

   void   Draw( HPS hps, ODFacet* facet );
   void   DrawShadow( HPS hps );
   BOOL   Read( Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part );
   void   PostReadInit( Environment *ev, GraphicsPart* part, BOOL cloned );
   BOOL   Write( Environment *ev, ODStorageUnit* su, ODDraftKey key );

   void   SetId( ODULong newId );
   ODULong  HandleCount() { return 2; }

   POINTL QueryHandle( ODULong indx );
   void   SetHandle( ODULong indx, POINTL pt, BOOL commit = FALSE );

   BOOL   Hit( HPS hps, PPOINTL ptl, ODULong dx);
   HRGN   Region( HPS hps, ODFacet* facet )
            { return containingPart!=0 && !fTranslucent ?
              containingPart->QueryEmbedRgn( 0, id, facet ) : NULLHANDLE; }
   void   SetRegion( HRGN rgn, ODFacet* facet )
            { if (containingPart) containingPart->SetEmbedRgn( 0, id, facet, rgn ); }
   void   Lock( BOOL fLock );
   void   SetSelected( BOOL fSelected )
            { if (containingPart) containingPart->SelectEmbed( 0, id, fSelected ); }
   void   SetDragging( BOOL fDragging );

   PLINESTUFF QueryLinestuff() { return &ls; }
   PFILLSTUFF QueryFillstuff() { return &fs; }
   PMATRIXLF  QueryTransform() { return &mx; }
   GtkObject* Copy(BOOL shallow = FALSE);

   BOOL   SetAttribute(ULONG aType, long aVal);
   void   SetTranslucent( BOOL translucent ) { fTranslucent = translucent; }
   BOOL   IsTranslucent() { return fTranslucent; }

   void   Move( long dx, long dy );
   void   Transform( MATRIXLF* mx );

   void   CopyFrom( GtkObject* original, BOOL shallow = FALSE );

   void   UpdateBounds();

protected:

   PPOINTL Pts();
   void    UpdateTranslucence();

   POINTL     ptl0;                                      // FRAME LL corner
   POINTL     ptl1;                                      // FRAME UR corner
   MATRIXLF   mx;                                        // transform

   BOOL       fTranslucent;
   LINESTUFF  ls;                                        // key bgnd paint on this!
   FILLSTUFF  fs;

   // NON-persistent stuff
   GraphicsPart*  containingPart;
   ODID           frameID;
   ODMatrix       odmx;

}; /* end GtkEmbed */


// --------------------------------------------------------------------------
class GtkBox : public GtkObject
{
public:

   GtkBox(POINTL ptlLL, POINTL ptlUR, long rx = 0);

   void   Draw( HPS hps, ODFacet* facet );
   void   DrawShadow( HPS hps );
   BOOL   Read( Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part );
   BOOL   Write( Environment *ev, ODStorageUnit* su, ODDraftKey key );

   BOOL   Hit(HPS hps, PPOINTL ptl, ODULong dx);
   BOOL   FindPoint( PPOINTL ptl, ODULong mode, PULONG distance, PPOINTL ptlHit );
   ODULong  HandleCount() { return 2; }
   POINTL QueryHandle( ODULong indx );
   void   SetHandle( ODULong indx, POINTL pt, BOOL commit = FALSE );

   PMATRIXLF QueryTransform() { return &mx; }
   GtkObject* Copy(BOOL shallow = FALSE);

   void   CopyFrom( GtkObject* original, BOOL shallow = FALSE );
   PLINESTUFF QueryLinestuff() { return &ls; }
   PFILLSTUFF QueryFillstuff() { return &fs; }
   void   UpdateBounds();

   VOID   SetRounding(LONG rx=0, long ry=0) { rounding.cx=rx; rounding.cy=ry; }

protected:

   PPOINTL Pts();

   POINTL    ptl0;                                      // LL corner
   POINTL    ptl1;                                      // UR corner
   MATRIXLF  mx;                                        // transform
   SIZEL     rounding;
   LINESTUFF ls;
   FILLSTUFF fs;

}; /* end GtkBox */

// --------------------------------------------------------------------------
class GtkEllipse : public GtkObject
{
public:

   GtkEllipse(POINTL ptlLL, POINTL ptlUR);

   void   Draw( HPS hps, ODFacet* facet );
   void   DrawShadow( HPS hps );
   BOOL   Read( Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part );
   BOOL   Write( Environment *ev, ODStorageUnit* su, ODDraftKey key );

   BOOL   Hit(HPS hps, PPOINTL ptl, ODULong dx);
   BOOL   FindPoint( PPOINTL ptl, ODULong mode, PULONG distance, PPOINTL ptlHit );
   ODULong  HandleCount() { return 2; }
   POINTL QueryHandle( ODULong indx );
   void   SetHandle( ODULong indx, POINTL pt, BOOL commit = FALSE );

   PMATRIXLF QueryTransform() { return &mx; }
   GtkObject* Copy(BOOL shallow = FALSE);

   void   CopyFrom( GtkObject* original, BOOL shallow = FALSE );
   PLINESTUFF QueryLinestuff() { return &ls; }
   PFILLSTUFF QueryFillstuff() { return &fs; }
   void   UpdateBounds();

protected:

   PPOINTL Pts( PULONG count );

   POINTL    ptl0;                                      // center
   POINTL    ptl1;                                      // corner
   MATRIXLF  mx;                                        // transform
   LINESTUFF ls;
   FILLSTUFF fs;

}; /* end GtkEllipse */

// --------------------------------------------------------------------------
class GtkArc : public GtkObject
{
public:

   GtkArc(POINTL ptl0, long radius, FIXED iAngle, FIXED fAngle);

   void   Draw( HPS hps, ODFacet* facet );
   void   DrawShadow( HPS hps );
   BOOL   Read( Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part );
   BOOL   Write( Environment *ev, ODStorageUnit* su, ODDraftKey key );

   BOOL   Hit(HPS hps, PPOINTL ptl, ODULong dx);
   BOOL   FindPoint( PPOINTL ptl, ODULong mode, PULONG distance, PPOINTL ptlHit );
   ODULong  HandleCount() { return 3; }
   POINTL QueryHandle( ODULong indx );
   void   SetHandle( ODULong indx, POINTL pt, BOOL commit = FALSE );

   PMATRIXLF QueryTransform() { return &mx; }
   GtkObject* Copy(BOOL shallow = FALSE);

   void   CopyFrom( GtkObject* original, BOOL shallow = FALSE );
   PLINESTUFF QueryLinestuff() { return &ls; }
   PFILLSTUFF QueryFillstuff() { return &fs; }
   PLONG  QueryClosing() { return &closing; }
   void   UpdateBounds();

protected:

   PPOINTL Pts( PULONG count );

   FIXED     iAngle, fAngle;                          // end angle
   long      radius;
   POINTL    ptls[3];                                 // center, start, end pts
   MATRIXLF  mx;                                      // transform

   LINESTUFF ls;
   FILLSTUFF fs;
   long      closing;

}; /* end GtkArc */


// --------------------------------------------------------------------------
class GtkGroup : public GtkObject
{
public:

   GtkGroup(ODxOrderedCollection* objs = 0);
   ~GtkGroup();

   GtkObject* Obj(ODULong Id);
   GtkLink*   LinkObj(ODLink* aLink);
   void       ShowLink( BOOL fShow );

   void   Draw( HPS hps, ODFacet* facet );
   void   DrawShadow( HPS hps );
   BOOL   Read( Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part );
   BOOL   Write( Environment *ev, ODStorageUnit* su, ODDraftKey key );

   ODULong  HandleCount() { return 2; }

   POINTL QueryHandle( ODULong indx );
   void   SetHandle( ODULong indx, POINTL pt, BOOL commit = FALSE );

   BOOL   Hit( HPS hps, PPOINTL ptl, ODULong dx);
   BOOL   FindPoint( PPOINTL ptl, ODULong mode, PULONG distance, PPOINTL ptlHit );

   PMATRIXLF QueryTransform() { return &mx; }
   GtkObject* Copy(BOOL shallow = FALSE);
   void   Move( long dx, long dy );
   void   Transform( MATRIXLF* mx );
   BOOL   SetAttribute(ULONG aType, long aVal);

   void   CopyFrom( GtkObject* original, BOOL shallow = FALSE );

   ODULong  Count() { return count; }
   GtkObject** Objs() { return objs; }
   void   Clear();
   void   Delete();

   void   UpdateBounds();

protected:

   MATRIXLF    mx;                                        // transform

   ODULong     count;
   GtkObject** objs;

}; /* end GtkGroup */


// --------------------------------------------------------------------------
class GtkLinkSource : public GtkObject
{
public:

   GtkLinkSource(GtkPic* pic);
   ~GtkLinkSource();

   BOOL   Init( Environment* ev, GraphicsPart* part, ODULong* pData );
   void   Export( Environment* ev, GraphicsPart* part );

   void   Draw( HPS hps, ODFacet* facet );
   void   ShowLink( BOOL fShow ) { fHighlighted = fShow; }
   ODLinkSource* LinkSourcePtr() { return linkSource; }

   BOOL   Read( Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part );
   void   PostReadInit( Environment *ev, GraphicsPart* part, BOOL cloned ) { return; }
   BOOL   Write( Environment *ev, ODStorageUnit* su, ODDraftKey key );

   void   UpdateBounds();
   void   UpdateIfNecessary( Environment* ev, GraphicsPart* part,
                             ODxOrderedCollection* baseObjs );
   BOOL   SpecMatch( ODULong* pData );

protected:

  BOOL          fHighlighted;
  ODLinkSource* linkSource;
  GtkPic*       picture;
  ODULong       count;
  ODULong*      ids;

}; /* end GtkLinkSource */


// --------------------------------------------------------------------------
class GtkLink : public GtkGroup
{
public:

   GtkLink() { type = GOT_LINK; fHighlighted = kODFalse; _link = kODNULL; }
   ~GtkLink() { _link->Release(SOM_ENVIRONMENT); }

   GtkObject* Obj(ODULong Id) { return this->id==Id ? this : NULL; }

   ODUpdateID ChangeID() { return _link ? _linkInfo.change : 0; }
   void       ShowLink( BOOL fShow ) { fHighlighted = fShow; }
   GtkLink*   LinkObj(ODLink* link) { return link==_link ? this : NULL; }

   BOOL   Init( Environment* ev, GraphicsPart* part, ODLink* link );
   void   Draw( HPS hps, ODFacet* facet );

   BOOL   Read( Environment *ev, ODStorageUnit* su, ODDraftKey key, GraphicsPart* part );
   void   PostReadInit( Environment *ev, GraphicsPart* part, BOOL cloned );
   BOOL   Write( Environment *ev, ODStorageUnit* su, ODDraftKey key );

   void   Move( long dx, long dy );
   void   Transform( MATRIXLF* mx );

   void   Update(Environment* ev, GraphicsPart* part);

protected:

  ODID            _linkID;
  BOOL            fHighlighted;
  ODStorageUnit*  _readSU;

  ODLink*     _link;
  ODLinkInfo  _linkInfo;


}; /* end GtkLinkTarget */

#endif

