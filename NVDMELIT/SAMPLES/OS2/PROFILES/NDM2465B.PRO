TargetDir = C:\IBMNVDM2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NVDM2.CLIENT.BOOT.WKSTNAME.REF.2.0.XR20465
   Description="NVDM/2 client install for boot environment"
End

Section Install
Begin
     Program = SA:\IMG\NVDM2\20\XR20465\NVDMCLT.EXE
     Parms = /B:C  /R:$(ResponseFile) /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     SourceDir = SA:\IMG\NVDM2\20\XR20465
     ResponseFile = SB:\RSP\NVDM2\20\XR20465\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\NVDM2\20\XR20465\$(WorkStatName).LOG
     PhaseEnd = Yes
End
