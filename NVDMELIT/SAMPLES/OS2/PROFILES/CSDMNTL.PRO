TargetDir = C:\SERVICE


Section Catalog
Begin
   ObjectType=Software
   GlobalName=OS2.LBUILD.MAINTENANCE.INSTALL.FOR.XR06200.SERVICEPAK.REF.2.1
   Description="Install OS/2 2.1 maintenance system for 2.11 install"
End

Section Install
Begin
     Program = SA:\EXE\OS2\21\GA\SEMAINT.EXE
     Parms = /B:C: /T:$(TargetDir) /S:$(SourceDir) /S2:$(SA)\IMG\OS2\21\XR06200 /L1:$(LogFile1)
     SourceDir = SA:\IMG\OS2\21\GA
     LogFile1 = SB:\LOG\OS2\21\GA\$(WorkStatName).LOG
End
