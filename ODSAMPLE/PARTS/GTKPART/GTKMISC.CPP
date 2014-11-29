
#define VARIABLE_MACROS

#ifndef _EXCEPT_
#include "Except.h"
#endif

#ifndef GraphicsPartMethodDebug
   #define GraphicsPartMethodDebug(c,m) SOMMethodDebug(c,m)
#endif

#define INCL_DOS
#define INCL_GPIBITMAPS
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES
#define INCL_GPIREGIONS
#define INCL_GPITRANSFORMS
#define INCL_GPIPATHS
#define INCL_GPILOGCOLORTABLE
#define INCL_WINACCELERATORS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINMESSAGEMGR
#define INCL_WINPOINTERS
#define INCL_WINSTDDRAG
#define INCL_WINTRACKRECT
#define INCL_WINWINDOWMGR
#define INCL_WINRECTANGLES
#define INCL_WINSYS
#define INCL_WINERRORS
#define INCL_DEV

#define INCL_ODAPI
#define INCL_ODARBITRATOR
#define INCL_ODCANVAS
#define INCL_ODCLIPBOARD
#define INCL_ODDOCUMENT
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
#define INCL_ODSESSION
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

#ifndef _FOCUSLIB_
   #include "focuslib.h"
#endif

#ifndef _WINUTILS_
   #include "winutils.h"
#endif

#ifndef __CMDDEFS__
   #include "cmddefs.xh"
#endif

#include    <Status.xh>
#include    <Info.xh>
#include    <StdExts.xh>
#include    <popup.xh>
#include    <ODViewEx.xh>

#ifndef SOM_XEmbeddedFramesIterator_xh
   #include <XFrmIter.xh>
#endif

#ifndef _TEMPOBJ_
#include "TempObj.h"
#endif

#ifndef _ORDCOLL_
#include "OrdColl.h"
#endif

#include "debug.hpp"
#include "partinfo.h"

#include "common.hpp"

#define UNDONEEDSFIXINMAYBE

#include <stdlib.h>
#include <stdio.h>
#include <builtin.h>
#include <string.h>

#include "gtkbase.h"
#include "gtkpic.h"
#include "gtkres.h"
//#include "print.h"
#include "mygpi.h"
#include "actions.h"
#include "colordlg.h"
#include "toolbar.h"
#include "platform.h"


//#define ODDebug2


const ODValueType     kODViewTypes[] = { kODViewAsSmallIcon,
                                         kODViewAsLargeIcon,
                                         kODViewAsThumbnail,
                                         kODViewAsFrame };

//------------------------------------------------------------------------------
extern MRESULT EXPENTRY PartInfoDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
void ClearMyLinkSpec( Environment* ev, ODSession* session,
                      ODFrame* frame, ODUpdateID* myID );

//------------------------------------------------------------------------------
void DisplayHelp( Environment* ev, ODSession* session,
                  ODHelp** fHelp, ODULong panelID )
{
  if (!*fHelp)
    *fHelp = session->GetHelp(ev);

  if (*fHelp) {
    if (panelID) {
      (*fHelp)->DisplayHelp(ev, HELPFILE_NAME, panelID);
    } else {
      (*fHelp)->DisplayHelpIndex(ev, HELPFILE_NAME);
    }
  }
}

//------------------------------------------------------------------------------
void SetScaling2( Environment *ev, ODFacet* facet, HPS hps )
{
  SOM_TRY

  ODBoolean dynamic = (hps == (HPS)0);
  if (dynamic) hps = facet->GetCanvas(ev)->GetPlatformCanvas(ev, kODGPI)->GetPS(ev, facet);

  MATRIXLF mtx;
  TempODTransform xform = facet->AcquireContentTransform(ev, kODNULL);
  xform->GetMATRIXLF(ev, &mtx);

  GpiSetDefaultViewMatrix( hps, 9, &mtx, TRANSFORM_REPLACE );
//  GpiSetModelTransformMatrix(hps, 0, NULL, TRANSFORM_REPLACE);
  GpiSetModelTransformMatrix( hps, 9, &mx0, TRANSFORM_REPLACE );  // 143738

  if (dynamic)
    facet->GetCanvas(ev)->GetPlatformCanvas(ev, kODGPI)->ReleasePS(ev, facet);

  SOM_CATCH_ALL
  SOM_ENDTRY
}

//------------------------------------------------------------------------------
ODFacet* ContainedFacet( Environment *ev, ODFrame* frame, ODFacet* containingFacet )
{
  ODFacet* embFacet = kODNULL;

  SOM_TRY

  ODFrameFacetIterator* facets = frame->CreateFacetIterator(ev);
  for (ODFacet* f1 = facets->First(ev);
        facets->IsNotComplete(ev); f1 = facets->Next(ev)) {
    if (f1->GetContainingFacet(ev)==containingFacet) {
      embFacet = f1;
      break;
    }
  }
  delete facets;

  SOM_CATCH_ALL
  SOM_ENDTRY

  return embFacet;
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartInvalidateRECTL( GraphicsPart *somSelf,
                                                     Environment *ev,
                                                     RECTL* prcl)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","Invalidate");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering InvalidateRECTL\n");
  #endif

  SOM_TRY

  RECTL rcl = *prcl;
  InflateRect( &rcl, 2*BOUNDS_BORDER, 2*BOUNDS_BORDER );

  ODRect rect;
  rect.left   = MAKEFIXED( rcl.xLeft,   0 );
  rect.right  = MAKEFIXED( rcl.xRight,  0 );
  rect.bottom = MAKEFIXED( rcl.yBottom, 0 );
  rect.top    = MAKEFIXED( rcl.yTop,    0 );

  TempODShape tmpShape = _fCurFrame->CreateShape(ev);
  tmpShape->SetRectangle(ev, &rect );

  ODxOrderedCollectionIterator iter1(_fDisplayFrames);
  for (ODFrame* frame = (ODFrame*)iter1.First(); iter1.IsNotComplete();
                frame = (ODFrame*)iter1.Next() ) {
    frame->Invalidate(ev, tmpShape, kODNULL);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving InvalidateRECTL\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartInvalidateObject(GraphicsPart *somSelf,
                                                     Environment *ev,
                                                     GtkObject* obj)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","Invalidate");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering InvalidateObject\n");
  #endif

  SOM_TRY

  if (!obj)
    obj = _picture->HitObject();

  RECTL rcl = obj ? obj->Bounds() : *_picture->SelectedBounds();

  somSelf->InvalidateRECTL( ev, &rcl );

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving InvalidateObject\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope Proxy*  SOMLINK GraphicsPartProxyForFrame(GraphicsPart *somSelf,
                                                      Environment *ev,
                                                     ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","ProxyForFrame");

  SOM_TRY

  if (_fEmbeddedFrames->Contains(frame))
  {
    Proxy* p;
    ODxOrderedCollectionIterator i(_fContents);
    for (p = (Proxy*) i.First(); i.IsNotComplete(); p = (Proxy*) i.Next())
      if (p->frame == frame) return p;
    return kODNULL;
  }
  else
    THROW(kODErrInvalidFrame);

  SOM_CATCH_ALL
  SOM_ENDTRY

  return kODNULL;
}

//------------------------------------------------------------------------------
SOM_Scope Proxy*  SOMLINK GraphicsPartProxyFromId(GraphicsPart *somSelf,
                                           Environment *ev, ODULong id)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","ProxyFromId");

  SOM_TRY

  ODxOrderedCollectionIterator  Iter(_fContents);
  Proxy* p;
  for ( p=(Proxy*)Iter.First(); Iter.IsNotComplete(); p=(Proxy*)Iter.Next() )
    if (p->id == id ) return p;

  SOM_CATCH_ALL
  SOM_ENDTRY

  return 0;
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartAddProxy( GraphicsPart *somSelf, Environment *ev,
                                              Proxy* proxy, ODBoolean fixup )
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","AddProxy");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering AddProxy\n");
  #endif

  SOM_TRY

  // add it to our list of embedded frames
  _fEmbeddedFrames->AddLast(proxy->frame);
  // add its proxy to our proxy list
  _fContents->AddLast(proxy);

  if (fixup) {
    ODFrame* containingFrame = (ODFrame*)_fDisplayFrames->First();
    proxy->frame->SetContainingFrame(ev, containingFrame);

    somSelf->CreateEmbeddedFacets( ev, containingFrame, proxy->frame );
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving AddProxy\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartRemoveProxy( GraphicsPart *somSelf,
                                                 Environment *ev, Proxy* proxy )
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","RemoveProxy");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering RemoveProxy\n");
  #endif

  // add it to our list of embedded frames
  _fEmbeddedFrames->AddLast(proxy->frame);
  // add its proxy to our proxy list
  _fContents->AddLast(proxy);

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving RemoveProxy\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartClipEmbeddedFrames(GraphicsPart *somSelf,
                                            Environment *ev,
                                           ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","ClipEmbeddedFrames");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering ClipEmbeddedFrames\n");
  #endif

  SOM_TRY

  ODFrameFacetIterator* facets = frame->CreateFacetIterator(ev);
  for (ODFacet* facet = facets->First(ev); facets->IsNotComplete(ev); facet = facets->Next(ev))
//          somSelf->ClipEmbeddedFacets(ev, facet);
     somSelf->ClipEmbeds(ev, facet, (RECTL*)0 );
  delete facets;

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving ClipEmbeddedFrames\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope GtkPic*  SOMLINK GraphicsPartGetPicture(GraphicsPart *somSelf,
                                                         Environment *ev)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","GetPicture");

  return _picture;
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartHighlightSelection(GraphicsPart *somSelf,
                                                        Environment *ev,
                                                       ODFacet* facet,
                                                       ODShape* invalShape)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","HighlightSelection");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering HighlightSelection\n");
  #endif

  SOM_TRY

  ODBoolean fsAcquired = kODFalse;

  if (_picture->SelectedCount()) {

    // fix for selection marks painting problems? (Ctl+Click embed select)
    if (facet && !invalShape) {
      fsAcquired = kODTrue;
      invalShape = facet->GetFrame(ev)->AcquireFrameShape(ev, kODNULL);
    }

    if (facet) {

      HPS hpsDraw;
      CFocus f(ev, facet, invalShape, &hpsDraw);

      RGB_HPS(hpsDraw);                                  // set hps to RGB mode!
      _picture->DrawMarks( hpsDraw, _fAltMark );

      #ifndef ODHPS
        GpiRestorePS( hpsDraw, -1 );    // lPS );
      #endif

    } else {
      ODxOrderedCollectionIterator iter(_fDisplayFrames);
      ODFrame* frame;
      for (frame = (ODFrame*)iter.First(); iter.IsNotComplete();
            frame = (ODFrame*)iter.Next() ) {
        facet = FirstFacet(ev, frame);
        if (facet)
          somSelf->HighlightSelection( ev, facet, invalShape );
      }
    }

    if (fsAcquired)
      ODReleaseObject(ev, invalShape);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving HighlightSelection\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartClipEmbeds( GraphicsPart *somSelf,
                                                Environment *ev,
                                                ODFacet* facet,
                                                RECTL* rcl)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","ClipEmbeds");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering ClipEmbeds\n");
  #endif

  SOM_TRY

  // working a particular gtkpart facet?
  if (facet) {

    ODTransform* xform;

    // update clipshape for each embed
    _picture->ClipEmbeds( _fsmHps, facet, rcl );

    // cached tmp shape!
    TempODShape embedShape = facet->GetFrame(ev)->CreateShape(ev);
    embedShape->Reset(ev);

    // net embed shape for background clipping.  collect in WINDOW coords
    ODShape* contentClip = (ODShape*)facet->GetPartInfo(ev);

    contentClip->Reset(ev);

    // iterate over all embeds
    ODFacet* embFacet;
    ODFacetIterator* facets = facet->CreateFacetIterator(ev, kODChildrenOnly, kODFrontToBack);
    for (embFacet = facets->First(ev); facets->IsNotComplete(ev); embFacet = facets->Next(ev)) {

      // get the embed's associated graphics object
      Proxy* p = somSelf->ProxyForFrame(ev, embFacet->GetFrame(ev));
      GtkEmbed* embedObj = (GtkEmbed*)_picture->Object(p->id);

      // if we got it and it is an embed that should be clipped off...
      if (embedObj && embedObj->IsEmbed() && !embedObj->IsTranslucent()) {

        // get a copy of the embed's usedShape
        TempODShape tmpShape = embFacet->GetFrame(ev)->AcquireUsedShape(ev, kODNULL);
        embedShape->CopyFrom(ev, tmpShape);

        // convert to window coords
        xform = embFacet->AcquireWindowFrameTransform(ev, kODNULL);
        embedShape->Transform(ev, xform);
        ODReleaseObject(ev, xform);

        // add it to aggregate
        contentClip->Union(ev, embedShape);
      }
    }
    delete facets;

    // take aggregate clip shape back into MY frame coords
    TempODTransform xform2 = facet->AcquireWindowFrameTransform(ev, kODNULL);
    xform = xform2->Copy(ev)->Invert(ev);
    contentClip->Transform(ev, xform);
    ODReleaseObject(ev, xform);

  // or iterating over all my frames & facets?!!
  } else {

    ODxOrderedCollectionIterator iter(_fDisplayFrames);
    ODFrame* frame;
    for (frame = (ODFrame*)iter.First(); iter.IsNotComplete();
          frame = (ODFrame*)iter.Next() ) {

      somSelf->ClipEmbeddedFrames( ev, frame );
    }
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving ClipEmbeds\n");
  #endif
}

//------------------------------------------------------------------------------
void LogUndoAction( GraphicsPart *somSelf, Environment *ev,
                    PVOID buf, ULONG bufSize,
                    ODSession * fSession, PSZ uTxt)
{
  // create undo structure for this action
  char undoTxt[80] = "~Undo ";
  char redoTxt[80] = "~Redo ";
  ODIText*  undoActionName;
  ODIText*  redoActionName;

  strcat( undoTxt, uTxt );
  strcat( redoTxt, uTxt );
  undoActionName = CreateITextCString(0, 0, undoTxt);
  redoActionName = CreateITextCString(0, 0, redoTxt);

  ODActionData *undoBuf  = (ODActionData *)CreateByteArray(buf, bufSize);
  ODUndo * theUndo =      fSession->GetUndo(ev);
  theUndo->AddActionToHistory(ev, somSelf, undoBuf,
                              kODSingleAction,
                              (ODName*)undoActionName,
                              (ODName*)redoActionName);
  DisposeByteArray(undoBuf);  // remove byte array
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartPartUpdated( GraphicsPart *somSelf,
                                                 Environment *ev,
                                                 ODFrame* frame, ODULong flags)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","PartUpdated");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering PartUpdated\n");
  #endif

  SOM_TRY

  if (_fReadOnly) {
    ALARM();
    somSelf->DisplayResourceText(ev, 295, _fCurFrame );
    DosSleep(1000);
    return;
  } else {
    somSelf->GetStorageUnit(ev)->GetDraft(ev)->SetChangedFromPrev(ev);
  }

  _fNeedToExternalize = kODTrue;

  // delete invalid thumbnail
  if (_hbmThumbnail) {
    GpiDeleteBitmap(_hbmThumbnail);
    _hbmThumbnail = NULL;
  }

  if ((flags&PUF_CLEARUNDO) != 0) {
    // clear the undo actions list
    ODUndo * theUndo = _fSession->GetUndo(ev);
    theUndo->ClearActionHistory( ev, kODRespectMarks );
  }

  // lets try and determine if we REALLY need to clip embeds!
  if ((flags&PUF_CLIPEMBEDS) != 0) {
    // problems here - even if there are no embeds in the selection, the
    // selection HANDLES may lie over an embed and its clipping need to be adjusted.
    #if 0
     // check out the selection set
     ULONG ulObjFlags = 0, ulStateFlags;
     _picture->ClassifySelection( &ulObjFlags, &ulStateFlags );

     // is our frame inactive OR are there any embeds in the "action set"??
     if (!FrameIsActive(ev, _fCurFrame) || (ulObjFlags&GOT_EMBED) != 0)
    #endif
      somSelf->ClipEmbeds( ev, 0, 0 );
  }

  if ((flags&PUF_REPAINT) != 0) {
    somSelf->InvalidateObject( ev, 0 );
  }

  if ((flags&PUF_REPAINTALL) != 0) {
    ODxOrderedCollectionIterator aIter(_fDisplayFrames);
    for (frame = (ODFrame*)aIter.First(); aIter.IsNotComplete();
        frame = (ODFrame*)aIter.Next() )
      frame->Invalidate(ev, NULL, kODNULL);
  }

  if ((flags&PUF_RESETFSM) != 0) {
//    somSelf->fsm_execute( ev, FSA_RESET, NULL );
    somSelf->fsm_set_state( ev, 0, 0 );
    somSelf->UpdatePalette(ev, _fCurFrame);
  }

  if ((flags&PUF_UPDATELINKS) != 0) {
    _picture->UpdateLinks(ev, somSelf);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving PartUpdated\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartCreateThumbnail( GraphicsPart *somSelf,
                                                     Environment *ev, ODFrame* frame )
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","CreateThumbnail");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering CreateThumbnail\n");
  #endif

  SOM_TRY

  // delete old
  if (_hbmThumbnail) {
    GpiDeleteBitmap(_hbmThumbnail);
    _hbmThumbnail = NULL;
  }

  // create the thumbnail hps/bitmap
  RECTL rclBitmap = {0,0,THUMBSIZE,THUMBSIZE};
  HPS hps = CreateHps( THUMBSIZE, THUMBSIZE, HPS_BITMAP );
  HDC hdc;

  // render to it
  #if 1
    #if 1
      RGB_HPS(hps);                                  // set hps to RGB mode!
      GpiSetMix( hps, FM_DEFAULT );
      GpiSetPattern( hps, PATSYM_SOLID );

      RECTL rclPic;
      _picture->QueryBounds(&rclPic);

      FillRect( hps, &rclPic, _fBGColor );
    #endif

    _picture->Draw( hps, &rclBitmap, (ODFacet*)0 );
  #else

    // create a copy frame and "attach" it
    ODShape* frameShape = ODCopyAndRelease(ev, frame->AcquireFrameShape( ev, kODNULL )); // [121801]
    ODFrame* newFrame = frame->GetStorageUnit(ev)->GetDraft(ev)->
                                    CreateFrame(ev, kODNonPersistentFrameObject,
                                                kODNULL, frameShape, kODNULL,
                                                _fPartWrapper, FRAME_VIEW,
                                                PRES_DEFAULT, kODTrue, kODFalse);

    TempODPart tempPart = newFrame->AcquirePart(ev);
    tempPart->AttachSourceFrame(ev, newFrame, frame);

    // create a facet for this frame
    TempODShape clipshape = frame->CreateShape(ev);
    ODRect bounds = { 0, MAKEFIXED(THUMBSIZE,0), MAKEFIXED(THUMBSIZE,0), 0 };
    clipshape->SetRectangle(ev, &bounds);

    // create a copy facet mirroring display
    TempODTransform xform = frame->CreateTransform(ev);
    TempODFacet newFacet = _fSession->GetWindowState(ev)->
                 CreateFacet(ev, newFrame, clipshape, xform, kODNULL, kODNULL);

    // create a canvas for this new facet using the bitmap hps
    ODPlatformCanvas* platformCanvas = newFacet->CreatePlatformCanvas(ev, hps);
    ODCanvas* newCanvas = newFacet->CreateCanvas(ev, kODGPI, platformCanvas,
                                                 kODFalse, kODFalse);

//    prCanvas->SetPlatformPrintJob(ev, kODGPI, &pPI->printDest);

    // finish connecting things up
    newFacet->ChangeCanvas(ev, newCanvas);
    newFrame->FacetAdded(ev, newFacet);

    // set up scaling and clipping
    TempODTransform xform2 = newFacet->CreateTransform(ev);
//    ODPoint      scale  = { MAKEFIXED(0,65536/8), MAKEFIXED(0,65536/8) };
//    ODPoint      scale  = { MAKEFIXED(1,0), MAKEFIXED(1,0) };
    ODPoint      scale  = { 65536/4, 65536/4 };
    xform2->ScaleBy(ev, &scale);
    newFacet->ChangeGeometry(ev, kODNULL, xform2, kODNULL);

    // trigger rendering
    TempODShape invalShape = frameShape->Copy(ev);
    newFacet->Update(ev, invalShape, kODNULL);

    newFrame->FacetRemoved(ev, newFacet);
    delete newCanvas;

    newFrame->Remove(ev);
  #endif

  // get the bitmap handle and keep it
  _hbmThumbnail = GpiSetBitmap( hps, NULL );

  // clean-up time!
  hdc = GpiQueryDevice(hps);
  GpiDestroyPS( hps );
  DevCloseDC( hdc );

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving CreateThumbnail\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartHandleMenuEvent( GraphicsPart *somSelf,
                                                          Environment *ev,
                                                          ODFrame* focusFrame,
                                                          ODEventData* event)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","HandleMenuEvent");

  ODBoolean   handled = kODFalse;
  ODCommandID command = LONGFROMMP(event->mp1);

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering HandleMenuEvent: %d\n", command);
  #endif

  SOM_TRY

  ODFacet* facet = FirstFacet(ev, focusFrame);

  HWND hwndFrame = facet->GetWindow(ev)->GetPlatformWindow(ev);
  HWND hwndFacet = WinWindowFromID( hwndFrame, FID_CLIENT );
//  HWND hwndFacet = facet ? facet->GetFacetHWND(ev) : 0;

  char str255[255];

  switch (command) {

  // some keyboard handling entries ++++++++++++++++++++++

    case IDC_ESC :
      if ( focusFrame->GetViewType(ev) == FRAME_VIEW )
        handled = somSelf->fsm_execute( ev, FSA_RESET, NULL );
      break;

    case IDC_CONTEXTMENU :
      if ( !_fsmState ) {
        POINTL ptl = CursorPosition( ev, facet );
        somSelf->DoPopup(ev, facet, &ptl );
        handled = kODTrue;
      }
      break;

    case IDC_ENTER :
      if ( focusFrame->GetViewType(ev) == FRAME_VIEW ) {
        POINTL ptl = CursorPosition( ev, facet );
        RECTL  rcl;
        WinQueryWindowRect( hwndFacet, &rcl);
        if (WinPtInRect((HAB)0, &rcl, &ptl)) {
          GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &ptl );
          handled = somSelf->fsm_execute( ev, FSA_ENTER, &ptl );
        } /* endif */
      }
      break;

    case IDC_UP    :
    case IDC_DOWN  :
    case IDC_LEFT  :
    case IDC_RIGHT :
      if ( focusFrame->GetViewType(ev) == FRAME_VIEW &&
           _picture->SelectedCount() ) {

        handled = kODTrue;

        RECTL rclSelection;
        rclSelection = *_picture->SelectedBounds();
        ULONG delta = 1, dx, dy;
        POINTL ptl1, ptl2;

        // we have to do this loop 'cause on high res machines adjacent
        // pels can have the same world coords... this points to a problem
        // in our internal scaling - 72 ppi is not fine enough.
        do {

          // get selection lower-left point in screen coords
          ptl1.x = rclSelection.xLeft;
          ptl1.y = rclSelection.yBottom;
          GpiConvert( _fsmHps, CVTC_WORLD, CVTC_DEVICE, 1, &ptl1 );

          // figure out shift-point in pels
          ptl2 = ptl1;
          switch (command) {
            case IDC_UP    : ptl2.y += delta; break;
            case IDC_DOWN  : ptl2.y -= delta; break;
            case IDC_LEFT  : ptl2.x -= delta; break;
            case IDC_RIGHT : ptl2.x += delta; break;
          }

          // convert 'em both to world coords
          GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &ptl1 );
          GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &ptl2 );

          // difference is the Move shift
          dx = ptl2.x - ptl1.x;
          dy = ptl2.y - ptl1.y;
          delta += 1;

        } while (!dx && !dy);

        if (dx || dy) {

          // prep MOVE undo structure (will fail if no moveable selection)
          UndoMove* um = new UndoMove( dx, dy );
          if ( !_picture->CreateUndoStruct( UNDOCLS_MOVE, (PVOID*)&um ) ) {
            delete um;
            break;
          }

          // log the UNDO op
          LogUndoAction( somSelf, ev, um, sizeof(*um), _fSession, "move" );

          // do the Move op (don't need to pre-invalidate for one-pel moves!)
          _picture->Move( dx, dy );

          // mark dirty and update everything
          somSelf->PartUpdated( ev, focusFrame, PUF_CLIPEMBEDS | PUF_REPAINT | PUF_UPDATELINKS );
        }
      }
      break;

    case IDC_NEAREST_PT :
    case IDC_NEAREST_ENDPT :
    case IDC_NEAREST_CENTER :
      if ( focusFrame->GetViewType(ev) == FRAME_VIEW ) {
        POINTL ptl = CursorPosition( ev, facet );
        RECTL  rcl;
        WinQueryWindowRect( hwndFacet, &rcl);
        if (WinPtInRect((HAB)0, &rcl, &ptl)) {
          GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &ptl );
          if ( _picture->FindPoint( &ptl, (command-IDC_NEAREST_PT) ) ) {
            put_pointer( hwndFacet, _fsmHps, &ptl );
            somSelf->fsm_execute( ev, FSA_MOVE, &ptl );
            handled = somSelf->fsm_execute( ev, FSA_ENTER, &ptl );
          }
        } /* endif */
      }
      break;

  // EDIT menu options ++++++++++++++++++++++

    case EDIT_DELETE     :
    case IDM_POPUP_DELETE :
      handled = somSelf->DoClear(ev, focusFrame);
      break;

    case EDIT_COPY :
      handled = somSelf->DoCopy(ev, focusFrame);
      break;

    case EDIT_CUT :
      handled = somSelf->DoCut(ev, focusFrame);
      break;

    case EDIT_PASTE :
    case EDIT_PASTELINK :
    case EDIT_PASTEAS :
      {
      ODBoolean keyboardused = (SHORT1FROMMP(event->mp2) & CMDSRC_ACCELERATOR) != 0;
      handled = somSelf->DoPaste(ev, focusFrame, command, keyboardused);
      }
      break;

    case EDIT_BREAKLINK :
      if (_picture->SelectedCount()) {
        _picture->BreakLinks(ev);                   // BREAKLINK from Selected objs
        somSelf->PartUpdated( ev, focusFrame, PUF_REPAINT );
        handled = kODTrue;
      }
      break;

    case EDIT_SELECTALL :
    case EDIT_DESELECTALL :
      if ( _picture->ObjectCount()  && !_fsmState &&
           focusFrame->GetViewType(ev) == FRAME_VIEW ) {
        somSelf->HighlightSelection(ev, facet, 0);
        _fAltMark = FALSE;
        if (command == EDIT_SELECTALL) {
          _picture->SelectAll();
          somSelf->HighlightSelection(ev, facet, 0);
        } else _picture->Select();
        somSelf->ClipEmbeds( ev, facet, 0 );
        handled = kODTrue;
      }
      break;

  // VIEW menu options ++++++++++++++++++++++

    case VIEW_REFRESH :
      focusFrame->Invalidate(ev, kODNULL , kODNULL );
      handled = kODTrue;
      break;

    case VIEW_BGNDMODE_COLOR :
    case VIEW_BGNDMODE_TILED :
    case VIEW_BGNDMODE_BLTFIT :
      if (_fBGMode != command - VIEW_BGNDMODE_COLOR) {

        _fMenuBar->CheckMenuItem( ev, IDMS_VIEW, VIEW_BGNDMODE_COLOR+_fBGMode, kODFalse );
        _fBGMode = command - VIEW_BGNDMODE_COLOR;
        _fMenuBar->CheckMenuItem( ev, IDMS_VIEW, VIEW_BGNDMODE_COLOR+_fBGMode, kODTrue );

        focusFrame->Invalidate(ev, kODNULL , kODNULL );
      }
      handled = kODTrue;
      break;

    case VIEW_OATREE:
    case VIEW_OAICON:
    case VIEW_OADETAILS:
    case DOC_OATREE:
    case DOC_OAICON:
    case DOC_OADETAILS:
      {
        // init the View Extension if necessary
        if (!_fViewExtension) {
          _fViewExtension = new ODViewExtension;
          _fViewExtension->InitViewExtension( ev, somSelf );
        }

        // select the View Type
        ODULong vType;
        if ((command == VIEW_OATREE) || (command == DOC_OATREE))
          vType = OD_TREEVIEW;
        else if ((command == VIEW_OAICON) || (command == DOC_OAICON))
          vType = OD_ICONVIEW;
        else
          vType = OD_DETAILSVIEW;

        // Open the View
        _fViewExtension->DisplayView( ev, somSelf, vType );
      }
      handled = kODTrue;
      break;

#if 1
    case VIEW_OAWINDOW:
      somSelf->Open( ev, kODNULL );
      handled = kODTrue;
      break;
#endif

    case VIEW_PROPERTIES:
      _fSession->GetInfo(ev)->ShowPartFrameInfo( ev, facet, kODFalse );
      handled = kODTrue;
      break;

//    case VIEW_SALARGEICON:
//    case VIEW_SASMALLICON:
//    case VIEW_SATHUMBNAIL:
//    case VIEW_SAFRAME:
    case EDIT_SEL_SASMALLICON:
    case EDIT_SEL_SALARGEICON:
    case EDIT_SEL_SATHUMBNAIL:
    case EDIT_SEL_SAFRAME:
      if (_picture->SelectedCount()) {
        GtkObject* obj;
//        ODTypeToken viewtype = _fSession->Tokenize(ev, kODViewTypes[command-VIEW_SALARGEICON] );
        ODTypeToken viewtype = _fSession->Tokenize(ev, kODViewTypes[command-EDIT_SEL_SASMALLICON] );
        ODxOrderedCollectionIterator iter(_fContents);
        for ( Proxy* p = (Proxy*)iter.First(); iter.IsNotComplete();
                     p = (Proxy*)iter.Next()) {

          obj = _picture->Object(p->id);

          if (obj && _picture->IsSelected(obj) ) {
            p->frame->ChangeViewType(ev, viewtype );
            TempODShape frameShape = ODCopyAndRelease(ev, p->frame->AcquireFrameShape(ev, kODNULL));
            p->frame->Invalidate(ev, frameShape, kODNULL );
          }
        }
        handled = kODTrue;
      }
      break;

    case VIEW_SHOWLINKS:
      _fLinksVisible = !_fLinksVisible;
      _picture->ShowLinks(_fLinksVisible);
      focusFrame->Invalidate( ev, kODNULL, kODNULL );
      handled = kODTrue;
      break;

    case VIEW_SHOWTOOLS:
      _fToolsVisible = !_fToolsVisible;
      somSelf->UpdatePalette(ev, focusFrame);

      _fMenuBar->SetMenuItemText(ev, IDMS_VIEW, VIEW_SHOWTOOLS,
                                 _fToolsVisible ? "Hide ~toolbar" : "Show ~toolbar" );
      SetStatusTextFromResource( VIEW_SHOWTOOLS, _fToolsVisible ? 262 : 261 );

      handled = kODTrue;
      break;

    case VIEW_BGND_COLOR:
      {
        char szBuffer[255];
        WinLoadString( (HAB)0, _hmod, 301, 255, szBuffer );
        ODIText*  dlgTitle = CreateITextCString( 0, 0, szBuffer );

        // user selects new background color?
        ULONG  rgb = _fBGColor;
//        if ( SelectColor( &rgb, "Graphics background color", hwndFacet ) ) {
        if ( ODSelectColor( &rgb, dlgTitle, focusFrame ) ) {

          // set up the UNDO/REDO stuff
          UndoBgndColor* undoRec = new UndoBgndColor(focusFrame, _fBGColor, rgb);
          LogUndoAction( somSelf, ev, undoRec, sizeof(*undoRec), _fSession,
                         "background color");

          // make the change (and propagate it)
          somSelf->SetBGColor(ev, focusFrame, rgb, kODTrue);
        }
        DisposeIText( dlgTitle );
        handled = kODTrue;
      }
      break;

  // SELECTED menu items  ++++++++++++++++++++++

    case SELECTED_ALTMARK :
      somSelf->HighlightSelection(ev, facet, 0);
      _fAltMark = !_fAltMark;
      // if altmarked, deselect any locked objects
      if (_fAltMark)
        _picture->Select( (GtkObject*)0, SELECT_TOGGLEONE );
      somSelf->HighlightSelection(ev, facet, 0);
      handled = kODTrue;
      break;

    case SELECTED_ROTATE :
    case IDM_POPUP_ROTATE :
      if ( !_fsmState && (_picture->SelectedCount() || _picture->HitObject()) ) {
        _fsmLastType = _fsmType;
        somSelf->fsm_set_state( ev, ACTION_ROTATE, 0 );
        handled = somSelf->fsm_execute( ev, FSA_ENTER, NULL );
      }
      break;

    case SELECTED_LOCK:
    case SELECTED_UNLOCK:
      {
      ODBoolean fLock = command == SELECTED_LOCK;

      // prep undostuff
      PVOID buf;
      ULONG bufSize;
      bufSize = _picture->CreateUndoStruct( fLock ? UNDOCLS_LOCK
                                                  : UNDOCLS_UNLOCK, &buf);
      // try the edit operation.
      if (_picture->Lock(fLock)) {

        // register undostuff
        LogUndoAction( somSelf, ev, buf, bufSize, _fSession,
                       fLock ? "lock" : "unlock" );

        somSelf->PartUpdated( ev, focusFrame, 0 );

        // fix for 131674
        if (_fAltMark) {
          somSelf->HighlightSelection(ev, facet, 0);
          _fAltMark = FALSE;
          somSelf->HighlightSelection(ev, facet, 0);
        }

      // no operation performed, tidy up
      } else {
        _picture->DestroyUndoStruct(buf);
      }
      handled = kODTrue;
      break;
      }

    case SELECTED_GROUP:
    case SELECTED_UNGROUP:
      {
      ODBoolean fGroup = command == SELECTED_GROUP;
      // un-highlight
      somSelf->HighlightSelection(ev, facet, 0);

      // prep undostuff
//      PVOID buf;
//      ULONG bufSize;
//      bufSize = _picture->CreateUndoStruct( fGroup ? UNDOCLS_GROUP
//                                                   : UNDOCLS_UNGROUP, &buf);
      // try the edit operation.
      if (_picture->Group(fGroup)) {

        // register undostuff
//        LogUndoAction( somSelf, ev, buf, bufSize, _fSession,
//                       fLock ? "lock" : "unlock" );

        somSelf->PartUpdated( ev, focusFrame, PUF_CLEARUNDO );

      // no operation performed, tidy up
      } else {
        // free up undostuff
//        _picture->DestroyUndoStruct(buf);
      }
      // re-highlight
      somSelf->HighlightSelection(ev, facet, 0);
      handled = kODTrue;
      break;
      }

    case SELECTED_OPAQUE:
    case SELECTED_TRANSLUCENT:
      if (_picture->SelectedCount()) {
        long aVal = (long)(command == SELECTED_TRANSLUCENT);

         // prep undostuff
         UndoAttributes *uas = new UndoAttributes( TRANSLUCENCE, aVal );
         _picture->CreateUndoStruct( uas->uclass, (PVOID*)&uas );

        if (_picture->SetAttribute( TRANSLUCENCE, aVal ) ) {

          // register undostuff
          LogUndoAction( somSelf, ev, uas, sizeof(*uas), _fSession, "embed background" );

          somSelf->PartUpdated( ev, focusFrame, PUF_REPAINT | PUF_CLIPEMBEDS | PUF_UPDATELINKS );

        // no operation performed, tidy up
        } else {
          // free up undostuff
          _picture->DestroyUndoStruct(uas);
        }
        handled = kODTrue;
      }
      break;

    case SELECTED_MOVETOFRONT :
    case SELECTED_MOVETOBACK  :
      {
        long aVal = command==SELECTED_MOVETOFRONT ? Z_ORDER_TOP : Z_ORDER_BOTTOM;

        _picture->Zorder(aVal);

        somSelf->ClipEmbeds( ev, facet, _picture->SelectedBounds() );

        somSelf->PartUpdated( ev, focusFrame, PUF_CLEARUNDO | PUF_REPAINT | PUF_UPDATELINKS );
      }
      handled = kODTrue;
      break;

  // TOOLBAR command items  ++++++++++++++++++++++

    case IDM_TOOL_POINTER :
    case IDM_TOOL_LINE    :
    case IDM_TOOL_POLYLINE:
    case IDM_TOOL_RECT    :
    case IDM_TOOL_BOX     :
    case IDM_TOOL_ELLIPSE :
    case IDM_TOOL_ARC     :
      if (somSelf->fsm_set_state( ev, (command-IDM_TOOL_POINTER), 0 ) ) {
        somSelf->UpdatePalette(ev, focusFrame);
        somSelf->DisplayResourceText(ev, command, focusFrame );
        handled = kODTrue;
      }
      break;

  // HELP menu items  ++++++++++++++++++++++

    case HELP_GENERAL:
      DisplayHelp( ev, _fSession, &_fHelp, GTK_GENERAL_HELP );
      handled = kODTrue;
      break;

    case HELP_INDEX:
      DisplayHelp( ev, _fSession, &_fHelp, 0 );
      handled = kODTrue;
      break;

    case HELP_PARTINFO:
      {
        WinDlgBox( HWND_DESKTOP, hwndFrame, PartInfoDlgProc, _hmod, ID_PARTINFODLG, 0 );
        handled = kODTrue;
      }
      break;

  // OBJECT ATTRIBUTE menu items  ++++++++++++++++++++++

    case IDM_LINE_COLOR:
    case IDM_FILL_COLOR:
      {
        ULONG    aType = (command==IDM_LINE_COLOR ? LS_COLOR : FS_COLOR);

        char szBuffer[255];
        WinLoadString( (HAB)0, _hmod, aType==LS_COLOR ? 302 : 303, 255, szBuffer );
        ODIText*  dlgTitle = CreateITextCString( 0, 0, szBuffer );

//        PCHAR    szTitle = aType==LS_COLOR ? "Object line/outline color"
//                                           : "Object fill color";

        GtkObject* obj = _picture->HitObject();

        ULONG    rgb = 0;
//        if ( SelectColor( &rgb, szTitle, hwndFacet ) ) {
        if ( ODSelectColor( &rgb, dlgTitle, focusFrame ) ) {

          obj = _picture->HitObject();

          // prep undostuff
          UndoAttributes *uas = new UndoAttributes( aType, (LONG)rgb );
          _picture->CreateUndoStruct( uas->uclass, (PVOID*)&uas );

          // try color change
          if (_picture->SetAttribute( aType, (LONG)rgb )) {

            // register undostuff
            LogUndoAction( somSelf, ev, uas, sizeof(*uas), _fSession,
                           command==IDM_LINE_COLOR ? "line color" : "fill color" );

            somSelf->PartUpdated( ev, focusFrame, PUF_REPAINT | PUF_UPDATELINKS );

          // no operation performed, tidy up
          } else {
            // free up undostuff
            _picture->DestroyUndoStruct(uas);
          }
        }
        DisposeIText( dlgTitle );
      }
      handled = kODTrue;
      break;

    case IDM_LW_THIN:
    case IDM_LW_THICK:
      {
        long aVal = command == IDM_LW_THIN ? LINEWIDTH_NORMAL : LINEWIDTH_THICK;

         // prep undostuff
         UndoAttributes *uas = new UndoAttributes( LS_WIDTH, aVal );
         _picture->CreateUndoStruct( uas->uclass, (PVOID*)&uas );

//        somSelf->InvalidateObject( ev, 0 );

        if (_picture->SetAttribute( LS_WIDTH, aVal ) ) {

          // register undostuff
          LogUndoAction( somSelf, ev, uas, sizeof(*uas), _fSession, "line width" );

          somSelf->PartUpdated( ev, focusFrame, PUF_REPAINT | PUF_CLIPEMBEDS | PUF_UPDATELINKS );

        // no operation performed, tidy up
        } else {
          // free up undostuff
          _picture->DestroyUndoStruct(uas);
        }
      }
      handled = kODTrue;
      break;

    default:
      // FILL STYLE changed?
      if (command>=IDM_FS_NONE && command<=IDM_FS_DIAG4) {
        static LONG fills[] = {15, 16, 17, 1 , 10, 9 , 18, 19, 12, 14 };
        long aVal = fills[command-IDM_FS_NONE];

        // prep undostuff
        UndoAttributes *uas = new UndoAttributes( FS_STYLE, aVal );
        _picture->CreateUndoStruct( uas->uclass, (PVOID*)&uas );

        somSelf->InvalidateObject( ev, 0 );
        if (_picture->SetAttribute( FS_STYLE, aVal )) {

          // register undostuff
          LogUndoAction( somSelf, ev, uas, sizeof(*uas), _fSession, "fill style" );

          somSelf->PartUpdated( ev, focusFrame, PUF_REPAINT | PUF_CLIPEMBEDS | PUF_UPDATELINKS );

        // no operation performed, tidy up
        } else {
          // free up undostuff
          _picture->DestroyUndoStruct(uas);
        }

      // SHADOW STYLE changed?
      } else if (command>=IDM_SH_NONE && command<=IDM_SH_DIAG4) {
        static LONG fills[] = {15, 16, 17, 1 , 10, 9 , 18, 19, 12, 14 };
        long aVal = fills[command-IDM_SH_NONE];

        // prep undostuff
        UndoAttributes *uas = new UndoAttributes( SHADOW_STYLE, aVal );
        _picture->CreateUndoStruct( uas->uclass, (PVOID*)&uas );

        somSelf->InvalidateObject( ev, 0 );
        if (_picture->SetAttribute( SHADOW_STYLE, aVal )) {

          // register undostuff
          LogUndoAction( somSelf, ev, uas, sizeof(*uas), _fSession, "shadow style" );

          somSelf->PartUpdated( ev, focusFrame, PUF_REPAINT | PUF_CLIPEMBEDS | PUF_UPDATELINKS );

        // no operation performed, tidy up
        } else {
          // free up undostuff
          _picture->DestroyUndoStruct(uas);
        }

      // LINE STYLE changed?
      } else if (command>=IDM_LS_DOT && command<=IDM_LS_INVISIBLE) {
        long aVal = command-IDM_LS_DOT+1;

        // prep undostuff
        UndoAttributes *uas = new UndoAttributes( LS_STYLE, aVal );
        _picture->CreateUndoStruct( uas->uclass, (PVOID*)&uas );

        somSelf->InvalidateObject( ev, 0 );
        if (_picture->SetAttribute( LS_STYLE, aVal )) {

          // register undostuff
          LogUndoAction( somSelf, ev, uas, sizeof(*uas), _fSession, "line style" );

          somSelf->PartUpdated( ev, focusFrame, PUF_REPAINT | PUF_CLIPEMBEDS | PUF_UPDATELINKS );

        // no operation performed, tidy up
        } else {
          // free up undostuff
          _picture->DestroyUndoStruct(uas);
        }

      // ARC CLOSING changed?
      } else if (command>=IDM_CLOSE_NONE && command<=IDM_CLOSE_CHORD) {
        long aVal = command-IDM_CLOSE_NONE;

        // prep undostuff
        UndoAttributes *uas = new UndoAttributes( ARC_CLOSING, aVal );
        _picture->CreateUndoStruct( uas->uclass, (PVOID*)&uas );

        somSelf->InvalidateObject( ev, 0 );
        if (_picture->SetAttribute( ARC_CLOSING, aVal )) {

          // register undostuff
          LogUndoAction( somSelf, ev, uas, sizeof(*uas), _fSession, "arc closing" );

          somSelf->PartUpdated( ev, focusFrame, PUF_REPAINT | PUF_CLIPEMBEDS | PUF_UPDATELINKS );

        // no operation performed, tidy up
        } else {
          // free up undostuff
          _picture->DestroyUndoStruct(uas);
        }
      }
      else break;          // THIS KEEPS US FROM RETURNING TRUE!!!

      handled = kODTrue;
      break;
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving HandleMenuEvent\n");
  #endif

  return handled;
}

//------------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartDoCut(GraphicsPart *somSelf,  Environment *ev,
                                   ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DoCut");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DoCut\n");
  #endif

  ODBoolean rc = kODFalse;

  SOM_TRY

  if ( _picture->SelectedCount() && somSelf->DoCopy( ev, frame ) ) {

    // if we put up a link spec, remove it as we are cutting the content
    if (_fClipboardChangeID)
      ClearMyLinkSpec( ev, _fSession, frame, &_fClipboardChangeID );

    rc = somSelf->DoClear( ev, frame );
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DoCut\n");
  #endif

  return rc;
}

//------------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartDoCopy( GraphicsPart *somSelf,
                                               Environment *ev, ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DoCopy");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DoCopy\n");
  #endif

  ODBoolean result = kODTrue;

  SOM_TRY

  ODArbitrator *arbitrator = _fSession->GetArbitrator( ev );
  ODTypeToken clipboardFocus = _fSession->Tokenize( ev, kODClipboardFocus );

  if ( _picture->SelectedCount() &&
       arbitrator->RequestFocus( ev, clipboardFocus, frame ) ) {

    SetSysPointer( SPTR_WAIT );                           // show we're busy!

    ODClipboard*  clipboard = _fSession->GetClipboard(ev);
    ODDraft*      fromDraft = somSelf->GetStorageUnit(ev)->GetDraft(ev);

    clipboard->Clear( ev );
    _fClipboardChangeID = 0;                 // we have no linkspec up now

    ODStorageUnit* clipSU = clipboard->GetContentStorageUnit(ev);
    ODDraft*       toDraft = clipSU->GetDraft(ev);

    // first, determine if there is a single selected frame to Copy
    ODFrame* selectedFrame = 0;
    GtkObject* obj = _picture->HitObject();

    if (obj && obj->IsEmbed()) {
      selectedFrame = somSelf->ProxyFromId( ev, obj->Id() )->frame;
    }

    // begin the keyed transaction
    ODDraftKey key = fromDraft->BeginClone(ev, toDraft, kODNULL, kODCloneCopy);

    if (!selectedFrame) {

   // write the primary contents to the clipboard su

      InitPropVal( ev, clipSU, kODPropContents, kKindGraphicsPart );
      _picture->WriteToStorage(ev, clipSU, key, EXTERNALIZE_SELECTED);

      // write the metadata we're expecting in initpartfromstorage
      // (in case we're pasted into a different kind of part)
      somSelf->QueryPaletteOffset(ev, &_toolsOffset);
      StorageUnitSetValue( clipSU, ev, sizeof(_fBGMode), (ODValue)&_fBGMode);
      StorageUnitSetValue( clipSU, ev, sizeof(_fBGColor), (ODValue)&_fBGColor);
      StorageUnitSetValue( clipSU, ev, sizeof(_fMatchBGColor), (ODValue)&_fMatchBGColor);
      StorageUnitSetValue( clipSU, ev, CCHMAXPATH, (ODValue)&_fBitmapFile[0]);
      StorageUnitSetValue( clipSU, ev, sizeof(_fToolsVisible), (ODValue)&_fToolsVisible);
      StorageUnitSetValue( clipSU, ev, sizeof(_toolsOffset), (ODValue)&_toolsOffset);

   // lets try a suggested frame shape now
      TempODShape frameShape = frame->CreateShape(ev);
      RECTL rclBounds = *_picture->SelectedBounds();

      // get shape in world coords
      ODRect newBounds = downcastToODRECTL(rclBounds);
      frameShape->SetRectangle(ev, &newBounds);

      // move shape into frame coords  (easier way??)
      TempODTransform xform = frame->AcquireInternalTransform(ev, kODNULL);
      frameShape->Transform(ev, xform);

      // write out the requested frame shape (can be ignored...)
      clipSU->AddProperty(ev, kODPropFrameShape);
      frameShape->WriteShape(ev, clipSU);

   // and how about the frame offset... put the LL corner at the mouse
      ODPoint offset = { MAKEFIXED(-rclBounds.xLeft,0), MAKEFIXED(-rclBounds.yBottom,0) };

      // write it out
#ifdef  OLDMOUSEOFFSET
      clipSU->AddProperty(ev, kPropMouseDownOffset)->AddValue(ev, kODPoint);
#else
      clipSU->AddProperty(ev, kODPropMouseDownOffset)->AddValue(ev, kODPoint);
#endif
      StorageUnitSetValue( clipSU, ev, sizeof(offset), &offset);

   // now write a linkspec obtained from _picture
      ODByteArray* LinkSpecData = _picture->LinkSpec();

      if(LinkSpecData) {

        ODLinkSpec* linkSpec = fromDraft->CreateLinkSpec(ev, somSelf, LinkSpecData);
        if(linkSpec) {
          InitPropVal( ev, clipSU, kODPropLinkSpec, kODLinkSpec );
          linkSpec->WriteLinkSpec(ev, clipSU);
//           ODReleaseObject(ev, linkSpec);

          // we now have a valid linkspec on the clipboard!
          _fClipboardChangeID = clipboard->GetUpdateID(ev);
        }

//         DisposeByteArray(LinkSpecData);  // remove byte array
      }

    } else {

      // when transfering frame, add content frame property
      clipSU->AddProperty(ev, kODPropContentFrame);

      TempODPart part = selectedFrame->AcquirePart(ev);
      part->CloneInto(ev, key, clipSU, selectedFrame);

      // write out a suggested frame shape (can be ignored...)
      TempODShape frameShape = selectedFrame->AcquireFrameShape( ev, kODNULL);
      clipSU->AddProperty(ev, kODPropFrameShape);
      frameShape->WriteShape(ev, clipSU);
    }

    fromDraft->EndClone(ev, key);

    clipboard->ExportClipboard( ev );

    _fClipboardChangeID = clipboard->GetUpdateID(ev);

    arbitrator->RelinquishFocus( ev, clipboardFocus, frame );

    null_mousemove();                       // restore the mouse pointer
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DoCopy\n");
  #endif

  return result;
}

//------------------------------------------------------------------------------
ODLinkKey LockLink( Environment* ev, ODLink* link )
{
  ODLinkKey linkKey;

  SOM_TRY

  // if we can't get a lock on it
  while (!link->Lock(ev, 0, &linkKey)) {

    // wait for one second
    DosSleep(1000);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return linkKey;
}

//------------------------------------------------------------------------------
ODLinkKey LockLinkSource( Environment* ev, ODLinkSource* linkSource )
{
  ODLinkKey linkKey;

  SOM_TRY

  // if we can't get a lock on it
  while (!linkSource->Lock(ev, 0, &linkKey)) {

    // wait for one second
    DosSleep(1000);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return linkKey;
}

//------------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartDoPaste( GraphicsPart *somSelf,
                                                  Environment *ev, ODFrame* frame,
                                                  ODULong pasteCmd,
                                                  ODBoolean keyboardused )
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DoPaste");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DoPaste\n");
  #endif

  ODBoolean pasteOK = kODFalse;

  SOM_TRY

  ODFacet*     facet = FirstFacet(ev, frame);
  ODClipboard* clipboard = _fSession->GetClipboard( ev );
  ODBoolean    forceEmbed = kODFalse;
  ODBoolean    forceLink = kODFalse;

  if (_fReadOnly)
    return kODFalse;

  if (pasteCmd==EDIT_PASTEAS) {
    ODBoolean       result;
    ODPasteAsResult resultPasteAs ;

    TRY {
      // display the PasteAs dialog, see if the user wants to create a LINK
      result = clipboard->ShowPasteAsDialog( ev, kODTrue, kODPasteAsEmbed,
                                             facet, FRAME_VIEW, &resultPasteAs );
    }
    CATCH_ALL
    ENDTRY

    // did the user press CANCEL?
    if (!result) {
      #ifdef ODDebug2
        somPrintf("GtkPart: Cancelled DoPaste\n");
      #endif

      return kODFalse;
    }

    // did the user choose the "PASTECONTENT" button?
    if (resultPasteAs.pasteLinkSetting)
      pasteCmd = EDIT_PASTELINK;
    else
      forceEmbed = !resultPasteAs.mergeSetting;
  }

  forceLink = (pasteCmd == EDIT_PASTELINK);

  ODArbitrator* arbitrator = _fSession->GetArbitrator( ev );
  ODTypeToken clipboardFocus = _fSession->Tokenize( ev, kODClipboardFocus );

  ODBoolean failed = kODFalse;

  if (arbitrator->RequestFocus( ev, clipboardFocus, frame )) {

    SetSysPointer( SPTR_WAIT );                           // show we're busy!

    ODDraft*      toDraft = somSelf->GetStorageUnit(ev)->GetDraft(ev);

    ODStorageUnit* clipSU = clipboard->GetContentStorageUnit(ev);
    ODDraft*      fromDraft = clipSU->GetDraft(ev);

    // pasting in link or merge of native content?
    if ( !forceEmbed )  {

      // IS THERE native content?
      if ( SetFocus( ev, clipSU, kODPropContents, kKindGraphicsPart) ) {

        // can we read it successfully?
        if ( somSelf->ReadNativeContent( ev, frame, clipSU, forceLink ) )
          pasteOK = kODTrue;

        // failed to link...
        else if (forceLink)

          somSelf->DisplayResourceText(ev, 263, frame);

      // no native content to link
      } else if (forceLink) {

        somSelf->DisplayResourceText(ev, 264, frame);
      }
    }

    if (!pasteOK && !forceLink) {

      ODID  newPartID;
      ODDraftKey key;

      try{
         key = fromDraft->BeginClone(ev, toDraft, kODNULL, kODClonePaste);
         newPartID = fromDraft->Clone(ev, key, clipSU->GetID(ev), 0, 0);
         fromDraft->EndClone(ev, key);
      } catch (... ) {
         fromDraft->AbortClone(ev, key);
         throw;
      } /* end catch */

      // 6/8/95  Drop offset fix
      ODPoint newTransPt = {20,20};

      ODShape*      newFrameShape;
      ODTransform*  newExternalXForm = frame->CreateTransform(ev);

      newExternalXForm->MoveBy(ev,  &newTransPt );

      if (SetFocus( ev, clipSU, kODPropFrameShape, NULL)) {
         newFrameShape = facet->CreateShape(ev);
         newFrameShape->ReadShape(ev, clipSU);
      } else {
         newFrameShape = kODNULL;     // use default size {80,80}
      }

      TempODPart newPart  = toDraft->AcquirePart(ev, newPartID);
      ODFrame* newFrame = somSelf->MakeEmbeddedFrame( ev, frame,
                                             newFrameShape, newExternalXForm,
                                             newPart, 0, kODFalse );

      somSelf->CreateEmbeddedFacets( ev, frame, newFrame );

      Proxy* p = somSelf->ProxyForFrame(ev, newFrame);
      GtkObject* obj = _picture->Object(p->id);

      if ( FrameIsActive(ev, frame) ) {
        somSelf->HighlightSelection(ev, facet, 0);         // erase old marks
        _fAltMark = kODFalse;
        _picture->Select( obj );                           // select new embed
      }

      pasteOK = kODTrue;
    }

    // done with the clipboard
    arbitrator->RelinquishFocus( ev, clipboardFocus, frame );

    null_mousemove();                       // restore the mouse pointer

  } else {
    somSelf->DisplayResourceText(ev, 265, frame);
  }

  if (pasteOK) {
    // lets figure the needed offset for positioning the selection
    POINTL ptl;

    // picture area (world coords)
    RECTL rclPic;
    _picture->QueryBounds(&rclPic);

    // default position
    ptl.x = rclPic.xLeft + 4*BOUNDS_BORDER;
    ptl.y = rclPic.yBottom + 4*BOUNDS_BORDER;

    // selection REAL bounds
    RECTL rclObjs;
    rclObjs = *_picture->SelectedBounds();
    InflateRect( &rclObjs, (-BOUNDS_BORDER), (-BOUNDS_BORDER) );

    // was it pasted using an accel?
    if (keyboardused) {

      // get the cursor position in world coords
      POINTL ptl2 = CursorPosition( ev, FirstFacet(ev,frame) );
      GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &ptl2 );

      // was the cursor over the pic??
      if (ptl2.x >= rclPic.xLeft   && ptl2.x <= rclPic.xRight &&
          ptl2.y >= rclPic.yBottom && ptl2.y <= rclPic.yTop        ) {
        ptl = ptl2;
      }
    }

    // Move the selection so the lower-left corner is at the desired position
    _picture->Move( ptl.x-rclObjs.xLeft, ptl.y-rclObjs.yBottom );

    somSelf->PartUpdated( ev, frame,
                       PUF_CLEARUNDO | PUF_REPAINT | PUF_CLIPEMBEDS | PUF_RESETFSM );
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DoPaste\n");
  #endif

  return pasteOK;
}

//------------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartDoClear(GraphicsPart *somSelf,
                                              Environment *ev, ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DoClear");

  ODBoolean rc = kODFalse;

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DoClear\n");
  #endif

  SOM_TRY

  ODFacet* facet = FirstFacet(ev, frame);

  if (!_picture->SelectedCount() || !facet) {
    #ifdef ODDebug2
      somPrintf("GtkPart: Nothing to DoClear\n");
    #endif

    return kODFalse;
  }

  // efficient clip calcs...
  RECTL rcl = *_picture->SelectedBounds();
  somSelf->InvalidateObject( ev, 0 );

  // try delete selected
  if (_picture->Delete()) {

    somSelf->ClipEmbeds( ev, facet, &rcl );

    somSelf->PartUpdated( ev, frame, PUF_CLEARUNDO | PUF_UPDATELINKS );

    rc = kODTrue;
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DoClear\n");
  #endif

  return rc;
}

//------------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartReadNativeContent(GraphicsPart *somSelf,
                                                           Environment *ev,
                                                           ODFrame* frame,
                                                           ODStorageUnit* sourceSU,
                                                           ODBoolean tryLink)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","ReadNativeContent");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering ReadNativeContent\n");
  #endif

  ODBoolean readOK = kODFalse;

  SOM_TRY

  // remove any prior selection
  if (_picture->SelectedCount()) {
    ODFacet* facet = FirstFacet(ev,frame);
    if ( FrameIsActive(ev, frame) && facet->GetWindow(ev)->IsActive(ev))
      somSelf->HighlightSelection( ev, facet, 0 );
    _picture->Select();
  }
  _picture->Correlate(0);                            // remove lastHit check

  ODDraft*  toDraft = somSelf->GetStorageUnit(ev)->GetDraft(ev);

  if (tryLink) {

    // valid linkSpec in storage unit??
    if (!SetFocus( ev, sourceSU, kODPropLinkSpec, kODLinkSpec))
      return kODFalse;

    // create an empty linkspec
    ODLinkSpec* linkSpec = toDraft->CreateLinkSpec(ev, somSelf, (ODByteArray*)kODNULL);

    // Read the link spec from the clipboard
    linkSpec->ReadLinkSpec(ev, sourceSU);

    // create a Link from the LinkSpec??!
    ODLink* link = kODNULL;

    TRY {
      link = toDraft->AcquireLink(ev, 0, linkSpec);
    }
    CATCH_ALL
      link = kODNULL;
    ENDTRY

    // should we do this tidying?  (I get an exception if I do...)
  //      ODReleaseObject(ev, linkSpec);

    // valid link created??
    if (!link)
      return kODFalse;

    // try creating the link target object
    readOK = (_picture->CreateLinkObj(ev, somSelf, link) != kODNULL);

  } else {
    ODDraft* fromDraft = sourceSU->GetDraft(ev);

    TRY {

      // begin the read transaction
      ODDraftKey key = fromDraft->BeginClone(ev, toDraft, kODNULL, kODClonePaste);

      // read stuff into my _picture
      _picture->ReadFromStorage(ev, sourceSU, key, somSelf);

      // end it all
      fromDraft->EndClone(ev, key);
      readOK = kODTrue;
    }
    CATCH_ALL
    ENDTRY
  }

  if (readOK)
    _picture->PostReadInit(ev, somSelf, kODTrue);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving ReadNativeContent\n");
  #endif

  return readOK;
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartActivateFrame(GraphicsPart *somSelf,
                                       Environment *ev, ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","ActivateFrame");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering ActivateFrame\n");
  #endif

  SOM_TRY

  if ( !FrameIsActive(ev, frame) ) {
    _fCurFrame = frame;

    if ( _fSession->GetArbitrator(ev)->RequestFocusSet(ev, _fFocusSet,frame) ) {
       somSelf->FocusAcquired(ev, _fSelectionFocus, frame);
       somSelf->FocusAcquired(ev, _fKeyFocus, frame);

       if (_fMenuFocus)
         somSelf->FocusAcquired(ev, _fMenuFocus, frame);
    }
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving ActivateFrame\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDeActivateFrame(GraphicsPart *somSelf,
                                         Environment *ev, ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DeActivateFrame");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DeActivateFrame\n");
  #endif

  SOM_TRY

  if (frame != kODNULL) {
    _fCurFrame = (ODFrame*)_fDisplayFrames->First();

    _fSession->GetArbitrator(ev)->RelinquishFocusSet(ev, _fFocusSet,frame);
    somSelf->FocusLost(ev, _fSelectionFocus, frame);
    somSelf->FocusLost(ev, _fKeyFocus, frame);

    if (_fMenuBar)
      somSelf->FocusLost(ev, _fMenuFocus, frame);

    somSelf->FocusLost(ev, _fStatusFocus, frame);
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DeActivateFrame\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartActivatingWindow(GraphicsPart *somSelf,
                                          Environment *ev, ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","ActivatingWindow");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering ActivatingWindow\n");
  #endif

  SOM_TRY

  PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);
  if (pInfo->fNeedsActivating)
  {
     somSelf->ActivateFrame(ev, frame);
     pInfo->fNeedsActivating = kODFalse;
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving ActivatingWindow\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDeActivatingWindow(GraphicsPart *somSelf,
                                            Environment *ev,
                                           ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DeActivatingWindow");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DeActivatingWindow\n");
  #endif

  SOM_TRY

  PartInfoRec* pInfo = (PartInfoRec*) frame->GetPartInfo(ev);
  TempODFrame ownerFrame = _fSession->GetArbitrator(ev)->AcquireFocusOwner(ev, _fSelectionFocus);
  if (frame == ownerFrame)
  {
     pInfo->fNeedsActivating = kODTrue;
     somSelf->DeActivateFrame(ev, frame);
  }
  else
     pInfo->fNeedsActivating = kODFalse;

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DeActivatingWindow\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartInstallMenus( GraphicsPart *somSelf,
                                                  Environment *ev,
                                                  ODFrame* ownerFrame )
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","InstallMenus");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering InstallMenus\n");
  #endif

  SOM_TRY

  // is there a menubar for the shell?
  if (!_fMenuFocus)
    return;

  // normal docshell menubar
  _fMenuBar = _fSession->GetWindowState(ev)->CopyBaseMenuBar(ev);

  // created menubar ok?
  if (!_fMenuBar)
    return;

  // first we take OFF the items we don't support yet...
  _fMenuBar->RemoveMenu(ev, EDIT_OPENSEL );
  _fMenuBar->RemoveMenu(ev, EDIT_SELPROPERTIES );

  // string for loading and setting text from Resource module
  char str255[255];

  // menuitem structure for inserting additional items
  ODPlatformMenuItem mi;
  memset((PCH)&mi, 0, sizeof(MENUITEM));
  mi.afStyle = MIS_TEXT;

  mi.id = VIEW_REFRESH;
  _fMenuBar->AddMenuItemBefore(ev, IDMS_VIEW, kODNULL, &mi, VIEW_OPENAS);
  _fMenuBar->SetMenuItemText(ev, IDMS_VIEW, VIEW_REFRESH, "~Refresh\tAlt+R");
  SetStatusTextFromResource( VIEW_REFRESH, 256 );

  mi.afStyle = MIS_SEPARATOR;           // Put separator at end of Help Menu
  mi.id = -1;
  _fMenuBar->AddMenuItemLast(ev, IDMS_HELP, kODNULL, &mi);

  mi.afStyle = MIS_TEXT;
  mi.id = HELP_PARTINFO;                // Insert PartInfo item into Help menu
  _fMenuBar->AddMenuItemLast(ev, IDMS_HELP, kODNULL, &mi);
  WinLoadString( (HAB)0, _hmod, 294, 255, str255 );
  _fMenuBar->SetMenuItemText(ev, IDMS_HELP, HELP_PARTINFO, str255);
  SetStatusTextFromResource( HELP_PARTINFO, 298 );

  if (!_fReadOnly) {

    mi.id = VIEW_SHOWTOOLS;
    _fMenuBar->AddMenuItemLast(ev, IDMS_VIEW, kODNULL, &mi);
    _fMenuBar->SetMenuItemText(ev, IDMS_VIEW, VIEW_SHOWTOOLS,
                               _fToolsVisible ? "Hide ~toolbar" : "Show ~toolbar" );
    SetStatusTextFromResource( VIEW_SHOWTOOLS, _fToolsVisible ? 262 : 261 );

    mi.id = IDM_VIEW_BG;
    _fMenuBar->AddMenuItemLast(ev, IDMS_VIEW, kODNULL, &mi);
    _fMenuBar->SetMenuItemText(ev, IDMS_VIEW, IDM_VIEW_BG, "~Background");
    SetStatusTextFromResource( IDM_VIEW_BG, 268 );

    ODPlatformMenu bgMenu = WinLoadMenu( HWND_OBJECT, _hmod, IDM_VIEW_BG );
    _fMenuBar->InsertSubmenu(ev, IDMS_VIEW, IDM_VIEW_BG, bgMenu);
    SetStatusTextFromResource( VIEW_BGNDMODE_COLOR, 257 );
    SetStatusTextFromResource( VIEW_BGNDMODE_TILED, 258 );
    SetStatusTextFromResource( VIEW_BGNDMODE_BLTFIT, 259 );
    SetStatusTextFromResource( VIEW_BGND_COLOR, 260 );

    _fMenuBar->EnableMenuItem( ev, IDMS_VIEW, VIEW_BGNDMODE_TILED, _hbmBackground!=0 );
    _fMenuBar->EnableMenuItem( ev, IDMS_VIEW, VIEW_BGNDMODE_BLTFIT, _hbmBackground!=0 );
    _fMenuBar->CheckMenuItem( ev, IDMS_VIEW, VIEW_BGNDMODE_COLOR+_fBGMode, kODTrue );

    // add our custom SELECTED MENU
    ODPlatformMenu selectedMenu = WinLoadMenu( HWND_OBJECT, _hmod, IDM_SELECTED );
    _fMenuBar->AddMenuLast(ev, IDM_SELECTED, selectedMenu, somSelf);
    DosFreeResource( (PVOID)selectedMenu );
    _fMenuBar->SetMenuItemText(ev, IDM_SELECTED, kODNULL, "~Selected");

    SetStatusTextFromResource( IDM_SELECTED, 293 );
     SetStatusTextFromResource( SELECTED_ROTATE, 271 );
     SetStatusTextFromResource( SELECTED_ARRANGE, 272 );
      SetStatusTextFromResource( SELECTED_MOVETOFRONT, 273 );
      SetStatusTextFromResource( SELECTED_MOVETOBACK, 274 );
     SetStatusTextFromResource( SELECTED_GROUPING, 275 );
      SetStatusTextFromResource( SELECTED_GROUP, 276 );
      SetStatusTextFromResource( SELECTED_UNGROUP, 277 );
     SetStatusTextFromResource( SELECTED_LOCKING, 278 );
      SetStatusTextFromResource( SELECTED_LOCK, 279 );
      SetStatusTextFromResource( SELECTED_UNLOCK, 280 );
     SetStatusTextFromResource( SELECTED_BGNDSTUFF, 281 );
      SetStatusTextFromResource( SELECTED_OPAQUE, 282 );
      SetStatusTextFromResource( SELECTED_TRANSLUCENT, 283 );
  }

  ODACCEL PartAccel;
  ULONG ulSize, ulCopied, ulCount;

  HACCEL hAccel;
  hAccel = WinLoadAccelTable( 1, _hmod, _fReadOnly ? ID_READONLYACCELTABLE
                                                   : ID_ACCELTABLE );
  ulSize = WinCopyAccelTable( hAccel, NULL, 0);

  ACCELTABLE* acctAccelTable;
  acctAccelTable = (ACCELTABLE*)malloc(ulSize);
  ulCopied = WinCopyAccelTable(hAccel, acctAccelTable, ulSize);

  #if 1

    // BROKEN, OpenDoc is only taking this AddToAccelTable *once*,
    // so if you loop through to get all your entries you only get
    // the last set.  ACCELMAX is *20* in beta driver...

    // make sure we limit to max available slots!
    int count = min(acctAccelTable->cAccel, ACCELMAX);
    for (int i = 0; i < count; i++) {
      PartAccel.aAccel[i] = acctAccelTable->aaccel[i];
    }

    _fMenuBar->AddToAccelTable(ev, count, &PartAccel);

  #else

    // OpenDoc ODACCEL structure limited to 20 items?!
    // loop here until all items added
    int start, count;
    start = 0;

    do {

      // number of items copied in this loop step
      count = min( acctAccelTable->cAccel - start, ACCELMAX );

      for (int i=0; i<count; i++)
        PartAccel.aAccel[i] = acctAccelTable->aaccel[i+start];

      _fMenuBar->AddToAccelTable(ev, count, &PartAccel );

      start += ACCELMAX;

    } while ( start < acctAccelTable->cAccel );

  #endif

  free(acctAccelTable);
  WinDestroyAccelTable(hAccel);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving InstallMenus\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartSetBGColor( GraphicsPart *somSelf,
                                                Environment *ev,
                                                ODFrame* frame, ODULong rgb,
                                                ODBoolean propagate )
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","SetBGColor");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering SetBGColor\n");
  #endif

  SOM_TRY

  if (rgb != _fBGColor) {

    _fBGColor = rgb;

    // if propagate, we are not getting this from our container
    if (propagate) {

      // if we were matching, we've broken the coupling here
      if (_fMatchBGColor) {

        _fMatchBGColor = kODFalse;

      // if we weren't, lets see if we've re-synched
      } else {

        // if we are root we'll get null for a containing frame..
        TempODFrame containingFrame = frame->AcquireContainingFrame(ev);
        if (containingFrame) {
          TempODPart containingPart = containingFrame->AcquirePart(ev);
          TempODStorageUnit propSU = containingPart->AcquireContainingPartProperties(ev, frame);

          if (SetFocus( ev, propSU, kODBackgroundColor, kODRGB2) ) {

            StorageUnitGetValue(propSU, ev, sizeof(RGBColor), &rgb);

            // are we re-synching our backgrounds?
            if (rgb == _fBGColor) {
              _fMatchBGColor = kODTrue;
            }
          }
        }
      }

      // notify embedded frames, std property change!
      // prep a storage unit
      TempODStorageUnit su = somSelf->GetStorageUnit(ev)->GetDraft(ev)->CreateStorageUnit(ev);
      InitPropVal( ev, su, kODBackgroundColor, kODRGB2 );
      StorageUnitSetValue(su, ev, sizeof(RGBColor), &_fBGColor);

      // tell 'em about it!
      ODFrame *subframe = (ODFrame*)_fEmbeddedFrames->First();
      while (subframe) {
         TempODPart embeddedPart = subframe->AcquirePart(ev);
         if (embeddedPart) {
            embeddedPart->ContainingPartPropertiesUpdated(ev, subframe, su);
         }
         subframe = (ODFrame*)_fEmbeddedFrames->After(subframe);
      }
    }

    // only repaint if we're in color mode!
    somSelf->PartUpdated( ev, frame, _fBGMode==BGM_COLOR ? PUF_REPAINTALL : 0 );
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving SetBGColor\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope ODFrame*  SOMLINK GraphicsPartMakeEmbeddedFrame( GraphicsPart *somSelf,
                                                           Environment *ev,
                                                           ODFrame* containingFrame,
                                                           ODShape* frameShape,
                                                           ODTransform* externalTransform,
                                                           ODPart* embedPart,
                                                           ODID embedId,
                                                           ODBoolean isOverlaid)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","MakeEmbeddedFrame");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering MakeEmbeddedFrame\n");
  #endif

  ODFrame* newFrame;
  ODShape* newShape;
  ULONG id;

  SOM_TRY

  // decide what newShape should be - for now, give it what it asked for
  if (frameShape)
    newShape = frameShape;

  else {                     // no frameShape specified, use a default size
    ODRect rect = { 0, MAKEFIXED(100,0), MAKEFIXED(100,0), 0};
    newShape = containingFrame->CreateShape(ev);
    newShape->SetRectangle(ev, &rect);
  };

  // create the new frame
  newFrame = somSelf->GetStorageUnit(ev)->GetDraft(ev)->
        CreateFrame(ev,
              (containingFrame->GetStorageUnit(ev)) ? kODFrameObject :
                        kODNonPersistentFrameObject,
              containingFrame, newShape, (ODCanvas*)kODNULL,
              embedPart,
              FRAME_VIEW,
              PRES_DEFAULT,
              kODFalse, isOverlaid);

  _fEmbeddedFrames->AddFirst(newFrame);

  if (embedId != 0xffffffff) {
    // Here's where we create the shadow-object in picture for the embedded Part.
    // Create the object passing in THIS part's handle, get back the id (which
    // is stored in the Proxy).

    ODRect boundingBox;
    TempODShape frameShape = newFrame->AcquireFrameShape(ev, kODNULL);
    frameShape->GetBoundingBox(ev, &boundingBox);

    #if 1
      RECTL rcl = ODRECTL( boundingBox );

      ODPoint offset;
      externalTransform->GetOffset(ev, &offset);
      ODPOINTL ptl(offset);
    #else
      TempODTransform transform = externalTransform->Copy(ev);

      // FIX for PRINT-TRAP
      ODFacet* facet = FirstFacet(ev, containingFrame);
      if (facet) {
        TempODTransform xform = facet->AcquireContentTransform(ev, kODNULL);
        transform->PostCompose(ev, xform);
      }

      POINTL ptl = {0,0};
      ODPoint botLeft = {boundingBox.left, boundingBox.bottom};
      ODPoint topRight = {boundingBox.right, boundingBox.top};
      transform->TransformPoint(ev, &botLeft);
      transform->TransformPoint(ev, &topRight);
      RECTL rcl;
      rcl = ODRECTL( botLeft, topRight);

      GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 2, (PPOINTL)&rcl );
    #endif

    GtkEmbed* obj = new GtkEmbed(somSelf, &rcl, &ptl);

    _picture->AddObject( obj );                    // this sets the ID!
    id = obj->Id();
  } else id = embedId;

  // create the proxy, giving it the gtkembed's id
  Proxy* p = new Proxy(newFrame, externalTransform, id);
  _fContents->AddFirst(p);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving MakeEmbeddedFrame\n");
  #endif

  return newFrame;
}

//------------------------------------------------------------------------------
SOM_Scope void SOMLINK GraphicsPartCreateEmbeddedFacets( GraphicsPart *somSelf,
                                                         Environment *ev,
                                                         ODFrame* containingFrame,
                                                         ODFrame* embeddedFrame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","CreateEmbeddedFacets");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering CreateEmbeddedFacets\n");
  #endif

  SOM_TRY

  ODFacet* facet;
  ODTransform* baseXform = somSelf->ProxyForFrame(ev, embeddedFrame)->transform;

  ODFrameFacetIterator* facets = containingFrame->CreateFacetIterator(ev);
  for (facet = facets->First(ev); facets->IsNotComplete(ev);
       facet = facets->Next(ev)) {

    TempODShape clip = ODCopyAndRelease(ev, embeddedFrame->AcquireFrameShape(ev, kODNULL));
    TempODTransform xform = baseXform->Copy(ev);

    facet->CreateEmbeddedFacet( ev, embeddedFrame, clip, xform,
                                kODNULL, kODNULL, kODNULL, kODFrameInFront);
  }
  delete facets;

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving CreateEmbeddedFacets\n");
  #endif
}

//------------------------------------------------------------------------------
void MoveProxy( Environment* ev, Proxy *p, Point *delta )
{
  SOM_TRY

  ODPoint Offset (downcastToODPOINTL(*delta));
  p->transform->MoveBy(ev, &Offset);

  TempODTransform newExternalXForm = p->transform->Copy(ev);

  ODFrameFacetIterator* facets = p->frame->CreateFacetIterator(ev);
  for (ODFacet* f1 = facets->First(ev);
        facets->IsNotComplete(ev); f1 = facets->Next(ev))
    f1->ChangeGeometry(ev, kODNULL, newExternalXForm, kODNULL);
//    f1->ChangeGeometry(ev, kODNULL, p->transform, kODNULL);
  delete facets;

  SOM_CATCH_ALL
  SOM_ENDTRY
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartInitPalette(GraphicsPart *somSelf,
                                                 Environment *ev,
                                                ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","InitPalette");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering InitPalette\n");
  #endif

  SOM_TRY

  ODFacet* facet = FirstFacet( ev, frame );
  ODWindow* window = facet->GetWindow(ev);

  if (!_hwndTools && window) {
    HWND hwndFrame = window->GetPlatformWindow(ev);
    HWND hwndClient = WinWindowFromID(hwndFrame, FID_CLIENT);

    _hwndTools = CreateToolbar( hwndFrame, _hmod, ID_TOOLS);
    if (_hwndTools) {
      WinSetOwner( _hwndTools, hwndClient );
//      WinSetOwner( _hwndTools, _hwndObj );
      POINTL ptl;
      RECTL  rcl, rclClient;
      QueryFacetOffset( ev, facet, &ptl );
      if (FALSE) {                           // _toolsOffset.x) {
        ptl.x += _toolsOffset.x;
        ptl.y += _toolsOffset.y;
      } else {
        WinQueryWindowRect( _hwndTools, &rcl);
        WinQueryWindowRect( hwndClient, &rclClient);
        WinMapWindowPoints( hwndClient, hwndFrame, (PPOINTL)&rclClient, 2 );
        if (frame->IsRoot(ev)) {
          ptl.x = rclClient.xLeft;
          ptl.y = rclClient.yTop - rcl.yTop;
//          ptl.x += WinQuerySysValue( HWND_DESKTOP, SV_CXSIZEBORDER );
//          ptl.y += WinQuerySysValue( HWND_DESKTOP, SV_CYSIZEBORDER );
//          ptl.y += (rclClient.yTop - rcl.yTop);
        } else {
          ptl.x -= (rcl.xRight - rclClient.xLeft + 4);
          ptl.y += rclClient.yBottom;
        }
      }
      #if 1
        WinSetWindowPos( _hwndTools, HWND_TOP, ptl.x, ptl.y,
                         0, 0, SWP_MOVE | SWP_ZORDER );
      #else
        WinQueryWindowRect( _hwndTools, &rcl);
        if (frame->IsRoot(ev))
          WinQueryWindowRect( WinWindowFromID(hwndFrame, FID_CLIENT), &rclClient);
          ptl.x += WinQuerySysValue( HWND_DESKTOP, SV_CXSIZEBORDER );
          ptl.y += WinQuerySysValue( HWND_DESKTOP, SV_CYSIZEBORDER );
          WinSetWindowPos( _hwndTools, HWND_TOP, ptl.x,
                                                 ptl.y + rclClient.yTop-rcl.yTop,
                                                 0, 0, SWP_MOVE | SWP_ZORDER );
        } else {
          ptl.x -= rcl.xRight + 3;
          WinSetWindowPos( _hwndTools, HWND_TOP, ptl.x,
                                                 ptl.y + _rclFrame.yTop-rcl.yTop,
                                                 0, 0, SWP_MOVE | SWP_ZORDER );
        }
      #endif
    }
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving InitPalette\n");
  #endif
}

//-------------------------------------------------------------------------
void  CheckToolbarItem( HWND hwndTools, int command )
{
  if (hwndTools) {
    HWND hwndMenu = WinWindowFromID( hwndTools, FID_MENU );
    for (int i=IDM_TOOL_POINTER; i<IDM_TOOL_POINTER+10; i++)
      WinCheckMenuItem( hwndMenu, i, i==command );
  }
}

//-----------------------------------------------------------------------------
VOID AdjustWindowPosition( HWND hwnd )
{
  RECTL rcl1, rcl2;
  LONG  x, y;
  HWND hwndFrame, hwndClient;

  hwndFrame = WinQueryWindow ( hwnd, QW_PARENT );
  hwndClient = WinWindowFromID( hwndFrame, FID_CLIENT );
  if (!hwndClient) hwndClient = hwndFrame;

  WinQueryWindowRect( hwndClient, &rcl1 );
  WinMapWindowPoints( hwndClient, hwndFrame, (PPOINTL)&rcl1, 2 );
  WinQueryWindowRect( hwnd, &rcl2 );
  WinMapWindowPoints( hwnd, hwndFrame, (PPOINTL)&rcl2, 2 );

  if (rcl2.yBottom < rcl1.yBottom) y = 0;
  else if (rcl2.yTop > rcl1.yTop) y = rcl1.yTop - (rcl2.yTop-rcl2.yBottom);
  else y = rcl2.yBottom;

  if (rcl2.xLeft < rcl1.xLeft) x = 0;
  else if (rcl2.xRight > rcl1.xRight) x = rcl1.xRight - (rcl2.xRight-rcl2.xLeft);
  else x = rcl2.xLeft;

  if ( x != rcl2.xLeft || y != rcl2.yBottom )
    WinSetWindowPos( hwnd, NULL, x, y, 0, 0, SWP_MOVE );
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartUpdatePalette(GraphicsPart *somSelf,
                                                   Environment *ev,
                                                  ODFrame* frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","UpdatePalette");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering UpdatePalette\n");
  #endif

  SOM_TRY

  ODTypeToken curView = frame->GetViewType(ev);
  ODBoolean fFrameView = (curView == FRAME_VIEW);

  // should we be showing the toolbar?
  if ( !_fReadOnly && FrameIsActive(ev, frame) && fFrameView  && _fToolsVisible ) {

    // has the toolbar been initialized?
    if (!_hwndTools) somSelf->InitPalette(ev, frame);

    // set it up properly
    if (_hwndTools) {
      CheckToolbarItem( _hwndTools, _fsmType+IDM_TOOL_POINTER );
      AdjustWindowPosition( _hwndTools );

      // FIX for main menubar overlay??
      WinSetWindowPos( _hwndTools, HWND_TOP, 0, 0, 0, 0, SWP_ZORDER );
      WinShowWindow( _hwndTools, kODTrue);
    }

  } else if (_hwndTools) {

    WinShowWindow(_hwndTools, FALSE);

  }
  null_mousemove();

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving UpdatePalette\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartMovePalette(GraphicsPart *somSelf,
                                                 Environment *ev,
                                                long dx, long dy)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","MovePalette");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering MovePalette\n");
  #endif

  if (_hwndTools) {
    SWP swp;
    WinQueryWindowPos( _hwndTools, &swp);
    WinSetWindowPos( _hwndTools, HWND_TOP, swp.x+dx, swp.y+dy, 0, 0, SWP_MOVE);
  }

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving MovePalette\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartQueryPaletteOffset(GraphicsPart *somSelf,
                                                        Environment *ev,
                                                       POINTL* ptlOffset)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","QueryPaletteOffset");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering QueryPaletteOffset\n");
  #endif

  if (_hwndTools) {
    SWP swp;
    WinQueryWindowPos( _hwndTools, &swp);
    ptlOffset->x = swp.x - _frameOffset.x;
    ptlOffset->y = swp.y - _frameOffset.y;
  }

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving QueryPaletteOffset\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartDisplayText( GraphicsPart *somSelf,
                                                      Environment *ev,
                                                      string str,
                                                      ODFrame * frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DisplayText");

  #ifdef ODDebug2
//    somPrintf("GtkPart: Entering DisplayText\n");
  #endif

  if (!_fStatusLn)
    return kODFalse;

  SOM_TRY

  // do we have the focus?
  if (! _fHaveStatusLineFocus) {
     // NO, ask for it
     if (_fSession->GetArbitrator(ev)->RequestFocus(ev, _fStatusFocus, frame))
         _fHaveStatusLineFocus = kODTrue; // part has focus
  }

  // have we gotten the focus?
  if ( _fHaveStatusLineFocus ) {

    ODULong refct = frame->GetRefCount(ev);                       // refct bug in SSLT

    _fStatusLn->SetStatusLineText(ev,str,frame);

    if ( refct < frame->GetRefCount(ev) )                         // refct bug in SSLT
      frame->Release(ev);

    return kODTrue;
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
//    somPrintf("GtkPart: Leaving DisplayText\n");
  #endif

  return kODFalse;
}

//------------------------------------------------------------------------------
// load a string from the resource file and display it on the Status Line
//------------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartDisplayResourceText(GraphicsPart *somSelf, Environment *ev,
                                  ODULong strId, ODFrame * frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DisplayResourceText");

  #ifdef ODDebug2
//    somPrintf("GtkPart: Entering DisplayResourceText\n");
  #endif

  ODBoolean rc;

  if (_fReadOnly && !strId)
    return kODFalse;

  SOM_TRY

  char string[256];
  WinLoadString( (HAB)0, _hmod,
                 strId ? strId : _fsmType+IDM_TOOL_POINTER, 255, string);

  rc = somSelf->DisplayText(ev, string, frame );

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
//    somPrintf("GtkPart: Leaving DisplayResourceText\n");
  #endif

  return rc;
}

//------------------------------------------------------------------------------
// load a string from the resource file and display it on the Status Line
//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDisplayMenuPromptText(GraphicsPart *somSelf, Environment *ev,
                                  ODULong menuItemId, ODFrame * frame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DisplayMenuPromptText");

  #ifdef ODDebug2
//    somPrintf("GtkPart: Entering DisplayMenuPromptText\n");
  #endif

  SOM_TRY

  ODULong strId = 0;

  switch (menuItemId) {
    case VIEW_REFRESH:
      strId = 256;
      break;
    case VIEW_BGNDMODE_COLOR:
      strId = 257;
      break;
    case VIEW_BGNDMODE_TILED:
      strId = 258;
      break;
    case VIEW_BGNDMODE_BLTFIT:
      strId = 259;
      break;
    case VIEW_BGND_COLOR:
      strId = 260;
      break;
    case VIEW_SHOWTOOLS:
      strId = _fToolsVisible ? 262 : 261;
      break;
    case IDM_VIEW_BG:
      strId = 268;
      break;
    case SELECTED_ALTMARK:
      strId = _fAltMark ? 270 : 269;
      break;
    case IDM_POPUP_ROTATE:
      strId = 292;
      break;
#if 0
    case IDM_SELECTED:
      strId = 293;
      break;
    case SELECTED_ROTATE:
      strId = 271;
      break;
    case SELECTED_ARRANGE:
      strId = 272;
      break;
    case SELECTED_MOVETOFRONT:
      strId = 273;
      break;
    case SELECTED_MOVETOBACK:
      strId = 274;
      break;
    case SELECTED_GROUPING:
      strId = 275;
      break;
    case SELECTED_GROUP:
      strId = 276;
      break;
    case SELECTED_UNGROUP:
      strId = 277;
      break;
    case SELECTED_LOCKING:
      strId = 278;
      break;
    case SELECTED_LOCK:
      strId = 279;
      break;
    case SELECTED_UNLOCK:
      strId = 280;
      break;
    case SELECTED_BGNDSTUFF:
      strId = 281;
      break;
    case SELECTED_OPAQUE:
      strId = 282;
      break;
    case SELECTED_TRANSLUCENT:
      strId = 283;
      break;
#endif
    case IDM_POPUP_DELETE:
      strId = 284;
      break;
    case IDM_LINE_COLOR:
      strId = 285;
      break;
    case IDM_FILL_COLOR:
      strId = 286;
      break;
    default:
      if (     menuItemId >= IDM_LINE_STYLES    && menuItemId <= IDM_LS_INVISIBLE )
        strId = 287;
      else if (menuItemId >= IDM_LINE_WIDTHS    && menuItemId <= IDM_LW_THICK )
        strId = 288;
      else if (menuItemId >= IDM_FILL_STYLES    && menuItemId <= IDM_FS_DIAG4 )
        strId = 289;
      else if (menuItemId >= ID_SHADOWSTUFFMENU && menuItemId <= IDM_SH_DIAG4 )
        strId = 290;
      else if (menuItemId >= ID_ARCSTUFFMENU    && menuItemId <= IDM_CLOSE_CHORD )
        strId = 291;

      break;
  }

  if (strId)
    somSelf->DisplayResourceText(ev, strId, frame );

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
//    somPrintf("GtkPart: Leaving DisplayMenuPromptText\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope ODWindow*  SOMLINK GraphicsPartCreateWindow(GraphicsPart *somSelf,
                                           Environment *ev, ODFrame* sourceFrame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","CreateWindow");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering CreateWindow\n");
  #endif

  ODWindow* window = kODNULL;

  SOM_TRY

  Rect windRect;
  ODPlatformWindow platformWindow = kODNULL;
  // B1C16 need to check if root frame
  if (sourceFrame && !sourceFrame->IsRoot(ev)) {
     SWP swp;
     swp.x = 100;
     swp.y = 100;
     swp.cx = 400;
     swp.cy = 400;
     platformWindow = _fSession->CreatePlatformWindow(ev, ODPlatformWindowDefaultCreateOptions | FCF_HORZSCROLL | FCF_VERTSCROLL);
     WinSetWindowPos(platformWindow, HWND_TOP,
                     swp.x, swp.y, swp.cx, swp.cy,
                     SWP_SIZE | SWP_MOVE);

   } else {
      platformWindow = _fSession->CreatePlatformWindow(ev, ODPlatformWindowDefaultCreateOptions);
  } /* endif */

  window =  _fSession->GetWindowState(ev)->
                   RegisterWindow(ev, platformWindow,
                                  kODFrameObject,
                                  (sourceFrame==kODNULL),      // is root
                                  kODTrue,                     // Is resizable
                                  kODFalse,                    // Is floating
                                  kODTrue,                     // should save
                                  kODTrue,                     // should dispose  [143654] - ced
                                  _fPartWrapper, FRAME_VIEW,
                                  PRES_DEFAULT, sourceFrame);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving CreateWindow\n");
  #endif

  return window;
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartLockEmbed(GraphicsPart *somSelf,
                                               Environment *ev,
                                              ODULong id, ODBoolean fLock)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","LockEmbed");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering LockEmbed\n");
  #endif

  SOM_TRY

  if (!ev) ev = SOM_ENVIRONMENT;

  Proxy* p = somSelf->ProxyFromId(ev, id);
  if (p) p->frame->SetFrozen(ev, fLock);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving LockEmbed\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartSelectEmbed(GraphicsPart *somSelf,
                                                 Environment *ev,
                                                ODULong id, ODBoolean fSelected)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","SelectEmbed");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering SelectEmbed\n");
  #endif

  SOM_TRY

  if (!ev) ev = SOM_ENVIRONMENT;
  Proxy* p = somSelf->ProxyFromId(ev, id);

  if (p) {
    ODFrameFacetIterator* facets = p->frame->CreateFacetIterator(ev);
    for (ODFacet* facet = facets->First(ev); facets->IsNotComplete(ev); facet = facets->Next(ev))
    {
       facet->SetSelected(ev, fSelected);
//       facet->ChangeHighlight(ev, fSelected ? kODFullHighlight : kODNoHighlight);
    }
    delete facets;
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving SelectEmbed\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartSetEmbedId(GraphicsPart *somSelf,
                                                Environment *ev,
                                               ODULong id, ODULong newId)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","SetEmbedId");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering SetEmbedId\n");
  #endif

  SOM_TRY

  if (!ev) ev = SOM_ENVIRONMENT;

  Proxy* p = somSelf->ProxyFromId(ev, id);
  if (p) p->id = newId;

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving SetEmbedId\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartCopyEmbed( GraphicsPart *somSelf,
                                               Environment *ev,
                                               ODULong idOld, ODULong idNew)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","CopyEmbed");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering CopyEmbed\n");
  #endif

  SOM_TRY

  if (!ev) ev = SOM_ENVIRONMENT;

  Proxy* original = somSelf->ProxyFromId(ev, idOld);
  if (original == kODNULL) return;

  ODFrame* oldFrame = original->frame;
  TempODFrame containingFrame = oldFrame->AcquireContainingFrame(ev);
  ODDraft* draft = somSelf->GetStorageUnit(ev)->GetDraft(ev);

  // create a new storage unit for the copy
  TempODStorageUnit  newSU = draft->CreateStorageUnit(ev);

  // clone duplicate into it
  ODDraftKey key = draft->BeginClone(ev, draft, kODNULL, kODCloneAll);
  TempODPart part = oldFrame->AcquirePart(ev);
  part->CloneInto(ev, key, newSU, oldFrame);
  draft->EndClone(ev, key);

  // handle to the new part
  TempODPart newPart = draft->AcquirePart(ev, newSU->GetID(ev));

  if (!newPart) {
    somSelf->DisplayResourceText(ev, 267, _fCurFrame);
    return;
  }

  ODTransform* xform = original->transform->Copy(ev);
  ODShape* frameShape = ODCopyAndRelease(ev, oldFrame->AcquireFrameShape(ev, kODNULL));

  ODFrame* newFrame;
  newFrame = somSelf->MakeEmbeddedFrame( ev, containingFrame, frameShape,
                                         xform, newPart, idNew, kODFalse);

  somSelf->CreateEmbeddedFacets( ev, containingFrame, newFrame );

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving CopyEmbed\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartReFrameEmbed(GraphicsPart *somSelf,
                                               Environment *ev,
                                              ODULong id, RECTL* rclNewFrame)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","ReFrameEmbed");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering ReFrameEmbed\n");
  #endif

  SOM_TRY

  if (!ev) ev = SOM_ENVIRONMENT;

  Proxy* p = somSelf->ProxyFromId(ev, id);
  if (p == kODNULL) return;

  TempODFrame frame = p->frame->AcquireContainingFrame(ev);
  ODFacet* facet = FirstFacet(ev, frame);

  ODShape* newShape = p->frame->CreateShape(ev);
  RECTL rclTmp = *rclNewFrame;
  ODRect newBounds = downcastToODRECTL(rclTmp);
  newShape->SetRectangle(ev, &newBounds);

  // invalidate the old area
  p->frame->Invalidate(ev, NULL, kODNULL );

  // change the embed's shape
  p->frame->ChangeFrameShape(ev, newShape, kODNULL);  // already in frame coords

  // recalc clipping
  somSelf->ClipEmbeds(ev, 0, 0);

  // invalidate new area
  p->frame->Invalidate(ev, NULL, kODNULL );

  // get the new frame shape to return, in case the embed altered it...
  ODRect boundingBox;
  TempODShape frameShape = p->frame->AcquireFrameShape(ev, kODNULL);
  frameShape->GetBoundingBox(ev, &boundingBox);
  *rclNewFrame = ODRECTL(boundingBox);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving ReFrameEmbed\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartTransformEmbed( GraphicsPart *somSelf,
                                                    Environment *ev,
                                                    ODULong id, MATRIXLF* mx )
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","TransformEmbed");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering TransformEmbed\n");
  #endif

  SOM_TRY

  if (!ev) ev = SOM_ENVIRONMENT;

  Proxy* p = somSelf->ProxyFromId(ev, id);
  if (p == kODNULL) return;

  TempODFrame frame = p->frame->AcquireContainingFrame(ev);
  ODFacet* facet = FirstFacet(ev, frame);

  TempODTransform newTransform = frame->CreateTransform(ev);
  {
    ODMatrix odmx;
    odmx.m[0][0] = mx->fxM11;
    odmx.m[0][1] = mx->fxM12;
    odmx.m[0][2] = 0;
    odmx.m[1][0] = mx->fxM21;
    odmx.m[1][1] = mx->fxM22;
    odmx.m[1][2] = 0;
    odmx.m[2][0] = MAKEFIXED(mx->lM31,0);
    odmx.m[2][1] = MAKEFIXED(mx->lM32,0);
    odmx.m[2][2] = 0x40000000;              // kODFract1;
    newTransform->SetMatrix( ev, &odmx );
  }

  // if transform has changed then update facets with new transform
  if (!p->transform->IsSameAs(ev, newTransform)) {

    p->transform->CopyFrom(ev, newTransform);

    ODFrameFacetIterator* facets = p->frame->CreateFacetIterator(ev);
    for (ODFacet* f1 = facets->First(ev);
          facets->IsNotComplete(ev); f1 = facets->Next(ev))

      f1->ChangeGeometry(ev, kODNULL, newTransform, kODNULL);

    delete facets;
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving TransformEmbed\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDeleteEmbed(GraphicsPart *somSelf,
                                                 Environment *ev,
                                                ODULong id)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DeleteEmbed");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DeleteEmbed\n");
  #endif

  SOM_TRY

  if (!ev) ev = SOM_ENVIRONMENT;

  Proxy* p = somSelf->ProxyFromId(ev, id);
  if (p == kODNULL) return;

  RemoveFrameFacets( ev, p->frame );
  somSelf->RemoveEmbeddedFrame(ev, p->frame);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DeleteEmbed\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDrawEmbed( GraphicsPart *somSelf,
                                               Environment *ev, ODULong id,
                                               ODFacet* containingFacet )
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DrawEmbed");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DrawEmbed\n");
  #endif

  SOM_TRY

  if (!ev) ev = SOM_ENVIRONMENT;

  Proxy* p = somSelf->ProxyFromId(ev, id);
  if (p == kODNULL) return;

  ODCanvas* canvas = containingFacet->GetCanvas(ev);
  ODBoolean onScreen = canvas->IsDynamic(ev);

  HPS hpsDraw;
//  if (!onScreen)
//    hpsDraw = canvas->GetPlatformCanvas(ev, kODGPI)->GetPS(ev, containingFacet);

  // for now, iterate over ALL embed frame's facets
  ODFrameFacetIterator* facets = p->frame->CreateFacetIterator(ev);
  for (ODFacet* f1 = facets->First(ev);
        facets->IsNotComplete(ev); f1 = facets->Next(ev)) {
    if (f1->GetContainingFacet(ev)==containingFacet) {

      if (onScreen) {
        f1->Update(ev, kODNULL, canvas);
      } else {
#if 0
        hpsDraw = canvas->GetPlatformCanvas(ev, kODGPI)->GetPS(ev, f1);

        LONG lPSid = GpiSavePS( hpsDraw );

        ULONG ec = WinGetLastError ( (HAB)0 );
        GpiResetPS( hpsDraw, GRES_ATTRS );
        ec = WinGetLastError ( (HAB)0 );

//        MATRIXLF mx;
//        GpiQueryModelTransformMatrix( hpsDraw, 9, &mx );
//        GpiQueryDefaultViewMatrix( hpsDraw, 9, &mx );

        GpiSetDefaultViewMatrix( hpsDraw, 9, &mx0, TRANSFORM_REPLACE );
      //  GpiSetModelTransformMatrix(hpsDraw, 0, NULL, TRANSFORM_REPLACE);
        GpiSetModelTransformMatrix( hpsDraw, 9, &mx0, TRANSFORM_REPLACE );
#endif
        f1->Update(ev, kODNULL, kODNULL);
#if 0
        GpiRestorePS( hpsDraw, lPSid );
        canvas->GetPlatformCanvas(ev, kODGPI)->ReleasePS(ev, f1);
#endif
      }
    }
  }
  delete facets;

  SOM_CATCH_ALL
  SOM_ENDTRY

//  if (!onScreen)
//    canvas->GetPlatformCanvas(ev, kODGPI)->ReleasePS(ev, containingFacet);

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DrawEmbed\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartEmbedHit(GraphicsPart *somSelf,
                                                   Environment *ev,
                                                  ODULong id,
                                                  POINTL* ptl)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","EmbedHit");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering EmbedHit\n");
  #endif

  ODBoolean hit;

  SOM_TRY

  if (!ev) ev = SOM_ENVIRONMENT;

  Proxy* p = somSelf->ProxyFromId(ev, id);
  if (p == kODNULL) return FALSE;

  #if 1
    TempODShape embedShape = ODCopyAndRelease(ev, p->frame->AcquireUsedShape(ev, kODNULL));
    TempODTransform xform = FirstFacet(ev,p->frame)->AcquireWindowFrameTransform(ev, kODNULL);

    // put it in window coords
    embedShape->Transform(ev, xform);
    HRGN rgn = embedShape->CopyRegion(ev);

  #else
    POINTL offset;  // = frameOffset;
    QueryFrameOffset( ev, p->frame, &offset );

    // ggg
    HRGN rgn = p->frame->AcquireUsedShape(ev, kODNULL)->CopyRegion(ev);
    GpiOffsetRegion( _hpsMem, rgn, &offset );
  #endif

  hit = (GpiPtInRegion(_hpsMem, rgn, ptl) == PRGN_INSIDE);

  // destroy the Region!
  GpiDestroyRegion(_hpsMem, rgn);

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving EmbedHit\n");
  #endif

  SOM_CATCH_ALL
  SOM_ENDTRY

  return hit;
}

//------------------------------------------------------------------------------
SOM_Scope HRGN  SOMLINK GraphicsPartQueryEmbedRgn( GraphicsPart *somSelf,
                                                   Environment *ev,
                                                   ODULong id,
                                                   ODFacet* containingFacet)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","QueryEmbedRgn");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering QueryEmbedRgn\n");
  #endif

  HRGN rgn;

  SOM_TRY

  if (!ev) ev = SOM_ENVIRONMENT;

  Proxy* p = somSelf->ProxyFromId(ev, id);
  if (p == kODNULL) return 0;

  // this abort is for times where this is called before embeds are ready...
  ODFacet* embFacet = ContainedFacet( ev, p->frame, containingFacet );
  if (embFacet == kODNULL) return 0;

  TempODShape embedShape = ODCopyAndRelease(ev, p->frame->AcquireUsedShape(ev, kODNULL));

  // transform to window coordinates
  TempODTransform xform = embFacet->AcquireWindowFrameTransform(ev, kODNULL);
  embedShape->Transform(ev, xform);

  rgn = embedShape->CopyRegion(ev);
  POINTL offset = {0,1};
  GpiOffsetRegion( _hpsMem, rgn, &offset );

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving QueryEmbedRgn\n");
  #endif

  return rgn;
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartSetEmbedRgn( GraphicsPart *somSelf,
                                                 Environment *ev, ODULong id,
                                                 ODFacet* containingFacet,
                                                 HRGN rgn )
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","SetEmbedRgn");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering SetEmbedRgn\n");
  #endif

  SOM_TRY

  if (!ev) ev = SOM_ENVIRONMENT;

  Proxy* p = somSelf->ProxyFromId(ev, id);
  if (p == kODNULL) return;

  // this abort is for times where this is called before embeds are ready...
  ODFacet* embFacet = ContainedFacet( ev, p->frame, containingFacet );
  if (embFacet == kODNULL) return;

  // get the container's UsedShape, in Window coords
  ODFrame* frame = containingFacet->GetFrame(ev);

  TempODShape frameShape = ODCopyAndRelease(ev, frame->AcquireUsedShape(ev, kODNULL));

  ODTransform* xform = containingFacet->AcquireWindowFrameTransform(ev, kODNULL);
  frameShape->Transform(ev, xform);    // now in window coordinates
  ODReleaseObject(ev, xform);

  // convert the clip Region to a Shape and intersect it with the working area
  TempODShape clipShape = frame->CreateShape(ev);
  clipShape->SetRegion(ev, rgn);

  // nnn 12/2 FIX
  clipShape->Intersect(ev, frameShape);

  // transform the clip shape into embed's coords and Intersect
  xform = ODCopyAndRelease(ev, embFacet->AcquireWindowFrameTransform(ev, kODNULL) );
  xform->Invert(ev);
  clipShape->Transform(ev, xform);          // now in embed frame coordinates
  ODReleaseObject(ev, xform);

  TempODShape usedShape = p->frame->AcquireUsedShape(ev, kODNULL);
  clipShape->Intersect(ev, usedShape);

  // apply the new clip shape to the embed
  embFacet->ChangeGeometry(ev, clipShape, kODNULL, kODNULL);  // facet now owns shape storage

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving SetEmbedRgn\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartInitDrag(GraphicsPart *somSelf,
                                                   Environment *ev,
                                                  ODEventData* event)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","InitDrag");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering InitDrag\n");
  #endif

  ODBoolean handled = kODTrue;

  SOM_TRY

  event = &_baseEvent;
  event->msg = WM_BUTTON2DOWN;

  ODBoolean singleEmbed = kODFalse;
  Proxy* selection = 0;
  RECTL rclBounds;

  GtkObject* obj = _picture->HitObject();
  if (obj && obj->IsEmbed()) {

    selection = somSelf->ProxyFromId( ev, obj->Id() );
    singleEmbed = kODTrue;

  } else {

    if (obj) {
      rclBounds = obj->Bounds();
      InflateRect( &rclBounds, BOUNDS_BORDER, BOUNDS_BORDER );
    } else {
      rclBounds = *_picture->SelectedBounds();
    }
  }

  ODFrame* frame = _fCurFrame ? _fCurFrame : (ODFrame*)_fDisplayFrames->First();
  ODFacet* facet = FirstFacet( ev, frame);

  ODPart* destPart;
  ODPoint pt = {0, 0};
  Point mdOffset = {0,0};
  HRGN  dragRgn, tempRgn;
  ODID  frameInfoData;

  ODPOINTL odptlMouse(SHORT1FROMMP(event->mp1), SHORT2FROMMP(event->mp1));
  _fDragStart = odptlMouse;
  GpiConvert( _fsmHps, CVTC_DEVICE, CVTC_WORLD, 1, &_fDragStart );

  POINTL pts[13];
  RECTL shapeRect;
  if (singleEmbed) {

    TempODShape trackingShape = ODCopyAndRelease(ev, selection->frame->AcquireFrameShape(ev, kODNULL));
    trackingShape->Transform(ev, selection->transform);

    TempODTransform xform = facet->AcquireWindowFrameTransform(ev, kODNULL);
    trackingShape->Transform(ev, xform);

    GpiQueryRegionBox( _hpsMem, trackingShape->GetRegion(ev), &shapeRect);

  } else {

    shapeRect = rclBounds;
    InflateRect( &shapeRect, (-BOUNDS_BORDER), (-BOUNDS_BORDER) );
    GpiConvert( _fsmHps, CVTC_WORLD, CVTC_DEVICE, 2, (PPOINTL)&shapeRect );
  }

  shapeRect.xLeft   -= odptlMouse.x;
  shapeRect.xRight  -= odptlMouse.x;
  shapeRect.yBottom -= odptlMouse.y;
  shapeRect.yTop    -= odptlMouse.y;

  // specify inner box
  pts[ 0].x = 0;   pts[0].y = 0;
  pts[ 1].x = shapeRect.xLeft ;  pts[ 1].y = shapeRect.yBottom;
  pts[ 2].x = shapeRect.xLeft ;  pts[ 2].y = shapeRect.yTop;
  pts[ 3].x = shapeRect.xRight;  pts[ 3].y = shapeRect.yTop;
  pts[ 4].x = shapeRect.xRight;  pts[ 4].y = shapeRect.yBottom;
//  pts[ 5].x = shapeRect.xLeft ;  pts[ 5].y = shapeRect.yBottom;
  pts[ 5] = pts[1];
  // do another slightly larger box
  pts[ 6].x = pts[ 1].x - 2   ;  pts[ 6].y = pts[ 1].y - 2;
  pts[ 7].x = pts[ 2].x - 2   ;  pts[ 7].y = pts[ 2].y + 2;
  pts[ 8].x = pts[ 3].x + 2   ;  pts[ 8].y = pts[ 3].y + 2;
  pts[ 9].x = pts[ 4].x + 2   ;  pts[ 9].y = pts[ 4].y - 2;
  pts[10].x = pts[ 5].x - 2   ;  pts[10].y = pts[ 5].y - 2;
  // return to inner box
//  pts[11].x = shapeRect.xLeft ;  pts[11].y = shapeRect.yBottom;
  pts[11] = pts[1];
  // return to origin to erase xor vector out to edge of box
//  pts[12].x = 0;  pts[12].y = 0;
  pts[12] = pts[0];

  DRAGIMAGE dimg;                         // DRAGIMAGE structure
  dimg.cb     = sizeof(DRAGIMAGE);        // Size control block
  dimg.fl     = DRG_POLYGON;              // Flags passed to DrgDrag
  dimg.cptl   = 13;                       // number of points
  dimg.hImage = (ULONG)(PPOINTL)pts;

  dimg.cxOffset = 0;                      // Offset of the origin of the
  dimg.cyOffset = 0;                      // image from the pointer hotspot

  // *** Fill DAD SU with Props n' Vals
  ODDragAndDrop*   dad = _fSession->GetDragAndDrop(ev);
  dad->Clear(ev);

  ODStorageUnit*   unit = dad->GetContentStorageUnit(ev);
  ODDraft* fromDraft = somSelf->GetStorageUnit(ev)->GetDraft(ev);

  ODDraftKey key = fromDraft->BeginClone(ev, unit->GetDraft(ev), kODNULL, kODCloneCopy);

  if (singleEmbed) {
    TempODPart part = selection->frame->AcquirePart(ev);
    part->CloneInto(ev, key, unit, selection->frame);
  } else {
    InitPropVal( ev, unit, kODPropContents, kKindGraphicsPart );
    _picture->WriteToStorage(ev, unit, key, EXTERNALIZE_SELECTED);

    // now write a linkspec obtained from _picture
    ODByteArray* LinkSpecData = _picture->LinkSpec();

    if(LinkSpecData) {
      ODLinkSpec* linkSpec = fromDraft->CreateLinkSpec(ev, somSelf, LinkSpecData);
      if(linkSpec) {
        InitPropVal( ev, unit, kODPropLinkSpec, kODLinkSpec );
        linkSpec->WriteLinkSpec(ev, unit);
//        ODReleaseObject(ev, linkSpec);
      }
//    DisposeByteArray(LinkSpecData);  // remove byte array
    }
  }

  fromDraft->EndClone(ev, key);

  // lets write out a suggested frame shape.
  ODShape* frameShape;

  if (singleEmbed) {

    // *** (optional) save offset between mousedown pt and topLeft pt of selection
    // get the lower-left corner of the embed (in frame coords)
    ODPoint botLeft;
    selection->transform->GetOffset(ev, &botLeft);

    // get the mouse position in frame coords
    ODPoint offset(odptlMouse);  // offset of mouse in window coords
    TempODTransform xform = facet->AcquireWindowFrameTransform(ev, kODNULL);
    xform->InvertPoint(ev, &offset);

    // difference is the offset from cursor to embed LL corner (in frame coords)
    offset.x -= botLeft.x;
    offset.y -= botLeft.y;

    // write it out
#ifdef  OLDMOUSEOFFSET
    unit->AddProperty(ev, kPropMouseDownOffset)->AddValue(ev, kODPoint);
#else
    unit->AddProperty(ev, kODPropMouseDownOffset)->AddValue(ev, kODPoint);
#endif
    StorageUnitSetValue( unit, ev, sizeof(mdOffset), (ODValue) &offset);

    // *** (optional) if dragging one frame, save its external transform (not aggregate)

    ODMatrix m;
    selection->transform->GetMatrix(ev, &m);
    unit->AddProperty(ev, kODPropExternalTransform)->AddValue(ev, kODTransform);
    StorageUnitSetValue( unit, ev, sizeof(m), (ODValue) &m);

    frameShape = selection->frame->AcquireFrameShape(ev, kODNULL);

  } else {

    // get the mouse position in frame coords
    ODPoint offset(odptlMouse);  // offset of mouse in window coords
    ODTransform* xform = facet->AcquireWindowFrameTransform(ev, kODNULL);
    xform->InvertPoint(ev, &offset);
    ODReleaseObject(ev, xform);

    // write it out
#ifdef  OLDMOUSEOFFSET
    unit->AddProperty(ev, kPropMouseDownOffset)->AddValue(ev, kODPoint);
#else
    unit->AddProperty(ev, kODPropMouseDownOffset)->AddValue(ev, kODPoint);
#endif
    StorageUnitSetValue( unit, ev, sizeof(mdOffset), (ODValue) &offset);

    // get the selection offset in window coords
    POINTL offset2 = {rclBounds.xLeft, rclBounds.yBottom};
    GpiConvert( _fsmHps, CVTC_WORLD, CVTC_DEVICE, 1, &offset2 );
    offset2.x = odptlMouse.x - offset2.x;
    offset2.y = odptlMouse.y - offset2.y;

    // write it out
    unit->AddValue(ev, kGtkPOINTL);
    StorageUnitSetValue( unit, ev, sizeof(offset2), (ODValue)&offset2);

    frameShape = frame->CreateShape(ev);

    // get shape in world coords
    ODRect newBounds = downcastToODRECTL(rclBounds);
    frameShape->SetRectangle(ev, &newBounds);

    // move shape into frame coords  (easier way??)
    xform = frame->AcquireInternalTransform(ev, kODNULL);
    frameShape->Transform(ev, xform);
    ODReleaseObject(ev, xform);
  }

  // write out the requested frame shape (can be ignored...)
  unit->AddProperty(ev, kODPropFrameShape);
  frameShape->WriteShape(ev, unit);

  ODReleaseObject(ev, frameShape);

  _picture->SetDragging(kODTrue, obj);

  ODByteArray dragImgBA = CreateByteArrayStruct(&dimg, sizeof(DRAGIMAGE));
  ODByteArray eventBA = CreateByteArrayStruct(&event, sizeof(ODEventData*));

  ODDropResult dropResult;
  dropResult = dad->StartDrag( ev, frame, 0, &dragImgBA, &destPart, &eventBA);

  DisposeByteArrayStruct(&dragImgBA); // [119548]
  DisposeByteArrayStruct(&eventBA);

  _picture->SetDragging(kODFalse, obj);

  if (dropResult == kODDropMove && destPart != _fPartWrapper) {

    somSelf->InvalidateObject( ev, obj );
    _picture->Delete( obj );

    somSelf->PartUpdated( ev, frame, PUF_CLIPEMBEDS | PUF_UPDATELINKS );
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving InitDrag\n");
  #endif

  return handled;
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartResize(GraphicsPart *somSelf,
                                            Environment *ev,
                                           ODFrame* frame, ODULong ulMode)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","Resize");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering Resize\n");
  #endif

  SOM_TRY

  // here we assume that we adjust the pic bounds to the change in frame
  // shape (no scaling or stretching of the contents
  ODFacet* facet = FirstFacet(ev,frame);
  if (facet) {
    TempODShape frameShape = ODCopyAndRelease(ev, frame->AcquireFrameShape(ev, kODNULL));
    ODTransform* xform = facet->AcquireWindowFrameTransform(ev, kODNULL);
    frameShape->Transform(ev, xform);
    ODReleaseObject(ev, xform);

    HRGN  hrgnFrame = (HRGN)frameShape->GetRegion(ev);
    GpiQueryRegionBox( _hpsMem, hrgnFrame, &_rclFrame);

    if (frame->GetViewType(ev) == FRAME_VIEW)
      _rclFrameView = _rclFrame;

    xform = ODCopyAndRelease(ev, facet->AcquireContentTransform(ev, kODNULL));
    xform->Invert(ev);
    frameShape->Transform(ev, xform);
    ODReleaseObject(ev, xform);

    hrgnFrame = (HRGN)frameShape->GetRegion(ev);
    RECTL rclPic;
    GpiQueryRegionBox( _hpsMem, hrgnFrame, &rclPic);

    _picture->SetBounds(&rclPic);
    SetScaling2( ev, facet, _fsmHps );
  }

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving Resize\n");
  #endif
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartDoPopup(GraphicsPart *somSelf,
                                             Environment *ev,
                                            ODFacet* facet, POINTL* ptl)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","DoPopup");

  #ifdef ODDebug2
    somPrintf("GtkPart: Entering DoPopup\n");
  #endif

  SOM_TRY

  ODFrame *frame = facet->GetFrame(ev);

  if ( !FrameIsActive(ev, frame) )
    somSelf->ActivateFrame(ev, frame);

  // string for loading and setting text from Resource module
  char str255[255];
  POINTL   ptlWorld;

  BOOL notFrame = !(frame->GetViewType(ev) == FRAME_VIEW);

  ptlWorld = _ptlUpDown;

  ODPopup *fPopup = _fSession->GetWindowState(ev)->CopyBasePopup(ev);

  // READONLY, leave the default popup
  if ( _fReadOnly ) {

  // part background popup
  } else if ( frame->GetViewType(ev) != FRAME_VIEW     ||
            !_picture->Correlate(_fsmHps, &ptlWorld)    ) {

    fPopup->RemoveMenuItem(ev, kODNULL, HELP_SEPARATOR1 );

    // menuitem structure for inserting additional items
    ODPlatformMenuItem mi;
    memset((PCH)&mi, 0, sizeof(MENUITEM));

    mi.afStyle = MIS_SEPARATOR;
    mi.id = -1;
    fPopup->AddMenuItemBefore(ev, kODNULL, kODNULL, &mi, IDMS_HELP );

    mi.afStyle = MIS_TEXT;

    mi.id = VIEW_BGND_COLOR;
    fPopup->AddMenuItemBefore(ev, kODNULL, kODNULL, &mi, IDMS_HELP );
    char* pch;
    if (_fMenuBar) _fMenuBar->GetMenuItemText(ev, kODNULL, mi.id, &pch);
    else pch = "~Background color...";
    fPopup->SetMenuItemText(ev, kODNULL, mi.id, pch);
    SetPopupStatusTextFromResource( VIEW_BGND_COLOR, 260 );

    mi.id = VIEW_SHOWTOOLS;
    fPopup->AddMenuItemBefore(ev, kODNULL, kODNULL, &mi, IDMS_HELP );
    fPopup->SetMenuItemText(ev, kODNULL, mi.id,
                            _fToolsVisible ? "Hide ~toolbar" : "Show ~toolbar" );
    SetPopupStatusTextFromResource( VIEW_SHOWTOOLS, _fToolsVisible ? 262 : 261 );

    mi.afStyle = MIS_SEPARATOR;
    mi.id = -1;
    fPopup->AddMenuItemBefore(ev, kODNULL, kODNULL, &mi, IDMS_HELP );

  // object popup
  } else {

    // first we should remove the non-appropriate popup items
    fPopup->RemoveMenu(ev, VIEW_OPENAS );
    fPopup->RemoveMenu(ev, VIEW_PROPERTIES );
    fPopup->RemoveMenu(ev, VIEW_SHOWAS );
    fPopup->RemoveMenu(ev, IDMS_HELP );
    fPopup->RemoveMenuItem(ev, kODNULL, HELP_SEPARATOR1 );

    // menuitem structure for inserting additional items
    ODPlatformMenuItem mi;
    memset((PCH)&mi, 0, sizeof(MENUITEM));

    mi.afStyle = MIS_TEXT;
    mi.id = IDM_POPUP_DELETE;
    fPopup->AddMenuItemLast(ev, kODNULL, kODNULL, &mi);
    fPopup->SetMenuItemText(ev, kODNULL, mi.id, "~Delete");
    SetPopupStatusTextFromResource( IDM_POPUP_DELETE, 284 );

    mi.id = IDM_POPUP_ROTATE;
    fPopup->AddMenuItemLast(ev, kODNULL, kODNULL, &mi);
    fPopup->SetMenuItemText(ev, kODNULL, mi.id, "~Rotate");
    SetPopupStatusTextFromResource( IDM_POPUP_ROTATE, 292 );

    mi.afStyle = MIS_SEPARATOR;
    mi.id = -1;
    fPopup->AddMenuItemLast(ev, kODNULL, kODNULL, &mi);

    ULONG ulObjFlags, ulStateFlags;
    _picture->ClassifySelection( &ulObjFlags, &ulStateFlags );

    HWND hwndSubmenu;
    static LONG fills[] = {15, 16, 17, 1 , 10, 9 , 18, 19, 12, 14 };
    GtkObject* selected = _picture->HitObject();

    // group objects can have attribs set on grouped objects, but we cannot
    // query the current attribute values for menu item checkmarks... same
    // as for any multiple selection
    if (selected && (ulObjFlags&GOT_GROUP)!=0)
      selected = kODNULL;

    if ((ulObjFlags&LINE_OBJECTS)!=0) {
      hwndSubmenu = WinLoadMenu( HWND_OBJECT, _hmod, ID_LINESTUFFMENU );
      fPopup->AddMenuLast( ev, ID_LINESTUFFMENU, hwndSubmenu, somSelf );
      fPopup->SetMenuItemText(ev, ID_LINESTUFFMENU, kODNULL, "~Line");
      SetPopupStatusTextFromResource( ID_LINESTUFFMENU, 299 );
      SetPopupStatusTextFromResource( IDM_LINE_COLOR, 285 );
      SetPopupStatusTextFromResource( IDM_LINE_STYLES, 287 );
      SetPopupStatusTextFromResource( IDM_LINE_WIDTHS, 288 );
      DosFreeResource( (PVOID)hwndSubmenu );

      if (selected) {
        PLINESTUFF ls = selected->QueryLinestuff();
        fPopup->CheckMenuItem( ev, ID_LINESTUFFMENU,
                                IDM_LS_SOLID+ls->style-LINETYPE_SOLID, kODTrue );
        fPopup->CheckMenuItem( ev, ID_LINESTUFFMENU,
                                ls->width==LINEWIDTH_NORMAL ? IDM_LW_THIN
                                                            : IDM_LW_THICK, kODTrue );
      }
    }

    if ((ulObjFlags&FILL_OBJECTS)!=0) {
      hwndSubmenu = WinLoadMenu( HWND_OBJECT, _hmod, ID_FILLSTUFFMENU );
      fPopup->AddMenuLast( ev, ID_FILLSTUFFMENU, hwndSubmenu, somSelf );
      fPopup->SetMenuItemText(ev, ID_FILLSTUFFMENU, kODNULL, "~Fill");
      DosFreeResource( (PVOID)hwndSubmenu );
      SetPopupStatusTextFromResource( ID_FILLSTUFFMENU, 300 );
      SetPopupStatusTextFromResource( IDM_FILL_COLOR, 286 );
      SetPopupStatusTextFromResource( IDM_FILL_STYLES, 289 );

      if (selected) {
        PFILLSTUFF fs = selected->QueryFillstuff();
        for (int i=0; i<10; i++)
          if (fills[i]==fs->style) break;
        fPopup->CheckMenuItem( ev, ID_FILLSTUFFMENU, IDM_FS_NONE+i, kODTrue );
      }
    }

    if ((ulObjFlags&SHADOW_OBJECTS)!=0) {
      hwndSubmenu = WinLoadMenu( HWND_OBJECT, _hmod, ID_SHADOWSTUFFMENU );
      fPopup->AddMenuLast( ev, ID_SHADOWSTUFFMENU, hwndSubmenu, somSelf );
      fPopup->SetMenuItemText(ev, ID_SHADOWSTUFFMENU, kODNULL, "~Shadow");
      DosFreeResource( (PVOID)hwndSubmenu );
      SetPopupStatusTextFromResource( ID_SHADOWSTUFFMENU, 290 );

      if (selected) {
        LONG shadow;
        selected->QueryAttribute( SHADOW_STYLE, &shadow );
        for (int i=0; i<10; i++)
          if (fills[i]==shadow) break;
        fPopup->CheckMenuItem( ev, ID_SHADOWSTUFFMENU, IDM_SH_NONE+i, kODTrue );
      }
    }

    if ((ulObjFlags&GOT_ARC)!=0) {
      hwndSubmenu = WinLoadMenu( HWND_OBJECT, _hmod, ID_ARCSTUFFMENU );
      fPopup->AddMenuLast( ev, ID_ARCSTUFFMENU, hwndSubmenu, somSelf );
      fPopup->SetMenuItemText(ev, ID_ARCSTUFFMENU, kODNULL, "~Arc");
      DosFreeResource( (PVOID)hwndSubmenu );
      SetPopupStatusTextFromResource( ID_ARCSTUFFMENU, 291 );

      if (selected) {
        LONG closing;
        selected->QueryAttribute( ARC_CLOSING, &closing );
        fPopup->CheckMenuItem( ev, ID_ARCSTUFFMENU,
                                IDM_CLOSE_NONE+closing, kODTrue );
      }
    }
  }

  fPopup->Display(ev);

  SOM_CATCH_ALL
  SOM_ENDTRY

  #ifdef ODDebug2
    somPrintf("GtkPart: Leaving DoPopup\n");
  #endif
}

