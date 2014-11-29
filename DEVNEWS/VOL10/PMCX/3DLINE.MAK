
# MAKE file for 3DLine.MAK     Release 5

# Revised:  1995-12-11

# Copyright ¸ 1987-1995  Prominare Inc.

# Macro definitions

C_SW=-G5e- -O+ -Rn -C 


3D.Dll: 3DLine.Obj
 ILink /FREE /EXEPACK /ALIGN:4 /BASE:0x14000000 /OUT:3DLine.Dll 3DLine.Obj 3d.Lib 3DLine.Def
 rc -x 3DLine.Res 3DLine.Dll

3DLine.Obj: 3DLine.C
 Icc $(C_SW) -Fo$*.Obj 3DLine.C

