
TargetDir=C:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=DB22.SERVER.ANXDB2S.WKSTNAME.REF.1.2.WR07035
     Description="Install Procedure for Migrating Database OS/2 1.2 to WR07035 on a NVDM2 CC Server"
End

Section Install
Begin
      Program= SA:\IMG\DB22\12\WR07035\ANXDB2S.CMD
      Parms= /S:$(SourceDir)  /R:$(ResponseFile)  /L2:$(logfile2)  /L3:$(Logfile3) /BD:C /DBNAME:IBMNVDM2 /L5:$(LogFile5) 
      SourceDir= SA:\IMG\DB22\12\WR07035
      ResponseFile= SB:\RSP\DB22\12\WR07035\DB2CS\$(WorkStatName).SRV
      LogFile2= SB:\LOG\DB22\12\WR07035\DB2CS\$(WorkStatName).L2
      LogFile3= SB:\LOG\DB22\12\WR07035\DB2CS\$(WorkStatName).L3
      LogFile5= SB:\LOG\DB22\12\WR07035\DB2CS\$(WorkStatName).L5
 End
