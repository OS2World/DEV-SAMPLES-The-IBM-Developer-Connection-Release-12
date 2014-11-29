/* ******************************************************************** *  
 * Copyright International Business Machines Corporation 1995, 1996     *
 * DISCLAIMER OF WARRANTIES.                                            *  
 * The following [enclosed] code is sample code created by IBM          *  
 * Corporation. This sample code is not part of any standard or IBM     *  
 * product and is provided to you solely for the purpose of assisting   *  
 * you in the development of your applications.  The code is provided   *  
 * "AS IS". IBM MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT  *  
 * NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS *  
 * FOR A PARTICULAR PURPOSE, REGARDING THE FUNCTION OR PERFORMANCE OF   *  
 * THIS CODE.  IBM shall not be liable for any damages arising out of   *  
 * your use of the sample code, even if they have been advised of the   *  
 * possibility of such damages.                                         *  
 *                                                                      *  
 * DISTRIBUTION.                                                        *  
 * This sample code can be freely distributed, copied, altered, and     *  
 * incorporated into other software, provided that it bears the above   *  
 * Copyright notice and DISCLAIMER intact.                              *  
 * -------------------------------------------------------------------- *  
 * This file is part of the source for OpenDoc parts document in:       *
 *  IBM International Technical Support Organization Bulletin (Redbook) *  
 *  "OpenDoc : An Introduction to Part Development" (SG24-4673)         *
 *                                                                      *
 * ******************************************************************** *  
 */   

#ifndef SOM_Module_itsocntr_Source
#define SOM_Module_itsocntr_Source
#endif
#define ITSOContainerPart_Class_Source
#define M_ITSOContainerPart_Class_Source

#define VARIABLE_MACROS                /* Generate macros to access      */
                                       /* instance data                  */

/*
    Defines are used to control which sections of the os2.h and od.h files
    are included in the compile.
*/
#define INCL_DOSMODULEMGR
#define INCL_DOSRESOURCES
#define INCL_WINACCELERATORS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINPOINTERS
#define INCL_WINSCROLLBARS
#define INCL_WINSTATICS
#define INCL_WINSTDDRAG
#define INCL_WINSYS
#define INCL_WINWINDOWMGR
#define INCL_GPI
#define INCL_GPIPOLYGON
#define INCL_ODARBITRATOR
#define INCL_ODCANVAS
#define INCL_ODDRAFT
#define INCL_ODDRAGANDDROP
#define INCL_ODERRORS
#define INCL_ODFACET
#define INCL_ODFOCUSSET
#define INCL_ODFRAME
#define INCL_ODMENUBAR
#define INCL_ODSHAPE
#define INCL_ODSESSION
#define INCL_ODSTORAGEUNIT
#define INCL_ODSTORAGEUNITVIEW
#define INCL_ODTRANSFORM
#define INCL_ODTYPELIST
#define INCL_ODUNDO
#define INCL_ODINFO
#define INCL_ODWINDOW
#define INCL_ODWINDOWSTATE
#define INCL_ODPARTHANDLERINFO
#define INCL_ODPARTHANDLERREGISTRY
#define INCL_ODREGISTRYMANAGER

#include <os2.h>
#include <od.h>
#include <stdio.h>

#include "xfrmiter.xh"
#include "ordcoll.h"
#include "itsocntr.xih"
#include "itsodefs.h"
#include "itsorc.h"
#include "od2iclui.h"
#include "partinfo.hpp"

#include <winutils.h>
#include <odmenuid.h>

/*
 *   common.hpp contains helper functions and classes for drawing.
 *   focuslib.h contains the CFocus helper class.
 */

#include <common.hpp>              // Helper stuff provided by DevCon8 Toolkit
#include <focuslib.h>

/* ************************************************************************** */
/* Implementation of overriden methods...these are all the methods we         */
/* have inherited from ODPart (and above).                                    */
/* ************************************************************************** */

// ************************************************
//   Initialize/Unitialize Methods
// ************************************************

// ------------------------------------------------
//  Method:             somInit
//
//  Introduced by:      SOMObject
//
//    Called when SOM object is first instantiated--
//      Used to initialize instance variables and
//      do any other object initialization processing.
//      We set all instance variables to NULL.  
//
//    Other part specific initiailization can be 
//      handled in InitPart or InitPartFromStorage
// ------------------------------------------------

SOM_Scope void  SOMLINK somInit(ITSOContainerPart *somSelf)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","somInit");

    ITSOContainerPart_parent_ODPart_somInit(somSelf);

    DosQueryModuleHandle(MODULENAME, &_hmod);   // Used later to load resources

    _CurrentView          = kODNULL;          
    _FrameView            = kODNULL;
    _LargeIconView        = kODNULL;
    _PresDefault          = kODNULL;
    _SmallIconView        = kODNULL;
    _ThumbnailView        = kODNULL;
    _fAlreadyActive       = kODFalse;
    _fContents            = kODNULL;
    _fDisplayFrames       = kODNULL;
    _fEmbeddedFrames      = kODNULL;
    _fExtension           = kODNULL;
    _fFocusSet            = kODNULL;
    _fFrameGroupIDCounter = 1;                   
    _fHaveStatusLineFocus = kODFalse;
    _fIgnoreContextMenu   = kODFalse;
    _fKeyFocus            = kODNULL;
    _fMenuBar             = kODNULL;
    _fMenuFocus           = kODNULL;
    _fMouseFocus          = kODNULL;
    _fMouseMode           = kNormal;
    _fNeedToExternalize   = kODFalse;
    _fNumPolygons         = 0;
    _fPartWrapper         = kODNULL;
    _fPolygons            = kODNULL;
    _fPopup               = kODNULL;
    _fSavedFacetClipShapes= kODNULL;
    _fSelectSizingHandlesShape      = kODNULL;
    _fSelection           = kODNULL;
    _fSelectionFocus      = kODNULL;
    _fSelectionFrame      = kODNULL;
    _fSequence            = 0;
    _fSession             = kODNULL;
    _fStatusFocus         = kODNULL;
    _fStatusLn            = kODNULL;
    _fTrackingFacet       = kODNULL;
    _fTrackingMode        = 0;
    _fViewExtension       = kODNULL;
    _fWindowID            = 0;
    _ptEnd.x         = _ptEnd.y         = 0;
    _ptMouseOffset.x = _ptMouseOffset.y = 0;
    _ptMouseStart.x  = _ptMouseStart.y  = 0;
    _ptOrigin.x      = _ptOrigin.y      = 0;
    _ptRef.x         = _ptRef.y         = 0;
    _dadSelection         = kODNULL; 
}

// ------------------------------------------------
//  Method:             somUninit
//
//  Introduced by:      SOMObject
//
//    Called when SOM object is destroyed--
//      Free up any memory allocated by the object 
//      do any other object destruction processing.
// ------------------------------------------------

SOM_Scope void  SOMLINK somUninit(ITSOContainerPart *somSelf)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","somUninit");

    Environment* ev = somGetGlobalEnvironment();

    _hmod           = 0;

    if (_fContents)      { delete _fContents; }
    if (_fDisplayFrames) { delete _fDisplayFrames; }
    if (_fEmbeddedFrames){ delete _fEmbeddedFrames; }
    if (_fSelection)     { delete _fSelection; }
    if (_fViewExtension) { delete _fViewExtension; }
    if (_fSelectSizingHandlesShape)   { _fSelectSizingHandlesShape->Release(ev); }
    if (_fStatusLn)      { _fStatusLn->Release(ev); }
 
    if (_fSavedFacetClipShapes->Count()) {
      while (_fSavedFacetClipShapes->Count())
      {
        FacetClip* facetClip = (FacetClip*)_fSavedFacetClipShapes->First();
        facetClip->clip->Release(ev);
        _fSavedFacetClipShapes->Remove(facetClip);
        delete facetClip;
      }
      delete _fSavedFacetClipShapes;
    } /* endif */

    ITSOContainerPart_parent_ODPart_somUninit(somSelf);
}


// ------------------------------------------------
//  Method:             InitPart
//
//  Introduced by:      ODPart
//
//   InitPart is called when a part instance is being
//   created from scratch (not from a storage unit.  
//   This may occur during initial part registration or
//   if this part is created dynamically by another part.
// ------------------------------------------------

SOM_Scope void  SOMLINK InitPart(ITSOContainerPart *somSelf, 
                                 Environment *ev, 
                                 ODStorageUnit* storageUnit, 
                                 ODPart* partWrapper)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","InitPart");

    if (somSelf->IsInitialized(ev)) { return; }  // Already initialized
      
    _fPartWrapper = partWrapper;                 // Store our wrapper object
    somSelf->InitPersistentObject(ev, storageUnit);
    somSelf->ICPCommonInit(ev);                  // common part initialization
    somSelf->ICPInitSU(ev,storageUnit);
}

// ------------------------------------------------
//  Method:             InitPartFromStorage
//
//  Introduced by:      ODPart
//
//   InitPartFromStorage is called when a part instance 
//   is being recreated from storage (a storage unit).
//   Typically this is when adding a part to a document from
//   a template or when resotring a document from storage.
// ------------------------------------------------

SOM_Scope void  SOMLINK InitPartFromStorage(ITSOContainerPart *somSelf,
                                            Environment       *ev,
                                            ODStorageUnit     *storageUnit,
                                            ODPart            *partWrapper)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","InitPartFromStorage");

    if (somSelf->IsInitialized(ev)) return;      // Already Initialized

    _fPartWrapper = partWrapper;
    somSelf->InitPersistentObjectFromStorage(ev, storageUnit);
    somSelf->ICPCommonInit(ev);
    somSelf->ICPReadFrameGroup(ev,storageUnit);
    somSelf->ICPReadEmbeddedFrames(ev,storageUnit);
}

// ------------------------------------------------
//  Method:             Externalize
//
//  Introduced by:      ODPersistentObject
//
//   Externalize is called whenever a part must store
//   itself.  For instance,when a docuemtn is closing
//   or a user selects SAVE.
// ------------------------------------------------

SOM_Scope void  SOMLINK Externalize(ITSOContainerPart *somSelf, 
                                    Environment *ev)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","Externalize");

    ODStorageUnit    * su = somSelf->GetStorageUnit(ev);

    somSelf->ICPSaveFrameGroup(ev,su);
    somSelf->ICPSaveEmbeddedFrames(ev,su);
    _fNeedToExternalize = kODFalse;
}

// ------------------------------------------------
//  Method:             ExternalizeKinds
//
//  Introduced by:      ODPart
//
//   ExternalizeKinds is called, asking our part to 
//   externalize our data as a specific kind (or set of kinds).
//
//   The following code shows a basic template for doing
//   this, though since we have no intrinsic data we
//   do not really support ExternalizeKinds.
// ------------------------------------------------

SOM_Scope void  SOMLINK ExternalizeKinds(ITSOContainerPart *somSelf, 
                                         Environment *ev, 
                                         ODTypeList* kindset)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ExternalizeKinds");

    ODTypeListIterator *tli;

    if (kindset) {               // kindset is a lit of kinds to externalize
       tli = kindset->CreateTypeListIterator(ev);  // Create iterator

        for (ODType type = tli->First(ev);
             tli->IsNotComplete(ev);
             type = tli->Next(ev))
        {
        // Should externalize specific type here...
        // Not implemented in this part...
        }
    } /* endif */
    _fNeedToExternalize = kODTrue;
    somSelf->Externalize(ev);  // With different types not supported, we'll
                               // just externalize everything
    delete tli;
}


// ------------------------------------------------
//  Method:             WritePartInfo
//
//  Introduced by:      ODPart
//
//    Write out frame specific part information
//    to the frames SU.  This method is called
//    by the frame during its externaliztion
//    process.
// ------------------------------------------------

SOM_Scope void  SOMLINK WritePartInfo(ITSOContainerPart *somSelf, 
                                      Environment *ev, 
                                      ODInfoType partInfo, 
                                      ODStorageUnitView* storageUnitView)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","WritePartInfo");

    if (partInfo) {
      ODBoolean needsActivating = ((PartInfoRec*)partInfo)->fNeedsActivating ||
                                  ((PartInfoRec*)partInfo)->fIsActive;
      StorageUnitViewSetValue(storageUnitView, ev, sizeof(ODBoolean), (ODValue)&needsActivating);
    }
}

// ------------------------------------------------
//  Method:             ReadPartInfo
//
//  Introduced by:      ODPart
//
//    Read in frame specific part information
//    from the frames SU.  This method is called
//    by the frame during its internalization
//    process.
// ------------------------------------------------
SOM_Scope ODInfoType  SOMLINK ReadPartInfo(ITSOContainerPart *somSelf, 
                                           Environment *ev, 
                                           ODFrame* frame, 
                                           ODStorageUnitView* storageUnitView)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ReadPartInfo");

    if (storageUnitView->GetSize(ev)) {
      PartInfoRec* partInfo = new PartInfoRec;
      ODBoolean needsActivating;
      StorageUnitViewGetValue(storageUnitView, ev, sizeof(ODBoolean), (ODValue)&(needsActivating));
      partInfo->fNeedsActivating = needsActivating;
      return((ODInfoType)partInfo);
    } else {
      return((ODInfoType)kODNULL);
    }
}

// ------------------------------------------------
//  Method:             ClonePartInfo
//
//  Introduced by:      ODPart
//
//    Called to clone a frames part info data
//    into a stroage unt view.
//    
//    We have not implemented this method
// ------------------------------------------------

SOM_Scope void  SOMLINK ClonePartInfo(ITSOContainerPart *somSelf, 
                                      Environment *ev, ODDraftKey key, 
                                      ODInfoType partInfo, ODStorageUnitView* storageUnitView, 
                                      ODFrame* scope)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ClonePartInfo");

}

// ------------------------------------------------
//  Method:             CloneInto
//
//  Introduced by:      ODPart
//
//    Clones the contents of one SU to another...
//    CloneInto creates a new storage unit (toSU). 
//    During registration this method creates the 
//    storage unit for the template.
// ------------------------------------------------

SOM_Scope void  SOMLINK CloneInto(ITSOContainerPart *somSelf, 
                                  Environment *ev, 
                                  ODDraftKey key, 
                                  ODStorageUnit* toSU, 
                                  ODFrame* scope)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","CloneInto");

    ODID scopeFrameID = 0;
    ODStorageUnit * mySU = somSelf->GetStorageUnit(ev);
    if (scope != kODNULL)
        scopeFrameID = scope->GetStorageUnit(ev)->GetID(ev);
    mySU->CloneInto(ev, key, toSU, scopeFrameID);

}


// ----------------------------------
// Drag and Drop Protocol Methods
// ----------------------------------

// ------------------------------------------------
//  Method:             FulFillPromise
//
//  Introduced by:      ODPart
//
//  Fulfills the promise of copying data. I am still
//  not sure if this is for dragging the ITSO
//  container itself or for dragging of parts and
//  dropping them // on the ITSOContainer
// ------------------------------------------------

SOM_Scope void  SOMLINK FulfillPromise(ITSOContainerPart *somSelf, 
                                       Environment *ev, 
                                       ODStorageUnitView* promiseSUView)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","FulfillPromise");

    ODPropertyName property = promiseSUView->GetProperty(ev);

    if (strcmp(property, kPropFrameInfo) == 0) {
       Proxy* selection = kODNULL;
       StorageUnitViewGetValue(promiseSUView, ev, sizeof(Proxy), &selection);
       promiseSUView->SetOffset(ev, 0);
       ODID frameInfoData = selection->frame->GetID(ev);
       StorageUnitViewSetValue(promiseSUView, ev, sizeof(ODID), (ODValue) &frameInfoData);
    } else {
       Proxy         * selection = _dadSelection;
       ODStorageUnit * unit      = promiseSUView->GetStorageUnit(ev);
       ODDraft       * fromDraft = somSelf->GetStorageUnit(ev)->GetDraft(ev);
       ODDraftKey      key = 0;

       try
       {
          key = fromDraft->BeginClone(ev, unit->GetDraft(ev), kODCloneCut);
          ODPart* tempPart = selection->frame->AcquirePart(ev);
          fromDraft->Clone(ev,
                           key,
                           tempPart->GetID(ev),
                           unit->GetID(ev),
                           selection->frame->GetID(ev));
//          tempPart->Release(ev);  // DevCon 8 bug, not all Acquire..()'s inc RefCnt 
          fromDraft->EndClone(ev, key);
       }
       catch (...)
       {
           if (key != 0) {
             fromDraft->AbortClone(ev, key);
           }
       }

       // if dragging one frame, write its shape
       unit->AddProperty(ev, kODPropFrameShape);
       ODShape* tempShape = selection->frame->AcquireFrameShape(ev, kODNULL);
       tempShape->WriteShape(ev, unit);
       tempShape->Release(ev); tempShape = kODNULL;
    }
}

// ------------------------------------------------
//  Method:             Drop
//
//  Introduced by:      ODPart
//
//  Called when another object is dropped on one of our
//  facets.
// ------------------------------------------------

SOM_Scope ODDropResult  SOMLINK Drop(ITSOContainerPart *somSelf, 
                                     Environment *ev, 
                                     ODDragItemIterator* dropInfo,
                                     ODFacet* facet,
                                     ODPoint* where)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","Drop");

    ODULong              attributes;
    short                movePart;
    ODStorageUnit       *dropSU;
    ODDropResult         dropResult = kODDropMove;
    DRAGITEM             DragItem;
    CHAR                 szSelectedRMF[CCHMAXPATH];
    ODStorageUnitView   *dropView;
    ODStorageUnit       *renderedSU;

    somSelf->ICPInvertDragHilite(ev, facet);  // presumably I've highlighted myself,
                                              //  so turn it off now.
    attributes = _fSession->GetDragAndDrop(ev)->GetDragAttributes(ev);
    movePart = (attributes & kODDropIsInSourceFrame);
    ODBoolean  notDone = kODTrue;

    for (dropSU = dropInfo->First(ev); 
         dropSU && notDone; 
         dropSU = dropInfo->Next(ev))
    {
        if ( !dropSU->Exists(ev, kODPropContents, (ODValueType)kODDragitem, 0)) {
          continue;
        }

        dropSU->Focus(ev, kODPropContents, kODPosUndefined,
                      (ODValueType)kODDragitem,   0, kODPosUndefined);
        StorageUnitGetValue(dropSU, ev,
                            sizeof( DRAGITEM ), &DragItem );
        szSelectedRMF[0] = 0;
        if (!dmSelectRMF(&DragItem, szSelectedRMF)) {
          // can't find something we like....
          continue;
        }

        ODStorageUnitView *dropView = dropSU->CreateView(ev);

        if (! _fSession->
                 GetDragAndDrop(ev)->
                    GetDataFromDragManager(ev,
                                           dropView,
                                           szSelectedRMF,
                                           &renderedSU )){
          // error in transfer
          continue;
        }

        // NOTE: the target part must not free / release the renderedSU.
        // it will be cleaned up when we return from Drop() by the drag
        // manager.

        if (movePart) {
            somSelf->ICPHandleLocalDrop(ev,renderedSU,facet,where);
        } else {
            somSelf->ICPHandleRemoteDrop(ev,renderedSU,facet,where);
        }
    }
    somSelf->GetStorageUnit(ev)->GetDraft(ev)->SetChangedFromPrev(ev);
    return dropResult;
}

// ------------------------------------------------
//  Method:             DropComplete
//
//  Introduced by:      ODPart
//
//  Notifies us (the source) that the last Drag/Drop
//  action has completed.
//
//  We have not implemented this...
// ------------------------------------------------
SOM_Scope void  SOMLINK DropCompleted(ITSOContainerPart *somSelf, 
                                      Environment *ev, 
                                      ODPart* destPart, 
                                      ODDropResult dropResult)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","DropCompleted");
}

// ------------------------------------------------
//  Method:             DragEnter
//
//  Introduced by:      ODPart
//
//  Notifies our part when a drag operation
//  has entered one of our facets.
//
// ------------------------------------------------

SOM_Scope ODDragResult  SOMLINK DragEnter(ITSOContainerPart *somSelf, 
                                          Environment *ev, 
                                          ODDragItemIterator* dragInfo, 
                                          ODFacet* facet, 
                                          ODPoint* where)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","DragEnter");

    somSelf->ICPInvertDragHilite(ev, facet);  // Highlights our agg.
                                              // clipshape to show our
                                              // drop area.
    return((ODDragResult)MRFROM2SHORT(DOR_DROP, DO_MOVE));
}

// ------------------------------------------------
//  Method:             DragWithin
//
//  Introduced by:      ODPart
//
//  Notifies our part when a drag operation
//  coninues within our part.
//
//  Continue dragging...
// ------------------------------------------------

SOM_Scope ODDragResult  SOMLINK DragWithin(ITSOContainerPart *somSelf,
                                           Environment *ev, 
                                           ODDragItemIterator* dragInfo, 
                                           ODFacet* facet, 
                                           ODPoint* where)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","DragWithin");

    return((ODDragResult)MRFROM2SHORT(DOR_DROP, DO_MOVE));
}

// ------------------------------------------------
//  Method:             DragLeave
//
//  Introduced by:      ODPart
//
//  Notifies our part when a drag operation
//  has left our facet.
//
//  Reset our highlighting
// ------------------------------------------------
SOM_Scope void  SOMLINK DragLeave(ITSOContainerPart *somSelf, 
                                  Environment *ev, 
                                  ODFacet* facet, 
                                  ODPoint* where)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","DragLeave");

    somSelf->ICPInvertDragHilite(ev, facet); // Takes away our highlight
}

// ----------------------------------
// Methods Releated to Managing the Foci
// ----------------------------------

// ------------------------------------------------
//  Method:             FocusAcquired
//
//  Introduced by:      ODPart
//
//   Called whenever one of our frames receives
//   a focus.
// ------------------------------------------------

SOM_Scope void  SOMLINK FocusAcquired(ITSOContainerPart *somSelf, 
                                      Environment *ev, 
                                      ODTypeToken focus, 
                                      ODFrame* ownerFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","FocusAcquired");

    if (focus == _fSelectionFocus) {                  // If selection (activation),
      PartInfoRec* pInfo = (PartInfoRec*) ownerFrame->  // set our fIsActive indicator
                                             GetPartInfo(ev); // to true
      pInfo->fIsActive = kODTrue;
      ownerFrame->SetPartInfo(ev, (ODInfoType)pInfo);
    } else {
      if (focus == _fMenuFocus) {                     // If menu, show
        _fMenuBar->Display(ev);                       // our menu bar.
      }
    }
}

// ------------------------------------------------
//  Method:             BeginRelinquishFocus
//
//  Introduced by:      ODPart
//
//  Called to request we give up specified focus.
//
//  Return True to concur.
// ------------------------------------------------
SOM_Scope ODBoolean  SOMLINK BeginRelinquishFocus(ITSOContainerPart *somSelf, 
                                                  Environment *ev, 
                                                  ODTypeToken focus, 
                                                  ODFrame* ownerFrame, 
                                                  ODFrame* proposedFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","BeginRelinquishFocus");

    return(kODTrue);
}

// ------------------------------------------------
//  Method:             CommitRelinquishFocus
//
//  Introduced by:      ODPart
//
//  Called to commit our relinquish started with
//  BeginRelinquishFocus.
//  
//  Call our own FocusLost method... FocusLost
//  is normally called when we have unexpectedly
//  lost the focus...but our processing is the same
//  so we will place all focus lost processing there...
// ------------------------------------------------
SOM_Scope void  SOMLINK CommitRelinquishFocus(ITSOContainerPart *somSelf, 
                                              Environment *ev, 
                                              ODTypeToken focus, 
                                              ODFrame* ownerFrame, 
                                              ODFrame* proposedFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","CommitRelinquishFocus");

    somSelf->FocusLost(ev, focus, ownerFrame);
}

// ------------------------------------------------
//  Method:             AbortRelinquishFocus
//
//  Introduced by:      ODPart
//
//  Called to abort our relinquish started with
//  BeginRelinquishFocus.
//  
//  Usually because another part holding part of
//  a focus set refused to relinquish their focus.
// ------------------------------------------------
SOM_Scope void  SOMLINK AbortRelinquishFocus(ITSOContainerPart *somSelf, 
                                             Environment *ev, 
                                             ODTypeToken focus, 
                                             ODFrame* ownerFrame,
                                             ODFrame* proposedFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","AbortRelinquishFocus");

    // No state to restore as BeginRelinquishFocus did not do any specific release
}

// ------------------------------------------------
//  Method:             FocusLost
//
//  Introduced by:      ODPart
//
//  Called when we 'unexpectedly' lose focus.
//  
//  We call this from our own methods so all focus
//  lost processing is in one place.
// ------------------------------------------------
SOM_Scope void  SOMLINK FocusLost(ITSOContainerPart *somSelf, 
                                  Environment *ev, 
                                  ODTypeToken focus, 
                                  ODFrame* ownerFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","FocusLost");
 
    if (focus == _fSelectionFocus) {
      if (_fMouseMode != kNormal)
         somSelf->ICPResetMouseMode(ev);

      PartInfoRec* pInfo = (PartInfoRec*) ownerFrame->GetPartInfo(ev);
      pInfo->fIsActive = kODFalse;
      ownerFrame->SetPartInfo(ev, (ODInfoType)pInfo);    
      ownerFrame->Invalidate(ev, kODNULL, kODNULL);
      if (_fSelection->Count()) {
        somSelf->ICPEmptySelection(ev);
        somSelf->ICPClipSelectionFromEmbeddedFrames(ev, ownerFrame);
      }
    } else 
       if (focus == _fMouseFocus) {
           somSelf->ICPMouseFocusLost(ev, ownerFrame);
       } else 
          if (focus == _fStatusFocus) {
             _fHaveStatusLineFocus = kODFalse;// 124281
         }
}

// ************************************************
//  Methods which affect our DisplayFrames
// ************************************************

// ------------------------------------------------
//  Method:             AttachSourceFrame
//
//  Introduced by:      ODPart
//
//   Notifies our part that another frame (frame)
//   is being added to our part and that it should
//   mirror the contents of the specified display
//   frame (sourceFrame) of our part.
// ------------------------------------------------

SOM_Scope void  SOMLINK AttachSourceFrame(ITSOContainerPart *somSelf,
                                          Environment *ev, 
                                          ODFrame* frame, 
                                          ODFrame* sourceFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","AttachSourceFrame");

    if ((_fDisplayFrames->Contains(frame)) &&        // frame is one of ours, and...
        (sourceFrame) &&                             // source is not null and...
        (_fDisplayFrames->Contains(sourceFrame))) {  // source is one of ours

       frame->                                       // New frame group ID
          SetFrameGroup(ev, sourceFrame->
                              GetFrameGroup(ev) + 1);
       frame->                                       // Use same sequence number
          ChangeSequenceNumber(ev,sourceFrame->
                                    GetSequenceNumber(ev));
       somSelf->                                     // Duplicates each embedded
         ICPDuplicateAndAttachEmbedded(ev,           // frame and attaches it to
                                       frame,        // its counterpart in 
                                       sourceFrame); // the source frame
    } else {
      THROW(kODErrInvalidFrame);
    }
}

// ------------------------------------------------
//  Method:             DisplayFrameAdded
//
//  Introduced by:      ODPart
//
//   Called whenever a display frame is added for our part.
//   We should update our internal list of frames we are
//   displayed in and set any partinfo for this frame.
// ------------------------------------------------

SOM_Scope void  SOMLINK DisplayFrameAdded(ITSOContainerPart *somSelf, 
                                          Environment *ev, 
                                          ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","DisplayFrameAdded");

    ODPart* tempPart = frame->AcquirePart(ev);
    tempPart->Acquire(ev);
    if (tempPart == _fPartWrapper) {   // frame belongs to me

      PartInfoRec* pInfo = new PartInfoRec;
      if (frame->IsRoot(ev)) {                      // Ensure our part is
          pInfo->fNeedsActivating = kODTrue;        // activated when it is the
      }                                             // root.  This var. checked by
      frame->SetPartInfo(ev, (ODInfoType)pInfo);    // ICPActivatingWindow

      frame->SetDroppable(ev, kODTrue);             // Could be used to determine
                                                    // if items can be dropped
                                                    // on us. (Not used by us currently.)
      _fDisplayFrames->AddLast(frame);              // Add to our internal list of our
      frame->Acquire(ev);                           // display frames.  Bump ref count
                                                    // since we are caching this reference.

      if (frame->GetViewType(ev) == kODNullTypeToken) {
         frame->SetViewType(ev, _FrameView);
      }
      if (frame->GetPresentation(ev) == kODNullTypeToken) {
        frame->SetPresentation(ev, _PresDefault);
      }
      _fNeedToExternalize = kODTrue;                 // We have changed... 
    } else {
      THROW(kODErrInvalidFrame);
    }
// tempPart->Release(ev);   // DevCon 8 bug, not all Acquire..()'s inc RefCnt 
}
// ------------------------------------------------
//  Method:             DisplayFrameConnected
//
//  Introduced by:      ODPart
//
//   Called whenever a display frame is connected to our part.
//   We should update our internal list of frames we are
//   displayed in and set any partinfo for this frame.
//
//   This method is similar to DisplayFrameAdded,
//   it is normally called when a document is being
//   restored. The peristent frame is created first
//   and then connected to our part during part
//   initialization.
// ------------------------------------------------

SOM_Scope void  SOMLINK DisplayFrameConnected(ITSOContainerPart *somSelf, 
                                              Environment *ev, 
                                              ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","DisplayFrameConnected");
    ODPart* tempPart = frame->AcquirePart(ev);
    tempPart->Acquire(ev);
    if (tempPart == _fPartWrapper) {   // frame belongs to me
       _fDisplayFrames->AddLast(frame);              // Add to our internal list of our      
       frame->Acquire(ev);                           // display frames.  Bump ref count      
                                                     // since we are caching this reference. 
       frame->SetDroppable(ev, kODTrue);             // Could be used to determine         
                                                     // if items can be dropped            
                                                     // on us. (Not used by us currently.) 
       _fNeedToExternalize = kODTrue;                // We have changed...

    } else {
       THROW(kODErrInvalidFrame);
    }
    tempPart->Release(ev);  
}
// ------------------------------------------------
//  Method:             DisplayFrameRemoved
//
//  Introduced by:      ODPart
//
//   Called whenever one of our display frames
//   is being removed.
// ------------------------------------------------


SOM_Scope void  SOMLINK DisplayFrameRemoved(ITSOContainerPart *somSelf, 
                                            Environment *ev, 
                                            ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","DisplayFrameRemoved");

    if (frame && (_fDisplayFrames->Contains(frame))) { // Is it in our list of frames?
       if (frame == _fSelectionFrame) {                // Is it the selected frame?
          somSelf->ICPEmptySelection(ev);
       }

       somSelf->ICPRemoveEmbeddedFrames(ev,frame);    // Remove all frames embedded
                                                      // within this frame.
       somSelf->ICPRemovePartWindows(ev,frame);       // Remove part windows
                                                      // associated with this frame
       _fSession->                                           
          GetArbitrator(ev)->
             RelinquishFocusSet(ev, _fFocusSet, frame);      // Relinquish Focus

       PartInfoRec* pInfo = (PartInfoRec*)frame->GetPartInfo(ev);
       frame->SetPartInfo(ev, (ODInfoType)kODNULL);          // Set Partinfo to null
       delete pInfo;                                         // and release memory

       _fDisplayFrames->Remove(frame);                       // Remove from our list
       frame->Release(ev);                                   // Decrement ref count

       _fNeedToExternalize = kODTrue;
    } else {
        THROW(kODErrInvalidFrame);
    }
}

// ------------------------------------------------
//  Method:             DisplayFrameClosed
//
//  Introduced by:      ODPart
//
//   Called when a frame is being removed from the 
//   draft.
// ------------------------------------------------

SOM_Scope void  SOMLINK DisplayFrameClosed(ITSOContainerPart *somSelf, 
                                           Environment *ev, 
                                           ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","DisplayFrameClosed");

    if (_fDisplayFrames->Contains(frame)) {
       ODxOrderedCollection* embeddedFrames = new ODxOrderedCollection;
       {
          ODEmbeddedFramesIterator* iter=somSelf->        // Copy collection
                                          CreateEmbeddedFramesIterator(ev,kODNULL);
          for (ODFrame* embeddedFrame = (ODFrame*) iter->First(ev);
              iter->IsNotComplete(ev);
              embeddedFrame = (ODFrame*) iter->Next(ev))
          {
             embeddedFrames->AddLast(embeddedFrame);
          }
       }

       ODxOrderedCollectionIterator iter(embeddedFrames);     // For each embedded frame...
       for (ODFrame* embeddedFrame = (ODFrame*) iter.First(); 
           iter.IsNotComplete();                              
           embeddedFrame = (ODFrame*) iter.Next())
       {
          if (embeddedFrame->AcquireContainingFrame(ev) == frame) {  // if it is embedded in the 
                                                                     // bug in acf() - no increment refcount
             embeddedFrame->Close(ev);                               // frame being closed, close it.
             _fEmbeddedFrames->Remove(embeddedFrame);                // Remove from our lists...
             _fContents->Remove(somSelf->ICPProxyForFrame(ev,embeddedFrame)); 
          }
       }
       delete embeddedFrames;

       _fSession->                                                  // Relinquish any focus
          GetArbitrator(ev)->                                       // it may have...
             RelinquishFocusSet(ev, _fFocusSet, frame);

       PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);  // Free up memory
       frame->SetPartInfo(ev, (ODInfoType) kODNULL);                // associated with PartInfo
       delete pInfo;
                                                                    // Remove closed frame
       _fDisplayFrames->Remove(frame);                              // from our list.
       frame->Release(ev);
       _fNeedToExternalize = kODTrue;
    } else {
       THROW(kODErrInvalidFrame);
    }
}

// ------------------------------------------------
//  Method:             FrameShapeChanged
//
//  Introduced by:      ODPart
//
//   Called when our frame shape has been changed.
//   For our part, we renegitate in an attempt
//   to ensure we have a minimum size. (This is done
//   in our ICPResize() method.)
//
// ------------------------------------------------

SOM_Scope void  SOMLINK FrameShapeChanged(ITSOContainerPart *somSelf, 
                                          Environment *ev, 
                                          ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","FrameShapeChanged");

    if (_fDisplayFrames->Contains(frame)) {
      somSelf->ICPResize(ev, frame, 0);
      somSelf->ICPClipEmbeddedFacets(ev, FirstFacet(ev, frame));
    } else {
      THROW(kODErrInvalidFrame);
    }
}

// ------------------------------------------------
//  Method:             ViewTypeChanged
//
//  Introduced by:      ODPart
//
//   Called when one of our frame's view type
//   has been changed.
//
// ------------------------------------------------
SOM_Scope void  SOMLINK ViewTypeChanged(ITSOContainerPart *somSelf, 
                                        Environment *ev, 
                                        ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ViewTypeChanged");

    if (_fDisplayFrames->Contains(frame)) {       // Valid Frame
        ODTypeToken view = frame->GetViewType(ev);// Get new view type
        if (view == kODNullTypeToken) {           // If null....set to default
           frame->SetViewType(ev, _FrameView);    // of frameview.
           view = _FrameView;
        }

      ODRect Rect = {0, 0, 0, 0};
      if (view == _FrameView) {
        frame->ChangeUsedShape(ev, kODNULL, kODNULL);
        _CurrentView = _FrameView;
      } else {
        ODShape* newUsedShape = frame->CreateShape(ev);
        if (view == _LargeIconView) {
          Rect.right = MAKEFIXED(LARGEICONSIZE, 0);
          Rect.top   = MAKEFIXED(LARGEICONSIZE, 0);
          _CurrentView = _LargeIconView;
        } else {
          if (view == _SmallIconView) {
            Rect.right = MAKEFIXED(SMALLICONSIZE, 0);
            Rect.top   = MAKEFIXED(SMALLICONSIZE, 0);
            _CurrentView = _SmallIconView;
          } else {
            if (view == _ThumbnailView) {
              Rect.right = MAKEFIXED(THUMBNAILSIZE, 0);
              Rect.top   = MAKEFIXED(THUMBNAILSIZE, 0);
              _CurrentView = _ThumbnailView;
            }
          }
        }
        newUsedShape->SetRectangle(ev, &Rect);
        frame->ChangeUsedShape(ev, newUsedShape, kODNULL);
        newUsedShape->Release(ev);
      }
      frame->SetViewType(ev, _CurrentView);
      ODFrame *containingFrame = frame->AcquireContainingFrame(ev);
// bug in acf() - no inc refcount
      if (containingFrame != kODNULL) {
          containingFrame->Invalidate(ev, kODNULL, kODNULL);
      }
// bug above      containingFrame->Release(ev);
    } else {
      THROW(kODErrInvalidFrame);
    }
}

// ************************************
//   Methods related to menu's
// ************************************

// ------------------------------------------------
//  Method:             CreateRootMenuBar
//
//  Introduced by:      ODPart
//
//   Called when we are the root part.  Allows us
//   to toally replace the default BaseMenuBar.
//   kODNULL implies use of BaseMenuBar
// ------------------------------------------------
SOM_Scope ODMenuBar*  SOMLINK CreateRootMenuBar(ITSOContainerPart *somSelf, 
                                                Environment *ev, 
                                                ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","CreateRootMenuBar");

    return(kODNULL);
}

// ------------------------------------------------
//  Method:             AdjustMenus
//
//  Introduced by:      ODPart
//
//   Called when we have the menu focus and the
//   use clicks on the menu bar.  we can enable and
//   disable appropriate menu items at this time.
// ------------------------------------------------
SOM_Scope void  SOMLINK AdjustMenus(ITSOContainerPart *somSelf, 
                                    Environment *ev, 
                                    ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","AdjustMenus");

    PartInfoRec* pInfo = (PartInfoRec *) frame->GetPartInfo(ev);

    _fMenuBar->EnableMenuItem(ev,IDMS_VIEW,VIEW_SHOWAS,kODTrue);

    _fMenuBar->EnableMenuItem(ev,IDMS_VIEW,VIEW_SASMALLICON, kODTrue);
    _fMenuBar->EnableMenuItem(ev,IDMS_VIEW,VIEW_SALARGEICON, kODTrue);
    _fMenuBar->EnableMenuItem(ev,IDMS_VIEW,VIEW_SATHUMBNAIL, kODTrue);
    _fMenuBar->EnableMenuItem(ev,IDMS_VIEW,VIEW_SAFRAME, kODTrue);

    _fMenuBar->CheckMenuItem(ev,IDMS_VIEW,VIEW_SASMALLICON,_CurrentView==_SmallIconView);
    _fMenuBar->CheckMenuItem(ev,IDMS_VIEW,VIEW_SALARGEICON,_CurrentView==_LargeIconView);
    _fMenuBar->CheckMenuItem(ev,IDMS_VIEW,VIEW_SATHUMBNAIL,_CurrentView==_ThumbnailView);
    _fMenuBar->CheckMenuItem(ev,IDMS_VIEW,VIEW_SAFRAME,_CurrentView==_FrameView);

    _fMenuBar->EnableMenuItem(ev,IDMS_VIEW, VIEW_PROPERTIES, kODTrue);

    _fMenuBar->EnableMenuItem(ev,IDMS_VIEW,VIEW_OPENAS,kODTrue);
    _fMenuBar->EnableMenuItem(ev,IDMS_VIEW,VIEW_OATREE, kODTrue);
    _fMenuBar->EnableMenuItem(ev,IDMS_VIEW,VIEW_OADETAILS, kODTrue);
    _fMenuBar->EnableMenuItem(ev,IDMS_VIEW,VIEW_OAICON, kODTrue);

    switch (_fSelection->Count())
    {
       case 0:     // no selection
          break;
       case 1:     // single selection
          {
        _fMenuBar->EnableMenuItem(ev,IDMS_EDIT, EDIT_DELETE, kODTrue);
        _fMenuBar->EnableMenuItem(ev,IDMS_EDIT, EDIT_MOVE, kODTrue);
          };
          break;
       default: // multiple selection
          {
        _fMenuBar->EnableMenuItem(ev,IDMS_EDIT, EDIT_DELETE, kODTrue);
          }
    }
}

// -------------------------------------------
//  Embedded Frame handling methods
// -------------------------------------------

// ------------------------------------------------
//  Method:             RevealFrame
//
//  Introduced by:      ODPart
//
//   Called to ask us to ensure an embedded parts
//   frame is scrolled into view.
// ------------------------------------------------

SOM_Scope ODBoolean  SOMLINK RevealFrame(ITSOContainerPart *somSelf, 
                                         Environment *ev, 
                                         ODFrame* embeddedFrame, 
                                         ODShape* revealShape)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","RevealFrame");

    // This method should scroll the embedded frame into view
    return(kODFalse);
}

// ------------------------------------------------
//  Method:             EmbeddedFrameSpec
//
//  Introduced by:      ODPart
//
//   Create object specifier for embedded frame
// ------------------------------------------------
SOM_Scope void  SOMLINK EmbeddedFrameSpec(ITSOContainerPart *somSelf, 
                                          Environment *ev, 
                                          ODFrame* embeddedFrame, 
                                          ODObjectSpec* spec)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","EmbeddedFrameSpec");
}

// ------------------------------------------------
//  Method:             CreateEmbeddedFramesIterator
//
//  Introduced by:      ODPart
//
//   Returns an iterator to walk though our embedded
//   frames.  Since we are responsible for storing
//   info on our embedded frames, this iterator provides
//   a standard way for other code to get access to
//   our list.
// ------------------------------------------------

SOM_Scope ODEmbeddedFramesIterator*  SOMLINK CreateEmbeddedFramesIterator(ITSOContainerPart *somSelf, 
                                                                          Environment *ev,
                                                                          ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","CreateEmbeddedFramesIterator");

    XEmbeddedFramesIterator * xefi = new XEmbeddedFramesIterator();
    xefi->InitXEmbeddedFramesIterator(ev, _fEmbeddedFrames);
    return(xefi);
}

// ------------------------------------------------
//  Method:             RequestEmbeddedFrame
//
//  Introduced by:      ODPart
//
//   Called by embedded parts who wish for an
//   additional frame.
// ------------------------------------------------
SOM_Scope ODFrame*  SOMLINK RequestEmbeddedFrame(ITSOContainerPart *somSelf, 
                                                 Environment       *ev,
                                                 ODFrame*           containingFrame,
                                                 ODFrame*           baseFrame,
                                                 ODShape*           frameShape,
                                                 ODPart*            embedPart,
                                                 ODTypeToken        viewType,
                                                 ODTypeToken        presentation,
                                                 ODBoolean          isOverlaid)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","RequestEmbeddedFrame");

    ODFrame* embeddedFrame;
    ODRect   rect;
    if (!frameShape) {
       ODShape * tempShape = baseFrame->AcquireFrameShape(ev,kODNULL);
       frameShape = tempShape->Copy(ev);
//       tempShape->Release(ev);  // DevCon 8 bug, not all Acquire..()'s inc RefCnt
    } /* endif */
    frameShape->GetBoundingBox(ev, &rect);

    ODTransform* externalXForm = new ODTransform;
    if (baseFrame) {
      ODPoint ptTemp = {(rect.right - rect.left) + MAKEFIXED(20, 0), 0};
      externalXForm->CopyFrom(ev, somSelf->ICPProxyForFrame(ev, baseFrame)->transform);
      externalXForm->MoveBy(ev, &ptTemp);
    } else {
      ODSLong count = 1 + _fContents->Count();
      ODPoint ptTemp = {MAKEFIXED(count * 20, 0), MAKEFIXED(count * 20, 0)};
      externalXForm->SetOffset(ev, &ptTemp);
    }

    embeddedFrame = somSelf->ICPMakeEmbeddedFrame(ev,
                                               containingFrame,
                                               frameShape,
                                               externalXForm,
                                               embedPart,
                                               kODFalse);

    somSelf->ICPCreateEmbeddedFacets(ev, containingFrame, embeddedFrame);
    somSelf->ICPClipEmbeddedFacets(ev, FirstFacet(ev, containingFrame));
    embeddedFrame->Invalidate(ev, kODNULL, kODNULL);
    return(embeddedFrame);
}

// ------------------------------------------------
//  Method:             RemoveEmbeddedFrame
//
//  Introduced by:      ODPart
//
//   Removes an embedded frame
// ------------------------------------------------
SOM_Scope void  SOMLINK RemoveEmbeddedFrame(ITSOContainerPart *somSelf, 
                                            Environment *ev, 
                                            ODFrame* embeddedFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","RemoveEmbeddedFrame");

    if (_fEmbeddedFrames->Contains(embeddedFrame)) {
       ODFrame* containingFrame = embeddedFrame->AcquireContainingFrame(ev);
       containingFrame->Acquire(ev); // bug in acf() - no inc refcount
       Proxy *p = somSelf->ICPProxyForFrame(ev, embeddedFrame);

       if (p) {
          _fNeedToExternalize = kODTrue;
          _fContents->Remove(p);
          p->frame->Release(ev);                 
          delete p;
       }

       _fEmbeddedFrames->Remove(embeddedFrame);
       embeddedFrame->Release(ev);                

       somSelf->ICPClipEmbeddedFrames(ev, containingFrame);
       if (containingFrame) {
           containingFrame->Invalidate(ev, kODNULL, kODNULL);
           containingFrame->Release(ev);           
       }
    } else {
      THROW(kODErrInvalidFrame);
    }
}

// ------------------------------------------------
//  Method:             RequestFrameShape
//
//  Introduced by:      ODPart
//
//   Called by embedded part desiring a new frame shape.
// We are very gracious and grant all shapes!
// ------------------------------------------------
SOM_Scope ODShape*  SOMLINK RequestFrameShape(ITSOContainerPart *somSelf, 
                                              Environment *ev, 
                                              ODFrame* embeddedFrame, 
                                              ODShape* frameShape)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","RequestFrameShape");

    ODShape* grantedShape = frameShape->Copy(ev);
    return grantedShape;
}

// ------------------------------------------------
//  Method:             UsedShapeChanged
//
//  Introduced by:      ODPart
//
// Used shape of an embedded part has changed
// ------------------------------------------------
SOM_Scope void  SOMLINK UsedShapeChanged(ITSOContainerPart *somSelf, 
                                         Environment *ev, 
                                         ODFrame* embeddedFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","UsedShapeChanged");
    if (_fEmbeddedFrames->Contains(embeddedFrame)) {
        Proxy* p = somSelf->ICPProxyForFrame(ev, embeddedFrame);
        ODFrame* ownerFrame = embeddedFrame->AcquireContainingFrame(ev);//***

        if (_fSelection->Contains(p)) {
           somSelf->ICPCreateSelectSizingHandlesShape(ev);
        }

        somSelf->ICPClipEmbeddedFrames(ev, ownerFrame);
        embeddedFrame->Invalidate(ev, kODNULL, kODNULL); 
//***        ownerFrame->Release(ev);  // Bug in acquirecontaingframe...does 
                                       // not increment refcount
    } else {
      THROW(kODErrInvalidFrame);
    }
}

// ------------------------------------------------
//  Method:             EmbeddedFrameUpdated
//
//  Introduced by:      ODPart
//
//    Notifies us that the content of an
//    embedded frame has been changed.
// ------------------------------------------------

SOM_Scope void  SOMLINK EmbeddedFrameUpdated(ITSOContainerPart *somSelf, 
                                             Environment *ev, 
                                             ODFrame* frame,
                                             ODUpdateID change)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","EmbeddedFrameUpdated");

    // I don't care if embedded frames change
}


// ------------------------------------------------
//   Handling User Events
// ------------------------------------------------

// ------------------------------------------------
//  Method:             HandleEvent
//
//  Introduced by:      ODPart
//
//    Our main event handling method.  All user
//    interface events result in this method being
//    called.
// ------------------------------------------------

SOM_Scope ODBoolean  SOMLINK HandleEvent(ITSOContainerPart *somSelf, 
                                         Environment *ev, 
                                         ODEventData* event, 
                                         ODFrame* frame, 
                                         ODFacet* facet, 
                                         ODEventInfo* eventInfo)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","HandleEvent");

    ODBoolean handled = kODFalse;

    switch (event->msg)
    {
       case WM_BUTTON1MOTIONSTART:
          if (!eventInfo->flags) {
             ODPoint windowODPoint(ODPOINTL(SHORT1FROMMP(event->mp1),
                                            SHORT2FROMMP(event->mp1)));
             handled = somSelf->ICPHandleButton1MotionStart(ev, facet, &windowODPoint, event);
             break;
          }
       case WM_BUTTON1CLICK:
        {
          ODPoint windowODPoint(ODPOINTL(SHORT1FROMMP(event->mp1),
                                         SHORT2FROMMP(event->mp1)));
          if (eventInfo->flags == kODInEmbedded) {
                handled = somSelf->ICPHandleButton1ClickInEmbeddedFrame(ev, facet,
                                  eventInfo->embeddedFacet, &windowODPoint, event);
          } else if (eventInfo->flags == kODInBorder) {
                handled = somSelf->ICPHandleButton1ClickInBorder(ev, facet,
                                  eventInfo->embeddedFacet, &windowODPoint, event);
          } else {
                handled = somSelf->ICPHandleButton1Click(ev, facet, &windowODPoint, event);
          }
          break;
        }
       case WM_BEGINDRAG:
          {
          ODPoint windowODPoint(ODPOINTL(SHORT1FROMMP(event->mp1),
                                         SHORT2FROMMP(event->mp1)));
          return somSelf->ICPHandleMouseDrag(ev, facet,
                    (eventInfo->flags == kODInEmbedded || eventInfo->flags == kODPropagated) ?
                        eventInfo->embeddedFacet : kODNULL,
                    &windowODPoint, event);
          }
       case WM_CONTEXTMENU:
          handled = kODTrue;
          if (_fIgnoreContextMenu && !SHORT2FROMMP(event->mp2))
             break;                // ignore if mouse event

          else
          {
             PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);
             if (!(pInfo->fIsActive)){
                   somSelf->ICPActivateFrame(ev, frame);
                  _fPopup->RemoveSelectedMenu(ev);
             }
             if ((pInfo->fHasSelectedPart) && !(pInfo->fSelPopupAttached)) {
                if (!_fExtension)
                 if (pInfo->fSelectedPart->HasExtension(ev, kODMenuExtension))
                   _fExtension = (ODMenuExtension*)(pInfo->fSelectedPart->AcquireExtension(ev, kODMenuExtension));
                   if (_fExtension) {
                     ODPlatformMenu selPartMenu = _fExtension->GetSelectedPopup(ev);
                     _fPopup->AddSelectedMenu(ev, selPartMenu);
                     pInfo->fSelPopupAttached = kODTrue;
                   } /* endif */
             } /* endif */
             _fPopup->Display(ev);
          }
          break;
       case WM_MOUSEMOVE:
          {
          ODPoint windowODPoint(ODPOINTL(SHORT1FROMMP(event->mp1),
                                         SHORT2FROMMP(event->mp1)));
          handled = somSelf->ICPHandleMouseMove(ev, facet, frame, &windowODPoint, event);
          }
          break;
       case WM_BUTTON1MOTIONEND:
          {
          ODPoint windowODPoint(ODPOINTL(SHORT1FROMMP(event->mp1),
                                         SHORT2FROMMP(event->mp1)));
          handled = somSelf->ICPHandleButton1MotionEnd(ev, facet, frame, &windowODPoint, event);
          }
          break;
       case WM_BUTTON2DOWN:
          if (_fMouseMode != kNormal) {
             somSelf->ICPResetMouseMode(ev);
             _fIgnoreContextMenu = kODTrue;
             handled = TRUE;
          } else
             _fIgnoreContextMenu = kODFalse;
          break;
       case WM_CHAR:
          if (!((SHORT1FROMMP(event->mp1) & KC_KEYUP))) {
            handled = somSelf->ICPHandleKeyDown(ev, frame, event);
          }
          break;
       case WM_ACTIVATE:
          handled = kODTrue; // actually ignored by dispatcher
          if (SHORT1FROMMP(event->mp1) != 0) {
             somSelf->ICPActivatingWindow(ev, frame);
          } else {
             somSelf->ICPDeActivatingWindow(ev, frame);
          }
          break;
       case WM_COMMAND:
          if (SHORT1FROMMP(event->mp2) & CMDSRC_MENU ||
              SHORT1FROMMP(event->mp2) & CMDSRC_ACCELERATOR) {
            handled = somSelf->ICPHandleMenuEvent(ev, frame, event);
          }
          break;
       default:
          return kODFalse;
    }
    return handled;
}

// ------------------------------------------------
// Methods Related to our facets
// ------------------------------------------------

// ------------------------------------------------
//  Method:             HighLightChanged
//
//  Introduced by:      ODPart
//
//      Notifies us that this facet's highlight state
//      has changd.  We should propogate this change
//      to our embedded facets.
// ------------------------------------------------

SOM_Scope void  SOMLINK HighlightChanged(ITSOContainerPart *somSelf, 
                                         Environment *ev, 
                                         ODFacet* facet)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","HighlightChanged");

    ODHighlight highlight = facet->GetHighlight(ev);

    ODFacetIterator* facets = facet->CreateFacetIterator(ev, kODChildrenOnly, kODFrontToBack);
    for (ODFacet* embFacet = facets->First(ev);
          facets->IsNotComplete(ev);
          embFacet = facets->Next(ev))
    {
        embFacet->ChangeHighlight(ev, highlight);
    }
    facet->Invalidate(ev, kODNULL, kODNULL);
}

// ------------------------------------------------
//  Method:             FacetAdded
//
//  Introduced by:      ODPart
//
//      Called when our container has added a
//      facet for our part.
// ------------------------------------------------
SOM_Scope void  SOMLINK FacetAdded(ITSOContainerPart *somSelf, 
                                   Environment *ev, 
                                   ODFacet* facet)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","FacetAdded");

    ODFrame* dispFrame = facet->GetFrame(ev);
    if (!_fDisplayFrames->Contains(dispFrame))
       THROW(kODErrInvalidFacet);

    ODShape* contentClip = facet->CreateShape(ev);
    facet->SetPartInfo(ev, (ODInfoType)contentClip);

    ODEmbeddedFramesIterator* frames;
    frames = somSelf-> CreateEmbeddedFramesIterator(ev,kODNULL);
    for (ODFrame* frame = (ODFrame*) frames->First(ev);
        frames->IsNotComplete(ev);
        frame = (ODFrame*) frames->Next(ev))
    {
        ODFrame* tempFrame = frame->AcquireContainingFrame(ev);
        if (tempFrame == dispFrame) {  //Bug in acf()

           ODShape* frameShape = frame->AcquireFrameShape(ev, kODNULL);
           ODShape* clip = frameShape->Copy(ev);
           frameShape->Release(ev);

           ODTransform* xform = somSelf->
                                 ICPProxyForFrame(ev, frame)->
                                   transform->
                                       Copy(ev);
           facet->CreateEmbeddedFacet(ev, frame, clip, xform,
                                      kODNULL, kODNULL, kODNULL, kODFrameBehind);
           clip->Release(ev); clip = kODNULL;
           xform->Release(ev); xform = kODNULL;
      }
 //     tempFrame->Release(ev);  // Commented due to bug in acf - refcnt
    }
}

// ------------------------------------------------
//  Method:             FacetRemoved
//
//  Introduced by:      ODPart
//
//      Called when our container has removed a
//      facet for our part.
//
//      Remove a facet and its children
// ------------------------------------------------
SOM_Scope void  SOMLINK FacetRemoved(ITSOContainerPart *somSelf, 
                                     Environment *ev, 
                                     ODFacet* facet)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","FacetRemoved");

    ODFrame* dispFrame = facet->GetFrame(ev);
    if (!_fDisplayFrames->Contains(dispFrame)) {
        THROW(kODErrInvalidFacet);
    }

    ODShape *clipShape = (ODShape*)facet->GetPartInfo(ev);
    if (clipShape) {
        clipShape->Release(ev);
        facet->SetPartInfo(ev, kODNULL);
    }

    // make a copy-collection of our facet's children
    ODxOrderedCollection* children = new ODxOrderedCollection;

    ODFacetIterator* facets = facet->
                                CreateFacetIterator(ev, 
                                                    kODChildrenOnly, 
                                                    kODFrontToBack);
    for (ODFacet* childFacet = facets->First(ev); 
         facets->IsNotComplete(ev);
         childFacet = facets->Next(ev))
    {
      children->AddLast(childFacet);
    }
    delete facets;

    // delete from the copy-collection
    ODxOrderedCollectionIterator iter(children);

    for (childFacet = (ODFacet*)iter.First(); 
         iter.IsNotComplete();
         childFacet = (ODFacet*)iter.Next())
    {
      facet->RemoveFacet(ev, childFacet);
      delete childFacet;
    }
    delete children;
}

// ------------------------------------------------
//  Method:             GeometryChanged
//
//  Introduced by:      ODPart
//
//      Called when either our external transform
//      or clip shaoe has changed.
//
// ------------------------------------------------

SOM_Scope void  SOMLINK GeometryChanged(ITSOContainerPart *somSelf, 
                                        Environment *ev, 
                                        ODFacet* facet, 
                                        ODBoolean clipShapeChanged, 
                                        ODBoolean externalTransformChanged)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","GeometryChanged");
}


// ------------------------------------------------
//  Method:             AdjustBorderShape
//
//  Introduced by:      ODPart
//
//   Called so that we may adjust the active border shape
//   of an embedded part.  Allows us to ensure the 
//   active border shape is clipped by overlaying parts.
// ------------------------------------------------

SOM_Scope ODShape*  SOMLINK AdjustBorderShape(ITSOContainerPart *somSelf, 
                                              Environment *ev, 
                                              ODFacet* embeddedFacet, 
                                              ODShape* shape)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","AdjustBorderShape");

    ODShape* adjustedShape= shape->Copy(ev);    // Copy Shape passed to us
    ODFacet* containingFacet = embeddedFacet->
                                 GetContainingFacet(ev);
    ODTransform* transform = embeddedFacet->
                               AcquireFrameTransform(ev, kODNULL);
    adjustedShape->Transform(ev, transform);    // Adjust shape to its frame
                                                // coordinates
    ODFacetIterator* facets = containingFacet->
                                  CreateFacetIterator(ev, 
                                                      kODChildrenOnly, 
                                                      kODFrontToBack);
    for (ODFacet* embFacet = facets->First(ev);
          facets->IsNotComplete(ev) && embFacet != embeddedFacet;  // For each embedded facet
          embFacet = facets->Next(ev))                             // of the container
    {
       ODShape* usedShape = ODCopyAndRelease(ev,embFacet->         // of that facets frame
                                                  GetFrame(ev)->
                                                     AcquireUsedShape(ev, kODNULL));
       ODTransform* xform = embFacet->                             // Get that facets frame
                              AcquireFrameTransform(ev, kODNULL);  // transform
       usedShape->Transform(ev, xform);                            // Put used shape into
                                                                   // its frames cooridnates
       adjustedShape->Subtract(ev, usedShape);                     // Subtract its used shape
                                                                   // from current border shape
       usedShape->Release(ev);
       xform->Release(ev);
    }
    delete facets;

    adjustedShape->InverseTransform(ev, transform);               // Take transform our of shape
    transform->Release(ev);                                       // Release transform
    return adjustedShape;                                         // Return adjusted shape
}

// ------------------------------------------------
//  Method:             Open
//
//  Introduced by:      ODPart
//
//      Open this part into a window...either 
//      root or part window.
//
// ------------------------------------------------
SOM_Scope ODID  SOMLINK Open(ITSOContainerPart *somSelf, 
                             Environment *ev, 
                             ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","Open");

    ODWindow* window = kODNULL;

    if (frame) {               // Doing a 'View As Window' or 'Open Root'
      if (frame->IsRoot(ev)) { // Create Window For Root Frame
         WindowProperties props;
         BeginGetWindowProperties(ev, frame, &props);
         ODULong flCreateFlags = props.createFlags ? props.createFlags :
                                                       ODPlatformWindowDefaultCreateOptions;
         HWND hwndFrame = _fSession->CreatePlatformWindow(ev, flCreateFlags);
         WinSetWindowPos(hwndFrame, HWND_TOP, 
                         props.boundsRect.xLeft,
                         props.boundsRect.yBottom,
                         props.boundsRect.xRight,
                         props.boundsRect.yTop,
                         (SWP_SIZE | SWP_MOVE| props.swpFlags ));
         window =  _fSession->
                      GetWindowState(ev)->
                         RegisterWindowForFrame(ev, hwndFrame,
                                                frame,
                                                props.isRootWindow, // Keeps draft open
                                                kODTrue,            // Is resizable
                                                kODFalse,           // Is floating
                                                kODTrue,            // should save
                                                props.sourceFrame);
         EndGetWindowProperties(ev, &props); // Release source frame
         window->Open(ev);
         window->Show(ev);
      } else { // View In Window
         window = _fSession->GetWindowState(ev)->AcquireWindow(ev, _fWindowID);
         if (window) {         // Window already exits...
            window->Select(ev); // so select it
         } else {
            window = somSelf->ICPCreateWindow(ev, frame); // Create new window
            _fWindowID = window->GetID(ev);     // store this window ID
            window->Open(ev);                   //
            window->Show(ev);
            window->Select(ev);
         }
      }
    } else {
      window = somSelf->ICPCreateWindow(ev, frame);
      _fWindowID = window->GetID(ev);
      window->Open(ev);
      window->Show(ev);
      window->Select(ev);
    }
    return window->GetID(ev);
}

// ------------------------------------------------
//  Method:             Draw
//
//  Introduced by:      ODPart
//
//    Draw is called when the display of the part 
//    requires updating, either the whole part 
//    (invalidShape == kODNULL) or a specified 
//    region (the invalidShape specifies the region
//    to redraw).
// ------------------------------------------------
SOM_Scope void  SOMLINK Draw(ITSOContainerPart *somSelf, 
                             Environment *ev, 
                             ODFacet* facet, 
                             ODShape* invalidShape)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","Draw");

    ODFrame* displayFrame = facet->GetFrame(ev);
    if (_fDisplayFrames->Contains(displayFrame)) {
      PartInfoRec* pInfo = (PartInfoRec*) displayFrame->GetPartInfo(ev);
      if (facet->GetFrame(ev)->GetPresentation(ev) == _PresDefault) {
         ODTypeToken curView = displayFrame->GetViewType(ev);
         if ((curView == _FrameView)  || (curView == _ThumbnailView)) {
            somSelf->ICPDrawFrameView(ev,facet,invalidShape);

         } else {
           if (curView == _LargeIconView) {
             somSelf->ICPDrawIconView(ev, facet, LARGEICONSIZE);
           } else {
             if (curView == _SmallIconView) {
               somSelf->ICPDrawIconView(ev, facet, SMALLICONSIZE);
             }
           }
         }
      }
    } else {
      THROW(kODErrInvalidFrame);
    }
}


//********************************* 
// Miscellaneous  Methods
//*********************************


// Not used
SOM_Scope ODULong  SOMLINK GetPrintResolution(ITSOContainerPart *somSelf, 
                                              Environment *ev, 
                                              ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","GetPrintResolution");

    return(0);
}

// We only have one kind
SOM_Scope void  SOMLINK ChangeKind(ITSOContainerPart *somSelf, 
                                   Environment *ev, 
                                   ODType kind)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ChangeKind");
}



// The presentation has changed
SOM_Scope void  SOMLINK PresentationChanged(ITSOContainerPart *somSelf, 
                                            Environment *ev, 
                                            ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","PresentationChanged");

    // This part does not support different presentation types
}

// The sequence numbed has changed
SOM_Scope void  SOMLINK SequenceChanged(ITSOContainerPart *somSelf, 
                                        Environment *ev, 
                                        ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","SequenceChanged");

    // Not supporting multiple frames for the same part
}


// ---------------------------------------
//   Methods related to Linking Protocol
// ---------------------------------------


// No linking supported
SOM_Scope ODLinkSource*  SOMLINK CreateLink(ITSOContainerPart *somSelf, 
                                            Environment *ev, 
                                            ODByteArray* data)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","CreateLink");

    // No link support
    return(kODNULL);
}

// No linking supported
SOM_Scope void  SOMLINK LinkUpdated(ITSOContainerPart *somSelf, 
                                    Environment *ev, 
                                    ODLink* updatedLink,
                                    ODUpdateID change)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","LinkUpdated");

    // No linking
}

// No linking supported
SOM_Scope void  SOMLINK RevealLink(ITSOContainerPart *somSelf, 
                                   Environment *ev, 
                                   ODLinkSource* linkSource)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","RevealLink");

    // Not sure what it does and not sure about standard
}

// No linking
SOM_Scope void  SOMLINK LinkStatusChanged(ITSOContainerPart *somSelf, 
                                          Environment *ev, 
                                          ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","LinkStatusChanged");

    // No link support
}


SOM_Scope ODBoolean  SOMLINK EditInLinkAttempted(ITSOContainerPart *somSelf, 
                                                 Environment *ev,
                                                 ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","EditInLinkAttempted");

    return kODFalse;
}

// ---------------------------------------
//   Methods related to our Extensions
// ---------------------------------------

// Acquire extensions
SOM_Scope ODExtension*  SOMLINK AcquireExtension(ITSOContainerPart *somSelf, 
                                                 Environment *ev, 
                                                 ODType extensionName)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","AcquireExtension");

    // Could add a notebook page here or view type

    return (ITSOContainerPart_parent_ODPart_AcquireExtension(somSelf, 
                                                             ev, 
                                                             extensionName));
}

// Release extensions
SOM_Scope void  SOMLINK ReleaseExtension(ITSOContainerPart *somSelf, 
                                         Environment *ev, 
                                         ODExtension* extension)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ReleaseExtension");

    if (!extension) {
      return;
    } else {
      ITSOContainerPart_parent_ODPart_ReleaseExtension(somSelf,
                                                       ev, extension);
    }
}

// Determine if extension already acquired
SOM_Scope ODBoolean  SOMLINK HasExtension(ITSOContainerPart *somSelf, 
                                          Environment *ev, 
                                          ODType extensionName)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","HasExtension");

    return (ITSOContainerPart_parent_ODPart_HasExtension(somSelf,
                                                         ev,
                                                         extensionName));
}

// ---------------------------------------
//   Methods related to our Canvas
// ---------------------------------------

// Instructs part that a facet has been moved to image on another canvas
SOM_Scope void  SOMLINK CanvasChanged(ITSOContainerPart *somSelf, 
                                      Environment *ev, 
                                      ODFacet* facet)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","CanvasChanged");

}

// No off-screen canvas
SOM_Scope void  SOMLINK CanvasUpdated(ITSOContainerPart *somSelf, 
                                      Environment *ev, 
                                      ODCanvas* canvas)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","CanvasUpdated");

    // This part has no offscreen canvas' and no use for this method
}

// ---------------------------------------
//   Methods related to our containing part
// ---------------------------------------

// No cascading property setting
SOM_Scope void  SOMLINK ContainingPartPropertiesUpdated(ITSOContainerPart *somSelf, 
                                                        Environment *ev, 
                                                        ODFrame* frame, 
                                                        ODStorageUnit* propertyUnit)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ContainingPartPropertiesUpdated");

    // Not cascading updated properties down hierarchy at this stage
}

// No cascading property setting
SOM_Scope ODStorageUnit*  SOMLINK AcquireContainingPartProperties(ITSOContainerPart *somSelf, 
                                                                  Environment *ev, 
                                                                  ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","AcquireContainingPartProperties");

    return(kODNULL);
}


// ---------------------------------------
//   Methods related to UnDo protocol
// ---------------------------------------

// No undo
SOM_Scope ODActionData  SOMLINK ReadActionState(ITSOContainerPart *somSelf, 
                                                Environment *ev, 
                                                ODStorageUnitView* storageUnitView)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ReadActionState");

    ODActionData data;
    data._maximum = 0;
    data._length = 0;
    data._buffer = kODNULL;
    return data;
}

// No undo
SOM_Scope void  SOMLINK UndoAction(ITSOContainerPart *somSelf, 
                                   Environment *ev, 
                                   ODActionData* actionState)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","UndoAction");
}

// No undo
SOM_Scope void  SOMLINK RedoAction(ITSOContainerPart *somSelf, 
                                   Environment *ev, 
                                   ODActionData* actionState)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","RedoAction");
}

// No undo
SOM_Scope void  SOMLINK DisposeActionState(ITSOContainerPart *somSelf, 
                                           Environment *ev, 
                                           ODActionData* actionState,
                                           ODDoneState doneState)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","DisposeActionState");
}

// No undo
SOM_Scope void  SOMLINK WriteActionState(ITSOContainerPart *somSelf, 
                                         Environment *ev, 
                                         ODActionData* actionState,
                                         ODStorageUnitView* storageUnitView)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","WriteActionState");
}

//==============================================
// End of Overridden Methods
//==============================================
//********************************************************************** */

// ---------------------------------------------
// ITSOContainerPart Specific Methods
//  All start with ICP...
// ---------------------------------------------

SOM_Scope void  SOMLINK ICPInitSU(ITSOContainerPart *somSelf, 
                                  Environment *ev, ODStorageUnit* su)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPInitSU");

    if (!su->Exists(ev, kODPropContents, kKind, 0)) {
      su->AddProperty(ev, kODPropContents)->AddValue(ev, kKind);
    }     
    su->AddProperty(ev, kODPropDisplayFrames)->AddValue(ev, kODWeakStorageUnitRefs);
    su->AddProperty(ev, kODPropFrameGroup)->AddValue(ev, kODULong);
    su->AddProperty(ev, kODPropEmbeddedFrames)->AddValue(ev, kODStrongStorageUnitRefs);
    su->AddProperty (ev, kODPropPreferredKind)->AddValue (ev, kODISOStr);
    StorageUnitSetValue(su, ev, strlen(kKind) + 1, kKind);

}

SOM_Scope void  SOMLINK ICPSaveFrameGroup(ITSOContainerPart *somSelf, 
                                          Environment *ev, ODStorageUnit* su)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPSaveFrameGroup");
    if (_fNeedToExternalize) {
       su->Focus(ev, kODPropFrameGroup,kODPosUndefined,  // focus on the framegroup property and store the id
                     0,1,kODPosFirstSib);
       StorageUnitSetValue(su, ev, sizeof(_fFrameGroupIDCounter), (ODValue)&_fFrameGroupIDCounter);
    } /* endif */
}

SOM_Scope void  SOMLINK ICPSaveEmbeddedFrames(ITSOContainerPart *somSelf, 
                                              Environment       *ev,
                                              ODStorageUnit     *su)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPSaveEmbeddedFrames");

    ODStorageUnitRef   aSURef;
    Proxy            * proxy;
    ODULong            offset;

    ODxOrderedCollectionIterator cIter(_fContents);

    if ( _fNeedToExternalize ) {
       su->Focus(ev, kODPropEmbeddedFrames,kODPosUndefined,
                     0,1,kODPosFirstSib);
       su->Remove(ev);
       su->AddProperty(ev, kODPropEmbeddedFrames);
       su->AddValue(ev, kODStrongStorageUnitRef);
    }
    offset = 0;
    // iterate through the embedded frames and store a strong reference to them 
    for (proxy = (Proxy*)cIter.First(); cIter.IsNotComplete(); proxy = (Proxy*)cIter.Next())
    {
      ODMatrix mtx;
      proxy->frame->Externalize(ev);

      if (_fNeedToExternalize ) {
         su->GetStrongStorageUnitRef(ev, proxy->
                                           frame->
                                             GetStorageUnit(ev)->
                                               GetID(ev), aSURef);
         su->SetOffset(ev, offset);
         StorageUnitSetValue(su, ev, sizeof(ODStorageUnitRef), (ODValue)&aSURef);
         offset += sizeof(ODStorageUnitRef);
         proxy->transform->GetMatrix(ev, &mtx);
         su->SetOffset(ev, offset);
         StorageUnitSetValue(su, ev, sizeof(ODMatrix), (ODValue)&mtx);
         offset += sizeof(ODMatrix);
      } /* endif */
    }
}

SOM_Scope void  SOMLINK ICPReadEmbeddedFrames(ITSOContainerPart *somSelf, 
                                              Environment *ev, 
                                              ODStorageUnit* su)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPReadEmbeddedFrames");

    ODStorageUnitRef   aSURef;
    ODFrame          * frame;
    ODMatrix           mtx;
    ODTransform      * transform;

    su->Focus(ev, kODPropEmbeddedFrames,kODPosSame,0,1,kODPosFirstSib);
    long offsetLimit = su->GetSize(ev);

    for (long offset = 0; offset < offsetLimit;)
    {
      su->SetOffset(ev, offset);
      StorageUnitGetValue(su, ev, sizeof(ODStorageUnitRef), (ODValue)&aSURef);
      offset += sizeof(ODStorageUnitRef);

      frame = su->GetDraft(ev)->AcquireFrame(ev, su->GetIDFromStorageUnitRef(ev, aSURef));
      _fEmbeddedFrames->AddLast((ElementType)frame);
      frame->Acquire(ev);             //***BJ  

      su->SetOffset(ev, offset);
      StorageUnitGetValue(su, ev, sizeof(ODMatrix), (ODValue)&mtx);
      offset += sizeof(ODMatrix);

      transform = frame->CreateTransform(ev)->SetMatrix(ev, &mtx);
      _fContents->AddLast((ElementType)(new Proxy(frame, transform)));
      frame->Acquire(ev);             //***BJ

    }
}

SOM_Scope void  SOMLINK ICPReadFrameGroup(ITSOContainerPart *somSelf, 
                                          Environment *ev, ODStorageUnit* su)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPReadFrameGroup");

    su->Focus(ev, kODPropFrameGroup,kODPosSame,0,1,kODPosFirstSib);
    StorageUnitGetValue(su, ev, sizeof(_fFrameGroupIDCounter),(ODValue)&_fFrameGroupIDCounter);
}

SOM_Scope Proxy*  SOMLINK ICPProxyForFrame(ITSOContainerPart *somSelf,
                                           Environment *ev,
                                           ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPProxyForFrame");

    Proxy *p = kODNULL;

    if (_fEmbeddedFrames->Contains(frame)) {

      ODxOrderedCollectionIterator i(_fContents);
 
      for (p = (Proxy*)i.First();
           i.IsNotComplete();
           p = (Proxy*)i.Next())
      {
        if (p->frame == frame) {
          break;
        } else {
          p=kODNULL;
        }
      }
    }

    return(p);
}

// Return the appropriate proxy based on the specified frame ID
SOM_Scope Proxy*  SOMLINK ICPProxyForFrameID(ITSOContainerPart *somSelf,
                                             Environment *ev, 
                                             ODID frameID)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPProxyForFrameID");

    ODFrame* frame = somSelf->
                       GetStorageUnit(ev)->
                         GetDraft(ev)->
                           AcquireFrame(ev, frameID);
    Proxy* p = somSelf->ICPProxyForFrame(ev, frame);
//    frame->Release(ev);  // DevCon 8 bug, not all Acquire..()'s inc RefCnt 
    return(p);
}

SOM_Scope void  SOMLINK ICPCommonInit(ITSOContainerPart *somSelf,
                                      Environment *ev)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPCommonInit");


    _fSession        = somSelf->GetStorageUnit(ev)->GetSession(ev);

    _fSelectionFocus = _fSession->Tokenize(ev, kODSelectionFocus);
    _FrameView       = _fSession->Tokenize(ev, kODViewAsFrame);
    _LargeIconView   = _fSession->Tokenize(ev, kODViewAsLargeIcon);
    _SmallIconView   = _fSession->Tokenize(ev, kODViewAsSmallIcon);
    _ThumbnailView   = _fSession->Tokenize(ev, kODViewAsThumbnail);
    _PresDefault     = _fSession->Tokenize(ev, kODPresDefault);

    _fMenuFocus      = _fSession->Tokenize(ev, kODMenuFocus);
    _fKeyFocus       = _fSession->Tokenize(ev, kODKeyFocus);
    _fMouseFocus     = _fSession->Tokenize(ev, kODMouseFocus);

    _fFocusSet       = _fSession->GetArbitrator(ev)->CreateFocusSet(ev);
    _fFocusSet->Add(ev, _fSelectionFocus);
    _fFocusSet->Add(ev, _fMenuFocus);
    _fFocusSet->Add(ev, _fKeyFocus);

    _fDisplayFrames  = new ODxOrderedCollection;
    _fEmbeddedFrames = new ODxOrderedCollection;
    _fContents       = new ODxOrderedCollection;
    _fSelection      = new ODxOrderedCollection;
    _fSavedFacetClipShapes = new ODxOrderedCollection;

    ODRect initRect = {0, 0, 0, 0};
    _fSelectSizingHandlesShape = new ODShape;
    _fSelectSizingHandlesShape->SetRectangle(ev, &initRect);
    _fSelectSizingHandlesShape->Acquire(ev);

    if (_fSession->HasExtension(ev, kODExtStatusLine)) {
        _fStatusLn    = (ODStatusLineExtension*)_fSession->
                                                  AcquireExtension(ev, 
                                                                   kODExtStatusLine);
        _fStatusFocus = _fSession->Tokenize(ev, kODStatusLineFocus);
    }


    _fMenuBar = _fSession->GetWindowState(ev)->CopyBaseMenuBar(ev);

    // Need to check this as there are specific methods for menu manipulation
    if (_fMenuBar) {
      _fPopup = _fSession->GetWindowState(ev)->CopyBasePopup(ev);
      somSelf->ICPInstallMenus(ev);
    }
}

// This routine highlights the selction border around the selected object
// Not sure why with multiple selection the HighlightContentObject is performed
SOM_Scope void  SOMLINK ICPHighlightSelection(ITSOContainerPart *somSelf,
                                              Environment *ev, 
                                              ODFacet* facet)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPHighlightSelection");

    if (facet->GetCanvas(ev)->IsDynamic(ev))
    {
       switch (_fSelection->Count())
       {
          case 0:     // no selection
             break;

          case 1:     // single selection
             {  ODxOrderedCollectionIterator i(_fSelection);
                somSelf->ICPHighlightProxyBorder(ev, (Proxy*)i.First(), facet);
             };
             break;

          default: // multiple selection
             {  Proxy* p;
                ODxOrderedCollectionIterator i(_fSelection);
                somSelf->ICPHighlightProxyBorder(ev, (Proxy*)i.First(), facet);
                for (p = (Proxy*) i.First(); i.IsNotComplete(); p = (Proxy*) i.Next())
                {  if (_fSelection->Contains(p)){
                  //    somSelf->HighlightContentObject(ev, p, facet);
                   }
                };
             };
       };
    }
}

// This method handles all menu events dispatched by the event handler
SOM_Scope ODBoolean  SOMLINK ICPHandleMenuEvent(ITSOContainerPart *somSelf,
                                                Environment *ev, 
                                                ODFrame* focusFrame,
                                                ODEventData* event)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPHandleMenuEvent");

    PartInfoRec* pInfo   = (PartInfoRec*) focusFrame->GetPartInfo(ev);
    ODBoolean    handled = kODFalse;
    ODBoolean    isRoot  = focusFrame->IsRoot(ev);
    ODCommandID  command = LONGFROMMP(event->mp1);
    ODFacet*     facet   = FirstFacet(ev, focusFrame);

    if (pInfo->fIsActive) {
      switch (command)
      {
        case EDIT_DELETE:
          somSelf->ICPDoClear(ev, focusFrame);
          handled = kODTrue;
          break;
        case IDMA_MOVETOFRONT:
          somSelf->ICPMoveToFront(ev, focusFrame);
          handled = kODTrue;
          break;
        case IDMA_MOVETOBACK:
          somSelf->ICPMoveToBack(ev, focusFrame);
          handled = kODTrue;
          break;
        case IDMA_MOVEFORWARD :
          somSelf->ICPMoveForward(ev, focusFrame);
          handled = kODTrue;
          break;
        case IDMA_MOVEBACKWARD:
          somSelf->ICPMoveBackward(ev, focusFrame);
          handled = kODTrue;
          break;
        case VIEW_REFRESH :
          focusFrame->Invalidate(ev, kODNULL , kODNULL );
          handled = kODTrue;
          break;
        case VIEW_OATREE:
        case VIEW_OAICON:
        case VIEW_OADETAILS:
          {
            ODULong vType;
            ODBoolean retVal;
            if (_fViewExtension == kODNULL) {
              _fViewExtension = new ODViewExtension;
              _fViewExtension->InitViewExtension(ev,
                                                 somSelf);
            }
            if (command == VIEW_OATREE) {
              vType = OD_TREEVIEW;
            } else {
              if (command == VIEW_OAICON) {
                vType = OD_ICONVIEW;
              } else {
                vType = OD_DETAILSVIEW;
              }
            }
           
            _fViewExtension->DisplayView(ev,somSelf,vType);
            handled = kODTrue;
          }
          break;
        case IDS_OPENWINDOW:
          somSelf->Open(ev, focusFrame);
          handled = kODTrue;
          break;
        case VIEW_PROPERTIES:
          {
            ODFrameFacetIterator* facets = focusFrame->CreateFacetIterator(ev);
            facets->InitFrameFacetIterator(ev,focusFrame);
            _fSession->GetInfo(ev)->ShowPartFrameInfo(ev,facets->First(ev), kODFalse);
            delete facets;
            handled = kODTrue;
          }
          break;
        case VIEW_SASMALLICON:
          if ((_CurrentView != _SmallIconView) && (isRoot == kODFalse)) {
            focusFrame->ChangeViewType(ev, _SmallIconView);
          }
          handled = kODTrue;
          break;
        case VIEW_SALARGEICON:
          if ((_CurrentView != _LargeIconView) && (isRoot == kODFalse)) {
            focusFrame->ChangeViewType(ev, _LargeIconView);
          }
          handled = kODTrue;
          break;
        case VIEW_SATHUMBNAIL:
          if ((_CurrentView != _ThumbnailView) && (isRoot == kODFalse)) {
            focusFrame->ChangeViewType(ev, _ThumbnailView);
          }
          handled = kODTrue;
          break;
        case VIEW_SAFRAME:
          if (_CurrentView != _FrameView) {
            focusFrame->ChangeViewType(ev, _FrameView);
          }
          handled = kODTrue;
          break;
        case IDMA_ICLUIWINDOW:
          ISWindow();
          handled = kODTrue;
          break;
        case IDMA_PARTINFO:
          ISDialog();
          handled = kODTrue;
          break;
        default:
          break;
      }
    } 

    if (pInfo->fHasSelectedPart && _fExtension &&
        (command >= MENUID_SELECTEDPART_FIRST  &&
         command <= MENUID_SELECTEDPART_LAST)) {
      handled = pInfo->fSelectedPart->HandleEvent(ev, event, pInfo->fSelPartFrame, kODNULL, kODNULL);
      _fExtension->ReleaseSelectedMenu(ev);
      _fMenuBar->RestoreSelectedMenu(ev);
      ODPlatformMenu selPartMenu = _fExtension->GetSelectedMenu(ev);
      _fMenuBar->AddSelectedMenu(ev, selPartMenu);
    }
    return handled;
}

SOM_Scope void  SOMLINK ICPMouseFocusLost(ITSOContainerPart *somSelf,
                                          Environment *ev, 
                                          ODFrame* ownerFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPMouseFocusLost");

    if (_fMouseMode == kTracking) {
      somSelf->ICPUpdateTrackRect(ev, &_ptOrigin, _fTrackingFacet, kUpdateModeEnd);
      _fTrackingFacet = kODNULL;
      _fMouseMode = kNormal;
      WinSetCapture(HWND_DESKTOP, 0);
      WinLockWindowUpdate(HWND_DESKTOP, 0);
    }

    if (_fPolygons) {
      for (int i = 0; i < _fNumPolygons; i++)
      {
        delete[] _fPolygons[i].aPointl;
      }
      delete[] _fPolygons;
      _fPolygons = kODNULL;
    }
}

SOM_Scope ODBoolean  SOMLINK ICPDoClear(ITSOContainerPart *somSelf,
                                        Environment *ev, 
                                        ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPDoClear");

    if (frame == _fSelectionFrame) {
       Proxy* p;
       ODULong seqnum;
       ODULong frgrp;

       ODxOrderedCollection* s = new ODxOrderedCollection;
       ODxOrderedCollectionIterator i(_fSelection);
       for (p = (Proxy*) i.First();
           i.IsNotComplete();
           p = (Proxy*) i.Next())
       {
          s->AddLast(p);
       }

       frame->Invalidate(ev, kODNULL, kODNULL);
       somSelf->ICPEmptySelection(ev);

       ODxOrderedCollectionIterator iter(s);
       for (p = (Proxy*) iter.First();
           iter.IsNotComplete();
           p = (Proxy*) iter.Next())
       {
          p->frame->Invalidate(ev, NULL, kODNULL);
          seqnum = p->frame->GetSequenceNumber(ev);
          frgrp = p->frame->GetFrameGroup(ev);

          ODxOrderedCollection* tempFacets = new ODxOrderedCollection;

          ODFrameFacetIterator* facets = p->frame->CreateFacetIterator(ev);
          for (ODFacet* f1 = facets->First(ev);
              facets->IsNotComplete(ev);
              f1 = facets->Next(ev))
          {
             tempFacets->AddLast(f1);
          }

          delete facets;

          ODxOrderedCollectionIterator t(tempFacets);
          for (ODFacet* f2 = (ODFacet*)t.First();
               t.IsNotComplete();
               f2 = (ODFacet*)t.Next())
          {
             f2->GetContainingFacet(ev)->RemoveFacet(ev, f2);
             ODDeleteObject(f2);
          }

          delete tempFacets;
          somSelf->RemoveEmbeddedFrame(ev, p->frame);

          ODxOrderedCollection* embedframes = new ODxOrderedCollection;     //for openaswindow

          ODEmbeddedFramesIterator* bIter=somSelf->CreateEmbeddedFramesIterator(ev,kODNULL);
          for (ODFrame* frame2 = (ODFrame*) bIter->First(ev);
              bIter->IsNotComplete(ev);
              frame2 = (ODFrame*) bIter->Next(ev))
          {
             embedframes->AddLast(frame2);
          }

          ODxOrderedCollectionIterator iter2(embedframes);      //iterate embedded frames
          for (ODFrame* EmbedFrame = (ODFrame*) iter2.First();  //for same seq. number as
               iter2.IsNotComplete();                         //deleted frame
               EmbedFrame = (ODFrame*) iter2.Next())
          {
              if (frame != EmbedFrame) {
                 ODULong framegrp = EmbedFrame->GetFrameGroup(ev);
                 if (framegrp != frgrp) {
                    ODULong seqnumber = EmbedFrame->GetSequenceNumber(ev);
                    if (seqnumber == seqnum) {
                       Proxy* p2 = somSelf->ICPProxyForFrame(ev, EmbedFrame);
                       p2->frame->Invalidate(ev, NULL, kODNULL);

                       ODxOrderedCollection* tempFacets2 = new ODxOrderedCollection;
                       ODFrameFacetIterator* facets2 = p2->frame->CreateFacetIterator(ev);
                       for (ODFacet* f2 = facets2->First(ev);
                           facets2->IsNotComplete(ev);
                           f2 = facets2->Next(ev))
                       {
                          tempFacets2->AddLast(f2);
                       }
                       delete facets2;
                       ODxOrderedCollectionIterator t2(tempFacets2);
                       for (ODFacet* f3 = (ODFacet*)t2.First();
                           t2.IsNotComplete();
                           f3 = (ODFacet*)t2.Next())
                       {
                          f3->GetContainingFacet(ev)->RemoveFacet(ev, f3);
                          ODDeleteObject(f3);
                       }

                       delete tempFacets2;
                       somSelf->RemoveEmbeddedFrame(ev,p2->frame);
                    } /* endif */
                 } /* endif */
              } /* endif */
          }
          delete embedframes;
      }
      delete s;
      somSelf->ICPClipEmbeddedFrames(ev, frame);
      somSelf->GetStorageUnit(ev)->GetDraft(ev)->SetChangedFromPrev(ev);
    }
    return kODTrue;
}

SOM_Scope void  SOMLINK ICPActivateFrame(ITSOContainerPart *somSelf,
                                         Environment *ev, 
                                         ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPActivateFrame");

    if (frame != kODNULL) {
      PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);
      if (!(pInfo->fIsActive)) {
        ODBoolean succeeded = kODFalse;
        succeeded = _fSession->GetArbitrator(ev)->RequestFocusSet(ev, _fFocusSet,frame);
        if (succeeded) {
          somSelf->FocusAcquired(ev, _fSelectionFocus, frame);
          somSelf->FocusAcquired(ev, _fMenuFocus, frame);
          somSelf->FocusAcquired(ev, _fKeyFocus, frame);
          somSelf->FocusAcquired(ev, _fStatusFocus,frame); 
        }
      }
    }
}

SOM_Scope void  SOMLINK ICPDeActivateFrame(ITSOContainerPart *somSelf,
                                           Environment *ev,
                                           ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPDeActivateFrame");

    if (frame != kODNULL) {
      _fSession->GetArbitrator(ev)->RelinquishFocusSet(ev, _fFocusSet,frame);
      somSelf->FocusLost(ev, _fSelectionFocus, frame);
      somSelf->FocusLost(ev, _fMenuFocus, frame);
      somSelf->FocusLost(ev, _fKeyFocus, frame);
      somSelf->FocusLost(ev, _fStatusFocus,frame);
    }
}

SOM_Scope void  SOMLINK ICPInstallMenus(ITSOContainerPart *somSelf,
                                        Environment *ev)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPInstallMenus");

    HWND  hwndMenu = NULLHANDLE;
    HWND  hwndMoveSubmenu = NULLHANDLE;

    ODPlatformMenuItem mi;

    hwndMenu = WinLoadMenu(HWND_OBJECT,
                           _hmod,
                           IDM_ITSOCONTAINER);

    _fMenuBar->AddMenuLast(ev, IDM_ITSOCONTAINER, hwndMenu, somSelf);
    _fMenuBar->SetMenuItemText(ev, IDM_ITSOCONTAINER, kODNULL, "ITSOContainerPart");

    memset((PCH)&mi, 0, sizeof(MENUITEM));
    mi.afStyle = MIS_TEXT;
    mi.id = VIEW_REFRESH;
    _fMenuBar->AddMenuItemBefore(ev, IDMS_VIEW, kODNULL, &mi, VIEW_OPENAS);
    _fMenuBar->SetMenuItemText(ev, IDMS_VIEW, VIEW_REFRESH, "Refresh");

    memset((PCH)&mi, 0, sizeof(MENUITEM));
    mi.afStyle = MIS_TEXT;
    mi.id = IDS_OPENWINDOW;
    _fMenuBar->AddMenuItemLast(ev, IDMS_VIEW, VIEW_OPENAS, &mi);
    _fMenuBar->SetMenuItemText(ev, IDMS_VIEW, IDS_OPENWINDOW, "Window");

    memset((PCH)&mi, 0, sizeof(MENUITEM));
    mi.id = IDMA_PARTINFO;
    mi.afStyle = MIS_TEXT;
    _fMenuBar->AddMenuItemLast(ev, IDMS_HELP, kODNULL, &mi);
    _fMenuBar->SetMenuItemText(ev, IDMS_HELP, IDMA_PARTINFO,"ITSOContainerPart Info...");

    hwndMoveSubmenu = WinLoadMenu(HWND_OBJECT,
                                  _hmod,
                                  IDM_ARRANGE);

    _fMenuBar->InsertSubmenu(ev, IDMS_EDIT, EDIT_MOVE, hwndMoveSubmenu);

     memset((PCH)&mi, 0, sizeof(MENUITEM));
     mi.id = IDMA_ICLUIWINDOW;
     mi.afStyle = MIS_TEXT;
     _fPopup->AddMenuItemLast(ev, kODNULL, kODNULL, &mi);
     _fPopup->SetMenuItemText(ev, kODNULL, IDMA_ICLUIWINDOW, "ICLUI Window...");
}

SOM_Scope void  SOMLINK ICPCreateEmbeddedFacets(ITSOContainerPart *somSelf,
                                                Environment *ev, 
                                                ODFrame* containingFrame, 
                                                ODFrame* embeddedFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPCreateEmbeddedFacets");

    ODFacet* facet;
    ODFrameFacetIterator* facets = containingFrame->CreateFacetIterator(ev);

    for (facet = facets->First(ev);
         facets->IsNotComplete(ev);
         facet = facets->Next(ev))
    {
      ODShape* clip = new ODShape;
      ODShape* tempShape = embeddedFrame->AcquireFrameShape(ev, kODNULL);
      clip->CopyFrom(ev,tempShape);
      tempShape->Release(ev);
      ODTransform* xform = new ODTransform;
      xform->CopyFrom(ev, somSelf->ICPProxyForFrame(ev, embeddedFrame)->transform);
      facet->CreateEmbeddedFacet(ev,
                                 embeddedFrame,
                                 clip,
                                 xform,
                                 kODNULL,
                                 kODNULL,
                                 kODNULL,
                                 kODFrameInFront);
    }
    delete facets;
}

SOM_Scope ODWindow*  SOMLINK ICPCreateWindow(ITSOContainerPart *somSelf,
                                             Environment *ev, 
                                             ODFrame* sourceFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPCreateWindow");

    Rect windRect;
    ODPlatformWindow platformWindow = kODNULL;
    ODWindow* window = kODNULL;
  
    if (sourceFrame &&
        !sourceFrame->IsRoot(ev)) {
      SWP swp;
      swp.x = 100;
      swp.y = 100;
      swp.cx = 400;
      swp.cy = 400;

      platformWindow = 
        _fSession->CreatePlatformWindow(ev,
                                        ODPlatformWindowDefaultCreateOptions | 
                                        FCF_HORZSCROLL | FCF_VERTSCROLL);
      WinSetWindowPos(platformWindow,
                      HWND_TOP,
                      swp.x,
                      swp.y,
                      swp.cx,
                      swp.cy,
                      SWP_SIZE | SWP_MOVE);
    } else {
      platformWindow = _fSession->CreatePlatformWindow(ev, ODPlatformWindowDefaultCreateOptions);
    } 

    window = _fSession->
                GetWindowState(ev)->
                     RegisterWindow(ev,
                           platformWindow,
                           kODFrameObject,
                           (sourceFrame == kODNULL),   // is root
                           kODTrue,                    // Is resizable
                           kODFalse,                   // Is floating
                           kODTrue,                    // should save
                          _fPartWrapper,
                          _FrameView,
                          _PresDefault,
                          sourceFrame);
    return window;
}


// This routin clips the selection handles from each selected embedded frame by
// iterating through the embedded frames and then performing the clip on the
// facet
SOM_Scope void  SOMLINK ICPClipSelectionFromEmbeddedFrames(ITSOContainerPart *somSelf,
                                                           Environment *ev, 
                                                           ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPClipSelectionFromEmbeddedFrames");

    ODFrameFacetIterator* facets = frame->CreateFacetIterator(ev);

    for (ODFacet* facet = facets->First(ev);
         facets->IsNotComplete(ev); 
         facet = facets->Next(ev))
    {
       somSelf->ICPClipSelectionFromEmbeddedFacets(ev, facet);
    }

    delete facets;
    if (!_fSelection->Count()) {
       /*
        * Check to make sure that there are no more entries in
        * _fSavedFacetClipShapes.  Any remaining entries are there
        * because the facet has been deleted so we only need to
        * release the clip shape and remove the entry.
        */
       while (_fSavedFacetClipShapes->Count())
       {
          FacetClip* facetClip = (FacetClip*)_fSavedFacetClipShapes->First();
          facetClip->clip->Release(ev);
          _fSavedFacetClipShapes->RemoveFirst();
          delete facetClip;
       }
    }
}

// Basically if a facet is selected its clip shape is updated not to 
// include the selection border thus clipping this data
SOM_Scope void  SOMLINK ICPClipSelectionFromEmbeddedFacets(ITSOContainerPart *somSelf,
                                                           Environment *ev, 
                                                           ODFacet* facet)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPClipSelectionFromEmbeddedFacets");

    ODRect bbox;
    ODPoint ptHandles[4];

    if (_fSelection->Count() == 1) {
       /*
        * Transform the coordinates of the selection handles for the selected
        * frame to our local coordinates.
        */

       ODRect bbox;
       Proxy* p = (Proxy*)_fSelection->First();

       ODShape* frameShape = p->frame->AcquireFrameShape(ev, kODNULL);
       frameShape->GetBoundingBox(ev, &bbox);
//       frameShape->Release(ev);    // DevCon 8 bug, not all Acquire..()'s inc RefCnt 

       ptHandles[0].x = bbox.left;  ptHandles[0].y = bbox.bottom;
       ptHandles[1].x = bbox.left;  ptHandles[1].y = bbox.top;
       ptHandles[2].x = bbox.right; ptHandles[2].y = bbox.top;
       ptHandles[3].x = bbox.right; ptHandles[3].y = bbox.bottom;
       p->transform->TransformPoints(ev, ptHandles, 4);
    }

    ODFacetIterator* facets = facet->CreateFacetIterator(ev, kODChildrenOnly, kODFrontToBack);

    for (ODFacet* embFacet = facets->First(ev);
         facets->IsNotComplete(ev);
         embFacet = facets->Next(ev))
    {
       /*
        * We only clip the selection handles from embedded facets for the
        * single selection case since for multiple selection, the handle are
        * just adornments and do not respond to mouse input.
        */
       ODBoolean clipHandles = kODFalse;
       if (_fSelection->Count() == 1) {
          /*
           * Instead of blindly subtracting the handle shape from each
           * embedded frame, we'll attempt to be intelegent about it and
           * only subtract the handle shape from those frames that we determine
           * overlap the handles.
           */

          int i;
          ODFrame* embFrame = embFacet->GetFrame(ev);
          ODPoint ptHandlesLocal[4];
          memcpy(ptHandlesLocal, ptHandles, sizeof(ptHandles));
          Proxy* p = somSelf->ICPProxyForFrame(ev, embFrame);
          for (i = 0; i < 4; i++)
          {
             ptHandlesLocal[i] = p->transform->InvertPoint(ev, &ptHandlesLocal[i]);
          }

          /*
           * Determine if any of the sizing handles might overlap the frame
           * rectangle.  We inflate the frame rectangle by half the width of
           * the handles multiplied by 1.5 to approximate the diagonal from
           * the center point of the handle to a corner since the frame might
           * be rotated with respect to the handles.
           */

          ODShape* frameShape = embFrame->AcquireFrameShape(ev, kODNULL);
          frameShape->GetBoundingBox(ev, &bbox);
//          frameShape->Release(ev);    // DevCon 8 bug, not all Acquire..()'s inc RefCnt 

          ODCoordinate width = kSelectionHandleWidth >> 1; // divide by 2
          width = width + (width >> 1);    // multiply by 1.5
          InflateRect(bbox, width, width);   //Defined in common.hpp

          for (i = 0; i < 4; i++)
          {
             /*
              * Does the rectangle contain the point.
              */
             if (ptHandlesLocal[i].x >= bbox.left   &&
                 ptHandlesLocal[i].x <  bbox.right  &&
                 ptHandlesLocal[i].y >= bbox.bottom &&
                 ptHandlesLocal[i].y <  bbox.top) {
                clipHandles = kODTrue;
                break;
             }
          }
       }

       FacetClip* facetClip = somSelf->ICPFindFacetClip(ev, embFacet);

       if (clipHandles) {
          if (!facetClip) {
             facetClip = new FacetClip;
             facetClip->facet = embFacet;
             facetClip->clip = embFacet->AcquireClipShape(ev, kODNULL);
             _fSavedFacetClipShapes->AddFirst(facetClip);
          }

          ODShape* newClipShape = facetClip->clip->Copy(ev);
          Proxy* p = somSelf->ICPProxyForFrame(ev, embFacet->GetFrame(ev));
          newClipShape->Transform(ev, p->transform);
          newClipShape->Subtract(ev, _fSelectSizingHandlesShape);

          ODTransform* invert = p->transform->Copy(ev)->Invert(ev);
          newClipShape->Transform(ev, invert);
          invert->Release(ev);

          embFacet->ChangeGeometry(ev, newClipShape, kODNULL, kODNULL);
          newClipShape->Release(ev);
       } else {
          if (facetClip) {
             embFacet->ChangeGeometry(ev, facetClip->clip, kODNULL, kODNULL);
             facetClip->clip->Release(ev);
             _fSavedFacetClipShapes->Remove(facetClip);
             delete facetClip;
          }
       }
    }
    delete facets;
}

// This routine draws the selection border around the selected object
SOM_Scope void  SOMLINK ICPHighlightProxyBorder(ITSOContainerPart *somSelf,
                                                Environment *ev, 
                                                Proxy* p, 
                                                ODFacet* facet)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPHighlightProxyBorder");

    if (_fSelection->Count()) {
       AREABUNDLE areaBundle;
       LINEBUNDLE lineBundle;
       MATRIXLF mtx, mtxSave;
       HPS hps = facet->GetCanvas(ev)->GetPlatformCanvas(ev, kODGPI)->GetPS(ev, facet);
       areaBundle.lColor = (_fSelection->Count() == 1) ? CLR_BLUE : CLR_PALEGRAY;
       GpiSetAttrs(hps, PRIM_AREA, ABB_COLOR, 0, &areaBundle);
       GpiQueryModelTransformMatrix(hps, 9, &mtxSave);
       Proxy* p;
       ODxOrderedCollectionIterator i(_fSelection);

       for (p = (Proxy*) i.First(); i.IsNotComplete(); p = (Proxy*) i.Next())
       {
          ODRect bbox;
          ODPoint pt[4];
          ODPOINTL ptl;

          ODShape* frameShape = p->frame->AcquireFrameShape(ev, kODNULL);
          frameShape->GetBoundingBox(ev, &bbox);
//          frameShape->Release(ev);     // DevCon 8 bug, not all Acquire..()'s inc RefCnt 

          pt[0].x = bbox.left;  pt[0].y = bbox.bottom;
          pt[1].x = bbox.left;  pt[1].y = bbox.top;
          pt[2].x = bbox.right; pt[2].y = bbox.top;
          pt[3].x = bbox.right; pt[3].y = bbox.bottom;
          p->transform->TransformPoints(ev, pt, 4);
          lineBundle.lColor = CLR_WHITE;
          lineBundle.usMixMode = FM_OVERPAINT;
          GpiSetAttrs(hps, PRIM_LINE, LBB_COLOR | LBB_MIX_MODE, 0, &lineBundle);
          GpiSetLineType(hps, LINETYPE_DOT);
          for (int i = 0; i < 4; i++)
          {
             ODPoint point = pt[i];
             point.x -= (kSelectionHandleWidth / 2);
             point.y -= (kSelectionHandleWidth / 2);
             ptl = ODPOINTL(point);
             GpiMove(hps, &ptl);
             point.x += kSelectionHandleWidth;
             point.y += kSelectionHandleWidth;
             ptl = ODPOINTL(point);
             GpiBox(hps, DRO_OUTLINEFILL, &ptl, 0, 0);
          }

          p->transform->GetMATRIXLF(ev, &mtx);
          GpiSetModelTransformMatrix(hps, 9, &mtx, TRANSFORM_PREEMPT);
          lineBundle.lColor = CLR_BLUE;
          lineBundle.usMixMode = FM_INVERT;
          GpiSetAttrs(hps, PRIM_LINE, LBB_COLOR | LBB_MIX_MODE, 0, &lineBundle);
          GpiSetLineType(hps, LINETYPE_DOT);
          InflateRect(bbox, kSelectionHandleWidth / 2, kSelectionHandleWidth / 2);
          ODRECTL rclBox(bbox);
          ptl.x = rclBox.xLeft;  ptl.y = rclBox.yBottom;
          GpiMove(hps, &ptl);
          ptl.x = rclBox.xRight; ptl.y = rclBox.yTop;

          GpiBox(hps, DRO_OUTLINE, &ptl, 0, 0);
          GpiSetModelTransformMatrix(hps, 9, &mtxSave, TRANSFORM_REPLACE);
       }
       facet->GetCanvas(ev)->GetPlatformCanvas(ev, kODGPI)->ReleasePS(ev, facet);
    }
}

SOM_Scope void  SOMLINK ICPEmptySelection(ITSOContainerPart *somSelf,
                                          Environment *ev)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPEmptySelection");

    if (_fSelection->Count()) {
       Proxy* p;
       for(ODULong i = _fSelection->Count(); i > 0; i--)
       {
          p = (Proxy*)_fSelection->First();
          somSelf->ICPSetFrameSelected(ev, p->frame, kODFalse);
          _fSelection->RemoveFirst();
       }

       _fSelectSizingHandlesShape->Reset(ev);

       PartInfoRec* pInfo = (PartInfoRec*) _fSelectionFrame->GetPartInfo(ev);
       pInfo->fHasSelectedPart = kODFalse;

       if (_fExtension) {
           _fExtension->ReleaseSelectedMenu(ev);
           _fExtension->ReleaseSelectedPopup(ev);
           pInfo->fSelectedPart->ReleaseExtension(ev, _fExtension);
           _fExtension = kODNULL;
           _fPopup->RemoveSelectedMenu(ev);
           pInfo->fSelPopupAttached = kODFalse;
       } /* endif */

       pInfo->fSelectedPart = kODNULL;
       pInfo->fSelPartFrame = kODNULL;
       pInfo->fRestoreSelMenu = kODTrue;

       //do we need to restore the default Selected menu
       if (pInfo->fRestoreSelMenu) {
           _fMenuBar->RestoreSelectedMenu(ev);
           pInfo->fRestoreSelMenu = kODFalse;
       } /* endif */
       _fSelectionFrame = kODNULL;
    }
}

SOM_Scope ODBoolean  SOMLINK ICPHandleButton1Click(ITSOContainerPart *somSelf,
                                                   Environment *ev, 
                                                   ODFacet* facet, 
                                                   ODPoint* where, 
                                                   ODEventData* event)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPHandleButton1Click");

    ODBoolean handled = kODFalse;

    if (_fMouseMode == kNormal) {
       ODWindow *tempWindow = facet->AcquireWindow(ev);
       if (!tempWindow->IsActive(ev)) {
          tempWindow->Select(ev);
       }
//       tempWindow->Release(ev);  // DevCon 8 bug, not all Acquire..()'s inc RefCnt 
       if (event->msg == WM_BUTTON1CLICK) {
          somSelf->ICPActivateFrame(ev, facet->GetFrame(ev));
          if (_fSelection->Count()) {
             facet->GetFrame(ev)->Invalidate(ev, kODNULL, kODNULL);
             somSelf->ICPEmptySelection(ev);
             somSelf->ICPClipSelectionFromEmbeddedFrames(ev, facet->GetFrame(ev));
          }
          handled = kODTrue;
       }
    } else {
       handled = kODTrue;
    }
    return handled;
}

SOM_Scope ODBoolean  SOMLINK ICPHandleButton1ClickInEmbeddedFrame(ITSOContainerPart *somSelf,
                                                                  Environment *ev, 
                                                                  ODFacet* container, 
                                                                  ODFacet* facet, 
                                                                  ODPoint* where, 
                                                                  ODEventData* event)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPHandleButton1ClickInEmbeddedFrame");

    ODWindow* tempWindow = container->AcquireWindow(ev);
    if (!tempWindow->IsActive(ev))
    {
       tempWindow->Select(ev);
    }
//    tempWindow->Release(ev); // DevCon 8 bug, not all Acquire..()'s inc RefCnt 

    somSelf->ICPActivateFrame(ev, container->GetFrame(ev));
    Proxy* p = somSelf->ICPProxyForFrame(ev, facet->GetFrame(ev));
    PartInfoRec* pInfo = (PartInfoRec*) container->GetFrame(ev)->GetPartInfo(ev);

    if (event->msg == WM_BUTTON1CLICK ||
        (event->msg == WM_BUTTON1CLICK  &&
         (SHORT2FROMMP(event->mp2) & (KC_ALT | KC_SHIFT | KC_CTRL)))) {
       if (_fSelection->Count() == 0) {
          somSelf->ICPSetFrameSelected(ev, p->frame, kODTrue);
          _fSelection->AddFirst((ElementType)p);
       } else {
          container->GetFrame(ev)->Invalidate(ev, kODNULL, kODNULL);
          /*
           * If frame is already in selection then remove it from selection
           * else add it to selection.
           */
          Proxy* proxy;
          ODxOrderedCollectionIterator i(_fSelection);
          ODFrame* frame = facet->GetFrame(ev);

          for (proxy = (Proxy*) i.First();
               i.IsNotComplete();
               proxy = (Proxy*) i.Next())
          {
             if (frame == proxy->frame) {
               break;
             }
          }

          if (i.IsNotComplete()) {
             somSelf->ICPSetFrameSelected(ev, p->frame, kODFalse);

             _fSelection->Remove((ElementType)p);
          } else {
             somSelf->ICPSetFrameSelected(ev, p->frame, kODTrue);
             _fSelection->AddFirst((ElementType)p);
          }
       }

       switch (_fSelection->Count())
       {
          case 1:
              pInfo->fRestoreSelMenu = kODFalse;
              pInfo->fHasSelectedPart = kODTrue;
              pInfo->fSelPartFrame = p->frame;
              pInfo->fSelectedPart = p->frame->AcquirePart( ev);
              pInfo->fSelectedPart->Acquire(ev); // DevCon 8 bug, not all Acquire..()'s inc RefCnt 
              if (pInfo->fSelectedPart->HasExtension(ev, kODMenuExtension)) {
                _fExtension = (ODMenuExtension*)(pInfo->fSelectedPart->AcquireExtension(ev, kODMenuExtension));
                ODPlatformMenu selPartMenu = _fExtension->GetSelectedMenu(ev);
                _fMenuBar->AddSelectedMenu(ev, selPartMenu);
                pInfo->fRestoreSelMenu = kODTrue;
              } else {
                _fMenuBar->SetNothingAvailable(ev);
                pInfo->fRestoreSelMenu = kODTrue;
              } /* endif */
              break;
          default:
              pInfo->fHasSelectedPart = kODFalse;
              if (_fExtension) {
                _fExtension->ReleaseSelectedMenu(ev);
                _fExtension->ReleaseSelectedPopup(ev);
                pInfo->fSelectedPart->ReleaseExtension(ev, _fExtension);
                _fExtension = kODNULL;
                _fPopup->RemoveSelectedMenu(ev);
                pInfo->fSelPopupAttached = kODFalse;
                pInfo->fSelectedPart = kODNULL;
                _fSelectionFrame = kODNULL;
                _fMenuBar->RestoreSelectedMenu(ev);
                _fMenuBar->SetNothingAvailable(ev);
              } /* endif */
              break;
       }
       somSelf->ICPCreateSelectSizingHandlesShape(ev);
       somSelf->ICPClipSelectionFromEmbeddedFrames(ev, container->GetFrame(ev));
       container->GetFrame(ev)->Invalidate(ev, kODNULL, kODNULL);
    }
    return kODTrue;
}

SOM_Scope ODBoolean  SOMLINK ICPHandleButton1ClickInBorder(ITSOContainerPart *somSelf,
                                                           Environment *ev, 
                                                           ODFacet* container, 
                                                           ODFacet* facet, 
                                                           ODPoint* where, 
                                                           ODEventData* event)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPHandleButton1ClickInBorder");

    ODWindow* tempWindow = container->AcquireWindow(ev);
    if (!tempWindow->IsActive(ev)) {
       tempWindow->Select(ev);
    }
    tempWindow->Release(ev);

    somSelf->ICPActivateFrame(ev, container->GetFrame(ev));
    Proxy* p = somSelf->ICPProxyForFrame(ev, facet->GetFrame(ev));
    PartInfoRec* pInfo = (PartInfoRec*) container->GetFrame(ev)->GetPartInfo(ev);
    somSelf->ICPSetFrameSelected(ev, p->frame, kODTrue);
    _fSelection->AddFirst((ElementType)p);
    pInfo->fHasSelectedPart = kODTrue;
    pInfo->fRestoreSelMenu = kODFalse;
    pInfo->fSelectedPart = p->frame->AcquirePart(ev);
    pInfo->fSelectedPart->Acquire(ev); // DevCon8 Bug in AcquirePart 
                                       //  above did not increment refct
    if (pInfo->fSelectedPart->HasExtension(ev, kODMenuExtension)) {
      pInfo->fSelPartFrame = p->frame;
      _fExtension = (ODMenuExtension*)(pInfo->
                                         fSelectedPart->
                                            AcquireExtension(ev, kODMenuExtension));
      ODPlatformMenu selPartMenu = _fExtension->GetSelectedMenu(ev);
      _fMenuBar->AddSelectedMenu(ev, selPartMenu);
      pInfo->fRestoreSelMenu = kODTrue;
    } else {
      _fMenuBar->SetNothingAvailable(ev);
      pInfo->fRestoreSelMenu = kODTrue;
    } /* endif */

    somSelf->ICPCreateSelectSizingHandlesShape(ev);
    somSelf->ICPClipSelectionFromEmbeddedFrames(ev, container->GetFrame(ev));
    container->GetFrame(ev)->Invalidate(ev, kODNULL, kODNULL);
    return kODTrue;
}

SOM_Scope ODBoolean  SOMLINK ICPHandleKeyDown(ITSOContainerPart *somSelf,
                                              Environment *ev, 
                                              ODFrame* focusFrame, 
                                              ODEventData* event)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPHandleKeyDown");

    ODBoolean handled = kODFalse;

    if ((SHORT1FROMMP(event->mp1) & KC_VIRTUALKEY) &&
             SHORT2FROMMP(event->mp2) == VK_ESC) {
       if (_fMouseMode != kNormal) {
          somSelf->ICPResetMouseMode(ev);
       }
       handled = kODTrue;
    }
    return handled;
}

SOM_Scope ODBoolean  SOMLINK ICPHandleMouseMove(ITSOContainerPart *somSelf,
                                                Environment *ev, 
                                                ODFacet* facet, 
                                                ODFrame* frame, 
                                                ODPoint* where,
                                                ODEventData* event)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPHandleMouseMove");

    ODBoolean handled = kODFalse;
    RECTL rclBox;
    PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);

    switch (_fMouseMode)
    {
       case kTracking:
          if (_fTrackingFacet) {
            if (!SHORT1FROMMP(event->mp2)) {    // Mouse still captured? 

              ODTransform* xform = _fTrackingFacet->AcquireWindowFrameTransform(ev, _fTrackingFacet->GetCanvas(ev));
              ODPoint localPoint = xform->InvertPoint(ev, where);      // window to frame coords
              xform->Release(ev);

              somSelf->ICPUpdateTrackRect(ev, &localPoint, _fTrackingFacet, kUpdateModeContinue);
              _ptEnd = localPoint;
            } else {                           // mouse is no longer captured 
              somSelf->ICPResetMouseMode(ev);
            }
          } else {
            ODWindow* tempWindow = facet->AcquireWindow(ev);
            if (tempWindow->IsActive(ev) ) {
               if (_fSelection->Count() >= 1) {
                  ODPoint mouse = *where;
                  ODTransform* xform = facet->AcquireWindowContentTransform(ev, facet->GetCanvas(ev));
                  ODRect bbox;
                  mouse = xform->InvertPoint(ev, &mouse);
                  xform->Release(ev);
               }
            }
//            tempWindow->Release(ev); // DevCon 8 bug, not all Acquire..()'s inc RefCnt 
          }
          handled = kODTrue;
          break;
       default:
          break;
    }
    return handled;
}

SOM_Scope ODBoolean  SOMLINK ICPHandleButton1MotionEnd(ITSOContainerPart *somSelf,
                                                       Environment *ev, 
                                                       ODFacet* facet, 
                                                       ODFrame* frame, 
                                                       ODPoint* where, 
                                                       ODEventData* event)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPHandleButton1MotionEnd");

    PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);

    if (_fMouseMode == kTracking &&
        (_fTrackingMode == kRubberBandSelection ||
         _fTrackingMode == kBotLeftResize       ||
         _fTrackingMode == kTopLeftResize       ||
         _fTrackingMode == kTopRightResize      ||
         _fTrackingMode == kBotRightResize))
    {
      /*
       * _fTrackingFacet is cleared in ResetMouseMode so save the pointer
       */
      ODFacet* trackingFacet = _fTrackingFacet;
      somSelf->ICPResetMouseMode(ev);

      ODTransform* xform = trackingFacet->AcquireWindowContentTransform(ev, kODNULL);
      ODPoint localPoint = xform->InvertPoint(ev, where);      // window to content coords
      xform->Release(ev);

      switch (_fTrackingMode)
      {
         case kRubberBandSelection:
         {
            /* select the embedded frames within the tracking rectangle */
            ODRect trackRect;
            trackRect.left   = min(_ptOrigin.x, localPoint.x);
            trackRect.bottom = min(_ptOrigin.y, localPoint.y);
            trackRect.right  = max(_ptOrigin.x, localPoint.x);
            trackRect.top    = max(_ptOrigin.y, localPoint.y);
            ODShape* trackShape = trackingFacet->GetFrame(ev)->CreateShape(ev);
            trackShape->SetRectangle(ev, &trackRect);

            ODTransform* contentXform = trackingFacet->AcquireContentTransform(ev, kODNULL);
            trackShape->Transform(ev, contentXform);
            contentXform->Release(ev);

            /*
             * Iterate thru the embedded facets of the tracking facet and add
             * those embedded facets that are contained within the tracking
             * shape to the selection.
             */

            ODFacetIterator* facets = trackingFacet->CreateFacetIterator(ev, kODChildrenOnly, kODFrontToBack);

            for (ODFacet* embFacet = facets->First(ev);
                  facets->IsNotComplete(ev);
                  embFacet = facets->Next(ev))
            {
               if (!embFacet) {
                 break;
               }

               Proxy* p = somSelf->ICPProxyForFrame(ev, embFacet->GetFrame(ev));
               //below 2 lines only used when selection is 1 embed
               pInfo->fSelectedPart = p->frame->AcquirePart( ev);
               pInfo->fSelectedPart->Acquire(ev);  // DevCon 8 bug, not all Acquire..()'s inc RefCnt 
               pInfo->fSelPartFrame = p->frame;
               ODShape* tempShape = p->frame->AcquireUsedShape(ev,kODNULL);
               ODShape* usedShape = ODCopyAndRelease(ev,tempShape);

               ODTransform* xform = embFacet->AcquireFrameTransform(ev, kODNULL);
               usedShape->Transform(ev, xform);
               xform->Release(ev);

               usedShape->Subtract(ev, trackShape);

               if (usedShape->IsEmpty(ev)) {
                  somSelf->ICPSetFrameSelected(ev, p->frame, kODTrue);
                  _fSelection->AddFirst((ElementType)p);
               }
               usedShape->Release(ev);
            }
            trackShape->Release(ev);

            if (_fSelection->Count()) {
               somSelf->ICPCreateSelectSizingHandlesShape(ev);
               somSelf->ICPClipSelectionFromEmbeddedFrames(ev, trackingFacet->GetFrame(ev));
               trackingFacet->GetFrame(ev)->Invalidate(ev, kODNULL, kODNULL);

               switch (_fSelection->Count())
               {
                  case 0:
                     break;
                  case 1:
                      pInfo->fRestoreSelMenu = kODFalse;
                      pInfo->fHasSelectedPart = kODTrue;
                      if (pInfo->fSelectedPart->HasExtension(ev, kODMenuExtension))
                      {
                        _fExtension = (ODMenuExtension*)(pInfo->fSelectedPart->AcquireExtension(ev, kODMenuExtension));
                        ODPlatformMenu selPartMenu = _fExtension->GetSelectedMenu(ev);
                        _fMenuBar->AddSelectedMenu(ev, selPartMenu);
                        pInfo->fRestoreSelMenu = kODTrue;
                      } else {
                        _fMenuBar->SetNothingAvailable(ev);
                        pInfo->fRestoreSelMenu = kODTrue;
                      } /* endif */
                      break;
                  default:    //multiple selection
                      pInfo->fHasSelectedPart = kODFalse;
                      if (_fExtension) {
                        _fExtension->ReleaseSelectedMenu(ev);
                        _fExtension->ReleaseSelectedPopup(ev);
                        pInfo->fSelectedPart->ReleaseExtension(ev, _fExtension);
                        _fExtension = kODNULL;
                        _fPopup->RemoveSelectedMenu(ev);
                        pInfo->fSelPopupAttached = kODFalse;
                        pInfo->fSelectedPart = kODNULL;
                        pInfo->fSelPartFrame = kODNULL;
                        _fSelectionFrame = kODNULL;
                      } /* endif */

                      _fMenuBar->RestoreSelectedMenu(ev);
                      _fMenuBar->SetNothingAvailable(ev);
                      break;
               }
            }
         }
         break;
         case kBotLeftResize:
         case kTopLeftResize:
         case kTopRightResize:
         case kBotRightResize:
         {
            ODPoint ptNewEnd, ptSelMouse, ptOffset;
            ODRect bbox;
            ptNewEnd.x = localPoint.x - _ptMouseOffset.x;
            ptNewEnd.y = localPoint.y - _ptMouseOffset.y;
            Proxy* p = (Proxy*)_fSelection->First();

            ODShape* frameShape = p->frame->AcquireFrameShape(ev, kODNULL);
            frameShape->GetBoundingBox(ev, &bbox);
            frameShape->Release(ev);

            ptSelMouse = p->transform->InvertPoint(ev, &ptNewEnd); // in selections frame coords
            switch (_fTrackingMode)
            {
               case kBotLeftResize:
                  if(ptSelMouse.x > bbox.right || ptSelMouse.y > bbox.top) {
                    return kODTrue;    
                  }

                  bbox.right += (-ptSelMouse.x);
                  bbox.top   += (-ptSelMouse.y);
                  ptOffset = ptSelMouse;
                  break;
               case kTopLeftResize:
                  if(ptSelMouse.x > bbox.right || ptSelMouse.y < bbox.bottom) {
                    return kODTrue;  
                  }

                  bbox.right += (-ptSelMouse.x);
                  bbox.top   = ptSelMouse.y;
                  ptOffset.x = ptSelMouse.x;
                  ptOffset.y = 0;
                  break;
               case kTopRightResize:
                  if(ptSelMouse.x < bbox.left || ptSelMouse.y < bbox.bottom) {
                    return kODTrue;   
                  }

                  bbox.right = ptSelMouse.x;
                  bbox.top   = ptSelMouse.y;
                  ptOffset.x = ptOffset.y = 0;
                  break;
               case kBotRightResize:
                  if(ptSelMouse.x < bbox.left || ptSelMouse.y > bbox.top) {
                    return kODTrue;
                  }

                  bbox.right = ptSelMouse.x;
                  bbox.top   += (-ptSelMouse.y);
                  ptOffset.x = 0;
                  ptOffset.y = ptSelMouse.y;
                  break;
            }

            frameShape = p->frame->CreateShape(ev);
            frameShape->SetRectangle(ev, &bbox);
            p->frame->ChangeFrameShape(ev, frameShape, kODNULL);
            frameShape->Release(ev);
            if (ptOffset.x || ptOffset.y) {
               ODTransform* xform = p->frame->CreateTransform(ev);
               xform->SetOffset(ev, &ptOffset);
               p->transform->PreCompose(ev, xform);
               xform->Release(ev);
               p->transformChanged = kODTrue;
            }
         }
         break;
         default:
           break;
      }

      somSelf->ICPCreateSelectSizingHandlesShape(ev);
      somSelf->ICPClipEmbeddedFrames(ev, trackingFacet->GetFrame(ev));
      trackingFacet->Invalidate(ev, kODNULL, kODNULL);
      somSelf->GetStorageUnit(ev)->GetDraft(ev)->SetChangedFromPrev(ev);
    }
    return kODTrue;
}

// This method removes the mouse focus from the tracking facet
SOM_Scope void  SOMLINK ICPResetMouseMode(ITSOContainerPart *somSelf,
                                          Environment *ev)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPResetMouseMode");

    if (_fMouseMode == kTracking) {
       ODFrame* frame = _fTrackingFacet->GetFrame(ev);
       _fSession->GetArbitrator(ev)->RelinquishFocus(ev, _fMouseFocus, frame);
       somSelf->FocusLost(ev, _fMouseFocus, frame);
       somSelf->ICPDisplayStatusLineText(ev,"",frame);
    } else {
      if (_fMouseMode != kNormal) {
        ODFrame* frame = _fTrackingFacet->GetFrame(ev);
        somSelf->ICPFreezeEmbeddedFrames(ev, kODFalse);
        WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));
        somSelf->ICPDisplayStatusLineText(ev,"",frame);
      }
    }
    _fMouseMode = kNormal;
}

// This method updates the tracking rectangle associated with mouse movement
SOM_Scope void  SOMLINK ICPUpdateTrackRect(ITSOContainerPart *somSelf,
                                           Environment *ev,
                                           ODPoint* ptMouse, 
                                           ODFacet* facet, 
                                           TrackingUpdateMode updateMode)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPUpdateTrackRect");

    HPS hps;

    ODPOINTL ptlOrigin(_ptOrigin), ptlEnd(_ptEnd), ptlMouse(*ptMouse),
             ptlMouseOffset(_ptMouseOffset), ptlRef(_ptRef), ptl;

    MATRIXLF mtx;
    int i;
    CFocusWindow f(ev, facet, (ODShape*)kODNULL, &hps, (HWND*)kODNULL, CFocusWindow::LockedWindowUpdatePS);
    GpiSetMix(hps, FM_INVERT);

    switch (_fTrackingMode)
    {
       case kBotLeftResize:
       case kTopLeftResize:
       case kTopRightResize:
       case kBotRightResize:
         {
            ODPoint pt, ptSelEnd, ptSelMouse, ptSelMouseOffset, ptSelOrigin, ptSelRef;
            FIXED afxScale[2];
            int i;
            Proxy* p = (Proxy*)_fSelection->First();

            /* Adjust end-points for mouse offset */
            ptSelEnd.x = _ptEnd.x - _ptMouseOffset.x;
            ptSelEnd.y = _ptEnd.y - _ptMouseOffset.y;
            ptSelOrigin.x = _ptOrigin.x - _ptMouseOffset.x;
            ptSelOrigin.y = _ptOrigin.y - _ptMouseOffset.y;
            ptSelMouse.x = ptMouse->x - _ptMouseOffset.x;
            ptSelMouse.y = ptMouse->y - _ptMouseOffset.y;
            /* convert end-points to selected frames local coords */
            ptSelEnd = p->transform->InvertPoint(ev, &ptSelEnd);
            ptSelMouse = p->transform->InvertPoint(ev, &ptSelMouse);
            ptSelOrigin = p->transform->InvertPoint(ev, &ptSelOrigin);
            ptSelRef = p->transform->InvertPoint(ev, &_ptRef);
            ODPOINTL ptlSelRef(ptSelRef);

            for (i = 0, pt = ptSelEnd; i < 2; i++, pt = ptSelMouse)
            {
               if (pt.x != ptSelOrigin.x && pt.y != ptSelOrigin.y) {
                  afxScale[0] = (pt.x - ptSelRef.x) /
                                ((ptSelOrigin.x - ptSelRef.x) >> 16);
                  afxScale[1] = (pt.y - ptSelRef.y) /
                                ((ptSelOrigin.y - ptSelRef.y) >> 16);
                  if (afxScale[0] <= 0 || afxScale[1] <= 0)
                     afxScale[0] = afxScale[1] = 0x10000;
                  p->transform->GetMATRIXLF(ev, &mtx);
                  GpiSetModelTransformMatrix(hps, 9, &mtx, TRANSFORM_REPLACE);
                  GpiScale(hps, &mtx, TRANSFORM_REPLACE, afxScale, &ptlSelRef);
                  GpiSetModelTransformMatrix(hps, 9, &mtx, TRANSFORM_PREEMPT);
                  somSelf->ICPDrawPolygons(ev, hps);
               }
            }
         }
         break;
       case kRubberBandSelection:
          GpiSetColor(hps, CLR_BLUE);
          GpiSetLineType(hps, LINETYPE_DOT);
          GpiMove(hps, &ptlOrigin);
          GpiBox(hps, DRO_OUTLINE, &ptlEnd, 0, 0);
          GpiMove(hps, &ptlOrigin);
          GpiBox(hps, DRO_OUTLINE, &ptlMouse, 0, 0);
          break;
       default:
          break;
    }
}

// This method draws the polygons associated with mouse tracking rectangles
SOM_Scope void  SOMLINK ICPDrawPolygons(ITSOContainerPart *somSelf,
                                        Environment *ev, 
                                        HPS hps)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPDrawPolygons");

    GpiSetLineWidth(hps, LINEWIDTH_NORMAL);
    GpiSetColor(hps, CLR_BLUE);
    GpiSetLineType(hps, LINETYPE_DOT);
    for (int i=0; i<_fNumPolygons; i++)
    {
       GpiMove(hps, &_fPolygons[i].aPointl[_fPolygons[i].ulPoints-1]);
       GpiPolyLine(hps, _fPolygons[i].ulPoints, &_fPolygons[i].aPointl[0]);
    }
}

// This method set the facet up to receive all mouse events
SOM_Scope ODBoolean  SOMLINK ICPBeginTrackingMouse(ITSOContainerPart *somSelf,
                                                   Environment *ev, 
                                                   ODFacet* facet)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPBeginTrackingMouse");

    ODBoolean handled = kODFalse;
    ODWindow* tempWindow = facet->AcquireWindow(ev);
    HWND      hwndFrame = tempWindow->GetPlatformWindow(ev);
//    tempWindow->Release(ev);    // DevCon 8 bug, not all Acquire..()'s inc RefCnt 
    HWND      hwnd = WinWindowFromID(hwndFrame, FID_CLIENT);
    ODFrame* frame = facet->GetFrame(ev);

    if (_fSession->GetArbitrator(ev)->RequestFocus(ev, _fMouseFocus, frame)) {
      if (WinSetCapture(HWND_DESKTOP, hwnd)) {
        if (WinLockWindowUpdate(HWND_DESKTOP, hwnd)) {
          _fTrackingFacet = facet;
          _fMouseMode = kTracking;
          handled = kODTrue;
        } else {
          WinSetCapture(HWND_DESKTOP, 0);
          _fSession->GetArbitrator(ev)->RelinquishFocus(ev, _fMouseFocus,
                                                        frame);
        }
      } else {
        _fSession->GetArbitrator(ev)->RelinquishFocus(ev, _fMouseFocus,
                                                      frame);
      }
    }
    return handled;
}

// This method handles the dragging of objects by building the required OPDOC / drag
// objects and managing the drag
SOM_Scope ODBoolean  SOMLINK ICPHandleMouseDrag(ITSOContainerPart *somSelf,
                                                Environment *ev,
                                                ODFacet* container, 
                                                ODFacet* facet, 
                                                ODPoint* mouse, 
                                                ODEventData* event)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPHandleMouseDrag");

    ODPart         * destPart;
    Point            mdOffset = {0,0};
    ODID             frameInfoData;
    ODStorageUnit  * dadSU;
    ODDropResult     dropResult;
    ODFrame        * containingFrame = container->GetFrame(ev);
    Proxy          * p;
    int i;
    if (!_fSelection->Count()) {                 // If nothing is selected...
       return kODTrue;                           //   return 
    }

    ODDragAndDrop  * dad = _fSession->GetDragAndDrop(ev);
    dad->Clear(ev);

    Rect         shapeRect;
    ODULong      numRects      = _fSelection->Count();
    ODULong      numPoints     = numRects * 6 + 2;
    POINTL      *aptl          = new ODPOINTL[numPoints];
    ODTransform *xform         = containingFrame->CreateTransform(ev);
    ODPoint      ptNegMouse    = {-mouse->x, -mouse->y};
    PPOINTL      pptl          = &aptl[0];

    ODTransform *contentTransform = container->AcquireWindowContentTransform(ev, kODNULL);
    pptl->x = (pptl++)->y = 0;

    ODxOrderedCollectionIterator iter(_fSelection);
    for (p = (Proxy*) iter.First();
         iter.IsNotComplete();
         p = (Proxy*) iter.Next())
    {
       ODRect    bbox;
       ODPoint   pt[5];

       ODShape  *shape = p->                            // These three lines
                          frame->                       // obtain the ODRect
                             AcquireFrameShape(ev, kODNULL);  // associated
       shape->GetBoundingBox(ev, &bbox);                // with the selected frame's
//       shape->Release(ev);                              // shape.

       xform->CopyFrom(ev, p->transform);               // Combine xternal transform
       xform->PostCompose(ev, contentTransform);        // of selected frame with
       xform->MoveBy(ev, &ptNegMouse);                  // our window transform less the
                                                        // offset of the mosue.

       pt[0].x = bbox.left;  pt[0].y = bbox.bottom;     // Define a set of five points
       pt[1].x = bbox.left;  pt[1].y = bbox.top;        // defining the frames rectangle.
       pt[2].x = bbox.right; pt[2].y = bbox.top;        // The bottom left corner is
       pt[3].x = bbox.right; pt[3].y = bbox.bottom;     // included twice to close the
       pt[4].x = bbox.left;  pt[4].y = bbox.bottom;     // polygon.

       xform->TransformPoints(ev, pt, 5);               // Transform these points

       for (int j=0; j<5; j++) {
          *(pptl++) = ODPOINTL(pt[j]);                  // Copy the points into the
       }                                                // aptl array.
    } /* end for */                                     // This loop has built an array
                                                        // of all cornrer sof all selected
                                                        // frames.
    for (i = numRects * 5 - 4; i > 0; i -= 5) {
      *(pptl++) = aptl[i];
    }

    pptl->x = pptl->y = 0;
    ODReleaseObject(ev, xform);
//    delete[] arcl;

    DRAGIMAGE dimg;                    /* DRAGIMAGE structure             */

    dimg.cb       = sizeof(DRAGIMAGE); /* Size control block              */
    dimg.cptl     = numPoints;         /* Number of points                */
    dimg.hImage   = (ULONG)aptl;       /* Image handle passed to DrgDrag  */
    dimg.fl       = DRG_POLYGON;       /* Flags passed to DrgDrag         */
    dimg.cxOffset = 0;                 /* Offset of the origin of         */
    dimg.cyOffset = 0;                 /* the image from the pointer      */
                                       /* hotspot                         */

    for (p = (Proxy*) iter.First(); iter.IsNotComplete(); p = (Proxy*) iter.Next())
    {
       dadSU = dad->GetContentStorageUnit(ev);
       _ptMouseStart = ODPoint(*mouse);    

       ODPoint offset(*mouse);             // offset of mouse in window coords

       ODPoint botLeft;      
       p->transform->GetOffset(ev, &botLeft);  // Gets offset out of transformation

       offset = contentTransform->InvertPoint(ev, &offset);
       offset.x -= botLeft.x;
       offset.y -= botLeft.y;

       // offset contains offset of mouse relative to position of frame
       // being dragged in my frame coords (same coords as frame shape).

       dadSU->AddProperty(ev, kPropMouseDownOffset)->AddValue(ev, kODPoint);
       StorageUnitSetValue(dadSU, ev, sizeof(mdOffset), (ODValue) &offset);

       ODMatrix m;
       p->transform->GetMatrix(ev, &m);
       dadSU->AddProperty(ev, kODPropExternalTransform)->AddValue(ev, kODTransform);
       StorageUnitSetValue(dadSU, ev, sizeof(m), (ODValue) &m);

       dadSU->AddProperty(ev, kODPropFrameShape);
       ODShape* tempShape = p->frame->AcquireFrameShape(ev, kODNULL); 
       tempShape->WriteShape(ev, dadSU);
//       tempShape->Release(ev);  // DevCon 8 bug, not all Acquire..()'s inc RefCnt 

       ODDraft* fromDraft = somSelf->GetStorageUnit(ev)->GetDraft(ev);
       ODDraftKey key = fromDraft->BeginClone(ev, dadSU->GetDraft(ev), kODCloneCut);
       ODPart *tempPart = p->frame->AcquirePart(ev);
       tempPart->CloneInto(ev, key, dadSU, p->frame);
//       tempPart->Release(ev);   // DevCon 8 bug, not all Acquire..()'s inc RefCnt 
       fromDraft->EndClone(ev, key);

       _dadSelection = p;
       frameInfoData = p->frame->GetID(ev);

       dadSU->AddProperty(ev, kPropFrameInfo)->AddValue(ev, kODFrameRef);
       StorageUnitSetValue(dadSU, ev, sizeof(ODID), (ODValue) &frameInfoData);
       p->frame->SetDragging(ev, kODTrue);
    }

    ODReleaseObject(ev, contentTransform);

    ODByteArray dragImgBA = CreateByteArrayStruct(&dimg, sizeof(DRAGIMAGE));
    ODByteArray eventBA = CreateByteArrayStruct(&event, sizeof(ODEventData*));

    dropResult = dad->StartDrag(ev, containingFrame, 0, &dragImgBA, &destPart, &eventBA);

    DisposeByteArrayStruct(&dragImgBA); 
    DisposeByteArrayStruct(&eventBA);




    if ((dropResult == kODDropCopy) || (dropResult == kODDropMove)) {
       if (destPart) {
          if (destPart == _fPartWrapper) {           // Our part
             containingFrame->Invalidate(ev, kODNULL, kODNULL); // Redraw
          } else {                                // Another part
             if ( dropResult == kODDropMove ) {

                ODxOrderedCollection* selectionProxies = new ODxOrderedCollection;
                for (p = (Proxy*) iter.First();
                     iter.IsNotComplete();
                     p = (Proxy*) iter.Next())
                {
                   selectionProxies->AddLast(p);
                }

                somSelf->ICPEmptySelection(ev);   // Deselect all selected parts

                ODxOrderedCollectionIterator s(selectionProxies);
                for (p = (Proxy*) s.First();
                     s.IsNotComplete();           // For each frame which
                     p = (Proxy*) s.Next())       // was selected....
                {
                   ODxOrderedCollection* tempFacets = new ODxOrderedCollection;
                   ODFrameFacetIterator* facets = p->frame->CreateFacetIterator(ev);
                   for (ODFacet* f1 = facets->First(ev);
                         facets->IsNotComplete(ev);
                         f1 = facets->Next(ev))
                   {
                      if ( f1->GetFrame(ev) == p->frame ) {  // Is this needed?
                         tempFacets->AddLast(f1);  // Create a list of facets
                      }
                   }
                   delete facets;

                   ODxOrderedCollectionIterator t(tempFacets);
                   for (ODFacet* f2 = (ODFacet*)t.First();
                         t.IsNotComplete();            // For each of those
                         f2 = (ODFacet*)t.Next())      // facets
                   {
                      f2->
                        GetContainingFacet(ev)->      // Remove facet
                           RemoveFacet(ev, f2);
                      ODDeleteObject(f2);             // and delete
                   }
                   delete tempFacets;
                   somSelf->RemoveEmbeddedFrame(ev, p->frame);
                }
                somSelf->GetStorageUnit(ev)->GetDraft(ev)->SetChangedFromPrev(ev);
                containingFrame->Invalidate(ev, NULL, kODNULL);
                somSelf->ICPClipEmbeddedFrames(ev, containingFrame);
                delete selectionProxies;
            }
         }
       }
    }
    for (p = (Proxy*) iter.First();
        iter.IsNotComplete();
        p = (Proxy*) iter.Next())
    {
        p->frame->SetDragging(ev, kODFalse);
    }
    delete[] aptl;
    return kODTrue;
}

SOM_Scope ODBoolean  SOMLINK ICPMoveToFront(ITSOContainerPart *somSelf,
                                            Environment *ev, 
                                            ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPMoveToFront");

    if (_fSelection->Count() == 1) {
       ODxOrderedCollectionIterator i(_fSelection);
       ODxOrderedCollection *tempFacets = new ODxOrderedCollection;
       Proxy* p = (Proxy*) i.First();
       _fEmbeddedFrames->Remove(p->frame);
       _fEmbeddedFrames->AddFirst(p->frame);
       _fContents->Remove(p);
       _fContents->AddFirst(p);
       _fNeedToExternalize = kODTrue;
       ODFrameFacetIterator* facets = p->frame->CreateFacetIterator(ev);

       for (ODFacet* facet = facets->First(ev);
            facets->IsNotComplete(ev);
            facet = facets->Next(ev))
       {
          tempFacets->AddFirst(facet);
       }

       ODxOrderedCollectionIterator iter(tempFacets);
       for (facet = (ODFacet*)iter.First();
            iter.IsNotComplete();
            facet = (ODFacet*)iter.Next())
       {
          facet->GetContainingFacet(ev)->MoveBefore(ev, facet, kODNULL);
       }

       somSelf->ICPClipEmbeddedFrames(ev, frame);
       p->frame->Invalidate(ev, kODNULL, kODNULL);
       somSelf->GetStorageUnit(ev)->GetDraft(ev)->SetChangedFromPrev(ev);
       delete tempFacets;
    }
    return kODTrue;
}

SOM_Scope ODBoolean  SOMLINK ICPMoveToBack(ITSOContainerPart *somSelf,
                                           Environment *ev, 
                                           ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPMoveToBack");

    if (_fSelection->Count() == 1) {
       ODxOrderedCollectionIterator i(_fSelection);
       ODxOrderedCollection *tempFacets = new ODxOrderedCollection;
       Proxy* p = (Proxy*) i.First();
       _fEmbeddedFrames->Remove(p->frame);
       _fEmbeddedFrames->AddLast(p->frame);
       _fContents->Remove(p);
       _fContents->AddLast(p);
       _fNeedToExternalize = kODTrue;
       ODFrameFacetIterator* facets = p->frame->CreateFacetIterator(ev);

       for (ODFacet* facet = facets->First(ev);
            facets->IsNotComplete(ev); 
            facet = facets->Next(ev))
       {
          tempFacets->AddLast(facet);
       }

       ODxOrderedCollectionIterator iter(tempFacets);
       for (facet = (ODFacet*)iter.First();
            iter.IsNotComplete();
            facet = (ODFacet*)iter.Next())
       {
          facet->GetContainingFacet(ev)->MoveBehind(ev, facet, kODNULL);
       }

       p->frame->Invalidate(ev, kODNULL, kODNULL);
       somSelf->ICPClipEmbeddedFrames(ev, frame);
       somSelf->GetStorageUnit(ev)->GetDraft(ev)->SetChangedFromPrev(ev);
       delete tempFacets;
    }
    return kODTrue;
}

SOM_Scope ODBoolean  SOMLINK ICPMoveForward(ITSOContainerPart *somSelf,
                                            Environment *ev, 
                                            ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPMoveForward");

    if (_fSelection->Count() == 1) {
       ODxOrderedCollectionIterator i(_fSelection);
       ODxOrderedCollection *tempFacets = new ODxOrderedCollection;
       Proxy* p = (Proxy*) i.First();
       Proxy* siblingFrameProxy = (Proxy*)_fContents->Before(p);
       if (siblingFrameProxy) {
          _fEmbeddedFrames->Remove(p->frame);
          _fEmbeddedFrames->AddBefore(siblingFrameProxy->frame, p->frame);
          _fContents->Remove(p);
          _fContents->AddBefore(siblingFrameProxy, p);
          _fNeedToExternalize = kODTrue;

          ODFrameFacetIterator* facets = p->frame->CreateFacetIterator(ev);
          for (ODFacet* facet = facets->First(ev);
               facets->IsNotComplete(ev);
               facet = facets->Next(ev))
          {
             tempFacets->AddFirst(facet);
          }

          ODxOrderedCollectionIterator iter(tempFacets);
          ODFrameFacetIterator* siblingFacets = siblingFrameProxy->frame->CreateFacetIterator(ev);
          ODFacet* siblingFacet = siblingFacets->First(ev);

          for (facet = (ODFacet*)iter.First();
               iter.IsNotComplete();
               facet = (ODFacet*)iter.Next())
          {
             facet->GetContainingFacet(ev)->MoveBefore(ev, facet, siblingFacet);
             siblingFacet = facet;
          }

          somSelf->ICPClipEmbeddedFrames(ev, frame);
          p->frame->Invalidate(ev, kODNULL, kODNULL);
          somSelf->GetStorageUnit(ev)->GetDraft(ev)->SetChangedFromPrev(ev);
       }
       delete tempFacets;
    }
    return kODTrue;
}

SOM_Scope ODBoolean  SOMLINK ICPMoveBackward(ITSOContainerPart *somSelf,
                                             Environment *ev, 
                                             ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPMoveBackward");

    if (_fSelection->Count() == 1) {
       ODxOrderedCollectionIterator i(_fSelection);
       ODxOrderedCollection *tempFacets = new ODxOrderedCollection;
       Proxy* p = (Proxy*) i.First();
       Proxy* siblingFrameProxy = (Proxy*)_fContents->After(p);
       if (siblingFrameProxy) {
          _fEmbeddedFrames->Remove(p->frame);
          _fEmbeddedFrames->AddAfter( siblingFrameProxy->frame, p->frame);
          _fContents->Remove(p);
          _fContents->AddAfter(siblingFrameProxy, p);
          _fNeedToExternalize = kODTrue;

          ODFacet *facet;
          ODFrameFacetIterator* facets = p->frame->CreateFacetIterator(ev);
          for (facet = facets->First(ev); facets->IsNotComplete(ev); facet = facets->Next(ev))
          {
             tempFacets->AddLast(facet);
          }

          ODFrameFacetIterator* siblingFacets = siblingFrameProxy->frame->CreateFacetIterator(ev);
          ODFacet* siblingFacet;
          for (facet = siblingFacets->First(ev);
               siblingFacets->IsNotComplete(ev);
               facet = siblingFacets->Next(ev))
          {
             siblingFacet = facet;          // Find last (most behind) facet
          }                                 // of sibling frame.

          ODxOrderedCollectionIterator iter(tempFacets);
          for (facet = (ODFacet*)iter.First();
               iter.IsNotComplete();
               facet = (ODFacet*)iter.Next())
          {
             facet->GetContainingFacet(ev)->MoveBehind(ev, facet, siblingFacet);
             siblingFacet = facet;    // Set just moved facet to the last
          }                           // (most behind) facet.

          p->frame->Invalidate(ev, kODNULL, kODNULL);
          somSelf->ICPClipEmbeddedFrames(ev, frame);
          somSelf->GetStorageUnit(ev)->GetDraft(ev)->SetChangedFromPrev(ev);
       }
       delete tempFacets;
    }
    return(kODTrue);
}

// ------------------------------------------------
//  Method:             ICPDisplayStatusLineText
//
//  Introduced by:      ITSOContainerPart
//
// Displays text on the status line
//
// ------------------------------------------------
// Display text on the status line
SOM_Scope ODBoolean  SOMLINK ICPDisplayStatusLineText(ITSOContainerPart *somSelf,
                                                      Environment *ev, 
                                                      string str, 
                                                      ODFrame* curFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPDisplayStatusLineText");

    if (_fStatusLn) {
       if (! _fHaveStatusLineFocus) {
          if (_fSession->GetArbitrator(ev)->RequestFocus(ev, _fStatusFocus, curFrame)) {
              _fHaveStatusLineFocus = kODTrue; // part has focus
          }
       } /* endif */

       if ( _fHaveStatusLineFocus) {
          _fStatusLn->SetStatusLineText(ev,str,curFrame);
          return kODTrue;
       }
    }
    return kODFalse;
}

// ------------------------------------------------
//  Method:             ICPFreezeEmbeddedFrames
//
//  Introduced by:      ITSOContainerPart
//
// Bundles all embedded frames
// The embedded frames can be selected but not activated
//
// ********** Not currently called... implement or remove.
// If implement...need a thaw method?
// ------------------------------------------------
SOM_Scope void  SOMLINK ICPFreezeEmbeddedFrames(ITSOContainerPart *somSelf,
                                                Environment *ev, 
                                                ODBoolean freeze)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPFreezeEmbeddedFrames");

    ODEmbeddedFramesIterator* iter=somSelf->CreateEmbeddedFramesIterator(ev,kODNULL);

    for (ODFrame* frame = (ODFrame*)iter->First(ev);
         iter->IsNotComplete(ev);
         frame = (ODFrame*)iter->Next(ev))
    {
      frame->SetFrozen(ev, freeze);
    }
}

// ------------------------------------------------
//  Method:             ICPCreateTrackingPolygons
//
//  Introduced by:      ITSOContainerPart
//
// This method creates the polygons to be 
// used during mouse manipulation of selected objects
//
// ********** Check out transformPoints parm????
// ------------------------------------------------

SOM_Scope void  SOMLINK ICPCreateTrackingPolygons(ITSOContainerPart *somSelf,
                                                  Environment *ev, 
                                                  ODBoolean transformPoints)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPCreateTrackingPolygons");

    _fNumPolygons = _fSelection->Count();
    _fPolygons = new POLYGON[_fNumPolygons];

    Proxy* p;
    int    i;

    ODxOrderedCollectionIterator iter(_fSelection);

    for (i = 0, p = (Proxy*) iter.First(); 
         iter.IsNotComplete(); 
         i++, p = (Proxy*) iter.Next())
    {
       ODRect bbox;

       ODShape *frameShape = p->frame->AcquireFrameShape(ev, kODNULL);
       frameShape->GetBoundingBox(ev, &bbox);
//       frameShape->Release(ev);   // DevCon 8 bug, not all Acquire..()'s inc RefCnt 

       _fPolygons[i].ulPoints = 4;
       _fPolygons[i].aPointl = new POINTL[4];
       ODPoint apt[4];
       apt[0].x = bbox.left;  apt[0].y = bbox.bottom;
       apt[1].x = bbox.left;  apt[1].y = bbox.top;
       apt[2].x = bbox.right; apt[2].y = bbox.top;
       apt[3].x = bbox.right; apt[3].y = bbox.bottom;

       for (int j=0; j<4; j++)
       {
         if (transformPoints) {
            apt[j] = p->transform->TransformPoint(ev, &apt[j]);
         }
         _fPolygons[i].aPointl[j] = ODPOINTL(apt[j]);
       }
    }
}

// ------------------------------------------------
//  Method:             ICPRemovePartWindows
//
//  Introduced by:      ITSOContainerPart
//
//     Removes a PartWindow if the passed frame
// ********** Need a description and better
//   implementation.
// ------------------------------------------------
SOM_Scope void  SOMLINK ICPRemovePartWindows(ITSOContainerPart *somSelf, 
                                             Environment *ev, 
                                             ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPRemovePartWindows");

    ODxOrderedCollection *displayFrames = new ODxOrderedCollection;

    ODxOrderedCollectionIterator iter1(_fDisplayFrames);
    for (ODFrame* dispframe = (ODFrame*)iter1.First();
         iter1.IsNotComplete();                           // Copy Ordered collection
         dispframe = (ODFrame*)iter1.Next())              // to avoid side effects
    {                                                     // on iterator
      displayFrames->AddFirst(dispframe);
    }

    ODxOrderedCollectionIterator iter2(displayFrames);
    for (ODFrame* dispFrame = (ODFrame*)iter2.First();
         iter2.IsNotComplete();
         dispFrame = (ODFrame*)iter2.Next())
    {                                                     // If another display frame
      if ((dispFrame != frame) &&                         // is the root of a secondary
         (dispFrame->AcquireContainingFrame(ev) == kODNULL)) { // window, then delete
                                                    // bug in ACF - no inc refcount
         ODWindow* partWindow = dispFrame->AcquireWindow(ev);  // the window.  We should
         partWindow->Acquire(ev);
                                                               // be checking that this
         if ((partWindow) && (!partWindow->IsRootWindow(ev))) {// other frame is attached
            partWindow->CloseAndRemove(ev);                    // to the frame being
         }                                                     // deleted.
      }
    }
    delete displayFrames;                                 // Deletes copy of ordered
                                                          // collection
}

// ------------------------------------------------
//  Method:             ICPFindFacetClip
//
//  Introduced by:      ITSOContainerPart
//
//     Searches the ordered collection of FacetClip
//     structures and returns the strucuture associated
//     with the specified facet if it exists.
// ------------------------------------------------
SOM_Scope FacetClip*  SOMLINK ICPFindFacetClip(ITSOContainerPart *somSelf,
                                               Environment *ev, 
                                               ODFacet* facet)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPFindFacetClip");

    FacetClip* p;

    if (_fSavedFacetClipShapes->Count()) {
       ODxOrderedCollectionIterator i(_fSavedFacetClipShapes);
       for (p = (FacetClip*)i.First();
            i.IsNotComplete();
            p = (FacetClip*)i.Next())
       {
          if (p->facet == facet) {
            return p;
          }
       }
    }
    return kODNULL;
}

// ------------------------------------------------
//  Method:             ICPClipEmbeddedFacets
//
//  Introduced by:      ITSOContainerPart
//
//     This method creates a shape representing 
//     the union of all of the embedded facet's 
//      clipshapes.
// ------------------------------------------------
SOM_Scope void  SOMLINK ICPClipEmbeddedFacets(ITSOContainerPart *somSelf,
                                              Environment *ev, 
                                              ODFacet* facet)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPClipEmbeddedFacets");

    ODTransform* wclipTrans  = facet->GetFrame(ev)->CreateTransform(ev);
    ODShape*     workingClip = (ODShape *)facet->GetPartInfo(ev); // Our facet's PartInfo stores
                                                                  // clipshape
    workingClip->Reset(ev);                                       // Reste our clipshape

    ODTransform* contentTransform;
    contentTransform = facet->                     // Get our content xform
                         AcquireContentTransform(ev, kODNULL);

    ODShape* tempShape = facet->GetFrame(ev)->CreateShape(ev);    // Create temporary shape
    ODRect emptyRect = {0,0,0,0};
    tempShape->SetRectangle(ev,&emptyRect);        // Inits temp shape to 0 shape

    tempShape->Transform(ev, contentTransform);    // now in content coordinates
    workingClip->CopyFrom(ev, tempShape);          // Starts working clip at 0 shape

    ODFacetIterator* facets = facet->
                                CreateFacetIterator(ev, kODChildrenOnly, kODFrontToBack);
    for (ODFacet *embFacet = facets->First(ev);
         facets->IsNotComplete(ev);
         embFacet = facets->Next(ev))              // For each embedded facet...
    {
       Proxy* p = somSelf->                        // Obtain our proxy for the facets frame
                   ICPProxyForFrame(ev, embFacet->GetFrame(ev));


       ODShape* newClipShape =                     // Get copy of frames used shape
                    ODCopyAndRelease(ev,
                                     embFacet->
                                       GetFrame(ev)->
                                         AcquireUsedShape(ev, kODNULL));

      // The next several lines basically creates a union of this facets
      // clip shape with that already gathered...and puts it in terms of
      // our frame's coordinates.

       wclipTrans->CopyFrom(ev, contentTransform); // Copy our content xform...
       wclipTrans->PreCompose(ev, p->transform);   // compose with frames xform
       wclipTrans->Invert(ev);
       workingClip->Transform(ev, wclipTrans);     // now in frame coordinates
       newClipShape->Subtract(ev, workingClip);    // subtract anythin we already have
       workingClip->Union(ev, newClipShape);       // so union works properly.
       wclipTrans->Invert(ev);
       workingClip->Transform(ev, wclipTrans);     // now in window coordinates

       if (p->transformChanged) {
          embFacet->
             ChangeGeometry(ev, 
                            newClipShape, 
                            p->transform, 
                            kODNULL);
          p->transformChanged = kODFalse;
       } else {
          embFacet->
             ChangeGeometry(ev, 
                            newClipShape, 
                            kODNULL, 
                            kODNULL);            // facet now owns shape storage
       }

       FacetClip* facetClip = somSelf->ICPFindFacetClip(ev, embFacet);

       if (facetClip) {
          facetClip->clip->Release(ev);
          _fSavedFacetClipShapes->Remove(facetClip);
          delete facetClip;
       }
    }

    workingClip->Subtract(ev,tempShape);
    tempShape->Release(ev);
    ODTransform* tempTrans = facet->AcquireWindowFrameTransform(ev, kODNULL);
    wclipTrans->CopyFrom(ev,tempTrans);
    tempTrans->Release(ev);
    wclipTrans->Invert(ev);
    workingClip->Transform(ev,wclipTrans);
    delete facets;
    delete wclipTrans;

    somSelf->ICPClipSelectionFromEmbeddedFacets(ev, facet);
}

// ------------------------------------------------
//  Method:             ICPClipEmbeddedFrames
//
//  Introduced by:      ITSOContainerPart
//
//      For the passed frame, clip all embedded
//      facets.  Simply uses the frames facet
//      iterator and calls ICPClipEmbeddedFacets
//      on each facet of this frame.
// ------------------------------------------------

SOM_Scope void  SOMLINK ICPClipEmbeddedFrames(ITSOContainerPart *somSelf,
                                              Environment *ev, 
                                              ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPClipEmbeddedFrames");

    ODFrameFacetIterator* facets = frame->CreateFacetIterator(ev);

    for (ODFacet* facet = facets->First(ev);
         facets->IsNotComplete(ev);
         facet = facets->Next(ev))
    {
       somSelf->ICPClipEmbeddedFacets(ev, facet);
    }
    delete facets;
}

// ------------------------------------------------
//  Method:             ICPMakeEmbeddedFrame
//
//  Introduced by:      ITSOContainerPart
//
//      Creates a new embedded frame in our
//      frame, 
//
// ------------------------------------------------
SOM_Scope ODFrame*  SOMLINK ICPMakeEmbeddedFrame(ITSOContainerPart *somSelf,
                                                 Environment* ev,
                                                 ODFrame*     containingFrame,
                                                 ODShape*     frameShape,
                                                 ODTransform* externalTransform, 
                                                 ODPart*      embedPart,
                                                 ODBoolean    isOverlaid)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPMakeEmbeddedFrame");

    ODShape* newShape = NULL; 
                                  // I can negotiate here      
    if (frameShape) {             // decide what newShape      
      newShape = frameShape;      // should be - for now,      
      newShape->Acquire(ev);      // give it what it asked for 
    } else {                      
      ODRect rect = {0,           // else use same default shape
                     MAKEFIXED(100,0),
                     MAKEFIXED(100,0), 0};
      newShape = containingFrame->CreateShape(ev);
      newShape->SetRectangle(ev, &rect);
    }

    int rc = embedPart->GetRefCount(ev);
    ODFrame* newFrame = somSelf->
                          GetStorageUnit(ev)->
                            GetDraft(ev)->
                              CreateFrame(ev,
                                          (containingFrame->GetStorageUnit(ev)) ? kODFrameObject : kODNonPersistentFrameObject,
                                          containingFrame,
                                          newShape,
                                          (ODCanvas*)kODNULL,
                                          embedPart,
                                          _FrameView,
                                          _PresDefault,
                                          kODFalse,
                                          isOverlaid);

    rc = embedPart->GetRefCount(ev);
    newFrame->
       SetFrameGroup(ev,(ODULong)1);        // Add to our default framegroup
    newFrame->
       ChangeSequenceNumber(ev,             // Assign and increment sequence
                           (ODULong)_fSequence++); // number

    _fEmbeddedFrames->AddFirst(newFrame);   // Add to our list of embedded frames
    newFrame->Acquire(ev);                  //***BJ

    Proxy* p = new Proxy(newFrame,          // Also create a proxy and
                         externalTransform);// add to our proxy list
    _fContents->AddFirst(p);
    newFrame->Acquire(ev);                  //***BJ

    _fNeedToExternalize = kODTrue;
    return newFrame;
}

// ------------------------------------------------
//  Method:             ICPActivatingWindow
//
//  Introduced by:      ITSOContainerPart
//
//
//      Called whenever the document window is
//      activated (by processing WM_ACTIVATE message).
//
//      Ensures that this part retrieves the focus
//      set if it had it when the window was deactivated.
// ------------------------------------------------

SOM_Scope void  SOMLINK ICPActivatingWindow(ITSOContainerPart *somSelf,
                                            Environment *ev, 
                                            ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPActivatingWindow");

    PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);
    if (pInfo->fNeedsActivating) {
      somSelf->ICPActivateFrame(ev, frame);
      pInfo->fNeedsActivating = kODFalse;
    }
}

// ------------------------------------------------
//  Method:             ICPDeActivatingWindow
//
//  Introduced by:      ITSOContainerPart
//
//
//      Called whenever the document window is
//      deactivated (by processing WM_ACTIVATE message).
//
//      Ensures that this part stores an indicator of
//      this frames active state in the frames partinfo.
// ------------------------------------------------

SOM_Scope void  SOMLINK ICPDeActivatingWindow(ITSOContainerPart *somSelf,
                                              Environment *ev, 
                                              ODFrame* frame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPDeActivatingWindow");

    PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);
    if (frame == _fSession->
                   GetArbitrator(ev)->
                     AcquireFocusOwner(ev, _fSelectionFocus)) {
       pInfo->fNeedsActivating = kODTrue;
       if (_fMouseMode != kNormal) {
           somSelf->ICPResetMouseMode(ev);
       }
    } else {
       pInfo->fNeedsActivating = kODFalse;
    }
}

// ------------------------------------------------
//  Method:             ICPHandleRemoteDrop
//
//  Introduced by:      ITSOContainerPart
//
//      Used to handle the drop of a part which was
//      dragged from another frame.
//
// ------------------------------------------------
SOM_Scope void  SOMLINK ICPHandleRemoteDrop(ITSOContainerPart *somSelf, 
                                            Environment *ev, 
                                            ODStorageUnit* su, 
                                            ODFacet* facet, ODPoint* where)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPHandleRemoteDrop");

    ODDraft     * theDraft = somSelf->GetStorageUnit(ev)->GetDraft(ev);
    ODDraft     * fromDraft = su->GetDraft( ev );
    ODDraftKey    key;
    ODID          suID;
    ODShape     * newFrameShape;
    ODTransform * newExternalXForm;
    ODPart      * newPart;
    ODFrame     * frameDroppedIn;
    ODFrame     * newFrame = kODNULL;

    try {
       key = fromDraft->BeginClone(ev, theDraft, kODClonePaste);

       suID = fromDraft->              // Clone the storage unit
                 Clone(ev,             // will eventually be used
                       key,            // to acquire the part.
                       su->GetID(ev), 
                       0, 0); 
       fromDraft->EndClone(ev, key);        // End Clone
    } catch (... ) {
       fromDraft->AbortClone(ev, key);      // Abort clone of error
       throw;
    } /* end catch */

    newPart = theDraft->                 // Gets part associated with SU ID
                AcquirePart(ev,          // or builds creates new part 
                            suID );
    newPart->Acquire(ev); // DevCon 8 bug, not all Acquire..()'s inc RefCnt 

    ODPoint newTransPt = *where;
    if (su->Exists(ev, kPropMouseDownOffset, kODPoint, 0)) {
       ODPoint offset;
       su->Focus(ev, kPropMouseDownOffset, kODPosUndefined,       
                 kODPoint, 0, kODPosFirstSib);       
       StorageUnitGetValue(su,          
                           ev,              // Gets offset of mouse             
                           sizeof(offset),  // as specified in SU...helps       
                           (ODValue) &offset);  // to position content in our frame 
       newTransPt.x -= offset.x;            // Modify point by offset, if
       newTransPt.y -= offset.y;            // it exists.
    }

    newExternalXForm = new ODTransform;     // Identity transform
    newExternalXForm->
       MoveBy(ev,  &newTransPt );           // transform now modified by offset.

    if (su->Exists(ev, kODPropFrameShape, NULL, 0)) {
        su->Focus(ev, kODPropFrameShape, kODPosUndefined, 
                  NULL, 1, kODPosUndefined);
        newFrameShape = facet->
                          CreateShape(ev);
        newFrameShape->ReadShape(ev, su);// Get the frame shape from
                                         // the SU (if it exists)
    } else {
        newFrameShape = kODNULL;         // uses default size {80,80}
    }
    frameDroppedIn = facet->GetFrame(ev);// Get our frame in which 
                                         // this was dropped

    newFrame = somSelf->                                  
                 ICPMakeEmbeddedFrame(ev, frameDroppedIn, // Creates new frame for  
                                      newFrameShape,      // the new part, in our   
                                      newExternalXForm,   // frame with appropriate 
                                      newPart,            // shape and transform.
                                      kODFalse);

    ODxOrderedCollectionIterator aIter(_fDisplayFrames);  // Create a display frame
                                                          // iterator
    for (ODFrame* dispframe = (ODFrame*) aIter.First();
         aIter.IsNotComplete();                           // For each of our
         dispframe = (ODFrame*) aIter.Next())             // display frames
    {
      if (dispframe != frameDroppedIn) {         // If it is NOT the frame in
       ODFrame* newFrame2 = somSelf->
                              ICPMakeEmbeddedFrame(ev, dispframe, // Creates new frame for
                                      newFrameShape,      // the new part, in our   
                                      newExternalXForm,   // frame with appropriate 
                                      newPart,            // shape and transform.
                                      kODFalse);

          newPart->AttachSourceFrame(ev,newFrame2,newFrame);

          ODFrameFacetIterator* facets = dispframe->CreateFacetIterator(ev);
          for (ODFacet* faceti = facets->First(ev); facets->IsNotComplete(ev); faceti = facets->Next(ev))
          {
             ODShape* clip = ODCopyAndRelease(ev, newFrame2->AcquireFrameShape(ev, kODNULL));
             faceti->CreateEmbeddedFacet(ev, newFrame2, clip, newExternalXForm,
                                        kODNULL, kODNULL, kODNULL, kODFrameInFront);
             clip->Release(ev);
          }
          delete facets;

          somSelf->ICPClipEmbeddedFrames(ev,dispframe);
          dispframe->Invalidate(ev, NULL, kODNULL);
      } /* endif */
    } /* endfor */

    newFrame->SetPropagateEvents(ev, kODTrue);

    ODFrameFacetIterator* facets = frameDroppedIn->CreateFacetIterator(ev);
    for (ODFacet* faceti = facets->First(ev); facets->IsNotComplete(ev); faceti = facets->Next(ev))
    {
        ODShape* clip = ODCopyAndRelease(ev,newFrame->AcquireFrameShape(ev,kODNULL));
        faceti->CreateEmbeddedFacet(ev, newFrame, clip, newExternalXForm,
                     kODNULL, kODNULL, kODNULL, kODFrameInFront);
        clip->Release(ev);
    }
    delete facets;
    somSelf->ICPClipEmbeddedFrames(ev, frameDroppedIn);
    frameDroppedIn->Invalidate(ev, NULL, kODNULL);
    newPart->Release(ev);
}

// ------------------------------------------------
//  Method:             ICPHandleLocalDrop
//
//  Introduced by:      ITSOContainerPart
//
//      Used to handle the drop of a part which was
//      dragged from elsewhere within our frame.
//
//      Basically calculates transforms and offsets
//      to correctly modify the embedded parts
//      transform.
// ------------------------------------------------
SOM_Scope void  SOMLINK ICPHandleLocalDrop(ITSOContainerPart *somSelf, 
                                           Environment *ev, 
                                           ODStorageUnit* su, 
                                           ODFacet* facet, ODPoint* where)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPHandleLocalDrop");

    ODPoint   delta;
    ODID      draggedFrameID;

    ODTransform* tempTrans = facet->     
                               AcquireWindowFrameTransform(ev, kODNULL);
    ODPoint             ms = tempTrans->InvertPoint(ev, &_ptMouseStart);
    tempTrans->Release(ev);

    delta.x = where->x - ms.x;
    delta.y = where->y - ms.y;

    su->Focus(ev, kPropFrameInfo, kODPosUndefined,
                  kODFrameRef, 0, kODPosFirstSib);
    StorageUnitGetValue(su, ev, sizeof(ODID), (ODValue) &draggedFrameID);
    Proxy *p = somSelf->ICPProxyForFrameID(ev, draggedFrameID);

    p->transform->MoveBy(ev, &delta); 
    p->transformChanged = kODTrue;    // Used by our ClipEmbedded routines
                                      // to ensure we have the correct
                                      // transforms.
    _fNeedToExternalize = kODTrue;
    somSelf->ICPCreateSelectSizingHandlesShape(ev); 

    somSelf->
      ICPClipEmbeddedFrames(ev,
                            p->
                             frame->      
                               AcquireContainingFrame(ev)); // Passes our frame 
                                                            // to ICPClipEmbeddedFrames

                             // bug in ACF() - no inc refcount
}


// ------------------------------------------------
//  Method:             ICPDrawIconView
//
//  Introduced by:      ITSOContainerPart
//
//      Used to display our frame as a small or
//      large ICON.
//
// ------------------------------------------------
SOM_Scope void  SOMLINK ICPDrawIconView(ITSOContainerPart *somSelf,
                                        Environment *ev, 
                                        ODFacet* facet, 
                                        ODULong size)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPDrawIconView");

    BITMAPINFOHEADER2 bmp; 
    HBITMAP           hbm;
    POINTL            aptl[4];
    ODFrame*          displayFrame = facet->GetFrame(ev);
    ODCanvas*         canvas = facet->GetCanvas(ev);
    ODRect            rect;

    displayFrame->                       // Gets our Used Shape
      AcquireUsedShape(ev, canvas)->
        GetBoundingBox(ev, &rect);

    ODRECTL           frameRect(rect);   // Converts to PMRECTL
                                         // (defined as ODRECTL)
    MATRIXLF     mtx;
    ODTransform* localToGlobal = facet->
                                   AcquireContentTransform(ev, canvas);
    localToGlobal->GetMATRIXLF(ev, &mtx);
    localToGlobal->Release(ev);

    HPS hps = canvas->GetPlatformCanvas(ev, kODGPI)->GetPS(ev, facet);
    GpiSetModelTransformMatrix(hps, 9, &mtx, TRANSFORM_REPLACE);
    hbm = GpiLoadBitmap(hps, _hmod, IDB_BITMAP, size, size);  // Load Bitmap

    bmp.cbFix = sizeof(bmp);
    if (GpiQueryBitmapInfoHeader(hbm, &bmp) == GPI_OK) {
      aptl[0].x = frameRect.xLeft;    // Our frame corners
      aptl[0].y = frameRect.yBottom;
      aptl[1].x = frameRect.xRight;
      aptl[1].y = frameRect.yTop;
      aptl[2].x = 0;                 // Target ICON corners
      aptl[2].y = 0;
      aptl[3].x = bmp.cx;
      aptl[3].y = bmp.cy;

      if (GpiWCBitBlt(hps, hbm, 4, aptl, ROP_SRCCOPY, BBO_IGNORE) == GPI_ERROR) { 
          DosBeep(700UL, 500UL);
      }
    }
    canvas->GetPlatformCanvas(ev, kODGPI)->ReleasePS(ev, facet);
}


// ------------------------------------------------
//  Method:             ICPInvertDragHilite
//
//  Introduced by:      ITSOContainerPart
//
//      Highlights our clipshape when something
//      is being dragged over us.  Helps to ensure
//      the drop occurs within our part.
//
//      Creates polygon from our clipshape, then
//      creates a path along the contour of that
//      polygon.
//
//      Used as a toggle... once highlighted, calling
//      this method again removes the highlighting.
//
// ------------------------------------------------
SOM_Scope void  SOMLINK ICPInvertDragHilite(ITSOContainerPart *somSelf, 
                                            Environment *ev, 
                                            ODFacet* facet)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPInvertDragHilite");

  struct ODContour
  {
    ODSLong nVertices;
    ODPoint vertex[1];
  };

  struct ODPolygonData
  {
    ODSLong   nContours;
    ODContour firstContour;
  };
 
  HPS          hps;
  CFocusWindow f(ev,                    // Set up drawing environment
                 facet,                 // Our facet
                 (ODShape*)kODNULL,     // InvalidShape (Use null for this)
                 &hps,                  // Pointer for returning HPS
                 (HWND*)kODNULL,        // The window handle (Use null)
                 CFocusWindow::DragPS); // A Drag PS (Enum item)

  ODPolygon    poly;

  ODShape*     clipShape = facet->       // Get our current clipshape
                             AcquireAggregateClipShape(ev, kODNULL);
  poly    = clipShape->CopyPolygon(ev);  // Create equivalent ODPolygon
  clipShape->Release(ev);                // Release clipshape

  ODContour*     pContour;
  ODPolygonData* pPolygonData = (ODPolygonData*)poly._buffer;
  PPOLYGON       ppoly        = new POLYGON[pPolygonData->nContours];

  pContour = &pPolygonData->firstContour;
  for (int i = 0; i < pPolygonData->nContours; i++) {
    ppoly[i].ulPoints = pContour->nVertices;
    ppoly[i].aPointl  = new POINTL[pContour->nVertices];

    for (int j = 0; j < pContour->nVertices; j++) {
      ppoly[i].aPointl[j] = ODPOINTL(pContour->vertex[j]);
      ppoly[i].aPointl[j].x -= 1;
      ppoly[i].aPointl[j].y -= 1;
    }
    pContour = (ODContour*)((char*)pContour + sizeof(ODSLong) +
               pContour->nVertices * sizeof(ODPoint));
  }

  GpiBeginPath(hps, 1);            // Path to sketch out polygon
    for (i = 0; i < pPolygonData->nContours; i++) {
      GpiMove(hps,
              &ppoly[i].aPointl[ppoly[i].ulPoints - 1]);
      GpiPolyLine(hps,
                  ppoly[i].ulPoints, ppoly[i].aPointl);
    }
  GpiEndPath(hps);

  GpiSetMix(hps, FM_INVERT);       // Set up for easy removal of hilite
  GpiSetLineWidthGeom(hps, 8);     // Set width of hilite
  GpiStrokePath(hps, 1, 0);        // Draw hilite

  for (i = 0; i < pPolygonData->nContours; i++) {  // Release storage
    delete[] ppoly[i].aPointl;
    delete[] ppoly;
  }
  DisposeByteArrayStruct(&poly);                   // Release storage
}

// ------------------------------------------------
//  Method:             ICPHandleButton1MotionStart
//
//  Introduced by:      ITSOContainerPart
//
//    Determines if one is dragging a selected corner
//    or just doing a rubber band selection process.
// ------------------------------------------------
SOM_Scope ODBoolean  SOMLINK ICPHandleButton1MotionStart(ITSOContainerPart *somSelf,
                                                         Environment *ev,
                                                         ODFacet* facet, 
                                                         ODPoint* where, 
                                                         ODEventData* event)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPHandleButton1MotionStart");

    ODBoolean handled = kODFalse;
    ODFrame* frame = facet->GetFrame(ev);
    PartInfoRec* pInfo = (PartInfoRec*)frame->GetPartInfo(ev);
    if (pInfo->fIsActive && _fMouseMode == kNormal) {
       ODPoint mouse = *where;

       ODTransform* xform = facet->AcquireWindowContentTransform(ev, kODNULL);
       mouse = xform->InvertPoint(ev, &mouse);
       xform->Release(ev);

       if (_fSelection->Count() == 1 &&      // If one frame selected
           _fSelectSizingHandlesShape->      // and mouse is in sizing handle
                       ContainsPoint(ev, &mouse)) {
          Proxy* p = (Proxy*)_fSelection->First();  // Proxy for selected frame
          ODRect bbox;
          ODPoint ptCorners[4], ptSelMouse, ptSelCenter;

          ODShape* frameShape = p->frame->AcquireFrameShape(ev, kODNULL);
          frameShape->GetBoundingBox(ev, &bbox);
//          frameShape->Release(ev);   // DevCon 8 bug, not all Acquire..()'s inc RefCnt 

          ptSelMouse = p->transform->InvertPoint(ev, &mouse); // in selected frame coords
          ptSelCenter.x = (bbox.left + bbox.right) / 2;
          ptSelCenter.y = (bbox.top + bbox.bottom) / 2;
          ptCorners[0].x = bbox.left;  ptCorners[0].y = bbox.bottom;
          ptCorners[1].x = bbox.left;  ptCorners[1].y = bbox.top;
          ptCorners[2].x = bbox.right; ptCorners[2].y = bbox.top;
          ptCorners[3].x = bbox.right; ptCorners[3].y = bbox.bottom;
          p->transform->TransformPoints(ev, ptCorners, 4);

          if (ptSelMouse.x < ptSelCenter.x && ptSelMouse.y < ptSelCenter.y) {
             _fTrackingMode = kBotLeftResize;
             _ptRef = ptCorners[2];
             _ptMouseOffset.x = ptCorners[0].x - mouse.x;
             _ptMouseOffset.y = ptCorners[0].y - mouse.y;
          } else {
            if (ptSelMouse.x < ptSelCenter.x && ptSelMouse.y > ptSelCenter.y) {
               _fTrackingMode = kTopLeftResize;
               _ptRef = ptCorners[3];
               _ptMouseOffset.x = ptCorners[1].x - mouse.x;
               _ptMouseOffset.y = ptCorners[1].y - mouse.y;
            } else {
              if (ptSelMouse.x > ptSelCenter.x && ptSelMouse.y > ptSelCenter.y) {
                _fTrackingMode = kTopRightResize;
                _ptRef = ptCorners[0];
                _ptMouseOffset.x = ptCorners[2].x - mouse.x;
                _ptMouseOffset.y = ptCorners[2].y - mouse.y;
              } else {
                if (ptSelMouse.x > ptSelCenter.x && ptSelMouse.y < ptSelCenter.y) {
                  _fTrackingMode = kBotRightResize;
                  _ptRef = ptCorners[1];
                  _ptMouseOffset.x = ptCorners[3].x - mouse.x;
                  _ptMouseOffset.y = ptCorners[3].y - mouse.y;
                }
              }
            }
          }
          _ptEnd = _ptOrigin = mouse;
          somSelf->ICPCreateTrackingPolygons(ev, kODFalse);
       } else {
          _fTrackingMode = kRubberBandSelection;
          _ptEnd = _ptOrigin = mouse;

          if (_fSelection->Count()) {
             facet->GetFrame(ev)->Invalidate(ev, kODNULL, kODNULL);
             somSelf->ICPEmptySelection(ev);
             somSelf->ICPClipSelectionFromEmbeddedFrames(ev, facet->GetFrame(ev));
             /*
              * Force updating now since we're about to lock window updating
              */
             ODWindow * tempWindow = facet->AcquireWindow(ev);
             WinUpdateWindow(tempWindow->GetPlatformWindow(ev));
//             tempWindow->Release(ev);   // DevCon 8 bug, not all Acquire..()'s inc RefCnt 
          }
       }

       somSelf->ICPBeginTrackingMouse(ev, facet);
       _ptEnd = mouse;
       handled = kODTrue;
    }
    return handled;
}

// ------------------------------------------------
//  Method:             ICPSetFrameSelected
//
//  Introduced by:      ITSOContainerPart
//
//      Iterates through all facets of the frame and
//      sets each ones Selected state to TRUE and
//      tells each facet to notify its part of a
//      change in highlight state.
// ------------------------------------------------
SOM_Scope void  SOMLINK ICPSetFrameSelected(ITSOContainerPart *somSelf,
                                            Environment *ev, 
                                            ODFrame* frame, 
                                            ODBoolean isSelected)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPSetFrameSelected");
 
    ODFrameFacetIterator* facets = frame->CreateFacetIterator(ev);

    for (ODFacet* facet = facets->First(ev);
         facets->IsNotComplete(ev);
         facet = facets->Next(ev))
    {
      facet->SetSelected(ev, isSelected);
      facet->ChangeHighlight(ev, isSelected ? kODFullHighlight : kODNoHighlight);
    }
    delete facets;
}

// ------------------------------------------------
//  Method:             ICPCreateSelectSizingHandlesShape
//
//  Introduced by:      ITSOContainerPart
//
//      Loops through all selected parts and builds a
//      shape which is the union of all sizing handles.
// ------------------------------------------------
SOM_Scope void  SOMLINK ICPCreateSelectSizingHandlesShape(ITSOContainerPart *somSelf,
                                                          Environment *ev)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPCreateSelectSizingHandlesShape");

    _fSelectSizingHandlesShape->Reset(ev);
    if (_fSelection->Count()) {
       ODRect   bbox;
       ODPoint  ptCorners[4];
       Proxy*   p;
       ODxOrderedCollectionIterator iter(_fSelection);
       ODShape *frameShape, *tempShape;
       tempShape = _fSelectSizingHandlesShape->Copy(ev);

       for (p = (Proxy*)iter.First();
            iter.IsNotComplete();
            p = (Proxy*) iter.Next())
       {
          frameShape = ODCopyAndRelease(ev, p->frame->AcquireFrameShape(ev, kODNULL));

          frameShape->GetBoundingBox(ev, &bbox);
          frameShape->Release(ev);
          ptCorners[0].x = bbox.left;   ptCorners[0].y = bbox.bottom;
          ptCorners[1].x = bbox.left;   ptCorners[1].y = bbox.top;
          ptCorners[2].x = bbox.right;  ptCorners[2].y = bbox.top;
          ptCorners[3].x = bbox.right;  ptCorners[3].y = bbox.bottom;
          p->transform->TransformPoints(ev, ptCorners, 4);

          for (int i=0; i<4; i++) {
             bbox.left   = ptCorners[i].x - kSelectionHandleWidth / 2;
             bbox.top    = ptCorners[i].y + kSelectionHandleWidth / 2;
             bbox.right  = ptCorners[i].x + kSelectionHandleWidth / 2;
             bbox.bottom = ptCorners[i].y - kSelectionHandleWidth / 2;
             tempShape->SetRectangle(ev, &bbox);
             _fSelectSizingHandlesShape->Union(ev, tempShape);
          }
       }
       tempShape->Release(ev);
       _fSelectionFrame = ((Proxy*)_fSelection->First())->frame->AcquireContainingFrame(ev);
                // bug in ACF() - no inc refcount
    }
}

SOM_Scope void  SOMLINK ICPDuplicateAndAttachEmbedded(ITSOContainerPart *somSelf, 
                                                      Environment *ev, 
                                                      ODFrame* frame, 
                                                      ODFrame* sourceFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPDuplicateAndAttachEmbedded");

    ODxOrderedCollection* embeddedFrames = new ODxOrderedCollection;
    ODEmbeddedFramesIterator* iter=somSelf->CreateEmbeddedFramesIterator(ev,kODNULL);

                                               // For each or our embedded frames
                                               // if they are contained in the source
                                               // frame, create equivalents in new frame.
    for (ODFrame* embeddedFrame = (ODFrame*)iter->First(ev);
        iter->IsNotComplete(ev);
        embeddedFrame = (ODFrame*)iter->Next(ev))
    {
       if (embeddedFrame->AcquireContainingFrame(ev) == sourceFrame) {
                                      // bug in ACF() - no increment refcount
           ODPart*      embeddedPart = embeddedFrame->AcquirePart(ev);
           embeddedPart->Acquire(ev); // DevCon 8 bug, not all Acquire..()'s inc RefCnt 
           ODShape*     frameShape   = embeddedFrame->AcquireFrameShape(ev, kODNULL);
           ODShape*     newShape     = frameShape->Copy(ev);
           ODTransform* newXForm     = somSelf->ICPProxyForFrame(ev, embeddedFrame)->transform->Copy(ev);
           ODFrame*     newFrame     = somSelf->ICPMakeEmbeddedFrame(ev,
                                                                     frame,
                                                                     newShape,
                                                                     newXForm,
                                                                     embeddedPart,
                                                                     embeddedFrame->IsOverlaid(ev));
           embeddedPart->AttachSourceFrame(ev, newFrame, embeddedFrame);
           frameShape->Release(ev); frameShape = kODNULL;
           newShape->Release(ev); newShape = kODNULL;
       }
    }
    delete embeddedFrames;
}

SOM_Scope void  SOMLINK ICPRemoveEmbeddedFrames(ITSOContainerPart *somSelf, 
                                                Environment *ev, 
                                                ODFrame* displayFrame)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPRemoveEmbeddedFrames");

    
    ODxOrderedCollection* embeddedFrames = new ODxOrderedCollection;
    {
      ODEmbeddedFramesIterator* iter=somSelf->
                                       CreateEmbeddedFramesIterator(ev,kODNULL);
      for (ODFrame* embeddedFrame = (ODFrame*)iter->First(ev);
           iter->IsNotComplete(ev);
           embeddedFrame = (ODFrame*)iter->Next(ev))  // Creating a copy of our collection
      {                                               // of embedded frames
        embeddedFrames->AddLast(embeddedFrame);
      }
    }
    {
      ODxOrderedCollectionIterator iter(embeddedFrames);
      for (ODFrame* embeddedFrame = (ODFrame*)iter.First();
          iter.IsNotComplete();                   // For each embedded frame...
          embeddedFrame = (ODFrame*)iter.Next())
      {
         if (embeddedFrame->AcquireContainingFrame(ev) == displayFrame) {
 // bug in ACF() - no inc refcount
             somSelf->RemoveEmbeddedFrame(ev, embeddedFrame);
         }
      }
    }
    delete embeddedFrames;
}

SOM_Scope void  SOMLINK ICPResize(ITSOContainerPart *somSelf,
                                  Environment *ev, 
                                  ODFrame* frame, 
                                  ODULong ulMode)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPResize");

    ODRect   frameRect;
    LONG     cxFrame;
    LONG     cyFrame;
    ODShape* requestedShape;

    frame->AcquireFrameShape(ev, kODNULL)->GetBoundingBox(ev, &frameRect);
    cxFrame = frameRect.right - frameRect.left;
    cyFrame = frameRect.top   - frameRect.bottom;

    if (cxFrame < MAKEFIXED(ITSOCNTR_WIDTH, 0)) {
      frameRect.right = frameRect.left + MAKEFIXED(ITSOCNTR_WIDTH, 0);
    }
    if (cyFrame < MAKEFIXED(ITSOCNTR_HEIGHT, 0)) {
      frameRect.top = frameRect.bottom + MAKEFIXED(ITSOCNTR_HEIGHT, 0);
    }
    requestedShape = frame->CreateShape(ev);
    requestedShape->SetRectangle(ev, &frameRect);
    frame->RequestFrameShape(ev, requestedShape, kODNULL);
    requestedShape->Release(ev);
}

SOM_Scope void  SOMLINK ICPDrawFrameView(ITSOContainerPart *somSelf, 
                                         Environment *ev, ODFacet* facet,
                                         ODShape* invalidShape)
{
    ITSOContainerPartData *somThis = ITSOContainerPartGetData(somSelf);
    ITSOContainerPartMethodDebug("ITSOContainerPart","ICPDrawFrameView");

    ODRect rect;

    ODShape* shape = facet->GetFrame(ev)->
                        AcquireFrameShape(ev, facet->GetCanvas(ev));
    shape->GetBoundingBox(ev, &rect);
    shape->Release(ev);

    ODRECTL frameRect(rect);
    ODShape *clipShape = (ODShape*)facet->GetPartInfo(ev);
    HPS hpsDraw;
    HRGN hrgnClip, hrgnOld, hrgnAggClip;
    CFocus f(ev, facet, invalidShape, &hpsDraw, &hrgnAggClip);
    hrgnClip = GpiCreateRegion(hpsDraw, 0, 0);
    GpiCombineRegion(hpsDraw, hrgnClip, hrgnAggClip, 0, CRGN_COPY);

    if (clipShape) {
       ODTransform* xform = facet->AcquireContentTransform(ev, kODNULL);
       ODShape* tempShape = clipShape->Copy(ev);
       tempShape->Transform(ev, xform);
       GpiCombineRegion(hpsDraw, hrgnClip, hrgnClip, tempShape->GetRegion(ev), CRGN_DIFF);
       xform->Release(ev);
       tempShape->Release(ev);
    }
    GpiSetClipRegion(hpsDraw, hrgnClip, &hrgnOld);
    GpiSetColor(hpsDraw, CLR_WHITE);
    POINTL ptl = {0, 0};
    GpiMove(hpsDraw, &ptl);
    ptl.x = frameRect.xRight;
    ptl.y = frameRect.yTop;
    GpiBox(hpsDraw, DRO_FILL, &ptl, 0, 0);
    GpiSetLineType(hpsDraw, LINETYPE_DOT);
    GpiSetColor(hpsDraw, CLR_BLUE);
    GpiBox(hpsDraw, DRO_OUTLINE, &ptl, 0, 0);

    // calculate size of text string, and centering coordinates
    POINTL aptl[TXTBOX_COUNT];
    GpiQueryTextBox(hpsDraw, strlen("ITSOContainerPart"), "ITSOContainerPart", TXTBOX_COUNT, aptl);

    ptl.x = (frameRect.xRight - aptl[TXTBOX_TOPRIGHT].x) / 2;
    ptl.y = (frameRect.yTop - aptl[TXTBOX_TOPRIGHT].y) / 2;

    // draw text
    GpiSetColor(hpsDraw, CLR_BLUE);
    GpiCharStringAt(hpsDraw, &ptl, strlen("ITSOContainerPart"), "ITSOContainerPart");

    // highlite the selected frame.
    if (facet->GetFrame(ev) == _fSelectionFrame) {  // drawing active frame - highlight selection
       GpiSetClipRegion(hpsDraw, hrgnAggClip, &hrgnOld);
       ODFacetIterator* facets = facet->CreateFacetIterator(ev, kODChildrenOnly, kODBackToFront);
       for (ODFacet* embFacet = facets->First(ev);
            facets->IsNotComplete(ev);
            embFacet = facets->Next(ev))
       {
          embFacet->Update(ev, kODNULL, kODNULL);
       }

       somSelf->ICPHighlightSelection(ev, facet);
    }

    GpiSetClipRegion(hpsDraw, 0, &hrgnOld);
    GpiDestroyRegion(hpsDraw, hrgnClip);
    GpiDestroyRegion(hpsDraw, hrgnAggClip);
}
//===================================================
//   Metaclass Methods
//===================================================

/*
    Return part handler name (ITSOContainerPart)
*/
SOM_Scope ISOString  SOMLINK clsGetODPartHandlerName(M_ITSOContainerPart *somSelf, 
                                                     Environment *ev)
{
    /* M_ITSOContainerPartData *somThis = M_ITSOContainerPartGetData(somSelf); */
    M_ITSOContainerPartMethodDebug("M_ITSOContainerPart","clsGetODPartHandlerName");

    string handlerName = kPartHandlerName;
    return ((ISOString) handlerName);
}

/*
    Return part handler display name (ITSOContainer Part Kind Kind).
*/
SOM_Scope string  SOMLINK clsGetODPartHandlerDisplayName(M_ITSOContainerPart *somSelf, 
                                                         Environment *ev)
{
    /* M_ITSOContainerPartData *somThis = M_ITSOContainerPartGetData(somSelf); */
    M_ITSOContainerPartMethodDebug("M_ITSOContainerPart","clsGetODPartHandlerDisplayName");

    string displayName = kKindDisplayName;
    return (displayName);
}

/*
    Set the part kinds that this part handles.
*/
SOM_Scope _IDL_SEQUENCE_PartKindInfo  SOMLINK clsGetODPartKinds(M_ITSOContainerPart *somSelf, 
                                                                Environment *ev)
{
    /* M_ITSOContainerPartData *somThis = M_ITSOContainerPartGetData(somSelf); */
    M_ITSOContainerPartMethodDebug("M_ITSOContainerPart","clsGetODPartKinds");

    _IDL_SEQUENCE_PartKindInfo kindInfo;

    // Create structure PartKindInfo  and allocate memory for variable
    PartKindInfo *info = (PartKindInfo *) SOMMalloc (sizeof (PartKindInfo));
    info->partKindName = (ISOString) SOMMalloc (strlen (kKind) + 1);
    info->partKindDisplayName = (string) SOMMalloc (strlen (kKindDisplayName) + 1);
    info->filenameFilters =  (string) SOMMalloc (strlen ("") + 1);
    info->filenameTypes =  (string) SOMMalloc (strlen ("") + 1);
    info->categories =  (string) SOMMalloc (strlen (kCategory) + 1);
    info->objectID =  (string) SOMMalloc (strlen ("") + 1);

    // Copy the information into the structure
    strcpy (info->partKindName , kKind);
    strcpy (info->partKindDisplayName, kKindDisplayName);
    strcpy (info->filenameFilters, "");
    strcpy (info->filenameTypes, "");
    strcpy (info->categories, kCategory);
    strcpy (info->objectID, "");

    kindInfo._maximum = 1;
    kindInfo._length = 1;
    kindInfo._buffer = info;

    return (kindInfo);
}

/*
    Return an OLE 2 class id (a part must have an OLE id in order to be
    embedded within an OLE document).
*/
SOM_Scope string  SOMLINK clsGetOLE2ClassId(M_ITSOContainerPart *somSelf, 
                                            Environment *ev)
{
    /* M_ITSOContainerPartData *somThis = M_ITSOContainerPartGetData(somSelf); */
    M_ITSOContainerPartMethodDebug("M_ITSOContainerPart","clsGetOLE2ClassId");

    string classID = "";
    return (classID);
}

/*
    For OLE, sets the name of the file that will be used by an OLE document
    as the icon for the part.
*/
SOM_Scope string  SOMLINK clsGetWindowsIconFileName(M_ITSOContainerPart *somSelf, 
                                                    Environment *ev)
{
    /* M_ITSOContainerPartData *somThis = M_ITSOContainerPartGetData(somSelf); */
    M_ITSOContainerPartMethodDebug("M_ITSOContainerPart","clsGetWindowsIconFileName");

    string fileName = "";
    return (fileName);
}

// This procedure determines whether drag item can be accepted
// Needs to be more work done to exclude non-opendoc objects
ODBoolean dmSelectRMF(PDRAGITEM pDragItem, PSZ pszSelectedRMF)
{

   if ( DrgVerifyRMF( pDragItem, "DRM_OS2FILE", "DRF_OPENDOCDOCUMENT" ))
      strcpy( pszSelectedRMF, "<DRM_OS2FILE,DRF_OPENDOCDOCUMENT>" );
   if ( DrgVerifyRMF( pDragItem, "DRM_SHAREDMEM", "DRF_OPENDOCDOCUMENT" ))
      strcpy( pszSelectedRMF, "<DRM_SHAREDMEM,DRF_OPENDOCDOCUMENT>" );
   else strcpy( pszSelectedRMF, "<DRM_OS2FILE,DRF_UNKNOWN>" );

   return( TRUE );
}

