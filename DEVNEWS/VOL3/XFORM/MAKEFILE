#-----------------
# TEST1 make file
#-----------------

xform.exe : xform.obj xform.def xform.res xform.lnk
     link386 @xform.lnk
     rc xform.res xform.exe
     mapsym xform

xform.obj : xform.c xform.h
     icc /O /Kb /Mp /C /Sm /Ss xform.c

xform.res : xform.rc xform.h
     rc -r xform.rc

