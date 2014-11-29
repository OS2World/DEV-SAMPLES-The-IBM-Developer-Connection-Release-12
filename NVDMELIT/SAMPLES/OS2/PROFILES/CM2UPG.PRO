TargetDir = C:\
Section Catalog
Begin
  ObjectType = Software
  GlobalName = CM2.CM2UPG.MIGRATION.INSTALL.REF.1.11
  Description= "Migration Install for CM/2"
End

Section Install
Begin
  Program = SA:\IMG\CM2\111\GA\ANXCM2.CMD
  Parms   = /BD:C /S:$(SourceDir) /R:$(ResponseFile) /L1:$(LogFile1) /L2:$(LogFile2)
  SourceDir = SA:\IMG\CM2\111\GA
  ResponseFile = SB:\RSP\CM2\111\GA\CM2UPG.RSP
  LogFile1 = SB:\LOG\CM2\111\GA\$(WorkstatName).l1
  LogFile2 = SB:\LOG\CM2\111\GA\$(WorkstatName).l2
End
