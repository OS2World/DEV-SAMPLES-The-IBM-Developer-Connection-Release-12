# Createdby IBM WorkFrame/2 MakeMake at 19:07:23 on 04/06/94
#
# This makefile should be run in the following directory:
#   .\parts\hello 
# (NOTE: Validated for DevCon 6; earlier toolkits will not work without a fix to the
#    container part)
#   Notes:
#   If you prefer to build without debug information, switch between the two DEBUG macros.
#
#       Final Version for DevCon6               Robert Tycast           1/06/95

FILENAME = hello
CNTNRPRT = ..\cntnrprt
INCLUDEPATH = .;..\..\..\os2h;..\..\..\include;..\cntnrprt;..\public
LIBPATH = ..\..\..\lib
IDLPATH = ..\..\..\idl
PUBLIC=..\public
#DEBUG = /Ti-
DEBUG = /Ti+


.all: idl $(FILENAME).dll 

idl: $(PUBLIC)\cntnrprt.xh $(FILENAME).xh 

.SUFFIXES: .idl .xh .cpp .rc .c .lib .dll .obj

$(FILENAME).xh: $(FILENAME).idl
$(FILENAME).cpp: $(FILENAME).xh
$(PUBLIC)\cntnrprt.lib: $(PUBLIC)\cntnrprt.xh

.xh.cpp:
        sc -D _PLATFORM_OS2_ -I$(IDLPATH) -I$(PUBLIC) "-sxh;xih" -p $(@B).idl

.idl.xh:
        sc  -D _PLATFORM_OS2_ -I$(IDLPATH) -I$(PUBLIC) "-sxh;def" $(@B).idl

.cpp.obj:
      @echo WF::COMPILE::C Set ++ Compile
      icc.exe /Gm+ /Ge- /Tl- /Fd /Gd+ /I$(INCLUDEPATH) /Sp4 /Q /Fi /Si $(DEBUG) /N30 /Ft"$(LIBPATH)" /C %s

$(FILENAME).res:   $(FILENAME).rc
     rc  -r -I ..\..\include $(FILENAME).rc

$(FILENAME).dll:\
    .\$(FILENAME).obj \
    .\$(FILENAME).res \
    $(LIBPATH)\opendoc.lib \
    $(PUBLIC)\cntnrprt.lib \
    .\$(FILENAME).def
      @echo WF::LINK::C Set ++ Link
      icc.exe @<<
/Tl- /Fd /Ge- /Gd+
 /Sp4 /Q
 /Fi /Si
 /N30 $(DEBUG)
 /B" /de /pmtype:pm /nologo /nod /noi /noe /map"
 /Fe$(FILENAME).dll
 /Fm$(FILENAME).map
 .\hello.obj
 os2386.lib
 somtk.lib
 $(LIBPATH)\os2cmbsi.lib
 $(LIBPATH)\OpenDoc.lib
 $(PUBLIC)\cntnrprt.lib
 .\$(FILENAME).def
<<
        rc $(FILENAME).res $(FILENAME).dll
