
TargetDir=D:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=DDCS2.MULTIUSER.INSTALL.REF.2.2
     Description="Install Procedure for DDCS/2 2.2 Multi User"
End

Section Install
Begin
      Program =  SA:\IMG\DDCS2\22\GA\DB2DMU\DDCSCID.EXE
      Parms= /S:$(SourceDir) /R:$(ResponseFile) /L2:$(logfile2) /L3:$(Logfile3) /L1:$(lOGFILE1) /L4:$(LOGFILE4)
      SourceDir= SA:\IMG\DDCS2\22\GA\DB2DMU
      ResponseFile= SB:\RSP\DDCS2\22\GA\DB2DMU\DDCSMU.RSP
      Logfile1=SB:\LOG\DDCS2\22\GA\DB2DMU\$(WorkStatName).L1
      Logfile2=SB:\LOG\DDCS2\22\GA\DB2DMU\$(WorkStatName).L2
      Logfile3=SB:\LOG\DDCS2\22\GA\DB2DMU\$(WorkStatName).L3
      Logfile4=SB:\LOG\DDCS2\22\GA\DB2DMU\$(WorkStatName).L4
 End
