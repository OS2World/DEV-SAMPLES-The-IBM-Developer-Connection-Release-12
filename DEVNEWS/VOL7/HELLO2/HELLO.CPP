/******************************************************************************/
/*                                                                            */
/* COPYRIGHT:                                                                 */
/* ----------                                                                 */
/* Copyright (C) International Business Machines Corp., 1994.                 */
/* Copyright:(C) Apple Computer, Inc., 1994                                   */
/*                                                                            */
/* DISCLAIMER OF WARRANTIES:                                                  */
/* -------------------------                                                  */
/* The following [enclosed] code is sample code created by IBM                */
/* Corporation.  This sample code is not part of any standard IBM product     */
/* and is provided to you solely for the purpose of assisting you in the      */
/* development of your applications.  The code is provided "AS IS",           */
/* without warranty of any kind.  IBM shall not be liable for any damages     */
/* arising out of your use of the sample code, even if they have been         */
/* advised of the possibility of such damages.                                */
/*                                                                            */
/******************************************************************************/
/*
   File:    hello.cpp

   Contains:   Implementation of class HelloPart - Version 2
                For use with OpenDoc DK1 --- DEVCON7 Level

   Change History (most recent first):

        Final Version for Devcon7               Robert Tycast           4/08/95
        View as support started                 Robert Tycast           3/08/95 
        Final Version for DevCon6               Robert Tycast           1/06/95 
        Changed to SOM                          Robert Tycast          11/13/94 
        Ported to OS/2                          Dan Harkey              9/16/94
*/
#ifndef SOM_Module_hello_Source
#define SOM_Module_hello_Source
#endif
#define HelloPart_Class_Source

#define INCL_WIN   
#define INCL_WINERRORS
#define INCL_GPI
#define INCL_GPIERRORS
#define INCL_ODALTPOINT
#define INCL_ODARBITRATOR
#define INCL_ODCANVAS
#define INCL_ODFACET
#define INCL_ODFOCI
#define INCL_ODFOCUSSET
#define INCL_ODFRAME
#define INCL_ODSHAPE
#define INCL_ODSTDTYPES
#define INCL_ODSTDPROPS
#define INCL_ODTRANSFORM
#define INCL_ODSESSION
#define INCL_ODSTORAGEUNIT
//+
#define INCL_DOSMODULEMGR
#define INCL_DOSRESOURCES
//-
#include <os2.h>
#include <stdio.h>
#include <string.h>
//int SOM_TraceLevel = 1;
#include "hello.xih"
//+
#include "hello.h"
#include "common.hpp"
//-
#define kHelloWorldPart "IBM:Kind:HelloWorldPart"

//+  Some convenient Math Macros
#define double_t double
const ODFixed kODFixedHalf      = 0x00008000;
#define ODFixedToFloat(a)  ((ODFixed)(a) / 65536.0)
#define ODFloatToFixed(a)  ((ODFixed)((double_t)(a) * 65536.0))
#define ODIntToFixed(a)    ((ODFixed)(a) << 16)
#define ODFixedRound(a)    ((ODSShort)((ODFixed)(a) + kODFixedHalf >> 16))
//-
SOM_Scope void  SOMLINK HelloCommonInitPart(HelloPart *somSelf, 
                                             Environment *ev, 
                                            ODStorageUnit* su)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HelloCommonInitPart");

//+
   somThis->Session = su->GetSession(ev);
   somThis->FrameView     = somThis->Session->Tokenize(ev, kODViewAsFrame);
   somThis->LargeIconView   = somThis->Session->Tokenize(ev, kODViewAsLargeIcon);
   somThis->SmallIconView   = somThis->Session->Tokenize(ev, kODViewAsSmallIcon);
   somThis->ThumbnailView   = somThis->Session->Tokenize(ev, kODViewAsThumbnail);

//-
}

SOM_Scope void  SOMLINK HelloSetOrigin(HelloPart *somSelf,  Environment *ev, 
                                       ODFacet* facet)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HelloSetOrigin");

    ODTransform* localToGlobal = facet->GetContentTransform(ev);
    HPS hps = facet->GetCanvas(ev)->GetPlatformCanvas(ev);
    MATRIXLF mtx;
    facet->GetContentTransform(ev)->GetMATRIXLF(ev, &mtx);
    GpiSetModelTransformMatrix(hps, 9, &mtx, TRANSFORM_REPLACE);
    facet->GetCanvas(ev)->ReleasePlatformCanvas(ev);       
}

SOM_Scope void  SOMLINK HelloRenderPart(HelloPart *somSelf,  Environment *ev, 
                                        ODFacet* facet, RECTL* pframeRect, 
                                        HelloPart_HPS hpsDraw)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HelloRenderPart");

   // paint the background

   GpiResetPS(hpsDraw, GRES_ATTRS);

   somSelf->SetOrigin(ev, facet);

   // say "Hello World"

   FATTRS fat;
   POINTL grad = { 1, 4 };
   POINTL aptlPoints[TXTBOX_COUNT];
   LONG lcid = 101;
   memset(&fat, 0, sizeof(FATTRS));
   fat.usRecordLength = sizeof(FATTRS);
   strcpy(fat.szFacename, "Helv Bold");
   fat.fsFontUse = FATTR_FONTUSE_OUTLINE;

   FONTMETRICS fm;
   if (FONT_MATCH == GpiCreateLogFont( hpsDraw, NULL, lcid, &fat))
   {
      GpiSetCharSet(hpsDraw, lcid);
      GpiQueryFontMetrics(hpsDraw, sizeof(fm), &fm);
      GpiSetCharShear(hpsDraw, &grad);
      SIZEF sizfxBox ;
      sizfxBox.cy = MAKEFIXED(24,0);
      sizfxBox.cx = 24 * fm.lEmHeight * 0x10000 / fm.lEmInc;
      GpiSetCharBox(hpsDraw, &sizfxBox);
   }
   char   buffer[23];
   short  bufferlength;
   POINTL ptlString ;
   RECTL frameRect;

   GpiSetColor ( hpsDraw, CLR_BLACK );
   GpiSetTextAlignment(hpsDraw, TA_CENTER, TA_HALF);
   frameRect = *pframeRect;
   ptlString.x = frameRect.xRight/2;
   ptlString.y = frameRect.yTop;

   ptlString.y -= (fm.lMaxBaselineExt + fm.lMaxDescender)*2;
   bufferlength = sprintf(buffer,"Hello World!");
   GpiCharStringAt( hpsDraw, &ptlString, bufferlength, buffer);

   bufferlength = sprintf(buffer,"Click Rt. Mouse");
   ptlString.y -= (fm.lMaxBaselineExt + fm.lMaxDescender)*2;
   GpiCharStringAt ( hpsDraw, &ptlString, bufferlength, buffer);

   bufferlength = sprintf(buffer,"Button Here");
   ptlString.y -= (fm.lMaxBaselineExt + fm.lMaxDescender);
   GpiCharStringAt ( hpsDraw, &ptlString, bufferlength, buffer);

   bufferlength = sprintf(buffer,"%d", somThis->Iteration);
   ptlString.y -= (fm.lMaxBaselineExt + fm.lMaxDescender);
   GpiCharStringAt ( hpsDraw, &ptlString, bufferlength, buffer);

   GpiDeleteSetId(hpsDraw, lcid);
}

SOM_Scope void  SOMLINK HelloDrawFrameView(HelloPart *somSelf, 
                                            Environment *ev, 
                                           ODFacet* facet)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HelloDrawFrameView");
//+
   HelloPart_parent_ContainerPart_Draw(somSelf, ev, facet, kODNULL);
   ODFrame* displayFrame = facet->GetFrame(ev);

   // Get Presentation Space

   HPS hpsDraw = facet->GetCanvas(ev)->GetPlatformCanvas(ev);

   // Get Bounding Box

   ODRgnHandle frameRgn = displayFrame->GetFrameShape(ev)->GetRegion(ev);
   RECTL frameRect;
   GpiQueryRegionBox(hpsDraw, frameRgn, &frameRect);

   // set up clipping
   ODRgnHandle saveClip;
   ODShape* clipShape = new ODShape;
   clipShape->CopyFrom(ev, facet->GetAggregateClipShape(ev));
   clipShape->Transform(ev, facet->GetFrameTransform(ev));

// put it in PM terms

   ODRgnHandle clip = clipShape->GetRegion(ev);
   GpiSetClipRegion(hpsDraw, clip, &saveClip);

// Put your PM drawing routines here!
  
   somSelf->RenderPart(ev, facet, &frameRect, hpsDraw);  
     
   GpiSetClipRegion(hpsDraw, 0, &saveClip);
   facet->GetCanvas(ev)->ReleasePlatformCanvas(ev); 
   delete clipShape;
//-
}

SOM_Scope void  SOMLINK HelloDrawIconView(HelloPart *somSelf, 
                                           Environment *ev, ODFacet* facet, 
                                          ODULong size)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HelloDrawIconView");
//+
      HMODULE hmodDLL = kODNULL;
      BITMAPINFOHEADER2 bmp;

   ODFrame* displayFrame = facet->GetFrame(ev);

   ODRect rect;
   displayFrame->GetUsedShape(ev)->GetBoundingBox(ev, &rect);
   ODRECTL frameRect(rect);

   HPS hpsDraw;
   HBITMAP hbm, hbmNew, hbmOld;
   POINTL aptl[4];

   somSelf->SetOrigin(ev, facet);


   POINTL ptl = {frameRect.xRight, frameRect.yTop};

      DosQueryModuleHandle( "hello", &hmodDLL);
      HPS hps = facet->GetCanvas(ev)->GetPlatformCanvas(ev);
      hbm = GpiLoadBitmap(hps, hmodDLL, IDB_BMP1, size, size );

      bmp.cbFix = sizeof(bmp);
      if (GpiQueryBitmapInfoHeader( hbm, &bmp ) == GPI_OK)
      {
         aptl[0].x = frameRect.xLeft;
         aptl[0].y = frameRect.yBottom;
         aptl[1].x = frameRect.xRight;
         aptl[1].y = frameRect.yTop;
         aptl[2].x = aptl[2].y = 0;
         aptl[3].x = bmp.cx;
         aptl[3].y = bmp.cy;

         if (GpiWCBitBlt(hps, hbm, 4, aptl, ROP_SRCCOPY, BBO_IGNORE) == GPI_ERROR)
         {
            ULONG errid = (WinGetLastError(WinQueryAnchorBlock(HWND_DESKTOP)) & 0x0000FFFF);
            if (errid == PMERR_INCOMPATIBLE_BITMAP || errid == PMERR_INV_HBITMAP)
            {
               /* The DC associated with the target presentation space is for a
                * device that is not compatible with the screen DC (it may be a
                * printer or off-screen canvas).  Create a copy of the bitmap
                * that is compatible with the target DC and try the BitBlt again.
                */

    
               HAB hab = WinQueryAnchorBlock(HWND_DESKTOP);
               HDC  HdcMem = DevOpenDC( hab,OD_MEMORY,"*",0L,NULL,NULLHANDLE);
               SIZEL sizlPage = {0, 0};
               HPS  HpsMem = GpiCreatePS(hab, HdcMem, &sizlPage, PU_PELS | GPIA_ASSOC );
               ULONG cbBuffer, cbBitmapInfo;
               PBYTE pbBuffer;
               PBITMAPINFO2 pbmi;

               cbBuffer = (((bmp.cBitCount * bmp.cx) + 31) / 32) * 4 *  bmp.cy * bmp.cPlanes;
               cbBitmapInfo = sizeof(BITMAPINFO2) + (sizeof(RGB2) * (1 << bmp.cBitCount));
               DosAllocMem ((PPVOID)&pbmi, cbBitmapInfo + cbBuffer,
                             PAG_COMMIT | PAG_READ | PAG_WRITE);
               pbBuffer = (PBYTE)pbmi + cbBitmapInfo;
               memcpy(pbmi, &bmp, sizeof(bmp));
               hbmOld = GpiSetBitmap(HpsMem, hbm);
               GpiQueryBitmapBits(HpsMem, 0, bmp.cy, pbBuffer, pbmi);
               GpiSetBitmap(HpsMem, hbmOld);
               hbmNew = GpiCreateBitmap(hps, (PBITMAPINFOHEADER2)pbmi, CBM_INIT, pbBuffer, pbmi);
               if (hbmNew != GPI_ERROR)
               {
                  GpiWCBitBlt(hps, hbmNew, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);
                  GpiDeleteBitmap(hbmNew);
               }
               DosFreeMem(pbmi);
            }
         }
      }

    facet->GetCanvas(ev)->ReleasePlatformCanvas(ev);
//- 
}

SOM_Scope void  SOMLINK HelloDrawThumbnailView(HelloPart *somSelf, 
                                                Environment *ev, 
                                               ODFacet* facet)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HelloDrawThumbnailView");

// There is nothing special to do in our draw routine for a thumbnail.   All the work has already
// been done in "ViewTypeChanged."  However, we keep this routine anyway to preserve the
// code structure i.e. a draw routine per view type...

        somSelf->DrawFrameView(ev, facet);
//-
}

SOM_Scope void  SOMLINK HelloCalcAndSetScalingTransform(HelloPart *somSelf, 
                                                         Environment *ev, 
                                                        ODFrame* frame, ODTypeToken viewType)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HelloCalcAndSetScalingTransform");

//+
    ODShape* frameShape = frame->GetFrameShape(ev);
    ODTransform* scaledTransform = new ODTransform;
    ODFixed thumbnailSize = MAKEFIXED(64,0);             
    ODTransform* internalTransform; 
    ODPoint scale;
    ODFixed bottom;
    ODFixed side;
    ODFloat  m;
    ODFloat  n;
    ODFloat ratio; 
    ODRect boundingBox;                                 
             
    internalTransform = frame->GetInternalTransform(ev);     
    frameShape->GetBoundingBox(ev, &boundingBox);
    bottom = boundingBox.right-boundingBox.left;
    side = boundingBox.top-boundingBox.bottom;
    n = ODFixedToFloat(thumbnailSize);
    if (side >= bottom) m = ODFixedToFloat(side); else  m = ODFixedToFloat(bottom);
    ratio = m/n;
    scale.x = scale.y = ODFloatToFixed(ratio);
    scaledTransform->CopyFrom(ev, internalTransform);
    if (viewType != somThis->ThumbnailView) scaledTransform->ScaleBy( ev, &scale);  //scale up!
       else scaledTransform->ScaleDownBy( ev, &scale);
    frame->ChangeInternalTransform( ev, scaledTransform);
//-
}

SOM_Scope void  SOMLINK HellosomInit(HelloPart *somSelf)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HellosomInit");

    somThis->Iteration = 0;         //Set up a value to be externalized
    HelloPart_parent_ContainerPart_somInit(somSelf);
}

SOM_Scope void  SOMLINK HellosomUninit(HelloPart *somSelf)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HellosomUninit");

    HelloPart_parent_ContainerPart_somUninit(somSelf);
}

SOM_Scope void  SOMLINK HelloDraw(HelloPart *somSelf,  Environment *ev, 
                                  ODFacet* facet, ODShape* invalidShape)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HelloDraw");

//  HelloPart_parent_ContainerPart_Draw(somSelf, ev, facet, invalidShape);
//+

    ODShape* activeShape = new ODShape;
    activeShape->CopyFrom(ev, facet->GetFrame(ev)->GetUsedShape(ev));
    facet->ChangeActiveShape(ev, activeShape);
    ODFrame* frame = facet->GetFrame(ev);
    ODTypeToken view = frame->GetViewType(ev);
  
  if ( view == somThis->FrameView )
    somSelf->DrawFrameView(ev, facet);
  else if ( view == somThis->LargeIconView )
    somSelf->DrawIconView(ev, facet, 32);
  else if ( view == somThis->SmallIconView )
    somSelf->DrawIconView(ev, facet, 16);
  else if ( view == somThis->ThumbnailView )
    somSelf->DrawThumbnailView(ev, facet);
//-
}

SOM_Scope void  SOMLINK HelloExternalize(HelloPart *somSelf, 
                                          Environment *ev)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HelloExternalize");

    HelloPart_parent_ContainerPart_Externalize(somSelf, ev);

// The moment of truth: write out our persistent value!

   ODStorageUnit* su = somSelf->GetStorageUnit(ev);    
   su->Focus(ev, kODPropContents,kODPosUndefined,
                                kHelloWorldPart,0,kODPosUndefined);
   su->SetValue(ev, sizeof(somThis->Iteration), &somThis->Iteration);
}

SOM_Scope ODBoolean  SOMLINK HelloHandleEvent(HelloPart *somSelf, 
                                               Environment *ev, 
                                              ODEventData* event, 
                                              ODFrame* frame, 
                                              ODFacet* facet)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HelloHandleEvent");

   Point windowPoint;
   ODBoolean handled = kODFalse;

   switch (event->msg)
   {
      case WM_BUTTON2DOWN:
            {
               somThis->Iteration++;
               frame->Invalidate(ev, kODNULL);    // Arrange for a reDraw!
            }
            handled = kODTrue;
            break;

      default:
      break;
  //       return kODFalse;

   }
//   return handled;


    return (HelloPart_parent_ContainerPart_HandleEvent(somSelf, 
                                                       ev, event, 
                                                       frame, 
                                                       facet));
}

SOM_Scope void  SOMLINK HelloInitPart(HelloPart *somSelf,  Environment *ev, 
                                      ODStorageUnit* storageUnit)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HelloInitPart");
//+
    somSelf->CommonInitPart(ev, storageUnit);    
//-
// Setup a value in contents property in our storage unit
        
    storageUnit->AddProperty(ev, kODPropContents)->AddValue(ev, kHelloWorldPart);

    HelloPart_parent_ContainerPart_InitPart(somSelf, ev, storageUnit);
}

SOM_Scope void  SOMLINK HelloInitPartFromStorage(HelloPart *somSelf, 
                                                  Environment *ev, 
                                                 ODStorageUnit* storageUnit)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HelloInitPartFromStorage");

//+
    somSelf->CommonInitPart(ev, storageUnit);    
//-
    HelloPart_parent_ContainerPart_InitPartFromStorage(somSelf, 
                                                       ev, storageUnit);
   storageUnit->Focus(ev, kODPropContents,kODPosUndefined,kHelloWorldPart,
                        0,kODPosUndefined);
   storageUnit->GetValue(ev, storageUnit->GetSize(ev),&somThis->Iteration);
}


SOM_Scope void  SOMLINK HelloViewTypeChanged(HelloPart *somSelf, 
                                              Environment *ev, 
                                             ODFrame* frame)
{
    HelloPartData *somThis = HelloPartGetData(somSelf);
    HelloPartMethodDebug("HelloPart","HelloViewTypeChanged");

    HelloPart_parent_ContainerPart_ViewTypeChanged(somSelf, ev, 
                                                   frame);
//+

       ODTypeToken    view =  frame->GetViewType(ev);

  // The view field may be null, if so, we prefer to be displayed
  // in a frame view.

  if ( view == kODNullTypeToken )
      frame->SetViewType(ev, somThis->FrameView);

       ODRect thumbnailRect = { 0, MAKEFIXED(64,0), MAKEFIXED(64,0), 0};
       ODRect usedShapeRect;
       ODBoolean wasThumbnail;
       ODShape* newUsedShape = new ODShape();

       newUsedShape->CopyFrom(ev,frame->GetFrameShape(ev));

// The following routine is used to figure out if our previous view was a thumbnail, and thus
// we need to scaleup from the 64x64 size we set in the transform.  This is not the best
// way to do this.  A better way to do this would be to store the information in the frames
// partinfo.  But since our parent, the container part, does not yet have an accessor method we
// can use to add something to the frame's partinfo, we will have to use some workaround like 
// below.  

     frame->GetUsedShape(ev)->GetBoundingBox(ev, &usedShapeRect);
             if (usedShapeRect.right == thumbnailRect.right && usedShapeRect.left == thumbnailRect.left)
           wasThumbnail = kODTrue;

       if ( view == somThis->FrameView )
        {           
          frame->ChangeUsedShape(ev, kODNULL);  // Reset used shape to frame shape
          if (wasThumbnail == kODTrue) somSelf->CalcAndSetScalingTransform(ev, frame, view);  // Reset Transform 
        }
       else if ( view == somThis->LargeIconView )
            {
             ODRect Rect = { 0, MAKEFIXED(32,0), MAKEFIXED(32,0), 0};
             newUsedShape->SetRectangle(ev, &Rect);
             frame->ChangeUsedShape(ev, newUsedShape);
             if (wasThumbnail == kODTrue) somSelf->CalcAndSetScalingTransform(ev, frame, view);  // Reset Transform 
            }
       else if ( view == somThis->SmallIconView )
            {
             ODRect Rect = { 0, MAKEFIXED(16,0), MAKEFIXED(16,0), 0};
             newUsedShape->SetRectangle(ev, &Rect);
             frame->ChangeUsedShape(ev, newUsedShape);
             if (wasThumbnail == kODTrue) somSelf->CalcAndSetScalingTransform(ev, frame, view);  // Reset Transform 
            }

       else if ( view == somThis->ThumbnailView )
            {
             newUsedShape->SetRectangle(ev, &thumbnailRect);
             frame->ChangeUsedShape(ev, newUsedShape);
             if (wasThumbnail != kODTrue) somSelf->CalcAndSetScalingTransform(ev, frame, view);  // Reset Transform 
            }

// Set active shape to equal used shape
    ODFrameFacetIterator* facets = frame->CreateFacetIterator(ev);
    for (ODFacet* childFacet = facets->First(ev);
      facets->IsNotComplete(ev);
      childFacet = facets->Next(ev))
    {
    ODShape* newActiveShape = new ODShape;
    newActiveShape->CopyFrom(ev, newUsedShape);
    childFacet->ChangeActiveShape(ev, newActiveShape);
    }
    delete facets;

       frame->GetContainingFrame(ev)->Invalidate(ev, kODNULL); 
//-
  
}
