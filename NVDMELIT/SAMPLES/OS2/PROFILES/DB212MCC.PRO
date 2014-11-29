
TargetDir=C:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=DB22.CLIENT_SERVER.MIGRATE.WITH_CURRENT_CFG.REF.1.2
     Description="Migration with current cfg Procedure for Database OS/2 1.2"
End

Section Install
Begin
      Program= SA:\IMG\DB22\12\GA\DB2CS\DBCID.EXE
      Parms= /S:$(SourceDir)  /R:$(ResponseFile)  /L2:$(logfile2)  /L3:$(Logfile3) /L1:$(LogFile1) /L4:$(LogFile4) /Z
      SourceDir= SA:\IMG\DB22\12\GA\DB2CS
      ResponseFile= SB:\RSP\DB22\12\GA\DB2CS\$(WorkStatName).SRV
      Logfile1= SB:\LOG\DB22\12\GA\DB2CS\$(WorkStatName).L1
      Logfile2= SB:\LOG\DB22\12\GA\DB2CS\$(WorkStatName).L2
      Logfile3= SB:\LOG\DB22\12\GA\DB2CS\$(WorkStatName).L3
      Logfile4= SB:\LOG\DB22\12\GA\DB2CS\$(WorkStatName).L4
 End
