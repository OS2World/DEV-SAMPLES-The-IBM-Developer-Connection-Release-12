TargetDir = C:\LMU2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LNMU.MANAGING.REF.1.1
   Description="LNMU 1.1 MANAGING Install"

End

Section Install
Begin
     Program = SA:\IMG\LNMU\10\csdinst\lmu2\LMUCID.CMD
     Parms =  /R:$(ResponseFile) /T:C: /S:$(SourceDir) /L1:$(LogFile1)
     SourceDir = SA:\IMG\LNMU\10\csdinst\lmu2
     ResponseFile = SB:\RSP\LNMU\10\csdinst\LMUSRV.RSP
     LogFile1 = SB:\LOG\LNMU\10\csdinst\lmu2\$(WorkStatName).LOG
End
