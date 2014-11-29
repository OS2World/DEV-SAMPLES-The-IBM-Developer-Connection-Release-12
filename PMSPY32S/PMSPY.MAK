#*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
#*³                                                                          ³*/
#*³ PROGRAM NAME: PMSPY                                                      ³*/
#*³ -------------                                                            ³*/
#*³  A PM program that is used to look at or 'spy' on the message queue of   ³*/
#*³  other PM applications windows.                                          ³*/
#*³                                                                          ³*/
#*³ COPYRIGHT:                                                               ³*/
#*³ ----------                                                               ³*/
#*³  Copyright (C) International Business Machines Corp., 1992               ³*/
#*³                                                                          ³*/
#*³ DISCLAIMER OF WARRANTIES:                                                ³*/
#*³ -------------------------                                                ³*/
#*³  The following [enclosed] code is sample code created by IBM Corporation.³*/
#*³  This sample code is not part of any standard IBM product and is provided³*/
#*³  to you solely for the purpose of assisting you in the development of    ³*/
#*³  your applications.  The code is provided "AS IS", without warranty of   ³*/
#*³  any kind.  IBM shall not be liable for any damages arising out of your  ³*/
#*³  use of the sample code, even if they have been advised of the           ³*/
#*³  possibility of such damages.                                            ³*/
#*³                                                                          ³*/
#*³ For details on what this program does etc., please see the PMSPY.C file. ³*/
#*³                                                                          ³*/
#*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#
#*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
#*³ PMSPY.MAK                                                                ³*/
#*³                                                                          ³*/
#*³ main makefile                                                            ³*/
#*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

#------------------------------------------------------------------------------
# Symbolic names for PRODuct pieces
#------------------------------------------------------------------------------
PROD    = PMSPY

PRODVER = 1.10

# definitions for PMSPY.EXE (code and Non-NLS resources)

PRODX    = $(PROD).EXE
PRODXH   = $(PROD).H
PRODXHMM = $(PROD)MM.H
PRODXHMP = $(PROD)MP.H
PRODXHINI= $(PROD)INI.H

PRODXC   = PMSPY.C \
PMSPYABT.C \
PMSPYFIL.C \
PMSPYFON.C \
PMSPYGRP.C \
PMSPYINI.C \
PMSPYIPF.C \
PMSPYMD.C \
PMSPYMP.C \
PMSPYMSG.C \
PMSPYNOT.C \
PMSPYOW.C \
PMSPYPRO.C \
PMSPYUTL.C \
PMSPYWIN.C

PRODXOBJ = $(PRODXC:.C=.OBJ)

PRODXDEF = $(PROD).DEF
PRODXMAP = $(PROD).MAP
PRODXRC  = $(PROD).RC
PRODXRES = $(PROD).RES
PRODXPRO = $(PROD)DEF.PRO

# definitions for PMSPYHK.DLL ("hook" code)

PRODH    = $(PROD)HK.DLL
PRODHH   = $(PROD)HK.H
PRODHC   = $(PROD)HK.C
PRODHOBJ = $(PROD)HK.OBJ
PRODHDEF = $(PROD)HK.DEF
PRODHLIB = $(PROD)HK.LIB

# definitions for PMSPY.DLL (NLS resources)

PRODN    = $(PROD).DLL
PRODNC   = $(PROD)NLS.C
PRODNOBJ = $(PROD)NLS.OBJ
PRODNDEF = $(PROD)NLS.DEF
PRODNRC  = $(PROD)NLS.RC
PRODNRES = $(PROD)NLS.RES
PRODNH   = $(PROD)RC.H

# definitions for PMSPY.HLP (IPF Help panels)

PRODI    = $(PROD).HLP
PRODIH   = $(PROD)IPF.H
PRODIN   = $(PRODIH) $(PRODNH)
PRODIIPF = $(PROD).IPF

# definitions for PMSPY source modules

PRODSH   = $(PRODXH) $(PRODHH) $(PRODNH)
PRODSHI  = $(PRODSH) $(PRODIH)

#------------------------------------------------------------------------------
# MMPM/2 support
#------------------------------------------------------------------------------
!ifdef wantMMPM
defMMPM  = /Ss+ /DMMPM
!else
defMMPM  =
!endif

#------------------------------------------------------------------------------
# ICC parameters
#------------------------------------------------------------------------------
cWARN  = /W3 /Wall+uni-ext-ppt-ppc-
cOPTS  = /c /Q+ /V"$(PROD) $(PRODVER)"
cEXE   = /Ge+ /Re
cDLL   = /Ge- /Rn

# decide about listings..

!ifdef wantLST
cLST   = /Lf /Lt "PMSPY" /Lu "Version $(PRODVER)"
!endif

# decide about Debug Info...

!ifdef wantDBG
cCV    = /O- /Ti+ /Gh+
eXTRA  = \IBMCPP\LIB\DDE4XTRA.OBJ
!else
cCV    = /O+
eXTRA  =
!endif

!ifdef TEE
teeline=| tee $*.err
!endif

#------------------------------------------------------------------------------
# LINK parameters
#------------------------------------------------------------------------------
lMain           = /MAP /NOI /EXEPACK /ALIGN:16

!ifdef wantDBG
lMain           = $(lMain) /CO /LI /DE
!endif

#------------------------------------------------------------------------------
# *** THIS is THE inference rule to generate our code.... ***
#------------------------------------------------------------------------------
.C.OBJ:
  @-erase $*.ERR $*.OBJ 2>NUL
  icc $(cOPTS) $(cCV) $(cWARN) $(cEXE) $(cLST) $(defMMPM) $*.C $(teeline)

#------------------------------------------------------------------------------
# *** THIS is THE major dependency rule...it forces anything to happen
#------------------------------------------------------------------------------
$(PROD):     $(PRODX) \
             $(PRODH) \
             $(PRODN) \
             $(PRODI)

#------------------------------------------------------------------------------
# Generate PMSPY.EXE (Code and Non-NLS resources)
#------------------------------------------------------------------------------
$(PRODX):     $(PRODXOBJ) \
              $(PRODXDEF) \
              $(PRODXRES) \
              $(PRODHLIB)
  link386 @<<
$(PRODXOBJ)+
$(lMain) $(eXTRA)
$(PRODX)
$(PRODXMAP)
$(PRODHLIB)
$(PRODXDEF)
<<
  rc $(PRODXRES) $(PRODX)
 -@echo ~~*~~ $(PRODX) re-built

$(PRODXRES):  $(PRODXRC)          \
              $(PRODNH)           \
              $(PRODIH)           \
              $(PROD).ICO         \
              $(PROD).PTR         \
              $(PRODXPRO)
  rc -r $(PRODXRC)

$(PRODXPRO)  : PMSPYDEF.EXE
  PMSPYDEF

PMSPYDEF.EXE : PMSPYDEF.OBJ
  link386 $(lMain) @<<
PMSPYDEF.OBJ
PMSPYDEF.EXE
NUL


<<

#-------------------------------------------------------------------------------
# Generate PMSPYHK.DLL ("Hook" code)
#------------------------------------------------------------------------------
$(PRODH) : $(PRODHOBJ) $(PRODHDEF)
  link386 $(lMain) /NOE @<<
$(PRODHOBJ)
$(PRODH)
NUL

$(PRODHDEF)
<<
 -@echo ~~*~~ $(PRODH) re-built

#-------------------------------------------------------------------------------
# Generate PMSPYHK.LIB (Import LIB for "Hook" code)
#------------------------------------------------------------------------------
$(PRODHLIB) : $(PRODH)
  implib $(PRODHLIB) $(PRODHDEF)
 -@echo ~~*~~ $(PRODHLIB) re-built

#-------------------------------------------------------------------------------
# Generate PMSPY.DLL (NLS resources)
#------------------------------------------------------------------------------
$(PRODN):     $(PRODNOBJ)         \
              $(PRODNDEF)         \
              $(PRODNRES)
  link386 $(lMain) @<<
$(PRODNOBJ)
$(PRODN)
NUL

$(PRODNDEF)
<<
  rc $(PRODNRES) $(PRODN)
 -@echo ~~*~~ $(PRODN) re-built

$(PRODNRES):  $(PRODNRC)          \
              $(PRODNH)           \
              $(PROD).ICO
  rc -r -dOS2TOOLS $(PRODNRC)

#-------------------------------------------------------------------------------
# Generate PMSPY.HLP (IPF Help panels)
#------------------------------------------------------------------------------
$(PRODI):     $(PRODIIPF) $(PRODIN)
  icc /Pe+ $(PRODIIPF)
  ipfc $(PRODI:.HLP=.I)
# @-del $(PRODI:.HLP=.I) 2>NUL
 -@echo ~~*~~ $(PRODI) re-built

#------------------------------------------------------------------------------
# Individual components
#------------------------------------------------------------------------------
PMSPY.OBJ:    PMSPY.C    $(PRODSH)

PMSPYABT.OBJ: PMSPYABT.C $(PRODSH)

PMSPYDEF.OBJ: PMSPYDEF.C $(PRODXHMM)
  @-erase $*.ERR $*.OBJ 2>NUL
  icc $(cOPTS) $(cWARN) $(cEXE) $(cCV) $(defMMPM) $*.C $(teeline)

PMSPYFIL.OBJ: PMSPYFIL.C $(PRODSH)

PMSPYFON.OBJ: PMSPYFON.C $(PRODSH)

PMSPYGRP.OBJ: PMSPYGRP.C $(PRODSH)

PMSPYHK.OBJ:  PMSPYHK.C  $(PRODHH)
  @-erase $*.ERR $*.OBJ 2>NUL
  icc $(cOPTS) $(cWARN) $(cDLL) $(cCV) $(cLST) $*.c $(teeline)

PMSPYIPF.OBJ: PMSPYIPF.C $(PRODSH)

PMSPYINI.OBJ: PMSPYINI.C $(PRODSH) $(PRODXHINI)

PMSPYMD.OBJ:  PMSPYMD.C $(PRODSH)  $(PRODXHMP)

PMSPYMP.OBJ:  PMSPYMP.C $(PRODSH)  $(PRODXHMP)

PMSPYMSG.OBJ: PMSPYMSG.C $(PRODSH)

PMSPYNLS.OBJ: PMSPYNLS.C

PMSPYNOT.OBJ: PMSPYNOT.C $(PRODSH)

PMSPYOW.OBJ:  PMSPYOW.C  $(PRODSH)

PMSPYPRO.OBJ: PMSPYPRO.C $(PRODSH) $(PRODXHINI)

PMSPYUTL.OBJ: PMSPYUTL.C $(PRODSH) $(PRODXHMM)

PMSPYWIN.OBJ: PMSPYWIN.C $(PRODSH) $(PRODIH) $(PRODXHINI)
