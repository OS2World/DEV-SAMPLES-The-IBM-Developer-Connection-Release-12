TITLEBAR.dll: TITLEBAR.obj  TITLEBAR.l TITLEBAR.def classinf.obj
  link @TITLEBAR.l

TITLEBAR.lib: TITLEBAR.dll
  implib TITLEBAR.lib TITLEBAR.def

TITLEBAR.obj: TITLEBAR.c
  cl -DDLL /c  /B1 d:\c\bin\c1l.exe /Alfu /W3 /G2s /Oinltw /Fc /Zpe TITLEBAR.c

CLASSINF.obj: CLASSINF.c
  cl -DDLL /c  /B1 d:\c\bin\c1l.exe /Alfu /W3 /G2s /Oinltw /Fc /Zpe CLASSINF.c

