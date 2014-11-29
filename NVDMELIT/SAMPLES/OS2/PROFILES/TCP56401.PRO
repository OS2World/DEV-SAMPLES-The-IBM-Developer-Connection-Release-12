TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=TCPIP.BASE.INSTALL.REF.2.0.UN56401
   Description="Install TCP/IP 2.0 Base kit CSD UN56401"
End

Section Install
Begin
     Program = SA:\IMG\TCPIP\20\TCP56401\TCPINST.EXE
     Parms = /TU:C:  /R:$(ResponseFile) /S:$(SourceDir)  /L1:$(Logfile1) /SF- /A-
     ResponseFile = SB:\RSP\TCPIP\20\TCP56401\TCPCSD.RSP
     SourceDir = SA:\IMG\TCPIP\20\TCP56401
     LogFile1 = SB:\LOG\TCPIP\20\TCP56401\$(WorkStatName).LOG
End
