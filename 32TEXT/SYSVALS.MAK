#
# Simple makefile
#
sysvals.exe:  sysvals.c os2char2.lib
    icc -q -i. sysvals.c sysvals.def os2char2.lib
