#=========================================#
# Make RSRCJPN.DLL for Japanese Resource  #
#                                         #
#  usage :  NMAKE /f RSRCJPN.MAK          #
#                                         #
#        (5/12/92)                        #
#=========================================#

#
#  You must rename APSYM932.APS to APSYMBOL.APS befor compiling ORDRJPN.IPF
#

rsrcjpn.dll : rsrcjpn.res rsrcjpn.obj rsrcjpn.def
  link386 rsrcjpn, rsrcjpn.dll /ALIGN:4 /BASE:0x12000000, NUL,, rsrcjpn
  rc -cp 942 rsrcjpn.res rsrcjpn.dll

rsrcjpn.obj : rsrcjpn.c ordent.h
  icc /Ss /Sn /C /W3 rsrcjpn.c

rsrcjpn.res : rsrcjpn.rc ordent.ico ordent.h bmapj.bmp
  rc -r -cp 942 -p rsrcjpn.rc

ordrjpn.hlp : ordrjpn.ipf
  ipfc /COUNTRY=081 /CODEPAGE=942 /LANGUAGE=JPN ordrjpn.ipf
