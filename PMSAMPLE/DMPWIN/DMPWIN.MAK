# IBM Developer's Workframe/2 Make File Creation run at 13:34:19 on 03/25/93

# Make File Creation run in directory:
#   H:\DMPWIN;

.SUFFIXES:

.SUFFIXES: .c

DMPWIN.DLL:  \
  DMPWIN.OBJ \
  DMPWIN.DEF \
  DMPWIN.MAK
   @REM @<<DMPWIN.@0
     DMPWIN.OBJ/MAP
     DMPWIN.DLL
     DMPWIN.MAP
     REXX.LIB
     DMPWIN.DEF;
<<
   LINK386.EXE @DMPWIN.@0
   IMPLIB DMPWIN.LIB DMPWIN.DEF
   MAPSYM DMPWIN.MAP

{.}.c.obj:
   ICC.EXE  /Sp1 /Ss /Ge- /C /Sm .\$*.c

!include DMPWIN.DEP
