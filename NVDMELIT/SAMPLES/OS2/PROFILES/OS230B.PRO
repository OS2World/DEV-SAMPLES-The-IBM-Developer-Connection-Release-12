TargetDir = C:\


Section Catalog
Begin
   ObjectType=Software
   GlobalName=OS2.FORMAT.HPFS.ALLOPTIONS.REF.3.0.WARP
   Description="OS/2 3.0 WARP"
End

Section Install
Begin
     Program = SB:\EXE\OS2\WARP30\GA\SEINST.EXE
     Parms = /B:C:  /T:A:\  /R:$(ResponseFile) /S:$(SourceDir)  /L1:$(LogFile1)
     ResponseFile = SB:\RSP\OS2\WARP30\GA\FHPFS.RSP
     SourceDir = SB:\IMG\OS2\WARP30\GA
     LogFile1 = SB:\LOG\OS2\WARP30\GA\$(WorkStatName).LOG
End
