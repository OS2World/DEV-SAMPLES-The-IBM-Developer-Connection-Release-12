###############################################################################
#
#--------------------------------------------------------------------------
#
# MAKE file for the PMJRN
#
#--------------------------------------------------------------------------
#
###############################################################################

#------------------------------------------------------------------------------
# Symbolic names for PRODuct pieces
#------------------------------------------------------------------------------

# this is our PRODuct ID

PROD    = PMJRN

PRODVER = 1.00

PRODDEF = $(PROD).DEF
PRODDLL = $(PROD)DLL.DLL
PRODEXE = $(PROD).EXE
PRODLIB = $(PROD)DLL.LIB
PRODMAP = NUL.MAP

PRODSRC = pmjrn.c   pmjrnhel.c   pmjrnwin.c
PRODOBJ = pmjrn.obj pmjrnhel.obj pmjrnwin.obj
PRODHLP = pmjrn.hlp pmjrn.new

#------------------------------------------------------------------------------
# These are the external interface LIBraries we require
#------------------------------------------------------------------------------

EXTERN_LIB      =
EXTERN_LIB_C    =
EXTERN_LIB_OS2  = os2

#------------------------------------------------------------------------------
# C parameters
#------------------------------------------------------------------------------

cWARN           = /W3
cMEM            = /Gt
cOPTS           = /c /Ge+ /Gm

# decide about listings..

!ifdef wantLST
cLST            = /Fa
!endif

# decide about CODEVIEW...

!ifdef wantCV
cOPTS           = $(cOPTS) /Ti+
!else
cOPTS           = $(cOPTS) /O+
!endif

#------------------------------------------------------------------------------
# LINK parameters
#------------------------------------------------------------------------------

lMain           = /MAP

!ifdef wantCV
lMain           = $(lMain)/COD
!endif

#-------------------------------------------------------------------------------
# *** THIS is THE inference rule to generate our code.... ***
#
# the leading '-' tells MAKE to not abort on a CL error
#
#------------------------------------------------------------------------------

.c.obj:
#
  icc  $(cOPTS) $(cMEM) $(cWARN) $(cLST) $*.c

#-------------------------------------------------------------------------------
#
# *** THIS is THE major dependency rule...it forces anything to happen
#
#------------------------------------------------------------------------------

$(PRODEXE):   pmjrn.h             \
              $(PRODSRC)          \
              $(PRODOBJ)          \
              pmjrn.def           \
              pmjrn.rc  pmjrn.res \
              $(PRODHLP)          \
              pmjrn.ico pmjrn.ptr \
              $(PRODDLL)
  link386 $(lMain) @<<
$(PRODOBJ)
$(PRODEXE)
$(PRODMAP)
$(PRODLIB)
$(PRODDEF)
<<
  rc pmjrn.res

pmjrn.obj:    pmjrn.c    pmjrn.h

pmjrnhel.obj: pmjrnhel.c pmjrn.h

pmjrnwin.obj: pmjrnwin.c pmjrn.h

pmjrn.res:    pmjrn.rc   pmjrn.h pmjrn.ico pmjrn.ptr $(PRODHLP)
  rc -r pmjrn.rc
