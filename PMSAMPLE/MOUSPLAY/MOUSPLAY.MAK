# Compiler Switches:
#
# /c    - Supresses Linking
# /Alfu - Customised large memory model
#         (l) far pointers for code
#         (f) far pointers for data
#         (u) different segments for stack and data
# /W3   - Sets warning message level
# /Gs   - Removes stack probes
# /Gc   - Controls function calling sequences

mousplay.exe: mousplay.l mousplay.obj mousplay.def mousplay.res mousedll.lib
        link386 @mousplay.l
        rc mousplay.res

mousplay.obj: mousplay.c mousplay.h
        icc /c /W3 /Gm  mousplay.c

mousplay.res: mousplay.rc mousplay.dlg mousplay.h
        rc -r mousplay.rc

mousedll.lib: mousedll.dll
  implib mousedll.lib mousedll.def

mousedll.obj: mousedll.c
  icc /Ge- /c mousedll.c

mousedll.dll: mousedll.obj mousedll.l mousedll.def
  link386 @mousedll.l

