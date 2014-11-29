TargetDir = C:\IBMNVDM2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NVDM2.SERVER.PROD.CDRIVE.WKSTNAME.REF.2.1.XR20460
   Description="NVDM/2 server install CSD XR20460 for production environment on Drive C"
End

Section Install
Begin
     Program = SA:\IMG\NVDM2\21\XR20460\NVDMINST.EXE
     Parms =  /CDM:H /SRV /R:$(ResponseFile) /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     SourceDir = SA:\IMG\NVDM2\21\XR20460
     ResponseFile = SB:\RSP\NVDM2\21\XR20460\$(WorkStatName).SRV
     LogFile1 = SB:\LOG\NVDM2\21\XR20460\$(WorkStatName).LOG
End
