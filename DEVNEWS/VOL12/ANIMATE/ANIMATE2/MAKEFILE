#---------------------------------
# animate2.exe make file
#
# Author:  Kelvin R. Lawrence
#
# 29th-September-1996
#---------------------------------

animate2.exe : animate2.obj animate2.def animate2.res animate2.lnk
#     link386 @animate2.lnk
      ilink /NOFREE /debug @animate2.lnk
# Use this line for debug version
#    link386 /debug @animate2.lnk
     rc animate2.res animate2.exe
     mapsym animate2

animate2.obj : animate2.c animate2.h
#     icc /O /Kb /Mp /C /Sm /Ss animate2.c
# Use this lne for debug version
    icc /O- /Oi- /Mp /C /Sm /Ss /Ti+ /G3 animate2.c

animate2.res : animate2.rc animate2.h 
     rc -r animate2.rc
