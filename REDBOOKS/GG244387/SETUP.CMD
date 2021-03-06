/* SETUP.CMD        */
/*  (C) Copyright IBM Corporation 1994                           */
/*  International Technical Support Organization                 */
/*  Austin Center                                                */
/* ************************************************************* */
/* Utility for preparing a new LAN Server from LAN Manager data  */
/* Written by Vincent Delaruelle, CBA Informatique, PARIS FRANCE */
/* Compuserve : 100117,525  Internet : 100117.525@COMPUSERVE.COM */
/* ************************************************************* */
Call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
Call SysLoadFuncs 
'@RXQUEUE /CLEAR'
/* Default value for operating mode is Batch (B).                */
/* You can change it below to Live (L) or Run (R).               */
/* */
OpMode = "B"
/* ************************************************************* */
/* Change StartVal if you do not want resources to be shared at  */
/* server startup. */
StartVal = '/W:STARTUP'
/* *** */
AdminID = "";AdminPwd = "";MigDrive = "";RootDrive = ""
/* ************************************************************* */
/* Below are the default values for the user input variables     */
/* You may change them to suit your environment                  */
/* *** */
DefMigDrive = "M";DefRootDrive = "C"
NoAudit = 0;NoSched = 0;NoScripts = 0
/* *** Input parameters **************************************** */
Call getAdminFile
Call userInput
If MigDrive = "" Then MigDrive = DefMigDrive
If RootDrive = "" Then RootDrive = DefRootDrive
If NewSrv = "" Then Call errNewSrv
If AdminID = "" Then Call errAdminID
Say '**************************'
Say '* Setting up '
Say '* Server    : 'NewSrv
IF OpMode = "R" THEN Say '* in Run mode'
IF OpMode = "B" THEN Say '* in Batch mode'
IF OpMode = "L" THEN Say '* in Live mode'
Say '**************************'
Queue "**** Setup log for server "NewSrv" ****"
Queue "**** on "Date()" "Time()" ****"
Queue "*********************************"
/* ************************************************************* */
SrvPath = '\MIGRATE\LS\'NewSrv
call SysFileTree '\MIGRATE\'NewSrv,NoServer,'DSO'
If NoServer.0 = 0 Then Call errSrvName
/* ************************************************************* */
call SysFileTree '\MIGRATE\'NewSrv'\NETAUD.BKP',NoAud,'FSO'
If NoAud.0 = 0 Then NoAudit = 1
call SysFileTree '\MIGRATE\'NewSrv'\SCHED.LOG',NoSch,'FSO'
If NoSch.0 = 0 Then NoSched = 1
call SysFileTree '\MIGRATE\'NewSrv'\*.*',NoScr,'DSO'
If NoScr.0 = 0 Then NoScripts = 1
/* ************************************************************* */
/* If we are in Live or Batch mode we create the STARTUP.CMD     */
/* file for the server to setup.                                 */
/* If in Run mode we will use an existing file                   */
/* */
IF OpMode = "R" THEN call liveOp
/* */
Queue "**** First prepare mode to create STARTUP.CMD ****"
Queue "**** Creating directory in \MIGRATE\LS ****"
/* Create directory for the current server */
Call CreateDirWork
ScrPath = "\REPL\IMPORT\SCRIPTS"
Queue "**** Preparing the STARTUP.CMD file ****"
/* Creating the strings     ******* */
NewSrvStart = SrvPath"\STARTUP.CMD"
RootLS = "LSRoot = LSDrv'\IBMLAN\'"
AccLS = "LSAcc = LSRoot'ACCOUNTS\'"
LogLS = "LSLog = LSRoot'LOGS\'"
RexFunc1 = "Call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'"
RexFunc2 = "Call SysLoadFuncs" 
Call findLS
NetAcc = "'@COPY 'LSAcc'NETACC.BKP 'LSAcc'NET.ACC >>MIGLS.LOG'"
NetAud = "'@COPY 'LSLog'NETAUD.BKP 'LSLog'NET.AUD >>MIGLS.LOG'"
UPMParam = "'UPMCSET /E'"
NetLog = "'@LOGON "AdminID" /P:"AdminPwd"'"
/* Creating the file STARTUP.CMD ******** */
Call LineOut NewSrvStart,"/* "NewSrv" "Date()" "Time()" */"
Call LineOut NewSrvStart, RexFunc1
Call LineOut NewSrvStart, RexFunc2
Do i = 1 To 10
	Call LineOut NewSrvStart, Fls.i
End
Call LineOut NewSrvStart, RootLS
Call LineOut NewSrvStart, AccLS
Call LineOut NewSrvStart, LogLS
Call LineOut NewSrvStart, NetAcc
If NoAudit = 0 Then Do
	Call LineOut NewSrvStart, NetAud
	End
	Else nop
Call LineOut NewSrvStart, UPMParam
Call prepACL
Call LineOut NewSrvStart, "'@NET START SERVER 2>>ERRMIGLS.LOG'"
Call LineOut NewSrvStart, NetLog
Call shareProc
Call LineOut NewSrvStart, "'@COPY \STARTUP.ORG STARTUP.CMD'"
Call LineOut NewSrvStart, "'@LOGOFF'"
Call LineOut NewSrvStart
/* ************************************************************ */
/* If in Batch mode we are done                                 */
/* If in Live mode we will now connect to the new               */
/* server and start its setup                                   */
/* If in Run mode we should not be here !                       */
/* */
IF OpMode = "L" THEN call liveOp
/* */
Call endProc
Exit
/* ************************************************* */
liveOp:
Queue "**** Now setting up the remote new LS server ****"
Queue "**** Connecting to the remote new LS server ****"
Queue "**** Copying backup account file to remote LS server ****"
'@COPY \MIGRATE\'NewSrv'\NETACC.BKP 'MigDrive':\ACCOUNTS\*.* 2>&1|RXQUEUE'
If rc <> 0 Then call errFindDir
If NoAudit = 0 Then Do
Queue "**** Copying backup audit file to remote LS server ****"
'@COPY \MIGRATE\'NewSrv'\NETAUD.BKP 'MigDrive':\LOGS\*.* 2>&1|RXQUEUE'
End
Else nop
Queue "**** Copying backup ACL files to remote LS server ****"
'@COPY \MIGRATE\'NewSrv'\*.ACL 'MigDrive':\ACCOUNTS\*.* 2>&1|RXQUEUE'
If NoSched = 0 Then Do
Queue "**** Copying SCHED.LOG to the new LS server ****"
'@COPY \MIGRATE\'NewSrv'\SCHED.LOG 'MigDrive':\LOGS\*.* 2>&1|RXQUEUE'
End
Else nop
If NoScripts = 0 Then Do
Queue "**** Copying logon scripts to the new LS server ****"
'COPY \MIGRATE\'NewSrv'\SCRIPTS\*.BAT 'MigDrive':'ScrPath' 2>&1|RXQUEUE'
'COPY \MIGRATE\'NewSrv'\SCRIPTS\*.CMD 'MigDrive':'ScrPath' 2>&1|RXQUEUE'
End
Else nop
'@NET USE 'MigDrive': /D 2>&1|RXQUEUE'
'@NET USE 'MigDrive': \\'NewSrv'\'RootDrive'$ 2>&1|RXQUEUE'
Queue "**** Saving original STARTUP.CMD on remote LS ****"
'@COPY 'MigDrive':\STARTUP.CMD 'MigDrive':\STARTUP.ORG 2>&1|RXQUEUE'
Queue "**** Copying migration STARTUP.CMD to remote LS ****"
'@COPY \MIGRATE\LS\'NewSrv'\STARTUP.CMD 'MigDrive':\*.* 2>&1|RXQUEUE'
'@NET USE 'MigDrive': /D 2>&1|RXQUEUE'
Queue "**** Rebooting remote LS server ****"
'@NET ADMIN \\'NewSrv' /C \OS2\SETBOOT /IBD:'RootDrive' 2>&1|RXQUEUE'
Call endProc
Exit
/* ************************************************************ */
prepACL:
/* Retrieve ACL backup information from extraction              */
/* and prepare the corresponding RESTACC commands               */
/* */
ACLFile = '\MIGRATE\'NewSrv'\SAVEACL.DAT'
DO While Lines(ACLFile)
ListDrive = LineIn(ACLFile)
END
call LineOut ACLFile
CountDrive = WORDS(ListDrive)
DO i = 1 TO CountDrive
	CurDrive = WORD(ListDrive,i)
	RestACL.i = "'@RESTACC "CurDrive": /S /Y >DUMMY.DAT'"
	call LineOut NewSrvStart,RestACL.i
END
Return
/* ************************************************* */
accDenied:
Say 'You do not have the right privileges.'
Say 'Please log on as administrator.'
Say 'Files have been prepared. After logon start'
Say 'SETUP for this server in Run mode (R).'
Exit
/* ************************************************************ */
errMkDir:
Say 'Unable to create the LM directory on 'NewSrv'.'
Exit
/* ************************************************************ */
CreateDirWork:
Call SysFileTree SrvPath,IsDirWork,'DSO'
If IsDirWork.0 <> 0 Then Do
	Say "Directory "SrvPath" already exists."
	Say "You should not overwrite existing data."
	Exit
	End
DirErr = SysMkDir(SrvPath)
If DirErr = 0 Then Return
If DirErr = 5 Then Do
	Say "Access denied to directory "SrvPath"."
	Exit
	End
	Else Do
	Say "Error "DirErr" while creating directory "SrvPath"."
	Exit
	End
/* ************************************************************ */
errFindDir:
Say 'Unable to locate the STARTUP.CMD file for server 'NewSrv'.'
IF OpMode = "R" THEN Do
	Say 'You are in Run mode.'
	Say 'Check that you have performed Batch mode'
	Say 'on server 'NewSrv'.'
	Exit
	End
	Else Do
	Say 'Check directory \MIGRATE\LS\'NewSrv'.'
	Exit
	End
Exit
/* ************************************************************ */
userInput:
/* Very simple user input routine !                            */
Say
Say 'Please enter the name of the server to setup : '
Pull NewSrv
Say
Say 'Please enter the user ID to be used for setup.'
Say 'It must be an account with administrator privileges'
Say 'in the LAN Manager domain you are migrating : '
If AdminID = "" Then Do
	Pull AdminID
	End
	Else Do
	Say "Your default account is : "AdminID
	Say "You may change it now : "
	Pull NewAdminID
	If NewAdminID <> "" Then AdminID = NewAdminID
	End
Say
Say 'Please enter the password of that user ID : '
If AdminPwd = "" Then Do
	Pull AdminPwd
	End
	Else Do
	Say "Your default password is : "AdminPwd
	Say "You may change it now : "
	Pull NewAdminPwd
	If NewAdminPwd <> "" Then AdminPwd = NewAdminPwd
	End
Say
Say 'Please choose the operating mode.'
Say 'Default is Batch where files will be prepared but no'
Say 'operation will be performed on the new LAN Server to setup.'
Say "You may choose L for Live where all operations are performed"
Say "in one run, or R for Run to perform operations using the"
Say "files created previously in Batch mode (B/L/R) : "
Pull OpMode
Say
Say 'Please enter the drive letter for the network drive.'
Say 'The default is 'DefMigDrive' : '
Pull MigDrive
IF LENGTH(MigDrive) <> 1 THEN MigDrive = LEFT(MigDrive,1)
Say
Say 'Please enter the drive letter of the drive where LAN '
Say 'Manager is installed on the server to setup. '
Say 'The default is 'DefRootDrive' : '
Pull RootDrive
IF LENGTH(RootDrive) <> 1 THEN RootDrive = LEFT(RootDrive,1)
Say
Return
/* *********************************************************** */
errNewSrv:
Say 'You must provide a server name to setup.'
Say 'Try again.'
Queue "**** No server name. Aborting ****"
Call endProc
Exit
/* *********************************************************** */
errAdminID:
If OpMode = "R" Then Return
Say 'You must provide an administrator account name.'
Say 'Try again.'
Queue "**** No admin account. Aborting ****"
Call endProc
Exit
/* *********************************************************** */
findLS:
Fls.1 = "ConfigFile = '\CONFIG.SYS';LSDrv = '';LSPath = ''"
Fls.2 = "Do While Lines(ConfigFile) > 0"
Fls.3 = "CurLine = Linein(ConfigFile)"
Fls.4 = "If pos('\NETWKSTA.200',CurLine) > 0 Then Do"
Fls.5 = "PathInfo = WORD(CurLine,2)"
Fls.6 = "LSPath = RIGHT(PathInfo,(LENGTH(PathInfo) - 3))"
Fls.7 = "LSDrv = LEFT(LSPath,2)"
Fls.8 = "End"
Fls.9 = "Else nop"
Fls.10 = "End"
Return 

/* ************************************************************ */
shareProc:
/* Migrating from LAN Manager to LAN Server                     */
/* Processing the SRVAUTO.PRO file to create shares and aliases */
/* */
Server = "\\"NewSrv
Printer = 0;Disk = 0;Comm = ""
Sharefile = '\MIGRATE\'NewSrv'\SRVAUTO.PRO'
Do While Lines(Sharefile) > 0
	CurLine = Linein(Sharefile)
	If pos('NET SHARE',CurLine) > 0 Then Do
		MaxNumUsers = ''; Printer = 0
		Parse var CurLine . . AllLine
		SpAlEnd = pos('"',AllLine,2)
		Alias = Left(AllLine,SpAlEnd)
		Remaining = Right(AllLine,Length(AllLine)-SpAlEnd)
		If pos('/PRINT',CurLine) <> 0 Then call prnAlias
			Else call diskAlias
		If pos('/UNLIMITED',Remaining) <> 0 Then Do
			MaxNumUsers = '/UN' 
			End
			Else Do 
			MaxNumUsers = ''
			End	
		If pos('/USERS:',Remaining) <> 0 Then Do
			UserSize = Length(Remaining) - (pos('/USERS:',Remaining) - 1)
			Userlim = Right(Remaining,UserSize)
			Parse var Userlim MaxNumUsers Trash
			End
			Else Do
			nop
			End
		If pos('/REMARK:',Remaining) <> 0 Then Do
			RemSize = Length(Remaining) - (pos('/REMARK:',Remaining) + 8)
			Rem1 = Right(Remaining,RemSize);Rem = Left(Rem1,pos('"',Rem1))
			If Length(Rem) > 41 Then Do
			Rem = Left(Rem,40)||'"'
			End
			Else nop
			Rem = '/R:"'||Rem 
			End
			Else Do 
			Rem = ''
			End
		If pos('/COMM',Remaining) <> 0 Then Comm = "/COMM"
		If Printer = 1 Then Do
		AliasCmd = "'NET ALIAS " AliasName Server AliasName StartVal "/PRINT" Rem MaxNumUsers"'"
		call LineOut NewSrvStart, AliasCmd
		Printer = 0
		End
		Else Do
		If Printer = 8 Then Do
		SharePrn = "'NET SHARE " Alias MaxNumUsers Rem "/PRINT'"
		call LineOut NewSrvStart, SharePrn
		Printer = 0
		End
		Else nop
		If Disk = 1 Then Do
		AliasCmd = "'NET ALIAS " AliasName Server ResourceName StartVal Comm Rem MaxNumUsers"'"
		call LineOut NewSrvStart, AliasCmd
		Disk = 0
		Comm = ""
		End
		Else Do
			If Disk = 8 Then Do
			ShareCmd = "'NET SHARE "Alias MaxNumUsers Rem Comm"'"
			call LineOut NewSrvStart, ShareCmd
			Disk = 0
			Comm = ""
			End
			Else nop
		End
	End
	Else Do
	If pos('NET COMM',CurLine) <> 0 Then Do
		CommCmd = "'"CurLine"'"
		call LineOut NewSrvStart, CommCmd
		End
		Else nop
	End
End	
RC = Lineout(Sharefile)
Return
/* ********************************************** */
prnAlias:
AliasName = Strip(Alias,'b','"') ; Printer = 1
If Length(AliasName) > 8 Then Printer = 8
If Words(AliasName) > 1 Then AliasName = '"'AliasName'"'
Return
/* ********************************************** */
/* ********************************************** */
diskAlias:
AliasEnd = pos('=',Alias) - 1;Alias1 = Left(Alias,AliasEnd)
AliasName = Right(Alias1,Length(Alias1)-1)
If AliasName = 'USERS' Then Return
If AliasName = 'NETLOGON' Then Return
If AliasName = 'PUBLIC' Then Return
If AliasName = 'REPL$' Then Return
Resource = Length(Alias) - (AliasEnd +1)
ResourceName = Right(Alias,Resource)
ResourceName = Left(ResourceName,Length(ResourceName)-1)
Disk = 1
If Length(AliasName) > 8 Then Disk = 8
If Words(AliasName) > 1 Then AliasName = '"'AliasName'"'
Return
/* ********************************************** */
getAdminFile:
AdminFile = '\MIGRATE\ADMIN.DAT'
Call SysFileTree AdminFile,NoAdmin,'FSO'
If NoAdmin.0 = 0 Then Return
Do While Lines(AdminFile) > 0
	CurLine = LineIn(AdminFile)
	If pos('NAME ',CurLine) > 0 Then Do
	AdminID = Word(CurLine,2)
	End
	If pos('PWD ',CurLine) > 0 Then Do
	AdminPwd = Word(CurLine,2)
	End
End
Return
/* ********************************************** */
errSrvName:
Say "Extraction does not seem to have been performed on"
Say "server "NewSrv"."
Say "There is no directory \MIGRATE\"NewSrv"."
Exit 
/* ********************************************** */
endProc:
LogFile = '\MIGRATE\LS\'NewSrv'\SETUP.LOG'
Do Queued()
	Call LineOut LogFile,LineIn('Queue:')
End
Call LineOut LogFile
Call Beep 300,500
Call Beep 500,250
'@RXQUEUE /CLEAR'
Return
