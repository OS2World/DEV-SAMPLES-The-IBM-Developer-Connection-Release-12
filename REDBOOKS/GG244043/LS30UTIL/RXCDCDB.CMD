/****************************************************************************/
/* REXX LAN Server 3.0 XCOPY DCDB restore procedure RXCDCDB.CMD             */
/*                                                                          */
/* The following parameters are required                                    */
/*                                                                          */
/*  copytype  The extent of the xcopy option. Either ALL or CHANGES only    */
/*  landrive  The IBMLAN drive like for example C:                          */
/*  backpath  The backup path for XCOPY like X:\BACKUP\DEPT01DC             */
/*                                                                          */
/****************************************************************************/

Parse Upper Arg copytype landrive backpath

SELECT
 WHEN copytype='ALL'     THEN XC_OPT = '/S /E';
 WHEN copytype='CHANGES' THEN XC_OPT = '/S /E /M';
 OTHERWISE
  EXIT 9
END

root = landrive || '\IBMLAN\'

'XCOPY ' backpath||'\DCDB\APPS\*.*' root || 'DCDB\APPS\*.* ' XC_OPT
'XCOPY ' backpath||'\DCDB\DATA\*.*' root || 'DCDB\DATA\*.* ' XC_OPT
'XCOPY ' backpath||'\DCDB\DEVICES\*.*' root || 'DCDB\DEVICES\*.* ' XC_OPT
'XCOPY ' backpath||'\DCDB\FILES\*.*' root || 'DCDB\FILES\*.* ' XC_OPT
'XCOPY ' backpath||'\DCDB\LISTS\*.*' root || 'DCDB\LISTS\*.* ' XC_OPT
'XCOPY ' backpath||'\DCDB\PRINTERS\*.*' root || 'DCDB\PRINTERS\*.* ' XC_OPT
'XCOPY ' backpath||'\DCDB\USERS\*.*' root || 'DCDB\USERS\*.* ' XC_OPT
'XCOPY ' backpath||'\LOGS\*.*' root || 'LOGS\*.* ' XC_OPT

exit 0

