TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LD11.REMOTE.INSTALL.WKSTNAME.REF.1.1
   Description="Install LD 1.1 Client, check Response file for modem type"
End

Section Install
Begin
     Program = SA:\IMG\LD\11\GA\LO093R1\INSTALL.EXE
     Parms = /T:C: /R:$(ResponseFile) /L1:$(LogFile1)
     ResponseFile = SB:\RSP\LD\11\GA\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\LD\11\GA\$(WorkStatName).LOG
End
