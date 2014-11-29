TargetDir = C:\SERVICE


Section Catalog
Begin
   ObjectType=Software
   GlobalName=OS2.INSTALL.HPFS.ALLOPTIONS.REF.2.1.WR06200
   Description="OS/2 2.1 Manufacturing Refresh"
End

Section Install
Begin
     Program = SA:\EXE\OS2\211\GA\SEINST.EXE
     Parms = /B:C: /T:$(TargetDir) /R:$(ResponseFile) /S:$(SourceDir)  /L1:$(LogFile1)
     ResponseFile = SB:\RSP\OS2\211\GA\IHPFS.RSP
     SourceDir = SA:\IMG\OS2\211\GA
     LogFile1 = SB:\LOG\OS2\211\GA\$(WorkStatName).LOG
End
