/*******************************************************************/

Say 'This program will call many of the REXX functions provided'
Say 'in LS30UT.DLL. The first one is SysLoadFuncs, which makes'
Say 'all the others available.'

call RxFuncAdd 'LoadLs30utFuncs', 'LS30UT', 'LoadLs30utFuncs'
call LoadLs30utFuncs
Say 'LoadLs30utFuncs is complete.'

dcName = ''

parse value GetDCName() with rc dcName

say 'Return Code is:' rc
say 'Domain Controller Name is :' dcName

server = dcName
dumpfile = 'd:\dump.dat'

logfile = 'd:\logit.dat'
/* server = '\\LS3SRV02'   This is the server to insert users */
newpw = 'PASSWORD'

rc = InsertAllUsers(server, dumpfile, logfile, newpw)
say 'Return value from InsertAllUsers ' rc

call DropLs30utFuncs

Say 'The LS30UT functions have now been dropped.'
exit 0
