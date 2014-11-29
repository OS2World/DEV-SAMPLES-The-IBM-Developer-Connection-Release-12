
all: int3386.lib int3286.lib prog1.exe prog2.exe prog3.exe dbprintf.obj

#------------------------------------
int3386.obj: int3386.asm
  masm /ml int3386;

int3386.lib: int3386.obj
  lib int3386 -+int3386;

#------------------------------------
int3286.obj: int3286.asm
  masm /ml int3286;

int3286.lib: int3286.obj
  lib int3286 -+int3286;

#------------------------------------
prog1.obj: prog1.c
  icc /c /ss $*.c

prog1.exe: prog1.obj
  link386 /map /nod /pmtype:vio prog1,,,os2386 dde4mbs int3386;
  mapsym prog1.map

#------------------------------------

prog2.obj: prog2.c
  icc /c /ss $*.c

prog2.exe: prog2.obj
  link386 /nod /map /pmtype:vio prog2,,,os2386 dde4mbs int3386;
  mapsym prog2.map

#------------------------------------
prog3.obj: prog3.c
  icc /c /ss $*.c

prog3.exe: prog3.obj
  link386 /nod /map /pmtype:vio prog3,,,os2386 dde4mbs int3386;
  mapsym prog3.map

#------------------------------------

dbprintf.obj: dbprintf.c dbprintf.h
  icc /c /ss $*.c

