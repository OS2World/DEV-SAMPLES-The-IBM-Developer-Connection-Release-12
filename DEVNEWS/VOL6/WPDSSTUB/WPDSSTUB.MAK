#
# Makefile module: WPDSSTUB.MAK
# Author.........: Eric E. Osmann
# Copyright (c) International Business Machines Corporation 1994
#
# Use this makefile off src\pmwp directory.
#

## - Change the following name as appropriate - ##
appname   = wpsdsom

ROOT      = ..\..\..
SOMINC    = $(ROOT)\som20\include
SOMBIN    = $(ROOT)\som20\bin
SOMMSG    = $(ROOT)\som20\msg
SOMLIB    = $(ROOT)\som20\lib
H         = $(ROOT)\h
IBMH      = $(ROOT)\ibmh
IBMLIB    = $(ROOT)\ibmc\clib
IBMINC    = $(ROOT)\ibmc\cinclude
IBMMSG    = $(ROOT)\ibmc\chelp;$(ROOT)\ibmc\clocale;$(SOMMSG)
LIB       = $(ROOT)\lib
TOOLSPATH = $(SOMBIN);$(ROOT)\tools;$(ROOT)\ibmc\cbin
INCLUDE   = $(SOMINC);$(IBMINC);$(IBMH);$(H);..;.
DPATH     = $(IBMMSG);$(DPATH)

#
# -- Set our local environment here.
#
!if [set INCLUDE=$(INCLUDE);$(ROOT)\src\pmwp] || \
    [set LIB=$(SOMLIB);$(IBMLIB);$(LIB)] || \
    [set SMINCLUDE=$(SOMINC);..;.] || \
    [set SMTMP=%TMP%] || \
    [set SOMDDIR=.\] || \
    [set PATH=$(TOOLSPATH)] || \
    [set DPATH=$(DPATH)] || \
    [set HELP=$(IBMMSG);%help%] || \
    [set BOOKSHELF=$(IBMMSG);%bookshelf%]
!endif

CC      = icc
CFLAGS  = -D_OS2 -c -Q+ -Ss+ -Gd- -Gm+ -Gs+ -Sm -Kb -Ms -Ge+
LINKER  = link386
LFLAGS  = /exepack /packd /packc /align:4 /stack:360000 /pm:vio /noi /nol /map
LIBLIST = os2386 + somtk + dde4mbs

.SUFFIXES: .obj .c .h

.c.obj:
       $(CC) $(CFLAGS) -Ge+ $<

all:  $(appname).exe

$(appname).obj: $(appname).c
       $(CC) $(CFLAGS) $(appname).c

$(appname).exe: $(appname).obj
       $(LINKER) $(LFLAGS) \
                 $(appname).obj,\
                 $(appname).exe,\
                 $(appname).map,\
                 $(LIBLIST),\
                 $(appname);
       mapsym $(appname).map


