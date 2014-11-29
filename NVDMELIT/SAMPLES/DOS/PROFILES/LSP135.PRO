TargetDir = C:\

Section Catalog
 Begin
   ObjectType = SOFTWARE
   GlobalName = LSP.WKSTNAME.REF.1.35
   Description = 'Installation of LSP 1,35 Program'
 End

Section Install
 Begin
   Program = $(SourceDir)\DXMAID.EXE
   Parms = /S:$(SourceDir) /TU:$(TargetDir) /R:$(ResponseFile) /L1:$(LogFile1) /X
   SourceDir = SA:\IMG\LSP\135\GA
   ResponseFile = SB:\RSP\LSP\135\GA\$(WorkStatName).RSP
   LogFile1 = SB:\LOG\LSP\135\GA\$(WorkStatName).LOG
 End
