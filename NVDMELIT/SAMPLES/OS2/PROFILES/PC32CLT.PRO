TargetDir = C:\PC3270

Section Catalog
Begin
   ObjectType=Software
   GlobalName=PC3270.CLIENT.WKSTNAME.REF.4.00
   Description="Personal communications 3270 client install"
End

Section Install
Begin
     Program = SA:\IMG\pc3270\40\GA\install.exe
     Parms =  /R:$(ResponseFile) /G:$(SB)\RSP\PC3270\40\GA /L1:$(LogFile1) /L2:$(LogFile2)
     ResponseFile = SB:\RSP\PC3270\40\GA\$(WorkStatName).RSP
     SourceDir = SA:\IMG\PC3270\40\GA
     LogFile1 = SB:\LOG\PC3270\40\GA\$(WorkStatName).L1
     LogFile2 = SB:\LOG\PC3270\40\GA\$(WorkStatName).L2
End
