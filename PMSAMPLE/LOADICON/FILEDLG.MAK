#------------------
# FILEDLG make file
#------------------
#wantCV = 1
#------------------------------------------------------------------------------
# These are the external interface LIBraries we require
#------------------------------------------------------------------------------
EXTERN_LIB_C    = llibcdll.lib
EXTERN_LIB_OS2  = os2.lib
#------------------------------------------------------------------------------
# C parameters
#------------------------------------------------------------------------------
cWARN           = /W3
cMEM            = /Alfu
cOPTS           = /c /B1 d:\c\bin\c1l.exe /G2s
# decide about CODEVIEW...
!ifdef wantCV
Compile         = $(cOPTS) $(cMEM) $(cWARN) /Zpei /Od
!else
Compile         = $(cOPTS) $(cMEM) $(cWARN) /Zpe /Ox
!endif
#------------------------------------------------------------------------------
# LINK parameters
#------------------------------------------------------------------------------
lMain           = /NOD /A:16
!ifdef wantCV
lMain           = $(lMain) /CO /LI
!endif

#------------------------------------------------------------------------------
# Inferencing rules
#------------------------------------------------------------------------------
all: filedlg.dll filedlg.lib

filedlg.dll: _filedlg.res _filedlg.obj _filedlg.def filedlg.mak
  link $(lMain) @<<
_filedlg.obj
filedlg.dll
nul.map
$(EXTERN_LIB_OS2) + $(EXTERN_LIB_C)
_filedlg.def
<<
  rc _filedlg.res filedlg.dll

_filedlg.res: _filedlg.rc _filedlg.h filedlg.mak
  rc -r _filedlg.rc

_filedlg.obj: _filedlg.c _filedlg.h filedlg.mak
  cl $(Compile) _filedlg.c

filedlg.lib: filedlg.obj filedlg.h filedlg.mak

filedlg.obj: filedlg.c _filedlg.h filedlg.mak
  cl $(Compile) filedlg.c
  lib filedlg.lib -+filedlg.obj;

