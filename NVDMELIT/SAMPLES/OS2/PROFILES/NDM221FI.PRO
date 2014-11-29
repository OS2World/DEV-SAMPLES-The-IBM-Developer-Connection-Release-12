TargetDir = E:\IBMNVDM2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NVDM2.NVDMFPIN.DRIVE_E.REF.2.1
   Description="NVDM/2 2.1 FP Incremental install for Remote Admin function"
End

Section Install
Begin
     Program = SA:\IMG\NVDM2\21\GA\NVDMFPIN.EXE
     Parms =  /R:$(ResponseFile) /S:$(SourceDir) /L:$(LogFile1)
     SourceDir = SA:\IMG\NVDM2\21\GA
     ResponseFile = SB:\RSP\NVDM2\21\GA\NDM2FPIN.RSP
     LogFile1 = SB:\LOG\NVDM2\21\GA\$(WorkStatName).LOG
End
