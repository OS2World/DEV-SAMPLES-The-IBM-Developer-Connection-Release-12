
# MAKE file for 3D.MAK     Release 5

# Revised:  1995-12-11

# Copyright ¸ 1989-1995  Prominare Inc.

# Macro definitions

C_SW=-G5e- -O+ -Rn -C 


3D.Dll: 3D.Obj
 ILink /FREE /EXEPACK /ALIGN:4 /BASE:0x14000000 /OUT:3d.dll 3d.obj 3d.def

3D.Obj: 3D.C
 icc $(C_SW) -Fo$*.Obj 3D.C

