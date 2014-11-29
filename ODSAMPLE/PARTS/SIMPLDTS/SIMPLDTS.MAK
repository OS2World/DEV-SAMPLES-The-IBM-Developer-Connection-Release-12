#***************************************************************************
#
#  File Name   : SIMPLDTS.MAK
#
#  Description : Makefile for the DTS SimplePart OpenDoc part handler class
#
#  Notes       : This makefile should be run from this sample's subdirectory.
#
#                Regarding the DEBUG macro statements:
#                The file is shipped so that the build will not produce
#                debug information ("DEBUG = /Ti-" is uncommented and
#                "DEBUG = /Ti+" is commented out).  If you prefer to build
#                with debug information, comment out the first DEBUG macro
#                and uncomment the second one.
#
#       (C) COPYRIGHT International Business Machines Corp. 1995
#       All Rights Reserved
#       Licensed Materials - Property of IBM
#
#       US Government Users Restricted Rights - Use, duplication or
#       disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
#       DISCLAIMER OF WARRANTIES.  The following [enclosed] code is
#       sample code created by IBM Corporation. This sample code is not
#       part of any standard or IBM product and is provided to you solely
#       for the purpose of assisting you in the development of your
#       applications.  The code is provided "AS IS", without
#       warranty of any kind.  IBM shall not be liable for any damages
#       arising out of your use of the sample code, even if they have been
#       advised of the possibility of such damages.
#
#***************************************************************************

#===========================================================================
#  MACRO DEFINITIONS
#===========================================================================

PARENT   =
FILENAME = simpldts

DEBUG    = /Ti-
#DEBUG   = /Ti+

BASEPATH = $(WARPTOOLKIT)
PUBLIC   = $(BASEPATH)\samples\opendoc\parts\public
BETAPATH = $(WARPTOOLKIT)\beta
BETAPUBLIC = $(BETAPATH)\samples\opendoc\parts\public
PUBUTILS = $(BETAPATH)\samples\opendoc\pubutils
INCLUDEPATH = .;$(PUBLIC);$(BETAPUBLIC);$(PUBUTILS)

#---------------------------------------------------------------------------
#  OBJLIST   is the list of .OBJ files that will be linked with your .DLL
#            file.
#---------------------------------------------------------------------------

OBJLIST = $(FILENAME).obj ordcoll.obj

#===========================================================================
#  PSEUDOTARGETS:
#===========================================================================

.all:  $(FILENAME).dll $(PUBLIC)\$(FILENAME).lib

.SUFFIXES: .idl .xh .cpp .hpp .rc .c .lib .dll .obj


#===========================================================================
#  INFERENCE RULES:
#===========================================================================

.cpp.obj:
      icc.exe /Gm+ /Ge- /Tl- /Fd /Gd+ /I$(INCLUDEPATH) /Sp4 /Q /Fi /Si $(DEBUG) /N30 /C %s


#===========================================================================
#  DESCRIPTION BLOCKS (TARGETS):
#  Each .OBJ file should have its own description block with the right
#  dependent(s).
#===========================================================================

$(PUBUTILS)\pubutils.lib:
   @echo **************************************************  & \
    echo **************************************************  & \
    echo ****                                          ****  & \
    echo ***     $(PUBUTILS)\pubutils.mak not found!       *** & \
    echo ***                                            ***  & \
    echo ***   You need to make it before you           *** & \
    echo ***          can build this sample             ***  & \
    echo ****                                          ****  & \
    echo **************************************************  & \
    exit

$(FILENAME).obj:  $(FILENAME).cpp

$(FILENAME).dll:\
   $(OBJLIST) \
   $(FILENAME).def
   icc.exe @<<
   /Tl- /Fd /Ge- /Gd+ /Sp4 /Q /Fi /Si
   /N30 $(DEBUG)
   /B" /de /pmtype:pm /nod /noi /noe /map"
   /Fe$*.dll
   /Fm$*.map
   $(OBJLIST)
   somtk.lib
   os2386.lib
   cppom30.lib
   $(PUBUTILS)\pubutils.lib
   opendoc.lib
   $(FILENAME).def
<<


$(PUBLIC)\$(FILENAME).lib: $(FILENAME).dll
   @echo IMPLIB::Import library contruction
   IMPLIB /NOLOGO $(PUBLIC)\$(FILENAME).lib $(FILENAME).dll


#---------------------------------------------------------------------------
#  Remove build output
#---------------------------------------------------------------------------
CLEANUP:
   @echo ************************************************** & \
    echo **  Cleaning DTS Simple part sample build output** & \
    echo **************************************************
   -del *.def *.x* *.dll *.map *.obj >nul 2>&1
   -if exist CSET2PRE del CSET2PRE /n >nul 2>&1 & rd CSET2PRE >nul 2>&1
