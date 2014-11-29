TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=Warp.Connect.NSC.Tools.REF.1.11
   Description="Warp Connect Network Signon Coordinator and Tools install"

End

Section Install
Begin
     Program = SA:\EXE\CONNECT\TOOLS\CONTOOLS.CMD
     Parms = $(LogFile1) C: $(Sourcedir)
     SourceDir = SA:
     LogFile1 = SB:\LOG\CONNECT\TOOLS\$(WorkStatName).LOG
End
