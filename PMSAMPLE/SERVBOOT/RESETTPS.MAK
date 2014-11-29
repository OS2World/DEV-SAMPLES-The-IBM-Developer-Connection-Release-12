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
#    Make resettps Program
#
all:            resettps.exe

resettps.obj:   resettps.c resettps.mak
    cl          -c  -Lp -W3 -DLINT_ARGS -Fs $(CODV) -Zp -Ze -G2 -Gs -AS resettps.c

resettps.exe:     resettps.obj resettps.def
                link   /map /a:16 $(CO) @<<resettps.lrf
RESETTPS.OBJ    /NOD ,,,
SLIBCEP.LIB + DOSCALLS.LIB
RESETTPS.DEF ;
<<

