#===================================================================
#
#   Copyright 1992 IBM Corporation
#
#===================================================================

#
#

#===================================================================
#
#   Sample application makefile, common definitions for the IBM C
#   Set II compiler environment
#
#===================================================================
.SUFFIXES:
.SUFFIXES: .rc .res .obj .lst .c .asm .hlp .itl .ipf
#===================================================================
# Default compilation macros for sample programs
#
# Compile switches  that are enabled
# /c      compile don't link
# /ss     allow  "//" for comment lines
# /Ms     use the system calling convention and not _optlink as the default
# /Gd-    disable optimization
# /Se     allow C Set/2  extensions
#
#

CC         = icc

GCFLAGS =
OFLAGS  = /O- /Rn /Gr+ /Fa /Ls+
CFLAGS  = /G3 /Sp1 /W3 /Ss+ /DM_I386 $(CINC) /Q /C $(GCFLAGS) $(FLAGS)
SUPPRESSDLLLIBRARY = /gp
LFLAGS   = /NOI /NOE /NOD /MAP:0 /EXEPACK /ALIGN:16
LINK    = LINK386  $(LFLAGS)
VLIBS   = DDE4NBS + vdh + os2386
MLIBS   = DDE4MBS + os2386
LDFLAGS   = /NOI /NOE /NOD /MAP:0 /DEB

.c.lst:
    $(CC) -Fc$*.lst -Fo$*.obj $*.c

.c.obj:
#    icc /Gm+ /Ss+ /Wpro+ /Ti+ /Q /C $< ;
   $(CC) $(OFLAGS) $(CFLAGS) $(CINC) $< ;

#===================================================================
#
#   A list of all of the object files
#
#===================================================================

COBJ = vdft.obj
OS2OBJ = os2thrd.obj





#-------------------------------------------------------------------
#   This section lists all files to be built by the make.  The
#   makefile builds the executible as well as its associated help
#   file.
#-------------------------------------------------------------------
all: vdft.sys os2proc.exe



#-------------------------------------------------------------------
#   This section creates the command file used by the linker.  This
#   command file is recreated automatically every time you change
#   the object file list, linker flags, or library list.
#-------------------------------------------------------------------

#put in dependency for make file
os2proc.lnk:
     echo $(OS2OBJ)                >  os2proc.lnk
     echo os2proc.exe              >> os2proc.lnk
     echo os2proc.map              >> os2proc.lnk
     echo $(MLIBS)                  >> os2proc.lnk
     echo os2proc.def              >> os2proc.lnk





#===================================================================
#
# Dependencies
#
#   This section lists all object files needed to be built for the
#   application, along with the files it is dependent upon (e.g. its
#   source and any header files).
#
#===================================================================


os2proc.obj: os2thrd.c
     icc /Gm+ /Ss+ /Wpro+ /Ti+ /Q /C os2thrd.c


os2proc.exe:  $(OS2OBJ) os2proc.def os2proc.lnk
            $(LINK) $(LDFLAGS) @os2proc.lnk


