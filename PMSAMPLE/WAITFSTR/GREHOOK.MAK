# IBM Developer's Workframe/2 Make File Creation run at 13:34:19 on 03/25/93

# Make File Creation run in directory:
#   H:\GREHOOK;

.SUFFIXES:

.SUFFIXES: .c

GREHOOK.DLL:  \
  GREHOOK.OBJ \
  GREHOOK.DEF \
  GREHOOK.MAK
   @REM @<<GREHOOK.@0
     GREHOOK.OBJ/MAP
     GREHOOK.DLL
     GREHOOK.MAP

     GREHOOK.DEF;
<<
   LINK386.EXE @GREHOOK.@0
   IMPLIB GREHOOK.LIB GREHOOK.DEF
   MAPSYM GREHOOK.MAP

{.}.c.obj:
   ICC.EXE /Sp1 /Sm /Ss /Ge- /C /Fa /Lx .\$*.c

!include GREHOOK.DEP
