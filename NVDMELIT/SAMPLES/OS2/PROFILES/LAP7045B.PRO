TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LAPS.BOOT.PARM_MAINT.WKSTNAME.REF.1.0.WR07045
   Description="LAPS MAINT install"

End

Section Install
Begin
     Program = SA:\IMG\NTS2\LAPS\WR07045\LAPS.EXE
     Parms = /E:MAINT /R:$(ResponseFile) /T:$(TargetDir)  /TU:C:\  /S:$(SourceDir) /L1:$(LogFile1)
     SourceDir = SA:\IMG\NTS2\LAPS\WR07045
     ResponseFile = SB:\RSP\NTS2\LAPS\WR07045\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\NTS2\LAPS\WR07045\$(WorkStatName).LOG
End 
 
