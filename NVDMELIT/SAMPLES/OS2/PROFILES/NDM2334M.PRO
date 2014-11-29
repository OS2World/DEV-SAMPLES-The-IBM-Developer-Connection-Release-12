TargetDir = C:\IBMNVDM2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NVDM2.CLIENT.MAINT.WKSTNAME.REF.2.0.XR20334
   Description="NVDM/2 client install for maintenance environment"
End

Section Install
Begin
     Program = SA:\IMG\NVDM2\20\XR20334\NVDMCLT.EXE
     Parms = /M /CO /R:$(ResponseFile) /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     PhaseEnd = Yes
     SourceDir = SA:\IMG\NVDM2\20\XR20334
     ResponseFile = SB:\RSP\NVDM2\20\XR20334\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\NVDM2\20\XR20334\$(WorkStatName).LOG
End
