TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=WARP.CONNECT.TCPIP.INSTALL.WKSTNAME.REF.3.0
   Description="Warp Connect TCP/IP 3.0"
End

Section Install
Begin
     Program = SA:\IMG\CONNECT\CID\IMG\TCPAPPS\INSTALL.EXE 
     Parms = /b- /R:$(ResponseFile) /S:$(SourceDir)  /L1:$(LogFile1)
     ResponseFile = SA:\RSP\CONNECT\TCPAPPS\$(WorkStatName).RSP
     SourceDir = SA:\IMG\CONNECT\CID\IMG\TCPAPPS
     LogFile1 = SB:\LOG\CONNECT\TCPAPPS\$(WorkStatName).LOG
End
