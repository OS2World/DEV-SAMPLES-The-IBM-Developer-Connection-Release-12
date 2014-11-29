# makefile
# Created by IBM WorkFrame/2 MakeMake at 23:40:34 on 24 July 1995
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Link::Linker
#
# Modified by Larry Kyrala at 23:46 on 24 July 1995
#  changed paths to local directories for flexible builds.

ERASE = ERASE

.SUFFIXES: .cpp .obj

.all:  newshow.exe

.cpp.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Tdp /Gm /Gd /Fo"%|dpfF.obj" /C %s

newshow.exe:      \
    divewind.obj  \
    newshow.obj   \
    dive.obj      \
    divehdlr.obj  \
    {$(LIB)}mmpm2.lib
    @echo " Link::Linker "
    icc.exe @<<
    /Tdp
     /Gm /Gd
     /B" /pmtype:pm"
     /Fenewshow.exe
     mmpm2.lib
     divewind.obj
     newshow.obj
     dive.obj
     divehdlr.obj
<<


divewind.obj:                     \
    divewind.cpp                  \
    {.;$(INCLUDE);}divewind.hpp   \
    {.;$(INCLUDE);}dive.hpp       \
    {.;$(INCLUDE);}divehdlr.hpp


divehdlr.obj:                     \
    divehdlr.cpp                  \
    {.;$(INCLUDE);}divehdlr.hpp


dive.obj:                         \
    dive.cpp                      \
    {.;$(INCLUDE);}dive.hpp       \
    {.;$(INCLUDE);}dive.inl


newshow.obj:                     \
    newshow.cpp                  \
    {.;$(INCLUDE);}divewind.hpp  \
    {.;$(INCLUDE);}dive.hpp      \
    {.;$(INCLUDE);}divehdlr.hpp


clean:
       $(ERASE) *.OBJ


clobber:
       $(ERASE) *.OBJ
       $(ERASE) *.EXE

