# ======================================================================== #
#                                                                          #
#   Makefile for TOOLBAR                                                   #
#                                                                          #
# ======================================================================== #


toolbar.exe : toolbar.obj  toolbar.def toolbar.res
     link386  /NOE /NOI /DE toolbar,,,, toolbar.def
     rc toolbar.res  toolbar.exe

toolbar.obj : toolbar.c toolbar.h
     icc /C /Ti /Ss toolbar.c

toolbar.res : toolbar.rc toolbar.h
     rc -r toolbar

