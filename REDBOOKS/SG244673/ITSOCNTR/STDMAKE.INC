# ----Macro Definitions----Macro Definitions-----Macro Definitions----

SOMLIBPATH  = $(SOMBASE)\LIB
SOMIDLPATH  = $(SOMBASE)\INCLUDE
SOMINCPATH  = $(SOMBASE)\INCLUDE

ODLIBPATH   = $(ODBASE)\LIB
ODIDLPATH   = $(ODBASE)\IDL
ODINCPATH   = $(ODBASE)\INCLUDE

INCLUDEPATH = -I. -I$(INCLUDE) -I$(ODBASE)\OS2H -I$(PUBLIC) -I$(ODINCPATH) -I$(SOMINCPATH)
IDLPATH     = -I. -I$(SMINCLUDE) -I$(PUBLIC) -I$(ODIDLPATH) -I$(SOMIDLPATH)

SC          = sc
SCFLAGSXH   = -s"xh"     $(IDLPATH) -p -D _PLATFORM_OS2_ -S 98304
SCFLAGSXIH  = -s"xh;xih" $(IDLPATH) -p -D _PLATFORM_OS2_ -S 98304
SCFLAGSH    = -s"h"      $(IDLPATH) -p -D _PLATFORM_OS2_ -S 98304
SCFLAGSIH   = -s"h;ih"   $(IDLPATH) -p -D _PLATFORM_OS2_ -S 98304
SCDEF       = -sdef      $(IDLPATH) -p -D _PLATFORM_OS2_ -S 98304
SCCPP       = -sxc       $(IDLPATH) -p -D _PLATFORM_OS2_ -S 120000

IMPLIB     = implib

# ----Inference Rules-----Inference Rules----Inference Rules-----Inference Rules----

.SUFFIXES: .c .cpp .idl .pdl .h .ih .xh .xih .rc .res .def .lib .exe .dll

.idl.xh:
        $(SC) $(SCFLAGSXH) $*.idl

{$(PUBLIC)}.idl{$(PUBLIC)}.xh:
       $(SC) $(SCFLAGSXH) -d $(PUBLIC) $*.idl

.idl.xih:
        $(SC) $(SCFLAGSXIH) $*.idl

.idl.h:
        $(SC) $(SCFLAGSH) $*.idl

{$(PUBLIC)}.idl{$(PUBLIC)}.h:
        $(SC) $(SCFLAGSH) -d $(PUBLIC) $*.idl

.idl.ih:
        $(SC) $(SCFLAGSIH) $*.idl

.idl.cpp:
        $(SC) $(SCCPP) $*.idl

{$(PUBLIC)}.idl{$(PUBLIC)}.def:
        $(SC) $(SCDEF) -d $(PUBLIC) $*.idl

.idl.def:
        $(SC) $(SCDEF) $*.idl

.def{$(PUBLIC)}.lib:
       implib $(PUBLIC)\$(@B).lib $(@B).def

{$(PUBLIC)}.def{$(PUBLIC)}.lib:
       implib $(PUBLIC)\$(@B).lib $(PUBLIC)\$(@B).def

.idl{$PUBLIC}.pdl:
        pdl $(@B).idl
        @copy $(@B).pdl $(PUBLIC)\$(@B).pdl
        @copy $(PUBLIC)\$(@B).pdl $(PUBLIC)\$(@B).idl

.rc.res:
        rc -r $*.rc

.cpp.obj:
        $(CC) $(CFLAGS) $*.cpp

