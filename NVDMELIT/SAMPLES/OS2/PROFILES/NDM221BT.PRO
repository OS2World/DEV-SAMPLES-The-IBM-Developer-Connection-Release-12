TargetDir = C:\IBMNVDM2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NVDM2.CLIENT.BOOT.WKSTNAME.REF.2.1
   Description="NVDM/2 client install for boot environment"
End

Section Install
Begin
     Program = SA:\IMG\NVDM2\21\GA\NVDMCLT.EXE
     Parms = /B:C  /R:$(ResponseFile) /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     PhaseEnd = Yes
     SourceDir = SA:\IMG\NVDM2\21\GA
     ResponseFile = SB:\RSP\NVDM2\21\GA\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\NVDM2\21\GA\$(WorkStatName).LOG
End
