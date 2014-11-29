TargetDir = D:\SPM2V2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=SPM2.SERVICEPAK.INSTALL.REF.2.0.WR06075
   Description="SPM2 ServicePak WR06075 install"

End

Section Install
Begin
     Program = SA:\IMG\SPM2\20\WR06075\FSERVICE.EXE
     Parms = /R:$(ResponseFile) /S:$(SourceDir) /L1:$(LogFile1)
     ResponseFile = SB:\RSP\SPM2\20\WR06075\SPM2CSD.RSP
     SourceDir = SA:\IMG\SPM2\20\WR06075
     LogFile1 = SB:\LOG\SPM2\20\WR06075\$(WorkStatName).LOG
End
