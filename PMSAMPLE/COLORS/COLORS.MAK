colors.exe: colors.l colors.obj colors.def colors.res colors.c
        link @colors.l
        rc colors.res

colors.obj: colors.c colors.h
        cl /B1 D:\C\BIN\C1L.EXE /c /Alfu /W3 /Gs /Gc colors.c

colors.res: colors.rc colors.dlg colors.ico colors.h
        rc -r colors.rc

