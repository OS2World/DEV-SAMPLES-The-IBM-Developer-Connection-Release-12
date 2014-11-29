/* INSTALL.CMD */
/* Installation batch file for installing the LAN Man to LAN Server */  
/* migration utilities on a DOS workstation */
Call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
Call SysLoadFuncs 
ARG InstDrive SourceDrive
IF InstDrive = "" THEN call instHelp
IF InstDrive = "?" THEN call instHelp
IF LENGTH(InstDrive) <> 1 THEN InstDrive = LEFT(InstDrive,1)
IF SourceDrive = "" THEN SourceDrive = "A"
IF LENGTH(SourceDrive) <> 1 THEN SourceDrive = LEFT(SourceDrive,1)
'@MD 'InstDrive':\MIGRATE'
'@COPY 'SourceDrive':\1STREAD.ME 'InstDrive':\MIGRATE\*.*'
'@COPY 'SourceDrive':\MIGRATE.BAT 'InstDrive':\MIGRATE\*.*'
'@COPY 'SourceDrive':\MIGRATE.CMD 'InstDrive':\MIGRATE\*.*'
'@COPY 'SourceDrive':\EXTRACT.CMD 'InstDrive':\MIGRATE\*.*'
'@COPY 'SourceDrive':\EXTRACT.BAT 'InstDrive':\MIGRATE\*.*'
'@COPY 'SourceDrive':\SETUP.CMD 'InstDrive':\MIGRATE\*.*'
'@COPY 'SourceDrive':\CLEANUP.CMD 'InstDrive':\MIGRATE\*.*'
'@COPY 'SourceDrive':\NIF2NIF.CMD 'InstDrive':\MIGRATE\*.*'
'@COPY 'SourceDrive':\LM2LS.EXE 'InstDrive':\MIGRATE\*.*'
'@MD 'InstDrive':\MIGRATE\LS'
'@MD 'InstDrive':\MIGRATE\NIF'
MigPrg = InstDrive':\MIGRATE\LM2LS.EXE'
MigDir = InstDrive':\MIGRATE'
'@COPY 'SourceDrive':\BACKACC.EXE 'InstDrive':\MIGRATE\*.*'
rc = SysCreateObject('WPProgram','LM to LS','<WP_DESKTOP>',,
'EXENAME='MigPrg)
If rc = 0 Then Do
	Say "Could not create program object"
	End
Say;Say
Say 'Installation has been completed.'
Exit
instHelp:
Say 'The syntax is INSTALL TargetDrive SourceDrive'
Say
Say 'where TargetDrive is the drive letter where you want to install' 
Say 'the migration utilities'
Say
Say 'and where SourceDrive is the drive letter to install from.'
Say 
Say 'TargetDrive is mandatory.'
Say
Say 'Default for SourceDrive is A.'
Exit