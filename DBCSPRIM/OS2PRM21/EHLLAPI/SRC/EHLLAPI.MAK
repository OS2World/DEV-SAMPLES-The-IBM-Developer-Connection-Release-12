ehllapi.exe : ehllapi.obj dbcs.obj
   link ehllapi+dbcs /A:16,,, OS2.LIB+LLIBCE.LIB, ehllapi.def

ehllapi.obj : ehllapi.c
   cl -c -G2sw -W3 -Zep -AL ehllapi.c
