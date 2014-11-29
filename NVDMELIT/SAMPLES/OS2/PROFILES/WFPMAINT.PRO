TargetDir = C:\SERVICE


Section Catalog
Begin
   ObjectType=Software
   GlobalName=OS2.MAINTENANCE.INSTALL.REF.3.0.WFP
   Description="OS/2 3.0 WARP WINOS2 Maintenance System"
End

Section Install
Begin
     Program = SA:\EXE\OS2\WFP\GA\SEMAINT.EXE
     Parms = /B:C:  /T:$(TargetDir)  /S:$(SourceDir)  /L1:$(LogFile1)
     SourceDir = SA:\IMG\OS2\WFP\GA
     LogFile1 = SB:\LOG\OS2\WFP\GA\$(WorkStatName).LOG
End
