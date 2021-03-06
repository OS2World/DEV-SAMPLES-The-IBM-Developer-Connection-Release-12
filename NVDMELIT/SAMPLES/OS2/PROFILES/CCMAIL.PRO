TargetDir = C:\

Section Catalog
Begin
   ObjectType=Software
   GlobalName=SMART.SUITE.REF.1
   Description="Smart Suite Replication installation"
End

Section FileSpecList
Begin
\~INSX362.EXE
\~INSX462.EXE
\OS2\INSTALL\EPWICON.LOG
\OS2\DLL\pmgre.dat
\CCMAIL\ADDOBJ.CMD
\CCMAIL\ADDOBJ.DLL
\CCMAIL\ARCHIVE.SMI
\CCMAIL\BBOARD.SMI
\CCMAIL\BBSLIST.SMI
\CCMAIL\CCERR.DLL
\CCMAIL\CCFILE.DLL
\CCMAIL\CCFLDR.SMI
\CCMAIL\CCFOLDER.DLL
\CCMAIL\CCHELP.HLP
\CCMAIL\CCHELP.INF
\CCMAIL\CCMSG.DLL
\CCMAIL\CCMSG.SMI
\CCMAIL\CCMSGLST.DLL
\CCMAIL\CCRPC.DLL
\CCMAIL\CCSERVE.EXE
\CCMAIL\CCSMI.DLL
\CCMAIL\CCTHREAD.DLL
\CCMAIL\CCUTIL.DLL
\CCMAIL\CCVGA.DLL
\CCMAIL\CC_CRTL.DLL
\CCMAIL\CHARSET.DLL
\CCMAIL\CLUSEAU.EXE
\CCMAIL\DEFAULT.SMI
\CCMAIL\DIR.SMI
\CCMAIL\FLDRLIST.SMI
\CCMAIL\FOLDER.SMI
\CCMAIL\INBOX.SMI
\CCMAIL\LOTUSEN1.DIC
\CCMAIL\LOTUSFR1.DIC
\CCMAIL\LTSICN03.DLL
\CCMAIL\LTSSPL1.DLL
\CCMAIL\MAILENG.DLL
\CCMAIL\MAKEOBJ.EXE
\CCMAIL\MEMMAN.DLL
\CCMAIL\MLIST.SMI
\CCMAIL\MLISTLST.SMI
\CCMAIL\MSGFILE.DLL
\CCMAIL\MSGFIND.DLL
\CCMAIL\OUTBOX.DLL
\CCMAIL\PMLSTLST.SMI
\CCMAIL\PUBMLIST.SMI
\CCMAIL\README.TXT
\CCMAIL\RXMAC.DLL
\CCMAIL\SMI.DLL
\CCMAIL\UNLOCK.EXE
\CCMAIL\VIM.DLL
\CCMAIL\VLM.DLL
\SMART\SMART.MOD
End

Section Install
Begin
 Program     = NVDMUPO.EXE
 Parms       = $(TargetDir)SMART\SMART.MOD /T:$(TargetDir)
End

