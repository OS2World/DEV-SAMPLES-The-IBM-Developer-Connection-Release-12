TargetDir = C:\ 

Section Catalog
Begin
   ObjectType=Software
   GlobalName=AN2.NSPS.INSTALL.REF.1.0
   Description="AnyNet/2 1.0 NetBEUI over SNA PostScript Install"

End

Section Install
Begin
     Program = SB:\IMG\AN2\10\GA\NS\INSTALL.EXE
     Parms = /A:I /X  /L1:$(LogFile1)  /L2:$(LogFile2) /L3:$(LogFile3) /R:$(ResponseFile) /O:DRIVE "/P:"PostScript Documentation"" /S:$(SourceDir)
     SourceDir = SB:\IMG\AN2\10\GA\NS
     ResponseFile = SB:\RSP\AN2\10\GA\NS\AN2NSPS.RSP
     LogFile1 = SB:\LOG\AN2\10\GA\NS\$(WorkStatName).ERR
     LogFile2 = SB:\LOG\AN2\10\GA\NS\$(WorkStatName).HIS
     LogFile3 = SB:\LOG\AN2\10\GA\NS\$(WorkStatName).LOG
End
