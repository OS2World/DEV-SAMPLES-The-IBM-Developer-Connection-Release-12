/****************************************************************************/
/* REXX LAN Server 3.0 Advanced XCOPY Backup procedure. BXC4AS.CMD          */
/*                                                                          */
/* The following parameters are required                                    */
/*                                                                          */
/*  copytype  The extent of the xcopy option                                */
/*  landrive  The IBMLAN drive like for example C:                          */
/*  destpath  The destination drive for XCOPY like X:\BACKUP\TODAY          */
/*  logfile   The logfile path and file name                                */
/*                                                                          */
/****************************************************************************/

Parse Upper Arg copytype landrive destpath logfile

call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

SELECT
 WHEN copytype='ALL'     THEN XC_OPT = '/S /E';
 WHEN copytype='CHANGES' THEN XC_OPT = '/S /E /M';
 OTHERWISE DO
  CALL DSP_HELP;
  EXIT 9
 END
END

/************************/
/* Append to log file   */
/************************/
rc = LINEOUT(logfile,  'XCOPY of Server files started ' time() date() )
if rc <> 0 then do
 say 'Error opening ' logfile ' Program stopped'
 EXIT 99
END

/* Get local attached drives and perform BACKACC on them */
Call "LINEOUT" logfile
dMap = SysDriveMap('C:', 'LOCAL')
i = 1
do while (WORD(dMap,i) <> '')
 'backacc ' word(dMap,i) || '\ /S'
 i = i + 1
end
root = landrive || '\IBMLAN\'
Call "LINEOUT" logfile,  'XCOPY of Access Control Profiles started '
'XCOPY ' root || 'ACCOUNTS\*.ACL ' destpath ||'\ACCOUNTS\*.* /S'
'XCOPY ' root || 'ACCOUNTS\*.BAK ' destpath ||'\ACCOUNTS\*.* /S'
'XCOPY ' root || 'ACCOUNTS\*.BKP ' destpath ||'\ACCOUNTS\*.* /S'

/* Is this a Domain Controller or Backup Domain Controller? */
rc = SysMkDir(root || 'DCDB')
if rc <> 0 then do
 Call "LINEOUT" logfile,  'XCOPY of DCDB files started ' time() date()
 'XCOPY ' root || 'DCDB\APPS\*.* ' destpath||'\DCDB\APPS\*.*' XC_OPT
 'XCOPY ' root || 'DCDB\DATA\*.* ' destpath||'\DCDB\DATA\*.*' XC_OPT
 'XCOPY ' root || 'DCDB\DEVICES\*.* ' destpath||'\DCDB\DEVICES\*.*' XC_OPT
 'XCOPY ' root || 'DCDB\IMAGES\*.DEF ' destpath||'\DCDB\IMAGES\*.DEF' XC_OPT
 'XCOPY ' root || 'DCDB\FILES\*.* ' destpath||'\DCDB\FILES\*.*' XC_OPT
 'XCOPY ' root || 'DCDB\LISTS\*.* ' destpath||'\DCDB\LISTS\*.*' XC_OPT
 'XCOPY ' root || 'DCDB\PRINTERS\*.* ' destpath||'\DCDB\PRINTERS\*.*' XC_OPT
 'XCOPY ' root || 'DCDB\USERS\*.* ' destpath||'\DCDB\USERS\*.*' XC_OPT
 'XCOPY ' root || 'LOGS\*.* ' destpath||'\LOGS\*.*' XC_OPT
end
else do
 rc = SysRmDir(root || 'DCDB')
end

/* RIPL services installed? */
rc = SysMkDir(root || 'RPL')
if rc = 0 then do
 rc = SysRmDir(root || 'RPL')
 Call "LINEOUT" logfile
 exit 0
 end
else do
 Call "LINEOUT" logfile,  'XCOPY of RIPL files started ' time() date()
 'XCOPY ' root || 'RPLUSER\*.* ' destpath||'\RPLUSER\*.*' XC_OPT
 'XCOPY ' root || 'RPL\FITS\*.* ' destpath||'\RPL\FITS\*.*' XC_OPT
 'XCOPY ' root || 'RPL\IBMCOM\*.* ' destpath||'\RPL\IBMCOM\*.*' XC_OPT
 'XCOPY ' root || 'RPL\IBMLAN\*.* ' destpath||'\RPL\IBMLAN\*.*' XC_OPT
 'XCOPY ' root || 'RPL\MACHINES\*.* ' destpath||'\RPL\MACHINES\*.*' XC_OPT
end

:figseg fit=16.
/* close response files */
Call "LINEOUT" logfile

exit 0

/**************************************************************************/
/* Display options                                                        */
/**************************************************************************/
DSP_HELP:
say
say ' BXC4AS copytype landrive destpath logfile'
say ' where'
say ' copytype  ALL files or file CHANGES'
say ' landrive  The IBMLAN drive like for example C:'
say ' destpath  The destination path for XCOPY like X:\BACKUP\TODAY'
say ' logfile   The logfile path and file name'
return

