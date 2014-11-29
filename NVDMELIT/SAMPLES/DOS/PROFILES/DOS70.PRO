TargetDir = C:\

Section Catalog
Begin
    ObjectType  = SOFTWARE
    GlobalName  = DOS.WKSTNAME.REF.7.0
   Description = 'IBM DOS 7.0 installation files'
End

Section Install
 Begin
   Program = $(SourceDir)\USETUP.COM
   Parms = /R:$(ResponseFile) /L1:$(LogFile1) /L2:$(LogFile2)
   SourceDir = SA:\IMG\DOS\70\GA
   ResponseFile = SB:\RSP\DOS\70\GA\$(WorkStatName).RSP
   LogFile1 = SB:\LOG\DOS\70\GA\$(WorkStatName).L1
   LogFile2 = SB:\LOG\DOS\70\GA\$(WorkStatName).l2
 End
