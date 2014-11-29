//
// IOProc.cpp
//
#ifndef _IOPROC_
#include "ioproc.hpp"
#endif

#define     LONG_MAX    0x7FFFFFFF

IOProc::IOProc(unsigned long nlsdata, unsigned long name_table, unsigned long fourcc,
           unsigned long IOProcType, unsigned long MediaType,
           unsigned long CapsFlags, char *DefaultFormatExt) :
        nls_data(nlsdata), ioproc_name_table(name_table), ulCapsFlags(CapsFlags),
        ulFourcc(fourcc), ulIOProcType(IOProcType), ulMediaType(MediaType)
 {
  memset( pDefaultExt, 0, 10);
  strncpy(pDefaultExt,DefaultFormatExt, 10);
 }

IOProc::~IOProc()
 {
 }

unsigned long IOProc::stub(MMIOINFO * pmmioinfo)
 {
  if(pmmioinfo)
    pmmioinfo->ulErrorRet = MMIOERR_UNSUPPORTED_MESSAGE;
  return (MMIOERR_UNSUPPORTED_MESSAGE);
 }

unsigned long IOProc::getModuleHandle() const
 {
  return hModuleHandle;
 }

IOProc& IOProc::setModuleHandle(unsigned long hMod)
 {
  hModuleHandle = hMod;
  return *this;
 }

unsigned long IOProc::getNLSDataID() const
 {
  return nls_data;
 }

unsigned long IOProc::getNameTableID() const
 {
  return ioproc_name_table;
 }

unsigned long IOProc::mmiomGetHeader(MMIOINFO * pmmioinfo, void *pHeader, long length)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomOpenFile(MMIOINFO * pmmioinfo, unsigned char *pFileName)
 {
   HMMIO           hmmioSS;
   FOURCC          fccStorageSystem;   // SS I/O Proc FOURCC
   MMIOINFO        mmioinfoSS;         // I/O info block for SS ref

   // Check for valid MMIOINFO block.
   if (!pmmioinfo)
      return (MMIO_ERROR);

   //
   // Determine the storage system/child IOProc that actually
   //   obtains the data for us.
   //
   if (!pmmioinfo->fccChildIOProc)
     {
      /* Need to determine SS if create from pmmioinfo and filename. */
      if (pmmioinfo->ulFlags & MMIO_CREATE)
         {
          if (mmioDetermineSSIOProc( (PSZ)pFileName, pmmioinfo, &fccStorageSystem, 0 ))
             fccStorageSystem = FOURCC_DOS;
         }
      else
         {
          if (mmioIdentifyStorageSystem( (PSZ)pFileName, pmmioinfo, &fccStorageSystem ))
             return (MMIO_ERROR);
         }

      if (!fccStorageSystem)
         return (MMIO_ERROR);
      else
         pmmioinfo->fccChildIOProc = fccStorageSystem;
     } // end storage system identification block

   //
   // Direct the open to the specific storage system necessary
   //
   memset( &mmioinfoSS, '\0', sizeof(MMIOINFO));
   memmove( &mmioinfoSS, pmmioinfo, sizeof(MMIOINFO));
   mmioinfoSS.pIOProc = 0;
   mmioinfoSS.fccIOProc = pmmioinfo->fccChildIOProc;
   mmioinfoSS.ulFlags |= MMIO_NOIDENTIFY;

   //
   // Try to open the file.  Add the NO IDENTIFY flag to
   //    ENSURE THAT WE DON'T LOOP RECURSIVELY!!!
   //
   hmmioSS = mmioOpen ((PSZ)pFileName, &mmioinfoSS, mmioinfoSS.ulFlags);

   //
   // Check if a DELETE was requested - mmioOpen returns a 1,
   //    so we much check this separately
   //
   if (pmmioinfo->ulFlags & MMIO_DELETE)
      {
       // was the delete successful?
       if (!hmmioSS)
         {
          pmmioinfo->ulErrorRet = MMIOERR_DELETE_FAILED;
          return (MMIO_ERROR);
         }
       else
          return (MMIO_SUCCESS);
      }

    //
    // Check the return code from the open call for an error.
    //    If not delete, then the open should have worked.
    //
    if (!hmmioSS)
       return (MMIO_ERROR);

    // Call open with handle...
    long rc = mmiomOpenHmmio(pmmioinfo, hmmioSS);

    // Close file if error...
    if(rc != MMIO_SUCCESS)
     mmioClose (hmmioSS, 0);

   return rc;
 }

unsigned long IOProc::mmiomOpenHmmio(MMIOINFO * pmmioinfo, HMMIO hmmio)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomGetFormatInfo(MMIOINFO *pmmioinfo, MMFORMATINFO *pMMFormatInfo)
 {
  pMMFormatInfo->ulStructLen  = sizeof (MMFORMATINFO);
  pMMFormatInfo->fccIOProc    = FOURCC(ulFourcc);
  pMMFormatInfo->ulIOProcType = ulIOProcType;
  pMMFormatInfo->ulMediaType  = ulMediaType;
  pMMFormatInfo->ulFlags      = ulCapsFlags;

  strcpy ((char *)pMMFormatInfo->szDefaultFormatExt, (char *)pDefaultExt);

  if (GetNLSData( &pMMFormatInfo->ulCodePage, &pMMFormatInfo->ulLanguage ))
     return( -1L );

  if (GetFormatStringLength( FOURCC(ulFourcc), &(pMMFormatInfo->lNameLength) ))
     return( -1L );

  return (MMIO_SUCCESS);
 }

unsigned long IOProc::mmiomGetFormatName(MMIOINFO * pmmioinfo,
                                 unsigned char *pFormatName, long lBytes)
 {
  return GetFormatString( FOURCC(ulFourcc), (unsigned char *)pFormatName, lBytes );
 }

unsigned long IOProc::mmiomIdentifyFile(MMIOINFO * pmmioinfo,
                         unsigned char *pFileName, HMMIO hmmio)
 {
   HMMIO hmmioTemp = hmmio;
   long curPos;
   unsigned long rc = MMIO_ERROR;

   // We need either a file name  or file handle
   if (!pFileName && !hmmio)
    {
     if(pmmioinfo)
       pmmioinfo->ulErrorRet = MMIOERR_INVALID_PARAMETER;

     return (MMIO_ERROR);
    }

   // If no handle, then open the file using the string name
   if (!hmmio)
     {
      if (!(hmmioTemp = mmioOpen ((PSZ) pFileName, 0, MMIO_READ
                                 | MMIO_DENYWRITE | MMIO_NOIDENTIFY)))
        {
         if(pmmioinfo)
            pmmioinfo->ulErrorRet = MMIOERR_INVALID_PARAMETER;

         return (MMIO_ERROR);
        }
     }
   else
     curPos = mmioSeek(hmmioTemp,0,SEEK_CUR);

   // Call identify hmmio...
   rc = mmiomIdentifyHmmio(pmmioinfo, hmmioTemp);

   // Close file before returning.
   if (!hmmio)
     mmioClose (hmmioTemp, 0);
   else
     mmioSeek(hmmioTemp, curPos, SEEK_SET);  // Seek back to curPos

   return rc;
 }

unsigned long IOProc::mmiomIdentifyHmmio(MMIOINFO * pmmioinfo, HMMIO hmmio)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomQueryHeaderLength(MMIOINFO * pmmioinfo)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomRead(MMIOINFO * pmmioinfo, unsigned char *pucBuffer,
                                long lBytes)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomSeek(MMIOINFO * pmmioinfo, long lByteOffset, long soff)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomSetHeader(MMIOINFO * pmmioinfo, void *pHeader, long length)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomWrite(MMIOINFO * pmmioinfo, unsigned char *pucSource,
                                 long lBytes)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomClose(MMIOINFO * pmmioinfo, long lFlags)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomUserMessage(MMIOINFO * pmmioinfo,unsigned short usMsg,
                         long lParam1, long lParam2)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomBeginStream(MMIOINFO * pmmioinfo, long lReadWrite, long pQosInfo)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomEndStream(MMIOINFO * pmmioinfo)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomBeginInsert(MMIOINFO * pmmioinfo)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomEndInsert(MMIOINFO * pmmioinfo)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomDelete(MMIOINFO * pmmioinfo, long lStartPos, long lLength)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomSeekByTime(MMIOINFO * pmmioinfo, long lByteOffset, long soff)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomUndo(MMIOINFO * pmmioinfo)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomRedo(MMIOINFO * pmmioinfo)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomTempChange(MMIOINFO * pmmioinfo, char *pPath)
 {
  return stub(pmmioinfo);
 }

unsigned long IOProc::mmiomSave(MMIOINFO * pmmioinfo, char *pOptionalFilename)
 {
  return stub(pmmioinfo);
 }

long IOProc::handleMMIOMessage (void *pmmioStr, unsigned short usMsg, long lParam1,
                                 long lParam2)
 {
    switch (usMsg)
     {
      //
      // Call Meta Class methods:
      //
      case MMIOM_GETFORMATINFO:
       return mmiomGetFormatInfo((PMMIOINFO)pmmioStr, (MMFORMATINFO *)lParam1);

      case MMIOM_GETFORMATNAME:
       return mmiomGetFormatName((PMMIOINFO)pmmioStr, (unsigned char *)lParam1, lParam2);

      case MMIOM_IDENTIFYFILE:
       return mmiomIdentifyFile((PMMIOINFO)pmmioStr, (unsigned char *)lParam1,
                                 (HMMIO)lParam2);
      case MMIOM_OPEN:
       {
        //
        // Call the Make function.
        //
        IOProc *pObj = Make();  // Instantiate an Object class.

        if(!pObj)
         return (MMIO_ERROR);
        else
         {
          ((PMMIOINFO)pmmioStr)->pExtraInfoStruct = (PVOID)pObj;
          pObj->setModuleHandle(hModuleHandle);
         }

        long rc = pObj->mmiomOpenFile((PMMIOINFO)pmmioStr,
                        (unsigned char*)lParam1);

        if(rc != MMIO_SUCCESS)
         {
          Destroy(pObj);  // Ask Meta Class to destroy the Object class.
          ((PMMIOINFO)pmmioStr)->pExtraInfoStruct = 0;
         }

        return rc;
       }

      default:
       {
        IOProc *pObj = 0;

        if(pmmioStr)
         pObj = (IOProc *)(((PMMIOINFO)pmmioStr)->pExtraInfoStruct);
        else
         {
          // An internal Error has occured if we get to this point:
          return (MMIOERR_INTERNAL_SYSTEM);
         }

        if(pObj)
         {
          switch(usMsg)
           {
            //
            // Call Object Class methods:
            //
            case MMIOM_CLOSE:
             {
              //
              // Call the Destroy function.
              //
              long rc = pObj->mmiomClose((PMMIOINFO)pmmioStr, lParam1);
              Destroy(pObj);  // Destroy the Object class.
              ((PMMIOINFO)pmmioStr)->pExtraInfoStruct = 0;
              return rc;
             }

            case MMIOM_GETHEADER:
             return pObj->mmiomGetHeader((PMMIOINFO)pmmioStr,
                 (void*)lParam1, lParam2);

            case MMIOM_QUERYHEADERLENGTH:
             return pObj->mmiomQueryHeaderLength((PMMIOINFO)pmmioStr);

            case MMIOM_READ:
             return pObj->mmiomRead((PMMIOINFO)pmmioStr,
                 (unsigned char*)lParam1, lParam2);

            case MMIOM_SEEK:
             return pObj->mmiomSeek((PMMIOINFO)pmmioStr, lParam1, lParam2);

            case MMIOM_SETHEADER:
             return pObj->mmiomSetHeader((PMMIOINFO)pmmioStr,
                 (void*)lParam1, lParam2);

            case MMIOM_WRITE:
             return pObj->mmiomWrite((PMMIOINFO)pmmioStr,
                 (unsigned char*)lParam1, lParam2);

            case MMIOM_BEGINSTREAM:
             return pObj->mmiomBeginStream((PMMIOINFO)pmmioStr,
                    lParam1, lParam2);

            case MMIOM_ENDSTREAM:
             return pObj->mmiomEndStream((PMMIOINFO)pmmioStr);

            case MMIOM_BEGININSERT:
             return pObj->mmiomBeginInsert((PMMIOINFO)pmmioStr);

            case MMIOM_ENDINSERT:
             return pObj->mmiomEndInsert((PMMIOINFO)pmmioStr);

            case MMIOM_DELETE:
             return pObj->mmiomDelete((PMMIOINFO)pmmioStr, lParam1,
                        lParam2);

            case MMIOM_SEEKBYTIME:
             return pObj->mmiomSeekByTime((PMMIOINFO)pmmioStr, lParam1,
                        lParam2);

            case MMIOM_UNDO:
             return pObj->mmiomUndo((PMMIOINFO)pmmioStr);

            case MMIOM_REDO:
             return pObj->mmiomRedo((PMMIOINFO)pmmioStr);

            case MMIOM_TEMPCHANGE:
             return pObj->mmiomTempChange((PMMIOINFO)pmmioStr,
                         (char *)lParam1);

            case MMIOM_SAVE:
             return pObj->mmiomSave((PMMIOINFO)pmmioStr, (char *)lParam1);

            default:
             return pObj->mmiomUserMessage((PMMIOINFO)pmmioStr, usMsg,
                        lParam1, lParam2); 
           }
         }
        else
         {
          // An internal Error has occured if we get to this point:
          if(pmmioStr)
            ((PMMIOINFO)pmmioStr)->ulErrorRet = 
                                MMIOERR_INTERNAL_SYSTEM;
          return (MMIOERR_INTERNAL_SYSTEM);
         }
       }  // End of default
     } // End of top switch
 }

long IOProc::GetFormatString (long lSearchId, unsigned char *pszFormatString, long lBytes)
 {
   void   *pResourceData;
   char   *pTemp;
   long   lStringLen;
   long   lRetVal = 0;
   long   lSearchTemp;

   if (DosGetResource(getModuleHandle(), RT_RCDATA, getNameTableID(), &pResourceData))
    return (MMIO_ERROR);

   //
   // The resource table is of the form : FOURCC String\0
   // Loop until a match is found, then return the string.
   // Copy up to the number of bytes specified.
   //

   lStringLen = 0;
   pTemp = (CHAR *)pResourceData;

   while (pTemp)
      {
      memmove(&lSearchTemp, pTemp, sizeof(LONG));

      if (lSearchTemp == 0L)
       break;  // End of table, search item not found

      if (lSearchTemp == lSearchId)   // Search item found?
         {
         pTemp += sizeof(long);       // Bypass numeric search id
         lStringLen = strlen(pTemp);  // Determine length of found string
         if (lStringLen >= lBytes)    // Truncate, if necessary
            {
            if (lBytes > 0)
               lStringLen = lBytes;   // Number of characters to return
            else
               {
               // Callers buffer has zero size.  Cannot return any data
               lRetVal = 0;           // Size of returned data
               break;                 // Done searching
               }
            }
         if (pszFormatString != 0)
            {
            memcpy(pszFormatString, pTemp, lStringLen); // Copy string to caller
            }
         lRetVal = lStringLen;        // Function return value
         break;                       // We're done searching
         }

      pTemp += sizeof(FOURCC);
      pTemp += (strlen(pTemp) + 1);   // Advance to next search key
      }

   DosFreeResource( pResourceData );

   return (lRetVal);  // Zero or strlen
 }

long IOProc::GetFormatStringLength (long lSearchId, long *plNameLength)
 {
   long  lStringSize;
   long  lRetVal;

   lStringSize = GetFormatString (lSearchId, 0, LONG_MAX);
   if (lStringSize > 0)             // String found?
      {
      *plNameLength = lStringSize;  // yes, return strlen
      lRetVal = 0;                  // and indicate success to caller
      }
   else
      {
      *plNameLength = 0;            // no, error.  Return zero for length
      lRetVal = lStringSize;        // and error code from called routine
      }
   return (lRetVal);
 }

unsigned long IOProc::GetNLSData( unsigned long *pulCodePage, unsigned long *pulLanguage )
 {
   void *pResourceData;
   char *pTemp;

   if (DosGetResource( getModuleHandle(), RT_RCDATA, getNLSDataID(), &pResourceData ))
      return (MMIO_ERROR);

   //
   // The resource table is of the form :
   //   usCodePage     Low
   //   usCountryCode  High
   //   usLanguage     Low
   //   usDialect      High
   //

   pTemp = (char *)pResourceData;

   while (pTemp)
      {
      memmove( pulCodePage, pTemp, sizeof(unsigned long) );
      pTemp += sizeof(unsigned long);

      if (pTemp == 0)
         {
         break;
         }

      memmove( pulLanguage, pTemp, sizeof(unsigned long) );

      break;
      }

   DosFreeResource( pResourceData );

   return (MMIO_SUCCESS);
 }
