#==========================================================#
# OS/2 XPG4 Primer:  XPRMOS2.EXE make file                 #
#          Initial:  09/26/95    Y. Kimura                 #
#          History:  11/14/95    Add product info. dialog  #
#==========================================================#
.SUFFIXES: .rc
.SUFFIXES: .def
.SUFFIXES: .lib

all: xprmos2.exe

reses: xprmres.res

exeobj= xprmmain.obj xprmordr.obj xprmcust.obj xprmvord.obj xprmdata.obj
header= xprmos2.h xprmdata.h xprmres.h xprminfo.h

CC= ICC /Ti /Ss /Q /G3 /C /Sn+

.c.obj:
    $(CC)  $*.c >> xprmos2.err

xprmos2.exe: $(exeobj) $(header) xprmos2.def xprmos2.mak xprminfo.res
  ILINK /debug /pm:pm /st:0x30000 /out:xprmos2.exe $(exeobj) xprmos2.def >>xprmos2.err
  rc xprminfo.res xprmos2.exe >> xprmos2.err

xprminfo.res: xprminfo.rc xprminfo.h
   rc -r xprminfo.rc >> xprmos2.err

