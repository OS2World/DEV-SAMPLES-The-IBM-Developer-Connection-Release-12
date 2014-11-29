TargetDir = C:\


Section Catalog
Begin
   ObjectType=Software
   GlobalName=OS2.MULTIMEDIA.MINSTALL.MOTION.VIDEO.REF.3.0.WFP
   Description="OS/2 3.0 WARP WINOS2 Multimedia install of Motion Video"
End

Section Install
Begin
     Program = C:\MMTEMP\MINSTALL.EXE
     Parms = /M /R:$(ResponseFile)
     ResponseFile = SB:\RSP\OS2\WFP\GA\MEDIA.RSP
End
