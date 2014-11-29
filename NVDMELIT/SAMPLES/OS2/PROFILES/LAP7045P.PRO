TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LAPS.PROD.PARM_PROD.WKSTNAME.REF.1.0.WR07045
   Description="LAPS PROD install"

End

Section Install
Begin
     Program = SA:\IMG\NTS2\LAPS\WR07045\LAPS.EXE
     Parms = /E:PROD /R:$(ResponseFile) /TU:C:\  /S:$(SourceDir) /L1:$(LogFile1)
     SourceDir = SA:\IMG\NTS2\LAPS\WR07045
     ResponseFile = SB:\RSP\NTS2\LAPS\WR07045\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\NTS2\LAPS\WR07045\$(WorkStatName).LOG
End
