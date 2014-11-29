
TargetDir=C:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=DB22.MIGR.FROM.ES.ON.CCSERVER.REF.1.20.ANXDB2
     Description="Install Procedure for Database/2 OS/2 on an NVDM/2 CC Server"
End

Section Install
Begin
      Program =  SA:\IMG\DB22\12\GA\DB2CS\ANXDB2.CMD
      Parms= /S:$(SourceDir)  /R:$(ResponseFile)  /L2:$(logfile2)  /L3:$(Logfile3) /L5:$(Logfile5) /BD:C /DBNAME:IBMNVDM2
      SourceDir= SA:\IMG\DB22\12\GA\DB2CS
      ResponseFile= SA:\RSP\DB22\12\GA\DB2CS\ANXDB2.RSP
      Logfile2=SB:\LOG\DB22\12\GA\DB2CS\$(WorkStatName).L2
      Logfile3=SB:\LOG\DB22\12\GA\DB2CS\$(WorkStatName).L3
      Logfile5=SB:\LOG\DB22\12\GA\DB2CS\$(WorkStatName).L5
End
