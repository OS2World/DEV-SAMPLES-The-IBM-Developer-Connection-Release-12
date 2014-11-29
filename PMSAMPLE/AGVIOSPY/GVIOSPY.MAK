
gviospym.exe: gviospym.c gviospym.obj  gviospym.l gviospy.lib
            link @gviospym.l

gviospy.lib: gviospy.obj gviospy.def gviospy.l gviospy.dll

gviospy.obj: gviospy.c gviospy.h
  cl -DDLL /Fa /c  /B1 d:\c\bin\c1l.exe /Alfw /W3 /G2s /Ox /Zpe gviospy.c


gviospym.obj: gviospym.c gviospy.h
  cl -DDLL /c /B1 d:\c\bin\c1l.exe /Alfu /W3 /G2s /Ox /Zpe gviospym.c

gviospy.dll: gviospy.obj  gviospy.l gviospy.def
  link @gviospy.l

gviospy.lib: gviospy.dll
  implib gviospy.lib gviospy.def

