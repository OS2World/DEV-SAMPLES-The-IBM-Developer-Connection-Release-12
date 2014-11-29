/*******************************************************************/
/* INSUSR.CMD                                                      */
/*                                                                 */
/* Insert user accounts information from a binary users accounts   */
/* dump file into the current domain.                              */
/*                                                                 */
/* Arguments:                                                      */
/*                                                                 */
/*  dumpfile        A file that will contain the binary user       */
/*                  information                                    */
/*  logfile         The name of the logfile, which will contain    */
/*                  log information of the insert operation        */
/*  newpassword     The new password for users inserted into       */
/*                  the current domain                             */
/*                                                                 */
/*******************************************************************/

parse upper arg dumpFile logfile newpassword

if dumpFile = '' then
 dumpFile = 'C:\DUMPFILE.BIN'

/* Get access to LS30UT.DLL functions */
call RxFuncAdd 'LoadLs30utFuncs', 'LS30UT', 'LoadLs30utFuncs'
call LoadLs30utFuncs

parse value GetDCName() with rc dcName

if rc <> '0' then do
 say 'Could not obtain Domain Controller name'
 exit 9
end

if dcName = '' then do
 say 'Could not obtain Domain Controller name'
 exit 9
end

if newpassword = '' then
 newpassword = 'PASSWORD'

if logfile = '' then
 logfile = 'C:\INSUSR.LOG'

rc = InsertAllUsers(dcName, dumpfile, logfile, newpassword)
say 'InsertAllUsers() returned: ' rc

call DropLs30utFuncs

exit 0
