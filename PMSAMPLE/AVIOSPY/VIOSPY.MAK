
viospym.exe: viospym.c viospym.obj  viospym.l viospy.lib
            link @viospym.l

viospy.lib: viospy.obj viospy.def viospy.l viospy.dll

viospy.obj: viospy.c viospy.h
  cl -DDLL /c  /B1 d:\c\bin\c1l.exe /Alfu /W3 /G2s /Ox /Zpe viospy.c


viospym.obj: viospym.c viospy.h
  cl -DDLL /c /B1 d:\c\bin\c1l.exe /Alfu /W3 /G2s /Ox /Zpe viospym.c

viospy.dll: viospy.obj  viospy.l viospy.def
  link @viospy.l

viospy.lib: viospy.dll
  implib viospy.lib viospy.def

