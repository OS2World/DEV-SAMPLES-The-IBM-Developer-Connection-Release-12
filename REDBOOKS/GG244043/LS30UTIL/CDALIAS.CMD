/*******************************************************************/

Say 'Testing QueryDirAliasPath'

call RxFuncAdd 'LoadLs30utFuncs', 'LS30UT', 'LoadLs30utFuncs'
call LoadLs30utFuncs
Say 'LoadLs30utFuncs is complete.'

dcName = ''

parse value GetDCName() with rc dcName
say 'Return Code is:' rc
say 'Domain Controller Name is :' dcName

alias = 'TESTALIA'
newpath = 'D:\TMP\NEWPATH'

rc = MoveDirAlias(dcName, alias, dcName, newpath)
say 'Returncode from MoveDirAlias: ' rc

parse value GetDCName() with rc dcName
say 'Return Code is:' rc
say 'Domain Controller Name is :' dcName

call DropLs30utFuncs

Say 'The LS30UT functions have now been dropped.'
exit 0


