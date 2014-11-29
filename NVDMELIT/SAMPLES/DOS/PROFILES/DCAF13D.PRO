TARGETDIR=C:\DCAF13D

Section Catalog                                   
Begin                                             
  ObjectType=SOFTWARE                             
  GlobalName=DCAF.DOS.INSTALL.REF.1.3
  Description="DCAF V1.3 DOS Install"
End                                               

Section Install
Begin
   Program = SA:\IMG\DCAF\13\GM\DOS\EQNDRESP.EXE
   Parms = /S:$(SourceDir) /R:$(ResponseFile) /L1:$(LogFile1) /T:$(TARGETDIR)
   ResponseFile = SB:\RSP\DCAF\13\GM\DOS\DOSTGT.RSP
   LogFile1 = SB:\LOG\DCAF\13\GM\DOS\$(WorkStatName).LOG
   SourceDir = SA:\IMG\DCAF\13\GM\DOS
End
