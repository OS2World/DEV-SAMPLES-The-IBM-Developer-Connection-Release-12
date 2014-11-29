TargetDir=D:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=CM2.DISTRIBUTED.FEATURE.INSTALL.REF.1.1.1
     Description="Install CM/2 V1.11 Distributed Feature from NVDM/2 CC server"
End

Section Install
Begin
      Program =  SB:\IMG\CM2\111\GA\CMIMAGEU\CMLIB\CMLAN.EXE
      Parms=   /R:$(responsefile)  /l1:$(logfile1)  /l2:$(logfile2)
      ResponseFile =  SB:\RSP\CM2\111\GA\$(WorkStatName).RSP
      Logfile1=SB:\LOG\CM2\111\GA\$(WorkStatName).l1
      Logfile2=SB:\LOG\CM2\111\GA\$(WorkStatName).l2
 End
