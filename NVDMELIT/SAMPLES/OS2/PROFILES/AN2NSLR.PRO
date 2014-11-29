TargetDir = C:\ 

Section Catalog
Begin
   ObjectType=Software
   GlobalName=AN2.NSLR.INSTALL.REF.1.0
   Description="AnyNet/2 1.0 NetBEUI over SNA Library Reader/2 Install"

End

Section Install
Begin
     Program = SB:\IMG\AN2\10\GA\NS\INSTALL.EXE
     Parms = /A:I /X  /L1:$(LogFile1)  /L2:$(LogFile2) /L3:$(LogFile3) /R:$(ResponseFile) /O:DRIVE "/P:"BookManager Documentation"" /S:$(SourceDir)
     SourceDir = SB:\IMG\AN2\10\GA\NS
     ResponseFile = SB:\RSP\AN2\10\GA\NS\AN2NSLR.RSP
     LogFile1 = SB:\LOG\AN2\10\GA\NS\$(WorkStatName).ERR
     LogFile2 = SB:\LOG\AN2\10\GA\NS\$(WorkStatName).HIS
     LogFile3 = SB:\LOG\AN2\10\GA\NS\$(WorkStatName).LOG
End
