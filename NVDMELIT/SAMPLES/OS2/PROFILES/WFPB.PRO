TargetDir = C:\


Section Catalog
Begin
   ObjectType=Software
   GlobalName=OS2.FORMAT.HPFS.ALLOPTIONS.REF.3.0.WFP
   Description="OS/2 3.0 WARP WINOS2, Format HPFS"
End

Section Install
Begin
     Program = SA:\EXE\OS2\WFP\GA\SEINST.EXE
     Parms = /B:C:  /T:A:\  /R:$(ResponseFile) /S:$(SourceDir)  /L1:$(LogFile1)
     ResponseFile = SB:\RSP\OS2\WFP\GA\FHPFS.RSP
     SourceDir = SA:\IMG\OS2\WFP\GA
     LogFile1 = SB:\LOG\OS2\WFP\GA\$(WorkStatName).LOG
End
