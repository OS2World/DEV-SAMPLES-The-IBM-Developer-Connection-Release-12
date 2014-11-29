TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=WARP.CONNECT.LS.ADV.REQUESTER.WKSTNAME.REF.4.0
   Description="Warp Connect LS 4.0 GM Requester for install"

End

Section Install
Begin
     Program = SA:\IMG\CONNECT\CID\IMG\LSREQ\LANINSTR.EXE
     Parms =   /REQ  /R:$(ResponseFile)  /L1:$(LogFile1)
     ResponseFile = SA:\RSP\CONNECT\LS\$(WorkStatName).REQ
     LogFile1 = SB:\LOG\CONNECT\LS\40\GM\ADV\$(WorkStatName).LOG
End
