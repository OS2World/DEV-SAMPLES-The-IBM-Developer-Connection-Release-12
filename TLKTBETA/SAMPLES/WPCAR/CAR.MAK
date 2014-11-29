#****************************************************************************
#
#  Work Place Shell Sample makefile - SOM 2.0 / IDL version
#
#  Copyright (C) 1992, 1993 IBM Corporation
#
#      DISCLAIMER OF WARRANTIES.  The following [enclosed] code is
#      sample code created by IBM Corporation. This sample code is not
#      part of any standard or IBM product and is provided to you solely
#      for  the purpose of assisting you in the development of your
#      applications.  The code is provided "AS IS", without
#      warranty of any kind.  IBM shall not be liable for any damages
#      arising out of your use of the sample code, even if they have been
#      advised of the possibility of such damages.                                                    *
#
#      NOTE: you will have to modify the environment variables to suit
#            your environment
#
#            It assumes that your SOMBASE directory is d:\som and
#            that your wpshidl\h directory is on d:\ as well.
#
#****************************************************************************

#****************************************************************************
#  Dot directive definition area (usually just suffixes)
#****************************************************************************

.SUFFIXES: .c .obj .dll .idl .h .ih .rc .res

#****************************************************************************
#  Environment Setup for the component(s).
#****************************************************************************

INCLUDE=d:\som\include;d:\ibmcpp\include;d:\wpshidl\h;d:\toolkt21\c\os2h;.
LIB=d:\som\lib;d:\ibmcpp\lib;d:\toolkt21\os2lib;
SMINCLUDE=d:\som\include;d:\wpshidl\idl;.;
SMTMP=%TMP%
PATH=d:\som\bin;d:\ibmcpp\bin;d:\toolkt21\os2bin;
SMEMIT=ih;h

!if [set INCLUDE=d:\som\include;d:\ibmcpp\include;d:\wpshidl\h;d:\toolkt21\c\os2h;.] || \
    [set LIB=d:\som\lib;d:\ibmcpp\lib;d:\toolkt21\os2lib;] || \
    [set SMINCLUDE=d:\som\include;d:\wpshidl\idl;.;] || \
    [set SMTMP=%TMP%] || \
    [set PATH=d:\som\bin;d:\ibmcpp\bin;d:\toolkt21\os2bin;] || \
    [set SMEMIT=ih;h]
!endif




#
# Compiler/tools Macros
#

CC         = $(CC) /Ge-
LINK       = LINK386
LDFLAGS    = /noi /map /nol /nod /exepack /packcode /packdata /align:16
DLLLIBS    = $(DLLLIBS) os2386.lib dde4nbs.lib somtk.lib

! ifdef DEBUG

LISTING  = $(CC) /Le /Li /Ls /Fc
ERRLEVEL = /W3 /Ka /Kg /Kp /Kr
CC       = $(CC) /Ti /DDEBUG
LDFLAGS  = $(LDFLAGS) /MAP /CODEVIEW

! endif

#****************************************************************************
# Set up Macros that will contain all the different dependencies for the
# executables and dlls etc. that are generated.
#****************************************************************************

OBJS = car.obj
! ifdef INCL_INT3
OBJS = $(OBJS) int3.obj
! endif

#****************************************************************************
#   Setup the inference rules for compiling source code to
#   object code.
#****************************************************************************

.c.obj:
        $(CC) -I$(INCLUDE) -c $<

.idl.ih:
        sc -maddstar -mnoint -r -S128000 -C128000 -v -p $*.idl

all: car.dll

#
# Specific Process Tag
#

! ifdef INCL_INT3
int3.obj:
    $(ASM)   $*.asm
! endif

car.ih:  $$(@B).idl

car.obj: $$(@B).c $$(@B).ih $$(@B).h $$(@B).idl

car.dll: $$(@B).def $(OBJS) car.res
         $(LINK) $(LDFLAGS) $(OBJS),$@,,$(DLLLIBS),$*;
         rc $*.res $*.dll
         mapsym car.map

car.res: car.rc car.ih
         rc -r $*.rc $*.res

#INCLUDES -C=ih

# Since all these files are emitted from the .IDL file, and there's
# nothing unique in the emitted files, we're going to emit them from scratch
# each time

clean:
    -mv -r build.lst deltas
    -mv -r checked.out deltas
    -del *.h *.ih *.log *.obj *.dll $*.slk asdf text.tmp
    -del *.res *.map *.scf *.sof *.som *.sym initterm.* dllstub.*
