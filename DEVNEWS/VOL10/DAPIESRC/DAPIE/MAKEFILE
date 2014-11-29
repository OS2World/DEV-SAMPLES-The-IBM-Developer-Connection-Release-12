.SUFFIXES:
.SUFFIXES: .c .rc .res


# Environment
#
IBMCPP     = C:\IBMCPP
TOOLKIT    = C:\TOOLKT30
H          = $(TOOLKIT)\H;$(IBMCPP)\INCLUDE
LIB        = $(TOOLKIT)\LIB;$(IBMCPP)\LIB
TOOLSPATH  = $(TOOLKIT)\BIN;$(IBMCPP)\BIN

!if [set INCLUDE=$(H)] \
||  [set LIB=$(LIB)] \
||  [set PATH=$(TOOLSPATH)]
!endif


# Build rules
#
ALL: SAMPLE.EXE

SAMPLE.EXE: MAIN.OBJ SAMPLE.OBJ CONTAIN.OBJ SAMPLE.DEF SAMPLE.RES MAKEFILE
   @REM @<<SAMPLE.@0
     /PM:PM +
     MAIN.OBJ SAMPLE.OBJ CONTAIN.OBJ
     SAMPLE.EXE

     
     SAMPLE.DEF;
<<
   LINK386.EXE @SAMPLE.@0 SAMPLE.DEF
   RC SAMPLE.RES SAMPLE.EXE


# Implied rules
#
{.}.c.obj:
   ICC.EXE /W3 /Wpro+ /Q /Gm /Se /Ss+ /C /DWIN32 .\$*.c

{.}.rc.res:
   RC -r .\$*.RC
