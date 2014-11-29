!IFDEF co
!UNDEF CO
CO =
!ENDIF

!IFDEF CO
!UNDEF CO
CO = /co
CODV = -Od -Zi
!ELSE
!UNDEF CODV
!ENDIF
#
#    Make watchdog Program
#
all:            watchdog.exe

watchdog.obj:   watchdog.c watchdog.mak
    cl          -c  -Lp -W3 -DLINT_ARGS -Fs $(CODV) -Zp -Ze -G2 -Gs -AS watchdog.c

watchdog.exe:     watchdog.obj watchdog.def
                link   /map /a:16 $(CO) @<<watchdog.lrf
WATCHDOG.OBJ    /NOD ,,,
SLIBCEP.LIB + DOSCALLS.LIB
WATCHDOG.DEF ;
<<

