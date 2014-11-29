# Compiler Switches:
#

loadicon.obj: loadicon.c loadicon.h
        cl /c /Alfu /W3 /B1 D:\C\BIN\C1L.EXE /Gs /Gc loadicon.c

loadicon.res: loadicon.rc loadicon.ico loadicon.h
        rc -r loadicon.rc

loadicon.exe: loadicon.l loadicon.obj loadicon.def loadicon.res
        link @loadicon.l
        rc loadicon.res
