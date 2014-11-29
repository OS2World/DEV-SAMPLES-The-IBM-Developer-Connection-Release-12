/* EXTRACT.CMD */
/* Getting all the data from the LAN Manager server to be migrated */
/* It is stored in a directory LM on the C drive until we export it */
/* to the migration machine by diskette, network or any other way */
/* **************************************************** */
/* Command line argument is the drive where BACKACC.EXE is located */
/* Default is A. */
Driveparam = ARG(1,E)
IF Driveparam = 0 THEN Drive = "A"
	ELSE Do
	ARG DriveParam SaveScr
	Drive = LEFT(DriveParam,1)
	End
StorDrive = Drive ; SaveScript = 2
IF SaveScr = "" THEN SaveScr = "Y"
/* *************************************************** */
/* Default value for path and drive where LAN Manager  */
/* is installed. Used if unable to find it with findLM */
/* *************************************************** */
DefLMPath = "C:\LANMAN"
DefLMDrive = "C"
/* *************************************************** */
/* Looking for drive where LAN Manager is installed    */
/* Default is C                                        */
/* *************************************************** */
Call findLM
IF LMDrive = '' THEN LMDrive = DefLMDrive
If LMPath = '' THEN LMPath = DefLMPath
/* If working from a diskette create the LM directory  */
/* (for storing the extracted data) on the drive where */
/* LAN Manager is installed                            */ 
IF Drive = 'A' THEN StorDrive = LMDrive
IF Drive = 'B' THEN StorDrive = LMDrive
/* *************************************************** */
Call CreateMIGDir
/* *************************************************** */
/* Checking if we are logged as administrator          */
/* *************************************************** */
'@NET ACCOUNTS >'StorDrive':\LM\ACCOUNTS.DAT 2>>LM.LOG'
IF rc = 2 THEN call accDenied 
/* *************************************************** */
'@COPY 'LMPath'\LANMAN.INI 'StorDrive':\LM >>LM.LOG'
'@COPY 'LMPath'\PROTOCOL.INI 'StorDrive':\LM >>LM.LOG'
'@COPY 'LMPath'\PROFILES\SRVAUTO.PRO 'StorDrive':\LM >>LM.LOG'
'@COPY 'LMPath'\LOGS\SCHED.LOG 'StorDrive':\LM >>LM.LOG 2>>ERRLM.LOG'
/* *************************************************** */
/* Now getting domain and server name from LANMAN.INI  */
/* They will be kept with the other files in order to  */
/* identify the machine the files come from            */
/* *************************************************** */
call getIniNames
/* **************************************************** */
/* Check the location of the logon scripts              */
/* **************************************************** */
call getIniInfo
/* **************************************************** */
/* if logon scripts are in default location, or in      */
/* EXPORT, save them. If not we assume they are saved   */
/* during data backup operation.                        */
/* COMMENT OUT THE NEXT LINE OF CODE IF YOU DO NOT      */
/* WANT THE LOGON SCRIPTS TO BE SAVED DURING MIGRATION  */
/* **************************************************** */
If SaveScr = "Y" THEN Call saveScripts
/* **************************************************** */
/* Deleting existing ACL backups from LAN Manager utility */
/* **************************************************** */
'@DEL 'LMPath'\ACCOUNTS\ACLBAK*.ACL'
/* **************************************************** */
/* Using BACKACC.EXE from LAN Server to backup the NET.ACC file */
/* **************************************************** */
'@'Drive':\BACKACC'
/* **************************************************** */
/* Using BACKACC.EXE to save ACLs on each disk */
/* **************************************************** */
call saveACLs
/* **************************************************** */
'@COPY 'LMPath'\ACCOUNTS\NETACC.BKP 'StorDrive':\LM >>LM.LOG 2>>ERRLM.LOG' 
'@COPY 'LMPath'\LOGS\NETAUD.BKP 'StorDrive':\LM >>LM.LOG 2>>ERRLM.LOG' 
'@COPY 'LMPath'\ACCOUNTS\ACLBAK*.ACL 'StorDrive':\LM >>LM.LOG 2>>ERRLM.LOG' 
'@COPY LM.LOG 'StorDrive':\LM >DUMMY.DAT'
'@COPY ERRLM.LOG 'StorDrive':\LM >DUMMY.DAT'
'@DEL LM.LOG'
'@DEL ERRLM.LOG'
'@DEL DUMMY.DAT'
/* ***************************************************** */
'@NET ACCESS \PRINT /TREE >'StorDrive':\LM\PRNACCES.DAT'
'@NET ACCESS \COMM /TREE >'StorDrive':\LM\COMACCES.DAT'
'@NET ACCESS \PIPE /TREE >'StorDrive':\LM\PIPACCES.DAT'
'@NET ACCESS >'StorDrive':\LM\ACCESS.DAT'
'@NET USERS >'StorDrive':\LM\USERS.DAT'
'@NET GROUP >'StorDrive':\LM\GROUPS.DAT'
/* ***************************************************** */
IF SaveScript = 0 Then Do 
	Say
	Say 'Logon scripts saved'
	End
	Else call errSaveScript
IF Drive = 'A' THEN call endMessage
IF Drive = 'B' THEN call endMessage
/* ***************************************************** */
Exit /* END OF MAIN CODE   ***************************** */
/* ***************************************************** */
findLM:
ConfigFile = 'C:\CONFIG.SYS'
LMDrive = ''
LMPath = ''
Do While Lines(ConfigFile) > 0
	CurLine = Linein(ConfigFile)
	If pos('\NETWKSTA.SYS',CurLine) > 0 Then Do
		PathInfo = WORD(CurLine,2)
		LMPath = RIGHT(PathInfo,(LENGTH(PathInfo) - 3))
                LMDrive = LEFT(LMPath,1)
                Return 
		End
        Else nop
End
Return 
/* ***************************************************** */
getIniNames:
LMIniFile = StorDrive':\LM\LANMAN.INI'
DomainFile = StorDrive':\LM\DOMAIN.DAT'
ServerFile = StorDrive':\LM\SERVER.DAT'
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
/* *****************************************************   */
/* if your first fixed disk is not C, modify the starting  */
/* value of I i.e 4 for D, 5 for E, ...                    */
/* if you have disks you want to exclude, modify the range */
/* for example if you want to save only disks C, D and M,  */
/* do Abase = 'CDM' and change the values for I to 1 and 3 */
/* *****************************************************   */ 
Abase = XRANGE('A','Z')
ACLFile = StorDrive':\LM\SAVEACL.DAT'
ListDrive = ""
Do I = 3 to 26
	SrvDrive = RIGHT(LEFT(Abase,I),1)
	'@'Drive':\BACKACC 'SrvDrive':\ /S 2>>ERRLM.LOG'
	If rc = 0 Then Do
		ListDrive = ListDrive" "SrvDrive
		Iterate
		End
		Else Do
		call LineOut ACLFile,ListDrive
		call LineOut ACLFile
		Return
		End 
End
/* ***************************************************** */
getIniInfo:
LMIniFile = StorDrive':\LM\LANMAN.INI'
Script = "DEF"
Do While Lines(LMIniFile) > 0
	CurLine = Linein(LMIniFile)
	If pos('scripts',CurLine) > 0 Then Call LookScriptPath
	If pos('SCRIPTS',CurLine) > 0 Then Call LookScriptPath 
End
Call LineOut LMIniFile
Return
/* ***************************************************** */
LookScriptPath:
Script = "CUS"
ScriptFile = StorDrive':\LM\SCRIPTS.DAT'
ScriptPathLgth = LENGTH(CurLine) - pos('=',CurLine)
ScriptPath = RIGHT(CurLine, (ScriptPathLgth - 1))
If ScriptPath = "REPL\IMPORT\SCRIPTS" Then Script = "DEF"
If ScriptPath = "repl\import\scripts" Then Script = "DEF"
If ScriptPath = "REPL\EXPORT\SCRIPTS" Then Script = "EXP"
If ScriptPath = "repl\export\scripts" Then Script = "EXP"
Call LineOut ScriptFile,Script
Call LineOut ScriptFile,ScriptPath
Call LineOut ScriptFile
Return
/* ***************************************************** */
saveScripts:
If Script = "DEF" Then call saveLogonScriptDef
If Script = "EXP" Then call saveLogonScriptExp
Return
/* ***************************************************** */
saveLogonScriptDef:
'@MD 'StorDrive':\LM\SCRIPTS'
ScriptDir = StorDrive':\LM\SCRIPTS\'
'@COPY 'LMPath'\REPL\IMPORT\SCRIPTS\*.CMD 'ScriptDir'*.* >>LM.LOG'
'@COPY 'LMPath'\REPL\IMPORT\SCRIPTS\*.BAT 'ScriptDir'*.* >>LM.LOG'
If rc <> 0 Then SaveScript = 1
	Else SaveScript = 0
Return
/* ***************************************************** */
saveLogonScriptExp:
'@MD 'StorDrive':\LM\SCRIPTS'
ScriptDir = StorDrive':\LM\SCRIPTS\'
'@COPY 'LMPath'\REPL\EXPORT\SCRIPTS\*.CMD 'ScriptDir'*.* >>LM.LOG'
'@COPY 'LMPath'\REPL\EXPORT\SCRIPTS\*.BAT 'ScriptDir'*.* >>LM.LOG'
If rc <> 0 Then SaveScript = 1
	Else SaveScript = 0
Return
/* ***************************************************** */
errSaveScript:
IF SaveScript = 2 THEN Return
Say;Say '********  WARNING  ********'
Say 'There was an error while saving the logon scripts.'
Say 'Please check directory LM\SCRIPTS'
Return
/* ***************************************************** */
createMIGDir:
'@MD 'StorDrive':\LM'
/* IF rc <> 0 THEN Call errMIGDir */
Return
/* ***************************************************** */
errMIGDir:
Say;Say '*** An error occurred while creating directory LM ***'
Say 'Please check disk space and structure on disk 'StorDrive
Exit
/* ***************************************************** */
accDenied:
Say;Say
Say 'You do not have the right privileges.'
Say 'Please log on as administrator and run Extract again.'
Exit
/* ***************************************************** */
endMessage:
DataLoc1 = 'Data has been stored in the directory LM on 'StorDrive'.' 
DataLoc2 = 'You should now save it for future use in the migration process.'
Say;Say DataLoc1;Say DataLoc2
Exit