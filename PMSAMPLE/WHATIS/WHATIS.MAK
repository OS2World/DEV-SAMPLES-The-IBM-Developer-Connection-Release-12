all: whatis.exe
# Compiler Switches:
#
# /c    - Supresses link386ing
# /Alfu - Customised small memory model
# /W3   - Sets warning message level
# /Gs   - Removes stack probes
# /Gc   - Controls function calling sequences

whatis.exe: whatis.l whatis.obj whatis.def whatis.res whatisd.lib
        link386 @whatis.l
        rc whatis.res

whatis.obj: whatis.c whatis.h
        icc /Ss /Sm /c /W3 /G3  whatis.c

whatis.res: whatis.rc whatis.ico whatis.h
        rc -r whatis.rc

whatisd.lib: whatisd.dll
  implib whatisd.lib whatisd.def

whatisd.obj: whatisd.c
  icc  /Ge- /c /Ss /W3 /Sm /G3  whatisd.c

whatisd.dll: whatisd.obj whatisd.l whatisd.def
  link386 @whatisd.l

