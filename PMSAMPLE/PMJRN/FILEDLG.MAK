Options = /c /Alfu /W2 /G2s /Ox /Zpe
filedlg.dll filedlg.lib: filedlg.c filedlg.h _filedlg.rc _filedlg.h _filedlg.c \
                         filedlg.obj _filedlg.obj

filedlg.obj: filedlg.c filedlg.h
  cl $(Options) filedlg.c
  lib filedlg.lib -+filedlg.obj;
  del filedlg.bak

_filedlg.obj: _filedlg.c _filedlg.h
  cl $(Options) _filedlg.c

_filedlg.res: _filedlg.rc _filedlg.h
  rc -r _filedlg.rc

filedlg.dll: _filedlg.res _filedlg.obj _filedlg.l _filedlg.def
  link @_filedlg.l
  rc _filedlg.res filedlg.dll
  copy filedlg.dll \os2tools\dll
