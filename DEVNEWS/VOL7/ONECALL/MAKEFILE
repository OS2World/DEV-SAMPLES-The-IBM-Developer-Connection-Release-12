# makefile for a SETUP.C and a driver program to test it.
# nmake                for a "retail" build
# nmake DEBUG=1        for a debug build ready for IPMD

# basic settings
CCBASE   = icc /c /Gd- /Se /Re /ss /Ms /Gm+  /Q+ /Fo$*.obj  /Gs- /Kb+
LINKBASE = link386 /nod /map
LIBSBASE = dde4mbs os2386

# default settings for retail
BIN    = RETAIL
CC     = $(CCBASE) /DNDEBUG
LINK   = $(LINKBASE)
LIBS   = $(LIBSBASE)


!ifdef DEBUG
# override settings for debug
BIN    = DEBUG
CC     = $(CCBASE) /Ti+
LINK   = $(LINKBASE) /DEBUG
LIBS   = $(LIBSBASE)
!endif


#-------------------------------------------------------------------------
# mkdirs for OBJ and EXE output
!if [ md $(BIN) 2>nul ]
!endif


#-------------------------------------------------------------------------
# build targets

all: $(BIN)\driver.exe

debug:
  nmake DEBUG=1


#-------------------------------------------------------------------------
#

driver.res: driver.rc  setup.dlg
  rc -r driver.rc

$(BIN)\setup.obj: setup.c setup.h
  $(CC) setup.c

$(BIN)\driver.obj: driver.c driver.h setup.h
  $(CC) driver.c

$(BIN)\object.obj: object.c driver.h setup.h
  $(CC) object.c


#-------------------------------------------------------------------------
# write DEF file

$(BIN)\driver.def: makefile
  @echo Writing <<$(BIN)\driver.def
NAME driver WINDOWAPI
<<KEEP


#-------------------------------------------------------------------------
# link exe

$(BIN)\driver.exe: $(BIN)\driver.obj $(BIN)\object.obj $(BIN)\setup.obj $(BIN)\driver.def makefile driver.res
  cd $(BIN)
  $(LINK) driver object setup, driver,, $(LIBS), driver
  rc ..\driver.res driver.exe
  mapsym driver
  cd ..


#-------------------------------------------------------------------------
# build a readme

readme.out: readme code1 code2
  if exist readme.out attrib -r readme.out
  em readme >readme.out
  attrib +r readme.out
  e readme.out


zip:
  if exist print.zip erase print.zip
  pkzip2 print *.c *.h makefile *.rc *.dlg readme readme.out *.cmd code*


