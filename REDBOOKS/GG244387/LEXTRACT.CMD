REM Getting all the data from the LAN Manager server to be migrated
REM It is stored in a directory LM on the C drive until we export it
REM to the migration machine by diskette, network or any other way 
IF %1 = "?" THEN GOTO HELP
IF %1 = "" THEN GOTO HELP
IF %2 = "" THEN GOTO HELP
@COPY C:\LANMAN\LANMAN.INI %2:\LM >>LM.LOG
@COPY C:\LANMAN\PROTOCOL.INI %2:\LM >>LM.LOG
@COPY C:\LANMAN\PROFILES\SRVAUTO.PRO %2:\LM >>LM.LOG
@COPY C:\LANMAN\LOGS\SCHED.LOG %2:\LM >>LM.LOG
REM /* **************************************************** */
REM Using BACKACC.EXE from LAN Server to backup the NET.ACC file */
REM /* **************************************************** */
@%1:\BACKACC
REM **************************************************** */
REM Using BACKACC.EXE to save ACLs on each disk */
REM Add one line for each drive */
REM **************************************************** */
@%1:\BACKACC C:\ /S 2>>ERRLM.LOG
REM @%1:\BACKACC x:\ /S 2>>ERRLM.LOG
REM **************************************************** */
@COPY C:\LANMAN\ACCOUNTS\NETACC.BKP %2:\LM >>LM.LOG 2>>ERRLM.LOG 
@COPY C:\LANMAN\LOGS\NETAUD.BKP %2:\LM >>LM.LOG 2>>ERRLM.LOG 
@COPY C:\LANMAN\ACCOUNTS\ACLBAK*.ACL %2:\LM >>LM.LOG 2>>ERRLM.LOG 
@COPY LM.LOG %2:\LM >DUMMY.DAT
@COPY ERRLM.LOG %2:\LM >DUMMY.DAT
@DEL LM.LOG
@DEL ERRLM.LOG
@DEL DUMMY.DAT
REM ***************************************************** */
@NET ACCESS \PRINT /TREE >%2:\LM\PRNACCES.DAT
@NET ACCESS \COMM /TREE >%2:\LM\COMACCES.DAT
@NET ACCESS \PIPE /TREE >%2:\LM\PIPACCES.DAT
@NET ACCESS >%2:\LM\ACCESS.DAT
@NET USERS >%2:\LM\USERS.DAT
@NET GROUP >%2:\LM\GROUPS.DAT
:ENDMESS
ECHO Data has been stored in the directory LM on C: 
ECHO You should now save it for future use in the migration process.
Exit
:HELP
ECHO The syntax is EXTRACT CodeDrive StoreDrive
ECHO where CodeDrive is the drive where EXTRACT.BAT and
ECHO BACKACC.EXE are located
ECHO and where StoreDrive is the drive where the extracted 
ECHO data will be stored.
EXIT