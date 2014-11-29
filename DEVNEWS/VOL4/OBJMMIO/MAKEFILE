#****************************************************************************
#  C++ Project Makefile
#
#  Sample Makefile
#     Modify directory variables for your directory structure.
#     DEF file EXPORTS were generated using cppfilt (see below).
#
#****************************************************************************

#****************************************************************************
#  Dot directive definition area (usually just suffixes)
#****************************************************************************

.SUFFIXES: .c .cpp .cxx .rc .inl .hpp .h .if

#****************************************************************************
#  Directory variables:
#****************************************************************************
TARGETFILE=IOPROC
TARGETDIR=.
CPPLIB=E:\IBMCPP\LIB
OS2LIB=D:\TOOLKT21\OS2LIB
MMLIB=F:\mmos2\mmtoolkt\lib
CPPINC=E:\IBMCPP\INCLUDE
CLASSINC=E:\IBMCPP\IBMCLASS
OS2TKINC=D:\TOOLKT21\CPLUS\OS2H
MMTKINC=F:\mmos2\mmtoolkt\h

#****************************************************************************
#  Directory variables defined in location.mak:
#****************************************************************************
#TARGETFILE - target file name without extension
#TARGETDIR  - directory where executable is to be placed
#CPPLIB     - directory where IBMCPP libraries are located
#OS2LIB     - directory where OS/2 TOOLKT libraries are located
#MMLIB      - directory where MMPM/2 libraries are located
#CPPINC     - directory where IBMCPP include headers are located
#CLASSINC   - directory where IBMCPP CLASS files are located
#OS2TKINC   - directory where OS/2 CPP headers are located
#MMTKINC    - directory where MMPM/2 include headers are located
#TMPINC     - directory where template resolution files are placed
#****************************************************************************

#****************************************************************************
#   Target Information
#****************************************************************************
ALL: $(TARGETDIR)\$(TARGETFILE).DLL 

#****************************************************************************
# Object & Library files
#****************************************************************************
Libs=$(MMLIB)\MMPM2.LIB $(OS2LIB)\OS2386.LIB \
     $(CPPLIB)\DDE4MBS.LIB

Objs=ioproc.obj \
!ifdef MMDEBUG
        $(CPPLIB)\dde4xtra.obj
!endif

#****************************************************************************
#  Compiler/Linker flags
#****************************************************************************
!ifdef MMOPT
C_OPT=/O+ /G4 /Ti- /Gh-
L_OPT=/BASE:65536 /EXEPACK
!endif

!ifdef MMDEBUG
C_DEBUG=/O- /Gh+ /Ti+  /Fb-
L_DEBUG=/DE
!endif

CPPFLAGS=/Gn+ /Gd- /Ss /C+ /Ge- /Gm+ /Q $(C_DEBUG) $(C_OPT)
LFLAGS=/NOI /NOE /NOD /ALIGN:16 /nol $(L_DEBUG) $(L_OPT)

#****************************************************************************
#  Compiler Flags
#****************************************************************************
#       /Fb[+/-]        [create/don't create] a browser file
#       /Fl[+/-]        listing file
#       /Fm[+/-]        map file
#       /Fo[+/-]        object file
#       /Fe             exe name
#       /Fm             map name
#       /Kb[+/-]        basic diagnostic messages
#       /Ti[+/-]        debugger information
#       /Ss[+/-]        '\\' comments
#       /Gd[+/-]        [dynamic/static] linking of runtime library
#       /Ge[+/-]        [.exe/.dll] build
#       /Gh[+/-]        [enable/disable] profiling hooks
#       /Gm[+/-]        [multi/single] - threaded libraries
#       /Gr[+/-]        ring 0 enablement
#       /G[3/4/5]       processor type ONLY-optimization, still will run on any 386/486/Pent.
#       /O[+/-]         enable optimization
#       /Oi[+/-/value]  inlining
#       /B"options"     "options" passed on to linker
#       /C[+/-]         [compile only/ compile and link]
#       /J[+/-]         default char type; unspecified char = [unsigned/signed]
#       /V"string"      imbeds version/copyright string in objects and executable files
#       /Q[+/-]         Compiler logo
#       -B"..."         pass ... flags onto linker

#****************************************************************************
#  Linker Flags
#****************************************************************************
#       /NOI            case sensitive
#       /BASE:65536     (.EXE ONLY) starting address of program
#       /ALIGN:16       align segments on 16 byte boundary in .exe or .dll
#       /EXEPACK        pack the .exe or .dll
#       /PM:PM          PM application
#       /MAP            map file
#       /debug          debug info
#       /NOL            nologo

#****************************************************************************
#  Variables
#****************************************************************************
ICC=icc.exe
RC=rc
LANG=US
CINC = -I. -I$(CPPINC) -I$(CLASSINC) -I$(OS2TKINC) -I$(MMTKINC)

#****************************************************************************
# Inference Rules
#****************************************************************************

.rc.res:
   @echo $@ > con
   $(RC) -r .\$*.RC

.c.obj:
   @echo $@ > con
   $(ICC) $(CPPFLAGS) .\$*.c

.cpp.obj:
   @echo $@ > con
   $(ICC) $(CPPFLAGS) $(CINC) .\$*.cpp

.cxx.obj:
   @echo $@ > con
   $(ICC) $(CPPFLAGS) .\$*.cxx

#****************************************************************************
#   ICC (Linker) Response File
#****************************************************************************   
$(TARGETFILE).l$(LANG): makefile
        @echo Creating file <<$(@B).L$(LANG)
/Q /Tdp  /B"$(LFLAGS)"
$(Objs) /Fe"$(TARGETDIR)\$(TARGETFILE).DLL"
$(Libs)
$(TARGETFILE).def
<<keep

#****************************************************************************
#   Module Definition File
#****************************************************************************
$(TARGETFILE).def: makefile
        @echo Creating file <<$(@B).def
LIBRARY    $(TARGETFILE) INITINSTANCE TERMINSTANCE
DESCRIPTION '$(TARGETFILE)'
BASE = 0x1B640000
SEGMENTS
   SHR_SEG   CLASS 'DATA' SHARED
DATA MULTIPLE NONSHARED
HEAPSIZE 0
IMPORTS
  mmio.mmioRead
  mmio.mmioSeek
  mmio.mmioOpen
  mmio.mmioClose
  mmio.mmioGetFormats
  mmio.mmioGetFormatName
  mmio.mmioIdentifyFile
  mmio.mmioIdentifyStorageSystem
  mmio.mmioQueryIOProcModuleHandle
  mmio.mmioInstallIOProc
  mmio.mmioQueryHeaderLength
  mmio.mmioStringToFOURCC
  mmio.mmioSendMessage
  mmio.mmioGetLastError

EXPORTS
;[E:\IOPROC\ARTICLE]cppfilt -b -p -n -o1 IOPROC.obj
;From object file:  E:\IOPROC\ARTICLE\IOPROC.obj
  ;PUBDEFs (Symbols available from object file):
    ;IOProc::mmiomSetHeader(_MMIOINFO*,void*,long)
    mmiomSetHeader__6IOProcFP9_MMIOINFOPvl    @1 NONAME
    ;IOProc::mmiomGetFormatInfo(_MMIOINFO*,_MMFORMATINFO*)
    mmiomGetFormatInfo__6IOProcFP9_MMIOINFOP13_MMFORMATINFO    @2 NONAME
    ;IOProc::mmiomGetHeader(_MMIOINFO*,void*,long)
    mmiomGetHeader__6IOProcFP9_MMIOINFOPvl    @3 NONAME
    ;IOProc::mmiomClose(_MMIOINFO*,long)
    mmiomClose__6IOProcFP9_MMIOINFOl    @4 NONAME
    ;IOProc::mmiomTempChange(_MMIOINFO*,char*)
    mmiomTempChange__6IOProcFP9_MMIOINFOPc    @5 NONAME
    ;IOProc::~IOProc()
    __dt__6IOProcFv    @6 NONAME
    ;IOProc::handleMMIOMessage(void*,unsigned short,long,long)
    handleMMIOMessage__6IOProcFPvUslT3    @7 NONAME
    ;IOProc::mmiomDelete(_MMIOINFO*,long,long)
    mmiomDelete__6IOProcFP9_MMIOINFOlT2    @8 NONAME
    ;IOProc::getNLSDataID() const
    getNLSDataID__6IOProcCFv    @9 NONAME
    ;IOProc::mmiomWrite(_MMIOINFO*,unsigned char*,long)
    mmiomWrite__6IOProcFP9_MMIOINFOPUcl    @10 NONAME
    __vft6IOProc    @11 NONAME
    ;IOProc::mmiomGetFormatName(_MMIOINFO*,unsigned char*,long)
    mmiomGetFormatName__6IOProcFP9_MMIOINFOPUcl    @12 NONAME
    ;IOProc::mmiomEndStream(_MMIOINFO*)
    mmiomEndStream__6IOProcFP9_MMIOINFO    @13 NONAME
    ;IOProc::mmiomUndo(_MMIOINFO*)
    mmiomUndo__6IOProcFP9_MMIOINFO    @14 NONAME
    ;IOProc::mmiomIdentifyHmmio(_MMIOINFO*,unsigned long)
    mmiomIdentifyHmmio__6IOProcFP9_MMIOINFOUl    @15 NONAME
    ;IOProc::setModuleHandle(unsigned long)
    setModuleHandle__6IOProcFUl    @16 NONAME
    ;IOProc::mmiomRead(_MMIOINFO*,unsigned char*,long)
    mmiomRead__6IOProcFP9_MMIOINFOPUcl    @17 NONAME
    ;IOProc::mmiomSeekByTime(_MMIOINFO*,long,long)
    mmiomSeekByTime__6IOProcFP9_MMIOINFOlT2    @18 NONAME
    ;IOProc::mmiomUserMessage(_MMIOINFO*,unsigned short,long,long)
    mmiomUserMessage__6IOProcFP9_MMIOINFOUslT3    @19 NONAME
    ;IOProc::IOProc(unsigned long,unsigned long,unsigned long,unsigned long,unsigned long,unsigned long,char*)
    __ct__6IOProcFUlN51Pc    @20 NONAME
    ;IOProc::mmiomEndInsert(_MMIOINFO*)
    mmiomEndInsert__6IOProcFP9_MMIOINFO    @21 NONAME
    ;IOProc::mmiomSeek(_MMIOINFO*,long,long)
    mmiomSeek__6IOProcFP9_MMIOINFOlT2    @22 NONAME
    ;IOProc::GetFormatString(long,unsigned char*,long)
    GetFormatString__6IOProcFlPUcT1    @23 NONAME
    ;IOProc::stub(_MMIOINFO*)
    stub__6IOProcFP9_MMIOINFO    @24 NONAME
    ;IOProc::mmiomBeginInsert(_MMIOINFO*)
    mmiomBeginInsert__6IOProcFP9_MMIOINFO    @25 NONAME
    ;IOProc::mmiomSave(_MMIOINFO*,char*)
    mmiomSave__6IOProcFP9_MMIOINFOPc    @26 NONAME
    ;IOProc::mmiomRedo(_MMIOINFO*)
    mmiomRedo__6IOProcFP9_MMIOINFO    @27 NONAME
    ;IOProc::mmiomQueryHeaderLength(_MMIOINFO*)
    mmiomQueryHeaderLength__6IOProcFP9_MMIOINFO    @28 NONAME
    ;IOProc::getNameTableID() const
    getNameTableID__6IOProcCFv    @29 NONAME
    ;IOProc::mmiomOpenFile(_MMIOINFO*,unsigned char*)
    mmiomOpenFile__6IOProcFP9_MMIOINFOPUc    @30 NONAME
    ;IOProc::mmiomOpenHmmio(_MMIOINFO*,unsigned long)
    mmiomOpenHmmio__6IOProcFP9_MMIOINFOUl    @31 NONAME
    ;IOProc::mmiomIdentifyFile(_MMIOINFO*,unsigned char*,unsigned long)
    mmiomIdentifyFile__6IOProcFP9_MMIOINFOPUcUl    @32 NONAME
    ;IOProc::getModuleHandle() const
    getModuleHandle__6IOProcCFv    @33 NONAME
    ;IOProc::GetFormatStringLength(long,long*)
    GetFormatStringLength__6IOProcFlPl    @34 NONAME
    ;IOProc::GetNLSData(unsigned long*,unsigned long*)
    GetNLSData__6IOProcFPUlT1    @35 NONAME
    ;IOProc::mmiomBeginStream(_MMIOINFO*,long,long)
    mmiomBeginStream__6IOProcFP9_MMIOINFOlT2    @36 NONAME
<<keep

#****************************************************************************
#   Target Dependencies
#****************************************************************************
$(TARGETDIR)\$(TARGETFILE).DLL: $(TARGETFILE).l$(LANG) $(TARGETFILE).def $(Objs) $(Libs)
        @echo $@ > con
         $(ICC) @$(TARGETFILE).l$(LANG)
         implib $(TARGETDIR)\$(TARGETFILE).LIB $(TARGETFILE).DEF


#****************************************************************************
#  Dependency generation and Checking
#****************************************************************************
ioproc.obj : ioproc.hpp ioproc.cpp
