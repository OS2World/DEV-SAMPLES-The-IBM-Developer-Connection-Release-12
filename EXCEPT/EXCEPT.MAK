# IBM Developer's Workframe/2 Make File Creation run at 19:30:24 on 10/09/92

# Make File Creation run in directory:
#   H:\EXCEPT;

.SUFFIXES:

.SUFFIXES: .c

EXCEPT.DLL:  \
  EXCEPT.OBJ \
  EXCEPT.DEF \
  EXCEPT.MAK
   @REM @<<EXCEPT.@0
     /M /NOL /PM:VIO +
     EXCEPT.OBJ
     EXCEPT.DLL

     DIS386.LIB
     EXCEPT.DEF;
<<
  ILINK /NOFREE @EXCEPT.@0
  IMPLIB EXCEPT.LIB EXCEPT.DEF

{.}.c.obj:
   ICC.EXE /I. /Sm /Ss  /O /Gm /Ge- /C .\$*.c

!include EXCEPT.DEP
