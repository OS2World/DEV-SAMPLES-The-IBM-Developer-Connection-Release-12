
// dive.cpp
//
//    This file contains the Dive class implementation


#ifndef _DIVE_CPP_
#define _DIVE_CPP_


// standard h

#include <string.h>


// os2 and mm h

#define INCL_MMIOOS2
#define INCL_GPI
#define INCL_GPIBITMAPS
#define INCL_GPIREGIONS

#include <os2.h>
#include <os2me.h>
#include <fourcc.h>
#include <dive.h>


// ioc h

#include <iwindow.hpp>
#include <ihandle.hpp>


#include "dive.hpp"
#include "dive.inl"



Dive::Dive()
{
   open();
   memset( &setBlit, '\0', sizeof(SETUP_BLITTER) );
}



Dive::~Dive()
{
   close();
}



int Dive::loadImage( char *filename )
{
   HMMIO    hmmio;
   MMIOINFO mmioinfo;
   ULONG    rc;

   memset( &mmioinfo, '\0', sizeof(MMIOINFO) );
   mmioinfo.ulTranslate = MMIO_TRANSLATEHEADER | MMIO_TRANSLATEDATA;

   hmmio = mmioOpen( filename, &mmioinfo, MMIO_READ );



   MMIMAGEHEADER mmImgHdr;
   LONG lBytesRead;

   rc = mmioGetHeader( hmmio,
                      (PVOID) &mmImgHdr,
                      (LONG)  sizeof(MMIMAGEHEADER),
                      (PLONG) &lBytesRead, 0L, 0L );

         // mmImgHdr.mmXDIBHeader.BMPInfoHeader2.cBitCount  **color depth**
         // assume 8 bit file for now!
   setBlit.fccSrcColorFormat = mmioFOURCC( 'L', 'U', 'T', '8' );

   setBlit.ulSrcWidth = mmImgHdr.mmXDIBHeader.BMPInfoHeader2.cx;
   setBlit.ulSrcHeight = mmImgHdr.mmXDIBHeader.BMPInfoHeader2.cy;

   setBlit.fInvert = TRUE;   // flip bmp orientation correctly


   PBYTE imagebuf;
   ULONG imageNum;

   imageNum = allocImageBuffer( setBlit.fccSrcColorFormat,
                                setBlit.ulSrcWidth,
                                setBlit.ulSrcHeight );

   imagebuf = beginImageBufferAccess( imageNum );

   rc = mmioRead( hmmio, (PCHAR) imagebuf,
                  mmImgHdr.mmXDIBHeader.BMPInfoHeader2.cbImage );

   endImageBufferAccess( imageNum );



   rc = mmioClose( hmmio, (USHORT) 0 );
   return imageNum;
}



void Dive::setupDestination( IWindow *dstWindow )
{
   setBlit.fccDstColorFormat = FOURCC_SCRN;

         // To stretch blit, use these two instead of...
   setBlit.ulDstWidth = dstWindow->size().width();
   setBlit.ulDstHeight = dstWindow->size().height();

         // These two.
 //  setBlit.ulDstWidth = setBlit.ulSrcWidth;
 //  setBlit.ulDstHeight = setBlit.ulSrcHeight;


   IPoint point( 0, 0 );
   IPoint mapPt( IWindow::mapPoint(point,
                                   dstWindow->handle(),
                                   IWindow::desktopWindow()->handle()) );

   setBlit.lScreenPosX = mapPt.x();
   setBlit.lScreenPosY = mapPt.y();



   RGNRECT  rgnCtl;
   HRGN     hrgn;
   HPS      hps;

   rgnCtl.ircStart = 1L;
   rgnCtl.crc = 50L;
   rgnCtl.ulDirection = RECTDIR_LFRT_TOPBOT;

   hps = IPresSpaceHandle( dstWindow->presSpace() );
   hrgn = GpiCreateRegion( hps, 0L, NULL );
   WinQueryVisibleRegion( dstWindow->handle(), hrgn );
   GpiQueryRegionRects( hps, hrgn, NULL, &rgnCtl, rctls );

   setBlit.ulNumDstRects = rgnCtl.crcReturned;
   setBlit.pVisDstRects = rctls;

   GpiDestroyRegion( hps, hrgn );
   dstWindow->releasePresSpace( hps );

         // everything ok, lock and load!
   setBlit.ulStructLen = sizeof (SETUP_BLITTER);
   setupBlitter( &setBlit );
}


#endif

