TargetDir = C:\IBMNVDM2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NVDM2.SERVER.PROD.DRIVEC.WKSTNAME.REF.2.0.XR20465
   Description="NVDM/2 server install for production environment"
End

Section Install
Begin
     Program = SA:\IMG\NVDM2\20\XR20465\NVDMINST.EXE
     Parms =  /CDM:H /SRV /R:$(ResponseFile) /T:$(TargetDir) /S:$(SourceDir) /L:$(LogFile1)
     SourceDir = SA:\IMG\NVDM2\20\XR20465
     ResponseFile = SB:\RSP\NVDM2\20\XR20465\$(WorkStatName).SRV
     LogFile1 = SB:\LOG\NVDM2\20\XR20465\$(WorkStatName).LOG
End
