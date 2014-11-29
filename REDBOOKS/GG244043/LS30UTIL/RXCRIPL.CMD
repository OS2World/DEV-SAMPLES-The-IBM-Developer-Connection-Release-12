/****************************************************************************/
/* REXX LAN Server 3.0 XCOPY RIPL restore procedure RXCRIPL.CMD             */
/*                                                                          */
/* The following parameters are required                                    */
/*                                                                          */
/*  copytype  The extent of the xcopy option. Either ALL or CHANGES only    */
/*  landrive  The IBMLAN drive like for example C:                          */
/*  backpath  The backup path for XCOPY like X:\BACKUP\RIPLSRV              */
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

'XCOPY ' backpath||'\RPLUSER\*.*' root||'RPLUSER\*.* ' XC_OPT
'XCOPY ' backpath||'\RPL\FITS\*.*' root||'RPL\FITS\*.* '  XC_OPT
'XCOPY ' backpath||'\RPL\IBMCOM\*.*' root||'RPL\IBMCOM\*.* ' XC_OPT
'XCOPY ' backpath||'\RPL\IBMLAN\*.*' root||'RPL\IBMLAN\*.* ' XC_OPT
'XCOPY ' backpath||'\RPL\MACHINES\*.*' root||'RPL\MACHINES\*.* ' XC_OPT
'XCOPY ' backpath||'\DCDB\IMAGES\*.DEF' root||'DCDB\IMAGES\*.DEF ' XC_OPT

exit 0

