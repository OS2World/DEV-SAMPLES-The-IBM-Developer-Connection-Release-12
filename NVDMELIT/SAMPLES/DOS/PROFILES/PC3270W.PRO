TargetDir = C:\

Section Catalog
Begin
     ObjectType = SOFTWARE
     GlobalName = PC3270.WINDOWS.INSTALL.REF.4.0
     Description = 'PC3270 4.0 Windows Installation via DISKCAMERA'
End

Section FileSpecList
Begin
\PCOMWIN\*.* /is 
\PC32WTMP\PC3270W.MOD
End

Section Install
Begin
 Program     = NVDMWIN.EXE
 Parms       = NVDMWUPD.EXE $(TargetDir)PC32WTMP\PC3270W.MOD /T:$(TargetDir)
End

