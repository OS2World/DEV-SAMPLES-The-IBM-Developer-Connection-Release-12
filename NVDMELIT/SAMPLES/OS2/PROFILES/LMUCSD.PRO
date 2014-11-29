TargetDir = C:\LMU2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LNMU.Install.CSD_EXTERNAL.MANAGED.REF.2.1.0
   Description="LNMU 1.0 210CSD External MANAGED Install"

End

Section Install
Begin
     Program = SA:\IMG\LNMU\10\csdinst\\lmu2\LMUCID.CMD
     Parms =  /R:$(ResponseFile) /T:C: /S:$(SourceDir) /L1:$(LogFile1)
     SourceDir = Sa:\img\lnmu\10\csdinst\lmu2
     ResponseFile = SB:\RSP\LNMU\10\csdinst\LMUCLT.RSP
     LogFile1 = SB:\LOG\LNMU\10\csdinst\$(WorkStatName).LOG
End
