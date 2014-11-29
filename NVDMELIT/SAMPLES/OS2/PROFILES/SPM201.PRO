TargetDir = D:\SPM2V2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=SPM2.INSTALL.REF.2.01
   Description="SPM2 full function 2.01 install - no API, not dist. feat."

End

Section Install
Begin
     Program = SA:\IMG\SPM2\201\GA\SPMINST.EXE
     Parms =  /T:$(TargetDir) /S:$(SourceDir) /L1:$(LogFile1)
     SourceDir = SA:\IMG\SPM2\201\GA
     LogFile1 = SB:\LOG\SPM2\201\GA\$(WorkStatName).LOG
End
