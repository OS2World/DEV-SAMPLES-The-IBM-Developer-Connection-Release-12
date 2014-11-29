TargetDir=C:\ 

Section Catalog
Begin
     ObjectType=Software
     GlobalName=LSM.DOS.CODEONLY.INSTALL.REF.1.0.UR39501
     Description="Install Code Only for DOS LSM 1.01"
End

Section Install
Begin
      WorkingDir = SA:\IMG\LSM\10\UR39501\DOS
      Program =  SA:\IMG\LSM\10\UR39501\DOS\LSMRSPHD.EXE
      Parms= /S:$(SourceDir)  /T:$(TargetDir)  /R:$(ResponseFile)  /L1:$(Logfile1)  /TU:C:
      SourceDir = SA:\IMG\LSM\10\UR39501\DOS
      ResponseFile = SB:\RSP\LSM\10\UR39501\DOS\LSMCODE.RSP
      Logfile1= SB:\LOG\LSM\10\UR39501\DOS\$(WorkStatName).LOG
 End


