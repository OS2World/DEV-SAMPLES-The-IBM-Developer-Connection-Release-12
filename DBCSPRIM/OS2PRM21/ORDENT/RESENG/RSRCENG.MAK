#=========================================#
# Make RSRCENG.DLL for English Resource   #
#                                         #
#  usage :  NMAKE /f RSRCENG.MAK          #
#                                         #
#        (5/12/92)                        #
#=========================================#

#
#  You must use APSYMBOL.APS for SBCS languages to compile ORDRENG.IPF
#


rsrceng.dll : rsrceng.res rsrceng.obj rsrceng.def
  link386 rsrceng, rsrceng.dll /ALIGN:4 /BASE:0x12000000, NUL,, rsrceng
  rc -cp 850 rsrceng.res rsrceng.dll

rsrceng.obj : rsrceng.c ordent.h
  icc /Ss /Sn /C /W3 rsrceng.c

rsrceng.res : rsrceng.rc ordent.ico ordent.h bmape.bmp
  rc -r -cp 850 -p rsrceng.rc

ordreng.hlp : ordreng.ipf
  ipfc /COUNTRY=001 /CODEPAGE=850 /LANGUAGE=ENU ordreng.ipf
