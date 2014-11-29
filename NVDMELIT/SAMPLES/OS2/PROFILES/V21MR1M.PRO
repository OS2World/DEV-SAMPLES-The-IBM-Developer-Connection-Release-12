TargetDir = C:\SERVICE


Section Catalog
Begin
   ObjectType=Software
   GlobalName=OS2.MAINTENANCE.INSTALL.REF.2.1.XR06200
   Description="Install OS/2 2.1 MR1 maintenance system"
End

Section Install
Begin
     Program = SA:\EXE\OS2\211\GA\SEMAINT.EXE
     Parms = /B:C: /T:$(TargetDir) /S:$(SourceDir) /L1:$(LogFile1)
     SourceDir = SA:\IMG\OS2\211\GA
     LogFile1 = SB:\LOG\OS2\211\GA\$(WorkStatName).LOG
End
