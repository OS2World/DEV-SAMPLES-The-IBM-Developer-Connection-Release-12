TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LAPS.ODINDIS.PARM_PROD.WKSTNAME.WCONNECT.REF.1
   Description="LAPS ODINDIS PROD install"

End

Section Install
Begin
     Program = SA:\IMG\CONNECT\CID\IMG\LAPS\MPTS.EXE
     Parms = /E:PROD /R:$(ResponseFile) /TU:C:\  /S:$(SourceDir) /L1:$(LogFile1)
     SourceDir = SA:\IMG\CONNECT\CID\IMG\LAPS
     ResponseFile = SB:\RSP\CONNECT\MPTS\ADDNW\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\CONNECT\MPTS\ADDNW\$(WorkStatName).LOG
End
