// *****************************************************************************
//
// CHANGES
//
//  12/3/95  acw removed gtkprint.cpp module
//  12/3/95  acw removed dead code (part methods)
//  11/27/95 acw various menu mnemonics fixed incl undo/redo 144778
//  11/27/95 st  144010 AdjustMenus: Allow for adjusting when root part but
//               not active part
//  11/26/95 acw removed initmenu dependency
//  11/26/95 acw fixed non-working arrow-key moves
//  11/26/95 acw converted to resource-based acceltable handling, ACCELMAX fix
//  11/26/95 acw removed APP-mode menu switching
//  11/26/95 acw fixed copy/paste of single embedded frame
//  11/22/95 acw Clipboard Edit-item enablement (5001)
//  11/22/95 acw Clipboard re-enable
//  11/21/95 acw Major delta, lots of fixes
//  11/17/95 acw Workaround for 143738 - bug in GPI
//  11/14/95 st  including menubar.xih/popup.xih because of private initmenubar
//  11/13/95 ced Apple DR4 IDL changes; ODObject::IsInitailized() removed
//  11/11/95     st    use cmddefs.xh not odmenuid.h
//   8/15/95     GROUP object added
//   8/8/95      Drag/Drop cleanup (offsets and such)
//   8/7/95      REMOVED object window (objwnd.cpp) as anchor for toolbar/mouse capture
//               Moved to cooked msgs wherever possible.
//               Dead code removal and cleanup.
//   8/6/95      First cut at mixed-content drag/drop
//   8/5/95      First cut at mixed-content clipboard
//   8/4/95      Moved embed read/write into GtkEmbed class
//   7/27/95     FIX for 131151, zordering of tool palette and main menubar
//   7/22/95     Fix for 128237 TRAP on part delete and re-create
//   7/07/95     Check for StatusLine 128543
//   6/24/95     Started Undo/Redo (background color)
//   6/23/95     Dropped Phase0 to Boca     +++++++++++++++++++++++++++
//   6/20/95     Toolbar initial positioning
//               Full FSM StatusLine enablement
//   6/15/95     Added Rotate FSM
//   6/8/95      Fixed Drop offset bug
//               Fixed Embed shadow handling
//               Re-enabled popups through new ODPopup class
//                  (means the part MUST activate... :( )
//               Re-enabled dragging an embed OUT (Alt+drag)
//
// ******************************************************


#ifndef SOM_Module_gtkpart_Source
#define SOM_Module_gtkpart_Source
#endif

#define GraphicsPart_Class_Source
#define VARIABLE_MACROS

#ifndef _EXCEPT_
#include "Except.h"
#endif

#define INCL_DOSFILEMGR         //     File Management
#define INCL_DOSMODULEMGR
#define INCL_DOSRESOURCES
#define INCL_DOSPROCESS
#define INCL_GPIBITMAPS
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES
#define INCL_GPIPOLYGON
#define INCL_GPIREGIONS
#define INCL_GPIPATHS
#define INCL_GPITRANSFORMS
#define INCL_GPILOGCOLORTABLE
#define INCL_WINSTDCNR
#define INCL_WINACCELERATORS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINMESSAGEMGR
#define INCL_WINPOINTERS
#define INCL_WINSTDDRAG
#define INCL_WINSYS
#define INCL_WINTRACKRECT
#define INCL_WINWINDOWMGR
#define INCL_WINERRORS
#define INCL_DEV

#define INCL_ODAPI
#define INCL_ODARBITRATOR
#define INCL_ODCANVAS
#define INCL_ODCLIPBOARD
#define INCL_ODDRAGANDDROP
#define INCL_ODDRAFT
#define INCL_ODERRORS
#define INCL_ODFACET
#define INCL_ODFOCUSSET
#define INCL_ODFRAME
#define INCL_ODINFO
#define INCL_ODLINK
#define INCL_ODLINKSOURCE
#define INCL_ODLINKSPEC
#define INCL_ODMENUBAR
#define INCL_ODPARTHANDLERINFO
#define INCL_ODPARTHANDLERREGISTRY
#define INCL_ODREGISTRYMANAGER
#define INCL_ODSESSION
//#define INCL_ODSEMANTICINTERFACE
#define INCL_ODSHAPE
#define INCL_ODSTORAGEUNIT
#define INCL_ODSTORAGEUNITVIEW
#define INCL_ODTRANSFORM
#define INCL_ODUI
#define INCL_ODUNDO
#define INCL_ODWINDOW
#define INCL_ODWINDOWSTATE

void  ODSetSOMException( void );

#include <os2.h>
#include <stdio.h>
#include "gtkpart.xih"

#include <GtkNoteb.xh>

#ifndef _FOCUSLIB_
   #include "focuslib.h"
#endif

#ifndef _TEMPOBJ_
#include "TempObj.h"
#endif

#ifndef _STDTYPIO_
#include "stdtypio.h"
#endif

#ifndef _ODDEBUG_
#include "ODDebug.h"
#endif

#ifndef _WINUTILS_
   #include "winutils.h"
#endif

#ifndef __CMDDEFS__
   #include "cmddefs.xh"
#endif

#ifndef SOM_XEmbeddedFramesIterator_xh
   #include <XFrmIter.xh>
#endif

#ifndef _ORDCOLL_
#include "OrdColl.h"
#endif

#include "gtkbase.h"
#include "gtkpic.h"
#include "gtkres.h"
//#include "print.h"
#include "actions.h"
#include "mygpi.h"
#include "platform.h"

#include "partinfo.h"
#include "common.hpp"
#include "debug.hpp"  // 124384


const ODType   kPartHandlerName              = "GraphicsPart";
const ODType   kPartHandlerDisplayName       = "IBM 2D Graphics Part";
const ODType   kGraphicsPartKindDisplayName  = "IBM 2D Graphics";
const ODType   kGraphicsPartCategory         = "2D Graphics";


//#define ODDebug2


// ------------------------------------------------------------------
//   some useful utilities...                                     aaa
// ------------------------------------------------------------------

// ------------------------------------------------------------------
ODFacet* FirstFacet( Environment* ev, ODFrame *frame )
{
  ODFrameFacetIterator* facets = frame->CreateFacetIterator(ev);

  ODFacet* facet = facets->First(ev);
  delete facets;
  return facet;
}

// ------------------------------------------------------------------------
void ClearMyLinkSpec( Environment* ev, ODSession* session,
                      ODFrame* frame, ODUpdateID* myID )
{
  if (*myID) {

    ODClipboard*  clipboard = session->GetClipboard(ev);

    // is MY data still there???
    if (*myID == clipboard->GetUpdateID(ev)) {

      ODArbitrator *arbitrator = session->GetArbitrator( ev );
      ODTypeToken clipboardFocus = session->Tokenize( ev, kODClipboardFocus );

      // get focus
      if ( arbitrator->RequestFocus( ev, clipboardFocus, frame ) ) {

        // re-verify changeID!
        if ( *myID == clipboard->GetUpdateID(ev) ) {

          ODStorageUnit* clipSU = clipboard->GetContentStorageUnit(ev);

          ODSURemoveProperty( ev, clipSU, kODPropLinkSpec );

          if (SetFocus( ev, clipSU, kODPropContents, kKindGraphicsPart))
            clipboard->ExportClipboard( ev );
        }

        arbitrator->RelinquishFocus( ev, clipboardFocus, frame );
      }
    }

    *myID = 0;
  }
}

// ------------------------------------------------------------------
void RemoveFrameFacets( Environment* ev, ODFrame *frame )
{
  /* Make a copy of the frame's facet collection so that we can  */
  /* iterate through the copy.  The reason for this is that when */
  /* we call RemoveFacet, the facet is removed from the frame's  */
  /* collection of facet's thereby invalidating the iterator     */
  /* cursor.  By iterating through the copy collection, we avoid */
  /* this side effect.                                           */

  ODxOrderedCollection* tempFacets = new ODxOrderedCollection;

  ODFrameFacetIterator* facets = frame->CreateFacetIterator(ev);
  for (ODFacet* f1 = facets->First(ev);
        facets->IsNotComplete(ev); f1 = facets->Next(ev))
     tempFacets->AddLast(f1);

  delete facets;

  ODxOrderedCollectionIterator t(tempFacets);
  for (ODFacet* f2 = (ODFacet*)t.First();
        t.IsNotComplete();
        f2 = (ODFacet*)t.Next())
  {
     f2->GetContainingFacet(ev)->RemoveFacet(ev, f2);
     delete f2;
  }
  delete tempFacets;
}

// ---------------------------------------------------------------
ODBoolean FrameIsActive(Environment* ev, ODFrame* frame)
{
  if (frame) {
    PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);
    return pInfo->fIsActive;
  } else return kODFalse;
}

// ---------------------------------------------------------------
void QueryFacetOffset( Environment* ev, ODFacet* facet, PPOINTL offset )
{
  if (facet) {
    // get the frame rect
    TempODShape shape = facet->GetFrame(ev)->AcquireFrameShape(ev, kODNULL);
    ODRect bounds;
    shape->GetBoundingBox( ev, &bounds );

    // put it in window coords
    ODPoint pt = {bounds.left,bounds.bottom};
    TempODTransform xform = facet->AcquireWindowFrameTransform(ev, kODNULL);
    xform->TransformPoint(ev, &pt);

    // convert
    offset->x = FIXED2LONG(pt.x);
    offset->y = FIXED2LONG(pt.y);
  } else {
    offset->x = offset->y = -1;
  }
}

// ---------------------------------------------------------------
void QueryFrameOffset( Environment* ev, ODFrame* frame, PPOINTL offset )
{ QueryFacetOffset( ev, FirstFacet( ev, frame ), offset ); }

// ------------------------------------------------------------------
//   Create/Destroy/restore support                               aaa
// ------------------------------------------------------------------

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartInitPart( GraphicsPart *somSelf,
                                              Environment *ev,
                                              ODStorageUnit* storageUnit,
                                              ODPart* partWrapper)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","InitPart");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering InitPart\n");
  #endif

  SOM_TRY

  parent_InitPart(somSelf, ev, storageUnit, partWrapper);

  _fPartWrapper = partWrapper;
  _fNeedToExternalize = kODTrue;

  ODStorageUnit* msu = somSelf->GetStorageUnit(ev);
  somSelf->CommonInitGraphicsPart(ev);

  InitPropVal( ev, msu, kODPropContents, kKindGraphicsPart );

  // new code to support custom Part icons
  ULONG  cbIconSize;                  /* Size of icon data returned           */
  PBYTE  pIconData = NULL;            /* Scratch ptr to a block of icon data  */
  if ( !DosQueryResourceSize( _hmod, RT_POINTER, ID_PARTICON, &cbIconSize) &&
       !DosGetResource( _hmod, RT_POINTER, ID_PARTICON, (PPVOID)&pIconData)  ) {

    InitPropVal( ev, msu, kODPropIconFamily, kODIconFamily );
    StorageUnitSetValue( msu, ev, cbIconSize, (ODValue)pIconData );

    DosFreeResource(pIconData);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving InitPart\n");
  #endif
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartInitPartFromStorage( GraphicsPart *somSelf,
                                                         Environment *ev,
                                                         ODStorageUnit* storageUnit,
                                                         ODPart* partWrapper)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","InitPartFromStorage");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering InitPartFromStorage\n");
  #endif

  SOM_TRY

//  somSelf->InitPersistentObjectFromStorage(ev, storageUnit);
  parent_InitPartFromStorage(somSelf, ev, storageUnit, partWrapper);

  _fPartWrapper = partWrapper;

  somSelf->CommonInitGraphicsPart(ev);
  _fNeedToExternalize = kODFalse;

  ODStorageUnit* su;
  ODStorageUnitRef aSURef;
  ODFrame* frame = 0;
  ODULong offset, offsetLimit;
  ODTransform* transform;
  ULONG id;                                   // acw - p-obj id

  su = somSelf->GetStorageUnit(ev);

  if (SetFocus( ev, su, kODPropDisplayFrames, kODWeakStorageUnitRef) ) {
    offsetLimit = su->GetSize(ev);

    for (offset = 0; offset < offsetLimit; offset += sizeof(ODStorageUnitRef))
    {
      su->SetOffset(ev, offset);
      StorageUnitGetValue(su, ev, sizeof(ODStorageUnitRef), (ODValue)&aSURef);

      if (su->IsValidStorageUnitRef(ev, aSURef)) {
        frame = su->GetDraft(ev)->AcquireFrame(ev, su->GetIDFromStorageUnitRef(ev, aSURef));

        _fDisplayFrames->AddLast(frame);
        frame->SetDroppable(ev, kODTrue);
      }
    }
  }

  if (SetFocus( ev, su, kODPropFrameGroup, kODULong))
    StorageUnitGetValue(su, ev, sizeof(_fFrameGroupIDCounter),(ODValue)&_fFrameGroupIDCounter);

  // read our real Contents, key=0 -> our su and not cloning in
  if (SetFocus( ev, su, kODPropContents, kKindGraphicsPart) ) {

    _picture->ReadFromStorage(ev, su, 0, somSelf);

    if (!frame || !frame->IsRoot(ev))
      _picture->PostReadInit(ev, somSelf, kODFalse);

    StorageUnitGetValue( su, ev, sizeof(_fBGMode), (ODValue)&_fBGMode);
    StorageUnitGetValue( su, ev, sizeof(_fBGColor), (ODValue)&_fBGColor);
    StorageUnitGetValue( su, ev, sizeof(_fMatchBGColor), (ODValue)&_fMatchBGColor);
    StorageUnitGetValue( su, ev, CCHMAXPATH, (ODValue)&_fBitmapFile[0]);
    StorageUnitGetValue( su, ev, sizeof(_fToolsVisible), (ODValue)&_fToolsVisible);
    StorageUnitGetValue( su, ev, sizeof(_toolsOffset), (ODValue)&_toolsOffset);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving InitPartFromStorage\n");
  #endif
}

// -- semi-BASE method  (MODIFIED) ----------------------------------
SOM_Scope void  SOMLINK GraphicsPartCommonInitGraphicsPart( GraphicsPart *somSelf,
                                                            Environment *ev)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","CommonInitGraphicsPart");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering CommonInitGraphicsPart\n");
  #endif

  SOM_TRY

  ODStorageUnit* su = somSelf->GetStorageUnit(ev);

  // used a LOT, save the session handle
  _fSession = su->GetSession(ev);

  _fReadOnly = (su->GetDraft(ev)->GetPermissions(ev) == kODDPReadOnly);
//  _fReadOnly = (su->GetDraft(ev)->GetPermissions(ev) == kDPReadOnly);

  // get resource module handle for menus, cursors, etc.
  CHAR  Error[256] = "";
  DosLoadModule( Error, sizeof(Error), DLL_NAME, &_hmod );

  if (_hmod == kODNULL) {
    // Note that the following strings CANNOT be loaded from the resource file :)
    WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
                   "Unable to load 2D Graphics Part's resource file.",
                   "Startup warning!", 0, MB_ENTER | MB_ERROR);
  }

  // init these values
  _fBGColor = 0x00FFFFFF;    // 0x00c9c9c9;
  _fMatchBGColor = kODTrue;
  _fBitmapFile[0] = '\0';

  _fCurFrame = kODNULL;

  _fSemtIntf = kODNULL;
  _fNotebook = kODNULL;
  _fHelp     = kODNULL;

  _fDisplayFrames = new ODxOrderedCollection;
  _fEmbeddedFrames = new ODxOrderedCollection;
  _fContents = new ODxOrderedCollection;

  // is there a status line for our window??
  if(_fSession->HasExtension(ev, kODExtStatusLine)) // 128543 jso
  {
    _fStatusLn =(ODStatusLineExtension*) _fSession->AcquireExtension(ev,kODExtStatusLine); // 124281
    _fStatusFocus = _fSession->Tokenize(ev, kODStatusLineFocus);
  }

  // set up focus handling stuff
  _fMouseFocus = _fSession->Tokenize(ev, kODMouseFocus);
  _fClipboardFocus = _fSession->Tokenize(ev, kODClipboardFocus);

  // create our focus set for standard activation
  _fFocusSet = _fSession->GetArbitrator(ev)->CreateFocusSet(ev);

  // add keyboard and selection focii always
  _fSelectionFocus = _fSession->Tokenize(ev, kODSelectionFocus);
  _fFocusSet->Add(ev, _fSelectionFocus);
  _fKeyFocus = _fSession->Tokenize(ev, kODKeyFocus);
  _fFocusSet->Add(ev, _fKeyFocus);

  // verify that the session HAS a menubar.  If not, fMenuFocus is left 0
  // and not added to our focusset.  Defer creation of our menu until we
  // actually get activated (and know whether we are rootFrame or not!).
  ODMenuBar* baseMenu = _fSession->GetWindowState(ev)->AcquireBaseMenuBar(ev);
  if (baseMenu) {
    _fMenuFocus = _fSession->Tokenize(ev, kODMenuFocus);
    _fFocusSet->Add(ev, _fMenuFocus);
    ODReleaseObject( ev, baseMenu );
  }

  _hpsMem = CreateHps( 400, 400, HPS_BITMAP );

  _ptrXhair = LoadPointer( _hmod, ID_XHAIR );

  _picture = new GtkPic();

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving CommonInitGraphicsPart\n");
  #endif
}


// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartsomInit(GraphicsPart *somSelf)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","somInit");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering somInit\n");
  #endif

  GraphicsPart_parent_ODPart_somInit(somSelf);

  _fDisplayFrames = kODNULL;
  _fEmbeddedFrames = kODNULL;
  _fWindowID = 0;

  _fContents = kODNULL;
   _fNeedToExternalize = kODFalse;

  _fFrameGroupIDCounter = 1;

  _fSession = kODNULL;
  _fFocusSet = kODNULL;

  _fStatusFocus = kODNULL;
  _fMouseFocus = kODNULL;
  _fClipboardFocus = kODNULL;
  _fMenuFocus = kODNULL;
  _fSelectionFocus = kODNULL;
  _fKeyFocus = kODNULL;

  _fSemtIntf = kODNULL;

  _fMenuBar = kODNULL;
  _fPartWrapper = kODNULL;
  _fExtension = kODNULL;
  _fHaveStatusLineFocus = kODNULL;  // 124281
  _fViewExtension = kODNULL;
  _fStatusLn = 0;

  _fClipboardChangeID = 0;

  // ** acw added constructor stuff *********************************
  _fLinksVisible = kODFalse;
  _fToolsVisible = kODTrue;
  _toolsOffset.x = _toolsOffset.y = 0;
  _fsmHps = 0;
  _fsmType = ACTION_SELECT;
  _fsmState = 0;
  _fAltMark = FALSE;
  _hbmThumbnail = NULL;

  _hpsMem = 0;

  _icon = 0;
  _ptr = 0;

  _ptrXhair = 0;
  _ptrLine = 0;
  _ptrBox = 0;
  _ptrEllipse = 0;
  _ptrPolyline = 0;
  _ptrArc = 0;

  _picture = 0;
  _hwndObj = 0;

  _rclFrame.xRight = 0;
  _hwndTools = 0;

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving somInit\n");
  #endif
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartsomUninit(GraphicsPart *somSelf)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","somUninit");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering somUninit\n");
  #endif

  Environment* ev = SOM_ENVIRONMENT;

  delete _icon;

  if (_ptrXhair) WinDestroyPointer( _ptrXhair );
  if (_ptrLine) WinDestroyPointer( _ptrLine );
  if (_ptrBox) WinDestroyPointer( _ptrBox );
  if (_ptrEllipse) WinDestroyPointer( _ptrEllipse );
  if (_ptrArc) WinDestroyPointer( _ptrArc );
  if (_ptrPolyline) WinDestroyPointer( _ptrPolyline );

  if (_hmod) {
//    DosFreeModule(_hmod);
    _hmod = 0;
  }

  delete _picture;

  DestroyWindow(_hwndTools);

  if (_fContents) delete _fContents;

  ReleaseHps( _hpsMem );

  if (_fDisplayFrames) delete _fDisplayFrames;
  if (_fEmbeddedFrames) delete _fEmbeddedFrames;

    #define SEMANTIC_INTERFACES_NOT_DONE
    #ifndef SEMANTIC_INTERFACES_NOT_DONE
       if (_fSemtIntf != kODNULL)
           delete _fSemtIntf;
    #endif

  ODReleaseObject(ev, _fStatusLn );
  ODReleaseObject(ev, _fViewExtension );
  ODReleaseObject(ev, _fNotebook );

  GraphicsPart_parent_ODPart_somUninit(somSelf);

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving somUninit\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartRelease(GraphicsPart *somSelf,  Environment *ev)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","Release");

  SOM_TRY

    Proxy*    p;
    ODFrame*  frame;

    GraphicsPart_parent_ODPart_Release(somSelf, ev);

    if (somSelf->GetRefCount(ev) == 0) {

      #ifdef ODDebug2
        somPrintf("GtkPart: Entering final Release\n");
      #endif

      frame = (ODFrame*) _fDisplayFrames->First();
      while (frame != kODNULL) {
        _fDisplayFrames->Remove(frame);
        ODReleaseObject(ev,frame);
        frame = (ODFrame*) _fDisplayFrames->First();
      }
      frame = (ODFrame*) _fEmbeddedFrames->First();
      while (frame != kODNULL) {
        p = somSelf->ProxyForFrame(ev, frame);
        _fContents->Remove(p);
        _fEmbeddedFrames->Remove(frame);
        ODReleaseObject(ev,frame);
        frame = (ODFrame*) _fEmbeddedFrames->First();
      }
      somSelf->GetStorageUnit(ev)->GetDraft(ev)->ReleasePart(ev, somSelf);

      #ifdef ODDebug2
        somPrintf("GtkPart: Leaving final Release\n");
      #endif
    }

  SOM_CATCH_ALL
  SOM_ENDTRY
}

// ------------------------------------------------------------------
//   EXTERNALIZATION                                              aaa
// ------------------------------------------------------------------

// --- UTILITY ------------------------------------------------------
ODBoolean SetFocus( Environment *ev, ODStorageUnit* su,
                    ODPropertyName prop, ODValueType val )
{
  if ( su->Exists(ev, prop, val, 0) ) {
    su->Focus(ev, prop, kODPosUndefined, val, (val != NULL ? 0 : 1), kODPosUndefined);
    return kODTrue;
  }
  return kODFalse;
}

// --- UTILITY ------------------------------------------------------
void InitPropVal( Environment *ev, ODStorageUnit* su,
                  ODPropertyName prop, ODValueType val )
{
  if ( SetFocus(ev, su, prop, val) ) {
    // clear the old data
    su->DeleteValue( ev, su->GetSize(ev) );
  } else {
    su->AddProperty(ev, prop)->AddValue(ev, val);
  }
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartCloneInto( GraphicsPart *somSelf,
                                               Environment *ev,
                                               ODDraftKey key,
                                               ODStorageUnit* toSU,
                                               ODFrame* scope)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","CloneInto");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering CloneInto\n");
  #endif

  SOM_TRY

//  ODID scopeFrameID = 0;
//  ODStorageUnit * destTestDrawSU = somSelf->GetStorageUnit(ev);
//  if (scope != kODNULL)
//    scopeFrameID = scope->GetStorageUnit(ev)->GetID(ev);
//  destTestDrawSU->CloneInto(ev, key, toSU, scopeFrameID);

  ODDraft* fromDraft = somSelf->GetStorageUnit(ev)->GetDraft(ev);
  ODDraft* toDraft = toSU->GetDraft(ev);

  ODStorageUnitRef aSURef;
  ODULong offset;
  ODFrame* frame;
  ODxOrderedCollectionIterator aIter(_fDisplayFrames);

  InitPropVal( ev, toSU, kODPropDisplayFrames, kODWeakStorageUnitRef );

  offset = 0;
  for (frame = (ODFrame*)aIter.First(); aIter.IsNotComplete();
      frame = (ODFrame*)aIter.Next(), offset+=sizeof(ODStorageUnitRef))
  {

    ODID toFrameID = fromDraft->WeakClone(ev, key, frame->GetID(ev), 0, 0);
    toSU->GetWeakStorageUnitRef(ev, toFrameID, aSURef);
    toSU->SetOffset(ev, offset);
    StorageUnitSetValue(toSU, ev, sizeof(ODStorageUnitRef), (ODValue)&aSURef);
  }

  InitPropVal( ev, toSU, kODPropFrameGroup, kODULong);
  StorageUnitSetValue( toSU, ev, sizeof(_fFrameGroupIDCounter), (ODValue)&_fFrameGroupIDCounter);

  // key!=0 >> writing to external su
  InitPropVal( ev, toSU, kODPropContents, kKindGraphicsPart );
  _picture->WriteToStorage(ev, toSU, key, EXTERNALIZE_ALL);

  // write the metadata
  somSelf->QueryPaletteOffset(ev, &_toolsOffset);
  StorageUnitSetValue( toSU, ev, sizeof(_fBGMode), (ODValue)&_fBGMode);
  StorageUnitSetValue( toSU, ev, sizeof(_fBGColor), (ODValue)&_fBGColor);
  StorageUnitSetValue( toSU, ev, sizeof(_fMatchBGColor), (ODValue)&_fMatchBGColor);
  StorageUnitSetValue( toSU, ev, CCHMAXPATH, (ODValue)&_fBitmapFile[0]);
  StorageUnitSetValue( toSU, ev, sizeof(_fToolsVisible), (ODValue)&_fToolsVisible);
  StorageUnitSetValue( toSU, ev, sizeof(_toolsOffset), (ODValue)&_toolsOffset);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving CloneInto\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartExternalizeKinds(GraphicsPart *somSelf,
                                          Environment *ev, ODTypeList* kindset)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","ExternalizeKinds");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering ExternalizeKinds\n");
  #endif

  SOM_TRY

  somSelf->Externalize(ev);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving ExternalizeKinds\n");
  #endif
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartExternalize(GraphicsPart *somSelf,  Environment *ev)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","Externalize");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering Externalize\n");
  #endif

  SOM_TRY

  if (_fNeedToExternalize && !_fReadOnly) {
    ODStorageUnit* su = somSelf->GetStorageUnit(ev);
    ODStorageUnitRef aSURef;
    ODFrame* frame;

    // write out weak SU's to our display frames
    InitPropVal( ev, su, kODPropDisplayFrames, kODWeakStorageUnitRef );

    ODxOrderedCollectionIterator aIter(_fDisplayFrames);
    for ( frame = (ODFrame*)aIter.First(); aIter.IsNotComplete();
          frame = (ODFrame*)aIter.Next() ) {
      su->GetWeakStorageUnitRef(ev, frame->GetStorageUnit(ev)->GetID(ev), aSURef);
      StorageUnitSetValue(su, ev, sizeof(ODStorageUnitRef), (ODValue)&aSURef);
    }

    // write out our frame group id (whatever that is...)
    InitPropVal( ev, su, kODPropFrameGroup, kODULong);
    StorageUnitSetValue( su, ev, sizeof(_fFrameGroupIDCounter), (ODValue)&_fFrameGroupIDCounter);

    // key=0 -> writing to our own su (not cloning)
    InitPropVal( ev, su, kODPropContents, kKindGraphicsPart );
    _picture->WriteToStorage(ev, su, 0, EXTERNALIZE_ALL);

    // write the metadata
    somSelf->QueryPaletteOffset(ev, &_toolsOffset);
    StorageUnitSetValue( su, ev, sizeof(_fBGMode), (ODValue)&_fBGMode);
    StorageUnitSetValue( su, ev, sizeof(_fBGColor), (ODValue)&_fBGColor);
    StorageUnitSetValue( su, ev, sizeof(_fMatchBGColor), (ODValue)&_fMatchBGColor);
    StorageUnitSetValue( su, ev, CCHMAXPATH, (ODValue)&_fBitmapFile[0]);
    StorageUnitSetValue( su, ev, sizeof(_fToolsVisible), (ODValue)&_fToolsVisible);
    StorageUnitSetValue( su, ev, sizeof(_toolsOffset), (ODValue)&_toolsOffset);

    _fNeedToExternalize = kODFalse;
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving Externalize\n");
  #endif
}

// -- ADDED method  -----------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartExternalizeContents( GraphicsPart *somSelf,
                                                         Environment *ev,
                                                         ODStorageUnit* su,
                                                         ODDraftKey key,
                                                         ODBoolean fSelected )
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","ExternalizeContents");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering ExternalizeContents\n");
  #endif

  SOM_TRY

  // write the graphics content
  InitPropVal( ev, su, kODPropContents, kKindGraphicsPart );
  _picture->WriteToStorage(ev, su, key, fSelected);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving ExternalizeContents\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartChangeKind(GraphicsPart *somSelf,  Environment *ev,
                                   ODType kind)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","ChangeKind");

  #ifdef ODDebug2
    somPrintf("GtkPart: null ChangeKind\n");
  #endif

}

// ------------------------------------------------------------------
//   DRAG/DROP support                                            aaa
// ------------------------------------------------------------------

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartFulfillPromise(GraphicsPart *somSelf,
                                        Environment *ev, ODStorageUnitView* promiseSUView)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","FulfillPromise");

  #ifdef ODDebug2
    somPrintf("GtkPart: null FulfillPromise\n");
  #endif

}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDropCompleted(GraphicsPart *somSelf,
                                       Environment *ev, ODPart* destPart,
                                      ODDropResult dropResult)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","DropCompleted");

  #ifdef ODDebug2
    somPrintf("GtkPart: null DropCompleted\n");
  #endif

}

// -- D&D UTILITY -------------------------------------------------------
void InvertDragHilite( Environment * ev, ODFacet* facet)
{
  #ifdef ODDebug2
    somPrintf("GtkPart: \n");
  #endif

   /* the following structure declaration need to be emtted via passthru */
   /* in polygon.idl                                                     */

   struct ODContour {
      ODSLong  nVertices;          // Variable-size struct:
      ODPoint  vertex[1];          // Array size is actually nVertices
   };
   struct ODPolygonData {
     ODSLong     nContours;        // Variable-size struct:
     ODContour  firstContour;      // Rest of contours follow after first
   };

   HPS hps;

   CFocusWindow f(ev, facet, (ODShape*)kODNULL, &hps, (HWND*)kODNULL, CFocusWindow::DragPS);

   ODPolygon poly;
   TempODShape clipShape = facet->AcquireAggregateClipShape(ev, kODNULL);
   clipShape->CopyPolygon(ev, &poly);

   ODContour* pContour;
   ODPolygonData* pPolygonData = (ODPolygonData*)poly._buffer;
   PPOLYGON ppoly = new POLYGON[pPolygonData->nContours];
   pContour = &pPolygonData->firstContour;
   for (int i = 0; i < pPolygonData->nContours; i++)
   {
      ppoly[i].ulPoints = pContour->nVertices;
      ppoly[i].aPointl  = new POINTL[pContour->nVertices];
      for (int j = 0; j < pContour->nVertices; j++)
      {
         ppoly[i].aPointl[j] = ODPOINTL(pContour->vertex[j]);
         ppoly[i].aPointl[j].x -= 1;
         ppoly[i].aPointl[j].y -= 1;
      }
      pContour = (ODContour*)((char*)pContour + sizeof(ODSLong) + pContour->nVertices * sizeof(ODPoint));
   }

   GpiSetMix( hps, FM_INVERT );
   GpiBeginPath(hps, 1);
   for (i = 0; i < pPolygonData->nContours; i++)
   {
      GpiMove(hps, &ppoly[i].aPointl[ppoly[i].ulPoints-1]);
      GpiPolyLine(hps, ppoly[i].ulPoints, ppoly[i].aPointl);
   }
   GpiEndPath(hps);
   GpiSetLineWidthGeom(hps, 8);
   GpiStrokePath(hps, 1, 0);

   // Clean up

   for (i = 0; i < pPolygonData->nContours; i++)
      delete[] ppoly[i].aPointl;
   delete[] ppoly;

   DisposeByteArrayStruct(&poly);
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope ODDragResult  SOMLINK GraphicsPartDragEnter(GraphicsPart *somSelf,
                                                   Environment *ev,
                                                  ODDragItemIterator* dragInfo,
                                                  ODFacet* facet,
                                                  ODPoint* where)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","DragEnter");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DragEnter\n");
  #endif

  SOM_TRY

    InvertDragHilite( ev, facet );

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DragEnter\n");
  #endif

    return (ODDragResult)MRFROM2SHORT (DOR_DROP, DO_MOVE);
}


// -- BASE method  (un-modified) ------------------------------------
SOM_Scope ODDragResult  SOMLINK GraphicsPartDragWithin(GraphicsPart *somSelf,
                                                    Environment *ev,
                                                   ODDragItemIterator* dragInfo,
                                                   ODFacet* facet,
                                                   ODPoint* where)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","DragWithin");

  #ifdef ODDebug2
//    somPrintf("GtkPart: null DragWithin\n");
  #endif

    return (ODDragResult)MRFROM2SHORT (DOR_DROP, DO_MOVE);
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDragLeave(GraphicsPart *somSelf,  Environment *ev,
                                  ODFacet* facet, ODPoint* where)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","DragLeave");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DragLeave\n");
  #endif

  SOM_TRY

    InvertDragHilite( ev, facet );

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DragLeave\n");
  #endif
}

// -- UTILITY routine ------------------------------------
ODBoolean dmSelectRMF( PDRAGITEM pDragItem, PSZ pszSelectedRMF,
                       ODBoolean ODdata )
{
   // These are the types we are capbale of handling... another part
   // might accept something different.

  if (ODdata) {
    if ( DrgVerifyRMF( pDragItem, "DRM_OS2FILE", "DRF_OPENDOCDOCUMENT" ))
      strcpy( pszSelectedRMF, "<DRM_OS2FILE,DRF_OPENDOCDOCUMENT>" );
    else if ( DrgVerifyRMF( pDragItem, "DRM_SHAREDMEM", "DRF_OPENDOCDOCUMENT" ))
      strcpy( pszSelectedRMF, "<DRM_SHAREDMEM,DRF_OPENDOCDOCUMENT>" );
    else strcpy( pszSelectedRMF, "<DRM_OS2FILE,DRF_UNKNOWN>" );
  } else {
    strcpy( pszSelectedRMF, "<DRM_OS2FILE,DRF_UNKNOWN>" );
  }

  return kODTrue;
}

// -- UTILITY routine ------------------------------------
ODBoolean DetermineSubjectFromExtension( CHAR szFile[], CHAR szSubject[] )
{
  char *ext = strrchr( (const char *)szFile, '.' );
  ODBoolean found = kODFalse;

  if ( ext ) {

    if ( !stricmp( ext, ".BMP" )) {
      strcpy( szSubject, "Bitmap" );
      found = kODTrue;
    } else if ( !stricmp( ext, ".MET" )) {
      strcpy( szSubject, "Metafile" );
      found = kODTrue;
    }
  }

  return( found );
}

ODStorageUnit* DropContentSU( Environment *ev,
                              ODDragAndDrop*  dad, ODStorageUnit* dropSU,
                              ODBoolean ODdata )
{
  // first thing is to check for dragitem value, can't do anything
  // more if it is not present... (BUG)
  if ( !SetFocus( ev, dropSU, kODPropContents, kODDragitem) )
     return kODNULL;

  DRAGITEM DragItem;
  StorageUnitGetValue(dropSU, ev, sizeof(DRAGITEM), &DragItem );

  // now look for a format we can deal with
  CHAR szSelectedRMF[CCHMAXPATH];
  szSelectedRMF[0] = 0;
  if (!dmSelectRMF(&DragItem, szSelectedRMF, ODdata))
     // can't find something we like....
     return kODNULL;

  // this is receiving part's chance to affect the dragitem if
  // it wishes. note: the the drag transfer is going to
  // use the fields to build hstrRenderToName

  // create a view of our focused value for the drag manager
  ODStorageUnitView *dropView = dropSU->CreateView(ev);

  // ask the drag manager to get it for us
  ODStorageUnit *renderedSU;
//  if ( !dad->GetDataFromDragManager(ev, dropView, szSelectedRMF, &renderedSU) )
  if ( !dad->GetDataFromDragManager(ev, dropView, &renderedSU) )
     // error in transfer
     return kODNULL;

  return renderedSU;
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope ODDropResult  SOMLINK GraphicsPartDrop(GraphicsPart *somSelf,
                                      Environment *ev, ODDragItemIterator* dropInfo,
                                     ODFacet* facet, ODPoint* where)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","Drop");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering Drop\n");
  #endif

  ODDropResult dropResult;

  SOM_TRY

  ODDraft*        theDraft = somSelf->GetStorageUnit(ev)->GetDraft(ev);
  ODPoint         delta;
  Point           dragPoint, dropPoint, pinnedPoint;
  short           mouseDownModifiers, mouseUpModifiers;
  Proxy*          p;
  ODFrame*        frameDroppedIn = facet->GetFrame(ev);
  ODFrame*        newFrame = kODNULL;
  ODShape*        clipShape, *newFrameShape;
  ODTransform*    newExternalXForm;
  ODBoolean       canHandleDrop = kODFalse;
  ODBoolean       activeFrame = FrameIsActive(ev, frameDroppedIn);

  ODDragAndDrop*  dad = _fSession->GetDragAndDrop(ev);
  ODStorageUnit*  dropSU;

  // presumably I've highlighted myself, so turn it off now.
  InvertDragHilite(ev, facet);

  if (_fReadOnly)
    return kODDropFail;

  // get the cursor position in window coords
  POINTL ptl = CursorPosition( ev, facet );

  // reset any fsm action and go to Selection mode
  if ( activeFrame ) {
    somSelf->fsm_set_state( ev, 0, 0 );
    somSelf->UpdatePalette(ev, frameDroppedIn);
  }

  // Get the attributes for this drag
  ODULong      attributes = dad->GetDragAttributes(ev);
  dropResult = (attributes&kODDropIsMove) ? kODDropMove : kODDropCopy;

  for (dropSU = dropInfo->First(ev); dropSU; dropSU = dropInfo->Next(ev)) {

    // reset some operation flags
    ODBoolean simpleMove = kODFalse,
              forceLink  = kODFalse,
              forceEmbed = kODFalse;

    // get the content storage unit
    ODStorageUnit *renderedSU = DropContentSU( ev, dad, dropSU, kODTrue );
    if (!renderedSU)
      continue;

    // see if the user wanted to display the DropAs dialog
    if (attributes & kODDropIsPasteAs) {

      ODBoolean       result = kODFalse;
      ODPasteAsResult resultPasteAs ;

      TRY {
        // display the PasteAs dialog, see if the user wants to create a LINK
        result = dad->ShowPasteAsDialog( ev, kODTrue, kODPasteAsEmbed, facet,
                                         FRAME_VIEW, renderedSU, &resultPasteAs);
      }
      CATCH_ALL
      ENDTRY

      // did the user press CANCEL?
      if (!result) {
        dropResult = kODDropFail;
        return dropResult;
      }

      // did the user choose the "PASTECONTENT" button?
      if (resultPasteAs.pasteLinkSetting)
        forceLink = kODTrue;
      else {
        // workaround for defect 146831 where the mergeSetting is not being
        // returned correctly.  It appears that it is returning something
        // other than 0x01 in the faulty case (it should be 0).

//        forceEmbed = !resultPasteAs.mergeSetting;

        forceEmbed = resultPasteAs.mergeSetting != 0x01;
      }

    } else {

      simpleMove = (attributes & kODDropIsInSourceFrame) &&
                   (attributes & kODDropIsMove);
      forceLink = (attributes & kODDropIsLink);
    }

    // did we just drag things within our own part??
    if (simpleMove) {

      #ifdef ODDebug2
        somPrintf("GtkPart:Drop: simple content move\n");
      #endif

      // obj will be the object if only one dragged, NULL if multiple selection
      GtkObject* obj = _picture->HitObject();

      // get the world coords location of the cursor at drop
      GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &ptl );

      // do the move
      somSelf->InvalidateObject( ev, obj );
      _picture->Move( ptl.x-_fDragStart.x, ptl.y-_fDragStart.y, obj );
      somSelf->InvalidateObject( ev, obj );

      somSelf->PartUpdated( ev, 0, PUF_CLEARUNDO|PUF_CLIPEMBEDS|PUF_UPDATELINKS);

    // dragdrop from somewhere else, need to clone stuff in
    } else {

      ODBoolean dropOK = kODFalse;

      ODDraft*    fromDraft = renderedSU->GetDraft(ev);
      ODDraftKey  key;

      // GRAPHICS CONTENT   (and not forcing embed)??
      if ( !forceEmbed &&
           SetFocus( ev, renderedSU, kODPropContents, kKindGraphicsPart) ) {

        #ifdef ODDebug2
          somPrintf("GtkPart:Drop: incorporate graphics content\n");
        #endif

        dropOK = somSelf->ReadNativeContent(ev, frameDroppedIn, renderedSU, forceLink);

        if (!dropOK) {
          dropResult = kODDropFail;
          return dropResult;
        }

        // window coords of correct new LL
        if (SetFocus( ev, renderedSU, kPropMouseDownOffset, kGtkPOINTL)) {
          POINTL offset;
          StorageUnitGetValue( renderedSU, ev, sizeof(offset), (ODValue) &offset);
          ptl.x -= offset.x;
          ptl.y -= offset.y;
        }

        // move it to world coords
        GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &ptl );

        // selection area (world coords)
        // (after the ReadFrom, new stuff is the selected content)
        RECTL rclSelection;
        rclSelection = *_picture->SelectedBounds();

        // move the selection to the correct new position
        _picture->Move( ptl.x-rclSelection.xLeft, ptl.y-rclSelection.yBottom, 0 );

        somSelf->PartUpdated( ev, frameDroppedIn, PUF_CLEARUNDO | PUF_REPAINT | PUF_CLIPEMBEDS );

      }

      // BITMAP FILE   being dropped?
      if ( !dropOK && !forceEmbed &&
           SetFocus( ev, renderedSU, kODPropContents, kODFileType) ) {

        #ifdef ODDebug2
          somPrintf("GtkPart:Drop: try bitmap drop\n");
        #endif

        ULONG dataSize = renderedSU->GetSize( ev );
        CHAR  szFile[CCHMAXPATH];
        CHAR  szSubject[CCHMAXPATH];

        StorageUnitGetValue(renderedSU, ev, dataSize, (ODValue) szFile );

        szSubject[0] = '\0';
        if (SetFocus( ev, renderedSU, kODPropContents, kODFileTypeEA) ) {
          ULONG dataSize = renderedSU->GetSize( ev );
          StorageUnitGetValue(renderedSU, ev, dataSize, (ODValue) szSubject );

        } else DetermineSubjectFromExtension( szFile, szSubject );

        if ( !strcmp( szSubject, "Bitmap" )) {

          if (_hbmBackground)
            GpiDeleteBitmap(_hbmBackground);

          _hbmBackground = read_bitmap( _hpsMem, szFile );

          // successful read?
          if (_hbmBackground) {

            strcpy( _fBitmapFile, szFile );

            if (_fBGMode == BGM_COLOR) {
              _fMenuBar->CheckMenuItem( ev, IDMS_VIEW, VIEW_BGNDMODE_COLOR, kODFalse );
              _fBGMode = BGM_TILE;
              _fMenuBar->CheckMenuItem( ev, IDMS_VIEW, VIEW_BGNDMODE_TILED, kODTrue );
            }

            _fMenuBar->EnableMenuItem( ev, IDMS_VIEW, VIEW_BGNDMODE_TILED, kODTrue );
            _fMenuBar->EnableMenuItem( ev, IDMS_VIEW, VIEW_BGNDMODE_BLTFIT, kODTrue );

          } else {
            _fBGMode == BGM_COLOR;
          }

          frameDroppedIn->Invalidate(ev, kODNULL , kODNULL );

          // we handled it ok
          dropOK = kODTrue;
        }
      }

      // EMBED another part?
      if (!dropOK) {

        ODID  newPartID;

        #ifdef ODDebug2
          somPrintf("GtkPart:Drop: new embed\n");
        #endif

        try{
           key = fromDraft->BeginClone(ev, theDraft, kODNULL,
                   dropResult == kODDropMove ? kODCloneDropMove : kODCloneDropCopy );
           newPartID = fromDraft->Clone(ev, key, renderedSU->GetID(ev), 0, 0);
           fromDraft->EndClone(ev, key);
        } catch (... ) {
           fromDraft->AbortClone(ev, key);
           throw;
        } /* end catch */

        // 6/8/95  Drop offset fix
        ODPoint newTransPt = *where;

#ifdef  OLDMOUSEOFFSET
        if (SetFocus( ev, renderedSU, kPropMouseDownOffset, kODPoint)) {
#else
        if (SetFocus( ev, renderedSU, kODPropMouseDownOffset, kODPoint)) {
#endif
           ODPoint offset;
           StorageUnitGetValue( renderedSU, ev, sizeof(offset), (ODValue) &offset);
           newTransPt.x -= offset.x;
           newTransPt.y -= offset.y;
        }

        newExternalXForm = frameDroppedIn->CreateTransform(ev);
        newExternalXForm->MoveBy(ev,  &newTransPt );

        if (SetFocus( ev, renderedSU, kODPropFrameShape, NULL)) {
           newFrameShape = facet->CreateShape(ev);
           newFrameShape->ReadShape(ev, renderedSU);
        } else {
           newFrameShape = kODNULL;     // use default size {80,80}
        }

        // We SHOULD be internalizing the frame from the storage unit...

        TempODPart newPart = theDraft->AcquirePart(ev, newPartID);
        newFrame = somSelf->MakeEmbeddedFrame( ev, frameDroppedIn,
                                               newFrameShape, newExternalXForm,
                                               newPart, 0, kODFalse );

        somSelf->CreateEmbeddedFacets( ev, frameDroppedIn, newFrame );

        Proxy* p = somSelf->ProxyForFrame(ev, newFrame);
        GtkObject* obj = _picture->Object(p->id);

        if ( activeFrame && facet->GetWindow(ev)->IsActive(ev))
          somSelf->HighlightSelection(ev, facet, 0);         // erase old marks

//        _fAltMark = kODFalse;
        _picture->Select( obj );                           // select new embed

        RECTL rcl = obj->Bounds();                              // update clipping
        somSelf->ClipEmbeds( ev, facet, &rcl );

        somSelf->PartUpdated( ev, frameDroppedIn, PUF_CLEARUNDO | PUF_REPAINT );
      }
    }
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving Drop\n");
  #endif

  return dropResult;
}

// ------------------------------------------------------------------
//   General CONTAINER stuff                                      aaa
// ------------------------------------------------------------------

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope ODFrame*  SOMLINK GraphicsPartRequestEmbeddedFrame(GraphicsPart *somSelf,
                                                  Environment *ev,
                                                  ODFrame* containingFrame,
                                                  ODFrame* baseFrame,
                                                  ODShape* frameShape,
                                                  ODPart* embedPart,
                                                  ODTypeToken viewType,
                                                  ODTypeToken presentation,
                                                  ODBoolean isOverlaid)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","RequestEmbeddedFrame");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering RequestEmbeddedFrame\n");
  #endif

  ODFrame* embeddedFrame;
  ODRect rect;

  SOM_TRY

  // if no frame shape requested, lets copy and trim one
  if (baseFrame) {   // !frameShape) {
    frameShape = ODCopyAndRelease(ev, baseFrame->AcquireFrameShape(ev, kODNULL));
    frameShape->GetBoundingBox(ev, &rect);
    rect.bottom = rect.top - MAKEFIXED(30,0);
    frameShape->SetRectangle(ev, &rect);
  } else {
    frameShape->GetBoundingBox(ev, &rect);
  }

  // now for the external transform... lets align it next to the baseFrame
  ODTransform* externalXForm = containingFrame->CreateTransform(ev);

  if (baseFrame) {
    ODPoint ptTemp={ (rect.right-rect.left) + MAKEFIXED(20,0), 0 };
    externalXForm->CopyFrom(ev, somSelf->ProxyForFrame(ev, baseFrame)->transform);
    externalXForm->MoveBy(ev, &ptTemp);
  } else {
    ODSLong count = 1 + _fContents->Count();
    ODPoint ptTemp = {MAKEFIXED(count*20,0),MAKEFIXED(count*20,0)};
    externalXForm->SetOffset(ev, &ptTemp);
  }

  embeddedFrame = somSelf->MakeEmbeddedFrame( ev, containingFrame, frameShape,
                                              externalXForm, embedPart,
                                              0, kODFalse );

  somSelf->CreateEmbeddedFacets( ev, containingFrame, embeddedFrame );

  somSelf->PartUpdated( ev, 0, PUF_REPAINTALL | PUF_CLIPEMBEDS );

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving RequestEmbeddedFrame\n");
  #endif

  return embeddedFrame;
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartRemoveEmbeddedFrame(GraphicsPart *somSelf,
                                             Environment *ev,
                                            ODFrame* embeddedFrame)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","RemoveEmbeddedFrame");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering RemoveEmbeddedFrame\n");
  #endif

  SOM_TRY

    if (_fEmbeddedFrames->Contains(embeddedFrame))
    {
      TempODFrame containingFrame = embeddedFrame->AcquireContainingFrame(ev);
      Proxy* p = somSelf->ProxyForFrame(ev, embeddedFrame);
      if (p)
      {
        _fContents->Remove(p);
        ODDeleteObject(p->transform);
        delete p;
      }
      _fEmbeddedFrames->Remove(embeddedFrame);
      embeddedFrame->Remove(ev);
      //somSelf->GetStorageUnit(ev)->GetDraft(ev)->RemoveFrame(ev, embeddedFrame);

      somSelf->ClipEmbeddedFrames(ev, containingFrame);
    }
    else
      THROW(kODErrInvalidFrame);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving RemoveEmbeddedFrame\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartRevealFrame(GraphicsPart *somSelf,  Environment *ev,
                                    ODFrame* embeddedFrame, ODShape* revealShape)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","RevealFrame");

  #ifdef ODDebug2
    somPrintf("GtkPart: null RevealFrame\n");
  #endif

  return kODFalse;
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartEmbeddedFrameSpec(GraphicsPart *somSelf,
                                           Environment *ev, ODFrame* embeddedFrame,
                                          ODObjectSpec* spec)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","EmbeddedFrameSpec");

  #ifdef ODDebug2
    somPrintf("GtkPart: null EmbeddedFrameSpec\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope ODEmbeddedFramesIterator*  SOMLINK GraphicsPartCreateEmbeddedFramesIterator(GraphicsPart *somSelf,
                                                                           Environment *ev,
                                                                          ODFrame* frame)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","CreateEmbeddedFramesIterator");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering CreateEmbeddedFramesIterator\n");
  #endif

               // todo: this method is required to set up the iterator so that
               //   it only iterates though the children of the given
               //   frame.  Clearly this code it is not doing this...
               //   or is assuming that a single display frame will
               //   be created.  The Apple code is no better yet.
               //   (jlc 94-9-16)

    XEmbeddedFramesIterator * xefi = new XEmbeddedFramesIterator();

  SOM_TRY

    xefi->InitXEmbeddedFramesIterator( ev, _fEmbeddedFrames );

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving CreateEmbeddedFramesIterator\n");
  #endif

    return xefi;
}

// this is a notification that the contents of an embedded frame have changed
// (in case it is is included in a link source)
//
// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartEmbeddedFrameUpdated(GraphicsPart *somSelf,
                                              Environment *ev,
                                             ODFrame* frame,
                                             ODUpdateID change)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","EmbeddedFrameUpdated");

  #ifdef ODDebug2
    somPrintf("GtkPart: null EmbeddedFrameUpdated\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope ODShape*  SOMLINK GraphicsPartAdjustBorderShape(GraphicsPart *somSelf,
                                               Environment *ev,
                                              ODFacet* embeddedFacet,
                                              ODShape* shape)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","AdjustBorderShape");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering AdjustBorderShape\n");
  #endif

  SOM_TRY

  ODFacet* containingFacet = embeddedFacet->GetContainingFacet(ev);
  ODFacetIterator* facets = containingFacet->CreateFacetIterator(ev, kODChildrenOnly, kODFrontToBack);

  TempODTransform transform = embeddedFacet->AcquireFrameTransform(ev, kODNULL);
  shape->Transform(ev, transform);

  for (ODFacet* embFacet = facets->First(ev);
        facets->IsNotComplete(ev) && embFacet != embeddedFacet;
        embFacet = facets->Next(ev))
  {
     TempODShape usedShape = ODCopyAndRelease(ev, embFacet->GetFrame(ev)->AcquireUsedShape(ev, kODNULL));
     TempODTransform xform = embFacet->AcquireFrameTransform(ev, kODNULL);
     usedShape->Transform(ev, xform);

     shape->Subtract(ev, usedShape);
  }
  delete facets;

  shape->InverseTransform(ev, transform);

  shape->Acquire(ev);

  SOM_CATCH_ALL
  SOM_ENDTRY

  return shape;

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving AdjustBorderShape\n");
  #endif

    return shape;
}

// ------------------------------------------------------------------
//   General FRAME management                                     aaa
// ------------------------------------------------------------------

// -- BASE method  (minor mods) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDisplayFrameAdded(GraphicsPart *somSelf,
                                         Environment *ev, ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DisplayFrameAdded");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DisplayFrameAdded\n");
  #endif

  SOM_TRY

  TempODPart thePart = frame->AcquirePart(ev);

  if (thePart == _fPartWrapper)    // frame belongs to me
  {
if (_fDisplayFrames->Count()==0) {
    ODxOrderedCollectionIterator  displayFramesIter(_fDisplayFrames);
    ODFrame* displayFrame = (ODFrame*) displayFramesIter.First();
    if (displayFramesIter.IsNotComplete() == kODFalse) {
      ODxOrderedCollectionIterator  contentsIter(_fContents);
      Proxy*            proxy;

      for (proxy = (Proxy*) contentsIter.First();
          contentsIter.IsNotComplete();
          proxy = (Proxy*) contentsIter.Next())
      {
        proxy->frame->SetContainingFrame(ev, frame);
      }
    }
} /* endif */

    // !!! do something with viewType and partInfo...
    PartInfoRec* pInfo = new PartInfoRec;
    pInfo->partwindowID = (ODID)kODNULL;
    if (frame->IsRoot(ev))
      pInfo->fNeedsActivating = kODTrue;
    frame->SetPartInfo(ev, (ODInfoType) pInfo);

    _fDisplayFrames->AddLast(frame);
    frame->Acquire(ev);
    frame->SetDroppable(ev, kODTrue);

#if 0
// you cannot do this here - your container has not gotten back your
// frame reference yet and cannot do the de-referencing to find out
// who you are or even tell that you are an embedded frame.  MUST defer
// this to the facetAdded method (just as you do for frame negotiation)
    // if this is our first frame, check for bgcolor coordination
    if ( _fMatchBGColor && _fDisplayFrames->Count()==1 ) {

      // if we are root we'll get null for a containing frame..
      TempODFrame containingFrame = frame->AcquireContainingFrame(ev);
      if (containingFrame) {
        TempODPart containingPart = containingFrame->AcquirePart(ev);
        TempODStorageUnit propSU = containingPart->AcquireContainingPartProperties(ev, frame);

        if (SetFocus( ev, propSU, kODBackgroundColor, kODRGB2) )
          StorageUnitGetValue(propSU, ev, sizeof(_fBGColor), &_fBGColor);
      }
    }
#endif
    somSelf->PartUpdated( ev, 0, 0 );

    if (frame->GetViewType(ev) == kODNullTypeToken)   // if frame view is set don't change it
      frame->SetViewType(ev, FRAME_VIEW);             // if not, make it viewasframe
    if (frame->GetPresentation(ev) == kODNullTypeToken)
      frame->SetPresentation(ev, PRES_DEFAULT);
  }
  else
    THROW(kODErrInvalidFrame);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DisplayFrameAdded\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartAttachSourceFrame(GraphicsPart *somSelf,
                                           Environment *ev, ODFrame* frame,
                                          ODFrame* sourceFrame)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","AttachSourceFrame");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering AttachSourceFrame\n");
  #endif

#if 0
  SOM_TRY

    if (_fDisplayFrames->Contains(frame)
       && sourceFrame && _fDisplayFrames->Contains(sourceFrame))
    {
       ODULong seqnumber = sourceFrame->GetSequenceNumber(ev);    //[pfe] 123945
       ODULong framegroup = sourceFrame->GetFrameGroup(ev);       //[pfe] 123945
       frame->ChangeSequenceNumber(ev,seqnumber);                 //[pfe] 123945
       frame->SetFrameGroup(ev,framegroup+1);                     //[pfe] 123945

      //RCR Note. DrawPart does not currently store embedded
      // frames on a per-containing-frame basis (eg. in the part info
      // of each frame). Since we can't add to the embedded frames list
      // while iterating over it, we make a temporary list here

        ODxOrderedCollection* embeddedFrames = new ODxOrderedCollection;
        {
          ODxOrderedCollectionIterator iter(_fEmbeddedFrames);
          for (ODFrame* embeddedFrame = (ODFrame*) iter.First();
              iter.IsNotComplete();
              embeddedFrame = (ODFrame*) iter.Next())
          {
            embeddedFrames->AddFirst(embeddedFrame);
          }
        }

        ODxOrderedCollectionIterator iter(embeddedFrames);
        for (ODFrame* embeddedFrame = (ODFrame*) iter.First();
            iter.IsNotComplete();
            embeddedFrame = (ODFrame*) iter.Next())
        {
          TempODFrame containingFrame = embeddedFrame->AcquireContainingFrame(ev);
          if (containingFrame == sourceFrame)
          {
            TempODPart embeddedPart = embeddedFrame->AcquirePart(ev);
            TempODShape frameShape = embeddedFrame->AcquireFrameShape(ev, kODNULL);
            TempODShape newShape = frameShape->Copy(ev);
            TempODTransform newXForm = somSelf->ProxyForFrame(ev, embeddedFrame)->transform->Copy(ev);

            ODFrame* newFrame = somSelf->MakeEmbeddedFrame(ev, frame,
                newShape, newXForm, embeddedPart, 0,
                embeddedFrame->IsOverlaid(ev));

            embeddedPart->AttachSourceFrame(ev, newFrame, embeddedFrame);

            ODULong seqnum = embeddedFrame->GetSequenceNumber(ev); //[pfe] 123945
            ODULong framegrp = embeddedFrame->GetFrameGroup(ev);   //[pfe] 123945
            newFrame->SetFrameGroup(ev,framegrp+1);                //[pfe] 123945
            newFrame->ChangeSequenceNumber(ev,seqnum);             //[pfe] 123945
          }
        }
        delete embeddedFrames; // Delete the copy
      }
    else
      THROW(kODErrInvalidFrame);

  SOM_CATCH_ALL
  SOM_ENDTRY

#endif
  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving AttachSourceFrame\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDisplayFrameRemoved(GraphicsPart *somSelf,
                                                          Environment *ev,
                                                         ODFrame* frame)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","DisplayFrameRemoved");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering D\n");
  #endif

  SOM_TRY

    if (frame != kODNULL ) {
      if (_fDisplayFrames->Contains(frame))
      {

        if (_fEmbeddedFrames->Count()) {

          ODxOrderedCollection* embeddedFrames = new ODxOrderedCollection;
          {
            ODxOrderedCollectionIterator iter(_fEmbeddedFrames);
            for (ODFrame* embeddedFrame = (ODFrame*) iter.First();
                iter.IsNotComplete();
                embeddedFrame = (ODFrame*) iter.Next())
            {
              embeddedFrames->AddLast(embeddedFrame);
            }
          }
          ODxOrderedCollectionIterator iter(embeddedFrames);
          for (ODFrame* embeddedFrame = (ODFrame*) iter.First();
              iter.IsNotComplete();
              embeddedFrame = (ODFrame*) iter.Next())
          {
            TempODFrame containingFrame = embeddedFrame->AcquireContainingFrame(ev);
            if (containingFrame == frame)
            {
              somSelf->RemoveEmbeddedFrame(ev, embeddedFrame);
            }
          }

          delete embeddedFrames; // Delete the copy
        }

//[pfe] 123945 begin *************
        ODxOrderedCollection *displayFrames = new ODxOrderedCollection;
        ODxOrderedCollectionIterator iter1(_fDisplayFrames);
        for (ODFrame* dispframe = (ODFrame*) iter1.First();
            iter1.IsNotComplete();
            dispframe = (ODFrame*) iter1.Next())
        {
           displayFrames->AddFirst(dispframe);     //copy of _fDisplayFrames
        }

        ODxOrderedCollectionIterator iter2(displayFrames);     //iterate to find partwindow
        for (ODFrame* dispFrame = (ODFrame*) iter2.First();    //display frame
            iter2.IsNotComplete();
            dispFrame = (ODFrame*) iter2.Next())
        {
           TempODFrame containingFrame = dispFrame->AcquireContainingFrame(ev);
           if ((dispFrame != frame) && (containingFrame == kODNULL))
           {
              TempODWindow partwindow = dispFrame->AcquireWindow(ev);
              if (partwindow && !partwindow->IsRootWindow(ev))  // [128494] - ced
              {
                partwindow->CloseAndRemove(ev);
              }
           }
        }
        delete displayFrames;

//[pfe] 123945 end *************
        _fSession->GetArbitrator(ev)->RelinquishFocusSet(ev, _fFocusSet,frame);

        ODFrame* tmpFrame = _fSession->GetArbitrator(ev)->AcquireFocusOwner(ev, _fClipboardFocus);
        if ( frame == tmpFrame )
        {
          _fSession->GetArbitrator(ev)->RelinquishFocus(ev, _fClipboardFocus, frame);
          somSelf->FocusLost(ev, _fClipboardFocus, frame);
        }
        ODReleaseObject(ev, tmpFrame);

        tmpFrame = _fSession->GetArbitrator(ev)->AcquireFocusOwner(ev, _fStatusFocus);
        if ( frame == tmpFrame ) // if have focus release it.
        {
           somSelf->FocusLost(ev, _fStatusFocus, frame);
        }
        ODReleaseObject(ev, tmpFrame);

        PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);
        delete pInfo;
        _fDisplayFrames->Remove(frame);

        // any display frames left?
        if (_fDisplayFrames->Count()==0) {

          // have we put a link spec on the clipboard this session??
          if (_fClipboardChangeID)
            ClearMyLinkSpec( ev, _fSession, frame, &_fClipboardChangeID );
        }

        ODReleaseObject(ev, frame);
      }
      else
        THROW(kODErrInvalidFrame);
    }

  SOM_CATCH_ALL
  SOM_ENDTRY
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDisplayFrameConnected(GraphicsPart *somSelf,
                                                            Environment *ev,
                                                           ODFrame* frame)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","DisplayFrameConnected");

  SOM_TRY

  // $$$$$ add frame to fDisplayFrames if reference lazily internalized
// 137097 - aml
  if ( !_fDisplayFrames->Contains(frame) )
  {
    _fDisplayFrames->AddLast(frame);
    frame->Acquire(ev);
    frame->SetDroppable(ev, kODTrue);

    // $$$$$ If any of our embedded frames had no containing frame, hook them up now
    ODxOrderedCollectionIterator bIter(_fEmbeddedFrames);
    for (ODFrame* embeddedFrame = (ODFrame*)bIter.First(); bIter.IsNotComplete(); embeddedFrame = (ODFrame*)bIter.Next())
    {
      TempODFrame tempFrame = embeddedFrame->AcquireContainingFrame(ev);
      if ( tempFrame == kODNULL )
      {
        embeddedFrame->SetContainingFrame(ev, frame);
      }
    }
  }

  PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);
  if (frame->IsRoot(ev))
    pInfo->fNeedsActivating = kODTrue;
// end 137097

#if 0
// you cannot do this here - your container has not gotten back your
// frame reference yet and cannot do the de-referencing to find out
// who you are or even tell that you are an embedded frame.  MUST defer
// this to the facetAdded method (just as you do for frame negotiation)
  // if this is our first frame, check for bgcolor coordination
  if ( _fMatchBGColor && _fDisplayFrames->Count()==1 ) {

    // if we are root we'll get null for a containing frame..
    TempODFrame containingFrame = frame->AcquireContainingFrame(ev);
    if (containingFrame) {
      TempODPart containingPart = containingFrame->AcquirePart(ev);
      TempODStorageUnit propSU = containingPart->AcquireContainingPartProperties(ev, frame);

      if (SetFocus( ev, propSU, kODBackgroundColor, kODRGB2) )
        StorageUnitGetValue(propSU, ev, sizeof(_fBGColor), &_fBGColor);
    }
  }
#endif

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: null DisplayFrameConnected\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDisplayFrameClosed(GraphicsPart *somSelf,
                                                         Environment *ev,
                                                        ODFrame* frame)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","DisplayFrameClosed");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DisplayFrameClosed\n");
  #endif

  SOM_TRY

    if (_fDisplayFrames->Contains(frame))
    {

      if (_fEmbeddedFrames->Count()) {

        ODxOrderedCollection* embeddedFrames = new ODxOrderedCollection;
        {
          ODxOrderedCollectionIterator iter(_fEmbeddedFrames);
          for (ODFrame* embeddedFrame = (ODFrame*) iter.First();
              iter.IsNotComplete();
              embeddedFrame = (ODFrame*) iter.Next())
          {
            embeddedFrames->AddLast(embeddedFrame);
          }
        }

        ODxOrderedCollectionIterator iter(embeddedFrames);
        for (ODFrame* embeddedFrame = (ODFrame*) iter.First();
            iter.IsNotComplete();
            embeddedFrame = (ODFrame*) iter.Next())
        {
          TempODFrame containingFrame = embeddedFrame->AcquireContainingFrame(ev);
          if (containingFrame == frame)
          {
            embeddedFrame->Close(ev);
            _fEmbeddedFrames->Remove(embeddedFrame);
          }
        }

        delete embeddedFrames; // Delete the copy
      }

      _fSession->GetArbitrator(ev)->RelinquishFocusSet(ev, _fFocusSet, frame);

      TempODFrame tmpFrame = _fSession->GetArbitrator(ev)->AcquireFocusOwner(ev, _fClipboardFocus);
      if ( frame == tmpFrame )
      {
        _fSession->GetArbitrator(ev)->RelinquishFocus(ev, _fClipboardFocus, frame);
        somSelf->FocusLost(ev, _fClipboardFocus, frame);
      }

      PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);
      // don't do the Set 'cause it marks the frame "dirty"...
//      frame->SetPartInfo(ev, (ODInfoType) kODNULL);
      delete pInfo;

      _fDisplayFrames->Remove(frame);

      // any display frames left?
      if (_fDisplayFrames->Count()==0) {

        // have we put a link spec on the clipboard this session??
        if (_fClipboardChangeID)
          ClearMyLinkSpec( ev, _fSession, frame, &_fClipboardChangeID );
      }

      ODReleaseObject(ev,frame);
    }
    else
      THROW(kODErrInvalidFrame);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DisplayFrameClosed\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope ODExtension*  SOMLINK GraphicsPartAcquireExtension(GraphicsPart *somSelf,
                                                           Environment *ev,
                                                          ODType extensionName)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","AcquireExtension");

  #ifdef ODDebug2
    somPrintf("GtkPart: null AcquireExtension\n");
  #endif

  ODExtension* theExtension;

  SOM_TRY

  if(!strcmp(extensionName, kODSettingsExtension) ){

    // create it if we don't already have one
    if (!_fNotebook) {
      _fNotebook = new GtkNotebook;
      _fNotebook->InitSettingsExtension( ev, _fPartWrapper);
    }

    // incr ref count
    _fNotebook->Acquire(ev);
    theExtension = _fNotebook;
  }
  else {
    theExtension = GraphicsPart_parent_ODPart_AcquireExtension(somSelf, ev, extensionName);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return theExtension;
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartReleaseExtension(GraphicsPart *somSelf,
                                                       Environment *ev,
                                                      ODExtension* extension)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","ReleaseExtension");

  #ifdef ODDebug2
    somPrintf("GtkPart: doing ReleaseExtension\n");
  #endif

  SOM_TRY

  if ( !extension ) return;

  if(_fNotebook == extension) {
     ODReleaseObject(ev, _fNotebook);
  }
  else {
     GraphicsPart_parent_ODPart_ReleaseExtension(somSelf, ev, extension);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartHasExtension(GraphicsPart *somSelf,
                                                        Environment *ev,
                                                       ODType extensionName)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","HasExtension");

  #ifdef ODDebug2
    somPrintf("GtkPart: null HasExtension\n");
  #endif

  ODBoolean rc;

  SOM_TRY

    if( !strcmp(extensionName, kODSettingsExtension) ) {
       rc = kODTrue;
    }
    else {
       rc = GraphicsPart_parent_ODPart_HasExtension(somSelf, ev, extensionName);
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return rc;
}

// -- BASE method  (MODIFIED) ------------------------------------
// modified properties are being passed down the heirarchy...
// inspect the ones we are interested in and then pass them along.
//----------------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartContainingPartPropertiesUpdated(GraphicsPart *somSelf,
                                                     Environment *ev, ODFrame* frame,
                                                     ODStorageUnit* propertyUnit)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","ContainingPartPropertiesUpdated");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering ContainingPartPropertiesUpdated\n");
  #endif

  SOM_TRY

  // is the background color in there???
  if (SetFocus( ev, propertyUnit, kODBackgroundColor, kODRGB2) ) {
    RGBColor rgb;
    StorageUnitGetValue(propertyUnit, ev, sizeof(RGBColor), &rgb);

    // are we doing background color matching?  (its the default)
    if (_fMatchBGColor) {

      // change it (don't propagate here, done below)
      somSelf->SetBGColor(ev, frame, rgb, kODFalse);

    // NO, keep it clean.
    } else {

      // are we re-synching our backgrounds?
      if (rgb == _fBGColor) {
        _fMatchBGColor = kODTrue;
      }

      // if this is the only property in the su, abort here so as to NOT propogate it
      if (propertyUnit->CountProperties(ev) == 1) {
        return;

      // otherwise remove this property and leave the rest
      } else {
        propertyUnit->Focus(ev, kODBackgroundColor, 0, 0, 0, kODPosUndefined);
        propertyUnit->Remove(ev);
      }
    }
  }

  // is the background transparency in there?
  if (SetFocus( ev, propertyUnit, kODBackgroundTransparency, kODBoolean )) {
    ODBoolean fTransparent = kODFalse;
    StorageUnitGetValue(propertyUnit, ev, sizeof(fTransparent), &fTransparent );

    if (fTransparent) {
      if (_fBGMode != BGM_TRANSPARENT) {
        _fBGMode = BGM_TRANSPARENT;
        somSelf->PartUpdated( ev, frame, PUF_REPAINTALL );
      }
    } else {
      if (_fBGMode == BGM_TRANSPARENT) {
        _fBGMode = BGM_COLOR;
        somSelf->PartUpdated( ev, frame, PUF_REPAINTALL );
      }
    }
  }

  // now pass 'em along to embeds
  ODFrame *subframe = (ODFrame*)_fEmbeddedFrames->First();
  while (subframe) {
     TempODPart embeddedPart = subframe->AcquirePart(ev);
     if (embeddedPart) {
        embeddedPart->ContainingPartPropertiesUpdated(ev, subframe, propertyUnit);
     }
     subframe = (ODFrame*)_fEmbeddedFrames->After(subframe);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving ContainingPartPropertiesUpdated\n");
  #endif
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope ODStorageUnit*  SOMLINK GraphicsPartAcquireContainingPartProperties(GraphicsPart *somSelf,
                                                               Environment *ev,
                                                              ODFrame* embedFrame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","AcquireContainingPartProperties");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering AcquireContainingPartProperties\n");
  #endif

  ODStorageUnit* suProps = kODNULL;

  SOM_TRY

  if (_fReadOnly)
    return kODNULL;

  // verify that we have a proxy for this embedFrame
  Proxy* p = somSelf->ProxyForFrame(ev, embedFrame);
  if (!p)
    return kODNULL;

  TempODFrame myFrame = embedFrame->AcquireContainingFrame(ev);

  // sanity check
  if (!_fDisplayFrames->Contains(myFrame)) {
    return kODNULL;
  }

  if (!myFrame->IsRoot(ev)) {
    TempODFrame containingFrame = myFrame->AcquireContainingFrame(ev);
    TempODPart containingPart = containingFrame->AcquirePart(ev);
    suProps = containingPart->AcquireContainingPartProperties(ev, myFrame);
  }

  if (!suProps) {
    suProps = somSelf->GetStorageUnit(ev)->GetDraft(ev)->CreateStorageUnit(ev);
  }

  // default to leave this value alone
  ODBoolean writeit = FALSE;

  // if the std prop isn't there, create it and we write it
  if (!SetFocus( ev, suProps, kODBackgroundColor, kODRGB2) ) {
    InitPropVal( ev, suProps, kODBackgroundColor, kODRGB2 );
    writeit = kODTrue;
  }

  // if we created the prop OR we are not synched with our container... write our color
  if (writeit || !_fMatchBGColor ) {
    StorageUnitSetValue(suProps, ev, sizeof(RGBColor), &_fBGColor);
  }

  // get the gtkObject for this embedded frame
  GtkEmbed* embedObj = (GtkEmbed*)_picture->Object(p->id);

  // verify the object type
  if ( embedObj->IsEmbed() ) {
    // get the value
    ODBoolean fTransparency = embedObj->IsTranslucent();

    // create the bin and write the embed's transparency state
    InitPropVal( ev, suProps, kODBackgroundTransparency, kODBoolean );
    StorageUnitSetValue( suProps, ev, sizeof(fTransparency), (ODValue)&fTransparency );
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving AcquireContainingPartProperties\n");
  #endif

  // return the filled storage unit
  return suProps;
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartFrameShapeChanged(GraphicsPart *somSelf,
                                           Environment *ev, ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","FrameShapeChanged");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering FrameShapeChanged\n");
  #endif

  SOM_TRY

  if (_fDisplayFrames->Contains(frame)) {

    if ( !_fReadOnly ) {
      somSelf->Resize( ev, frame, 0 );

      somSelf->PartUpdated( ev, 0, PUF_CLIPEMBEDS );

      if (frame->IsRoot(ev))
        somSelf->UpdatePalette(ev, frame);
    }

  } else
    THROW(kODErrInvalidFrame);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving FrameShapeChanged\n");
  #endif
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartViewTypeChanged( GraphicsPart *somSelf,
                                                     Environment *ev,
                                                     ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","ViewTypeChanged");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering ViewTypeChanged\n");
  #endif

  SOM_TRY

  // get the new view type
  ODTypeToken curView = frame->GetViewType(ev);

  ODULong cx, cy;

  if ( curView == THUMBNAIL_VIEW ) {

    if (!_hbmThumbnail) {
      somSelf->CreateThumbnail(ev, frame );
      if (!_hbmThumbnail) {
        // cannot create thumbnail???
        DosBeep(220,100);
        frame->SetViewType(ev, FRAME_VIEW);
        return;
      }
    }

    cx = cy = THUMBSIZE;

  } else if ( curView == LARGE_ICON_VIEW  || curView == SMALL_ICON_VIEW ) {

    if ( !_icon ) {
      _icon = new GtkPointer();
      if ( !_icon->Init( _hmod, ID_PARTICON ) ) {
        // cannot load icons???
        DosBeep(220,100);
        frame->SetViewType(ev, FRAME_VIEW);
        return;
      }
    }

    // select the right sub-bitmap for this view
    _icon->Select( (curView == LARGE_ICON_VIEW) ? IT_LARGEICON
                                                : IT_SMALLICON );
    // adjust the frame size
    cx = _icon->Cx();
    cy = _icon->Cy();

  } else if ( curView != FRAME_VIEW ) {

    // bad view type, reset to FrameView
    DosBeep(220,100);
    frame->SetViewType(ev, FRAME_VIEW);

    return;
  }

  ODFacet* facet = FirstFacet(ev,frame);
  if (!facet) return;

  ODCanvas* canvas = facet->GetCanvas(ev);
  // get the old usedShape and save it for invalidShape for repaint
  ODShape* invalShape = kODNULL;
  invalShape = frame->AcquireFrameShape(ev, canvas);

  if ( curView == FRAME_VIEW ) {

    // reset so that these mirror the FrameShape
    frame->ChangeUsedShape(ev, kODNULL, canvas );
    facet->ChangeActiveShape(ev, kODNULL, canvas );

    // need this in case frame was resized while in other view...
//    somSelf->ClipEmbeds( ev, facet, 0 );

  } else {

    // frame rcl in screen coords
    RECTL rclFrame, rclTmp;
    rclFrame.xLeft   = _frameOffset.x;
    rclFrame.yBottom = _frameOffset.y;
    rclFrame.xRight  = rclFrame.xLeft   + cx;
    rclFrame.yTop    = rclFrame.yBottom + cy;

    // create a shape for it
    TempODShape newUsedShape = frame->CreateShape(ev);
    HRGN frameRgn = GpiCreateRegion( _hpsMem, 1, &rclFrame );
    newUsedShape->SetRegion(ev, frameRgn);           // SHAPE OWNS THE REGION NOW!

    // put it in frame coords
    TempODTransform xform = ODCopyAndRelease(ev, facet->AcquireWindowFrameTransform(ev, canvas));
    xform->Invert(ev);
    newUsedShape->Transform(ev, xform);

    // calc the net invalid shape
//    invalShape = ODCopyAndRelease(ev, frame->AcquireUsedShape(ev, canvas) );
//    invalShape->Union(ev, newUsedShape);

    // update the used shape
    frame->ChangeUsedShape(ev, newUsedShape, canvas);

    // update the active shape
    if (canvas->IsDynamic(ev)) {
      TempODShape aShape = newUsedShape->Copy(ev);
      facet->ChangeActiveShape(ev, aShape, canvas);
    }
  }

  // invalidate union of old and new used shapes
  frame->Invalidate(ev, invalShape, canvas );

  ODReleaseObject(ev, invalShape);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving ViewTypeChanged\n");
  #endif
}

// -- BASE method  (un-MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartPresentationChanged( GraphicsPart *somSelf,
                                                         Environment *ev,
                                                         ODFrame* frame)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","PresentationChanged");

  #ifdef ODDebug2
    somPrintf("GtkPart: null PresentationChanged\n");
  #endif

    if (_fDisplayFrames->Contains(frame)) {
      { NOTDONE2("PresentationChanged"); /* !!! change presentation of frame */ }
    } else
      THROW(kODErrInvalidFrame);
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartSequenceChanged(GraphicsPart *somSelf,
                                         Environment *ev, ODFrame* frame)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","SequenceChanged");

  #ifdef ODDebug2
    somPrintf("GtkPart: null SequenceChanged\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartWritePartInfo(GraphicsPart *somSelf,
                                       Environment *ev, ODInfoType partInfo,
                                      ODStorageUnitView* storageUnitView)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","WritePartInfo");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering WritePartInfo\n");
  #endif

  SOM_TRY

    if (partInfo)
    {
      ODStorageUnit* su = storageUnitView->GetStorageUnit(ev);
      ODPropertyName propName = storageUnitView->GetProperty(ev);

      ODSUForceFocus(ev, su, propName, kKindGraphicsPart);

      ODBoolean needsActivating = ((PartInfoRec*)partInfo)->fNeedsActivating
                                          || ((PartInfoRec*)partInfo)->fIsActive;
      StorageUnitSetValue( su, ev, sizeof(ODBoolean), (ODValue)&needsActivating);

      StorageUnitSetValue( su, ev, sizeof(RECTL), (ODValue)&_rclFrame);
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving WritePartInfo\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope ODInfoType  SOMLINK GraphicsPartReadPartInfo(GraphicsPart *somSelf,
                                       Environment *ev, ODFrame* frame,
                                      ODStorageUnitView* storageUnitView)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","ReadPartInfo");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering ReadPartInfo\n");
  #endif

  PartInfoRec* partInfo = new PartInfoRec;

  SOM_TRY

    ODStorageUnit* su = storageUnitView->GetStorageUnit(ev);
    ODPropertyName propName = storageUnitView->GetProperty(ev);

    if ( SetFocus( ev, su, propName, kKindGraphicsPart) ) {

      ODBoolean needsActivating;
      StorageUnitGetValue( su, ev, sizeof(ODBoolean), (ODValue)&(needsActivating));
      partInfo->fNeedsActivating = needsActivating;

      StorageUnitGetValue( su, ev, sizeof(RECTL), (ODValue)&_rclFrame);
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving ReadPartInfo\n");
  #endif

    return (ODInfoType) partInfo;
}

SOM_Scope void  SOMLINK GraphicsPartClonePartInfo( GraphicsPart *somSelf, Environment *ev,
                                                   ODDraftKey key, ODInfoType partInfo,
                                                   ODStorageUnitView* storageUnitView,
                                                   ODFrame* scopeFrame )
{
    GraphicsPartData *somThis =  GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart", "ClonePartInfo");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering ClonePartInfo\n");
  #endif

  SOM_TRY

  if (partInfo) {
    ODStorageUnit* su = storageUnitView->GetStorageUnit(ev);
    ODPropertyName propName = storageUnitView->GetProperty(ev);

    ODSUForceFocus(ev, su, propName, kKindGraphicsPart);

    ODBoolean needsActivating = ((PartInfoRec*)partInfo)->fNeedsActivating
                                         || ((PartInfoRec*)partInfo)->fIsActive;
    StorageUnitSetValue( su, ev, sizeof(ODBoolean), (ODValue)&needsActivating);

    StorageUnitSetValue( su, ev, sizeof(RECTL), (ODValue)&_rclFrame);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving ClonePartInfo\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope ODID  SOMLINK GraphicsPartOpen(GraphicsPart *somSelf,  Environment *ev,
                             ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","Open");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering Open\n");
  #endif

  ODWindow* window = kODNULL;
  ODID windowID = 0;

  SOM_TRY

  if (frame) // Doing a View As Window or Open Root
  {
    if (frame->IsRoot(ev))  // Create Window For Root Frame
    {
       WindowProperties props;
       BeginGetWindowProperties(ev, frame, &props);

       ODULong flCreateFlags = props.createFlags ? props.createFlags :
                                                     ODPlatformWindowDefaultCreateOptions;

       HWND hwndFrame = _fSession->CreatePlatformWindow(ev, flCreateFlags);
       // position the window to shape it was closed in
       // defect 117197 open from file in some position/properties as was saved
       WinSetWindowPos(hwndFrame, HWND_TOP, props.boundsRect.xLeft,
                                         props.boundsRect.yBottom,
                                         props.boundsRect.xRight,
                                         props.boundsRect.yTop,(SWP_SIZE | SWP_MOVE|props.swpFlags ));
       // defect 117197 end code added

       window =  _fSession->GetWindowState(ev)->RegisterWindowForFrame(ev, hwndFrame,
                                           frame,
                                            props.isRootWindow, // Keeps draft open
                                            kODTrue,   // Is resizable
                                            kODFalse,  // Is floating
                                           kODTrue, // should save
                                           kODTrue, // should dispose
                                           props.sourceFrame);
       EndGetWindowProperties(ev, &props); // Release source frame
       window->Open(ev);
       window->Show(ev);
    }
    else  // View In Window
    {
       window = _fSession->GetWindowState(ev)->AcquireWindow(ev, _fWindowID);
       if (window)
          window->Select(ev);
       else
       {
          window = somSelf->CreateWindow(ev, frame);
          _fWindowID = window->GetID(ev);
          window->Open(ev);
          window->Show(ev);
          window->Select(ev);
       }
    }
  }
  else
  {
    // try and abort any attempt at OpenAsWindow???
    #if 0
      return 0;
    #else
      window = somSelf->CreateWindow(ev, frame);
      _fWindowID = window->GetID(ev);
      window->Open(ev);
      window->Show(ev);
      window->Select(ev);
    #endif
  }

  windowID = window->GetID(ev);
  ODReleaseObject(ev, window);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving Open\n");
  #endif

    return windowID;
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope ODShape*  SOMLINK GraphicsPartRequestFrameShape(GraphicsPart *somSelf,
                                               Environment *ev,
                                              ODFrame* embeddedFrame,
                                              ODShape* frameShape)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","RequestFrameShape");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering RequestFrameShape\n");
  #endif

  SOM_TRY

  if (_fEmbeddedFrames->Contains(embeddedFrame)) {

    Proxy* p = somSelf->ProxyForFrame(ev, embeddedFrame);

    if (p) {

      // update bounds of associated Object
      GtkObject* obj = _picture->Object(p->id);

      if (obj) {

        somSelf->InvalidateObject( ev, obj );

        TempODTransform transform = p->transform->Copy(ev);

        ODFrame* containingFrame = embeddedFrame->AcquireContainingFrame(ev);
        ODFacet* facet = FirstFacet(ev, containingFrame);
        if (facet) {
          TempODTransform contentTransform = facet->AcquireContentTransform(ev, kODNULL);
          transform->PostCompose(ev, contentTransform);
        }

        ODRect boundingBox;
        frameShape->GetBoundingBox(ev, &boundingBox);

        ODPoint botLeft = {boundingBox.left, boundingBox.bottom};
        ODPoint topRight = {boundingBox.right, boundingBox.top};
        transform->TransformPoint(ev, &botLeft);
        transform->TransformPoint(ev, &topRight);

        RECTL rcl = ODRECTL( botLeft, topRight );

        GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 2, (PPOINTL)&rcl );

        obj->SetHandle( 0, *(PPOINTL)&rcl.xLeft, FALSE );
        obj->SetHandle( 1, *(PPOINTL)&rcl.xRight, FALSE );

        if (_picture->IsSelected(obj))
          _picture->BoundSelected();

        // fix clipping of obscured frames
        somSelf->ClipEmbeds( ev, facet, &rcl );

        embeddedFrame->Invalidate(ev, kODNULL, kODNULL);
      }
    }
  }

  else
    THROW(kODErrInvalidFrame);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving RequestFrameShape\n");
  #endif

  return frameShape;
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartUsedShapeChanged( GraphicsPart *somSelf,
                                                      Environment *ev,
                                                      ODFrame* embeddedFrame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","UsedShapeChanged");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering UsedShapeChanged\n");
  #endif

  SOM_TRY

  if (_fEmbeddedFrames->Contains(embeddedFrame)) {
    somSelf->ClipEmbeds(ev, kODNULL, NULL);
  }
  else
    THROW(kODErrInvalidFrame);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving UsedShapeChanged\n");
  #endif
}

// ------------------------------------------------------------------
//   General FACET management                                     aaa
// ------------------------------------------------------------------

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartFacetAdded( GraphicsPart *somSelf,
                                                Environment *ev, ODFacet* facet)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","FacetAdded");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering FacetAdded\n");
  #endif

  SOM_TRY

  ODFrame* dispFrame = facet->GetFrame(ev);

  // check for valid facet??
  if (!_fDisplayFrames->Contains(dispFrame))
    THROW(kODErrInvalidFacet);

  ODBoolean isRoot = dispFrame->IsRoot(ev);
  _fFacetCount++;

  RECTL rclPic;

  if (_fFacetCount == 1) {

    QueryFacetOffset( ev, facet, &_frameOffset );
    _fCurFrame = dispFrame;

    ODWindow* window = facet->GetWindow(ev);
    HWND hwndFrame = window ? window->GetPlatformWindow(ev) : 0;

    HWND hwnd = hwndFrame ? WinWindowFromID( hwndFrame, FID_CLIENT ) : 0;

    // fancy status line if we are the root!
    if (isRoot && hwndFrame) {
      #define FID_STATUSLINE 0x7FFF
      HWND hwndStatus = WinWindowFromID( hwndFrame, FID_STATUSLINE );

      if ( WinIsWindow((HAB)0,hwndStatus) ) {
        LONG clr = CLR_PALEGRAY;
        WinSetPresParam( hwndStatus, PP_BACKGROUNDCOLORINDEX, 4L, &clr );
      }
    }

    // try to change client clipping style...
    if (hwnd) {
      ULONG ulStyle = WinQueryWindowULong( hwnd, QWL_STYLE );
      WinSetWindowULong( hwnd, QWL_STYLE, (ulStyle|WS_CLIPSIBLINGS) );
    }

    HDC hdc;

    if (hwnd) {
      hdc = WinOpenWindowDC( hwnd );
    } else {
      HPS hps = facet->GetCanvas(ev)->GetPlatformCanvas(ev, kODGPI)->GetPS(ev, facet);
      hdc = DevOpenDC( (HAB)0, OD_MEMORY, "*", 0, 0, GpiQueryDevice(hps) );
    }

    SIZEL  sizlWin = {0,0};
    _fsmHps = GpiCreatePS( (HAB)0, hdc, &sizlWin, PU_PELS | GPIA_ASSOC );

    RGB_HPS(_fsmHps);                                // set hps to RGB mode!

    if (!_ptrXhair)
      _ptrXhair = LoadPointer( _hmod, ID_XHAIR );

//    _hwndObj = facet->GetFacetHWND(ev);
    _hwndObj = hwnd;

    // set up picture bounds for NEW (empty) picture
//    if (!_picture->ObjectCount()) {

      HRGN  hrgnFrame;
      TempODShape frameShape = ODCopyAndRelease(ev, dispFrame->AcquireFrameShape(ev, kODNULL));

      ODTransform* xform = facet->AcquireWindowFrameTransform(ev, kODNULL);
      frameShape->Transform(ev, xform);
      ODReleaseObject(ev, xform);

      hrgnFrame = (HRGN)frameShape->GetRegion(ev);
      GpiQueryRegionBox( _hpsMem, hrgnFrame, &_rclFrame);

      xform = ODCopyAndRelease(ev, facet->AcquireContentTransform(ev, kODNULL));
      xform->Invert(ev);
      frameShape->Transform(ev, xform);
      ODReleaseObject(ev, xform);

      RECTL rclTmp;
      hrgnFrame = (HRGN)frameShape->GetRegion(ev);
      GpiQueryRegionBox( _hpsMem, hrgnFrame, &rclTmp);
      _picture->SetBounds(&rclTmp);
//    }

    SetScaling2( ev, facet, _fsmHps );
  }

  ODShape* contentClip = facet->CreateShape(ev);
  contentClip->Reset(ev);
  facet->SetPartInfo(ev, (ODInfoType)contentClip);

  // for first facet we MUST call PostReadInit
  if ( _fFacetCount == 1 ) {

    if (isRoot) {

      // first, lets do a simple background fill to get the docshell looking ok
      _picture->QueryBounds(&rclPic);
      FillRect( _fsmHps, &rclPic, _fBGColor );

      // show the toolbar if appropriate
      somSelf->UpdatePalette(ev, dispFrame);

      SetSysPointer( SPTR_WAIT );

      if (_fBitmapFile[0]) {
        _hbmBackground = read_bitmap( _hpsMem, _fBitmapFile );

        // UNsuccessful read?  Reset to color-only mode.
        if (!_hbmBackground) {
          _fBGMode == BGM_COLOR;
        }
      }

    } else {

      // if we aren't the root and we are matching bg color, as our container
      if ( _fMatchBGColor ) {

        // if we are root we'll get null for a containing frame..
        TempODFrame containingFrame = dispFrame->AcquireContainingFrame(ev);
        TempODPart containingPart = containingFrame->AcquirePart(ev);
        TempODStorageUnit propSU = containingPart->AcquireContainingPartProperties(ev, dispFrame);

        if (propSU && SetFocus( ev, propSU, kODBackgroundColor, kODRGB2) )
          StorageUnitGetValue(propSU, ev, sizeof(_fBGColor), &_fBGColor);
      }
    }

    // NOW, lets do the deferred internalization/setup of embeds
    _picture->PostReadInit(ev, somSelf, kODFalse);
  }

if (_fDisplayFrames->Count()==1) {
  ODxOrderedCollectionIterator frames(_fEmbeddedFrames);
  for (ODFrame* frame = (ODFrame*) frames.First();
      frames.IsNotComplete();
      frame = (ODFrame*) frames.Next())
  {
    TempODFrame containingFrame = frame->AcquireContainingFrame(ev);
    if (containingFrame == dispFrame)
    {
      TempODShape clip = ODCopyAndRelease(ev, frame->AcquireFrameShape(ev, kODNULL));
      TempODTransform xform = somSelf->ProxyForFrame(ev, frame)->transform->Copy(ev);

      facet->CreateEmbeddedFacet(ev, frame, clip, xform,
                                 kODNULL, kODNULL, kODNULL, kODFrameBehind);
    }
  }

  // make sure things are set up before painting!
  if ( _fFacetCount == 1 && dispFrame->GetViewType(ev) != FRAME_VIEW ) {
    somSelf->ViewTypeChanged(ev, dispFrame);
  } else {
//    somSelf->ClipEmbeds(ev, facet, 0 );
  }
} /* endif */

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving FacetAdded\n");
  #endif
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartFacetRemoved( GraphicsPart *somSelf,
                                                  Environment *ev, ODFacet* facet)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","FacetRemoved");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering FacetRemoved\n");
  #endif

  SOM_TRY

  // check for valid facet?
  ODFrame* dispFrame = facet->GetFrame(ev);
  if (!_fDisplayFrames->Contains(dispFrame))
    THROW(kODErrInvalidFacet);

  _fFacetCount--;

  if (!_fFacetCount) {
  }

  // delete the cached embed-clipping
  TempODShape clipShape = (ODShape*)facet->GetPartInfo(ev);
  facet->SetPartInfo(ev, (ODInfoType) kODNULL);

  // make a copy-collection of our facet's children
  ODxOrderedCollection* children = new ODxOrderedCollection;

  ODFacetIterator* facets = facet->CreateFacetIterator(ev, kODChildrenOnly, kODFrontToBack);
  for (ODFacet* childFacet = facets->First(ev); facets->IsNotComplete(ev);
       childFacet = facets->Next(ev)) {
    children->AddLast(childFacet);
  }
  delete facets;

  // delete from the copy-collection
  ODxOrderedCollectionIterator iter(children);
  for (childFacet = (ODFacet*)iter.First(); iter.IsNotComplete();
       childFacet = (ODFacet*)iter.Next()) {
    facet->RemoveFacet(ev, childFacet);
    delete childFacet;
  }
  delete children;

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving FacetRemoved\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartCanvasChanged(GraphicsPart *somSelf,
                                       Environment *ev, ODFacet* facet)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","CanvasChanged");

  #ifdef ODDebug2
    somPrintf("GtkPart: null CanvasChanged\n");
  #endif
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartGeometryChanged( GraphicsPart *somSelf,
                                                     Environment *ev,
                                                     ODFacet* facet,
                                                     ODBoolean clipShapeChanged,
                                                     ODBoolean externalTransformChanged)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","GeometryChanged");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering GeometryChanged\n");
  #endif

  SOM_TRY

  if (externalTransformChanged) {

    ODFacet* firstFacet = FirstFacet(ev,_fCurFrame);

      POINTL newOffset;
      QueryFacetOffset( ev, facet, &newOffset );

    // we only want to do these parts for the primary screen facet!!!
    if (facet == firstFacet) {
      somSelf->MovePalette( ev, newOffset.x - _frameOffset.x,
                                newOffset.y - _frameOffset.y );

      _frameOffset = newOffset;
    }

    SetScaling2( ev, facet, _fsmHps );

    somSelf->ClipEmbeds(ev, facet, (RECTL*)0 );

    if (facet != firstFacet)
      SetScaling2( ev, firstFacet, _fsmHps );
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving GeometryChanged\n");
  #endif
}

// ---------------------------------------------------------------------------
void superBlt( HPS hps, HBITMAP hbm, PPOINTL aptl )
{
  ULONG ec = WinGetLastError ( (HAB)0 );

  if (GpiWCBitBlt( hps, hbm, 4, aptl, ROP_SRCCOPY, BBO_IGNORE) == GPI_ERROR) {

    HDC hdcMem;
    HPS hpsMem;
    SIZEL sizl;

    ec = WinGetLastError ( (HAB)0 );

    // create compatible MEMORY hdc (must be mem hdc)
    hdcMem = DevOpenDC( (HAB)0, OD_MEMORY, "*",
                        0L, NULL, GpiQueryDevice(hps) );

    // create mem hps and associate it
    sizl.cx = sizl.cy = 0L;
    hpsMem = GpiCreatePS( (HAB)0, hdcMem, &sizl,
                          PU_PELS|GPIT_MICRO|GPIA_ASSOC );

    // make the bitmap compatible
    if ( GpiSetBitmap( hpsMem, hbm ) == HBM_ERROR )
      ec = WinGetLastError ( (HAB)0 );

    // free stuff up
    GpiSetBitmap( hpsMem, NULL );
    GpiDestroyPS( hpsMem );
    DevCloseDC( hdcMem );

    // retry the WCBitBlt
    if (GpiWCBitBlt( hps, hbm, 4, aptl, ROP_SRCCOPY, BBO_IGNORE) == GPI_ERROR)
      ec = WinGetLastError ( (HAB)0 );
  }
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDraw( GraphicsPart *somSelf,  Environment *ev,
                                          ODFacet* facet, ODShape* invalidShape)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","Draw");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering Draw\n");
  #endif

  SOM_TRY

  ODCanvas* canvas = facet->GetCanvas(ev);
  ODBoolean onScreen = canvas->IsDynamic(ev);
  ODFrame* displayFrame = facet->GetFrame(ev);

  if (_fDisplayFrames->Contains(displayFrame)) {

    HPS hpsDraw;
    ODTypeToken curView = displayFrame->GetViewType(ev);

    RECTL rclPic;
    ODRect rect;

    TempODShape frameShape = displayFrame->AcquireFrameShape(ev, canvas);
    frameShape->GetBoundingBox(ev, &rect);
    ODRECTL rcl(rect);
    rclPic = rcl;

    // FRAME view
    if (curView == FRAME_VIEW) {

      // fill background clipped to invalid area MINUS EMBED's areas!
      {
        // make a tmp copy of the invalid shape
        TempODShape bgClipShape = invalidShape->Copy(ev);

        // subtract off the embeds' shapes to avoid flicker
        ODShape* embedsShape = (ODShape*)facet->GetPartInfo(ev);
        if (!embedsShape->IsEmpty(ev)) {
          bgClipShape->Subtract(ev, embedsShape );
        }

        // set up the clipping for background painting
        CFocus f(ev, facet, bgClipShape, &hpsDraw );

        // new color or bitmap background
        #if 1

          RGB_HPS( hpsDraw );                                  // set hps to RGB mode!
          GpiSetMix( hpsDraw, FM_DEFAULT );
          GpiSetPattern( hpsDraw, PATSYM_SOLID );

          if (_fBGMode==BGM_COLOR) {

            FillRect( hpsDraw, &rclPic, _fBGColor );

          } else {

            BITMAPINFOHEADER2 bmp;
            POINTL aptl[4];
            ODULong cx, cy, dx, dy, x0, y0;

            bmp.cbFix = sizeof(bmp);
            GpiQueryBitmapInfoHeader( _hbmBackground, &bmp);

            if (_fBGMode==BGM_TILE) {

              long xRes, yRes;
//              HDC hdc = GpiQueryDevice(hpsDraw);
              HDC hdc = WinOpenWindowDC(HWND_DESKTOP);
              DevQueryCaps( hdc, CAPS_HORIZONTAL_RESOLUTION, 1L, &xRes );
              DevQueryCaps( hdc, CAPS_VERTICAL_RESOLUTION, 1L, &yRes );

              invalidShape->GetBoundingBox(ev, &rect);
              ODRECTL rcl2(rect);

              cx = (ODULong)( (float)bmp.cx * 72.0 / 0.0254 / xRes );
              cy = (ODULong)( (float)bmp.cy * 72.0 / 0.0254 / yRes );

              // bitmap source coords
              aptl[2].x = aptl[2].y = 0;
              aptl[3].x = bmp.cx;
              aptl[3].y = bmp.cy;

              aptl[0].x = rcl.xLeft;
              aptl[0].y = rcl.yBottom;
              aptl[1].x = aptl[0].x + cx;
              aptl[1].y = aptl[0].y + cy;

              // row loop
              do {

                // up into the invalid area yet?
                if (aptl[1].y>rcl2.yBottom)  {

                  // column step
                  do {
                    // over into the invalid area yet?
                    if (aptl[1].x>rcl2.xLeft)
//                      GpiWCBitBlt( hpsDraw, _hbmBackground, 4, aptl, ROP_SRCCOPY, BBO_IGNORE );
                      superBlt( hpsDraw, _hbmBackground, aptl );

                    // step to the right
                    aptl[0].x += cx; // dx;
                    aptl[1].x += cx; // dx;

                  // until we step out of the invalid area
                  } while (aptl[0].x < rcl2.xRight );

                  // reset to left
                  aptl[0].x = rcl.xLeft;
                  aptl[1].x = aptl[0].x + cx;
                }

                // step up
                aptl[0].y += cy; // dy;
                aptl[1].y += cy; // dy;

              // until we step out of the invalid area
              } while (aptl[0].y < rcl2.yTop );
            } else {

              aptl[0].x = rclPic.xLeft;
              aptl[0].y = rclPic.yBottom;
              aptl[1].x = rclPic.xRight;
              aptl[1].y = rclPic.yTop;
              aptl[2].x = aptl[2].y = 0;
              aptl[3].x = bmp.cx;
              aptl[3].y = bmp.cy;

//              GpiWCBitBlt( hpsDraw, _hbmBackground, 4, aptl, ROP_SRCCOPY, BBO_IGNORE );
              superBlt( hpsDraw, _hbmBackground, aptl );
            }
          }

        // old solid color only
        #else

          RGB_HPS( hpsDraw );                                  // set hps to RGB mode!
          GpiSetMix( hpsDraw, FM_DEFAULT );
          GpiSetPattern( hpsDraw, PATSYM_SOLID );

          FillRect( hpsDraw, &rclPic, _fBGColor );
        #endif

        GpiRestorePS( hpsDraw, -1 );
      }

      // draw CONTENT clipped to invalid area
      {
        CFocus f2(ev, facet, invalidShape, &hpsDraw);

        RGB_HPS( hpsDraw );
        GpiSetMix( hpsDraw, FM_DEFAULT );
        GpiSetPattern( hpsDraw, PATSYM_SOLID );

        _picture->Draw( hpsDraw, NULL, facet );
      }

      // highlite selection if we have the selection focus (and window active!)
      if ( onScreen && facet->GetWindow(ev)->IsActive(ev)) {
        TempODFrame tmpFrame = _fSession->GetArbitrator(ev)->AcquireFocusOwner(ev, _fSelectionFocus);
        if (displayFrame == tmpFrame)
          somSelf->HighlightSelection(ev, facet, invalidShape);
      }

    // other views
    } else {

      TempODShape usedShape = displayFrame->AcquireUsedShape(ev, canvas);
      usedShape->GetBoundingBox(ev, &rect);
      ODRECTL rcl2(rect);

      // set up hps
      CFocus f(ev, facet, invalidShape, &hpsDraw);

      // draw the view
      if (curView == THUMBNAIL_VIEW ) {
        POINTL aptl[4];

        aptl[0].x = rcl2.xLeft;
        aptl[0].y = rcl2.yBottom;
        aptl[1].x = rcl2.xRight - 1;
        aptl[1].y = rcl2.yTop - 1;

        aptl[2].x = aptl[2].y = 0;
        aptl[3].x = THUMBSIZE;
        aptl[3].y = THUMBSIZE;

        GpiWCBitBlt( hpsDraw, _hbmThumbnail, 4, aptl, ROP_SRCCOPY, BBO_IGNORE );
      } else {
        _icon->Draw( hpsDraw, &_frameOffset );
      }

      // here we validate all embedded facets so they don't actually get drawn!!
      ODFacetIterator* facets = facet->CreateFacetIterator(ev, kODChildrenOnly, kODFrontToBack);
      for (ODFacet* embFacet = facets->First(ev); facets->IsNotComplete(ev);
           embFacet = facets->Next(ev))
        embFacet->Validate(ev, kODNULL, kODNULL );
      delete facets;
    }
  }
  else
  {
    // !!! signal error: invalid frame
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving Draw\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartCanvasUpdated(GraphicsPart *somSelf,
                                       Environment *ev, ODCanvas* canvas)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","CanvasUpdated");

  #ifdef ODDebug2
    somPrintf("GtkPart: null CanvasUpdated\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartHighlightChanged(GraphicsPart *somSelf,
                                          Environment *ev, ODFacet* facet)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","HighlightChanged");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering HighlightChanged\n");
  #endif

  SOM_TRY

    ODHighlight highlight = facet->GetHighlight(ev);

    ODFacetIterator* facets = facet->CreateFacetIterator(ev, kODChildrenOnly, kODFrontToBack);
    for (ODFacet* embFacet = facets->First(ev);
          facets->IsNotComplete(ev);
          embFacet = facets->Next(ev))
    {
        embFacet->ChangeHighlight(ev, highlight);
    }
    /*
     * cause ourselves to be redrawn.  FocusLib will take care of setting the
     * default mix mode so that we paint properly.
     */
    facet->Invalidate(ev, kODNULL, kODNULL);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving HighlightChanged\n");
  #endif
}

// ------------------------------------------------------------------
//   LINKING  support                                             aaa
// ------------------------------------------------------------------

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope ODLinkSource*  SOMLINK GraphicsPartCreateLink( GraphicsPart *somSelf,
                                                         Environment *ev,
                                                         ODByteArray* data)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","CreateLink");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering CreateLink\n");
  #endif

  ODLinkSource* linkSource;

  SOM_TRY

  linkSource = _picture->CreateLinkSource( ev, somSelf, (ODULong*)data->_buffer );

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving CreateLink\n");
  #endif

  return linkSource;
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartLinkUpdated(GraphicsPart *somSelf,  Environment *ev,
                                    ODLink* updatedLink, ODUpdateID id)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","LinkUpdated");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering LinkUpdated\n");
  #endif

  SOM_TRY

  GtkLink* linkObj = _picture->LinkObj( updatedLink );
  if (linkObj) {
    RECTL rclInvalid, rclTmp;
    ODBoolean isSelected = _picture->IsSelected(linkObj);

    // save the original bounds
    rclInvalid = isSelected ? *_picture->SelectedBounds() : linkObj->Bounds();

    linkObj->Update(ev, somSelf);

    // union with the new bounds
    rclTmp = linkObj->Bounds();
    AddRect( &rclInvalid, &rclTmp );

    // update embed clipping
    somSelf->ClipEmbeds( ev, 0, &rclInvalid );

    // update the selection bounds and re-display the handles
    if (isSelected) {
      _picture->BoundSelected();
    }

    // repaint
    somSelf->InvalidateRECTL( ev, &rclInvalid );
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving LinkUpdated\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartRevealLink(GraphicsPart *somSelf,  Environment *ev,
                                   ODLinkSource* linkSource)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","RevealLink");

  #ifdef ODDebug2
    somPrintf("GtkPart: null RevealLink\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartLinkStatusChanged( GraphicsPart *somSelf,
                                                       Environment *ev,
                                                       ODFrame* frame)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","LinkStatusChanged");

  #ifdef ODDebug2
    somPrintf("GtkPart: null LinkStatusChanged\n");
  #endif
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartBreakLinkSource( GraphicsPart *somSelf,
                                                     Environment *ev,
                                                     ODLinkSource* linkSource)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","BreakLinkSource");

  #ifdef ODDebug2
    somPrintf("GtkPart: doing BreakLinkSource\n");
  #endif

  SOM_TRY

  _picture->BreakLinkSource( ev, linkSource );
  _fCurFrame->Invalidate(ev, kODNULL , kODNULL );
  somSelf->PartUpdated( ev, 0, 0 );

  SOM_CATCH_ALL
  SOM_ENDTRY
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartBreakLink( GraphicsPart *somSelf,
                                               Environment *ev, ODLink* link)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","BreakLink");

  #ifdef ODDebug2
    somPrintf("GtkPart: doing BreakLink\n");
  #endif

  SOM_TRY

  _picture->BreakLinks( ev, link );
  _fCurFrame->Invalidate(ev, kODNULL , kODNULL );
  somSelf->PartUpdated( ev, 0, 0 );

  SOM_CATCH_ALL
  SOM_ENDTRY
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartLinkBroken( GraphicsPart *somSelf,
                                                Environment *ev, ODLink* link)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","LinkBroken");

  #ifdef ODDebug2
    somPrintf("GtkPart: doing LinkBroken\n");
  #endif

  SOM_TRY

  _picture->BreakLinks( ev, link );
  _fCurFrame->Invalidate(ev, kODNULL , kODNULL );
  somSelf->PartUpdated( ev, 0, 0 );

  SOM_CATCH_ALL
  SOM_ENDTRY
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartUpdateFromLinkSource( GraphicsPart *somSelf,
                                                          Environment *ev,
                                                          ODLinkSource* linkSource)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","UpdateFromLinkSource");

  #ifdef ODDebug2
    somPrintf("GtkPart: doing UpdateFromLinkSource\n");
  #endif

  SOM_TRY

  _picture->UpdateLinks( ev, somSelf, linkSource );

  SOM_CATCH_ALL
  SOM_ENDTRY
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartShowLink( GraphicsPart *somSelf,
                                              Environment *ev,
                                              ODLink* link)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","ShowLink");

  #ifdef ODDebug2
    somPrintf("GtkPart: null ShowLink\n");
  #endif
}

// ------------------------------------------------------------------
//   FOCUS management                                             aaa
// ------------------------------------------------------------------

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartBeginRelinquishFocus(GraphicsPart *somSelf,
                                                   Environment *ev,
                                                  ODTypeToken focus,
                                                  ODFrame* ownerFrame,
                                                  ODFrame* proposedFrame)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","BeginRelinquishFocus");

  #ifdef ODDebug2
    somPrintf("GtkPart: null BeginRelinquishFocus\n");
  #endif

    return kODTrue;
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartCommitRelinquishFocus(GraphicsPart *somSelf,
                                               Environment *ev,
                                              ODTypeToken focus,
                                              ODFrame* ownerFrame,
                                              ODFrame* proposedFrame)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","CommitRelinquishFocus");

  #ifdef ODDebug2
    somPrintf("GtkPart: doing CommitRelinquishFocus\n");
  #endif

  SOM_TRY

  somSelf->FocusLost(ev, focus, ownerFrame);

  SOM_CATCH_ALL
  SOM_ENDTRY
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartAbortRelinquishFocus(GraphicsPart *somSelf,
                                              Environment *ev,
                                             ODTypeToken focus,
                                             ODFrame* ownerFrame,
                                             ODFrame* proposedFrame)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","AbortRelinquishFocus");

  #ifdef ODDebug2
    somPrintf("GtkPart: null AbortRelinquishFocus\n");
  #endif
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartFocusAcquired(GraphicsPart *somSelf,
                                       Environment *ev, ODTypeToken focus,
                                      ODFrame* ownerFrame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","FocusAcquired");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering FocusAcquired\n");
  #endif

  SOM_TRY

  if (focus == _fSelectionFocus) {

    PartInfoRec* pInfo = (PartInfoRec*) ownerFrame->GetPartInfo(ev);
    pInfo->fIsActive = kODTrue;

    // show selection marks
    somSelf->HighlightSelection( ev, FirstFacet(ev,ownerFrame), 0 );

  } else if (focus == _fMenuFocus) {

    // with deferred menubar creation we must check here...
    if (!_fMenuBar)
      somSelf->InstallMenus(ev, ownerFrame);

    // created ok???
    if (_fMenuBar)
      _fMenuBar->Display(ev);

    // FIX for 131151, move this here from the fSelectionFocus case
    // Display apparently does a zorder TOP on the main menubar...
    somSelf->UpdatePalette(ev, ownerFrame);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving FocusAcquired\n");
  #endif
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartFocusLost(GraphicsPart *somSelf,  Environment *ev,
                                  ODTypeToken focus, ODFrame* ownerFrame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","FocusLost");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering FocusLost\n");
  #endif

  SOM_TRY

  if (focus == _fSelectionFocus) {
    PartInfoRec* pInfo = (PartInfoRec*) ownerFrame->GetPartInfo(ev);
    pInfo->fIsActive = kODFalse;

    somSelf->fsm_set_state( ev, 0, 0 );

    if (_picture->SelectedCount()) {

      // erase selection marks
      somSelf->HighlightSelection( ev, FirstFacet(ev,ownerFrame), 0 );

      // clear the selection (for now)
      _picture->Select();                                          // acw

      somSelf->ClipEmbeds( ev, 0, 0 );
    }

    _fAltMark = FALSE;
    somSelf->UpdatePalette(ev, ownerFrame);
  }

// call the following for ANY focus loss!!!!!!!!
//  else if (focus == _fStatusFocus) {

    somSelf->DisplayText(ev,"",ownerFrame);

    _fSession->GetArbitrator(ev)->RelinquishFocus(ev, _fStatusFocus, ownerFrame);
    _fHaveStatusLineFocus = kODFalse;

//  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving FocusLost\n");
  #endif
}

// ------------------------------------------------------------------
//   EVENT handling                                               aaa
// ------------------------------------------------------------------

WorldPtFromOD( Environment *ev, PPOINTL ptl, PPOINTL ptlWorld, ODFacet* facet)
{
  ODPoint windowODPoint(ODPOINTL(ptl->x,ptl->y));

  // possible candidate for caching (inverted xform)???
  TempODTransform xform = facet->AcquireWindowContentTransform(ev, kODNULL);
  xform->InvertPoint(ev, &windowODPoint);

  ODPOINTL ptlOut(windowODPoint);
  *ptlWorld = ptlOut;
}

// ------------------------------------------------------------------
int HelpPanelID( MPARAM mp1, MPARAM mp2 )
{
  int    helpID = 0;
  SHORT  Context = SHORT1FROMMP(mp1);
  USHORT Topic   = SHORT1FROMMP(mp2);
  USHORT SubTopic = SHORT2FROMMP(mp2);

  if (Context==-2 ) {                 // HLPM_WINDOW) {
    return TOOLBAR_MH;
  }

  int    itemID = (SubTopic == 0xFFFF) ? Topic : SubTopic;

  switch (itemID) {

    case VIEW_REFRESH:
       helpID =  VIEW_REFRESH_MH;
       break;
    case VIEW_SHOWTOOLS:
       helpID = VIEW_SHOWTOOLS_MH;
       break;
    case IDM_VIEW_BG:
       helpID = IDM_VIEW_BG_MH;
       break;
    case VIEW_BGNDMODE_COLOR:
       helpID = VIEW_BGNDMODE_COLOR_MH;
       break;
    case VIEW_BGNDMODE_TILED:
       helpID = VIEW_BGNDMODE_TILED_MH;
       break;
    case VIEW_BGNDMODE_BLTFIT:
       helpID = VIEW_BGNDMODE_BLTFIT_MH;
       break;
    case VIEW_BGND_COLOR:
       helpID = VIEW_BGND_COLOR_MH;
       break;

    case SELECTED_ALTMARK:
       helpID = SELECTED_ALTMARK_MH;
       break;
    case SELECTED_ROTATE:
    case IDM_POPUP_ROTATE:
       helpID = SELECTED_ROTATE_MH;
       break;
    case SELECTED_GROUPING:
    case SELECTED_UNGROUP:
    case SELECTED_GROUP:
       helpID = SELECTED_GROUPING_MH;
       break;
    case SELECTED_ALIGNS:
       helpID = SELECTED_ALIGNS_MH;
       break;
    case SELECTED_ARRANGE:
       helpID = SELECTED_ARRANGE_MH;
       break;
    case SELECTED_LOCKING:
    case SELECTED_LOCK:
    case SELECTED_UNLOCK:
       helpID = SELECTED_LOCKING_MH;
       break;
    case SELECTED_BGNDSTUFF:
    case SELECTED_OPAQUE:
    case SELECTED_TRANSLUCENT:
       helpID = SELECTED_BGNDSTUFF_MH;
       break;
    case ID_LINESTUFFMENU:
       helpID = ID_LINESTUFFMENU_MH;
       break;
    case ID_FILLSTUFFMENU:
       helpID = FILLSTUFFMENU_MH;
       break;
    case ID_SHADOWSTUFFMENU:
       helpID = SHADOWSTUFFMENU_MH;
       break;
    case ID_ARCSTUFFMENU:
       helpID = ID_ARCSTUFFMENU_MH;
       break;
    case IDM_POPUP_DELETE:
       break;
    case IDM_TOOL_POINTER :
       helpID = SELECTIONTOOL_MH;
       break;
    case IDM_TOOL_LINE    :
       helpID = LINETOOL_MH;
       break;
    case IDM_TOOL_POLYLINE:
       helpID = POLYLINETOOL_MH;
       break;
    case IDM_TOOL_RECT    :
       helpID = RECTANGLETOOL_MH;
       break;
    case IDM_TOOL_BOX     :
       helpID = BOXTOOL_MH;
       break;
    case IDM_TOOL_ELLIPSE :
       helpID = ELLIPSETOOL_MH;
       break;
    case IDM_TOOL_ARC     :
       helpID = ARCTOOL_MH;
       break;
    default:
       break;
  }

  return helpID;
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartHandleEvent( GraphicsPart *somSelf,
                                                      Environment *ev,
                                                      ODEventData* event,
                                                      ODFrame* frame,
                                                      ODFacet* facet,
                                                      ODEventInfo* eventInfo)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","HandleEvent");

  #ifdef ODDebug2
//  somPrintf("GtkPart: Entering HandleEvent... ");
  #endif

  ODBoolean handled = kODFalse;

  SOM_TRY

  ODBoolean menucommand = (facet == kODNULL);
  POINTL ptl, ptlWorld;

  // captured msgs have other parts' (or no) frame/facet...
  if (!frame || !_fDisplayFrames->Contains(frame) ) {
    frame = _fCurFrame ? _fCurFrame : (ODFrame*)_fDisplayFrames->First();
  }

  // have to do second check here cause during capture the facet passed in
  // is that under the cursor rather than MINE... I need mine.
  if (!facet || facet->GetFrame(ev) != frame ) {
    facet = FirstFacet(ev, frame);
  }

  ptl.x = SHORT1FROMMP(event->mp1);
  ptl.y = SHORT2FROMMP(event->mp1);
  ptlWorld = ptl;

  switch (event->msg) {

    case WM_SINGLESELECT:

      #ifdef ODDebug2
        somPrintf(" ...WM_SINGLESELECT... \n");
      #endif

      if ( !_fIgnoreSingleSelect && frame->GetViewType(ev) == FRAME_VIEW ) {

        GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &ptlWorld );
        handled = somSelf->fsm_execute( ev, msg2action(event->msg), &ptlWorld );
      }
      break;

    case WM_BUTTON1CLICK :

      #ifdef ODDebug2
        somPrintf(" ...WM_BUTTON1CLICK... \n");
      #endif

      if ( !FrameIsActive(ev, frame) ) {
        somSelf->ActivateFrame(ev, frame);

        // did the user click in an embed's active border?
        if ( eventInfo->flags & kODInBorder ) {

          // select that embed
          ODFacet* embedFacet = eventInfo->embeddedFacet;
          _picture->Select( somSelf->ProxyForFrame( ev, embedFacet->GetFrame(ev) )->id );

          // adjust the selection view
          somSelf->ClipEmbeds( ev, facet, 0 );
          somSelf->HighlightSelection(ev, facet, 0);

          // set flag so wm_singleselect won't undo this selection...
          _fIgnoreSingleSelect = kODTrue;
        }
      }

      // get the ctrl+click here (since it isn't being passed in singleselect...)
      else if ( (eventInfo->flags & kODInEmbedded) && key_shift(VK_CTRL) ) {

        // handle it
        GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &ptlWorld );
        handled = somSelf->fsm_execute( ev, msg2action(event->msg), &ptlWorld );

        // set flag so wm_singleselect won't undo this selection... (catch 22?)
        _fIgnoreSingleSelect = kODTrue;
      }

      // FORCED, to prevent underlying container from stealing focus back!
      handled = kODTrue;
      break;

    case WM_MOUSEMOVE:

      #ifdef ODDebug2
      //    somPrintf(" ...WM_MOUSEMOVE... \n");
      #endif

      if ( frame->GetViewType(ev) == FRAME_VIEW ) {

        // use default handling if not in fsm and over embed border...
        if ( !_fsmState && eventInfo && (eventInfo->flags & kODInBorder) ) {

        // MY processing
        } else {

          GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &ptlWorld );
          handled = somSelf->fsm_execute( ev, msg2action(event->msg), &ptlWorld );
        }
      }
      break;

    case WM_BEGINDRAG:
    case WM_BEGINSELECT:

      #ifdef ODDebug2
        somPrintf(" ...WM_BEGINDRAG/SELECT... \n");
      #endif

      if ( frame->GetViewType(ev) == FRAME_VIEW ) {

        // if we aren't active, JUST activate ourselves...
        if ( event->msg == WM_BEGINSELECT && !FrameIsActive(ev, frame) ) {
          somSelf->ActivateFrame(ev, frame);
          handled = kODTrue;
          break;
        }

        if (_fReadOnly && event->msg==WM_BEGINDRAG)
          break;

        // cache this event for use in InitDrag()
        _baseEvent = *event;

        // handle the event
        handled = somSelf->fsm_execute( ev, msg2action(event->msg), &_ptlUpDown );
      }
      break;

    case WM_ENDDRAG:
    case WM_ENDSELECT:

      #ifdef ODDebug2
        somPrintf(" ...WM_ENDDRAG/SELECT... \n");
      #endif

      // these are only used if something's going on...
      if (_fsmState) {
        GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &ptlWorld );
        handled = somSelf->fsm_execute( ev, msg2action(event->msg), &ptlWorld );
      }
      break;

    case WM_BUTTON1DBLCLK:

      #ifdef ODDebug2
        somPrintf(" ...WM_BUTTON1DBLCLK... \n");
      #endif

      // only used if something's up
      if (_fsmState) {
        GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &ptlWorld );
        handled = somSelf->fsm_execute( ev, msg2action(event->msg), &ptlWorld );
      }
      break;

    case WM_BUTTON1DOWN:
    case WM_BUTTON2DOWN:
      {
        #ifdef ODDebug2
          somPrintf(" ...WM_BUTTON1/2DOWN... \n");
        #endif

        handled = kODTrue;

        // here we try to activate this document WINDOW if it isn't already
        ODWindow* window = facet->GetWindow(ev);
        if ( event->msg == WM_BUTTON1DOWN && !window->IsActive(ev)) {
          window->Select(ev);
          break;
        }

        // save the ACTUAL down point to use with WM_BEGIN* since the cooked
        // msgs don't pass on the correct points for us!
        _mousePt = ptl;
        GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &ptlWorld );
        _ptlUpDown = ptlWorld;

        // act on btn 2, and adjust contextmenu flag
        if ( event->msg == WM_BUTTON2DOWN ) {

          if ( _fsmState && frame->GetViewType(ev) == FRAME_VIEW ) {
            handled = somSelf->fsm_execute( ev, msg2action(event->msg), &ptlWorld );
            _fMB2handled = handled;
          }
          else _fMB2handled = kODFalse;
        }

        // clear this flag
        else _fIgnoreSingleSelect = kODFalse;
      }
      break;

    case WM_CONTEXTMENU:

      // handle if appropriate
      if ( !_fsmState && !_fMB2handled )
        somSelf->DoPopup(ev, facet, &ptl);

      // prevent msg fall-through
      handled = kODTrue;
      break;

    case WM_PRESPARAMCHANGED :
      if (LONGFROMMP(event->mp1)==PP_BACKGROUNDCOLOR) {
        ULONG rgb;
        WinQueryPresParam( event->hwnd, PP_BACKGROUNDCOLOR, 0, kODNULL,
                           sizeof(rgb), &rgb, QPF_NOINHERIT );

        // user drops on new background color?
        if ( rgb != _fBGColor ) {

          // set up the UNDO/REDO stuff
          UndoBgndColor* undoRec = new UndoBgndColor(frame, _fBGColor, rgb);
          LogUndoAction( somSelf, ev, undoRec, sizeof(*undoRec), _fSession,
                         "background color");

          // make the change (and propagate it)
          somSelf->SetBGColor(ev, frame, rgb, kODTrue);
        }
        handled = kODTrue;
      }
      break;

    case WM_CHAR :

      #ifdef ODDebug2
       //    somPrintf(" ...WM_CHAR... \n");
      #endif

      // we have to handle un-shifted keys here so as to not conflict with menu accel
      {
        SHORT  flag = SHORT1FROMMP(event->mp1);

        if ((SHORT1FROMMP(event->mp1) & KC_KEYUP) == 0) {
          USHORT cmd = 0;
          switch (SHORT1FROMMP(event->mp2)) {
            case 'c':
              cmd = IDC_NEAREST_CENTER;
              break;
            case 'e':
              cmd = IDC_NEAREST_ENDPT;
              break;
            case 'l':
              cmd = IDC_NEAREST_PT;
              break;
            default:
              break;
          }
          if (cmd) {
            event->msg = WM_COMMAND;
            event->mp1 = MPFROMSHORT(cmd);
            somSelf->HandleMenuEvent(ev, frame, event);
          }
        }

      // we want to dummy a mousemove so rubber-banding and such can adjust
      // immediately upon shift key press/release
        ODBoolean wasdown = (flag & KC_PREVDOWN) != 0;
        if ( !((flag & KC_PREVDOWN) != 0) )
          null_mousemove();
      }

      // fix BEEP???
      handled = kODTrue;
      break;

    case WM_ACTIVATE:

      #ifdef ODDebug2
        somPrintf(" ...WM_ACTIVATE... \n");
      #endif
      {
      TempODFrame focusFrame = _fSession->GetArbitrator(ev)->AcquireFocusOwner(ev, _fSelectionFocus);

      if (frame == focusFrame)

        somSelf->HighlightSelection(ev, facet, 0);         // toggle mark visibility

      else if (SHORT1FROMMP(event->mp1))

         somSelf->ActivatingWindow(ev, frame);

#if 0
      if (SHORT1FROMMP(event->mp1))
         somSelf->ActivatingWindow(ev, frame);
      else
         somSelf->DeActivatingWindow(ev, frame);
#endif

      handled = kODTrue;
      }
      break;

    case WM_COMMAND:

      #ifdef ODDebug2
        somPrintf(" ...WM_COMMAND... \n");
      #endif

      {
        USHORT command = SHORT1FROMMP(event->mp1);
        switch (command) {
          case IDC_ESC :                    // exclude these from Correlate()
          case IDC_CONTEXTMENU :
          case IDC_ENTER :
          case IDC_NEAREST_PT :
          case IDC_NEAREST_ENDPT :
          case IDC_NEAREST_CENTER :
          case IDM_POPUP_DELETE :
          case IDM_POPUP_ROTATE :
            break;
          default:
            if ( command < POPUP_FIRST || command > POPUP_LAST   )
              _picture->Correlate(0);         // remove lastHit check
            break;
        }

        handled = somSelf->HandleMenuEvent(ev, frame, event);
      }
      break;

    case OD_PRINT :
      #ifdef ODDebug2
        somPrintf("GtkPart: OD_PRINT command\n");
      #endif
      // just return FALSE to let Docshell do the printing!
      break;

    case OD_HELP:
      {
        #ifdef ODDebug2
          somPrintf("GtkPart: OD_HELP command\n");
        #endif

        int helpID = HelpPanelID( event->mp1, event->mp2 );

        if (helpID){
          DisplayHelp( ev, _fSession, &_fHelp, helpID );
          handled = kODTrue;
        }
      }
      break;

    default:
      #ifdef ODDebug2
        somPrintf(" ...OTHER EVENT... %d\n", event->msg);
      #endif
      break;
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    //  somPrintf(" ...Leaving HandleEvent\n");
  #endif

  return handled;
}


// -- BASE method  (un-modified) ------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartHandleEventInEmbedded(GraphicsPart *somSelf,
                                                                 Environment *ev,
                                                                ODEventData* event,
                                                                ODFrame* frame,
                                                                ODFacet* facet,
                                                                ODFrame* embeddedFrame,
                                                                ODFacet* embeddedFacet)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","HandleEventInEmbedded");

  #ifdef ODDebug2
    somPrintf("GtkPart: null HandleEventInEmbedded\n");
  #endif

    return kODFalse;
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartMouseEnter(GraphicsPart *somSelf,  Environment *ev,
                                   ODFacet* facet, ODPoint* where)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","MouseEnter");

  #ifdef ODDebug2
    somPrintf("GtkPart: null MouseEnter\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartMouseWithin(GraphicsPart *somSelf,  Environment *ev,
                                    ODFacet* facet, ODPoint* where)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","MouseWithin");

  #ifdef ODDebug2
    somPrintf("GtkPart: null MouseWithin\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartMouseLeave(GraphicsPart *somSelf,  Environment *ev,
                                   ODFacet* facet)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","MouseLeave");

    // we may need to restore the pointer here.  I'll check later. todo (jlc 94-8)
  #ifdef ODDebug2
    somPrintf("GtkPart: null MouseLeave\n");
  #endif
}

// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope ODMenuBar*  SOMLINK GraphicsPartCreateRootMenuBar( GraphicsPart *somSelf,
                                                             Environment *ev,
                                                             ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","CreateRootMenuBar");

  #ifdef ODDebug2
    somPrintf("GtkPart: null CreateRootMenuBar\n");
  #endif

  return (ODMenuBar*)0;
}


// -- BASE method  (MODIFIED) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartAdjustMenus( GraphicsPart *somSelf,
                                               Environment *ev, ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","AdjustMenus");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering AdjustMenus\n");
  #endif

  SOM_TRY

  PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);
  ODBoolean isRoot = frame->IsRoot(ev);

  // if we are NOT ACTIVE...
  if (!pInfo->fIsActive) {

    // ...and we are the root Part
    if (isRoot) {

      ODMenuBar* theMenu = (_fSession->GetWindowState(ev))->AcquireCurrentMenuBar(ev);
      theMenu->EnableMenuItem(ev, IDMS_DOCUMENT, DOC_OPENAS, kODTrue );
      theMenu->Release(ev);
    }

  // but if we ARE ACTIVE
  } else {

    if (!_fMenuBar)  return;

    // string for loading and setting text from Resource module
    char str255[255];

    ULONG count = _picture->ObjectCount();
    ULONG selected = _picture->SelectedCount();
    BOOL  ok;

    ULONG ulObjFlags, ulStateFlags;
    _picture->ClassifySelection( &ulObjFlags, &ulStateFlags );

    // std EDIT item enablement
    _fMenuBar->EnableMenuItem( ev, IDMS_EDIT, EDIT_SELECTALL, count>selected );
    _fMenuBar->EnableMenuItem( ev, IDMS_EDIT, EDIT_DESELECTALL, selected>0 );
    _fMenuBar->EnableMenuItem( ev, IDMS_EDIT, EDIT_COPY, selected>0 );

    if (!_fReadOnly) {

      _fMenuBar->EnableMenuItem( ev, IDMS_EDIT, EDIT_DELETE, selected>0 );
      _fMenuBar->EnableMenuItem( ev, IDMS_EDIT, EDIT_CUT, selected>0 );

      ODBoolean hasODContent = kODFalse;
      ODBoolean hasGraphicsLinkspec = kODFalse;

      ODClipboard* clipboard = _fSession->GetClipboard( ev );
      ODStorageUnit* clipSU = clipboard->GetContentStorageUnit(ev);

      if ( clipSU->Exists(ev, kODPropContents, kODNULL, 0) ) {
        hasODContent = kODTrue;
        if ( clipSU->Exists(ev, kODPropContents, kKindGraphicsPart, 0) &&
             clipSU->Exists(ev, kODPropLinkSpec, kODLinkSpec, 0)    ) {
          hasGraphicsLinkspec = kODTrue;
        }
      }

      _fMenuBar->EnableMenuItem( ev, IDMS_EDIT, EDIT_PASTE_MENU, hasODContent );
      _fMenuBar->EnableMenuItem( ev, IDMS_EDIT, EDIT_PASTELINK, hasGraphicsLinkspec );

      _fMenuBar->EnableMenuItem( ev, IDMS_EDIT, EDIT_BREAKLINK, (ulObjFlags&GOT_LINK) != 0 );
    }

    _fMenuBar->EnableMenuItem( ev, IDMS_EDIT, EDIT_SEL_SHOWAS, (ulObjFlags&GOT_EMBED) != 0 );

  // VIEW items

    // FIRST, lets do the enablement of all items that are not conditional
    _fMenuBar->EnableMenuItem(ev, IDMS_VIEW, VIEW_OPENAS,     kODTrue );

    // OpenAsWindow is BROKEN... we could disable it here.
//    _fMenuBar->EnableMenuItem(ev, IDMS_VIEW, VIEW_OAWINDOW,   kODFalse );

    _fMenuBar->EnableMenuItem(ev, IDMS_VIEW, VIEW_PROPERTIES, kODTrue);

// this really shouldn't be under View at all!
// my container controls my view type (restricted by what I can handle),
// and I set my selected embeds using the EDIT_SEL option...
//    _fMenuBar->EnableMenuItem(ev, IDMS_VIEW, VIEW_SHOWAS,      kODTrue );

    _fMenuBar->EnableMenuItem( ev, IDMS_VIEW, VIEW_SHOWLINKS, kODTrue );
    _fMenuBar->SetMenuItemText(ev, IDMS_VIEW, VIEW_SHOWLINKS,
                               _fLinksVisible ? "Hide ~links" : "Show ~links" );
    SetStatusTextFromResource( VIEW_SHOWLINKS, _fLinksVisible ? 297 : 296 );

  // DOCUMENT items

    if (isRoot)
      _fMenuBar->EnableMenuItem(ev, IDMS_DOCUMENT, DOC_OPENAS, kODTrue );

  // SELECTED items

    if (!_fReadOnly) {

      _fMenuBar->EnableMenuItem( ev, IDM_SELECTED, SELECTED_ALTMARK, selected );
      _fMenuBar->CheckMenuItem(  ev, IDM_SELECTED, SELECTED_ALTMARK, _fAltMark );
      SetStatusTextFromResource( SELECTED_ALTMARK, _fAltMark ? 270 : 269 );

      _fMenuBar->EnableMenuItem( ev, IDM_SELECTED, SELECTED_ROTATE, selected>0 );

      ok = selected && (count>selected);
      _fMenuBar->EnableMenuItem( ev, IDM_SELECTED, SELECTED_ARRANGE, ok );

      ok = (ulObjFlags&GOT_GROUP) != 0;
      _fMenuBar->EnableMenuItem( ev, IDM_SELECTED, SELECTED_GROUPING, ok || selected>1 );
      _fMenuBar->EnableMenuItem( ev, IDM_SELECTED, SELECTED_GROUP,   selected>1 );
      _fMenuBar->EnableMenuItem( ev, IDM_SELECTED, SELECTED_UNGROUP, ok );

      _fMenuBar->EnableMenuItem( ev, IDM_SELECTED, SELECTED_LOCKING,   selected );
      _fMenuBar->EnableMenuItem( ev, IDM_SELECTED, SELECTED_LOCK,   (ulStateFlags&GOS_ANYUNLOCKED) != 0 );
      _fMenuBar->EnableMenuItem( ev, IDM_SELECTED, SELECTED_UNLOCK, (ulStateFlags&GOS_ANYLOCKED) != 0 );

      ok = (ulObjFlags&GOT_EMBED) != 0;
      _fMenuBar->EnableMenuItem( ev, IDM_SELECTED, SELECTED_BGNDSTUFF, ok );
      _fMenuBar->EnableMenuItem( ev, IDM_SELECTED, SELECTED_OPAQUE,
                                 ok && (ulStateFlags&GOS_ANYTRANSPARENT) );
      _fMenuBar->EnableMenuItem( ev, IDM_SELECTED, SELECTED_TRANSLUCENT,
                                 ok && (ulStateFlags&GOS_ANYOPAQUE) );
    }

  // HELP items

    _fMenuBar->EnableMenuItem(ev, IDMS_HELP, HELP_GENERAL, kODTrue);
    _fMenuBar->EnableMenuItem(ev, IDMS_HELP, HELP_INDEX, kODTrue);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving AdjustMenus\n");
  #endif
}

// ------------------------------------------------------------------
//   UNDO/REDO handling                                           aaa
// ------------------------------------------------------------------

// -- BASE method  (modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartUndoAction( GraphicsPart *somSelf,  Environment *ev,
                                                ODActionData* actionState)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","UndoAction");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering UndoAction\n");
  #endif

  SOM_TRY

  // general cast to access action CLASS
  UndoBase*  undoRec = (UndoBase*)actionState->_buffer;

  if (undoRec->uclass == UNDOCLS_BGNDCOLOR) {

    UndoBgndColor*  ubg = (UndoBgndColor*)undoRec;
    somSelf->SetBGColor(ev, ubg->frame, ubg->oldColor, kODTrue );

  } else {

    _picture->DoUndo( undoRec );
    _picture->BoundSelected();

    somSelf->PartUpdated( ev, 0, PUF_CLIPEMBEDS | PUF_REPAINTALL | PUF_UPDATELINKS );
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving UndoAction\n");
  #endif
}

// -- BASE method  (modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartRedoAction( GraphicsPart *somSelf,  Environment *ev,
                                                ODActionData* actionState)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","RedoAction");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering RedoAction\n");
  #endif

  SOM_TRY

  // general cast to access action CLASS
  UndoBase*  undoRec = (UndoBase*)actionState->_buffer;

  if (undoRec->uclass == UNDOCLS_BGNDCOLOR) {

    UndoBgndColor*  ubg = (UndoBgndColor*)undoRec;
    somSelf->SetBGColor(ev, ubg->frame, ubg->newColor, kODTrue );

  } else {

    _picture->DoRedo( undoRec );
    _picture->BoundSelected();

    somSelf->PartUpdated( ev, 0, PUF_CLIPEMBEDS | PUF_REPAINTALL | PUF_UPDATELINKS );
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving RedoAction\n");
  #endif
}

// -- BASE method  (modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDisposeActionState(GraphicsPart *somSelf,
                                            Environment *ev,
                                           ODActionData* actionState,
                                           ODDoneState doneState)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DisposeActionState");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DisposeActionState\n");
  #endif

  SOM_TRY

  // general cast to access action CLASS
  UndoBase*  undoRec = (UndoBase*)actionState->_buffer;

  if (undoRec->uclass == UNDOCLS_BGNDCOLOR) {
    delete (UndoBgndColor*)undoRec;
  } else {
    _picture->DestroyUndoStruct( undoRec );
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DisposeActionState\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope void  SOMLINK GraphicsPartWriteActionState(GraphicsPart *somSelf,
                                          Environment *ev, ODActionData* actionState,
                                         ODStorageUnitView* storageUnitView)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","WriteActionState");

  #ifdef ODDebug2
    somPrintf("GtkPart: null WriteActionState\n");
  #endif
}

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope ODActionData  SOMLINK GraphicsPartReadActionState(GraphicsPart *somSelf,
                                          Environment *ev, ODStorageUnitView* storageUnitView)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","ReadActionState");

  #ifdef ODDebug2
    somPrintf("GtkPart: null ReadActionState\n");
  #endif

    ODActionData data;
    data._maximum = 0;
    data._length = 0;
    data._buffer = kODNULL;
    return data;
}

// ------------------------------------------------------------------
//   MISCELLANEOUS                                                aaa
// ------------------------------------------------------------------

// -- BASE method  (un-modified) ------------------------------------
SOM_Scope ODULong  SOMLINK GraphicsPartGetPrintResolution(GraphicsPart *somSelf,
                                               Environment *ev,
                                              ODFrame* frame)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","GetPrintResolution");

  #ifdef ODDebug2
    somPrintf("GtkPart: null GetPrintResolution\n");
  #endif

  return 0;
}

// -- semi-BASE method  (un-modified) -------------------------------
SOM_Scope ODSemanticInterface*  SOMLINK GraphicsPartGetSemanticInterface(GraphicsPart *somSelf,
                                                                           Environment *ev)
{
    GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
    GraphicsPartMethodDebug("GraphicsPart","GetSemanticInterface");

  #ifdef ODDebug2
    somPrintf("GtkPart: null GetSemanticInterface\n");
  #endif

    return _fSemtIntf;
}

#if 0
  #ifdef __IBMC___
    #pragma lnkage (SOMInitModule, system)
  #endif
#endif

SOMEXTERN void SOMLINK SOMInitModule (long majorVersion, long minorVersion,
                                 string className)
{
   SOM_IgnoreWarning (majorVersion);  /* This function makes   */
   SOM_IgnoreWarning (minorVersion);  /* no use of the passed  */
   SOM_IgnoreWarning (className);     /* arguments.            */

   GraphicsPartNewClass ( GraphicsPart_MajorVersion, GraphicsPart_MinorVersion);
}

void EXPENTRY AddMenuResourceLast( Environment *ev, HMODULE hmodDLL, long resid, ODMenuBar *menubar, ODMenuID menuid)
{
}


SOM_Scope ISOString  SOMLINK M_GraphicsPartclsGetODPartHandlerName(M_GraphicsPart *somSelf,
                                                                     Environment *ev)
{
    /* M_GraphicsPartData *somThis = M_GraphicsPartGetData(somSelf); */
    M_GraphicsPartMethodDebug("M_GraphicsPart","M_GraphicsPartclsGetODPartHandlerName");

    string handlerName = kPartHandlerName;
    return (ISOString) handlerName;

}

SOM_Scope string  SOMLINK M_GraphicsPartclsGetODPartHandlerDisplayName(M_GraphicsPart *somSelf,
                                                                         Environment *ev)
{
    /* M_GraphicsPartData *somThis = M_GraphicsPartGetData(somSelf); */
    M_GraphicsPartMethodDebug("M_GraphicsPart","M_GraphicsPartclsGetODPartHandlerDisplayName");

    string displayName = "2D Graphics";  //kPartHandlerDisplayName;
    return displayName;
}

SOM_Scope _IDL_SEQUENCE_PartKindInfo  SOMLINK M_GraphicsPartclsGetODPartKinds(M_GraphicsPart *somSelf,
                                                                                Environment *ev)
{
    /* M_GraphicsPartData *somThis = M_GraphicsPartGetData(somSelf); */
    M_GraphicsPartMethodDebug("M_GraphicsPart","M_GraphicsPartclsGetODPartKinds");

    _IDL_SEQUENCE_PartKindInfo GraphicsPartInfo;

  SOM_TRY

    // Create structure PartKindInfo  and allocate memory for variable
    PartKindInfo * info = (PartKindInfo *)SOMMalloc(sizeof(PartKindInfo));
    info->partKindName = (ISOString) SOMMalloc(strlen(kKindGraphicsPart) + 1);
    info->partKindDisplayName = (string)SOMMalloc(strlen(kGraphicsPartKindDisplayName)+1);
    info->filenameFilters =  (string)SOMMalloc(strlen("")+1);
    info->filenameTypes =  (string)SOMMalloc(strlen("")+1);
    info->categories =  (string)SOMMalloc(strlen(kGraphicsPartCategory)+1);
    info->categoryDisplayName =  (string)SOMMalloc(strlen("2D Graphics")+1);
    info->objectID =  (string)SOMMalloc(strlen("")+1);
    // Copy the information into the structure
    strcpy(info->partKindName , kKindGraphicsPart);
    strcpy(info->partKindDisplayName, kGraphicsPartKindDisplayName);
    strcpy(info->filenameFilters, "");
    strcpy(info->filenameTypes, "");
    strcpy(info->categories, kGraphicsPartCategory);
    strcpy(info->categoryDisplayName, "2D Graphics");
    strcpy(info->objectID, "");

    GraphicsPartInfo._maximum = 1;
    GraphicsPartInfo._length = 1;
    GraphicsPartInfo._buffer = info;

  SOM_CATCH_ALL
  SOM_ENDTRY

  return GraphicsPartInfo;
}

SOM_Scope string  SOMLINK M_GraphicsPartclsGetOLE2ClassId(M_GraphicsPart *somSelf,
                                                            Environment *ev)
{
    /* M_GraphicsPartData *somThis = M_GraphicsPartGetData(somSelf); */
    M_GraphicsPartMethodDebug("M_GraphicsPart","M_GraphicsPartclsGetOLE2ClassId");

    string classID = "";
    return classID;
}

SOM_Scope string  SOMLINK M_GraphicsPartclsGetWindowsIconFileName(M_GraphicsPart *somSelf,
                                                                    Environment *ev)
{
    /* M_GraphicsPartData *somThis = M_GraphicsPartGetData(somSelf); */
    M_GraphicsPartMethodDebug("M_GraphicsPart","M_GraphicsPartclsGetWindowsIconFileName");

    string fileName = "";
    return fileName;
}
