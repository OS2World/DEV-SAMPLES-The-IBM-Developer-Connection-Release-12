#-----------------
#  make file
#-----------------

font.exe : font.obj font.def font.res font.lnk
     link386 @font.lnk
     rc font.res font.exe
     mapsym font

font.obj : font.c font.h
     icc /O /Kb /Mp /C /Sm /Ss font.c

font.res : font.rc font.h
     rc -r font.rc
