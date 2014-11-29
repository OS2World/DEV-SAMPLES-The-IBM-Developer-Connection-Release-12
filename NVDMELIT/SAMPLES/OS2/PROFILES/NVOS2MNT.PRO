TargetDir = "C:\"
CompNameLen = 2

Section Catalog
Begin
      ObjectType = SOFTWARE
      GlobalName = "NVOS2.MAINT.REF.2.0"
      Description = "Maintenance Installation of NetView For OS/2 2.0"
End

Section Install
Begin
      Program = SA:\IMG\NVOS2\20\GA\INSTALL.EXE
      Parms = "/X /R:$(ResponseFile) /L1:$(LogFile1) /A:U"
      ResponseFile = SB:\RSP\NVOS2\20\GA\$(WorkStatName).RSP
      LogFile1 = SB:\LOG\NVOS2\20\GA\$(WorkStatName).LOG
End
