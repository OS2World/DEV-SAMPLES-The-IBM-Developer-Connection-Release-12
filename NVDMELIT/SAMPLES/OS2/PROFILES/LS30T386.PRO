TargetDir = C:\SERVICE

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LS.ADV.THIN386.REF.3.0
   Description="Install THIN386 in the Maintenance Environment"

End

Section Install
Begin
     Program = SA:\IMG\LS\30\GA\ADV\THIN386.EXE
     Parms =   /B:C /T:$(TargetDir)
End
