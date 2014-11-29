
TargetDir=C:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=DB22.CLIENT.WITH.DB.WKSTNAME.REF.1.2.WR07035
     Description="Install Procedure for Database OS/2 1.2 WR07035 configured Client w/databases"
End

Section Install
Begin
      Program= SA:\IMG\DB22\12\WR07035\DB2CID.EXE
      Parms= /S:$(SourceDir) /R:$(ResponseFile) /L2:$(logfile2) /L3:$(Logfile3) /L1:$(LogFile1) /L4:$(LogFile4)
      SourceDir= SA:\IMG\DB22\12\WR07035
      ResponseFile= SB:\RSP\DB22\12\WR07035\DB2CS\$(WorkStatName).CDB
      Logfile1= SB:\LOG\DB22\12\WR07035\DB2CS\$(WorkStatName).L1
      Logfile2= SB:\LOG\DB22\12\WR07035\DB2CS\$(WorkStatName).L2
      Logfile3= SB:\LOG\DB22\12\WR07035\DB2CS\$(WorkStatName).L3
      Logfile4= SB:\LOG\DB22\12\WR07035\DB2CS\$(WorkStatName).L4
 End
