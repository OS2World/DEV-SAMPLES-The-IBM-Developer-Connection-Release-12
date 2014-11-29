TargetDir = C:\IBMNVDM2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NVDM2.DOS.BOOT.REF.2.1.XR20460
   Description="NVDM/2 2.1 DOS client install for boot environment"
End

Section Install
Begin
     Program = $(SourceDir)\NVDMIDOS.EXE
     Parms = /B:C  /R:$(ResponseFile) /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     SourceDir = SA:\IMG\NVDM2\21\XR20460
     ResponseFile = SB:\RSP\NVDM2\21\XR20460\$(WorkStatName).DOS
     LogFile1 = SB:\LOG\NVDM2\21\XR20460\$(WorkStatName).LOG
End
