#include "gtkundo.h"

#define  DLL_NAME              "gtkpart"
#define  HELPFILE_NAME         "GtkPart.hlp"


#define  kKindGraphicsPart     "+//ISO 9070/ANSI::113722::US::CI LABS::IBM:Kind:2DGraphicsPart"

//#define  OLDMOUSEOFFSET
//#ifdef OLDMOUSEOFFSET
  #define  kPropMouseDownOffset  "+//ISO 9070/ANSI::113722::US::CI LABS::IBM:ContainerPart:Property:MouseDownOffset"
//#endif

#define  kGtkPOINTL            "GtkPart:Type:POINTL"

#define  THUMBSIZE  64

#define  BGM_COLOR         0
#define  BGM_TILE          1
#define  BGM_FILL          2
#define  BGM_FIT           3
#define  BGM_TRANSPARENT   4

#define  SHADOW_DX  5
#define  SHADOW_DY  8

extern MATRIXLF mx0;

class HpsFocus {
  public:
    HpsFocus( HPS hps, MATRIXLF* mx) {
      _hps = hps;
      GpiQueryModelTransformMatrix( hps, 9, &_oldmx );
      GpiSetModelTransformMatrix ( hps, 9, mx, TRANSFORM_PREEMPT );
      }
    HpsFocus( HPS hps ) {
      MATRIXLF mx = {65536,0,0,0,65536,0,SHADOW_DX,-SHADOW_DY,1};
      _hps = hps;
      GpiQueryModelTransformMatrix( hps, 9, &_oldmx );
      GpiSetModelTransformMatrix ( hps, 9, &mx, TRANSFORM_PREEMPT );
      }
    ~HpsFocus() {
      GpiSetModelTransformMatrix( _hps, 9, &_oldmx, TRANSFORM_REPLACE );
      }
  protected:
    HPS       _hps;
    MATRIXLF  _oldmx;
};

#define FIXED2LONG(f) (((f) + 0x8000) >> 16)


#define WM_QUERY_COLOR   0x00010001
#define WM_SET_COLOR     0x00010002

#define BOUNDS_BORDER 7

#define RSM_SCALE     1
#define RSM_STRETCH   2
#define RSM_AREA      3
#define RSM_BESTFIT   4

#define PUF_CLEARUNDO     1
#define PUF_RESETFSM      2
#define PUF_REPAINT       4
#define PUF_UPDATELINKS   8
#define PUF_CLIPEMBEDS   16
#define PUF_REPAINTALL   32             // NEWSTUFF

#define FLAGS_SHIFT    (BOOL)((flags&FSAF_SHIFT)!=0)
#define FLAGS_CONTROL  (BOOL)((flags&FSAF_CONTROL)!=0)
#define FLAGS_ALT      (BOOL)((flags&FSAF_ALT)!=0)


#define RVAL(x)  (SHORT)( (x >>16) & 0x000000FF )
#define GVAL(x)  (SHORT)( (x >> 8) & 0x000000FF )
#define BVAL(x)  (SHORT)(  x       & 0x000000FF )


struct Proxy {
   ODFrame*     frame;
   ODTransform* transform;
   ODULong      id;
   Proxy(ODFrame* f, ODTransform* t, ODULong i) {frame = f; transform = t; id = i;}
};


#define EXTERNALIZE_ALL       0
#define EXTERNALIZE_SELECTED  1

#define      FRAME_VIEW  _fSession->Tokenize(ev,kODViewAsFrame)
#define LARGE_ICON_VIEW  _fSession->Tokenize(ev,kODViewAsLargeIcon)
#define SMALL_ICON_VIEW  _fSession->Tokenize(ev,kODViewAsSmallIcon)
#define  THUMBNAIL_VIEW  _fSession->Tokenize(ev,kODViewAsThumbnail)

#define PRES_DEFAULT     _fSession->Tokenize(ev, kODPresDefault)

#define SOM_ENVIRONMENT somGetGlobalEnvironment()

ODFacet*  FirstFacet( Environment* ev, ODFrame *frame );
void RemoveFrameFacets( Environment* ev, ODFrame *frame );
ODBoolean FrameIsActive( Environment* ev, ODFrame* frame);
void QueryFacetOffset( Environment* ev, ODFacet* facet, PPOINTL offset );
void QueryFrameOffset( Environment* ev, ODFrame* frame, PPOINTL offset );

ODBoolean SetFocus( Environment *ev, ODStorageUnit* su,
                    ODPropertyName prop, ODValueType val );

void InitPropVal( Environment *ev, ODStorageUnit* su,
                  ODPropertyName prop, ODValueType val );

void DisplayHelp( Environment* ev, ODSession* session,
                  ODHelp** fHelp, ODULong panelID );

void LogUndoAction( GraphicsPart* somSelf, Environment* ev, PVOID buf,
                    ULONG bufSize, ODSession* fSession, PSZ uTxt);

void SetScaling2( Environment *ev, ODFacet* facet, HPS hps );
void SetupFacet( Environment* ev, ODFacet* facet, ODShape* invalShape,
                 HPS hps, BOOL clipEmbeds );

HWND ObjWindow( GraphicsPart *gtkpart );
void MBoxWarn( HWND hwnd, const char * msg );

