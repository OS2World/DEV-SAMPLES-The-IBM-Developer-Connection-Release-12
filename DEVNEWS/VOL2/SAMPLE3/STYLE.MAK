#===================================================================
#
#  Style Sample Makefile
#
#  Copyright (C) 1992 IBM Corporation
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
#===================================================================

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
LINK    = LINK386  $(LDFLAGS)
VLIBS   = DDE4NBS + vdh + os2386
MTLIBS   = DDE4MBS + os2386
LDFLAGS   = /NOI /NOE /NOD /MAP:0 /DEB

.c.lst:
    $(CC) -Fc$*.lst -Fo$*.obj $*.c

.c.obj:
    icc /Gm+ /Ss+ /Wpro+ /Ti+ /Q /C $< ;

#===================================================================
#
#   A list of all of the object files
#
#===================================================================
ALL_OBJ1 = sty_main.obj sty_user.obj sty_init.obj sty_pnt.obj sty_dlg.obj
ALL_OBJ2 = sty_file.obj sty_edit.obj sty_help.obj os2thrd.obj
HEADERS  = sty_dlg.h sty_help.h sty_main.h sty_xtrn.h
#===================================================================
#
#   A list of all of the Help files
#
#===================================================================
ALL_IPF = style.ipf sty_file.ipf sty_edit.ipf sty_help.ipf sty_menu.ipf \
          sty_dlg.ipf

#===================================================================
#
#   Dependencies
#
#===================================================================

all: style.exe style.hlp

# Resources
#
sty_main.res:      sty_main.h sty_main.ico sty_main.rc
        rc -r sty_main.rc

style.lnk: style.mak
    echo $(ALL_OBJ1) +         > style.lnk
    echo $(ALL_OBJ2)          >> style.lnk
    echo style.exe            >> style.lnk
    echo style.map            >> style.lnk
    echo $(MTLIBS)            >> style.lnk
    echo style.def            >> style.lnk

sty_main.res: sty_main.rc sty_main.h sty_dlg.h sty_main.dlg sty_help.rc sty_help.h

sty_main.obj: sty_main.c $(HEADERS)

sty_file.obj: sty_file.c $(HEADERS)

sty_edit.obj: sty_edit.c $(HEADERS)

sty_user.obj: sty_user.c $(HEADERS)

sty_init.obj: sty_init.c $(HEADERS)

sty_pnt.obj: sty_pnt.c $(HEADERS)

sty_dlg.obj: sty_dlg.c $(HEADERS)

sty_help.obj: sty_help.c $(HEADERS) sty_help.h

style.hlp: $(ALL_IPF)

style.exe: $(ALL_OBJ1) $(ALL_OBJ2) style.def style.lnk sty_main.res
    $(LINK) @style.lnk
    rc -p -x sty_main.res style.exe
