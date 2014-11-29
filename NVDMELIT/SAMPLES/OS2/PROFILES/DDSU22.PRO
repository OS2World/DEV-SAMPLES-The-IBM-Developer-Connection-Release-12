
TargetDir=C:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=DDCS2.SINGLEUSER.INSTALL.REF.2.2
     Description="Install Procedure for DDCS/2 2.2 single user"
End

Section Install
Begin
      Program =  SA:\IMG\DDCS2\22\GA\DB2DSU\DDCSCID.EXE
      Parms= /S:$(SourceDir) /R:$(ResponseFile) /L2:$(logfile2) /L3:$(Logfile3) /L1:$(lOGFILE1) /L4:$(LOGFILE4)
      SourceDir= SA:\IMG\DDCS2\22\GA\DB2DSU
      ResponseFile= SB:\RSP\DDCS2\22\GA\DB2DSU\DDCSSU.RSP
      Logfile1= SB:\LOG\DDCS2\22\GA\DB2DSU\$(WorkStatName).L1
      Logfile2= SB:\LOG\DDCS2\22\GA\DB2DSU\$(WorkStatName).L2
      Logfile3= SB:\LOG\DDCS2\22\GA\DB2DSU\$(WorkStatName).L3
      Logfile4= SB:\LOG\DDCS2\22\GA\DB2DSU\$(WorkStatName).L4
 End
