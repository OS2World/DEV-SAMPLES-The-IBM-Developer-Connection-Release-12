cc = icc
link = link386
rc = rc
ipfc = ipfc
cflags = /C /Q /W3 /Kb+
#If your tutorial will only be used on Warp, use the -EXEPACK:2 flag
#for better packing
lflags = /PM:PM /EXEPACK:1
rc1flags = -r
#If your tutorial will only be used on Warp, use the -x2 flag for better packing
rc2flags = -x

.c.obj:
   $(cc) $(cflags) $*.c

.rc.res:
   $(rc) $(rc1flags) $*.rc

.ipf.hlp:
   $(ipfc) $*.ipf

all: tutorial.exe tutorial.hlp

tutorial.exe: tutorial.obj tutuser.obj tutorial.res
   $(link) tutorial.obj tutuser.obj $(lflags),,,,tutorial.def
   $(rc) $(rc2flags) tutorial.res tutorial.exe

tutorial.res: os2tutor.ico tutorial.rc tutrc.h backdne.bmp backupd.bmp backupe.bmp fwddne.bmp fwdupd.bmp fwdupe.bmp sectdns.bmp sectdnu.bmp sectups.bmp sectupu.bmp gendnu.bmp genupu.bmp

tutuser.obj: tutuser.c

tutorial.hlp: tutorial.ipf
   $(ipfc) tutorial.ipf
