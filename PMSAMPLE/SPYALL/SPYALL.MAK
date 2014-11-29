getdir.exe  : getdir.c getdir.obj getdir.def
            link @getdir.l

getdir.obj: getdir.c SPYALL.DLL
  cl  /c  /B1 d:\c\bin\c1l.exe /Alfw /W3 /G2s /Ox /Zpe getdir.c

SPYALL.dll: SPYALL.obj  SPYALL.l SPYALL.def
  link @SPYALL.l

SPYALL.lib: SPYALL.dll
  implib SPYALL.lib SPYALL.def

SPYALL.obj: SPYALL.c
  cl -DDLL /c  /B1 d:\c\bin\c1l.exe /Alfw /W3 /G2s /Ox /Zpe SPYALL.c

