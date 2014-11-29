# newshow.mak
# Created by IBM WorkFrame/2 MakeMake at 23:09:19 on 23 Aug 1995
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Link::Linker

.SUFFIXES: .cpp .obj 

.all: \
    E:\tankland\divesamp\newshow.exe

.cpp.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Tdp /Ti /Gm /Gd /Fo"%|dpfF.obj" /C %s

E:\tankland\divesamp\newshow.exe: \
    E:\tankland\divesamp\divewind.obj \
    E:\tankland\divesamp\newshow.obj \
    E:\tankland\divesamp\dive.obj \
    E:\tankland\divesamp\divehdlr.obj \
    {$(LIB)}mmpm2.lib
    @echo " Link::Linker "
    icc.exe @<<
    /Tdp 
     /Gm /Gd 
     /B" /de /pmtype:pm"
     /FeE:\tankland\divesamp\newshow.exe 
     mmpm2.lib 
     E:\tankland\divesamp\divewind.obj
     E:\tankland\divesamp\newshow.obj
     E:\tankland\divesamp\dive.obj
     E:\tankland\divesamp\divehdlr.obj
<<

E:\tankland\divesamp\divewind.obj: \
    E:\tankland\divesamp\divewind.cpp \
    {E:\tankland\divesamp;$(INCLUDE);}dive.hpp \
    {E:\tankland\divesamp;$(INCLUDE);}divehdlr.hpp \
    {E:\tankland\divesamp;$(INCLUDE);}divewind.hpp

E:\tankland\divesamp\divehdlr.obj: \
    E:\tankland\divesamp\divehdlr.cpp \
    {E:\tankland\divesamp;$(INCLUDE);}divehdlr.hpp

E:\tankland\divesamp\dive.obj: \
    E:\tankland\divesamp\dive.cpp \
    {E:\tankland\divesamp;$(INCLUDE);}dive.hpp \
    {E:\tankland\divesamp;$(INCLUDE);}dive.inl

E:\tankland\divesamp\newshow.obj: \
    E:\tankland\divesamp\newshow.cpp \
    {E:\tankland\divesamp;$(INCLUDE);}dive.hpp \
    {E:\tankland\divesamp;$(INCLUDE);}divehdlr.hpp \
    {E:\tankland\divesamp;$(INCLUDE);}divewind.hpp
