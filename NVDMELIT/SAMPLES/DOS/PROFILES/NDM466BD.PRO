TargetDir = C:\IBMNVDM2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NVDM2.DOS.21.REF.466
   Description="NVDM/2 DOS client install for boot environment"
End

Section Install
Begin
     Program = $(SourceDir)\NVDMIDOS.EXE
     Parms = /B:C  /R:$(ResponseFile) /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     SourceDir = SA:\IMG\NVDM2\21\XR20466
     ResponseFile = SB:\466RSP\$(WorkStatName).RSP
     LogFile1 = SB:\466LOG\$(WorkStatName).LOG
End
