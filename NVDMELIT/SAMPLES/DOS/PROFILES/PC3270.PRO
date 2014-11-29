TargetDir = C:\

Section Catalog
Begin
    ObjectType = SOFTWARE
    GlobalName = PC3270.INSTALL.REF.4.0
    Description = 'PC3270 4.0 DOS Installation via DISKCAMERA'
End

Section FileSpecList
Begin
PC3270\*.* /is
$PC3270.BAT
PC32TMP\PC3270.MOD
End

Section Install
Begin
    Program = NVDMUPD.EXE
    Parms = $(TargetDir)PC32TMP\PC3270.MOD /T:$(TargetDir)
End
