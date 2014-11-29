# IBM Developer's Workframe/2 Make File Creation run at 19:30:24 on 10/09/92

# Make File Creation run in directory:
#   H:\EXCEPT;

.SUFFIXES:

.SUFFIXES: .c

EXCEPTQ.DLL:  \
  EXCEPTQ.OBJ \
  EXCEPTQ.DEF \
  EXCEPTQ.MAK
   @REM @<<EXCEPTQ.@0
     /M /NOL /PM:VIO +
     EXCEPTQ.OBJ INT3.OBJ
     EXCEPTQ.DLL

     DIS386.LIB
     EXCEPTQ.DEF;
<<
   ILINK /NOFREE @EXCEPTQ.@0
  IMPLIB EXCEPTQ.LIB EXCEPTQ.DEF

{.}.c.obj:
   ICC.EXE /Sm /Ss /O /Gm /Ge- /C .\$*.c

!include EXCEPTQ.DEP
