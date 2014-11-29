[SYSTEM.INI]
AddUniqueKeyPrf(386Enh,VPollHeapSize)
14
AddMultipleKeyPrf(386Enh,device)
$(TargetDir)PCOMWIN\VpollD.386
AddMultipleKeyPrf(386Enh,device)
$(TargetDir)PCOMWIN\VslanD.386
AddMultipleKeyPrf(386Enh,device)
$(TargetDir)PCOMWIN\VvlanD.386
AddMultipleKeyPrf(386Enh,device)
$(TargetDir)PCOMWIN\VdoshllD.386
[WIN.INI]
AddUniqueKeyPrf(PCSWIN,DIR)
$(TargetDir)PCOMWIN\
[PROGMAN.INI]
CreateGroup(Personal Communications)
AddGroupItem(Personal Communications,NOTEPAD.EXE $(TargetDir)PCOMWIN\README.TXT,Readme - Please!,)
AddGroupItem(Personal Communications,NOTEPAD.EXE $(TargetDir)PCOMWIN\PCOMOS2.TXT,How to Install under OS/2 v2,)
AddGroupItem(Personal Communications,NOTEPAD.EXE $(TargetDir)PCOMWIN\PCOM.TXT,About This Release,)
AddGroupItem(Personal Communications,$(TargetDir)PCOMWIN\ZIPPRINT.EXE,ZipPrint,$(TargetDir)PCOMWIN)
AddGroupItem(Personal Communications,$(TargetDir)PCOMWIN\PCSADMI.EXE,Administration Tools,$(TargetDir)PCOMWIN)
AddGroupItem(Personal Communications,$(TargetDir)PCOMWIN\PCSBAT.EXE,Multiple Sessions,$(TargetDir)PCOMWIN)
AddGroupItem(Personal Communications,$(TargetDir)PCOMWIN\PCSWS.EXE,Start/Configure Session,$(TargetDir)PCOMWIN)
