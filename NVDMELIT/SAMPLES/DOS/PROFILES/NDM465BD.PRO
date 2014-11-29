TargetDir = C:\IBMNVDM2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NVDM2.DOS.20.REF.465
   Description="NVDM/2 DOS client install for boot environment"
End

Section Install
Begin
     Program = $(SourceDir)\NVDMIDOS.EXE
     Parms = /B:C  /R:$(ResponseFile) /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     SourceDir = SB:\465DOS
     ResponseFile = SB:\465RSP\$(WorkStatName).RSP
     LogFile1 = SB:\465LOG\$(WorkStatName).LOG
End
