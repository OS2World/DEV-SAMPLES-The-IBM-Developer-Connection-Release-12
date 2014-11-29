TargetDir = C:\LM20

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LNM20.GA.REF.2.0
   Description="LAN Network Manager 2.0 GA"

End

Section Install
Begin
     Program = SB:\IMG\LNM\20\GA\INSTALL.EXE
     Parms =  "/X /R:$(ResponseFile)  /L1:$(LogFile1) /A:I /T:$(TargetDir) /C:$(SourceDir)\DFIIN.ICF /P:"LAN Network Manager for OS/2" /O:DRIVE"
     SourceDir = SB:\IMG\LNM\20\GA
     ResponseFile = SB:\RSP\LNM\20\GA\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\LNM\20\GA\$(WorkStatName).LOG
End
