TargetDir = C:\LMU2

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LNMU.MANAGING.REF.1.0
   Description="LNMU 1.0 MANAGING install"

End

Section Install
Begin
     Program = SA:\IMG\LNMU\10\GA\LMUCID.CMD
     Parms =  /R:$(ResponseFile) /T:C: /S:$(SourceDir) /L1:$(LogFile1)
     SourceDir = SA:\IMG\LNMU\10\GA
     ResponseFile = SB:\RSP\LNMU\10\GA\LNMUSRV.RSP
     LogFile1 = SB:\LOG\LNMU\10\GA\$(WorkStatName).LOG
End
