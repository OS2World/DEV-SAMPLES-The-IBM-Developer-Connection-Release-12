TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=CM2.UPGRADE.REF.1.1.1
   Description="Upgrade CM/2 1.0 or ES Communication Manager to CM/2 1.11"
End

Section Install
Begin
     Program = SA:\IMG\CM2\111\GA\CMSETUP.EXE
     Parms =  /R:$(ResponseFile) /S:$(SourceDir)  /L1:$(LogFile1) /L2:$(LogFile2)
     ResponseFile = SB:\RSP\CM2\111\GA\UPGRADE.RSP
     SourceDir = SA:\IMG\CM2\111\GA
     LogFile1 = SB:\LOG\CM2\111\GA\$(WorkStatName).L1
     LogFile2 = SB:\LOG\CM2\111\GA\$(WorkStatName).L2
End
