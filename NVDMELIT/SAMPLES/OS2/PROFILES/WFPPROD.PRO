TargetDir = C:\SERVICE


Section Catalog
Begin
   ObjectType=Software
   GlobalName=OS2.INSTALL.HPFS.ALLOPTIONS.REF.3.0.WFP
   Description="OS/2 3.0 WARP WINOS2"
End

Section Install
Begin
     Program = SA:\EXE\OS2\WFP\GA\SEINST.EXE
     Parms = /B:C: /T:$(TargetDir) /R:$(ResponseFile) /S:$(SourceDir)  /L1:$(LogFile1)
     SourceDir = SA:\IMG\OS2\WFP\GA
     ResponseFile = SB:\RSP\OS2\WFP\GA\IHPFS.RSP
     LogFile1 = SB:\LOG\OS2\WFP\GA\$(WorkStatName).LOG
End
