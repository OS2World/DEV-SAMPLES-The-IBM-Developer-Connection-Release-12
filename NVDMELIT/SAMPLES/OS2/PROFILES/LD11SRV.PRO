TargetDir = D:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LD11.SERVER.INSTALL.REF.1.1
   Description="Install LD 1.1 Connection Server"
End

Section Install
Begin
     Program = SA:\IMG\LD\11\GA\LO093A1\INSTALL.EXE
     Parms = /T:D: /R:$(ResponseFile) /L1:$(LogFile1)
     ResponseFile = SB:\RSP\LD\11\GA\LDSERV.RSP
     LogFile1 = SB:\LOG\LD\11\GA\$(WorkStatName).LOG
End
