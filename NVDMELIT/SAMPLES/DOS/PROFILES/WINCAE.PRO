TargetDir = C:\

Section Catalog
Begin
     ObjectType = SOFTWARE
     GlobalName = DB2.WINDOWS.CAE.INST.REF.1.2
     Description = "DB2 WINDOWS ONLY 1.2 CAE Install via Diskcamera"
End

Section FileSpecList
Begin
\SQLWINCA\*.* /is
\DBWINUPD\WINCAE.MOD
End

Section Install
Begin
 Program     = NVDMUPD.EXE
 Parms       = $(TargetDir)DBWINUPD\WINCAE.MOD /T:$(TargetDir)
End

