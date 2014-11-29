
clflags = /C /Ss /Kb /Ti+

.c.obj:
  icc $(clflags) $*.c

install.exe:     install.obj  install.def
    link386 /DE install   ,,,,install

install.obj:       install.c


