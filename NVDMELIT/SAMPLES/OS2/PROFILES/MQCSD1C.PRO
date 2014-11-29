TargetDir = C:\MQM

Section Catalog
Begin
   ObjectType=Software
   GlobalName=MQS.CLIENT.WKSTNAME.REF.2.0.U200001
   Description="MQSeries 2.0 client install of CSD U200001"
End

Section Install
Begin
     Program = SA:\IMG\MQS\20\GA\INSTALL.EXE
     Parms =  /A:U /R:$(ResponseFile) /S:$(SourceDir)  /L1:$(LogFile1) /L2:$(LogFile2) /X
     ResponseFile = SB:\RSP\MQS\20\U200001\CLT\$(WorkStatName).RSP
     SourceDir = SA:\IMG\MQS\20\U200001
     LogFile1 = SB:\LOG\MQS\20\U200001\$(WorkStatName).OUT
     LogFile2 = SB:\LOG\MQS\20\U200001\$(WorkStatName).HIS
End
