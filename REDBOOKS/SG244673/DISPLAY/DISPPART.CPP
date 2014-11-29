
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
#ifndef SOM_Module_DisplayPart_Source
#define SOM_Module_DisplayPart_Source
#endif
#define DisplayPart_Class_Source

#define VARIABLE_MACROS
#include "DispPart.xih"

#define INCL_BASE
#define INCL_GPI
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINSTDDRAG
#define INCL_WINWINDOWMGR
#define INCL_WINPOINTERS
#define INCL_WINMENUS

#define INCL_ODARBITRATOR
#define INCL_ODCANVAS
#define INCL_ODDRAFT
#define INCL_ODERRORS
#define INCL_ODFACET
#define INCL_ODFRAME
#define INCL_ODMENUBAR
#define INCL_ODSHAPE
#define INCL_ODSESSION
#define INCL_ODSTORAGEUNIT
#define INCL_ODSTORAGEUNITVIEW
#define INCL_ODTRANSFORM
#define INCL_ODWINDOW
#define INCL_ODWINDOWSTATE
#define INCL_ODCLIPBOARD
#define INCL_ODFOCUSSET
#define INCL_ODLINK
#define INCL_ODLINKSOURCE
#define INCL_ODLINKSPEC

#include <os2.h>
#include <od.h>
#include <stdio.h>
#include <focuslib.h>

#include "od2iclui.h"

const ODType kKind = "DisplayPartKind";
const ODType kKindDisplayName = "DisplayPart";
const ODType kPartHandlerName = "DisplayPart";
const ODType kCategory = "Text";


SOM_Scope void  SOMLINK somInit(DisplayPart *somSelf)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","somInit");

    DisplayPart_parent_SimplePart_somInit(somSelf);
    _dispString=(char *) SOMMalloc(MAXLEN);              // Allocate memory for 
                                                         // display string
}

SOM_Scope void  SOMLINK somUninit(DisplayPart *somSelf)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","somUninit");

    if(_dispString) SOMFree(_dispString);                // Free memory for
                                                         // display string     
    DisplayPart_parent_SimplePart_somUninit(somSelf);
}

SOM_Scope void  SOMLINK InitPart(DisplayPart *somSelf,  
                                 Environment *ev,
                                 ODStorageUnit* storageUnit, 
                                 ODPart *partWrapper)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","InitPart");

    DisplayPart_parent_SimplePart_InitPart(somSelf, ev, storageUnit, partWrapper);

    somSelf->DPCommonInitPart(ev);
}                                                        // Call our common init routine

SOM_Scope void  SOMLINK InitPartFromStorage(DisplayPart *somSelf, 
                                            Environment *ev,
                                            ODStorageUnit* storageUnit, 
                                            ODPart *partWrapper)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","InitPartFromStorage");

    DisplayPart_parent_SimplePart_InitPartFromStorage(somSelf, ev, storageUnit, partWrapper);

    DPCommonInitPart (somSelf, ev);                           // Call our common init routine

    storageUnit->Focus(ev, kODPropContents, kODPosUndefined,  // Focus on our property and
                       kODISOStr, 0, kODPosUndefined);        // value....
    StorageUnitGetValue (storageUnit, ev, storageUnit->GetSize(ev),  // and read stored
                         (ODValue) _dispString);                     // string into dispString
    somSelf->DPReadLinksFromStorage(ev);
}

SOM_Scope void  SOMLINK Externalize(DisplayPart *somSelf,  
                                    Environment *ev)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","Externalize");

    ODStorageUnit* su = somSelf->GetStorageUnit (ev);      // Obtain our storage unit

    if (! su->Exists (ev, kODPropContents, kKind, 0)) {    // Ensure our part kind's property/value
        su->AddProperty (ev, kODPropContents)->AddValue (ev, kKind); // exists
    }
  
    if (!su->Exists (ev, kODPropContents, kODISOStr, 0)) { // Ensure our data value type exists
        su->AddProperty (ev, kODPropContents)->AddValue (ev, kODISOStr);
    }
    su->Focus(ev, kODPropContents, kODPosUndefined,        // Focus on our property and
              kODISOStr, 0, kODPosUndefined);              // value....
    StorageUnitSetValue (su,ev,
                         strlen(_dispString)+1,            // Writes our data to storage
                         (ODValue) _dispString);
  
    somSelf->DPWriteLinksToStorage(ev);
    DisplayPart_parent_SimplePart_Externalize(somSelf, ev);
    return;
}

SOM_Scope ODBoolean  SOMLINK HandleEvent(DisplayPart *somSelf,  Environment *ev,
                                         ODEventData* event,
                                         ODFrame* frame, ODFacet* facet,
                                         ODEventInfo* eventInfo)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","HandleEvent");

    ODBoolean handled = kODFalse;

    switch( event->msg)                                  // Main event handling switch
    {
       case WM_BUTTON1DOWN:
       case WM_BUTTON2DOWN:
       case WM_BUTTON1CLICK:
            if (!facet->AcquireWindow(ev)->IsActive(ev)) // If winodw is not active,
               facet->AcquireWindow(ev)->Select(ev);     // make it active.
            else
               somSelf->ActivateFrame(ev, frame);        // Otherwise activate frame
            handled=kODTrue;
            break;
       case WM_COMMAND:
            switch (SHORT1FROMMP(event->mp1))
            {
             case EDIT_PASTEAS:
                  somSelf->DPDoPasteAs(ev, frame);       
                  frame->Invalidate(ev, kODNULL, kODNULL);
                  handled=kODTrue;
                  break;
            }
       default :
          break;
    }

    if (handled) {
       return handled;
    } else {
    return DisplayPart_parent_SimplePart_HandleEvent(somSelf, ev, event, 
                                                     frame, facet, eventInfo);
    }
}

SOM_Scope void  SOMLINK FacetAdded(DisplayPart *somSelf,  Environment *ev,
                                   ODFacet* facet)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","FacetAdded");

    _fFacet=facet;
    ODFrame * frame = facet->GetFrame(ev);
    ODRect newRect = {0, MAKEFIXED(30,0),            // Define rectangle we
                      MAKEFIXED(160,0), 0};          // will request for frame shape

    ODShape* newShape = frame->CreateShape(ev);      // Create shape
    newShape->InitShape(ev);
    newShape->SetRectangle(ev, &newRect);            // Set to rectangle defined above
    frame->RequestFrameShape(ev, newShape, kODNULL); // Request new frame shape
    newShape->Release(ev);

    HWND hwnd = (HWND)facet->GetFacetHWND(ev);       // Obtain window for facet
    _hwndCanvas = ICLWindow(hwnd);                   // Create ICLUI window for this facet
    somSelf->GeometryChanged(ev, facet, kODTrue, kODTrue ); // Usded to size the ICLUI window
                                                     // to our frame shape
}


SOM_Scope void  SOMLINK GeometryChanged(DisplayPart *somSelf,  Environment *ev,
                                        ODFacet* facet, ODBoolean clipShapeChanged,
                                        ODBoolean externalTransformChanged)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","GeometryChanged");

    ODRect rect;
    if( _hwndCanvas )
    {
       MATRIXLF mtx;
       ODTransform* transform = facet->AcquireFrameTransform(ev, kODNULL);

       if (transform->GetType(ev) < kODLinearXform ) // No shear, rotate, perspective
       {
          ODShape *frameShape = facet->GetFrame(ev)->AcquireFrameShape(ev,kODNULL);
          frameShape->GetBoundingBox(ev, &rect);
          ODPoint botLeft = {rect.left, rect.bottom}, topRight = {rect.right, rect.top};
          botLeft = transform->TransformPoint(ev, &botLeft);
          topRight = transform->TransformPoint(ev, &topRight);

          WinShowWindow(_hwndCanvas, FALSE);

          WinSetWindowPos( _hwndCanvas, 0,
                          FIXED2LONG(botLeft.x),
                          FIXED2LONG(botLeft.y),
                          FIXED2LONG(topRight.x - botLeft.x) + 1,
                          FIXED2LONG(topRight.y - botLeft.y) + 1,
                          SWP_MOVE | SWP_SIZE);

        ODShape* tmpShape = facet->AcquireAggregateClipShape(ev,kODNULL);
        ODShape* clipShape = tmpShape->Copy(ev);       // This section of code, takes
        tmpShape->Release(ev);                         // the clipshape, transforms it
        clipShape->Transform(ev, transform);           // to be in frame window coordinates,
        ODTransform* offset = new ODTransform;         // which also implies it has been scaled.
        botLeft.x = -botLeft.x;                        // We then use a pure offset transform to
        botLeft.y = -botLeft.y;                        // put it in terms of our ICLUI window
        offset->SetOffset(ev, &botLeft);               // coordinates (but still scaled).
        clipShape->Transform(ev, offset);

        WinSetClipRegion(_hwndCanvas, clipShape->GetRegion(ev));
//        offset->Release(ev);
//        transform->Release(ev);
//        clipShape->Release(ev);
//        frameShape->Release(ev);   // Bug in DevCon8 - Acquire doesn't always inc refcnt
       }
    }
}

SOM_Scope void  SOMLINK FocusAcquired(DisplayPart *somSelf,  Environment *ev,
                                      ODTypeToken focus, ODFrame* ownerFrame)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","FocusAcquired");

    if (focus==_fMenuFocus) {          // Display our menu bar when
        _fMenuBar->Display(ev);        // when we receive menu focus
    }
}


SOM_Scope void  SOMLINK Draw(DisplayPart *somSelf,  Environment *ev,
                             ODFacet* facet, ODShape* invalidShape)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","Draw");

    if((_hwndCanvas) && (_dispString)) {
       ICLSetOutput(_hwndCanvas,((char *)_dispString));
    }
    WinShowWindow(_hwndCanvas, TRUE);
}

SOM_Scope void  SOMLINK LinkUpdated(DisplayPart *somSelf,  Environment *ev,
                                    ODLink* updatedLink, ODUpdateID change)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","LinkUpdated");

    ODLinkKey   linkKey;
    ODLink    * link = updatedLink;

    if (link->Lock(ev, 0, &linkKey)) {          // Obtain lock for link
       try {
         ODStorageUnit* linkContentSU = link->  // Access link's SU
                                          GetContentStorageUnit(ev, linkKey);
     

                                                // Focus and retrieve data
         if (linkContentSU->Exists(ev, kODPropContents, kODISOStr, 0)) {
             linkContentSU->Focus(ev, kODPropContents, kODPosUndefined, 
                                  kODISOStr, 0, kODPosUndefined);
             StorageUnitGetValue(linkContentSU, ev, 
                                 linkContentSU->GetSize(ev), 
                                 (ODValue) _dispString);
         }

         if(_fFacet) {                          // Invalidate facet to update screen
            _fFacet->Invalidate( ev, kODNULL, kODNULL );
         }

         link->Unlock(ev, linkKey);             // Unlock
       } catch(...) {
         link->Unlock(ev, linkKey);
       } 
    } else {
      // Could include logic here such as a retry
    } 
}

SOM_Scope void  SOMLINK AdjustMenus(DisplayPart *somSelf,  Environment *ev,
                                    ODFrame* frame)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","AdjustMenus");

    DisplayPart_parent_SimplePart_AdjustMenus(somSelf, ev, frame);

    _fMenuBar->                                            // Set us up to allow
       EnableMenuItem(ev,IDMS_EDIT,EDIT_PASTEAS,kODTrue);  // pasting of links.

}

SOM_Scope void  SOMLINK DPCommonInitPart(DisplayPart *somSelf, 
                                          Environment *ev)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","DPCommonInitPart");

    ODStorageUnit* su = somSelf->GetStorageUnit(ev);       // Obtain our storage unit
    _fSession         = su->GetSession(ev);                // Set up instance variables
                                                           // with helpful values like session,
    _fMenuBar         = _fSession->                        // our menu bar,
                           GetWindowState(ev)->
                              CopyBaseMenuBar(ev);         // tokenized focus strings
    _fMenuFocus       = _fSession->Tokenize(ev, kODMenuFocus);
    _fClipboardFocus  = _fSession->Tokenize(ev, kODClipboardFocus);
    _fLinkPtr         = kODNULL;                           // Initially no link

    strcpy(_dispString,"DisplayPart");                     // Default value of display string

    if (! su->Exists (ev, kODPropContents, kKind, 0))      // Ensure storage units are have
        su->                                               // correct properties and
          AddProperty (ev, kODPropContents)->              // values.
             AddValue (ev, kKind);
    if (! su->Exists(ev, kODPropContents, kODISOStr, 0))
        su->
          AddProperty (ev, kODPropContents)->
             AddValue (ev, kODISOStr);
    return;
}

SOM_Scope ODBoolean  SOMLINK DPDoPasteAs(DisplayPart *somSelf,  
                                         Environment *ev,
                                         ODFrame* frame)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","DPDoPasteAs");

    ODBoolean       canPasteLink = kODTrue;
    ODBoolean       canEmbed     = kODFalse;
    ODBoolean       result       = kODTrue;
    ODTypeToken     viewType;
    ODPasteAsResult resultPasteAs;

    ODClipboard*   clipboard = _fSession->GetClipboard( ev );
    ODStorageUnit* clipContentSU = clipboard->GetContentStorageUnit( ev );

    result = clipboard->ShowPasteAsDialog(ev, canPasteLink,
                                          canEmbed,
                                          _fFacet,
                                          viewType,
                                          &resultPasteAs);
    if(result == kODFalse)  //if the user chooses the "CANCEL" button
       return result;

    if (resultPasteAs.pasteLinkSetting == kODTrue){ //if the user chooses the "PASTELINK" button
        somSelf->DPPasteLinkFromClipBoard( ev, frame );
    } 
    return result;
}

SOM_Scope void  SOMLINK DPPasteLinkFromClipBoard(DisplayPart *somSelf,
                                                 Environment *ev,
                                                 ODFrame* frame)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","DPPasteLinkFromClipBoard");

    ODBoolean      result = kODTrue;
    ODArbitrator * arbitrator;
    try {
       arbitrator     = _fSession->GetArbitrator( ev );
       arbitrator->RequestFocus( ev, _fClipboardFocus, frame );
    } catch(...) {
      return;
    }

    ODStorageUnit * clipContentSU;
    ODClipboard   * clipboard = _fSession->GetClipboard(ev);
    ODDraft       * myDraft = somSelf->GetStorageUnit(ev)->GetDraft(ev);
    try {
      clipContentSU = clipboard->GetContentStorageUnit(ev);
    } catch(...) {
      arbitrator->RelinquishFocus( ev, _fClipboardFocus, frame );
      return;
    }
    if (! clipContentSU->Exists(ev, (ODPropertyName) kODPropLinkSpec, kODLinkSpec, 0)){
      arbitrator->RelinquishFocus( ev, _fClipboardFocus, frame );
      return;
    }
    ODLinkSpec* linkSpec = myDraft->
                             CreateLinkSpec(ev,
                                            (ODPart *)somSelf,
                                            (ODByteArray*)kODNULL);
    try {
      clipContentSU->Focus(ev,
                          (ODPropertyName)kODPropLinkSpec,
                          kODPosSame,
                          kODLinkSpec,
                          0,
                          kODPosUndefined);
      linkSpec->ReadLinkSpec(ev, clipContentSU);
    } catch (... ) {
      result = kODFalse;
      delete linkSpec;
      arbitrator->RelinquishFocus( ev, _fClipboardFocus, frame );
      return;
    }
    try {
      _fLinkPtr = myDraft->AcquireLink(ev,0,linkSpec);

      if(_fLinkPtr != kODNULL) {
         _fLinkPtr->RegisterDependent(ev, somSelf, 0);
      }
    } catch(...) {
      result = kODFalse;
    }
    delete linkSpec;
    arbitrator->RelinquishFocus( ev, _fClipboardFocus, frame );
}

SOM_Scope void  SOMLINK DPReadLinksFromStorage(DisplayPart *somSelf, 
                                               Environment *ev)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","DPReadLinksFromStorage");

    ODStorageUnit *su = somSelf->GetStorageUnit(ev);
    ODStorageUnitRef suRef;

    _fLinkPtr=kODNULL;
    if (su->Exists(ev, kODPropLink, kODStrongStorageUnitRef,0)) {
       su->Focus(ev, kODPropLink, kODPosUndefined, kODStrongStorageUnitRef, 0, kODPosUndefined);
       StorageUnitGetValue(su,ev,sizeof(ODStorageUnitRef),&suRef);
       if(su->IsValidStorageUnitRef(ev,suRef)) {
          ODID linkID = su->GetIDFromStorageUnitRef(ev, suRef);
          _fLinkPtr = su->GetDraft(ev)->AcquireLink(ev,linkID,kODNULL);
          _fLinkPtr->RegisterDependent(ev,
                                       somSelf,
                                       kODUnknownUpdate);
       }
    }
}

SOM_Scope void  SOMLINK DPWriteLinksToStorage(DisplayPart *somSelf, 
                                              Environment *ev)
{
    DisplayPartData *somThis = DisplayPartGetData(somSelf);
    DisplayPartMethodDebug("DisplayPart","DPWriteLinksToStorage");

    ODStorageUnit *su = somSelf->GetStorageUnit(ev);
    ODStorageUnitRef suRef;

    if (_fLinkPtr != kODNULL) {
       if (! su->Exists(ev, kODPropLink, kODStrongStorageUnitRef, 0)) {
           su->AddProperty(ev, kODPropLink);
           su->AddValue(ev, kODStrongStorageUnitRef);
       } else {
           su->Focus(ev,kODPropLink,kODPosUndefined,kODStrongStorageUnitRef,0,kODPosUndefined);
       }
       su->GetStrongStorageUnitRef(ev, _fLinkPtr->GetStorageUnit(ev)->GetID(ev), suRef);
       StorageUnitSetValue(su, ev, sizeof(ODStorageUnitRef), (ODValue) &suRef);
    } else {
       if (su->Exists(ev, kODPropLink, kODStrongStorageUnitRef, 0)) {
           su->Focus(ev,kODPropLink,kODPosUndefined,kODNULL,0,kODPosUndefined);
           su->Remove(ev);
       }
    }
}

// =======================================================
// Standard class methods required for part registration
// =======================================================

SOM_Scope ISOString  SOMLINK clsGetODPartHandlerName(M_DisplayPart *somSelf,
                                                     Environment *ev)
{
    /* M_DisplayPartData *somThis = M_DisplayPartGetData(somSelf); */
    M_DisplayPartMethodDebug("M_DisplayPart","clsGetODPartHandlerName");

    string handlerName = kPartHandlerName;
    return ((ISOString) handlerName);
}

SOM_Scope string  SOMLINK clsGetODPartHandlerDisplayName(M_DisplayPart *somSelf,
                                                         Environment *ev)
{
    /* M_DisplayPartData *somThis = M_DisplayPartGetData(somSelf); */
    M_DisplayPartMethodDebug("M_DisplayPart","clsGetODPartHandlerDisplayName");

    string displayName = kKindDisplayName;
    return (displayName);
}

SOM_Scope _IDL_SEQUENCE_PartKindInfo  SOMLINK clsGetODPartKinds(M_DisplayPart *somSelf,
                                                                             Environment *ev)
{
    /* M_DisplayPartData *somThis = M_DisplayPartGetData(somSelf); */
    M_DisplayPartMethodDebug("M_DisplayPart","clsGetODPartKinds");

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

SOM_Scope string  SOMLINK clsGetOLE2ClassId(M_DisplayPart *somSelf,
                                                         Environment *ev)
{
    /* M_DisplayPartData *somThis = M_DisplayPartGetData(somSelf); */
    M_DisplayPartMethodDebug("M_DisplayPart","clsGetOLE2ClassId");

    string classID = "";
    return (classID);
}

SOM_Scope string  SOMLINK clsGetWindowsIconFileName(M_DisplayPart *somSelf,
                                                                 Environment *ev)
{
    /* M_DisplayPartData *somThis = M_DisplayPartGetData(somSelf); */
    M_DisplayPartMethodDebug("M_DisplayPart","clsGetWindowsIconFileName");

    string fileName = "";
    return (fileName);
}

