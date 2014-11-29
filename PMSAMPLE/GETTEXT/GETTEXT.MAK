
gettext.lib: gettext.h gettext.obj \
              gettext.def gettext.dll

gettext.obj: gettext.c
  cl -DDLL /c /Alfu /W2 /G2s /Ox /Zpe /B1 C1L.EXE gettext.c

gettext.dll: gettext.obj gettext.l gettext.def
  link @gettext.l

