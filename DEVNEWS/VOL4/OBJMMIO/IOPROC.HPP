//
// IOProc.hpp
//
#ifndef _IOPROC_
#define _IOPROC_

extern "C"
{
 #define INCL_DOSRESOURCES
 #include <os2.h>
 #include <string.h>
 #include <stdio.h>

 #define INCL_OS2MM
 #include <os2me.h>
}

class IOProc {
  private:
    unsigned long hModuleHandle;
    unsigned long stub(MMIOINFO * pmmioinfo);
    unsigned long nls_data, ioproc_name_table;
    unsigned long ulIOProcType, ulMediaType, ulCapsFlags, ulFourcc;
    char pDefaultExt[10];

  protected:
    IOProc(unsigned long nlsdata, unsigned long name_table,
           unsigned long fourcc, unsigned long IOProcType, unsigned long MediaType,
           unsigned long CapsFlags, char *DefaultFormatExt);

    virtual long GetFormatString (long lSearchId, unsigned char *pszFormatString,
                                  long lBytes);
    virtual long GetFormatStringLength (long lSearchId, long *plNameLength);
    virtual unsigned long GetNLSData (unsigned long *, unsigned long *);

    // Derived Classes Must Override these methods:
    virtual IOProc *Make() = 0;
    virtual Destroy(IOProc *pDOA) = 0;

  public:
    virtual ~IOProc();
    unsigned long getModuleHandle() const;
    unsigned long getNLSDataID() const;
    unsigned long getNameTableID() const;
    IOProc& setModuleHandle(unsigned long hMod);

    virtual long handleMMIOMessage(void *pmmioStr, unsigned short usMsg, long lParam1,
                                   long lParam2);

    virtual unsigned long mmiomUserMessage(MMIOINFO * pmmioinfo,unsigned short usMsg,
                                           long lParam1, long lParam2);

    //
    // Messages from MMIO manager:
    // MMIOM_... ==> mmiom...
    //
    virtual unsigned long mmiomGetHeader(MMIOINFO * pmmioinfo,
                                void *pHeader, long length);

    virtual unsigned long mmiomGetFormatInfo(MMIOINFO * pmmioinfo,
                                 MMFORMATINFO *pMMFormatInfo);

    virtual unsigned long mmiomGetFormatName(MMIOINFO * pmmioinfo,
                 unsigned char *pFormatName, long lBytes);

    virtual unsigned long mmiomIdentifyFile(MMIOINFO * pmmioinfo,
                                 unsigned char *pFileName, HMMIO hmmio);
    virtual unsigned long mmiomIdentifyHmmio(MMIOINFO * pmmioinfo, HMMIO hmmio);

    virtual unsigned long mmiomOpenFile(MMIOINFO * pmmioinfo, unsigned char *pFileName);
    virtual unsigned long mmiomOpenHmmio(MMIOINFO * pmmioinfo, HMMIO hmmio);

    virtual unsigned long mmiomQueryHeaderLength(MMIOINFO * pmmioinfo);

    virtual unsigned long mmiomRead(MMIOINFO * pmmioinfo, unsigned char *pucBuffer,
                                    long lBytes);

    virtual unsigned long mmiomSeek(MMIOINFO * pmmioinfo, long lByteOffset, long soff);

    virtual unsigned long mmiomSetHeader(MMIOINFO * pmmioinfo, void *pHeader, long length);

    virtual unsigned long mmiomWrite(MMIOINFO * pmmioinfo, unsigned char *pucSource,
                                     long lBytes);

    virtual unsigned long mmiomClose(MMIOINFO * pmmioinfo, long lFlags);

    virtual unsigned long mmiomBeginStream(MMIOINFO * pmmioinfo, long lReadWrite, long pQosInfo);
    virtual unsigned long mmiomEndStream(MMIOINFO * pmmioinfo);

    virtual unsigned long mmiomBeginInsert(MMIOINFO * pmmioinfo);
    virtual unsigned long mmiomEndInsert(MMIOINFO * pmmioinfo);

    virtual unsigned long mmiomDelete(MMIOINFO * pmmioinfo, long lStartPos, long lLength);

    virtual unsigned long mmiomSeekByTime(MMIOINFO * pmmioinfo, long lByteOffset, long soff);

    virtual unsigned long mmiomUndo(MMIOINFO * pmmioinfo);
    virtual unsigned long mmiomRedo(MMIOINFO * pmmioinfo);

    virtual unsigned long mmiomTempChange(MMIOINFO * pmmioinfo, char *pPath);
    virtual unsigned long mmiomSave(MMIOINFO * pmmioinfo, char *pOptionalFilename);
};

#endif
