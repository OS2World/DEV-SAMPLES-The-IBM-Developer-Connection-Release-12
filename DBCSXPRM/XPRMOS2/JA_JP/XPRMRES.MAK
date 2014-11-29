#============================================================#
# Make XPRMRES.MAK for Japanese Resource        11/15/95     #
#============================================================#

xprmres.dll : xprmres.res xprmres.obj xprmres.def xprmres.hlp
  ILINK /DLL /out:xprmres.dll xprmres.obj xprmres.def
  rc -cp 942 xprmres.res xprmres.dll

xprmres.obj : xprmres.c
  icc /Ge- /Ss /Q /G3 /C  xprmres.c

xprmres.res : xprmres.rc xprmhelp.rc xprmres.dlg xprmres.h xprmhelp.h
  rc -r -cp 942 -p xprmres.rc

xprmres.hlp : xprmres.ipf xprmhelp.h xprmhelp.rc
   ipfc -D:081 -C:942 -L:JPN xprmres.ipf
