TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LAPS.ODINDIS.PARM_PROD.WKSTNAME.MPTS40.REF.1
   Description="LAPS ODINDIS PROD for MPTS 40 install"

End

Section Install
Begin
     Program = SA:\IMG\LS\40\GM\LAPS\MPTS.EXE
     Parms = /E:PROD /R:$(ResponseFile) /TU:C:\  /S:$(SourceDir) /L1:$(LogFile1)
     SourceDir = SA:\IMG\LS\40\GM\LAPS
     ResponseFile = SB:\RSP\LS\40\GM\MPTS\ADDNW\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\LS\40\GM\MPTS\ADDNW\$(WorkStatName).LOG
End
