TargetDir = C:\PC3270

Section Catalog
Begin
   ObjectType=Software
   GlobalName=PC3270.REMOVE.ALL.REF.4.00
   Description="Personal communications 3270 client remove"
End

Section Install
Begin
     Program = SA:\IMG\pc3270\40\GA\install.exe
     Parms =  /R:$(ResponseFile) /L1:$(LogFile1) /L2:$(LogFile2) 
     ResponseFile = SB:\RSP\PC3270\40\GA\PC32REM.RSP
     LogFile1 = SB:\LOG\PC3270\40\GA\$(WorkStatName).L1
     LogFile2 = SB:\LOG\PC3270\40\GA\$(WorkStatName).L2
End
