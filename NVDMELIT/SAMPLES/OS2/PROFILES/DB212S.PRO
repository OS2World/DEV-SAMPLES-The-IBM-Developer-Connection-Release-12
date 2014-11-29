
TargetDir=C:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=DB22.SERVER.WKSTNAME.REF.1.2
     Description="Install Procedure for Database OS/2 1.2 configured as server"
End

Section Install
Begin
      Program= SA:\IMG\DB22\12\GA\DB2CS\DBCID.EXE
      Parms= /S:$(SourceDir)  /R:$(ResponseFile)  /L2:$(logfile2)  /L3:$(Logfile3) /L1:$(LogFile1) /L4:$(LogFile4) 
      SourceDir= SA:\IMG\DB22\12\GA\DB2CS
      ResponseFile= SB:\RSP\DB22\12\GA\DB2CS\$(WorkStatName).SRV
      Logfile1= SB:\LOG\DB22\12\GA\DB2CS\$(WorkStatName).L1
      Logfile2= SB:\LOG\DB22\12\GA\DB2CS\$(WorkStatName).L2
      Logfile3= SB:\LOG\DB22\12\GA\DB2CS\$(WorkStatName).L3
      Logfile4= SB:\LOG\DB22\12\GA\DB2CS\$(WorkStatName).L4
 End
