TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LS.ADV.REQUESTER.WKSTNAME.REF.4.0
   Description="LS 4.0 Requester install"

End

Section Install
Begin
     Program = SA:\IMG\LS\40\LANINSTR.EXE
     Parms =   /REQ  /R:$(ResponseFile)  /L1:$(LogFile1)
     ResponseFile = SB:\RSP\LS\40\ADV\$(WorkStatName).REQ
     LogFile1 = SB:\LOG\LS\40\ADV\$(WorkStatName).LOG
End
