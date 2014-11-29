#
#    Make SERBOOT  Device Driver
#
all:            servboot.sys

servboot.obj:   servboot.asm servboot.mak
    masm        -MU -N -P -L servboot.asm ;

servboot.sys:     servboot.obj servboot.def
                link   /map /a:16  @<<servboot.lrf
SERVBOOT.OBJ
SERVBOOT.SYS   /NOD /NOI ,,
DOSCALLS.LIB
SERVBOOT.DEF ;
<<

