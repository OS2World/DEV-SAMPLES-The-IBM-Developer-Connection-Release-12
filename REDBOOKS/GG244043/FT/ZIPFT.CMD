/* rexx command file to produce self-extracting zip file for */
/* LAN Server Fault Tolerance recovery of boot drive         */

'@echo off'
ADDRESS CMD

/* change the following paths as necessary  */
/* for your machine environment */
/* these are the source libraries for what will */
/* copied onto the disks */

os2dll = 'C:\OS2\DLL\'
muglibdll = 'C:\MUGLIB\DLL\'
ibmlannetprog = 'C:\IBMLAN\NETPROG\'
ibmlannetlib = 'C:\IBMLAN\NETLIB\'
os2 = 'C:\OS2\'

/* filename of final zip file */
zipfile = 'FTFILES'

/* pkunzip command to produce zip file */
zipcom = 'PKZIP2 -a'

/* load rexx dll, if not loaded previously */

call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

call SysCls
say ' '
say 'This utility will create a self-extracting zip file'
say 'which contains all the necessary files to recover'
say 'the server boot drive using FTREMOTE.'
say ' '
say 'This utility needs approx 2.5 Mb free space available.'
say 'for temporary files, and produces the file' zipfile || '.EXE.'
say ' '

/* zip files from \OS2\DLL directory  */
zipcom zipfile os2dll || 'DISPLAY.DLL'
zipcom zipfile os2dll || 'DOSCALL1.DLL'
zipcom zipfile os2dll || 'MSG.DLL'
zipcom zipfile os2dll || 'NLS.DLL'
zipcom zipfile os2dll || 'PMGPI.DLL'
zipcom zipfile os2dll || 'PMGRE.DLL'
zipcom zipfile os2dll || 'PMSHAPI.DLL'
zipcom zipfile os2dll || 'PMSHLTKT.DLL'
zipcom zipfile os2dll || 'PMSPL.DLL'
zipcom zipfile os2dll || 'PMWIN.DLL'
zipcom zipfile os2dll || 'SPL1B.DLL'
zipcom zipfile os2dll || 'NAMPIPES.DLL'

/* zip files from \MUGLIB\DLL directory  */
zipcom zipfile muglibdll || 'NETAPI.DLL'

    /* remove the following line if LS3.0 Service Pack not installed */
    /* since it is not part of the LS 3.0 GA package                 */
zipcom zipfile muglibdll || 'NETAPI32.DLL'

zipcom zipfile muglibdll || 'NETOEM.DLL'
zipcom zipfile muglibdll || 'NETSPOOL.DLL'
zipcom zipfile muglibdll || 'MAILSLOT.DLL'

/* zip files from \IBMLAN\NETPROG directory   */
zipcom zipfile ibmlannetprog || 'FTREMOTE.EXE'

/* zip files from \IBMLAN\NETLIB directory */
zipcom zipfile ibmlannetlib || 'FT.DLL'

/* zip files from \OS2  directory */
zipcom zipfile os2 || 'FDISK.COM'

'zip2exe' zipfile

say 'Deleting temporary ZIP file . . . . .'

rc = SysFileDelete(zipfile || '.ZIP')

call SysCls
say ' '
say 'File has been created. Copy file' zipfile || '.EXE'
say 'to floppy disk to enable recovery of boot drive.'
exit
