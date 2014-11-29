#****************************************************************************
#
#  PWFolder Work Place Shell Sample makefile - SOM 2.0 / IDL version
#
#  Converted from OS/2 Version 2.0 Technical Compendium, Volume 4,
#  GG24-3724 with permission.
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
#            that your wpshidl directory is on d:\ as well.
#
#****************************************************************************

#****************************************************************************
#  Dot directive definition area (usually just suffixes)
#****************************************************************************

.SUFFIXES: .c .obj .dll .idl .h .ih .rc .res

#****************************************************************************
#  Environment Setup for the component(s).
#****************************************************************************

INCLUDE=d:\som\include;d:\ibmcpp\include;d:\wpshidl\h;d:\toolkt21\c\os2h;.;
LIB=d:\som\lib;d:\ibmcpp\lib;d:\toolkt21\os2lib;
SMINCLUDE=d:\som\include;d:\wpshidl\idl;d:\toolkt21\sc;.;
SMTMP=%TMP%
PATH=d:\som\bin;d:\ibmcpp\bin;d:\toolkt21\os2bin;
SMEMIT=ih;h

#
# Compiler/tools Macros
#

CC         = icc /c /Ge- /Gd- /Se /Re /ss /Ms /Gm+
LINK       = link386
LDFLAGS    = /noi /map /nol /nod /exepack /packcode /packdata /align:16 /noe
LIBS       = os2386.lib dde4nbs.lib somtk.lib

#****************************************************************************
# Set up Macros that will contain all the different dependencies for the
# executables and dlls etc. that are generated.
#****************************************************************************

OBJS       = pwfolder.obj

#****************************************************************************
#   Setup the inference rules for compiling source code to
#   object code.
#****************************************************************************

.c.obj:
        $(CC) -I$(INCLUDE) -c $<

.idl.ih:
        sc -maddstar -mnoint -r -S128000 -C128000 -v -p  $*.idl

all: pwfolder.dll

pwfolder.obj: $*.c $*.ih $*.h $*.idl

pwfolder.dll: $(OBJS) pwfolder.res
         $(LINK) $(LDFLAGS) $(OBJS),$@,,$(LIBS),$*;
         rc $*.res $*.dll
         mapsym pwfolder.map

pwfolder.res: pwfolder.rc
         rc -r $*.rc $*.res
