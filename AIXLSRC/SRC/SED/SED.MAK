#CDFLAGS = /c /Gd- /Sm /Re /Ss /Ms /O- /Ti /Gm- /Q+ /I\unixlike\rel31\src\include
#CFLAGS  = /c /Gd- /Sm /Re /Ss /Ms /O+    /Gm- /Q+ /I\unixlike\rel31\src\include
#COFLAGS = /c /Gd- /Sm /Re /Ss /Ms /O- /Ti /Gm- /Q+ /I\unixlike\rel31\src\include
#CMFLAGS = /c /Gd- /Sm /Re /Ss /Ms /O+    /Gm+ /Q+ /i\unixlike\rel31\src\include
COFLAGS = /c /Gd- /Sm /Re /Ss /Ms /O+    /Gm- /Q+ /I\unixlike\rel31\src\include
#LDFLAGS = /DEBUG /ALIGN:32 /EXEPACK /M /NOI
#LFLAGS  =        /ALIGN:32 /EXEPACK /M /NOI
LFLAGS = /DEBUG /ALIGN:32 /EXEPACK /M /NOI

B = .
F = \unixlike\rel31\util\fmf
G = \unixlike\rel31\util\getopt
GU= \unixlike\rel31\util\greputil
M = \unixlike\rel31\util\myerror
MF = \unixlike\rel31\mapfiles
MP= \unixlike\rel31\util\makepath
P = \unixlike\rel31\util\getopt
S = \unixlike\rel31\src
SL =\unixlike\rel31\util\sleep
#Z = \unixlike\rel31\src\more

SEDOBJS =  $S\sed\sed+$S\sed\sedparse+$S\sed\sedapply+$S\sed\sedtable+$F\fmf+$M\myerror+$(GU)\greputil

$B\sed.exe      :  $F\fmf.obj $M\myerror.obj $(GU)\greputil.obj $S\sed\sed.obj \
            $S\sed\sed.def $S\sed\sedparse.obj $S\sed\sedapply.obj     \
            $S\sed\sedtable.obj sed.lnk
#         \toolkt20\os2bin\link386 $(LFLAGS) $(SEDOBJS),$B\sed.exe,NUL,,$S\sed\sed.def;
         \toolkt20\os2bin\link386 @$S\sed\sed.lnk

$S\sed\sed.obj           :  $S\sed\sed.c $S\sed\sed.h $F\fmf.h
         icc $(COFLAGS) /I$F /I$S\sed /Fo$S\sed\sed.obj $S\sed\sed.c

$S\sed\sedparse.obj      :  $S\sed\sedparse.c $S\sed\sed.h
         icc $(COFLAGS) /I$S\sed /Fo$S\sed\sedparse.obj $S\sed\sedparse.c

$S\sed\sedapply.obj      :  $S\sed\sedapply.c $S\sed\sed.h
         icc $(COFLAGS) /I$S\sed /I$S\grep /Fo$S\sed\sedapply.obj $S\sed\sedapply.c

$S\sed\sedtable.obj      :  $S\sed\sedtable.c $S\sed\sed.h
         icc $(COFLAGS) /I$S\sed /Fo$S\sed\sedtable.obj $S\sed\sedtable.c

$F\fmf.obj               :  $F\fmf.c $F\fmf.h
         icc $(COFLAGS) /Fo$F\fmf.obj $F\fmf.c

$(GU)\greputil.obj       : $(GU)\greputil.c $(GU)\greputil.h
         icc $(COFLAGS) /Fo$(GU)\greputil.obj -I$(GU) $(GU)\greputil.c

$M\myerror.obj           : $M\myerror.c
         icc $(COFLAGS) /Fo$M\myerror.obj $M\myerror.c

