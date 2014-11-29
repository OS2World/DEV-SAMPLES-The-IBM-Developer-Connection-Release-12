
TargetDir=C:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=DDCS2.SERVICEPAK.INSTALL.REF.2.2.WR07036
     Description="Install Procedure for DDCS/2 2.2 SP WR07036"
End

Section Install
Begin
      Program =  SA:\IMG\DDCS2\22\WR07036\DDCSCID.EXE
      Parms= /S:$(SourceDir)  /R:$(ResponseFile)  /L2:$(logfile2)  /L3:$(Logfile3) /L1:$(lOGFILE1) /L4:$(LOGFILE4)
      SourceDir= SA:\IMG\DDCS2\22\WR07036
      ResponseFile =  SB:\RSP\DDCS2\22\WR07036\DDCSCID.RSP
      Logfile1=SB:\LOG\DDCS2\22\WR07036\$(WorkStatName).L1
      Logfile2=SB:\LOG\DDCS2\22\WR07036\$(WorkStatName).L2
      Logfile3=SB:\LOG\DDCS2\22\WR07036\$(WorkStatName).L3
      Logfile4=SB:\LOG\DDCS2\22\WR07036\$(WorkStatName).L4
 End
