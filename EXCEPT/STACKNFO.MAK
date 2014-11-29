.SUFFIXES:

.SUFFIXES: .cpp .obj

ALL : stacknfo.exe

! IFDEF DEBUG
DBGOPTS = /ti+ /Wcnd+cnv+cpy+rea+trd+tru+und+uni+
! ELSE
DBGOPTS = /o+
! ENDIF

ICCOPTS = /q /Wgen+pro+ret+ord+uni+use+ /tx+ /gm+ /tdp /ft- $(DBGOPTS)
LNKOPTS = /m:full

stacknfo.exe : stacknfo.obj dbgbuf.obj
  icc $(ICCOPTS) /b"$(LNKOPTS)" stacknfo.obj dbgbuf.obj

.cpp.obj:
  icc /c $(ICCOPTS) $*.cpp

stacknfo.obj : stacknfo.cpp thread.hpp dbgbuf.hpp

dbgbuf.obj   : dbgbuf.hpp

ZIP :
  -pkzip stacknfo stacknfo.exe
  -pkzip stacknfs stacknfo.cpp dbgbuf.cpp dbgbuf.hpp thread.hpp stacknfo.mak

