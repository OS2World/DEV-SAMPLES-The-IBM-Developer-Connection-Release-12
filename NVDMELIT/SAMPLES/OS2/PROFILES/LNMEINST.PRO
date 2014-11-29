TargetDir=D:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=LNME.INSTALL.WKSTNAME.REF.1.0
      Description="Install Procedure for LAN Network Manager Entry 1.0"
End

Section Install
Begin
      Program =  SA:\IMG\LNME\10\GA\DFIINCID.EXE
      Parms= /S:$(SourceDir)  /T:$(TargetDir)  /R:$(responsefile)  /L1:$(logfile1) /L2:$(logfile2) 
      SourceDir= SA:\IMG\LNME\10\GA
      ResponseFile =  SB:\RSP\LNME\10\GA\$(workStatname).RSP
      Logfile1=SB:\LOG\LNME\10\GA\$(WorkStatName).L1
      Logfile2=SB:\LOG\LNME\10\GA\$(WorkStatName).L2
 End


