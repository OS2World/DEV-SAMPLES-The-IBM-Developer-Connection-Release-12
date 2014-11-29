TargetDir = C:\

Section Catalog
Begin
     ObjectType = SOFTWARE
     GlobalName = DB2.DOS.SDK.INST.REF.1.2
     Description = "DB2 DOS 1.2 SDK Install via Diskcamera"
End

Section FileSpecList
Begin
\SQLDOSSD\*.* /is
\DBSDKUPD\DBSDKUPD.MOD
End

Section Install
Begin
 Program     = NVDMUPD.EXE
 Parms       = $(TargetDir)DBSDKUPD\DBSDKUPD.MOD /T:$(TargetDir)
End

