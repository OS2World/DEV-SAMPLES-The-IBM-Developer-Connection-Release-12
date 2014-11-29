/*****************************************************/
/* Copy the content of USER and SYSTEM ini files to  */
/* the files specified as arguments                  */
/*****************************************************/

parse upper arg bootdrive userIni systemIni

/* Test arguments */
if (( bootdrive = '') | (userIni = '') | (systemIni = '')) then do
  call dspHelp
  exit 9
end

call RxFuncAdd SysLoadFuncs, RexxUtil, SysLoadFuncs
call SysLoadFuncs

front = bootdrive || '\OS2\'
userIniFile = bootdrive || '\OS2\' || userIni || '.INI'
systemIniFile = bootdrive || '\OS2\' || systemIni || '.INI'

rc = SysFileDelete(systemIniFile)
if ((rc = 5) | (rc = 32)) then do
 say 'The file ' systemIniFile ' is in use. Can not be deleted'
 exit 9
end

rc = SysFileDelete(front || userIni || '.INI')
if ((rc = 5) | (rc = 32)) then do
 say 'The file ' userIniFile ' is in use. Can not be deleted'
 exit 9
end

/* Make backup ini files */
'MAKEINI ' userIniFile front || 'INI.RC'
'MAKEINI ' systemIniFile front || 'INISYS.RC'

/* Do it first for USER (OS2.INI). In fact can be any file */
Result = SysIni('USER', 'All:', Apps.)

if Result \= 'ERROR:' then
do i = 1 to Apps.0
 call SysIni 'USER', Apps.i, 'All:', 'Keys'
 if Result \= 'ERROR:' then
  do j=1 to Keys.0
   val = SysIni('USER', Apps.i, Keys.j)
   rc  = SysIni(userIniFile, Apps.i, Keys.j, val)
  end
end

/* then for the SYSTEM Ini file (OS2SYS.INI) */
Result = SysIni('SYSTEM', 'All:', Apps.)

if Result \= 'ERROR:' then
do i = 1 to Apps.0
 call SysIni 'SYSTEM', Apps.i, 'All:', 'Keys'
 if Result \= 'ERROR:' then
  do j=1 to Keys.0
   val = SysIni('SYSTEM', Apps.i, Keys.j)
   rc  = SysIni(systemIniFile, Apps.i, Keys.j, val)
  end
end

exit 0

/*****************************************************/
/* Display Help                                      */
/*****************************************************/
dspHelp:

 say 'Backup OS2 and OS2SYS ini files. The syntax is:'
 say
 say 'BACKINI Bootdrive Userini Systemini'
 say
 say 'Userini   The name of the backup version of OS2.INI'
 say 'Systemini The name of the backup version of the OS2SYS.INI'
 say
 say 'Use for example the following to backup OS2.INI and OS2SYS.INI'
 say
 say 'BACKINI C: BOS2 BOS2SYS'

return
