# IBM Developer's Workframe/2 Make File Creation run at 13:34:19 on 03/25/93

# Make File Creation run in directory:
#   H:\WAITFSTR;

.SUFFIXES:

.SUFFIXES: .c

WAITFSTR.DLL:  \
  WAITFSTR.OBJ \
  WAITFSTR.DEF \
  WAITFSTR.MAK
   @REM @<<WAITFSTR.@0
     WAITFSTR.OBJ/MAP
     WAITFSTR.DLL
     WAITFSTR.MAP
     GREHOOK.LIB REXX.LIB
     WAITFSTR.DEF;
<<
   LINK386.EXE @WAITFSTR.@0
   IMPLIB WAITFSTR.LIB WAITFSTR.DEF
   MAPSYM WAITFSTR.MAP

{.}.c.obj:
   ICC.EXE /Sp1 /Ss /Ge- /C /Sm .\$*.c

!include WAITFSTR.DEP
