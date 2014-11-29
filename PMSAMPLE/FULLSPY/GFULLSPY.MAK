getdir.exe  : getdir.c getdir.obj fullspym.exe
            link @getdir.l

getdir.obj: getdir.c fullspym.exe
  cl  /c  /B1 d:\c\bin\c1l.exe /Alfw /W3 /G2s /Ox /Zpe getdir.c

fullspym.exe: fullspym.c fullspym.obj  fullspym.l GFULLSPY.lib
            link @fullspym.l

GFULLSPY.lib: GFULLSPY.obj GFULLSPY.def GFULLSPY.l GFULLSPY.dll

GFULLSPY.obj: GFULLSPY.c GFULLSPY.h
  cl -DDLL /c  /B1 d:\c\bin\c1l.exe /Alfw /W3 /G2s /Ox /Zpe GFULLSPY.c


fullspym.obj: fullspym.c GFULLSPY.h
  cl -DDLL /c /B1 d:\c\bin\c1l.exe /Alfu /W3 /G2s /Ox /Zpe fullspym.c

GFULLSPY.dll: GFULLSPY.obj  GFULLSPY.l GFULLSPY.def
  link @GFULLSPY.l

GFULLSPY.lib: GFULLSPY.dll
  implib GFULLSPY.lib GFULLSPY.def

