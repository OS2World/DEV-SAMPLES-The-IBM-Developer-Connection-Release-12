TargetDir = C:\ 

Section Catalog
Begin
   ObjectType=Software
   GlobalName=AN2.IPXPS.INSTALL.REF.1.0
   Description="AnyNet IPX over SNA Gateway Version 1.0 - Printable Documentation Install"

End

Section Install
Begin
     Program = SB:\IMG\AN2\ALL\ANYNVDM2.EXE
     Parms = /A:I /X  /L1:$(LogFile1)  /L2:$(LogFile2) /L3:$(LogFile3) /R:$(ResponseFile) /S:$(SourceDir)
     SourceDir = SB:\IMG\AN2\ALL
     ResponseFile = SB:\RSP\AN2\ALL\AN2IPXPS.RSP
     LogFile1 = SB:\LOG\AN2\ALL\$(WorkStatName).ERR
     LogFile2 = SB:\LOG\AN2\ALL\$(WorkStatName).HIS
     LogFile3 = SB:\LOG\AN2\ALL\$(WorkStatName).LOG
End
