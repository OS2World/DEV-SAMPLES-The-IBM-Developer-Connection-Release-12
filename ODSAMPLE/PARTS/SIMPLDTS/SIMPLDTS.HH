#ifndef SimplDTS_HH_INCLUDED
#define SimplDTS_HH_INCLUDED

/* Start Class SimpleDTSPart */

// #include <som.hh>

#pragma SOMAsDefault(on)

class SOMClass;
class SOMObject;
class ODFrame;
class ODTransform;
class ODFacet;
class ODPlatformCanvas;
class ODObject;
class ODExtension;
class ODRefCntObject;
class ODPersistentObject;
class ODStorageUnit;
class ODPart;
class ODCanvas;
class ODLink;
class ODLinkSource;
class ODMenuBar;
class ODShape;
class ODStorageUnitView;
class ODDragItemIterator;
class ODTypeList;
class ODEmbeddedFramesIterator;
class ODObjectSpec;
class ODWindow;
class ODSession;
class ODFocusSet;
class ODSemanticInterface;
class ODPopup;
class M_SimpleDTSPart;
class MyMenuExtension;
#pragma SOMAsDefault(pop)

#include <Part.hh>

class PartInfoRec;
class ODxOrderedCollection;
class ODxOrderedCollectionIterator;

typedef long PRNINFO;
typedef long RGBColor;

struct Proxy {
   ODFrame* frame;
   ODTransform* transform;
   ODBoolean transformChanged;
   Proxy(ODFrame* f, ODTransform* t) {frame = f; transform = t; transformChanged = kODTrue;}
};

class PartInfoRec
{
  public:
    PartInfoRec() { fIsActive = kODFalse; fNeedsActivating = kODFalse;
                    fIsPopupMenu = kODFalse; fIsSelected = kODFalse; }
    virtual ~PartInfoRec() { }
    ODBoolean  fIsActive;  // Does this frame have its focus set?
    ODBoolean  fNeedsActivating; // should this be activated when its window is activated?
    ODBoolean  fIsPopupMenu;                  // Is this a popup menu ?
    ODBoolean  fIsSelected;                   // Is this the selected frame?
};

#pragma SOMNonDTS(on)
#pragma SOMAsDefault(on)

class SimpleDTSPart : public ODPart {

     #pragma SOMClassName(*, "SimpleDTSPart")
     #pragma SOMNoMangling(*)
     #pragma SOMNoDataDirect(*)
     #pragma SOMNonDTS(*)
     #pragma SOMClassName (SimpleDTSPart, "SimpleDTSPart")
     #pragma SOMMetaClass (SimpleDTSPart, M_SimpleDTSPart)
     #pragma SOMClassVersion (SimpleDTSPart, 1, 0)
     #pragma SOMCallstyle (idl)
     #pragma SOMAsDefault(off)

  public :
     typedef struct simpleContent {
              long fColor;
     } simpleContent;

#pragma SOMAsDefault(pop)

     SimpleDTSPart();
     virtual ~SimpleDTSPart();

     virtual ODSemanticInterface* GetSemanticInterface();
     virtual void CommonInitSimpleDTSPart();
     virtual ODBoolean HandleMouseDown(ODFacet* facet, ODPoint* where,
                                       ODEventData* event);
     virtual void MoveResize(ODFacet* facet, Proxy* selection,
                             ODSLong fs);
     virtual ODBoolean HandleMouseDownDrag(ODFacet* facet, Proxy* selection, 
                                           ODEventData* event);
     virtual ODBoolean HandleMouseDownEdgeResize(ODFacet* facet,
                                                 Proxy* selection,
                                                 POINTL* mouse);
     virtual ODBoolean HandleMouseDownCornerResize(ODFacet* facet,
                                                   Proxy* selection,
                                                   POINTL* mouse);
     virtual ODBoolean HandleKeyDown(ODFrame* focusFrame, ODEventData* event);
     virtual ODBoolean HandleMouseMove(ODFacet* facet, ODPoint* where,
                                       ODEventData* event);
     virtual void ActivateFrame(ODFrame* frame);
     virtual void DeActivateFrame(ODFrame* frame);
     virtual void ActivatingWindow(ODFrame* frame);
     virtual void DeActivatingWindow(ODFrame* frame);
     virtual void UserSetBGColor(ODFrame* frame, RGBColor whichColor);
     virtual void SetBGColor(ODFrame* aFrame, ODUShort whichColor);
     virtual ODWindow* CreateWindow(ODFrame* sourceFrame);
     virtual ODBoolean HandleMenuEvent(ODFrame* focusFrame, ODEventData* event);
     virtual RGBColor GetDefaultColor();
     virtual void SetDefaultColor(RGBColor color);
     virtual void Activate(ODFacet* facet);
     virtual void InstallMenus();
     virtual PartInfoRec* AllocFramePartInfo();
     virtual ODxOrderedCollectionIterator* CreateDisplayFramesIterator();

     virtual void FulfillPromise(ODStorageUnitView* promiseSUView);
     virtual void DropCompleted(ODPart* destPart, ODDropResult dropResult);
     virtual MRESULT DragEnter(ODDragItemIterator* dragInfo, 
                               ODFacet* facet, ODPoint* where);
     virtual MRESULT DragWithin(ODDragItemIterator* dragInfo,
                                ODFacet* facet, ODPoint* where);
     virtual void DragLeave(ODFacet* facet, ODPoint* where);
     virtual ODDropResult Drop(ODDragItemIterator* dropInfo,
                               ODFacet* facet, ODPoint* where);
     virtual void ContainingPartPropertiesUpdated(ODFrame* frame,
                                                  ODStorageUnit* propertyUnit);
     virtual ODStorageUnit* AcquireContainingPartProperties(ODFrame* frame);
     virtual ODBoolean RevealFrame(ODFrame* embeddedFrame, ODShape* revealShape);
     virtual void EmbeddedFrameSpec(ODFrame* embeddedFrame, ODObjectSpec* spec);
     virtual ODEmbeddedFramesIterator* CreateEmbeddedFramesIterator(ODFrame* frame);
     virtual void DisplayFrameAdded(ODFrame* frame);
     virtual void DisplayFrameConnected(ODFrame* frame);
     virtual void AttachSourceFrame(ODFrame* frame, ODFrame* sourceFrame);
     virtual void DisplayFrameRemoved(ODFrame* frame);
     virtual void DisplayFrameClosed(ODFrame* frame);
     virtual void FrameShapeChanged(ODFrame* frame);
     virtual void ViewTypeChanged(ODFrame* frame);
     virtual void PresentationChanged(ODFrame* frame);
     virtual void SequenceChanged(ODFrame* frame);
     virtual void WritePartInfo(ODInfoType partInfo, ODStorageUnitView* storageUnitView);
     virtual ODInfoType ReadPartInfo(ODFrame* frame, ODStorageUnitView* storageUnitView);
     virtual void ClonePartInfo(ODDraftKey key, ODInfoType partInfo, 
                                ODStorageUnitView* storageUnitView, ODFrame* scopeFrame);
     virtual ODID Open(ODFrame* frame);
     virtual ODFrame* RequestEmbeddedFrame(ODFrame* containingFrame,
                                           ODFrame* baseFrame,
                                           ODShape* frameShape,
                                           ODPart* embedPart,
                                           ODTypeToken viewType,
                                           ODTypeToken presentation,
                                           ODBoolean isOverlaid);
     virtual void RemoveEmbeddedFrame(ODFrame* embeddedFrame);
     virtual ODShape* RequestFrameShape(ODFrame* embeddedFrame,
                                        ODShape* frameShape);
     virtual void UsedShapeChanged(ODFrame* embeddedFrame);
     virtual ODShape* AdjustBorderShape(ODFacet* embeddedFacet,
                                        ODShape* shape);
     virtual void FacetAdded(ODFacet* facet);
     virtual void FacetRemoved(ODFacet* facet);
     virtual void CanvasChanged(ODFacet* facet);
     virtual void GeometryChanged(ODFacet* facet, ODBoolean clipShapeChanged,
                                  ODBoolean externalTransformChanged);
     virtual void Draw(ODFacet* facet, ODShape* invalidShape);
     virtual void CanvasUpdated(ODCanvas* canvas);
     virtual void HighlightChanged(ODFacet* facet);
     virtual ODLinkSource* CreateLink(ODByteArray* data);
     virtual void LinkUpdated(ODLink* updatedLink, ODUpdateID change);
     virtual void RevealLink(ODLinkSource* linkSource);
     virtual void BreakLink(ODLink* link);
     virtual void BreakLinkSource(ODLinkSource* linkSource);
     virtual void LinkBroken(ODLink* link);
     virtual void UpdateFromLinkSource(ODLinkSource* linkSource);
     virtual void ShowLink(ODLink* link);
     virtual void EmbeddedFrameUpdated(ODFrame* frame, ODUpdateID change);
     virtual void LinkStatusChanged(ODFrame* frame);
     virtual ODBoolean BeginRelinquishFocus(ODTypeToken focus,
                                            ODFrame* ownerFrame,
                                            ODFrame* proposedFrame);
     virtual void CommitRelinquishFocus(ODTypeToken focus, ODFrame* ownerFrame,
                                        ODFrame* proposedFrame);
     virtual void AbortRelinquishFocus(ODTypeToken focus, ODFrame* ownerFrame,
                                       ODFrame* proposedFrame);
     virtual void FocusAcquired(ODTypeToken focus, ODFrame* ownerFrame);
     virtual void FocusLost(ODTypeToken focus, ODFrame* ownerFrame);
     virtual void CloneInto(ODDraftKey key, ODStorageUnit* toSU,
                            ODFrame* scope);
     virtual void ExternalizeKinds(ODTypeList* kindset);
     virtual void Externalize();
     virtual void ChangeKind(ODType kind);
     virtual ODBoolean HandleEvent(ODEventData* event, ODFrame* frame,
                                   ODFacet* facet, ODEventInfo* eventInfo);
     virtual ODBoolean HandleEventInEmbedded(ODEventData* event, 
                                             ODFrame* frame, ODFacet* facet, 
                                             ODFrame* embeddedFrame, 
                                             ODFacet* embeddedFacet);
     virtual void MouseEnter(ODFacet* facet, ODPoint* where);
     virtual void MouseWithin(ODFacet* facet, ODPoint* where);
     virtual void MouseLeave(ODFacet* facet);
     virtual void AdjustMenus(ODFrame* frame);
     virtual void UndoAction(ODActionData* actionState);
     virtual void RedoAction(ODActionData* actionState);
     virtual void DisposeActionState(ODActionData* actionState,
                                     ODDoneState doneState);
     virtual void WriteActionState(ODActionData* actionState,
                                   ODStorageUnitView* storageUnitView);
     virtual ODActionData ReadActionState(ODStorageUnitView* storageUnitView);
     virtual void InitPart(ODStorageUnit* storageUnit, ODPart* partWrapper);
     virtual void InitPartFromStorage(ODStorageUnit* storageUnit,
                                      ODPart* partWrapper);
     virtual void Release();
     virtual ODBoolean HasExtension(ODType extensionName);
     virtual ODExtension* AcquireExtension(ODType extensionName);
     virtual void ReleaseExtension(ODExtension* extension);

     ODPart *fPartWrapper;
#pragma SOMAttribute(fPartWrapper, readonly, virtualaccessors)

  private :
     ODID fWindowID;
     ODULong fFrameGroupIDCounter;
     POINTL fDragStart;
     ODxOrderedCollection *fContents;
     ODxOrderedCollection *fDisplayFrames;
     simpleContent fContentStruct;
     ODFocusSet *fFocusSet;
     ODTypeToken fSelectionFocus;
     ODTypeToken fMenuFocus;
     ODTypeToken fKeyFocus;
     ODSemanticInterface *fSemtIntf;
     ODSession *fSession;
     ODStorageUnit *fTestDrawSU;
     HPS hpsMem;
     HDC hdcMem;
     ODPoint mouseStart;
     ODMenuBar *fMenuBar;
     ODPopup *fPopup;
     MyMenuExtension *fMenuExt;
     ODBoolean fIsInitialized;

     #pragma SOMReleaseOrder ( \
                              "GetSemanticInterface", \
                              "CommonInitSimpleDTSPart", \
                              "HandleMouseDown", \
                              "MoveResize", \
                              "HandleMouseDownDrag", \
                              "HandleMouseDownEdgeResize", \
                              "HandleMouseDownCornerResize", \
                              "HandleKeyDown", \
                              "HandleMenuEvent", \
                              "HandleMouseMove", \
                              "ActivateFrame", \
                              "DeActivateFrame", \
                              "ActivatingWindow", \
                              "DeActivatingWindow", \
                              "SetBGColor", \
                              "UserSetBGColor", \
                              "CreateWindow", \
                              "GetDefaultColor", \
                              "SetDefaultColor", \
                              "Activate", \
                              "InstallMenus", \
                              "AllocFramePartInfo", \
                              "CreateDisplayFramesIterator", \
                              "HandleEventInEmbedded", \
                              "MouseEnter", \
                              "MouseWithin", \
                              "MouseLeave", \
                              "_get_fPartWrapper")
};

#pragma SOMAsDefault(pop)

#pragma SOMNonDTS(pop)

/* End SimpleDTSPart */

/* Start Class M_SimpleDTSPart */

// #include <Part.hh>
#include <somcls.hh>

#pragma SOMNonDTS(on)
#pragma SOMAsDefault(on)

class M_SimpleDTSPart : public M_ODPart {

     #pragma SOMClassName(*, "M_SimpleDTSPart")
     #pragma SOMNoMangling(*)
     #pragma SOMNoDataDirect(*)
     #pragma SOMNonDTS(*)
     #pragma SOMCallstyle (idl)
     #pragma SOMAsDefault(off)
     #pragma SOMAsDefault(pop)

  public :
     virtual ISOString clsGetODPartHandlerName();
     virtual string clsGetODPartHandlerDisplayName();
     virtual _IDL_SEQUENCE_PartKindInfo clsGetODPartKinds();
     virtual string clsGetOLE2ClassId();
     virtual string clsGetWindowsIconFileName();

     #pragma SOMReleaseOrder ()
};

#pragma SOMAsDefault(pop)
#pragma SOMNonDTS(pop)

/* End M_SimpleDTSPart */

#endif /* SimplDTS_HH_INCLUDED */
