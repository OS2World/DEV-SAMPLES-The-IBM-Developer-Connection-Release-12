#****************************************************************************
#
#  Work Place Shell - PileOf Sample makefile - SOM 2.0 / IDL version
#
#  Copyright (C) 1993, 1994 IBM Corporation
#
#      DISCLAIMER OF WARRANTIES.  The following [enclosed] code is
#      sample code created by IBM Corporation. This sample code is not
#      part of any standard or IBM product and is provided to you solely
#      for  the purpose of assisting you in the development of your
#      applications.  The code is provided "AS IS", without
#      warranty of any kind.  IBM shall not be liable for any damages
#      arising out of your use of the sample code, even if they have been
#      advised of the possibility of such damages.
#
#      NOTE: you will have to modify the environment variables to suit
#            your environment
#
#****************************************************************************
#!include ..\..\ibmsamp.inc

#****************************************************************************
#  Dot directive definition area (usually just suffixes)
#****************************************************************************
.SUFFIXES: .c .obj .dll .idl .h .ih .rc .res

#****************************************************************************
#  Environment Setup for the component(s).
#****************************************************************************

SOMTEMP = .\somtemp

!if [set SMTMP=$(SOMTEMP)] || \
    [set SMEMIT=ih;h;c] || \
    [set SMADDSTAR=1] || \
    [set SMNOTC=1]
!endif

!if [cd $(SOMTEMP)]
!  if [md $(SOMTEMP)]
!    error error creating $(SOMTEMP) directory
!  endif
!else
!  if [cd ..]
!    error - Couldn't cd .. from $(SOMTEMP) directory
!  endif
!endif

#
# Compiler/tools Macros
#
#===================================================================
# Default compilation macros for sample programs
#
# Compile switches  that are enabled
# /c      compile don't link
# /Gm+    use the multi-threaded libraries
# /ss     allow  "//" for comment lines
# /Ms     use the system calling convention and not _optlink as the default
# /Gd-    disable optimization
# /Se     allow C Set/2  extensions
#
#

CC         = icc /c /Gd- /Se /Re /ss /Ms /Gm+

CC      = $(CC) /Ge-
LINK    = LINK386
LDFLAGS = /noi /map /nol /nod /exepack /packcode /packdata /align:16
#DLLLIBS = $(DLLLIBS) somtk.lib
DLLLIBS = OS2386.LIB DDE4MBS.LIB somtk.lib

#****************************************************************************
# Set up Macros that will contain all the different dependencies for the
# executables and dlls etc. that are generated.
#****************************************************************************

OBJS = pileof.obj dbg.obj

#****************************************************************************
#   Setup the inference rules for compiling source code to
#   object code.
#****************************************************************************

.idl.ih:
        sc -p -v -maddstar -mnoint -S128000 -C128000  $*.idl

.idl.c:
        sc -p -v -maddstar -mnoint -S128000 -C128000 -sc $*.idl

.c.obj:
        $(CC) -I$(INCLUDE) -c $<

all: pileof.dll

#
# Specific Process Tag
#

pileof.ih:   $$(@B).idl

dbg.obj: $$(@B).c

pileof.obj: $$(@B).c $$(@B).ih $$(@B).h  $$(@B).idl

pileof.dll: $$(@B).def $(OBJS) pileof.res
         $(LINK) $(LDFLAGS) $(OBJS),$@,,$(DLLLIBS),$*;
         rc -p -x $*.res $*.dll
         mapsym pileof.map



pileof.res: $$(@B).rc $$(@B).ih
         rc -r $*.rc $*.res
