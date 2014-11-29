
# makefile for 32-bit Memory Management in OS/2 sources

all: tester.dll test.exe prag.exe heap.exe


#--------------------------------------------------------
prag.exe: prag.c prag.def
  icc /c /ss /q prag.c
  ilink prag.obj prag.def




#--------------------------------------------------------
heap.exe: heap.c
  icc /ss /c /q heap.c
  ilink /pmtype:vio heap.obj



#--------------------------------------------------------
tester.obj: tester.c
  icc /ss /c /q /Ge- tester.c

tester.dll: tester.obj tester.def
  ilink /noe tester.obj tester.def /OUT:tester.dll




#--------------------------------------------------------
test.exe: test.c
  icc /ss /c /q test.c
  ilink /pmtype:vio test.obj



#--------------------------------------------------------
FIGS=addspace subset

readme.out: $(FIGS) readme
  attrib -r readme.out
  em readme >readme.out
  attrib +r readme.out

#--------------------------------------------------------
SRC = readme readme.out heap.c test.c tester.c prag.c makefile prag.def tester.def $(FIGS) em.cmd
zip:
  if exist mem.zip erase mem.zip
  zip mem $(SRC)
