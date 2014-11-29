# Compiler Switches:
#
# /c    - Supresses Linking
# /Alfu - Customised small memory model
# /W3   - Sets warning message level
# /Gs   - Removes stack probes
# /Gc   - Controls function calling sequences

pmprt.exe: pmprt.l pmprt.obj pmprt.def pmprt.res pmprtd.lib
        link @pmprt.l
        rc pmprt.res

pmprt.obj: pmprt.c pmprt.h
        cl /c /Alfu /Oilnt /W3 /G2s /Zpe /B1 C1L.EXE  pmprt.c

pmprt.res: pmprt.rc pmprt.h
        rc -r pmprt.rc

pmprtd.lib: pmprtd.dll
  implib pmprtd.lib pmprtd.def

pmprtd.obj: pmprtd.c
  cl /c /B1 d:\c\bin\c1l.exe /Alfu /W3 /G2s  /Zpe pmprtd.c

pmprtd.dll: pmprtd.obj pmprtd.l pmprtd.def
  link @pmprtd.l

