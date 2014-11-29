#-------------------
# CLIPPATH make file
#-------------------

clippth.exe : clippth.obj clippth.def clippth.lnk
     link386 @clippth.lnk
     mapsym clippth

clippth.obj : clippth.c
     icc /O /Kb /Mp /C /Sm /Ss clippth.c
