#===================================================================
#
#   Copyright 1994 IBM Corporation
#
#===================================================================
# D.W. Kenner
# Make file for the  test program  that allows the test of
# a  real mode or protect mode process to interface with a VDD.
#
#
#===================================================================
#
#
#===================================================================
.SUFFIXES:
.SUFFIXES: .rc .res .obj .lst .c .asm .hlp .itl .ipf
#===================================================================
# Default compilation macros for sample programs
#

ROOT      = \c600\bin
CROOT     = \r207\tools
CC        = $(ROOT)\cl
ASM       = $(ROOT)\masm


GCFLAGS  =
OFLAGS   = /O- /Rn /Gr+ /Fa /Ls+
CFLAGS   = -G2s -Ons -W3 -Alfu -Zep  -nologo /B1 c1l.exe -Zi -Od  /c /Zi
LFLAGS   = /NOI /NOE  /MAP:0  /COD  /NOD
LINK     = LINK $(LFLAGS)
LIBS     = \c600\lib\llibcer.lib
LDFLAGS  =  /CODEVIEW

.c.lst:
    $(CC) -Fc$*.lst -Fo$*.obj $*.c

.c.obj:
   $(CC) $(CFLAGS) $(CFLAGS) $(CINC) $< ;

.asm.obj:
     $(ASM) /Zi  $< ;


#===================================================================
#
#   A list of all of the object files
#
#===================================================================

COBJ = winos2.obj wininit.obj





#-------------------------------------------------------------------
#   This section lists all files to be built by the make.  The
#   makefile builds the executible as well as its associated help
#   file.
#-------------------------------------------------------------------
all: winos2.exe



#-------------------------------------------------------------------
#   This section creates the command file used by the linker.  This
#   command file is recreated automatically every time you change
#   the object file list, linker flags, or library list.
#-------------------------------------------------------------------

#put in dependency for make file
winos2.lnk: winos2.mak
     echo $(COBJ)                  >  winos2.lnk
     echo winos2.exe               >> winos2.lnk
     echo winos2.map               >> winos2.lnk
     echo $(LIBS)                  >> winos2.lnk
     echo nul.def               >> winos2.lnk

#===================================================================
#
# Dependencies
#
#   This section lists all object files needed to be built for the
#   application, along with the files it is dependent upon (e.g. its
#   source and any header files).
#
#===================================================================


winos2.obj:winos2.c

wininit.obj:    wininit.asm


winos2.exe:  winos2.obj wininit.obj  winos2.lnk
            $(LINK)  @winos2.lnk


