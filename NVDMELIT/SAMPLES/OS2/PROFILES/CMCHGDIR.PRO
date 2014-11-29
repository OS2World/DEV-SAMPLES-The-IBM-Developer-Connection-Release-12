TargetDir=C:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=CM2.CMCHGDIR.REF.1.1.1
     Description="Change redirected drive for Distributed Feature"
End

Section Install
Begin
      Program =  SB:\IMG\CM2\111\GA\CMIMAGEU\CMCHGDIR.CMD
      Parms=   "/D:G:\CMLIB  /l:$(logfile1)  /f:$(sb)\IMG\CM2\111\GA\CMIMAGEU;:CMSETUP.FLD"
      WorkingDir = SB:\IMG\CM2\111\GA\CMIMAGEU
      Logfile1=SB:\LOG\CM2\111\GA\CMCHGDIR.LOG
 End
