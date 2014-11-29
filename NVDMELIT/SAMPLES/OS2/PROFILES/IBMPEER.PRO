TargetDir = C:


Section Catalog
Begin
   ObjectType=Software
   GlobalName=WARP.CONNECT.PEER.REF.1.0
   Description="Warp Connect Peer"
End

Section Install
Begin
     Program = SA:\IMG\CONNECT\CID\IMG\IBMPEER\PEERRMT.EXE
     Parms = /PEER /R:$(ResponseFile) /G:$(SourceDir) /L1:$(LogFile1) /L2:$(LogFile2)
     SourceDir = SA:\IMG\CONNECT\CID\IMG\IBMPEER
     ResponseFile = SA:\RSP\CONNECT\IBMPEER\$(WorkStatName).RSP
     LogFile1 = SB:\LOG\CONNECT\IBMPEER\$(WorkStatName).LOG
     LogFile2 = SB:\LOG\CONNECT\IBMPEER\$(WorkStatName).LG2
End
