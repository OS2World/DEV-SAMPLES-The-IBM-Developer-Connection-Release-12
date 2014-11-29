TargetDir = C:\


Section Catalog
Begin
   ObjectType=Software
   GlobalName=OS2.FORMAT.HPFS.ALLOPTIONS.REF.2.1.XR06200
   Description="OS/2 2.1 Manufacturing Refresh"
End

Section Install
Begin
     Program = SA:\EXE\OS2\211\GA\SEINST.EXE
     Parms = /B:C:  /T:A:\  /R:$(ResponseFile) /S:$(SourceDir)  /L1:$(LogFile1)
     ResponseFile = SB:\RSP\OS2\211\GA\FHPFS.RSP
     SourceDir = SA:\IMG\OS2\211\GA
     LogFile1 = SB:\LOG\OS2\211\GA\$(WorkStatName).LOG
End
