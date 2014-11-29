#-----------------
# TEST1 make file
#-----------------

star.exe : star.obj star.def star.lnk
     link386 @star.lnk
     mapsym star

star.obj : star.c 
     icc /O /Kb /Mp /C /Sm /Ss star.c

