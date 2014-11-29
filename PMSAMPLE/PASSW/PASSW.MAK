# Compiler Switches:
#

passw.obj: passw.c passw.h
  cl /c /Zi /Alfu /W3 /Gs /Gc /B1 D:\C\BIN\C1L.EXE /Oinltw /ID:\C\INCLUDE /ID:\TOOLKT13\c\include passw.c

passw.res: passw.rc passw.ico passw.h
        rc -r passw.rc

passw.exe: passw.l passw.obj passw.def passw.res
        link @passw.l
        rc passw.res
