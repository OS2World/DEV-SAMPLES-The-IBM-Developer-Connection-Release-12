
TargetDir=C:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=DB22.SOFTWARE.DEVELOPERS.KIT.REF.1.2
     Description="Install Procedure for Database OS/2 1.2 configured Software Developers Kit"
End

Section Install
Begin
      Program= SA:\IMG\DB22\12\GA\CAE2SDK\DB2CID.EXE
      Parms= /S:$(SourceDir) /R:$(ResponseFile) /L2:$(Logfile2) /L3:$(Logfile3) /L1:$(LogFile1) /L4:$(LogFile4)
      SourceDir= SA:\IMG\DB22\12\GA\CAE2SDK
      ResponseFile= SB:\RSP\DB22\12\GA\CAE2SDK\CAE2SDK.RSP
      Logfile1= SB:\LOG\DB22\12\GA\CAE2SDK\$(WorkStatName).L1
      Logfile2= SB:\LOG\DB22\12\GA\CAE2SDK\$(WorkStatName).L2
      Logfile3= SB:\LOG\DB22\12\GA\CAE2SDK\$(WorkStatName).L3
      Logfile4= SB:\LOG\DB22\12\GA\CAE2SDK\$(WorkStatName).L4
 End
