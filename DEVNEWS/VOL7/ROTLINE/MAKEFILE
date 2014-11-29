#-----------------
# ROTLINE make file
#-----------------

rotline.exe : rotline.obj rotline.def rotline.res rotline.lnk
     link386 @rotline.lnk
     rc rotline.res rotline.exe
     mapsym rotline

rotline.obj : rotline.c rotline.h
     icc /O /Kb /Mp /C /Sm /Ss rotline.c

rotline.res : rotline.rc rotline.h
     rc -r rotline.rc
