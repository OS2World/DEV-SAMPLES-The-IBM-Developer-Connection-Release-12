TargetDir = C:\ 

Section Catalog
Begin
   ObjectType=Software
   GlobalName=AN2.SIPS.INSTALL.REF.2.0
   Description="AnyNet\2 2.0 SNA over TCP/IP PostScript Install"

End

Section Install
Begin
     Program = SB:\IMG\AN2\20\GA\INSTALL.EXE
     Parms = /A:I /X  /L1:$(LogFile1)  /L2:$(LogFile2) /L3:$(LogFile3) /R:$(ResponseFile) /O:DRIVE "/P:"PostScript Documentation"" /S:$(SourceDir)
     SourceDir = SB:\IMG\AN2\20\GA
     ResponseFile = SB:\RSP\AN2\20\GA\AN2SIPS.RSP
     LogFile1 = SB:\LOG\AN2\20\GA\$(WorkStatName).ERR
     LogFile2 = SB:\LOG\AN2\20\GA\$(WorkStatName).HIS
     LogFile3 = SB:\LOG\AN2\20\GA\$(WorkStatName).LOG
End
