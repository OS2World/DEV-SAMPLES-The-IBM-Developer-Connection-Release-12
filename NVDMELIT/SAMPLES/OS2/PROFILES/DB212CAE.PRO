
TargetDir=C:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=DB22.CLIENT.APPL.ENABLER.REF.1.2
     Description="Install Procedure for Database OS/2 1.2 configured Client Application Enabler"
End

Section Install
Begin
      Program= SA:\IMG\DB22\12\GA\CAE2\DB2CID.EXE
      Parms= /S:$(SourceDir) /R:$(ResponseFile) /L2:$(logfile2) /L3:$(Logfile3) /L1:$(LogFile1) /L4:$(LogFile4)
      SourceDir= SA:\IMG\DB22\12\GA\CAE2
      ResponseFile= SB:\RSP\DB22\12\GA\CAE2\CAE2.RSP
      Logfile1= SB:\LOG\DB22\12\GA\CAE2\$(WorkStatName).L1
      Logfile2= SB:\LOG\DB22\12\GA\CAE2\$(WorkStatName).L2
      Logfile3= SB:\LOG\DB22\12\GA\CAE2\$(WorkStatName).L3
      Logfile4= SB:\LOG\DB22\12\GA\CAE2\$(WorkStatName).L4
 End
