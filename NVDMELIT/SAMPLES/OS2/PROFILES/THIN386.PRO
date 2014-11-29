TargetDir = C:\SERVICE

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LS.301.ADV.THIN386.REF.3.0
   Description="Install THIN386 in the Maintenance Environment"

End

Section Install
Begin
     Program = SA:\IMG\LS\301\GA\ADV\IBM301S1\THIN386.EXE
     Parms =   /B:C /T:$(TargetDir)
End
