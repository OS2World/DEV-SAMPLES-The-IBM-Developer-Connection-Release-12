# NMAKE-compatible MAKE file for the REXX sample program LS30UT.DLL.
# This uses the IBM C Set/2 compiler and LINK386.

LS30UT.DLL:     LS30UT.OBJ  LS30UT.DEF
         LINK386 LS30UT.OBJ,LS30UT.DLL,,REXX+NETAPI+NETOEM,LS30UT.DEF;

LS30UT.OBJ:     LS30UT.C
         icc /C /Ge- /Gt /O+ LS30UT.C


