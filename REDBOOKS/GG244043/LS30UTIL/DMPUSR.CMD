/*******************************************************************/
/* DMPUSR.CMD                                                      */
/*                                                                 */
/* Dump user information from one domain as preparation for        */
/* transfer to another domain                                      */
/*                                                                 */
/* Arguments:                                                      */
/*  users_list_file A file containing userids that will be         */
/*                  transferred                                    */
/*  dumpfile        A file that will contain the binary user       */
/*                  information                                    */
/*                                                                 */
/* users_list_file is a text file containing the userids that are  */
/*                 to be transferred.                              */
/*                                                                 */
/*          userid0                                                */
/*          userid1                                                */
/*          ...                                                    */
/*          useridn                                                */
/*                                                                 */
/*******************************************************************/

parse upper arg usrListFile dumpFile

if usrListFile = '' then do
 say 'Specify a user list file'
 exit 9
end

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

/* Ready to read user list file */
userid = linein(usrListFile,1,1)
if userid = '' then do
 say 'Could not obtain User List File'
 exit 9
end

do while userid <> ''
 rc = DumpUser(dcName, dumpFile, userid)
 say 'DumpUser() reported: ' rc
 userid = linein(usrListFile,,1)
end

call DropLs30utFuncs
exit 0

