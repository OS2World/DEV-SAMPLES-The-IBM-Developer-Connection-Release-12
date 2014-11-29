# IBM Developer's Workframe/2 Make File Creation run at 12:04:25 on 12/06/93

# Make File Creation run in directory:
#   D:\DRF6\SRC\PMDDHK;

H         = ..\..\h
.SUFFIXES: .c .cpp .cxx

PMDDHK.DLL:  \
  PMDDHK.OBJ \
  PMDDHK32.OBJ \
  MAKEFILE
   ICC.EXE @<<
  /B" /m:full"
  /Fe"PMDDHK.DLL" pmddhk.def
PMDDHK.OBJ 
PMDDHK32.OBJ
<<

{.}.c.obj:
   ICC.EXE /Ss /O /Op- /Gm /Ge- /Gs /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /O /Op- /Gm /Ge- /Gs /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /O /Op- /Gm /Ge- /Gs /C   .\$*.cxx

!include MAKEFILE.DEP
