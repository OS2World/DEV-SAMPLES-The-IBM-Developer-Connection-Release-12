#======================================================================#
#    make file for ORDER.EXE                                           #
#======================================================================#
.SUFFIXES: .rc
.SUFFIXES: .def
.SUFFIXES: .lib

all: order.exe

exeobj= main.obj orddlg.obj nlssub.obj custdlg.obj custsub.obj print.obj

CC= ICC /Ti /Ss /Sn /G3 /C /Q /Sp1

.c.obj:
    $(CC)  $*.c > $*.err

order.exe: $(exeobj) ordent.def
    LINK386 /CO /PM:PM $(exeobj), order.exe ,,,ordent.def; >> order.err

