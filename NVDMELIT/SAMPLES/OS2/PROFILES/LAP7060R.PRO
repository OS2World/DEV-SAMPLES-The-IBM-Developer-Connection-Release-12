TargetDir = C:\SERVICE

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LAPS.RESTORE.IND.WKSTNAME.REF.1.0.WR07060
   Description="LAPS FSERVICE install Independent of Maintenance Mode"

End

Section Install
Begin
     Program = SA:\IMG\NTS2\LAPS\WR07060\FSERVICE.EXE
     Parms = /R:$(ResponseFile)  /S:$(SourceDir) /L1:$(LogFile1) /L:$(LogFile2) 
     SourceDir = SA:\IMG\NTS2\LAPS\WR07060
     ResponseFile = SB:\RSP\NTS2\LAPS\WR07060\RESTORE.RSP
     LogFile1 = SB:\LOG\NTS2\LAPS\WR07060\$(WorkStatName).L1
     LogFile2 = SB:\LOG\NTS2\LAPS\WR07060\$(WorkStatName).L2
End
