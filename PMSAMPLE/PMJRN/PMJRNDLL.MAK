pmjrndll.lib: pmjrn.h pmjrndll.c pmjrndll.obj \
              pmjrndll.def pmjrndll.l pmjrndll.dll

pmjrndll.obj: pmjrndll.c pmjrn.h
  icc  /c /Ge- /O+  pmjrndll.c

pmjrndll.dll: pmjrndll.obj pmjrndll.l pmjrndll.def
  link386 @pmjrndll.l

pmjrndll.lib: pmjrndll.dll
  implib pmjrndll.lib pmjrndll.def
# copy pmjrndll.dll \os2tools\dll
