TargetDir = D:\IBMNVDM2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NVDM2.SERVER.RA.PROD.DRIVE_D.WKSTNAME.REF.2.1
   Description="NVDM/2 server install for production environment"
End

Section Install
Begin
     Program = SA:\IMG\NVDM2\21\GA\NVDMINST.EXE
     Parms =  /CDM:H /RA /SRV /R:$(ResponseFile) /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     SourceDir = SA:\IMG\NVDM2\21\GA
     ResponseFile = SB:\RSP\NVDM2\21\GA\$(WorkStatName).SRV
     LogFile1 = SB:\LOG\NVDM2\21\GA\$(WorkStatName).LOG
End
