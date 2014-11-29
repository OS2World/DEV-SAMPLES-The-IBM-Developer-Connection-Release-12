TargetDir = C:\

Section Catalog
 Begin
   ObjectType = SOFTWARE
   GlobalName = LSP.WKSTNAME.REF.1.37
   Description = 'Installation of LSP 1.37 Program'
 End

Section Install
 Begin
   Program = $(SourceDir)\DXMAID.EXE
   Parms = /S:$(SourceDir) /TU:$(TargetDir) /R:$(ResponseFile) /L1:$(LogFile1) /X
   SourceDir = SA:\IMG\LSP\137\GA
   ResponseFile = SB:\RSP\LSP\137\GA\$(WorkStatName).RSP
   LogFile1 = SB:\LOG\LSP\137\GA\$(WorkStatName).LOG
 End
