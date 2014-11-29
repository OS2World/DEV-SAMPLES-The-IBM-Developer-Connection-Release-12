TargetDir = C:


Section Catalog
Begin
   ObjectType=Software
   GlobalName=WARP.CONNECT.LAN.DISTANCE.REMOTE.REF.1.0
   Description="Warp Connect LAN Distance Remote"
End

Section Install
Begin
     Program = SA:\IMG\CONNECT\CID\IMG\LDR\LO265R1\INSTALL.EXE
     Parms =  /L1:$(LogFile1) /R:$(ResponseFile) 
     ResponseFile = SA:\RSP\CONNECT\LDR\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\CONNECT\LDR\$(WorkStatName).LOG
End
