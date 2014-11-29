#-------------------
# Wideline make file
#-------------------

wideln.exe : wideln.obj wideln.def wideln.res wideln.lnk
     link386 @wideln.lnk
     rc wideln.res wideln.exe
     mapsym wideln

wideln.obj : wideln.c wideln.h
     icc /O /Kb /Mp /C /Sm /Ss wideln.c

wideln.res : wideln.rc wideln.h
     rc -r wideln.rc

