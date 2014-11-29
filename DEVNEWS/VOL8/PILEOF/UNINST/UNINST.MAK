
clflags = /C /Ss /Kb /Ti+

.c.obj:
  icc $(clflags) $*.c

uninst.exe:     uninst.obj  uninst.def
    link386 uninst   ,,,,uninst

uninst.obj:       uninst.c


