TargetDir = C:\

Section Catalog
Begin
     ObjectType = SOFTWARE
     GlobalName = DB2.DOS.CAE.INST.REF.1.2
     Description = "DB2 DOS 1.2 CAE Install via Diskcamera"
End

Section FileSpecList
Begin
\SQLDOSCA\*.* /is
\DBCAEUPD\DBCAEUPD.MOD
End

Section Install
Begin
 Program     = NVDMUPD.EXE
 Parms       = $(TargetDir)DBCAEUPD\DBCAEUPD.MOD /T:$(TargetDir)
End

