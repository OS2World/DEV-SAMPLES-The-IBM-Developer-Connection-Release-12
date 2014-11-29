TARGETDIR=C:\DCAF12
 
Section Catalog                                                    
Begin                                                              
                                                                   
  ObjectType=SOFTWARE                                              
                                                                   
  GlobalName=DCAF.DEINSTALL.CMD.REF.1.2
                                                                   
  Description="DCAF V1.2 Remote De-install "
                                                                   
End                                                                
 
Section Install                           
Begin                                     
   Program = SA:\img\dcaf\12\DEINSTALL\deinst.cmd
   SourceDir= SA:\img\dcaf\12\DEINSTALL
   WorkingDir= C:\DCAF12
End                                       

