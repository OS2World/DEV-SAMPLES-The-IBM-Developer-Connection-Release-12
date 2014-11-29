TARGETDIR=C:\DCAF13W

Section Catalog
Begin
  ObjectType=SOFTWARE
  GlobalName=DCAF.TARGET.WIN.REF.1.3
  Description="DCAF V1.3 Windows Install"
End


Section Install
Begin
   Program = SB:\NVDMWIN.EXE
   Parms = $(SourceDir)\eqnwresp.exe /SD:$(SourceDir) /R:$(ResponseFile) /T:$(TARGETDIR) /L1:$(LogFile1)
   LogFile1 = SB:\$(WorkStatName).LOG
   ResponseFile = SB:\WINTGT.RSP
   SourceDir = SB:\DCAF13W
End
