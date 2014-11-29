TARGETDIR = C:\DLS40

Section Catalog                                   
Begin                                             
  ObjectType=SOFTWARE                             
  GlobalName=DLS.WIN.INSTALL.REF.4.0
  Description="DOS LAN Requester V4.0 Windows Install"
End                                               

Section Install
Begin
   Program = SA:\IMG\LS\40\GM\DLS\INSTALL.EXE
   Parms = /L1:$(LogFile1) /R:$(ResponseFile) /T:$(TARGETDIR)
   ResponseFile = SB:\RSP\LS\40\GM\DLS\$(WorkStatName).WIN
   LogFile1 = SB:\LOG\LS\40\GM\DLS\$(WorkStatName).LOG
End
