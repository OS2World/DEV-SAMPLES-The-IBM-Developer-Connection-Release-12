/* Remark a config.sys statement */

parse upper arg configSys backupName remarkThis

'copy ' configSys backupName
if RC <> 0 then do
 say 'Could not make a backup copy of the File'
 'pause'
 exit 9
end

readLine = LINEIN(backupName,1,1)
if readLine == '' then do
 say 'Could not open File'
 'pause'
 exit 9
end

call LINEOUT configSys, , 1

do while readLine <> ''
 parse upper var readLine theFirst theSecond

 if theFirst = remarkThis then do
  call LINEOUT configSys, 'REM ' || readLine
 end
 else do
  call LINEOUT configSys, readLine
 end

 readLine = LINEIN(backupName,,1)
end

call LINEOUT configSys

exit 0

