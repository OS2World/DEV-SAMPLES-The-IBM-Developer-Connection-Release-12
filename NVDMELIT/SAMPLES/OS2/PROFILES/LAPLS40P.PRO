TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LAPS.PROD.PARM_PROD.WKSTNAME.REF.4.0.LS.WR08000
   Description="LAPS PROD install"

End

Section Install
Begin
     Program = SA:\IMG\LS\40\LAPS\MPTS.EXE
     Parms = /E:PROD /R:$(ResponseFile) /TU:C:\  /S:$(SourceDir) /L1:$(LogFile1)
     SourceDir = SA:\IMG\LS\40\LAPS
     ResponseFile = SB:\RSP\LS\40\MPTS\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\LS\40\MPTS\$(WorkStatName).LOG
End
