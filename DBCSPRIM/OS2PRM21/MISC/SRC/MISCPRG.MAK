#===============================#
#  Make file for MISCPRG.EXE    #
#===============================#

.SUFFIXES: .rc
.SUFFIXES: .def
.SUFFIXES: .lib

all: miscprg.exe

reses: miscprg.res

exeobj= miscmain.obj miscwin.obj  miscedit.obj

CC= ICC /Ti /Ss /Q /G3 /C /Gm+

.c.obj:
    $(CC)  $*.c > $*.err


miscprg.res: miscprg.rc miscprg.h
    rc -r -cp 942 miscprg.rc >> miscprg.err

miscprg.exe: miscprg.res  $(exeobj)
    LINK386 /CO /PM:PM $(exeobj), miscprg.exe , miscprg, ,miscprg.def; >> miscprg.err
    rc miscprg.res miscprg.exe >> miscprg.err


