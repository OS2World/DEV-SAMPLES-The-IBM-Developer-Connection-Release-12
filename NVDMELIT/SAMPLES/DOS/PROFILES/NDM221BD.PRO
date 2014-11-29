TargetDir = C:\IBMNVDM2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NVDM2.DOS.CLIENT.BOOT.WKSTNAME.REF.2.1
   Description="NVDM/2 DOS client install for boot environment"
End

Section Install
Begin
     Program = $(SourceDir)\NVDMIDOS.EXE
     Parms = /B:C  /R:$(ResponseFile) /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     SourceDir = SA:\IMG\NVDM2\21\GA
     ResponseFile = SB:\RSP\NVDM2\21\GA\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\NVDM2\21\GA\$(WorkStatName).LOG
End
