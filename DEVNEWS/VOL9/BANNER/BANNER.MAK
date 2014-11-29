
# MAKE file for Banner.MAK

# Revised:  1995-10-03

# Copyright ¸ 1995  Prominare Inc.

# Macro definitions

CC=Icc
RC=Rc
LINK=ILink
C_SW=-G5e-s- -O- -Rn -W3 -C 
RC_SW=-r 
LINK_SW=/EXEPACK /NOFREE /ALIGN:4 /BASE:0x12000000 /IGNORECASE 

Banner.Dll: Banner.Obj Banner.Res
 $(LINK) $(LINK_SW) @Banner.Lnk
 $(RC) Banner.Res Banner.Dll

Banner.Obj: Banner.C
 $(CC) $(C_SW) -Fo$*.Obj Banner.C

Banner.Res: Banner.Rc
 $(RC) $(RC_SW) Banner.Rc $*.Res

