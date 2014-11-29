TargetDir = C:\NVDM2

Section Catalog
Begin
     ObjectType = Software
     GlobalName = NVDM2.SERVER.MAINT.WKSTNAME.REF.2.1
     Description = "NVDM/2 Server Install for Maintenance Environment"
End
Section Install
Begin
     Program = SA:\IMG\NVDM2\21\GA\NVDMINST.EXE
     Parms = /M /CO /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     PhaseEnd = Yes
     SourceDir = SA:\IMG\NVDM2\21\GA
     LogFile1 = SB:\LOG\NVDM2\21\GA\$(WorkStatName).LOG
End
