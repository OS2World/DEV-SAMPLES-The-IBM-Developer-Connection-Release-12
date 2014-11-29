
all: t1term.exe kill2.exe termq.exe kill.exe pmapp.exe readme.out


CC = icc /q /c /ss /Ti+ /Gm+
LINK = ilink /map /debug /pmtype:vio /nologo


#--------------------------------------------------

killme.obj: killme.c
  $(CC) killme.c

killme.exe: killme.obj
  $(LINK) killme.obj



#--------------------------------------------------

t1term.obj: t1term.c
  $(CC) t1term.c

t1term.exe: t1term.obj
  $(LINK) t1term.obj

#--------------------------------------------------

kill2.obj: kill2.c
  $(CC) kill2.c

kill2.exe: kill2.obj
  $(LINK) kill2.obj


#--------------------------------------------------

kill3.obj: kill3.c
  $(CC) kill3.c

kill3.exe: kill3.obj
  $(LINK) kill3.obj

#--------------------------------------------------

termq.obj: termq.c
  $(CC) termq.c

termq.exe: termq.obj
  $(LINK) termq.obj

#--------------------------------------------------

kill.obj: kill.c
  $(CC) kill.c

kill.exe: kill.obj
  $(LINK) kill.obj

#--------------------------------------------------

pmapp.obj: pmapp.c
  $(CC) pmapp.c

pmapp.exe: pmapp.obj
  ilink /map /debug /pmtype:pm /nologo pmapp.obj

#--------------------------------------------------

readme.out: readme
  if exist readme.out attrib -r readme.out
  em readme >readme.out
  attrib +r readme.out




#--------------------------------------------------

zip:
  if exist source.zip erase source.zip
  zip source readme* unused outline *.c makefile m.cmd em.cmd pids.cmd


cd:
  nmake
  if exist cd.zip erase cd.zip
  zip cd readme.out *.c *.exe pids.cmd makefile





