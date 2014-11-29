# rxdraw makefile
# nmake debug   to build debug version with asserts
# nmake         to build retail version, no asserts

# basic settings
CCBASE   = icc /c /Gd- /Se /Re /ss /Ms /Gm+ /Q+ /Fo$*.obj /Gs- /Kb+
LIBS     = dde4mbs os2386 rexx

# default is retail build
BIN           = RETAIL
MORECFLAGS    = /DNDEBUG
MORELINKFLAGS =

# overrides for debug build
!ifdef DEBUG
BIN           = DEBUG
MORECFLAGS    = /Ti+
MORELINKFLAGS = /DEBUG
!endif

CC   = $(CCBASE) $(MORECFLAGS)
LINK = link386 /nod /map $(MORELINKFLAGS)
OBJ  = $(BIN)\rxdraw.obj $(BIN)\object.obj
H    = rxdraw.h def.h pmassert.h

all: $(BIN)\rxdraw.exe

debug:
  nmake DEBUG=1

zip:
  nmake source.zip


# mkdir target directory for binaries
!if [ mkdir $(BIN) 2>nul ]
!endif


$(BIN)\object.obj: object.c   $(H)
  $(CC) object.c

$(BIN)\rxdraw.obj: rxdraw.c    $(H)
  $(CC) rxdraw.c

rxdraw.res: rxdraw.rc rxdraw.ico
  rc -r rxdraw.rc

# write linker response file
$(BIN)\rxdraw.rsp: makefile
  @echo Writing <<$(BIN)\RXDRAW.RSP
$(OBJ)
$(BIN)\rxdraw.exe
$(BIN)\rxdraw
$(LIBS)
rxdraw.def
<<KEEP


# link the exe
$(BIN)\rxdraw.exe: $(OBJ) rxdraw.res rxdraw.def $(BIN)\rxdraw.rsp
  $(LINK) @$(BIN)\rxdraw.rsp
  rc rxdraw.res $(BIN)\rxdraw.exe
  cd $(BIN)
  mapsym rxdraw
  cd ..



# zip the source
source.zip: *.c *.h *.rc readme *.cmd *.def *.ico
  if exist source.zip erase source.zip
  pkzip2 source $**
