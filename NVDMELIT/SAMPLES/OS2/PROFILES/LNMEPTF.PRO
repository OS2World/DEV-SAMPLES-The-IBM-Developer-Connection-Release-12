TargetDir=C:\

Section Catalog
Begin
     ObjectType=Software
     GlobalName=LNME.PTF.INSTALL.REF.1.0.UR40624
      Description="Install Procedure for LAN Network Manager Entry 1.0 UR40624"
End

Section Install
Begin
      Program =  SA:\IMG\LNME\10\UR40624\DFIINCID.EXE
      Parms= /S:$(SourceDir) /R:$(responsefile)  /L1:$(logfile1) /L2:$(logfile2) 
      SourceDir= SA:\IMG\LNME\10\UR40624
      ResponseFile =  SA:\RSP\LNME\10\UR40624\LNMEPTF.RSP
      Logfile1=SB:\LOG\LNME\10\UR40624\$(WorkStatName).L1
      Logfile2=SB:\LOG\LNME\10\UR40624\$(WorkStatName).L2
 End


