#========================================#
# XPG4 primer resource file make file.   #
#                                        #
#  Initial:  9/26/95          Y. Kimura  #
#========================================#

xprmres.dll : xprmres.res xprmres.obj xprmres.def xprmres.hlp
  ILINK /DLL /out:xprmres.dll xprmres.obj xprmres.def
  rc -cp 850 xprmres.res xprmres.dll

xprmres.obj : xprmres.c
  icc /Ge- /Ss /Q /G3 /C  xprmres.c

xprmres.res : xprmres.rc xprmres.dlg xprmres.h xprmhelp.rc xprmhelp.h
  rc -r -cp 850 -p xprmres.rc

xprmres.hlp : xprmres.ipf xprmhelp.rc xprmhelp.h
  ipfc -D:001 -C:850 -L:ENU xprmres.ipf
