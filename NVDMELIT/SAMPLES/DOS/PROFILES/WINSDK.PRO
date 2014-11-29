TargetDir = C:\

Section Catalog
Begin
     ObjectType = SOFTWARE
     GlobalName = DB2.WINDOWS.SDK.INST.REF.1.2
     Description = "DB2 WINDOWS ONLY 1.2 SDK Install via Diskcamera"
End

Section FileSpecList
Begin
\SQLWINSD\*.* /is
\DBWINUPD\WINSDK.MOD
End

Section Install
Begin
 Program     = NVDMUPD.EXE
 Parms       = $(TargetDir)DBWINUPD\WINSDK.MOD /T:$(TargetDir)
End

