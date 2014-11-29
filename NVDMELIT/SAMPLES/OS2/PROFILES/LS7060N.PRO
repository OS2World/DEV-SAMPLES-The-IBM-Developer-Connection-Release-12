TargetDir = C:\SERVICE

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LS.30.NO_BACKUP.FSERVICE.WKSTNAME.REF.3.0.IP07060
   Description="LS FSERVICE install Independent of Maintenance Mode"

End

Section Install
Begin
     Program = SA:\IMG\LS\30\IP07060\FSERVICE.EXE
     Parms = /R:$(ResponseFile)  /S:$(SourceDir) /L1:$(LogFile1) /L:$(LogFile2) 
     SourceDir = SA:\IMG\LS\30\IP07060
     ResponseFile = SB:\RSP\LS\30\IP07060\NOBACKUP.RSP
     LogFile1 = SB:\LOG\LS\30\IP07060\$(WorkStatName).L1
     LogFile2 = SB:\LOG\LS\30\IP07060\$(WorkStatName).L2
End
