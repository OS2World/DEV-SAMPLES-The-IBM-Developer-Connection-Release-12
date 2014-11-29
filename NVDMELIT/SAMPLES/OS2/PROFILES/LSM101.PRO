TargetDir=C:\HLM

Section Catalog
Begin
     ObjectType=Software
     GlobalName=LANSTAMG.INSTALL.REF.1.01
      Description="Install Procedure for LAN Station Manager (UR39501) 1.01"
End

Section Install
Begin
      WorkingDir = SA:\IMG\LSM\101\GA
      Program =  SA:\IMG\LSM\101\GA\LSMRSPHD.EXE
      Parms= /S:$(SourceDir)  /T:$(TargetDir)  /R:$(responsefile)  /L1:$(logfile1) /TU:C: 
      SourceDir= SA:\IMG\LSM\101\GA
      ResponseFile =  SA:\RSP\LSM\101\GA\$(WorkStatName).RSP
      Logfile1=SB:\LOG\LSM\101\GA\$(WorkStatName).LOG
 End


