TargetDir = C:\

Section Catalog
Begin
    ObjectType  = SOFTWARE
    GlobalName  = DOS.WKSTNAME.REF.6.3
   Description = 'IBM DOS 6.3 installation files'
End

Section Install
 Begin
   Program = $(SourceDir)\USETUP.COM
   Parms = /R:$(ResponseFile) /L1:$(LogFile1)
   SourceDir = SA:\IMG\DOS\63\GA
   ResponseFile = SB:\RSP\DOS\63\GA\$(WorkStatName).RSP
   LogFile1 = SB:\LOG\DOS\63\GA\$(WorkStatName).LOG
 End
