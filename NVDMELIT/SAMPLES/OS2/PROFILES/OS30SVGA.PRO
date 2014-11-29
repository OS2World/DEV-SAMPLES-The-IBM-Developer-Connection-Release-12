TargetDir = C:\


Section Catalog
Begin
   ObjectType=Software
   GlobalName=OS2.SVGA.DSPINSTL.REF.3.0.WARP
   Description="OS/2 3.0 WARP SVGA Install"
End

Section Install
Begin
     Program = C:\OS2\INSTALL\DSPINSTL.EXE
     Parms = /PD:C:\OS2\INSTALL\PSXGA32.DSC /S:$(SourceDir) /T:C: /RES:1024x768x256 /U
     SourceDir = SB:\IMG\OS2\WARP30\GA
End
