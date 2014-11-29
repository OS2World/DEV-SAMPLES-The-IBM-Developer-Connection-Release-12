/*******************************************************************/

Say 'This program will call many of the REXX functions provided'
Say 'in LS30UT.DLL. The first one is SysLoadFuncs, which makes'
Say 'all the others available.'

call RxFuncAdd 'LoadLs30utFuncs', 'LS30UT', 'LoadLs30utFuncs'
call LoadLs30utFuncs
Say 'LoadLs30utFuncs is complete.'

srcDir  = 'C:\IBMLAN\DCDB'
destDir = 'T:\IBMLAN\DCDB\A1\A2\A3'

rc = CopyDirAcls( srcDir, destDir )
say 'Returncode from CopyDirAcls( srcDir, destDir )' rc


call DropLs30utFuncs

Say 'The LS30UT functions have now been dropped.'
exit 0

