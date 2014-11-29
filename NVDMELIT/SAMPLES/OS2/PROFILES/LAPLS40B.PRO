TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LAPS.BOOT.PARM_MAINT.WKSTNAME.REF.4.0.LS.WR08000
   Description="LAPS MAINT install"

End

Section Install
Begin
     Program = SA:\IMG\LS\40\LAPS\MPTS.EXE
     Parms = /E:MAINT /R:$(ResponseFile) /T:$(TargetDir)  /TU:C:\  /S:$(SourceDir) /L1:$(LogFile1)
     SourceDir = SA:IMG\LS\40\LAPS
     ResponseFile = SB:\RSP\LS\40\MPTS\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\LS\40\MPTS\$(WorkStatName).LOG
End 
 
