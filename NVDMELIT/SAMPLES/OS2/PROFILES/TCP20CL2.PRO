  TargetDir = C:\

  Section Catalog
  Begin
     ObjectType=Software
     GlobalName=TCPIP.7045LAPS.INSTALL.WKSTNAME.REF.2.0.CSD2
     Description="Install TCP/IP 2.0 + CSDs with a modified 7045 LAPS to include TCP drivers"
  End

  Section Install
  Begin
       Program = SA:\IMG\TCPIP\20\GA_CSD2\TCPINST.EXE
       Parms = /TU:C:  /R:$(ResponseFile) /S:$(SourceDir)  /L1:$(LogFile1) /A- /LP:$(SA)\IMG\TCPIP\20\GA_CSD2\LAPS7045 /LR:$(SB)\RSP\TCPIP\20\GA_CSD2\LAPSTCP.RSP /SF- /SRV:inetd,nfsstart,sendmail,ftpd,pmx
       ResponseFile = SB:\RSP\TCPIP\20\GA_CSD2\$(WorkStatName).RSP
       SourceDir = SA:\IMG\TCPIP\20\GA_CSD2
       LogFile1 = SB:\LOG\TCPIP\20\GA_CSD2\$(WorkStatName).LOG
  End
