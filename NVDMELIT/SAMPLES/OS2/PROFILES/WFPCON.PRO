TargetDir = C:


Section Catalog
Begin
   ObjectType=Software
   GlobalName=WARP.CONNECT.OS2.INSTALL.HPFS.ALLOPTIONS.REF.3.0
   Description="Warp Connect Base OS/2 FullPak"
End

Section Install
Begin
     Program = SA:\EXE\OS2\WFPCON\SEINST.EXE
     Parms = /B:C: /T:$(TargetDir) /R:$(ResponseFile) /S:$(SourceDir)  /L1:$(LogFile1)
     SourceDir = SA:\IMG\CONNECT\OS2IMAGE
     ResponseFile = SA:\RSP\CONNECT\OS2\FHPFS.RSP
     LogFile1 = SB:\LOG\CONNECT\OS2\$(WorkStatName).LOG
End
