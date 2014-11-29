TargetDir = C:\IBMNVDM2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NVDM2.SERVER.MAINT.WKSTNAME.REF.2.0.XR20334
   Description="NVDM/2 server install for maintenance environment"
End

Section Install
Begin
     Program = SA:\IMG\NVDM2\20\XR20334\NVDMINST.EXE
     Parms = /M /CO /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     PhaseEnd = Yes
     SourceDir = SA:\IMG\NVDM2\20\XR20334
     LogFile1 = SB:\LOG\NVDM2\20\XR20334\$(WorkStatName).LOG
End
