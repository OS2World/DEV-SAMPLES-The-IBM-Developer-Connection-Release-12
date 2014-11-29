
TargetDir=C:\SERVICE

Section Catalog
Begin
   ObjectType=Software
   GlobalName=LS.CSD.IP07045.REF.3.0
   Description="Invoke the LAN Server IP07045 Corrective Service Facility"
End

Section Install
Begin
    Program =  SA:\IMG\LS\30\IP07045\FSERVICE.EXE
    Parms= /S:$(SourceDir)  /T:$(TargetDir) /L1:$(logfile1) /R:$(ResponseFile)
    SourceDir= SA:\IMG\LS\30\IP07045
    ResponseFile=SA:\RSP\LS\30\IP07045\LS30SRPK.RSP
    Logfile1=SB:\LOG\LS\30\IP07045\$(WorkStatName).LOG
 End

