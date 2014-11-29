
// dive.inl
//
//    This file contains most of the inline member implementations.


#ifndef _DIVE_INL_
#define _DIVE_INL_



inline ULONG Dive::allocImageBuffer( FOURCC fccColorSpace,
                                     ULONG  ulWidth,
                                     ULONG  ulHeight,
                                     PBYTE  pbImageBuffer,
                                     ULONG  ulLineSizeBytes )
{
   ULONG ulBufferNumber;

   ULONG rc = DiveAllocImageBuffer( hDiveInst,
                                    &ulBufferNumber,
                                    fccColorSpace,
                                    ulWidth,
                                    ulHeight,
                                    ulLineSizeBytes,
                                    pbImageBuffer     );

   return ulBufferNumber;    // very bad, no checking exceptions.
}



inline PBYTE Dive::beginImageBufferAccess( ULONG ulBufferNumber )
{
   PBYTE pbImageBuffer;
   ULONG ulBufferScanLineBytes;
   ULONG ulBufferScanLines;

   ULONG rc = DiveBeginImageBufferAccess( hDiveInst,
                                          ulBufferNumber,
                                          &pbImageBuffer,
                                          &ulBufferScanLineBytes,
                                          &ulBufferScanLines      );

   return pbImageBuffer;     // once again, shame on me.
}



inline void Dive::close()
{
   ULONG rc = DiveClose( hDiveInst );
}



inline void Dive::endImageBufferAccess( ULONG ulBufferNumber )
{
   ULONG rc = DiveEndImageBufferAccess( hDiveInst,
                                        ulBufferNumber );
}



inline void Dive::freeImageBuffer( ULONG ulBufferNumber )
{
   ULONG rc = DiveFreeImageBuffer( hDiveInst,
                                   ulBufferNumber );
}



inline void Dive::open( BOOL fNonScreenInstance )
{
   ULONG rc = DiveOpen( &hDiveInst,
                        fNonScreenInstance,
                        0                    );   // 0 for now...
}



inline void Dive::setDestinationPalette( PBYTE pbRGB2Entries,
                                         ULONG ulStartIndex,
                                         ULONG ulNumEntries )
{
   ULONG rc = DiveSetDestinationPalette( hDiveInst,
                                         ulStartIndex,
                                         ulNumEntries,
                                         pbRGB2Entries );
}



inline void Dive::setSourcePalette( PBYTE pbRGB2Entries,
                                    ULONG ulStartIndex,
                                    ULONG ulNumEntries )
{
   ULONG rc = DiveSetSourcePalette( hDiveInst,
                                    ulStartIndex,
                                    ulNumEntries,
                                    pbRGB2Entries );
}


#endif

