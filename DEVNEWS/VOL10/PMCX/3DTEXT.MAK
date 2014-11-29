
# MAKE file for 3DText.MAK     Release 5

# Revised:  1995-12-11

# Copyright ¸ 1987-1995  Prominare Inc.

# Macro definitions

C_SW=-G5e- -O+ -Rn -C 


3D.Dll: 3DText.Obj
 ILink /FREE /EXEPACK /ALIGN:4 /BASE:0x14000000 /OUT:3DText.Dll 3DText.Obj 3d.Lib 3DText.Def
 rc -x 3DText.Res 3DText.Dll

3DText.Obj: 3DText.C
 Icc $(C_SW) -Fo$*.Obj 3DText.C

