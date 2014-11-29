TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=NETWARE.OS2.REQUESTER.WCONNECT.REF.2.11
   Description="NetWare OS/2 Requester from Warp Connect"

End

Section Install
Begin
     Program = SB:\EXE\NWREQ\OS2\211\NWINST.CMD
     Parms = $(LogFile1) $(SourceDir) C
     SourceDir = SA:\IMG\CONNECT\CID\
     LogFile1 = SB:\LOG\NWREQ\OS2\211\$(WorkStatName).LOG
End
