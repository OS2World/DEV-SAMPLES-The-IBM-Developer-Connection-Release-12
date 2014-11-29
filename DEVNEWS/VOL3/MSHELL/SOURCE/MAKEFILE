

# basic settings
CCBASE   = icc /c /Gd- /Se /Re /ss /Ms /Gm+  /Q+ /Fo$*.obj  /Gs- /Kb+
LINKBASE = link386 /nod /map


# switches for debug and retail
DEBUGCFLAGS = /Ti+
RETAILCFLAGS = /DNDEBUG
DEBUGLINKFLAGS = /cod



# to build non-debug version: NMAKE NDEBUG=1    ( case sensitive )

!ifndef NDEBUG
# debug version
BIN    = debug
CC     = $(CCBASE) $(DEBUGCFLAGS)
LINK   = $(LINKBASE) $(DEBUGLINKFLAGS)
!else
# retail version
BIN    = retail
CC     = $(CCBASE) $(RETAILCFLAGS)
LINK   = $(LINKBASE)
!endif

all: $(BIN)\mshell.exe




# make target directory if necessary
!if [ mkdir $(BIN) 2>nul ]
!endif



OBJ  = $(BIN)\MSHELL.OBJ $(BIN)\CREATE.OBJ $(BIN)\OBJECT.OBJ $(BIN)\MENU.OBJ $(BIN)\DLG.OBJ
H    = MSHELL.H DEF.H
SRC  = *.c *.h *.rc *.dlg *.ico *.def makefile readme license.txt
LIBS = DDE4MBS OS2386



$(BIN)\dlg.obj: dlg.c $(H)
  $(CC) dlg.c

$(BIN)\menu.obj: menu.c   $(H)
  $(CC) menu.c

$(BIN)\create.obj: create.c   $(H)
  $(CC) create.c

$(BIN)\object.obj: object.c   $(H)
  $(CC) object.c

$(BIN)\mshell.obj: mshell.c    $(H)
  $(CC) mshell.c

$(BIN)\MSHELL.EXE: $(OBJ) mshell.def mshell.res
  $(LINK) $(OBJ), $(BIN)\MSHELL.EXE, $(BIN)\MSHELL.MAP, $(LIBS), MSHELL
  rc mshell.res $(BIN)\MSHELL.EXE
  cd $(BIN)
  mapsym mshell
  cd ..


mshell.res: mshell.dlg mshell.rc mshell.ico
  rc -r mshell.rc


zip:
  nmake source.zip
  nmake mshell.zip


source.zip: $(SRC)
  if exist source.zip  erase source.zip
  pkzip2 source $(SRC)

mshell.zip: retail\mshell.exe readme license.txt mshell.abs source.zip
  if exist mshell.zip  erase mshell.zip
  pkzip2 mshell $**

