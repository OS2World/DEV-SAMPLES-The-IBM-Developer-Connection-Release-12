TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=WARP.CONNECT.LAPS.BOOT.PARM_MAINT.WKSTNAME.REF.1.0.WR08000
   Description="Warp Connect Laps MAINT install"

End

Section Install
Begin
     Program = SA:\IMG\CONNECT\CID\IMG\LAPS\MPTS.EXE
     Parms = /E:MAINT /R:$(ResponseFile) /T:$(TargetDir)  /TU:C:\  /S:$(SourceDir) /L1:$(LogFile1)
     SourceDir = SA:IMG\CONNECT\CID\IMG\LAPS
     ResponseFile = SA:\RSP\CONNECT\MPTS\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\CONNECT\MPTS\$(WorkStatName).LOG
End 
 
