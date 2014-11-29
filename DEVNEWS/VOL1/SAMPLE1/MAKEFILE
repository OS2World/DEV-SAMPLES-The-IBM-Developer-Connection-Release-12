#-------------------------------------------------------------------
# makefile for APP.EXE, the 2-threaded PM sample

CC   = icc /c /gd- /re /ss /ms /gm+ /ti+ /q+ /Sm /kb+
LINK = link386 /nod /cod /map
LIBS = dde4mbs + os2386
H    = app.h
OBJ  = app.obj object.obj

all: app.exe

app.exe: $(OBJ) app.res app.def
  $(LINK)  $(OBJ) ,,, $(LIBS) , $*
  rc app.res

app.res: $*.rc threads.ico app.h
  rc -r $*.rc

app.obj: $*.c $(H)
  $(CC) $*.c

object.obj: $*.c $(H)
  $(CC) $*.c


# end of file makefile ---------------------------------------------
