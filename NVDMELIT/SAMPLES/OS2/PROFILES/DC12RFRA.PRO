TARGETDIR=C:\DCAF12

Section Catalog                                   
Begin                                             
  ObjectType=SOFTWARE                             
  GlobalName=DCAF.ALL.FUNCTIONS.REF.1.2.UB09790
  Description="DCAF V1.2 Refresh (UB09790) Install All Components"
End                                               

Section Install
Begin
   Program = SA:\img\dcaf\12\UB09790\DCAFALL.cmd
   SourceDir = SA:\img\dcaf\12\UB09790
   WorkingDir = SA:\img\dcaf\12\UB09790
End
