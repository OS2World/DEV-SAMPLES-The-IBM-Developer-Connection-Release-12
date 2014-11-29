TARGETDIR=C:\DCAF13

Section Catalog                                   
Begin                                             
  ObjectType=SOFTWARE                             
  GlobalName=DCAF.OS2.INSTALL.REF.1.3
  Description="DCAF V1.3 Install"
End                                               

Section Install
Begin
   Program = SA:\IMG\DCAF\13\GM\EQNRESP.EXE
   Parms = /S:$(SourceDir) /R:$(ResponseFile) /L1:$(LogFile1) /T:$(TARGETDIR)
   ResponseFile = SB:\RSP\DCAF\13\GM\ALLOS2.RSP
   LogFile1 = SB:\LOG\DCAF\13\GM\$(WorkStatName).LOG
   SourceDir = SA:\IMG\DCAF\13\GM
End
