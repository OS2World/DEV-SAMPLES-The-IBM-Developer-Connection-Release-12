#===================================================================
#
#   OSA Test Make file
#
#  Copyright (C) 1991, 1994 IBM Corporation
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
#===================================================================

#-------------------------------------------------------------------
# WARPTOOLKIT points to the directory for the toolkit (e.g. d:\toolkit)
#-------------------------------------------------------------------
!include $(WARPTOOLKIT)\samples\ibmsamp.inc

#-------------------------------------------------------------------
# Add suffix and compile command for CPP files
#-------------------------------------------------------------------
.SUFFIXES: .rc .res .obj .lst .c .asm .hlp .itl .ipf .cpp

.c.obj:
    $(CC) -Fo$*.obj $*.c

#-------------------------------------------------------------------
#   A list of all of the object files
#-------------------------------------------------------------------
OBJS = osaevnt.obj osamain.obj osaobj.obj osaprof.obj ucmutils.obj \
scredit.obj testevnt.obj testrec.obj ucmstat.obj

#-------------------------------------------------------------------
#   This section lists all targets to be built by the make.  The
#   makefile builds the executable as well as its associated help
#   file.
#-------------------------------------------------------------------
all: osatest.exe

#-------------------------------------------------------------------
#   Definition file
#-------------------------------------------------------------------
osatest.def: osatest.mak
        @echo Creating .DEF file -- <<$(@R).def
NAME    osatest WINDOWAPI

DESCRIPTION 'OSA Test Application'

CODE    MOVEABLE
DATA    MOVEABLE MULTIPLE

HEAPSIZE   18192
STACKSIZE  20384
<<keep

#===================================================================
#
# Dependencies
#
#   This section lists all object files needed to be built for the
#   application, along with the files it is dependent upon (e.g. its
#   source and any header files).
#
#===================================================================

osatest.res: osatest.rc osatest.ico osatest.dlg osadlg.h
        rc -r osatest.rc

#-------------------------------------------------------------------
#   This section links the object modules into the finished program
#   using the linker command file created earlier.  At the end, the
#   resource file is bound to the application.
#-------------------------------------------------------------------
osatest.exe: $(OBJS) osatest.def osatest.res
    $(LINK) @<<$(@R).lnk
$(OBJS)
$(@R).exe
$(@R).map
$(MTLIBS) + opendoc.lib + somtk.lib
$(@R).def
<<keep
    rc osatest.res osatest.exe
    rem Done!
