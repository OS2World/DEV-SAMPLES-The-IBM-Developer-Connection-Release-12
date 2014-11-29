/*******************************************************************/
/* DMPALLU.CMD                                                     */
/*                                                                 */
/* Dump all user information from one domain as preparation for    */
/* transfer to another domain                                      */
/*                                                                 */
/* Arguments:                                                      */
/*  dumpfile        A file that will contain the binary user       */
/*                  information                                    */
/*                                                                 */
/*******************************************************************/

parse upper arg dumpFile

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

rc = DumpAllUsers( dcName, dumpFile )
say 'DumpAllUsers() returned ' rc

call DropLs30utFuncs
exit 0
