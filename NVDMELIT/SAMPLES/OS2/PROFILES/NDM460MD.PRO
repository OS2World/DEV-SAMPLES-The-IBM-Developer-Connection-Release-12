TargetDir = D:\IBMNVDM2
Section Catalog
Begin
     ObjectType = Software
     GlobalName = NVDM2.SERVER.MAINT.DDRIVE.WKSTNAME.REF.2.1.XR20460
     Description = "NVDM/2 2.1 Server Install for Maintenance Environment"
End
Section Install
Begin
     Program = SA:\IMG\NVDM2\21\XR20460\NVDMINST.EXE
     Parms = /M /CO /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     SourceDir = SA:\IMG\NVDM2\21\XR20460
     LogFile1 = SB:\LOG\NVDM2\21\XR20460\$(WorkStatName).LOG
     PhaseEnd = Yes
End
