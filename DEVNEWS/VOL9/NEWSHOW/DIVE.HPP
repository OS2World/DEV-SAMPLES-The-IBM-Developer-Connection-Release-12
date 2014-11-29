
// dive.hpp
//
//    This file contains the Dive class definition.
//    Written by Larry Kyrala.


#ifndef _DIVE_HPP_
#define _DIVE_HPP_


class Dive {
   public:
     Dive();
     ~Dive();

         // high level functions built on top of wrappers...

     int loadImage( char* );
     void setupDestination( IWindow* );

     ISize getImageSize() { return ISize(setBlit.ulSrcWidth,
                                         setBlit.ulSrcHeight); }

         // pre-existing DIVE API wrappers...
         // all are implemented as inline for speed, see also dive.inl

     ULONG allocImageBuffer( FOURCC fccColorSpace,
                             ULONG  ulWidth,
                             ULONG  ulHeight,
                             PBYTE  pbImageBuffer = 0,
                             ULONG  ulLineSizeBytes = 0 );

     PBYTE beginImageBufferAccess( ULONG ulBufferNumber );

     void blitImage( ULONG ulSrcBufNumber,
                     ULONG ulDstBufNumber = DIVE_BUFFER_SCREEN );

     void close();

     void endImageBufferAccess( ULONG ulBufferNumber );

     void freeImageBuffer( ULONG ulBufferNumber );

     void open( BOOL fNonScreenInstance = FALSE );

     void setDestinationPalette( PBYTE pbRGB2Entries = 0,
                                 ULONG ulStartIndex = 0,
                                 ULONG ulNumEntries = 256 );

     void setSourcePalette( PBYTE pbRGB2Entries,
                            ULONG ulStartIndex = 0,
                            ULONG ulNumEntries = 256 );


     void setupBlitter( PSETUP_BLITTER pSetupBlitter = 0 );


  private:
     HDIVE             hDiveInst;
     SETUP_BLITTER     setBlit;
     RECTL             rctls[50];    // don't worry Mike, allocated as
                                     // obj space, not from stack.
};



inline void Dive::blitImage( ULONG ulSrcBufNumber,
                             ULONG ulDstBufNumber  )
{
   ULONG rc = DiveBlitImage( hDiveInst,
                             ulSrcBufNumber,
                             ulDstBufNumber );
}


inline void Dive::setupBlitter( PSETUP_BLITTER pSetupBlitter )
{
   ULONG rc = DiveSetupBlitter( hDiveInst,
                                pSetupBlitter );
}


#endif

