TargetDir = C:\ 

Section Catalog
Begin
   ObjectType=Software
   GlobalName=AN2.LRGW.INSTALL.REF.1.1
   Description="AnyNet/2 1.1 GW Library Reader/2 Install"

End

Section Install
Begin
     Program = SB:\IMG\AN2\11\GA\GW\INSTALL.EXE
     Parms = /A:I /X  /L1:$(LogFile1)  /L2:$(LogFile2) /L3:$(LogFile3) /R:$(ResponseFile) /O:DRIVE "/P:"BookManager Documentation"" /S:$(SourceDir)
     SourceDir = SB:\IMG\AN2\11\GA\GW
     ResponseFile = SB:\RSP\AN2\11\GA\GW\AN2LRGW.RSP
     LogFile1 = SB:\LOG\AN2\11\GA\GW\$(WorkStatName).ERR
     LogFile2 = SB:\LOG\AN2\11\GA\GW\$(WorkStatName).HIS
     LogFile3 = SB:\LOG\AN2\11\GA\GW\$(WorkStatName).LOG
End
