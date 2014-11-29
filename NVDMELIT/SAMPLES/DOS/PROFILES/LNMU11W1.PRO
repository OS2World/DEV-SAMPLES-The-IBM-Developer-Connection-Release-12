TargetDir = C:\LMU
CompNameLen = 3

Section Catalog
Begin
       ObjectType = SOFTWARE
       GlobalName = IBM.LNMU.WIN.REF.1.1
       Description = 'LMU 1.1 Windows installation'
End

Section Install
Begin
      Program = $(SourceDir)\LMUINSTW.BAT
      Parms = C:
      SourceDir = SB:\IMG\LNMU\11
      WorkingDir = $(SourceDir)
End
