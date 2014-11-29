TargetDir = D:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LS.ADV.SERVER.WKSTNAME.REF.4.0
   Description="LS 4.0 Server install"

End

Section Install
Begin
     Program = SA:\IMG\LS\40\LANINSTR.EXE
     Parms =   /SRV  /R:$(ResponseFile)  /L1:$(LogFile1)
     ResponseFile = SB:\RSP\LS\40\ADV\$(WorkStatName).SRV
     LogFile1 = SB:\LOG\LS\40\ADV\$(WorkStatName).LOG
End
