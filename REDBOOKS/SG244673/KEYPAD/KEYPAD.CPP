
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
#ifndef SOM_Module_keypad_Source
#define SOM_Module_keypad_Source
#endif
#define KeyPadPart_Class_Source

#define INCL_BASE
#define INCL_WIN
#define INCL_GPI
#define INCL_WINDIALOGS         /* Window Dialog Mgr Functions  */
#define INCL_WINPOINTERS        /* Window Pointer Functions     */
#define INCL_GPIREGIONS

#define INCL_ODAPI
#define INCL_ODARBITRATOR
#define INCL_ODCANVAS
#define INCL_ODCLIPBOARD
#define INCL_ODDRAFT
#define INCL_ODFACET
#define INCL_ODFOCUSSET
#define INCL_ODFRAME
#define INCL_ODLINK
#define INCL_ODLINKSOURCE
#define INCL_ODLINKSPEC
#define INCL_ODMENUBAR
#define INCL_OSA                                                             
#define INCL_ODOSA                                                           
#define INCL_ODSESSION
#define INCL_ODSHAPE
#define INCL_ODSTORAGE
#define INCL_ODSTORAGEUNIT
#define INCL_ODTRANSFORM
#define INCL_ODTYPELIST
#define INCL_ODUI
#define INCL_ODWINDOW
#define INCL_ODWINDOWSTATE
#define INCL_ODINFO

#define VARIABLE_MACROS            /* Forces SOM to generate _xxx      */
                                   /* macros for access to inst. vars. */
#include <os2.h>
#include <focuslib.h>
// #include "common.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <builtin.h>
#include <string.h>

#include "KeyPad.xih"                    
#include <kpnotebk.xh>
#include "KeyStuff.h"                     
#include "KPOSA.h"                      
#include "kpnoterc.h"                   

int SOMLINK myOutCharRoutine(char c)
{
   int ch;
    if((ch = fputc( c, stream))==EOF)
      return 0;
    else
      return 1;
};
//-------------------------------------------------------------------------
// Methods for KeyPadPart
//-------------------------------------------------------------------------

SOM_Scope void  SOMLINK somInit(KeyPadPart *somSelf)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","somInit");

    KeyPadPart_parent_SimplePart_somInit(somSelf);

#ifdef  TRACE_ON_BY_DEFAULT
    if (!SOM_TraceLevel) {
       if (( stream = fopen("somtrace.log", "w"))== NULL )
          {
              DosBeep(500,1000);
              DosBeep(1000,200);
              DosBeep(500,200);
          }
       SavedSOMOutCharRoutine=SOMOutCharRoutine;
       SOMOutCharRoutine = (somTD_SOMOutCharRoutine *)  myOutCharRoutine;
       SOM_TraceLevel=1;
    }
#endif
    _fPartWrapper   = kODNULL;

}

// ------------------------------------------------
//  Method:             somUninit
//
//  Introduced by:      SOMObject
//
//    Called on SOMObject destruction
//    Free up any memory we still own
//    on the heap.
// ------------------------------------------------

SOM_Scope void  SOMLINK somUninit(KeyPadPart *somSelf)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","somUninit");

    for (int i=0;i<12 ;i++ ) {                    // Free memory allocated
        if (_KPButtonSeq[i].butText) {            // to hold button text
           SOMFree(_KPButtonSeq[i].butText);
        }
    }

    if (_DisplayString) {
       SOMFree(_DisplayString);                   // Free memory allocated
    }                                             // to hold display text

    if (SOM_TraceLevel && stream ) {              // If SOM_Trace is still
       fclose(stream);                            // active, stop and close
       stream=NULL;                               // file.
       SOMOutCharRoutine=SavedSOMOutCharRoutine;
       SOM_TraceLevel=0;
    }
    KeyPadPart_parent_SimplePart_somUninit(somSelf);
}

// ------------------------------------------------
//  Method:             InitPart
//
//  Introduced by:      ODPart
//
//    Called when part is first instantiated--
//      Could be during part registration or
//      if specified as the root part for the
//      DocShell.
//
//    Not called when part created
//      from template.
// ------------------------------------------------

SOM_Scope void  SOMLINK InitPart(KeyPadPart    *somSelf,
                                 Environment   *ev,
                                 ODStorageUnit *storageUnit,
                                 ODPart* PartWrapper
                                 )
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","InitPart");

    KeyPadPart_parent_SimplePart_InitPart(somSelf, ev,
                                          storageUnit,PartWrapper);

    somSelf->KPCommonInit(ev, PartWrapper);          // Common Init stuff
    somSelf->KPInitButtonInfo(ev);                   // Init KeyPad button
                                                     // structures.
}

// ------------------------------------------------
//  Method:             InitPartFromStorage
//
//  Introduced by:      ODPart
//
//    Called when part is first instantiated from
//      a template or saved document.
//
//    Either this method or InitPart is called,
//      but not both for any one instance.
// ------------------------------------------------

SOM_Scope void  SOMLINK InitPartFromStorage(KeyPadPart    *somSelf,
                                            Environment   *ev,
                                            ODStorageUnit *storageUnit,
                                            ODPart        *PartWrapper)
{
       KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
       KeyPadPartMethodDebug("KeyPadPart","InitPartFromStorage");

       KeyPadPart_parent_SimplePart_InitPartFromStorage(somSelf, ev,
                                                        storageUnit,
                                                        PartWrapper);

       somSelf->KPCommonInit(ev, PartWrapper);          // Common init stuff
       somSelf->KPReadDataFromStorage(ev);              // Read KP data
       somSelf->KPReadLinksFromStorage(ev);             // Read Link info
}

// ------------------------------------------------
//  Method:             Externalize
//
//  Introduced by:      ODPersistentObject
//
//    Called when a part should write itself
//      to persistent storage.
//
//    Do we need to check isDirty here?
// ------------------------------------------------

SOM_Scope void  SOMLINK Externalize(KeyPadPart *somSelf,  Environment *ev)
{
       KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
       KeyPadPartMethodDebug("KeyPadPart","Externalize");

       somSelf->KPInitStorageUnit(ev);        // Ensure SU is initialized
       somSelf->KPWriteDataToStorage(ev);     // Write KP Data
       somSelf->KPWriteLinksToStorage(ev);    // Write Link info

       KeyPadPart_parent_SimplePart_Externalize(somSelf, ev);
}

// ------------------------------------------------
//  Method:             CloneInto
//
//  Introduced by:      ODPart
//
//    Called to clone a parts storage unit
//    from one document to another.  For instance,
//    when dropping a template into a full document,
//    we want to copy the contents of the templates
//    storage unit into the storage unit provided
//    in the new document for our part.
// ------------------------------------------------

SOM_Scope void  SOMLINK CloneInto(KeyPadPart *somSelf,  Environment *ev,
                                  ODDraftKey key, ODStorageUnit* toSU,
                                  ODFrame* scope)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","CloneInto");

    somSelf->Externalize(ev);

    ODStorageUnit*  su = somSelf->GetStorageUnit(ev);
    ODID scopeFrameID = kODNULL;
    if (scope != kODNULL)
      scopeFrameID = scope->GetStorageUnit(ev)->GetID(ev);
    su->CloneInto(ev, key, toSU, scopeFrameID);
    return;
}

// ------------------------------------------------
//  Method:             AdjustMenus
//
//  Introduced by:      ODPart
//
//    Called when ever our part has menu
//     focus and the user clicks on the
//     menu bar...
//
// ------------------------------------------------

SOM_Scope void  SOMLINK AdjustMenus(KeyPadPart *somSelf,  Environment *ev,
                                    ODFrame* frame)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","AdjustMenus");

    KeyPadPart_parent_SimplePart_AdjustMenus(somSelf, ev, frame);

    _fMenuBar->EnableMenuItem(ev,IDMS_VIEW, VIEW_PROPERTIES, kODTrue);

    if (!_fLinkSrcPtr) {
       _fMenuBar->                                            // Set us up to allow
          EnableMenuItem(ev,IDMS_EDIT,EDIT_COPY,kODTrue);     // pasting of links.
    } /* endif */

    if ((!_fLinkPtr) && somSelf->KPIsLinkInClipboard(ev,frame)) {
       _fMenuBar->                                            // Set us up to allow
          EnableMenuItem(ev,IDMS_EDIT,EDIT_PASTEAS,kODTrue);  // pasting of links.
    } /* endif */

    if (_fLinkSrcPtr || _fLinkPtr) {
       _fMenuBar->
          EnableMenuItem(ev,IDMS_EDIT,EDIT_BREAKLINK,kODTrue);
    } /* endif */
}

// ------------------------------------------------
//  Method:             DisplayFrameAdded
//
//  Introduced by:      ODPart
//
//    Called when a frame is added for our part.
//    Should add this frame to our list of display frames.
//    Also do any frame specific initialization, for instance,
//    check, adjust and do any calculations dependent on the
//    frame shape.  We should also add any partinfo to the the
//    frame.
//
// ------------------------------------------------

SOM_Scope void  SOMLINK DisplayFrameAdded(KeyPadPart *somSelf,
                                           Environment *ev, ODFrame* frame)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","DisplayFrameAdded");

    _fFrame = frame;                              // Store frame ref.
    somSelf->                                     // Perform logic
       FrameShapeChanged(ev,frame);               // when frame added

    KeyPadPart_parent_SimplePart_DisplayFrameAdded(somSelf, ev,
                                                   frame);
}

// ------------------------------------------------
//  Method:             DisplayFrameConnected
//
//  Introduced by:      ODPart
//
//    Called when an already existing frame is connected to
//    our part.  For instance, when restoring a document, this
//    method may be called instead of DisplayFrameAdded.
//
//    Should add this frame to our list of display frames.
//    Also do any frame specific initialization, for instance,
//    check, adjust and do any calculations dependent on the
//    frame shape.  We should also add any partinfo to the the
//    frame.
//
// ------------------------------------------------
SOM_Scope void  SOMLINK DisplayFrameConnected(KeyPadPart *somSelf,
                                               Environment *ev,
                                              ODFrame* frame)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","DisplayFrameConnected");

    _fFrame = frame;                               // Store frame ref.
    somSelf->FrameShapeChanged(ev,frame);          // Perform logic
                                                   // when frame added
    KeyPadPart_parent_SimplePart_DisplayFrameConnected(somSelf,
                                                       ev, frame);
}

// ------------------------------------------------
//  Method:             CreateLink
//
//  Introduced by:      ODPart
//
//    Called when a link is being formed with this
//    part as a source.
//
// ------------------------------------------------

SOM_Scope ODLinkSource*  SOMLINK CreateLink(KeyPadPart *somSelf,
                                             Environment *ev,
                                            ODByteArray* data)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","CreateLink");

    if (!_fIsLinkSource) {                  // If part not already
                                            // a link soruce.
        _fLinkSrcPtr = somSelf->
                         GetStorageUnit(ev)-> // Create the ODLinkSource Object
                           GetDraft(ev)->
                             CreateLinkSource(ev, somSelf);

        _fIsLinkSource=kODTrue;                // For easy checking
        somSelf->KPWriteLinkData(ev, _fFrame, 0); // Update link now

        if(_fFrame != kODNULL) {               // Our part changes appearance
           _fFrame->                           // when linked.
              Invalidate( ev, kODNULL, kODNULL );
        }
        _fFrame->ChangeLinkStatus(ev,kODInLinkSource);
        _fSession->GetClipboard( ev )->Clear(ev);
    }
    return _fLinkSrcPtr;
}

// ------------------------------------------------
//  Method:             Draw
//
//  Introduced by:      ODPart
//
//    Called when our part must render itself (or
//    some part of itself).
//
// ------------------------------------------------

SOM_Scope void  SOMLINK Draw(KeyPadPart *somSelf,  Environment *ev,
                             ODFacet* facet, ODShape* invalidShape)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","Draw");


    HWND hwndFacet = facet->GetFacetHWND(ev);    //Get our HWND

    somSelf->KPDrawButtons(ev);

    ULONG style(WinQueryWindowULong(hwndFacet,QWL_STYLE));
    WinSetWindowULong(hwndFacet, QWL_STYLE,      // Sets style of window to clip children
                     style| WS_CLIPCHILDREN);
                                                 // This causes us not to paint over
                                                 // our pushbutton controls and thus
                                                 // reduces flicker when we draw
    HPS       hpsDraw;
    CFocus f(ev, facet, invalidShape, &hpsDraw); // Set up drawing environment

    ODRect    rect;
    ODShape  *frameShapeRef=_fFrame->            // Gets ODRect equiv
                               AcquireFrameShape(ev, kODNULL); // of frameshape
    frameShapeRef->GetBoundingBox(ev, &rect);
    frameShapeRef->Release(ev);

    RECTL frameRect = { FIXED2LONG(rect.left),
                        FIXED2LONG(rect.bottom),
                        FIXED2LONG(rect.right),
                        FIXED2LONG(rect.top)
                      };
    somSelf->KPDrawFrame(ev,hpsDraw,&frameRect); // Draws frame
    somSelf->KPDrawDisplay(ev,hpsDraw);          // Draws display area
    somSelf->KPDrawTextToDisp(ev,hpsDraw);

    WinSetWindowULong(hwndFacet,QWL_STYLE,style);// reset PS style
}

// ------------------------------------------------
//  Method:             FacetAdded
//
//  Introduced by:      ODPart
//
//    Called when a facet has been added for our part.
//    We should add this facet to our list of facets,
//    and initialize any drawing specific data associated
//    with the facet.
//
// ------------------------------------------------

SOM_Scope void  SOMLINK FacetAdded(KeyPadPart *somSelf,  Environment *ev,
                                   ODFacet* facet)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","FacetAdded");

    KeyPadPart_parent_SimplePart_FacetAdded(somSelf, ev, facet);

    _fFacet=facet;

    somSelf->KPCreateButtons(ev,facet->GetFacetHWND(ev));
    somSelf->GeometryChanged(ev,facet,kODTrue,kODTrue);
}

// ------------------------------------------------
//  Method:             FacetRemoved
//
//  Introduced by:      ODPart
//
//    Called when our facet is being removed.  Should
//    free any resources allocated for our facet.
//
// ------------------------------------------------

SOM_Scope void  SOMLINK FacetRemoved(KeyPadPart *somSelf,  Environment *ev,
                                     ODFacet* facet)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","FacetRemoved");

    for (int i=0;i<12 ;i++ ) {                       // Destroy Windows
        if (_KPButtonSeq[i].hwndPB) {
            WinDestroyWindow(_KPButtonSeq[i].hwndPB);
            _KPButtonSeq[i].hwndPB=NULL;
        }
    }
    _fFacet=kODNULL;
    KeyPadPart_parent_SimplePart_FacetRemoved(somSelf, ev, facet);
}

// ------------------------------------------------
//  Method:             GeometryChanged
//
//  Introduced by:      ODPart
//
//    Called when our clip shape or external transform
//    has changed.  Perform calculations specific to
//    the value of these entities.
//
// ------------------------------------------------

SOM_Scope void  SOMLINK GeometryChanged(KeyPadPart *somSelf,
                                        Environment *ev, ODFacet* facet,
                                        ODBoolean clipShapeChanged,
                                        ODBoolean externalTransformChanged
                                        )
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","GeometryChanged");

    ODRect   rect;
    ODTransform * transform = facet->             // Obtain Frame Transfrom 
                                AcquireFrameTransform(ev,kODNULL); //for future calculations 
    ODShape *clipShapeRef = facet->
                              AcquireAggregateClipShape(ev,kODNULL);  // Obtain copy of
    ODShape *clipShape = clipShapeRef->Copy(ev);            // aggregate clip shape
    clipShapeRef->Release(ev);
    clipShape->Transform(ev, transform);          // Apply transform

    HWND hwndFacet = facet->GetFacetHWND(ev);
    WinSetClipRegion(hwndFacet,
                     clipShape->GetRegion(ev));   // New PM call...
                                                  // Ensures our child windows
                                                  // clip to our clip shape (region)
    delete clipShape;                             // No longer needed

    ODShape *frameShapeRef = facet->
                              GetFrame(ev)->
                               AcquireFrameShape(ev,kODNULL);
    frameShapeRef ->
       GetBoundingBox(ev, &rect);                 // rect is in 'fixed' OD coordinates
                                                  // The integer form of these is
                                                  // what we will use when
                                                  // using GPI to draw our part since the
                                                  // transform to PM Device coordinates
                                                  // will be applied to the PS.
                                                  // FIXED2LONG applied within
                                                  // KPCalcDisplayRect() will
                                                  // provide us with what we need.
    frameShapeRef->Release(ev);
    somSelf->KPCalcKPButLayout(ev,transform, &rect);// Based on rect and transform 
                                                  // calculate position of buttons.
    transform->Release(ev);
    facet->Invalidate(ev, kODNULL, kODNULL);
}

// ------------------------------------------------
//  Method:             FrameShapeChanged
//
//  Introduced by:      ODPart
//
//    Called when our frame shape has been changed.
//    Perform calculations required based on our
//    frame shape.
//
// ------------------------------------------------

SOM_Scope void  SOMLINK FrameShapeChanged(KeyPadPart *somSelf,
                                          Environment *ev,
                                          ODFrame* frame)
{

    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","FrameShapeChanged");

    ODRect     rect;

    somSelf-> KPResize(ev,  frame);               // Resize the KetPad
    ODShape *frameShapeRef = frame->
                              AcquireFrameShape(ev,kODNULL);
                                                  // frame shape
    frameShapeRef->
              GetBoundingBox(ev, &rect);          // rect is in 'fixed ' OD coordinates
                                                  // The integer form of these is what we will use when
                                                  // using GPI to draw our part since the
                                                  // transform to PM Device coordinates
                                                  // will be applied to the PS.
                                                  // FIXED2LONG applied within
                                                  // CalcDiplayRect will
                                                  // provide us with what we need.

    frameShapeRef->Release(ev);
    somSelf->KPCalcDisplayRect(ev,&rect);         // Calculate area for keypad display
                                                  // based on rect
}

// ------------------------------------------------
//  Method:             FocusAcquired
//
//  Introduced by:      ODPart
//
//    Called when our part obtains any of the foci
//    such as Selection Focus, Key Focus, Menu Focus, etc.
//
// ------------------------------------------------
SOM_Scope void  SOMLINK FocusAcquired(KeyPadPart *somSelf,  Environment *ev,
                                      ODTypeToken focus, ODFrame* ownerFrame)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","FocusAcquired");

    if (focus==_fMenuFocus) {
        _fMenuBar->Display(ev);
    }

    if (focus==_fSelectionFocus) {
        for (int i=0;i<12 ;i++ )
            WinEnableWindow(_KPButtonSeq[i].hwndPB,TRUE);
    }
}

// ------------------------------------------------
//  Method:             FocusLost
//
//  Introduced by:      ODPart
//
//    Called when a focus is unexpectedly removed.
//
// ------------------------------------------------

SOM_Scope void  SOMLINK FocusLost(KeyPadPart *somSelf,  Environment *ev,
                                  ODTypeToken focus, ODFrame* ownerFrame)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","FocusLost");

    if (focus==_fSelectionFocus) {
        _fAlreadyActive   = kODFalse;
        for (int i=0;i<12 ;i++ ) {
            WinEnableWindow(_KPButtonSeq[i].hwndPB,FALSE);
        }
    }
    KeyPadPart_parent_SimplePart_FocusLost(somSelf, ev, focus,
                                          ownerFrame);
}

// ------------------------------------------------
//  Method:             HandleEvent
//
//  Introduced by:      ODPart
//
//    Called to notify our part of UI events.
//
// ------------------------------------------------

SOM_Scope ODBoolean  SOMLINK HandleEvent(KeyPadPart *somSelf,
                                         Environment *ev,
                                         ODEventData* event,
                                         ODFrame* frame,
                                         ODFacet* facet,
                                         ODEventInfo* eventInfo
                                         )
{
    ODBoolean handled;

    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","HandleEvent");

    handled = kODFalse;

    switch (event->msg) {
         case WM_CHAR:
              somSelf->KPHandleKeyBoard(ev,event->mp1,event->mp2);
              handled=kODTrue;
              break;
         case WM_COMMAND:
              handled = somSelf->KPHandleCommand(ev,event,frame,facet);
              break;
         case WM_BUTTON1CLICK:
              handled=_fAlreadyActive;
              _fAlreadyActive= kODTrue;
              break;
         case WM_CONTEXTMENU:
              if (!_fAlreadyActive)
                  somSelf->ActivateFrame(ev, frame);
              _fAlreadyActive= kODTrue;
              somSelf->KPPopupMenu(ev);
              handled = kODTrue;
              break;
         case WM_TIMER:
              somSelf->KPWriteLinkData(ev,frame, 0);
              WinStopTimer(WinQueryAnchorBlock(facet->GetFacetHWND(ev)),
              facet->GetFacetHWND(ev),0);
              break;
         default:
              break;
    }
    if (handled)
       return (handled);
    return (KeyPadPart_parent_SimplePart_HandleEvent(somSelf,
                                                     ev, event,
                                                     frame, facet,
                                                     eventInfo
                                                     ));
}

// ------------------------------------------------
//  Method:             LinkUpdated
//
//  Introduced by:      ODPart
//
//    Called when a link for which we are a
//    target has been updated.
// ------------------------------------------------

SOM_Scope void  SOMLINK LinkUpdated(KeyPadPart *somSelf,  Environment *ev,
                                    ODLink* updatedLink, ODUpdateID change)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","LinkUpdated");

    ODLinkKey   linkKey;
    ODLink    * link         = updatedLink;
    ODDraft   * myDraft      = somSelf->GetStorageUnit(ev)->GetDraft(ev);
    ODBoolean   fLockSuccess = kODFalse;

    if (change==_fLinkInfo.change) {   // A circular link do not
       return;                         // continue to propogate
    } /* endif */
    while(!fLockSuccess) {
      if (link->Lock(ev, 0, &linkKey)) {
        fLockSuccess = kODTrue;
        try {
          ODStorageUnit* linkContentSU = link->
                                           GetContentStorageUnit(ev, linkKey);
          if (linkContentSU->Exists(ev, kODPropContents, kODISOStr, 0)) {
              linkContentSU->Focus(ev, kODPropContents, kODPosUndefined, kODISOStr, 0, kODPosUndefined);
              StorageUnitGetValue(linkContentSU, ev, linkContentSU->GetSize(ev), (ODValue) _DisplayString);
          }
          if (_fFrame) {
            _fFrame->Invalidate(ev, kODNULL, kODNULL);
          }
          _fLinkInfo.change = link->GetUpdateID(ev);
          _fLinkInfo.changeTime = link->GetChangeTime(ev);
          if (_fLinkSrcPtr) {
              somSelf->KPWriteLinkData(ev, _fFrame, _fLinkInfo.change);
          } /* endif */
          link->Unlock(ev, linkKey);
        } catch(...) {
          link->Unlock(ev, linkKey);
        } 
      } else {   // Wait a second then retry
        ULONG TimeInterval; 
        APIRET rc;
        TimeInterval = 1000; 

        rc = DosSleep(TimeInterval);
        if (rc != 0) {
          /* ERROR_TS_WAKEUP */
        }
      } 
    } 
}

// ------------------------------------------------
//  Method:             HasExtension
//
//  Introduced by:      ODPart
//
//    Called to ask this part if it supports
//    a particular extension.
// ------------------------------------------------
SOM_Scope ODBoolean  SOMLINK HasExtension(KeyPadPart *somSelf,
                                          Environment *ev,
                                          ODType extensionName)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","HasExtension");

    if(!strcmp(extensionName, kODSettingsExtension) ||
       !strcmp(extensionName, kODExtSemanticInterface)) {
            return kODTrue;
    } else {
       return (KeyPadPart_parent_SimplePart_HasExtension(somSelf,
                                                         ev,
                                                         extensionName));
    }
}


// ------------------------------------------------
//  Method:             AcquireExtension
//
//  Introduced by:      ODPart
//
//    Called to ask this part to return a reference
//    to the specified extension.
// ------------------------------------------------
SOM_Scope ODExtension*  SOMLINK AcquireExtension(KeyPadPart *somSelf,
                                                 Environment *ev,
                                                 ODType extensionName)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","AcquireExtension");

    if(!strcmp(extensionName, kODSettingsExtension) )
    {
       if (!_fNotebook) {
          _fNotebook = new KPNotebook;
          _fNotebook->InitSettingsExtension( ev, _fPartWrapper);
       }
       _fNotebook->Acquire(ev);

       return _fNotebook;
    }
    else if(!strcmp(extensionName, kODExtSemanticInterface)) {
       return _fSemtIntf;
    } else {
       return KeyPadPart_parent_SimplePart_AcquireExtension(somSelf,
                                                            ev,
                                                            extensionName);
    }
}

// ------------------------------------------------
//  Method:             AcquireExtension
//
//  Introduced by:      ODPart
//
//    Called to ask this part to release
//    the specified extension.
// ------------------------------------------------
SOM_Scope void  SOMLINK ReleaseExtension(KeyPadPart *somSelf,
                                         Environment *ev,
                                         ODExtension* extension)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","ReleaseExtension");

    if (!extension) return;

    if(_fNotebook == extension) {
       _fNotebook->Release( ev);
       _fNotebook = 0;
    } else
       if (_fSemtIntf == extension) {
          _fSemtIntf->Release( ev);
          _fSemtIntf = 0;
       } else {
         KeyPadPart_parent_SimplePart_ReleaseExtension(somSelf,
                                                      ev,
                                                      extension);
    }
}

/* *************************************************
 *  KeyPadPart Private Methods
 * *************************************************
 */

// ------------------------------------------------
//  Method:             KPSetMaxDisplayLength
//
//  Introduced by:      KeyPadPart
//
//    Alters the maximum length of the display
//    string of the KeyPadPart.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPSetMaxDisplayLength(KeyPadPart *somSelf,
                                               Environment *ev,
                                              long MaxDisplayLength)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPSetMaxDisplayLength");

    if (_DisplayString) {
       SOMFree(_DisplayString);
    } /* endif */

    _KPPartData.maxDisplayLength=MaxDisplayLength;
    _DisplayString=(char *) SOMMalloc(MaxDisplayLength+1);
    strcpy(_DisplayString,"");
    if (_fFacet) {
       WinPostMsg(_fFacet->GetFacetHWND(ev),
               WM_COMMAND,
               (MPARAM)(IDM_CLEAR),
               (MPARAM)MAKEULONG(CMDSRC_MENU,0));
    } /* endif */
}

// ------------------------------------------------
//  Method:             KPCommonInit
//
//  Introduced by:      KeyPadPart
//
//    Part initialization code called from both
//    InitPart and InitPartFromStorage
// ------------------------------------------------
SOM_Scope void  SOMLINK KPCommonInit(KeyPadPart *somSelf,  Environment *ev,
                                     ODPart* PartWrapper)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPCommonInit");

    _fSession        = somSelf->GetStorageUnit(ev)->GetSession(ev);
    _fFacet          = kODNULL;
    _fFrame          = kODNULL;
    _fMenuBar        = _fSession->GetWindowState(ev)->CopyBaseMenuBar(ev);
    _fPopup          = _fSession->GetWindowState(ev)->CopyBasePopup(ev);
    _fIsLinkSource   = kODFalse;
    _fIsLinkTarget   = kODFalse;
    _DisplayString   = kODNULL;
    _fClipboardFocus = _fSession->Tokenize(ev, kODClipboardFocus);
    _fSelectionFocus = _fSession->Tokenize(ev, kODSelectionFocus);
    _fMenuFocus      = _fSession->Tokenize(ev, kODMenuFocus);
    _fKeyFocus       = _fSession->Tokenize(ev, kODKeyFocus);
    _fNotebook       = kODNULL;
    _fPartWrapper    = PartWrapper;

    _KPPartData.LinkDelay = KP_DELAY;
    _KPPartData.Color = CLR_BLACK;
    _KPPartData.BeepOn = TRUE;
    _KPPartData.maxDisplayLength = MAXDISPLEN;
    somSelf->KPSetMaxDisplayLength(ev, MAXDISPLEN);

    somSelf->KPInitMenuBar(ev, PartWrapper);
    somSelf->KPInitPopup(ev);
    somSelf->KPInitStorageUnit(ev);
    somSelf->KPInitSemanticInterface(ev);


    for (int i=0;i<12 ;i++ ) {
        _KPButtonSeq[i].hwndPB = NULL;
        _KPButtonSeq[i].xPos =
          _KPButtonSeq[i].yPos =
            _KPButtonSeq[i].xSize =
              _KPButtonSeq[i].ySize = 0;
        _KPButtonSeq[i].winID = KPBUTBASE+i;
    } /* endfor */
}

// ------------------------------------------------
//  Method:             KPInitMenBar
//
//  Introduced by:      KeyPadPart
//
//    Initializes our part's menu bar.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPInitMenuBar(KeyPadPart *somSelf,  Environment *ev,
                                      ODPart* PartWrapper)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPInitMenuBar");

    if (_fMenuBar) {

       HMODULE menuHandle;
       DosQueryModuleHandle("Keypad", &menuHandle);

       HWND hwndMenu = WinLoadMenu(HWND_DESKTOP, menuHandle, KP_MENU);

       _fMenuBar->AddMenuLast(ev,KP_MENU, hwndMenu, PartWrapper);
       _fMenuBar->SetMenuItemText(ev,KP_MENU, kODNULL, "KeyPad");

       ODPlatformMenuItem miHelp;
       miHelp.iPosition = MIT_END;
       miHelp.afStyle = MIS_TEXT;
       miHelp.afAttribute = 0;
       miHelp.hwndSubMenu = 0;
       miHelp.hItem = (ULONG)"About KeyPad";
       miHelp.id = IDM_ABOUT;

       _fMenuBar->AddMenuItemLast(ev,IDMS_HELP,kODNULL,&miHelp);
       _fMenuBar->SetMenuItemText(ev, IDMS_HELP, IDM_ABOUT,"KeyPad Information");
    } /* endif */
}

// ------------------------------------------------
//  Method:             KPInitPopup
//
//  Introduced by:      KeyPadPart
//
//    Initializes our part's context menu
// ------------------------------------------------
SOM_Scope void  SOMLINK KPInitPopup(KeyPadPart *somSelf,  Environment *ev)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPInitPopup");

    if (_fPopup) {
        ODPlatformMenuItem miPopup;
        memset((PCH)&miPopup, 0, sizeof(MENUITEM));
        miPopup.id = IDM_REVERSE;                                               
        miPopup.afStyle = MIS_TEXT;                                             
       _fPopup->AddMenuItemLast(ev, kODNULL, kODNULL, &miPopup);                
       _fPopup->SetMenuItemText(ev, kODNULL, IDM_REVERSE, "Reverse..");         
        miPopup.id = IDM_CLEAR;                                                 
        miPopup.afStyle = MIS_TEXT;                                             
       _fPopup->AddMenuItemBefore(ev, kODNULL, kODNULL, &miPopup, IDM_REVERSE); 
       _fPopup->SetMenuItemText(ev, kODNULL, IDM_CLEAR, "Clear..");             

       _fPopup->RemoveMenuItem(ev,  kODNULL, VIEW_SHOWAS);
       _fPopup->RemoveMenuItem(ev,  kODNULL, VIEW_PROPERTIES);
       _fPopup->RemoveMenuItem(ev,  kODNULL, IDMS_HELP);
       _fPopup->RemoveMenuItem(ev,  kODNULL, VIEW_OPENAS);
    } 
}


// ------------------------------------------------
//  Method:             KPInitStorageUnit
//
//  Introduced by:      KeyPadPart
//
//    Initializes our storage unit with appropriate
//    properties and values.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPInitStorageUnit(KeyPadPart *somSelf,
                                           Environment *ev)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPInitStorageUnit");

    ODStorageUnit *su = somSelf->GetStorageUnit(ev);

    if (! su->Exists(ev, kODPropPart, kODISOStr, 0))
       su->AddProperty(ev, kODPropPart)->AddValue(ev, kODISOStr);

    if (! su->Exists (ev, kODPropContents, kKind, 0)) // If Part kind
        su->AddProperty (ev, kODPropContents)->AddValue (ev, kKind);

    if (! su->Exists(ev, kODPropContents, kODISOStr, 0))
       su->AddProperty(ev, kODPropContents)->AddValue(ev, kODISOStr);

    if (! su->Exists(ev, kODPropContents, kODSLong, 0))
       su->AddProperty(ev, kODPropContents)->AddValue(ev, kODSLong);
}

// ------------------------------------------------
//  Method:             KPReadDataFromStorage
//
//  Introduced by:      KeyPadPart
//
//    Reads our parts data from storage.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPReadDataFromStorage(KeyPadPart *somSelf,
                                               Environment *ev)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPReadDataFromStorage");

    ODStorageUnit *su = somSelf->GetStorageUnit(ev);
    su->Focus(ev, kODPropContents, kODPosUndefined,
                        kKind, 0, kODPosUndefined);
    long length;
    StorageUnitGetValue(su, ev, sizeof(KPPartDataStruct), (ODValue) &_KPPartData);
    somSelf->KPSetMaxDisplayLength(ev, _KPPartData.maxDisplayLength);

    for (int i=0; i<12;  i++) {
         StorageUnitGetValue(su,ev,
                               sizeof(long),
                               (ODValue) &length);

         if (_KPButtonSeq[i].butText)
            SOMFree(_KPButtonSeq[i].butText);

         _KPButtonSeq[i].butText =
                       (char *) SOMMalloc(length);

         StorageUnitGetValue(su,ev,
                             length,
                             (ODValue) _KPButtonSeq[i].butText);

        somSelf->KPChangeButtonText(ev,KPBUTBASE+i,_KPButtonSeq[i].butText);
     }/* endfor */

    su->Focus(ev, kODPropContents, kODPosUndefined,
                         kODISOStr, 0, kODPosUndefined);
    StorageUnitGetValue(su, ev, su->GetSize(ev),
                                  (ODValue) _DisplayString);

}

// ------------------------------------------------
//  Method:             KPReadLinksFromStorage
//
//  Introduced by:      KeyPadPart
//
//    Reads our parts link reference from storage.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPReadLinksFromStorage(KeyPadPart *somSelf,
                                                Environment *ev)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPReadLinksFromStorage");

    ODStorageUnit *su = somSelf->GetStorageUnit(ev);

    _fLinkPtr = kODNULL;
    ODStorageUnitRef aSURef;

    if ( su->Exists(ev, kODPropLink, kODStrongStorageUnitRef,0) )
    {
       su->Focus(ev, kODPropLink, kODPosUndefined, kODStrongStorageUnitRef, 0, kODPosUndefined);
       StorageUnitGetValue(su,ev,sizeof(ODStorageUnitRef),&aSURef);
       if(su->IsValidStorageUnitRef(ev,aSURef)) {
          ODID linkID = su->GetIDFromStorageUnitRef(ev, aSURef);
          _fLinkPtr = su->GetDraft(ev)->AcquireLink(ev,linkID,kODNULL);
          _fLinkPtr->RegisterDependent(ev,
                                       somSelf,
                                       _fLinkInfo.change);
         _fIsLinkTarget = kODTrue;
       }
    }

    _fLinkSrcPtr = kODNULL;

    if ( su->Exists(ev, kODPropLinkSource, kODStrongStorageUnitRef,0) )
    {
       su->Focus(ev, kODPropLinkSource, kODPosUndefined, kODStrongStorageUnitRef, 0, kODPosUndefined);
       StorageUnitGetValue(su, ev, sizeof(ODStorageUnitRef), &aSURef);

       if (su->IsValidStorageUnitRef(ev, aSURef))
       {
         ODID linkSourceID = su->GetIDFromStorageUnitRef(ev, aSURef);
         _fLinkSrcPtr = su->GetDraft(ev)->AcquireLinkSource(ev, linkSourceID);
         _fLinkSrcPtr->SetSourcePart(ev, somSelf->GetStorageUnit(ev));
         _fIsLinkSource = kODTrue;
       }
    }
}

// ------------------------------------------------
//  Method:             KPWriteDataToStorage
//
//  Introduced by:      KeyPadPart
//
//    Write our parts data storage
// ------------------------------------------------
SOM_Scope void  SOMLINK KPWriteDataToStorage(KeyPadPart *somSelf,
                                              Environment *ev)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPWriteDataToStorage");

    ODStorageUnit *su =somSelf->GetStorageUnit(ev);

    su->Focus(ev, kODPropPart, kODPosUndefined, kODISOStr, 0, kODPosUndefined);
    StorageUnitSetValue(su, ev, strlen(kPart) +1, (ODValue) kPart);

    su->Focus(ev, kODPropContents, kODPosUndefined, kODISOStr, 0, kODPosUndefined);
    su->DeleteValue(ev, su->GetSize(ev));
    StorageUnitSetValue(su, ev, strlen(_DisplayString)+1, (ODValue) _DisplayString);

    long dispValueAsLong = atoi(_DisplayString);
    su->Focus(ev, kODPropContents, kODPosUndefined, kODSLong, 0, kODPosUndefined);
    StorageUnitSetValue(su, ev, sizeof(long), (ODValue) &dispValueAsLong);

    su->Focus(ev, kODPropContents, kODPosUndefined, kKind, 0, kODPosUndefined);
    StorageUnitSetValue(su, ev, sizeof(KPPartDataStruct), (ODValue) &_KPPartData);
    long length;
    for (int i=0; i<12; i++)
    {
            length = strlen(_KPButtonSeq[i].butText)+1;
            StorageUnitSetValue(su, ev, sizeof(long), (ODValue) &length);
            StorageUnitSetValue(su, ev, length, (ODValue) _KPButtonSeq[i].butText);
    }
}

// ------------------------------------------------
//  Method:             KPWriteDataToStorage
//
//  Introduced by:      KeyPadPart
//
//    Write our parts link referneces to storage
// ------------------------------------------------
SOM_Scope void  SOMLINK KPWriteLinksToStorage(KeyPadPart *somSelf,
                                               Environment *ev)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPWriteLinksToStorage");

    ODStorageUnit *su = somSelf->GetStorageUnit(ev);
    ODStorageUnitRef suRef;

    if (_fLinkSrcPtr != kODNULL) {
       if(! su->Exists(ev, kODPropLinkSource, kODStrongStorageUnitRef, 0)) {
          su->AddProperty(ev, kODPropLinkSource);
          su->AddValue(ev, kODStrongStorageUnitRef);
       } else {
          su->Focus(ev,kODPropLinkSource,kODPosUndefined,
                    kODStrongStorageUnitRef,0,kODPosUndefined);
       }
       su->GetStrongStorageUnitRef(ev, _fLinkSrcPtr->
                                         GetStorageUnit(ev)->
                                           GetID(ev), suRef);
       StorageUnitSetValue(su, ev, sizeof(ODStorageUnitRef), (ODValue) &suRef);
    }

    if (_fLinkPtr != kODNULL) {
       if (_fLinkPtr->GetUpdateID(ev) != kODUnknownUpdate) {
          if (! su->Exists(ev, kODPropLink, kODStrongStorageUnitRef, 0)) {
              su->AddProperty(ev, kODPropLink);
              su->AddValue(ev, kODStrongStorageUnitRef);
          } else {
             su->Focus(ev,kODPropLink,kODPosUndefined,
                       kODStrongStorageUnitRef,0,kODPosUndefined);
          }
          su->GetStrongStorageUnitRef(ev, _fLinkPtr->GetStorageUnit(ev)->GetID(ev), suRef);
          StorageUnitSetValue(su, ev, sizeof(ODStorageUnitRef), (ODValue) &suRef);
       } else {
          somSelf->GetStorageUnit(ev)->GetDraft(ev)->RemoveLink(ev, _fLinkPtr);
       }
    }
}

// ------------------------------------------------
//  Method:             KPWriteDataToStorage
//
//  Introduced by:      KeyPadPart
//
//    Called when Paste As menu option is selected
// ------------------------------------------------
SOM_Scope ODBoolean  SOMLINK KPDoPasteAs(KeyPadPart *somSelf,
                                          Environment *ev, ODFrame* frame)
{
   ODBoolean       canPasteLink = kODTrue;
   ODBoolean       canEmbed = kODFalse;
   ODTypeToken     viewType;
   ODPasteAsResult resultPasteAs ;
   ODBoolean       result   = kODTrue;

   KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
   KeyPadPartMethodDebug("KeyPadPart","KPDoPasteAs");

   ODClipboard  * clipboard     = _fSession->GetClipboard( ev );
   ODStorageUnit* clipContentSU = clipboard->GetContentStorageUnit( ev );

   result = clipboard->ShowPasteAsDialog(ev, canPasteLink,
                                             canEmbed,
                                             _fFacet,
                                             viewType,
                                             &resultPasteAs);
   if(result == kODFalse)  //if the user chooses the "CANCEL" button
      return result;

   if (resultPasteAs.pasteLinkSetting == kODTrue){ //if the user chooses the "PASTELINK" button
        somSelf->KPPasteLinkFromClipboard( ev, frame );
   } 
   return result;
}

// ------------------------------------------------
//  Method:             KPPostLinkToClipBoard
//
//  Introduced by:      KeyPadPart
//
//    Called to post a link spec to the clipboard
// ------------------------------------------------
SOM_Scope void  SOMLINK KPPostLinkToClipBoard( KeyPadPart *somSelf,
                                             Environment *ev,
                                             ODFrame* frame )
{
  ODBoolean      result = kODTrue;
  KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
  KeyPadPartMethodDebug("KeyPadPart","KPPostLinkToClipBoard");

  ODArbitrator *arbitrator = _fSession->GetArbitrator( ev );
  try {
     arbitrator->RequestFocus(ev, _fClipboardFocus, frame);
  } catch(...) {
    return;
  }

  ODLinkSpec* linkSpec = somSelf->
                           GetStorageUnit(ev)->
                             GetDraft(ev)->
                               CreateLinkSpec(ev,
                                              (ODPart*)somSelf,
                                              (ODByteArray*)kODNULL);
  if(linkSpec) {
    ODClipboard* clipboard = _fSession->GetClipboard(ev);
    try {
      ODStorageUnit* clipRootSU = clipboard->GetContentStorageUnit(ev);
      clipRootSU->AddProperty(ev, kODPropContents)->AddValue(ev, kKind);
      clipRootSU->AddProperty(ev, kODPropLinkSpec);
      clipRootSU->AddValue(ev, kODLinkSpec);
      linkSpec->WriteLinkSpec(ev, clipRootSU);
      clipboard->ExportClipboard( ev );
    } catch (... ) {
      result = kODFalse;
    }
    delete linkSpec;
  }
  arbitrator->RelinquishFocus( ev, _fClipboardFocus, frame );
}

// ------------------------------------------------
//  Method:             KPPostLinkFromClipboard
//
//  Introduced by:      KeyPadPart
//
//    Called when creating a link as a target
//     from the clipboard.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPPasteLinkFromClipboard(KeyPadPart *somSelf,
                                                  Environment *ev,
                                                 ODFrame* frame)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPPasteLinkFromClipboard");

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
      ODLink* link = myDraft->AcquireLink(ev,0,linkSpec);

      if(link != kODNULL) {
         _fLinkPtr = link;
         link->RegisterDependent(ev,
                                 somSelf,
                                 _fLinkInfo.change);
         _fIsLinkTarget = kODTrue;
      }
    } catch(...) {
      result = kODFalse;
    }
    delete linkSpec;
    arbitrator->RelinquishFocus( ev, _fClipboardFocus, frame );
}

// ------------------------------------------------
//  Method:             KPWriteLinkData
//
//  Introduced by:      KeyPadPart
//
//    When a link source, this method is called
//     to write updated data to the linksource SU
// ------------------------------------------------

SOM_Scope void  SOMLINK KPWriteLinkData(KeyPadPart *somSelf,
                                        Environment *ev, 
                                        ODFrame* frame,
                                        ODUpdateID updateID)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPWriteLinkData");

    ODLinkKey     linkKey;
    ODLinkSource* linkSource   = _fLinkSrcPtr;
    ODDraft*      myDraft      = somSelf->GetStorageUnit(ev)->GetDraft(ev);
    ODBoolean     fLockSuccess = kODFalse;

    while(!fLockSuccess) {
      if(linkSource->Lock(ev,0,&linkKey)) {
        fLockSuccess = kODTrue;
        try {
          linkSource->Clear(ev, _fSession->UniqueUpdateID(ev), linkKey);
          ODStorageUnit* linkContentSU = linkSource->
                                           GetContentStorageUnit(ev, linkKey);
          try {
             if (linkContentSU->Exists(ev, kODPropContents, kODISOStr, 0)) {
                 linkContentSU->Focus( ev, kODPropContents, 0, kODISOStr, 1, kODPosUndefined );
             } else {
                  linkContentSU->AddProperty(ev, kODPropContents);
                  linkContentSU->AddValue(ev, (ODValueType) kODISOStr);
             }
             if(_DisplayString == kODNULL) {
                  StorageUnitSetValue(linkContentSU, ev, 1 , (ODValue)"");
             } else {
                 StorageUnitSetValue(linkContentSU, ev, 
                                     strlen(_DisplayString) + 1, 
                                     (ODValue)_DisplayString);
             }
             if (linkContentSU->Exists(ev, kODPropContents, kODSLong, 0) ) {
                 linkContentSU->Focus(ev, kODPropContents, kODPosUndefined,
                                      kODSLong, 0, kODPosUndefined);
             } else {
                 linkContentSU->AddValue(ev, (ODValueType) kODSLong);
             }
             long DisplayStringAsLong=atoi(_DisplayString);
             StorageUnitSetValue(linkContentSU, ev, sizeof(long), (ODValue)&DisplayStringAsLong);
          } catch(...) {
               // do something
          } 
          //Inform the link that it has changed, so destination can update.
          //The updateID parameter should be the changed causing the link
          //to update.
          if (updateID) {
              linkSource->ContentUpdated(ev, updateID, linkKey);
          } else {
              linkSource->ContentUpdated(ev, _fSession->UniqueUpdateID(ev), linkKey);
          } /* endif */
          linkSource->Unlock(ev, linkKey);
        } catch(...) {
          linkSource->Clear(ev, _fSession->UniqueUpdateID(ev), linkKey);
          linkSource->Unlock(ev, linkKey);
        } 
      } else {
        ULONG TimeInterval; 
        APIRET rc;
        TimeInterval = 1000;
        DosSleep(TimeInterval);
      } 
    } 
}

// ------------------------------------------------
//  Method:             KPInitButtonInfo
//
//  Introduced by:      KeyPadPart
//
//    Sets the text for all of the
//     pushbuttons.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPInitButtonInfo(KeyPadPart *somSelf,
                                          Environment *ev)
{

    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPInitButtonInfo");

    somSelf->KPChangeButtonText(ev,KPBUT_0,"0");  //Default text for buttons
    somSelf->KPChangeButtonText(ev,KPBUT_1,"1");
    somSelf->KPChangeButtonText(ev,KPBUT_2,"2");
    somSelf->KPChangeButtonText(ev,KPBUT_3,"3");
    somSelf->KPChangeButtonText(ev,KPBUT_4,"4");
    somSelf->KPChangeButtonText(ev,KPBUT_5,"5");
    somSelf->KPChangeButtonText(ev,KPBUT_6,"6");
    somSelf->KPChangeButtonText(ev,KPBUT_7,"7");
    somSelf->KPChangeButtonText(ev,KPBUT_8,"8");
    somSelf->KPChangeButtonText(ev,KPBUT_9,"9");
    somSelf->KPChangeButtonText(ev,KPBUT_10,"*");
    somSelf->KPChangeButtonText(ev,KPBUT_11,"#");
}

// ------------------------------------------------
//  Method:             KPCreateButtons
//
//  Introduced by:      KeyPadPart
//
//    Creates all of the pushbutton controls for
//     the KeyPadPart
// ------------------------------------------------
SOM_Scope void  SOMLINK KPCreateButtons(KeyPadPart *somSelf,
                                         Environment *ev, HWND hwndFacet)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPCreateButtons");

    for (int i=0;i<12 ;i++ ) {                        // Create Keypad Buttons
        _KPButtonSeq[i].hwndPB =
              WinCreateWindow(hwndFacet,
                              WC_BUTTON,
                              _KPButtonSeq[i].butText ,
                              BS_PUSHBUTTON | WS_DISABLED,
                              _KPButtonSeq[i].xPos,
                              _KPButtonSeq[i].yPos,
                              _KPButtonSeq[i].xSize,
                              _KPButtonSeq[i].ySize,
                              hwndFacet,
                              HWND_TOP,
                              _KPButtonSeq[i].winID,
                              NULL,NULL);

        pButClass = WinSubclassWindow(_KPButtonSeq[i].hwndPB,
                                         (PFNWP)SubclassWndProc);
        WinSetWindowULong(_KPButtonSeq[i].hwndPB,
                          QWL_USER,
                          (ULONG)somSelf);       // Store pointer to somSelf
    }/* endfor */                                // in Window Words
}

// ------------------------------------------------
//  Method:             KPChangeButtonText
//
//  Introduced by:      KeyPadPart
//
//    Changes the text of an individual button.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPChangeButtonText(KeyPadPart *somSelf,
                                            Environment *ev,
                                           long buttonID, string newText)
{
    int i=0;

    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPChangeButtonText");

    while ((_KPButtonSeq[i].winID != buttonID) && (i<12)) {
          i++;
    }
    if (i >= 12) {
       return;
    }

    if (_KPButtonSeq[i].butText) {
        SOMFree(_KPButtonSeq[i].butText);
    }
    strcpy( _KPButtonSeq[i].butText =
          (char *) SOMMalloc (strlen(newText)+1),newText);

    if (_KPButtonSeq[i].hwndPB) {
        WinSetWindowText(_KPButtonSeq[i].hwndPB,newText);
    } /* endif */
}

// ------------------------------------------------
//  Method:             KPPopup Menu
//
//  Introduced by:      KeyPadPart
//
//    Displays the context menu for the KeyPadPart
// ------------------------------------------------
SOM_Scope void  SOMLINK KPPopupMenu(KeyPadPart *somSelf,  Environment *ev)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPPopupMenu");

    // Could do menu manipulation here...

    _fPopup->Display(ev);
}

// ------------------------------------------------
//  Method:             KPHandleKeyBoard
//
//  Introduced by:      KeyPadPart
//
//    This method is called when a keystroke is received.
//     It converts to the appropriate WM_COMMAND message.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPHandleKeyBoard(KeyPadPart *somSelf,
                                          Environment *ev, MPARAM pm1,
                                         MPARAM pm2)
{
    UCHAR   chr;
    int     i;

    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPHandleKeyBoard");

    if (((ULONG)pm1)&KC_KEYUP)                    // upstate and ignore
       return;

    chr = LOUCHAR(LOUSHORT(pm2));                 // save character
    HWND hwndFacet = _fFacet->GetFacetHWND(ev);
    if (LOUCHAR(LOUSHORT(pm1))&KC_CHAR)           // if not a virtual key
        for (i=0;i<12 ;i++)
            if (_KPButtonSeq[i].butText[0]==chr){
                WinPostMsg(hwndFacet,
                           WM_COMMAND,
                           (MPARAM)_KPButtonSeq[i].winID,
                           (MPARAM)MAKEULONG(CMDSRC_PUSHBUTTON,0));
                 break;
            }
}

// ------------------------------------------------
//  Method:             KPHandleCommand
//
//  Introduced by:      KeyPadPart
//
//    Called from HandleEvent() to handle any WM_COMMAND
//     messages.
// ------------------------------------------------
SOM_Scope ODBoolean  SOMLINK KPHandleCommand(KeyPadPart *somSelf,
                                              Environment *ev,
                                             ODEventData* event,
                                             ODFrame* frame,
                                             ODFacet* facet)
{
    ODShape *frameShapeRef;
    ODLinkInfoResult linkInfoResult;

    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPHandleCommand");

    ODBoolean handled = kODFalse;
    frameShapeRef=frame->AcquireFrameShape(ev,kODNULL);

    switch (SHORT1FROMMP(event->mp1)) {
           case KPBUT_0:
           case KPBUT_1:
           case KPBUT_2:
           case KPBUT_3:
           case KPBUT_4:
           case KPBUT_5:
           case KPBUT_6:
           case KPBUT_7:
           case KPBUT_8:
           case KPBUT_9:
                if (strlen(_DisplayString)<_KPPartData.maxDisplayLength) {
                   strcat(_DisplayString,
                          _KPButtonSeq[SHORT1FROMMP(event->mp1) - KPBUTBASE].butText);
                    if(_KPPartData.BeepOn) {
                       DosBeep(100,100);
                    }
                   frame->Invalidate(ev,kODNULL,kODNULL);      
                } else {
                    if(_KPPartData.BeepOn) {
                       DosBeep(1000,250);
                    }
                }
                if (_fLinkSrcPtr) {
                  WinStartTimer(WinQueryAnchorBlock(facet->GetFacetHWND(ev)),
                                                    facet->GetFacetHWND(ev),
                                                    0,_KPPartData.LinkDelay);
                } /* endif */
                handled = kODTrue;
                break;
           case KPBUT_10:
                somSelf->Externalize(ev);
                writeSU(somSelf->GetStorageUnit(ev),"KeyPadPart SU");
                writeSU(frame->GetStorageUnit(ev),"KeyPad's Frame SU");
                if(_KPPartData.BeepOn)
                {
                   DosBeep(400,200);
                   DosBeep(600,200);
                 }
                handled = kODTrue;
                break;
           case KPBUT_11:
                   strcpy(_DisplayString,"");
                   frame->Invalidate(ev,kODNULL,kODNULL);      
                   if (_fLinkSrcPtr) {
                       somSelf->KPWriteLinkData(ev, _fFrame, 0); // Update link now
                   } /* endif */
                handled = kODTrue;
                break;
           case IDM_ABOUT:
                {
                  POINTL ptl;                     
                  HWND   hwndClient;              
                  WinQueryPointerPos(HWND_DESKTOP, &ptl);

                  WinMessageBox(HWND_DESKTOP,
                                HWND_DESKTOP,          
                                "KeyPadPart V1.1 built by ITSO-Austin for Redbook SG24-4673",
                                "About KeyPadPart",     
                                 0,                     
                                 MB_INFORMATION | MB_OK);
                }
                break;
           case IDM_CLEAR:
                {
                   strcpy(_DisplayString,"");
                   frame->Invalidate(ev, NULL,kODNULL);
                   if (_fLinkSrcPtr) {
                       somSelf->KPWriteLinkData(ev, _fFrame, 0); // Update link now
                   } /* endif */
                }
                break;
           case IDM_REVERSE:                                                    
                {                                                               
                        string tmpStr = (string)SOMMalloc(_KPPartData.maxDisplayLength+1); 
                        strcpy(tmpStr, _DisplayString);                         
                        somSelf->KPHandleOSAEventSend(ev,frame,strrev(tmpStr)); 
                        if(_KPPartData.BeepOn)
                        {
                          DosBeep(440,50);DosBeep(880,75);DosBeep(1760,50);     
                        }
                        handled = kODTrue;                                      
                        SOMFree(tmpStr);                                        
                        if (_fLinkSrcPtr) {
                            somSelf->KPWriteLinkData(ev, _fFrame, 0); // Update link now
                        } /* endif */
                }                                                               
                break;                                                          
           case IDM_TRACE:
                   if (!SOM_TraceLevel) {
                      if (( stream = fopen("somtrace.log", "w"))== NULL )
                         {
                           if(_KPPartData.BeepOn)
                           {
                             DosBeep(500,1000);
                             DosBeep(1000,200);
                             DosBeep(500,200);
                            }
                         }
                      SavedSOMOutCharRoutine=SOMOutCharRoutine;
                      SOMOutCharRoutine = (somTD_SOMOutCharRoutine *)  myOutCharRoutine;
                      SOM_TraceLevel=1;
                      _fMenuBar->CheckMenuItem(ev, KP_MENU, IDM_TRACE,kODTrue);
                      if(_KPPartData.BeepOn)
                      {
                        DosBeep(500,200);
                        DosBeep(1000,500);
                      }
                   } else
                   if (SOM_TraceLevel && stream ) {
                      SOM_TraceLevel=0;
                      fclose(stream);
                      stream=NULL;
                      SOMOutCharRoutine=SavedSOMOutCharRoutine;
                      _fMenuBar->CheckMenuItem(ev, KP_MENU, IDM_TRACE,kODFalse);
                      if(_KPPartData.BeepOn)
                      {
                         DosBeep(100,100);
                      }
                   }
                break;
           case EDIT_PASTEAS:
                somSelf->KPDoPasteAs(ev, frame);
                frame->Invalidate(ev, kODNULL, kODNULL);
                handled=kODTrue;
                break;
           case EDIT_COPY:
                somSelf->KPPostLinkToClipBoard( ev, frame );
                handled=kODTrue;
                break;
           case EDIT_BREAKLINK:
                if (_fLinkPtr) {
                   _fLinkPtr->UnregisterDependent(ev,somSelf);
                   _fLinkPtr->Release(ev);
                   _fLinkPtr=kODNULL;
                   _fIsLinkTarget=kODFalse;
                } /* endif */
                if (_fLinkSrcPtr) {
                   _fLinkSrcPtr->Release(ev);
                   _fLinkSrcPtr=kODNULL;
                   _fIsLinkSource=kODFalse;
                } /* endif */
                handled=kODTrue;
                break;

           case VIEW_PROPERTIES:
                   {
                     ODFrameFacetIterator* facets = frame->CreateFacetIterator(ev);
                     facets->InitFrameFacetIterator(ev,frame);
                     _fSession->GetInfo(ev)->ShowPartFrameInfo(ev,facets->First(ev), kODFalse);
                     delete facets;
                     handled = kODTrue;
                   }
                break;

           default:
                break;
    }
    frameShapeRef->Release(ev);
    return (handled);
}


// ------------------------------------------------
//  Method:             KPCalcDisplayRect
//
//  Introduced by:      KeyPadPart
//
//    Called to calculate the appropriate size of the
//    keypad's display area... based on the frame size.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPCalcDisplayRect(KeyPadPart *somSelf,
                                           Environment *ev, ODRect* rect)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPCalcDisplayRect");

    int tmp;

    _ODisplayRect.xLeft   = FIXED2LONG(rect->right)/10;
    _ODisplayRect.xRight  = 9*(FIXED2LONG(rect->right))/10;
    _ODisplayRect.yTop    = 17 *(FIXED2LONG(rect->top))/18;
    _ODisplayRect.yBottom = 4*(FIXED2LONG(rect->top))/5;

    _IDisplayRect = _ODisplayRect;
    tmp = (_IDisplayRect.xRight - _IDisplayRect.xLeft)/20; ;
    _IDisplayRect.xLeft  += tmp;
    _IDisplayRect.xRight -= tmp;

    tmp = (_IDisplayRect.yTop-_IDisplayRect.yBottom)/5;
    _IDisplayRect.yTop    -= tmp;
    _IDisplayRect.yBottom += tmp;
}

// ------------------------------------------------
//  Method:             KPCalcKPButLayout
//
//  Introduced by:      KeyPadPart
//
//    Called to calculate the appropriate size and position
//    of keypad buttons... based on the frame size.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPCalcKPButLayout(KeyPadPart *somSelf,
                                           Environment *ev, ODTransform* transform,
                                          ODRect* rect)
{
    MATRIXLF mtx;

    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPCalcKPButLayout");

    transform->GetMATRIXLF(ev, &mtx);             // Create MATRIXLF for the transform
    if (!(mtx.fxM12 == 0 && mtx.fxM21 == 0 && mtx.fxM11 > 0 && mtx.fxM22  > 0)) {
       for (int i=0;i<12 ;i++ ) {                     // We can't properly display the buttons in
           WinShowWindow(_KPButtonSeq[i].hwndPB, FALSE); // this frame with the given transform.
       } /* endfor */                             // Hide the window and we'll draw the text
       return;                                    // into the frame in our Draw method
    }
                                                  // Now we must prepare for conversion to PM
                                                  // device coorodnates for calculating our
                                                  // PM control's (button's) positions.
    ODPoint botLeft  = {rect->left, rect->bottom};// Create ODPoints from rect
    ODPoint topRight = {rect->right, rect->top};
    botLeft = transform->
                 TransformPoint(ev, &botLeft);    // Transform each point to 'fixed format' 
    topRight = transform->
                 TransformPoint(ev, &topRight);   // device coordinates.
    POINTL bLeft  = {FIXED2LONG(botLeft.x),
                     FIXED2LONG(botLeft.y)};      // Convert to long integer device
    POINTL tRight = {FIXED2LONG(topRight.x),      // coordinates.  Used below for       
                     FIXED2LONG(topRight.y)};     // for calcuation of button positions 
                                                  // and sizes.                         
     long  Row1, Row2, Row3, Row4, Col1, Col2, Col3, butTop, butBot, butHeight, butWidth;

     butTop    = 3 * (tRight.y - bLeft.y) / 4;
     butBot    = (tRight.y - bLeft.y) / 10;
     butHeight = (butTop -butBot) / 4;
     butWidth  = (tRight.x - bLeft.x) / 4;        // Width of buttons

     Row1 = butBot + bLeft.y;                     // Bottom of each row of buttons
     Row2 = Row1   + butHeight;
     Row3 = Row2   + butHeight;
     Row4 = Row3   + butHeight;

     Col1 = bLeft.x + (tRight.x - bLeft.x) / 8;   // Left side of each col of buttons
     Col2 = Col1 + butWidth;
     Col3 = Col2 + butWidth;

     _KPButtonSeq[1].xPos = _KPButtonSeq[4].xPos = _KPButtonSeq[7].xPos = _KPButtonSeq[10].xPos = Col1;
     _KPButtonSeq[2].xPos = _KPButtonSeq[5].xPos = _KPButtonSeq[8].xPos = _KPButtonSeq[0].xPos  = Col2;
     _KPButtonSeq[3].xPos = _KPButtonSeq[6].xPos = _KPButtonSeq[9].xPos = _KPButtonSeq[11].xPos = Col3;

     _KPButtonSeq[1].yPos  = _KPButtonSeq[2].yPos = _KPButtonSeq[3].yPos  = Row4;
     _KPButtonSeq[4].yPos  = _KPButtonSeq[5].yPos = _KPButtonSeq[6].yPos  = Row3;
     _KPButtonSeq[7].yPos  = _KPButtonSeq[8].yPos = _KPButtonSeq[9].yPos  = Row2;
     _KPButtonSeq[10].yPos = _KPButtonSeq[0].yPos = _KPButtonSeq[11].yPos = Row1;

     for (int i=0;i<12 ;i++ ) {
         _KPButtonSeq[i].xSize = butWidth;        // Set size of each button and show it...
         _KPButtonSeq[i].ySize = butHeight;

     }
}

// ------------------------------------------------
//  Method:             KPResize
//
//  Introduced by:      KeyPadPart
//
//    Called from FrameShapeChanged to attempt
//    to always obtain a square shape for the 
//    KeyPadPart's frame.  (WARNING: If a container
//    refuses to give us a square shape, this implementation
//    never gives inand we will loop.!!!)
// ------------------------------------------------
SOM_Scope void  SOMLINK KPResize(KeyPadPart *somSelf,  Environment *ev,
                                 ODFrame* frame)
{

    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPResize");

    ODRect    frameRect;
    LONG      cxFrame, cyFrame;
    ODShape * requestedShape;
    ODShape * frameShapeRef;

    frameShapeRef = frame-> AcquireFrameShape(ev,kODNULL);
    frameShapeRef->GetBoundingBox(ev, &frameRect);
    frameShapeRef->Release(ev);

    cxFrame = frameRect.right - frameRect.left;
    cyFrame = frameRect.top - frameRect.bottom;
    if (cxFrame==cyFrame) {
       return;
    } 
    if (cxFrame > cyFrame)
       frameRect.right = frameRect.left + cyFrame;
    else if (cxFrame < cyFrame)
       frameRect.top = frameRect.bottom + cxFrame;

    requestedShape = frame->CreateShape(ev);
    requestedShape->SetRectangle(ev, &frameRect);
    frame->RequestFrameShape(ev, requestedShape,kODNULL);

    requestedShape = kODNULL;                     // shape now owned by frame
}


// ------------------------------------------------
//  Method:             KPDrawDisplay
//
//  Introduced by:      KeyPadPart
//
//    Called to render the Display area of the
//    KeyPadPart.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPDrawDisplay(KeyPadPart *somSelf,  Environment *ev, 
                                      HPS hpsDraw)
{
    HRGN   disphrgn, dummy;
    POINTL xll,xul,xur,xlr;
    POINTL ill,iul,iur,ilr;

    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPDrawDisplay");

    xll.x = _ODisplayRect.xLeft;
    xll.y = _ODisplayRect.yBottom;
    xur.x = _ODisplayRect.xRight;
    xur.y = _ODisplayRect.yTop;
    xul.x = _ODisplayRect.xLeft;
    xul.y = _ODisplayRect.yTop;
    xlr.x = _ODisplayRect.xRight;
    xlr.y = _ODisplayRect.yBottom;
                         
                         
    ill.x = _IDisplayRect.xLeft;
    ill.y = _IDisplayRect.yBottom;
    iur.x = _IDisplayRect.xRight;
    iur.y = _IDisplayRect.yTop;
    iul.x = _IDisplayRect.xLeft;
    iul.y = _IDisplayRect.yTop;
    ilr.x = _IDisplayRect.xRight;
    ilr.y = _IDisplayRect.yBottom;

    GpiBeginPath(hpsDraw,1);                      // Path will let us define new clip area
      GpiSetCurrentPosition(hpsDraw,              // Lower left of display area (calculated in
                          (PPOINTL) &_ODisplayRect.xLeft);
                                                  // GeometryChanged
      GpiBox(hpsDraw,
             DRO_OUTLINE,
             (PPOINTL) &_ODisplayRect.xRight,0,0); // Draw outline box
      GpiLine(hpsDraw,&xul);
      GpiLine(hpsDraw,&xur);
      GpiLine(hpsDraw,&iur);
      GpiLine(hpsDraw,&ilr);
      GpiLine(hpsDraw,&ill);
      GpiLine(hpsDraw,&xll);
    GpiEndPath(hpsDraw);                          // End path

    GpiSetColor(hpsDraw,CLR_DARKGRAY);
    GpiFillPath(hpsDraw,1,FPATH_ALTERNATE);

    GpiSetColor(hpsDraw,CLR_BLACK);
    GpiSetCurrentPosition(hpsDraw, &xlr);
    GpiLine(hpsDraw,&ilr);

    GpiSetColor(hpsDraw,CLR_DARKGRAY);
    GpiSetCurrentPosition(hpsDraw, &xul);
    GpiLine(hpsDraw,&iul);

    GpiSetColor(hpsDraw,CLR_GREEN);
    GpiSetCurrentPosition(hpsDraw ,               // Lower left of display area (calculated in
                            (PPOINTL) &_IDisplayRect.xLeft);
                                                  // GeometryChanged
    GpiBox(hpsDraw,
           DRO_FILL,                 // was DRO_OUTLINE
           (PPOINTL) &_IDisplayRect.xRight,0,0);  // Draw outline box
}

// ------------------------------------------------
//  Method:             KPDrawTextToDisplay
//
//  Introduced by:      KeyPadPart
//
//    Called to write the text to the display area of the
//    KeyPadPart.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPDrawTextToDisp(KeyPadPart *somSelf,
                                          Environment *ev, HPS hpsDraw)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPDrawTextToDisp");
    GpiSetColor(hpsDraw, CLR_BLACK);              // Set text color
    SetOutlineFont(hpsDraw,                       // Call function to set font to fit our
                   _IDisplayRect.yTop-_IDisplayRect.yBottom); // display area (height only)
    GpiSetTextAlignment(hpsDraw,TA_RIGHT,TA_TOP );// Right align text
    GpiCharStringAt(hpsDraw,                      // Write displaystring
                    (PPOINTL) &_IDisplayRect.xRight,
                    strlen(_DisplayString), _DisplayString);
}

// ------------------------------------------------
//  Method:             KPDrawFrame
//
//  Introduced by:      KeyPadPart
//
//    Called to draw the body of the KeyPadPart.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPDrawFrame(KeyPadPart *somSelf,  Environment *ev,
                                    HPS hpsDraw, RECTL* frameRect)
{
    RECTL iframeRect;

    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPDrawFrame");

    GpiSetColor(hpsDraw, _KPPartData.Color);
    GpiMove(hpsDraw,
            (PPOINTL)&frameRect->xLeft );         // Set initial starting position (lower left)
    GpiBox(hpsDraw,                               // Fill box to upper right
           DRO_FILL,                              // Remember, this will clip around our
           (PPOINTL)&frameRect->xRight, 0, 0);    // buttons because WS_CLIPCHILDREN style
                                                  // set above.

    if (_fIsLinkSource || _fIsLinkTarget) {
       if (_fIsLinkSource) {
          GpiSetColor(hpsDraw,CLR_BLUE);
       } else {
          GpiSetColor(hpsDraw,CLR_RED);
       } /* endif */
    } else {
        GpiSetColor(hpsDraw,CLR_YELLOW);
    } /* endif */
    iframeRect.xRight  = frameRect->xRight - 3;
    iframeRect.xLeft   = frameRect->xLeft + 3;
    iframeRect.yTop    = frameRect->yTop - 3;
    iframeRect.yBottom = frameRect->yBottom + 3;

    GpiMove(hpsDraw, (PPOINTL)&iframeRect.xLeft ); // Set initial starting position (lower left)
    GpiBox(hpsDraw,
           DRO_OUTLINE,
           (PPOINTL)&iframeRect.xRight, 0, 0);     // Fill box to upper right
}

// ------------------------------------------------
//  Method:             KPDrawButtons
//
//  Introduced by:      KeyPadPart
//
//    Called to display the PM control pushbuttons
// ------------------------------------------------
SOM_Scope void  SOMLINK KPDrawButtons(KeyPadPart *somSelf,  Environment *ev)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPDrawButtons");

    for (int i=0;i<12 ;i++ ) {                     // Show buttons
        WinSetWindowPos(_KPButtonSeq[i].hwndPB, 0,
                        _KPButtonSeq[i].xPos,
                        _KPButtonSeq[i].yPos,
                        _KPButtonSeq[i].xSize,
                        _KPButtonSeq[i].ySize,
                        SWP_SHOW | SWP_MOVE | SWP_SIZE);
    }
}

// ------------------------------------------------
//  Method:             KPChangeSetting
//
//  Introduced by:      KeyPadPart
//
//    Called from properties notebook to change
//    settings.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPChangeSetting(KeyPadPart *somSelf,
                                        Environment *ev, ODFrame* aFrame,
                                        RGBColor whichColor,ODUShort idelay,
                                        ODBoolean beep )
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPChangeSetting");
    ODULong   tempColor;

    switch (whichColor)
    {
      case  IDMA_COLOR_GRAY   : tempColor =  CLR_DARKGRAY    ; break;
      case  IDMA_COLOR_RED    : tempColor =  CLR_RED         ; break;
      case  IDMA_COLOR_GREEN  : tempColor =  CLR_GREEN       ; break;
      case  IDMA_COLOR_YELLOW : tempColor =  CLR_YELLOW      ; break;
      case  IDMA_COLOR_BLUE   : tempColor =  CLR_BLUE        ; break;
      case  IDMA_COLOR_MAGENTA: tempColor =  CLR_DARKPINK    ; break;
      case  IDMA_COLOR_CYAN   : tempColor =  CLR_CYAN        ; break;
      case  IDMA_COLOR_BLACK  : tempColor =  CLR_BLACK       ; break;
      default: tempColor=CLR_BLACK;break;
    }

    _KPPartData.Color=tempColor;
    _KPPartData.LinkDelay = idelay;
    _KPPartData.BeepOn = beep;
}

// ------------------------------------------------
//  Method:             KPGetSetting
//
//  Introduced by:      KeyPadPart
//
//    Called from properties notebook to obtain
//    current settings.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPGetSetting(KeyPadPart *somSelf,  Environment *ev,
                                     PKPPartDataStruct PartData)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPGetSetting");
    ODUShort tempColor;

    switch (_KPPartData.Color)
    {
      case CLR_DARKGRAY  : tempColor =  IDMA_COLOR_GRAY       ; break;
      case CLR_RED       : tempColor =  IDMA_COLOR_RED        ; break;
      case CLR_GREEN     : tempColor =  IDMA_COLOR_GREEN      ; break;
      case CLR_YELLOW    : tempColor =  IDMA_COLOR_YELLOW     ; break;
      case CLR_BLUE      : tempColor =  IDMA_COLOR_BLUE       ; break;
      case CLR_DARKPINK  : tempColor =  IDMA_COLOR_MAGENTA    ; break;
      case CLR_CYAN      : tempColor =  IDMA_COLOR_CYAN       ; break;
      case CLR_BLACK     : tempColor =  IDMA_COLOR_BLACK      ; break;
      default: tempColor=IDMA_COLOR_BLACK; break;
    }

    PartData->Color = tempColor;
    PartData->LinkDelay =_KPPartData.LinkDelay;
    PartData->BeepOn =_KPPartData.BeepOn;
}

// ------------------------------------------------
//  Method:             KPIsLinkInClipboard
//
//  Introduced by:      KeyPadPart
//
//    Called during paste operation to determine if
//    the clipboard contains a linkspec.
// ------------------------------------------------

SOM_Scope ODBoolean  SOMLINK KPIsLinkInClipboard(KeyPadPart *somSelf,
                                                  Environment *ev,
                                                  ODFrame* frame)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPIsLinkInClipboard");

    ODArbitrator * arbitrator = _fSession->GetArbitrator( ev );
       arbitrator->RequestFocus( ev, _fClipboardFocus, frame );

    ODClipboard   * clipboard = _fSession->GetClipboard(ev);
    ODStorageUnit *   clipContentSU = clipboard->GetContentStorageUnit(ev);
    ODBoolean result = clipContentSU->
                          Exists(ev,
                                 (ODPropertyName) kODPropLinkSpec,
                                 kODLinkSpec,
                                 0);
    arbitrator->RelinquishFocus( ev, _fClipboardFocus, frame );
    return result;
}

// ------------------------------------------------
//  Method:             KPHandleOSAEventSend
//
//  Introduced by:      KeyPadPart
//
//    Called to generate and send an OSA event.
// ------------------------------------------------
/*
 *@SAKUMA
 */

SOM_Scope void  SOMLINK KPHandleOSAEventSend(KeyPadPart *somSelf,
                                             Environment *ev,
                                             ODFrame* frame,
                                             string myString)
{
        AEDesc    objspec, myDesc;
        ODError   err = noErr;

        KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
        KeyPadPartMethodDebug("KeyPadPart","KPHandleOSAEventSend");

        // Create a desc for the text
        THROW_IF_ERROR( AECreateDesc(typeChar, &myString, sizeof(myString), &myDesc) );
        // create objspec
        CreatePropObjSpec(somSelf, ev, frame, formUniqueID, pContents, objspec);
        TRY
        // Send the SetData event
        SendSetDataEvent(somSelf, ev, objspec, myDesc);
        CATCH_ALL
        err = ErrorCode();
        ENDTRY
        AEDisposeDesc(&myDesc);
        AEDisposeDesc(&objspec);
        THROW_IF_ERROR(err);

        return;

}  // KPHandleOSAEventSend()

// ------------------------------------------------
//  Method:             KPInitSemanticInterface
//
//  Introduced by:      KeyPadPart
//
//    Called to register the semantic interface extension
//    and to intialize it.
// ------------------------------------------------
SOM_Scope void  SOMLINK KPInitSemanticInterface(KeyPadPart *somSelf,
                                                 Environment *ev)
{
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);
    KeyPadPartMethodDebug("KeyPadPart","KPInitSemanticInterface");

     _fSemtIntf      = new ODSemanticInterface();                               
     _fSemtIntf->InitSemanticInterface(ev, somSelf, _fSession);                 
     ODObjectAccessorUPP theAccessorUPP;                                        
                                                                                
     theAccessorUPP = NewODObjectAccessorProc( GetPropertyFromNULL );           
     _fSemtIntf->InstallObjectAccessor(ev, cProperty, typeNull, theAccessorUPP, 
                                       (ODSLong)somSelf);                       
                                                                                
     ODEventHandlerUPP newHandler;                                              
                                                                                
     newHandler = NewODEventHandlerProc( HandleSetData ) ;                      
     _fSemtIntf->InstallEventHandler(ev, kAECoreSuite, kAESetData, newHandler,  
                                     (ODSLong)somSelf);                         
}


/* ********************************************
 *  Metaclass methods - Used for registration
 *
 * ********************************************
 */

SOM_Scope ISOString  SOMLINK clsGetODPartHandlerName(M_KeyPadPart *somSelf,
                                                      Environment *ev)
{
    /* M_KeyPadPartData *somThis = M_KeyPadPartGetData(somSelf); */
    M_KeyPadPartMethodDebug("M_KeyPadPart","clsGetODPartHandlerName");

    string handlerName = kPartHandlerName;
    return ((ISOString) handlerName);
}

SOM_Scope string  SOMLINK clsGetODPartHandlerDisplayName(M_KeyPadPart *somSelf,
                                                          Environment *ev)
{
    /* M_KeyPadPartData *somThis = M_KeyPadPartGetData(somSelf); */
    M_KeyPadPartMethodDebug("M_KeyPadPart","clsGetODPartHandlerDisplayName");

    string displayName = kPartHandlerName;
    return (displayName);
}

SOM_Scope _IDL_SEQUENCE_PartKindInfo  SOMLINK clsGetODPartKinds(M_KeyPadPart *somSelf,
                                                                 Environment *ev)
{
    /* M_KeyPadPartData *somThis = M_KeyPadPartGetData(somSelf); */
    M_KeyPadPartMethodDebug("M_KeyPadPart","clsGetODPartKinds");

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

// ==============================================
// Functions used to receive and handle OSA events.
// ==============================================


static ODError APIENTRY GetPropertyFromNULL(ODPart*     part,
                                            DescType    desiredClass,
                                            ODOSLToken* KeyPad,
                                            DescType    keypadClass,
                                            DescType    form,
                                            ODDesc*     selectionData,
                                            ODOSLToken* value,
                                            ODSLong     refCon)
{
        Environment*    ev = somGetGlobalEnvironment();
        ODSession*      session = ((ODPart *)refCon)->GetStorageUnit(ev)->GetSession(ev);
        ODNameResolver* resolver = session->GetNameResolver(ev);
        DescType        propID;
        KeyPadPropAccessor*     accessorObj;
        ODError         error = noErr;
        TRY
        if (form != formPropertyID) THROW(errAEWrongDataType);
        AEDesc          realData;
        DescType        typeCode;
        Size            dataSize = sizeof(DescType);
        THROW_IF_ERROR( ODDescToAEDesc( selectionData, &realData ) );
        AEGetDescData(&realData, &typeCode, (Ptr) &propID,
                      (Size)sizeof(DescType), &dataSize);
        switch(propID) {
                case pContents: {
                     if(!resolver->IsODToken(ev, value))
                             THROW( errAENoSuchObject );
                     AEDesc tokenDesc;
                     accessorObj = new KeyPadPropAccessor(propID, (KeyPadPart *)refCon);
                     THROW_IF_ERROR( AECreateDesc(cProperty, &accessorObj,
                                                  sizeof(accessorObj), &tokenDesc) );
                     ODDesc *userODDesc = new ODDesc;
                     userODDesc->InitODDesc(ev);
                     THROW_IF_ERROR( AEDescToODDesc( &tokenDesc, userODDesc ) );
                     resolver->SetUserToken(ev, value, userODDesc);
                     if (userODDesc) {
                        delete userODDesc;
                        userODDesc=kODNULL;
                     }
                 } break;
                default:
                        THROW(errAECantSupplyType);
                        break;
        }
        CATCH_ALL
        error = ErrorCode();
        ENDTRY
        return error;
}  // GetPropertyFromNULL()

void CreatePropObjSpec(KeyPadPart*  somSelf,
                       Environment* ev,
                       ODFrame*     frame,
                       DescType     form,
                       DescType     prop,
                       AEDesc&      objSpec)
{
        AEDesc          nullDesc = {typeNull, kODNULL};
        AEDesc          id, pr;
        ODPersistentObjectID  foo;
        ODDraft*        draft = somSelf->GetStorageUnit(ev)->GetDraft(ev);
        foo = draft->GetPersistentObjectID(ev, frame, kODFrameObject);
        THROW_IF_ERROR(AECreateDesc(typeInteger, (Ptr) &foo, sizeof(foo), &id));
        THROW_IF_ERROR(AECreateDesc(typeType, (Ptr) &prop, sizeof(prop), &pr));
        TRY
        // Create object specifier for this part
        THROW_IF_ERROR(AECreateObjSpecifier(cPart, &nullDesc, form, &id, kODTrue, &objSpec));
        // Create object specifier for the text
        THROW_IF_ERROR(AECreateObjSpecifier(cProperty, &objSpec, formPropertyID,
                                            &pr, kODTrue, &objSpec));
        CATCH_ALL
        AEDisposeDesc(&id);
        AEDisposeDesc(&pr);
        RERAISE;
        ENDTRY
}  // CreatePropObjSpec()

void SendSetDataEvent(KeyPadPart*  somSelf,
                      Environment* ev,
                      AEDesc&      objSpec,
                      AEDesc&      data)
{
        ODSession*      session = somSelf->GetStorageUnit(ev)->GetSession(ev);
        ODMessageInterface*  msg = session->GetMessageInterface(ev);
        ODOSAEvent*     event;
        AEDesc          tempReply;
        ODOSAEvent*     reply = new ODOSAEvent;
        OSAEvent        tempEvent;
        ODAddressDesc*  address;
        // Create an address descriptor for this document
        msg->CreatePartAddrDesc(ev, &address, somSelf);
        // Create the OSA event
        msg->CreateEvent(ev, kAECoreSuite, kAESetData, address, kAnyTransactionID, &event);
        if (address) {
           delete address;
           address=kODNULL;
        } /* endif */
        // Add parms to event
        ODDescToAEDesc(event, &tempEvent);
        AEPutParamDesc(&tempEvent, keyDirectObject, &objSpec);
        AEPutParamDesc(&tempEvent, keyAEData, &data);
        AEDescChanged(&tempEvent, event);
        // Reply must have an AEDesc even though were not using it
        AEDescToODDesc(&tempReply, (ODDesc *) reply);
        // Send the event
        msg->Send(ev, somSelf, event, reply, kAENoReply, kAENormalPriority, kAEDefaultTimeout);
        // Delete the event
        if (event) {
           delete event;
           event=kODNULL;
        } /* endif */
} // SendSetDataEvent()

void GetDirectParam(ODSession* session,
                    OSAEvent*  message,
                    AEDesc*    evtDp)
{
        Environment*    ev = somGetGlobalEnvironment();
        AEDesc          localDP;
        THROW_IF_ERROR(AEGetParamDesc(message, keyDirectObject, typeWildCard, &localDP));
        ODNameResolver* resolver = session->GetNameResolver(ev);
        ODOSLToken*     tmpWrapper = new ODOSLToken();
        tmpWrapper->InitODOSLToken(ev);
        THROW_IF_ERROR( AEDescToODDesc(&localDP, tmpWrapper ) );
        ODBoolean isToken = resolver->IsODToken(ev, tmpWrapper);
        if (tmpWrapper) {
           delete tmpWrapper;
           tmpWrapper=kODNULL;
        } /* endif */
        if(isToken) *evtDp = localDP;
        else THROW( errAEEventNotHandled );
}  // GetDirectParam()

static ODError APIENTRY HandleSetData(ODPart*     part,
                                      ODOSAEvent* message,
                                      ODOSAEvent* reply,
                                      ODSLong     handlerRefcon)
{
        AEDesc          theData;
        ODError         error = noErr;
        Environment*    ev = somGetGlobalEnvironment();
        ODSession*      session = ((ODPart *)handlerRefcon)->GetStorageUnit(ev)->GetSession(ev);
        ODNameResolver* resolver = session->GetNameResolver(ev);
        OSAEvent        realMessage, realReply;
        THROW_IF_ERROR( ODDescToAEDesc(message, &realMessage) );
        THROW_IF_ERROR( ODDescToAEDesc(reply, &realReply ));
        ODOSLToken*     tmpWrapper = kODNULL;
        TRY
        THROW_IF_ERROR(AEGetKeyDesc(&realMessage, keyAEData,
                    typeWildCard, &theData));
        AEDesc  evtDp;
        GetDirectParam( session, &realMessage, &evtDp);
        tmpWrapper = new ODOSLToken();
        tmpWrapper->InitODOSLToken(ev);
        THROW_IF_ERROR( AEDescToODDesc(&evtDp, tmpWrapper ) );
        if(!resolver->IsODToken(ev, tmpWrapper)) THROW( errAENoSuchObject );
        ODDesc* myTokenODDesc;
        resolver->GetUserToken(ev, tmpWrapper, &myTokenODDesc);
        AEDesc theToken;
        error = ODDescToAEDesc(myTokenODDesc, &theToken );
        if (error == noErr) {
                switch (theToken.descriptorType) {
                        case typeProperty: {
                                DescType typeCode;
                                KeyPadPropAccessor* embedPropAccessorObj;
                                Size     dataSize = sizeof(embedPropAccessorObj);
                                // get prop accessor obj ptr
                                AEGetDescData(&theToken, &typeCode,
                                              (Ptr) &embedPropAccessorObj,
                                              (Size)sizeof(embedPropAccessorObj),
                                              &dataSize);
                                embedPropAccessorObj->SetData(&theData);
                                if (embedPropAccessorObj) {
                                   delete embedPropAccessorObj;
                                   embedPropAccessorObj = kODNULL;
                                } /* endif */
                                } break;
                        default:
                                break;
                }
        }
        AEDisposeDesc(&theData);
        THROW_IF_ERROR(error);
        CATCH_ALL
        error = ErrorCode();
        ENDTRY
        if (tmpWrapper) {
           delete tmpWrapper;
           tmpWrapper=kODNULL;
        } /* endif */
        return error;
}  // HandleSetData()

KeyPadPropAccessor::KeyPadPropAccessor(DescType property, KeyPadPart* part)
{
        fProperty = property;
        fPart = part;
} // SendSetDataEvent()

void KeyPadPropAccessor::SetData(AEDesc* data)
{
        Environment*    ev = somGetGlobalEnvironment();
        KeyPadPartData* somThis = KeyPadPartGetData(fPart);
        switch(fProperty) {
                case pContents: {
                        DescType typeCode;
                        AEDesc   numDesc;
                        char*    newString = (char *) SOMMalloc(_KPPartData.maxDisplayLength +1);
                        
                        Size     dataSize = sizeof(newString);
                        THROW_IF_ERROR( AECoerceDesc(data, typeChar, &numDesc) );
                        AEGetDescData(&numDesc, &typeCode, (Ptr) &newString,
                                      dataSize, &dataSize);
                        strcpy(_DisplayString, newString);
                        _fFrame->Invalidate(ev, NULL,kODNULL);
                        THROW_IF_ERROR(AEDisposeDesc(&numDesc));
                        } break;
                default:
                        THROW(errAENotModifiable);
                        break;
        }
}  // KeyPadPropAccessor::SetData

//-------------------------------------------------
// SubclassWndProc:  subclass of pushbutton to get click
//-------------------------------------------------

MRESULT EXPENTRY SubclassWndProc(HWND   hwnd,
                                 LONG   msg,
                                 MPARAM mp1,
                                 MPARAM mp2)
{
    KeyPadPart    *somSelf;

    somSelf = (KeyPadPart*)WinQueryWindowPtr(hwnd, 0);
    KeyPadPartData *somThis = KeyPadPartGetData(somSelf);

    switch (msg) {
           case WM_BUTTON1DOWN:
                if (_fAlreadyActive == kODFalse){
                    WinSendMsg(
                        WinQueryWindow(hwnd,
                                       QW_OWNER), // OpenDoc must receive this event
                        WM_BUTTON1CLICK,
                        mp1,
                        mp2);
                    return (MRESULT(0));
                }
                break;
           case WM_CHAR:                          // Trap WM-CHAR so it doesn't go to
                somSelf->                         // facet window
                  KPHandleKeyBoard(somGetGlobalEnvironment(),
                                 mp1,mp2);
                return (MRESULT(0));
           case WM_SETFOCUS:
                WinSetWindowULong(hwnd,QWL_STYLE, // Takes 'default' highlighting away from button
                                  WinQueryWindowULong(hwnd,QWL_STYLE)&~BS_DEFAULT);
                WinInvalidateRect(hwnd,
                                  (PRECTL) NULL,
                                  TRUE);          // Forces button to reddraw with new Style
                return (MRESULT(0));
         }
   return( (MRESULT)(*pButClass)(hwnd,msg,mp1,mp2) ); // Standard proc to animate button
}

