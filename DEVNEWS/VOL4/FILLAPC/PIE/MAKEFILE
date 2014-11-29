#-----------------
# Pie make file
#-----------------

pie.exe : pie.obj pie.def pie.lnk
     link386 @pie.lnk
     mapsym pie

pie.obj : pie.c
     icc /O /Kb /Mp /C /Sm /Ss pie.c

