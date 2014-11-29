TargetDir = C:\
CompNameLen = 3

Section Catalog
Begin
       ObjectType = SOFTWARE
       GlobalName = IBM.LNMU.WINCUST.REF.1.1
       Description = 'LMU 1.1 Windows customization'
End

Section Install
Begin
      Program = $(SourceDir)\LMUCUSTW.EXE
      Parms = IBM
      SourceDir = C:\LMU2
      WorkingDir = $(SourceDir)
End
