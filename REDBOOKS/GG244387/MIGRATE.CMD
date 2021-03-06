/* MIGRATE.CMD */
/*  (C) Copyright IBM Corporation 1994                           */
/*  International Technical Support Organization                 */
/*  Austin Center                                                */
/* Getting all the data from the LAN Manager server to be migrated */
Call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
Call SysLoadFuncs 
'@RXQUEUE /CLEAR'
Call migUserInput
If SrvName = "" Then Call errSrvName
If StopSrv = "" Then StopSrv = "Y"
If NetDrive = "" Then NetDrive = "M"
If SaveLogonScr = "" Then SaveLogonScr = "Y"
If BootDrive = "" Then BootDrive = "C"
RemoteDir = NetDrive':\FROMLM'
Queue "**** Extraction log for server "SrvName" ****"
Queue "**** on "Date()" "Time()"    ****"
Queue "************************************"
Queue "**** Connecting to the LAN Manager server ****"
'@NET USE 'NetDrive': \\'SrvName'\ADMIN$ 2>&1|RXQUEUE'
Queue "**** Creating directory FROMLM on LM server ****"
Call createRemoteDir
Call SysFileTree '\MIGRATE\BACKACC.EXE', NoBack, 'FSO'
If NoBack.0 <> 0 Then Do
Queue "**** Copying BACKACC to LM Server ****"
'@COPY \MIGRATE\BACKACC.EXE 'RemoteDir' 2>&1|RXQUEUE'
End
Else Call error1
Queue "**** Creating directory "SrvName" ****"
Call createDirMig
'@NET ADMIN \\'SrvName' /C "NET SHARE" >\MIGRATE\'SrvName'\DISKS.DAT'
If rc = 2 Then Call accDenied
'@NET ADMIN \\'SrvName' /C "NET ACCOUNTS" >\MIGRATE\'SrvName'\ACCOUNTS.DAT'
'@NET ADMIN \\'SrvName' /C "NET ACCESS" >\MIGRATE\'SrvName'\ACCESS.DAT'
'@NET ADMIN \\'SrvName' /C "NET USERS" >\MIGRATE\'SrvName'\USERS.DAT'
'@NET ADMIN \\'SrvName' /C "NET GROUP" >\MIGRATE\'SrvName'\GROUPS.DAT'
'@NET ADMIN \\'SrvName' /C "NET ACCESS \PRINT /TREE" >\MIGRATE\'SrvName'\PRNACCES.DAT'
'@NET ADMIN \\'SrvName' /C "NET ACCESS \COMM /TREE" >\MIGRATE\'SrvName'\COMACCES.DAT'
'@NET ADMIN \\'SrvName' /C "NET ACCESS \PIPE /TREE" >\MIGRATE\'SrvName'\PIPACCES.DAT'
Queue "**** Copying LANMAN.INI from LM server ****"
'@COPY 'NetDrive':\LANMAN.INI \MIGRATE\'SrvName' 2>&1|RXQUEUE'
Queue "**** Copying PROTOCOL.INI from LM server ****"
'@COPY 'NetDrive':\PROTOCOL.INI \MIGRATE\'SrvName' 2>&1|RXQUEUE'
Queue "**** Copying SRVAUTO.PRO from LM server ****"
'@COPY 'NetDrive':\PROFILES\SRVAUTO.PRO \MIGRATE\'SrvName' 2>&1|RXQUEUE'
Call SysFileTree NetDrive':\LOGS\SCHED.LOG', NoSched, 'FS'
Parse var NoSched.1 . . SchedSize .
If SchedSize <> "0" Then Do
Queue "**** Copying SCHED.LOG from LM server ****"
'@COPY 'NetDrive':\LOGS\SCHED.LOG \MIGRATE\'SrvName' 2>&1|RXQUEUE'
End
Else nop
/* Now getting domain and server name from LANMAN.INI */
/* They will be kept with the other files in order to */
/* identify the machine the files come from           */
/* ************************************************** */
Call getIniNames
/* ************************************************** */
/* Now getting the location of the logon scripts      */
/* ************************************************** */
Call getIniScripts
/* Save logon scripts if asked to do so ************* */
If SaveLogonScr = "Y" Then Do
	If DefScr = 1 Then Do
		Call saveScripts
		DefScr = 0
		End
	Else nop
End
Else nop
/* ************************************************** */
/* If there are existing ACL backups we delete them   */
/* to avoid incompatibilities between LM and LS       */
/* ************************************************** */
Call SysFileTree NetDrive':\ACCOUNTS\ACL*.ACL', OldAcl, 'FSO'
If OldAcl.0 <> 0 Then Do
Queue "**** Removing previous ACLBAK files ****"
'@DEL 'NetDrive':\ACCOUNTS\ACL*.ACL 2>&1|RXQUEUE'
End
Else nop
Queue "**** Backup of NET.ACC and NET.AUD ****"
'@NET ADMIN \\'SrvName' /COMMAND "..\FROMLM\BACKACC"2>&1|RXQUEUE'
Call saveACLs
Queue "**** Copying the NETACC.BKP file from the LM server ****"
Call SysFileTree NetDrive':\ACCOUNTS\NETACC.BKP', NoAcc, 'FSO'
If NoAcc.0 <> 0 Then Do
'@COPY 'NetDrive':\ACCOUNTS\NETACC.BKP \MIGRATE\'SrvName'\ 2>&1|RXQUEUE'
End
Else Do
Say "Unable to retrieve the backup NET.ACC."
Say "The migration process is aborted."
Exit
End
Call SysFileTree NetDrive':\LOGS\NETAUD.BKP', NoAud, 'FSO'
If NoAud.0 <> 0 Then Do
Queue "**** Copying the NETAUD.BKP file from the LM server ****"
'@COPY 'NetDrive':\LOGS\NETAUD.BKP \MIGRATE\'SrvName'\ 2>&1|RXQUEUE'
End
Else nop
Call SysFileTree NetDrive':\ACCOUNTS\ACLBAK*.ACL', NoAcl, 'FSO'
If NoAcl.0 <> 0 Then Do
Queue "**** Copying the "Words(ListDrive)" ACLBAK* files from the LM server ****"
'@COPY 'NetDrive':\ACCOUNTS\ACLBAK*.ACL \MIGRATE\'SrvName'\ 2>&1|RXQUEUE'
End
Else nop
'@DEL 'RemoteDir'\BACKACC.EXE'
'@RD 'NetDrive':\FROMLM 2>&1|RXQUEUE'
Queue "**** Disconnecting from the LM server ****"
'@NET USE 'NetDrive': /D 2>&1|RXQUEUE'
Call prepEnd
Return
/* **************************************************** */
createDirMig:
/* If you wish to be able to overwrite existing data,   */
/* remove comments around the Return instruction.       */
Call SysFileTree '\MIGRATE\'SrvName,IsDirMig,'DSO'
If IsDirMig.0 <> 0 Then Do
/*	Return */ /* Activate this Return for overwriting */
	Say "Directory \MIGRATE\"SrvName" already exists."
	Say "You should not overwrite existing data."
	'@NET USE 'NetDrive': /D 2>&1|RXQUEUE'
	Exit
	End
DirErr = SysMkDir('\MIGRATE\'SrvName)
If DirErr = 0 Then Return
If DirErr = 5 Then Do
	Say "Access denied to directory \MIGRATE\"SrvName"."
	'@NET USE 'NetDrive': /D 2>&1|RXQUEUE'
	Exit
	End
	Else Do
	Say "Error "DirErr" while creating directory "SrvName"."
	'@NET USE 'NetDrive': /D 2>&1|RXQUEUE'
	Exit
	End
/* ************************************************************ */
createRemoteDir:
call SysFileTree RemoteDir,RemDir,'DSO'
If RemDir.0 = 1 Then Return
If RemDir.0 = 0 Then Do
	DirErr = SysMkDir(RemoteDir)
	If DirErr = 0 Then Return
	Else Do
	Say 'Error 'DirErr' while creating remote directory.'
	'@NET USE 'NetDrive': /D 2>&1|RXQUEUE'
	Exit
	End
Else Do
Say 'Too many remote directories.'
'@NET USE 'NetDrive': /D 2>&1|RXQUEUE'
Exit
End
/* *************************************************** */
error1:
Say 'You need the file BACKACC.EXE from LAN Server 3.0 to perform'
Say 'this operation. Make sure it is in the MIGRATE directory on'
Say 'this machine.'
Exit
/* ***************************************************** */
prepEnd:
IF StopSrv = "Y" THEN Do
	Queue "**** Stopping the LM server ****"
	'@NET ADMIN \\'SrvName' /C "NET STOP WORKSTATION /Y" 2>&1|RXQUEUE' 
	End
LogFile = '\MIGRATE\'SrvName'\EXTRACT.LOG'
Do Queued()
	Call LineOut LogFile,LineIn('Queue:')
End
Call LineOut LogFile
Call Beep 300,500
Call Beep 500,250
'@RXQUEUE /CLEAR'
Return
/* ***************************************************** */
getIniNames:
LMIniFile = '\MIGRATE\'SrvName'\LANMAN.INI'
DomainFile = '\MIGRATE\'SrvName'\DOMAIN.DAT'
ServerFile = '\MIGRATE\'SrvName'\SERVER.DAT'
Do While Lines(LMIniFile) > 0
	CurLine = Linein(LMIniFile)
  	If pos('computername',CurLine) > 0 Then Do 
	ServerLgth = LENGTH(CurLine) - pos('=',CurLine)
 	ServerName = RIGHT(CurLine, (ServerLgth - 1))
	End
	Else nop
  	If pos(' domain ',CurLine) > 0 Then Do 
	DomainLgth = LENGTH(CurLine) - pos('=',CurLine)
	DomainName = RIGHT(CurLine, (DomainLgth - 1))
	End
	Else nop
End
Call LineOut DomainFile,DomainName
Call LineOut DomainFile
Call LineOut ServerFile,ServerName
Call LineOut ServerFile
Call LineOut LMIniFile
Return
/* ***************************************************** */
saveACLs:
Queue "**** Saving the ACLs on all drives ****"
Abase = XRANGE('A','Z')
ACLFile = '\MIGRATE\'SrvName'\SAVEACL.DAT'
SaveFile = '\MIGRATE\'SrvName'\SAVEACL.CMD'
ListDrive = ""
Call getDisks
Call LineOut ACLFile,ListDrive
Call LineOut ACLFile
'@NET USE 'NetDrive': /D 2>&1|RXQUEUE'
Queue "**** Connecting to system disk ****"
'@NET USE 'NetDrive': \\'SrvName'\'BootDrive'$ 2>&1|RXQUEUE'
'@COPY \MIGRATE\BACKACC.EXE 'NetDrive':\*.* 2>&1|RXQUEUE'
Call LineOut SaveFile,'CD \'
Do ACLCount = 1 to Words(ListDrive)
CurACLDisk = Word(ListDrive,ACLCount)
Call LineOut SaveFile,''BootDrive':\BACKACC 'CurACLDisk':\ /S'
End
Call LineOut SaveFile
'@COPY 'SaveFile' 'NetDrive':\*.* 2>&1|RXQUEUE'
'@NET ADMIN \\'SrvName' /C "\SAVEACL.CMD"2>&1|RXQUEUE'
'@DEL 'NetDrive':\SAVEACL.CMD 2>&1|RXQUEUE'
'@DEL 'NetDrive':\BACKACC.EXE 2>&1|RXQUEUE'
'@NET USE 'NetDrive': /D 2>&1|RXQUEUE'
Queue "**** Reconnecting to ADMIN$ resource ****"
'@NET USE 'NetDrive': \\'SrvName'\ADMIN$ 2>&1|RXQUEUE'
Return
/* ***************************************************** */
getIniScripts:
LMIniFile = '\MIGRATE\'SrvName'\LANMAN.INI'
ScriptFile = '\MIGRATE\'SrvName'\SCRIPTS.DAT'
IsScripts = 0;DefScr = 0
Do While Lines(LMIniFile) > 0
	CurLine = Linein(LMIniFile)
	CurLine = Translate(CurLine)
	If pos('SCRIPTS =',CurLine) > 0 Then Do
	Call lookScriptPath
	Leave
	End
	Else nop 
End
Call LineOut LMIniFile
If IsScripts = 0 Then Do
	ScriptPath = "REPL\IMPORT\SCRIPTS"
	DefScr = 1
	End
	Else nop
IsScripts = 0
Queue "**** Saving scriptpath in SCRIPTS.DAT ****"
Call LineOut ScriptFile,ScriptPath
Call LineOut ScriptFile
Return
/* ***************************************************** */
lookScriptPath:
ScriptPathLgth = Length(Curline) - pos('=',CurLine)
ScriptPath = Right(CurLine,(ScriptPathLgth - 1))
IsScripts = 1
Return
/* ***************************************************** */
saveScripts:
DirErr = SysMkDir('\MIGRATE\'SrvName'\SCRIPTS')
If DirErr <> 0 Then Do
	Say "Error "DirErr" while creating directory "SrvName"\SCRIPTS."
	Exit
	End
	Else nop
ScriptDir = '\MIGRATE\'SrvName'\SCRIPTS'
IsScriptDr = FileSpec('Drive',ScriptPath)
IsScriptPath = FileSpec('Path',ScriptPath)
IsScriptPath = IsScriptPath||'\SCRIPTS'
If IsScriptDr = "" Then Call subSaveScripts
Else Do
ScriptNetDr = Left(IsScriptDr,1)
'@NET USE 'NetDrive': /D'
'@NET USE 'NetDrive': \\'SrvName'\'ScriptNetDr'$'
Call subSaveScripts
'@NET USE 'NetDrive': /D'
'@NET USE 'NetDrive': \\'SrvName'\ADMIN$ >LOCAL.LOG'
End
Return
subSaveScripts:
call SysFileTree NetDrive':\'IsScriptPath'\*.BAT',NoBat,'FSO'
call SysFileTree NetDrive':\'IsScriptPath'\*.CMD',NoCmd,'FSO'
If NoBat.0 = 0 Then Do
	If NoCmd.0 = 0 Then Do
	'@RD 'ScriptDir
	Return
	End
	Else Do
	Queue "**** Copying *.CMD scripts from LM server ****"
	'@COPY 'NetDrive':\'IsScriptPath'\*.CMD 'ScriptDir'\*.* 2>&1|RXQUEUE'
	Return
	End
End
Else Do
Queue "**** Copying *.BAT scripts from LM server ****"
'@COPY 'NetDrive':\'IsScriptPath'\*.BAT 'ScriptDir'\*.* 2>&1|RXQUEUE'
	If NoCmd.0 = 0 Then Return
	Else Do
	Queue "**** Copying *.CMD scripts from LM server ****"
	'@COPY 'NetDrive':\'IsScriptPath'\*.CMD 'ScriptDir'\*.* 2>&1|RXQUEUE'
	Return
	End
End	 
Return
/* ***************************************************** */
getDisks:
DisksFile = '\MIGRATE\'SrvName'\DISKS.DAT'
Do While Lines(DisksFile) > 0
	CurLine = LineIn(DisksFile)
	Parse var CurLine DiskShare .
	If Length(DiskShare) = 2 Then Do
		If Right(DiskShare,1) = "$" Then Do
		If Left(DiskShare,1) = "A" Then Iterate
		If Left(DiskShare,1) = "B" Then Iterate
		Do C = 1 to 26
		  Alpha = COMPARE(Left(DiskShare,1),Right(Left(Abase,C),1))
		  If Alpha = 0 Then Do
		  ACLDisk = Left(DiskShare,1)
		  ListDrive = ListDrive" "ACLDisk
		  Leave
		  End
		  Else nop
		End
		End
		Else Iterate
	End
	Else Iterate
End
Return
/* ***************************************************** */
errSrvName:
Say "You must provide the name of a server to migrate."
Say "Try again."
Exit
/* ***************************************************** */
accDenied:
Say
Say "You do not have the right privileges."
Say "Please log on as administrator and try again."
'@DEL 'NetDrive':\BACKACC.EXE 2>&1|RXQUEUE'
'@NET USE 'NetDrive': /D 2>&1|RXQUEUE'
'@RD \MIGRATE\'SrvName
Exit
/* ***************************************************** */
migUserInput:
Say
Say 'Please enter the name of the server to migrate : '
Pull SrvName
Say
Say 'Please enter the drive letter for the network drive.'
Say 'The default is M : '
Pull NetDrive
If Length(NetDrive) > 1 Then NetDrive = Left(NetDrive,1)
Say
Say 'Please enter the drive letter of the drive where LAN '
Say 'Manager is installed on the server to migrate.'
Say 'The default is C : '
Pull BootDrive
If Length(BootDrive) > 1 Then BootDrive = Left(BootDrive,1)
Say
Say 'Please indicate if the server should be stopped once'
Say 'the extraction process has been completed.'
Say 'The default is Yes (Y/N) ? '
Pull StopSrv
If Length(StopSrv) > 1 Then StopSrv = Left(StopSrv,1)
Say
Say 'Please indicate if you wish to save the logon scripts'
Say 'if they are located in the default location.'
Say 'The default is Yes (Y/N) ? '
Pull SaveLogonScr
If Length(SaveLogonScr) > 1 Then SaveLogonScr = Left(SaveLogonScr,1)
Return
/* ***************************************************** */
