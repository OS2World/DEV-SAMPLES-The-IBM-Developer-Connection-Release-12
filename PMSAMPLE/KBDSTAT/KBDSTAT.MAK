kbdstat.exe: kbdstat.l kbdstat.obj kbdstat.def kbdstat.res kbdstat.c
        link @kbdstat.l
        rc kbdstat.res

kbdstat.obj: kbdstat.c kbdstat.h
        cl /B1 D:\C\BIN\C1L.EXE /Zi /Od /c /Alfu /W3 /Gs /Gc kbdstat.c

kbdstat.res: kbdstat.rc kbdstat.dlg kbdstat.ico kbdstat.h
        rc -r kbdstat.rc

