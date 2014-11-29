
# MAKE file for 3DFrame.MAK     Release 5

# Revised:  1995-12-11

# Copyright ¸ 1987-1995  Prominare Inc.

# Macro definitions

C_SW=-G5e- -O+ -Rn -C 


3D.Dll: 3dFrame.Obj
 ILink /FREE /EXEPACK /ALIGN:4 /BASE:0x14000000 /OUT:3DFrame.Dll 3dFrame.Obj 3d.Lib 3dFrame.Def
 rc -x 3DFrame.Res 3DFrame.Dll

3dFrame.Obj: 3dFrame.C
 Icc $(C_SW) -Fo$*.Obj 3dFrame.C

