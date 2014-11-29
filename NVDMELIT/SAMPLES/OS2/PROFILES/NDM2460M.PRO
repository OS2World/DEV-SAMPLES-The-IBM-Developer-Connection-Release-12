TargetDir = C:\IBMNVDM2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NVDM2.CLIENT.MAINT.WKSTNAME.REF.2.1.XR20460
   Description="NVDM/2 2.1 client install for maintenance environment"
End

Section Install
Begin
     Program = SA:\IMG\NVDM2\21\XR20460\NVDMCLT.EXE
     Parms = /M /CO /R:$(ResponseFile) /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     SourceDir = SA:\IMG\NVDM2\21\XR20460
     ResponseFile = SB:\RSP\NVDM2\21\XR20460\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\NVDM2\21\XR20460\$(WorkStatName).LOG
     PhaseEnd = Yes
End
