TargetDir = C:\SERVICE

Section Catalog
Begin
   ObjectType=Software
   GlobalName=OS2.SERVICEPAK.INSTALL.REF.2.1.XR06200
   Description="OS/2 2.1 Service PAK install"
End

Section Install
Begin
     Program = SA:\IMG\OS2\21\XR06200\FSERVICE.EXE
     Parms = /CID /S:$(SourceDir)  /T:$(TargetDir)  /L1:$(LogFile1)  /R:$(ResponseFile)
     ResponseFile = SB:\RSP\OS2\21\XR06200\SERVPK.RSP
     SourceDir = SA:\IMG\OS2\21\XR06200
     LogFile1 = SB:\LOG\OS2\21\XR06200\$(WorkStatName).LOG
End
