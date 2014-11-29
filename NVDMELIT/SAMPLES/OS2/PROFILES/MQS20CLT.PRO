TargetDir = C:\MQM

Section Catalog
Begin
   ObjectType=Software
   GlobalName=MQS.CLIENT.WKSTNAME.REF.2.0
   Description="MQSeries 2.0 client install"
End

Section Install
Begin
     Program = SA:\IMG\MQS\20\GA\INSTALL.EXE
     Parms =  /A:I /R:$(ResponseFile) /S:$(SourceDir)  /L1:$(LogFile1) /L2:$(LogFile2) /X
     ResponseFile = SB:\RSP\MQS\20\GA\CLT\$(WorkStatName).RSP
     SourceDir = SA:\IMG\MQS\20\GA
     LogFile1 = SB:\LOG\MQS\20\GA\$(WorkStatName).OUT
     LogFile2 = SB:\LOG\MQS\20\GA\$(WorkStatName).HIS
End
