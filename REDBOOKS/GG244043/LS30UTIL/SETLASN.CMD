/*******************************************************************/
/* SETLASN.CMD                                                     */
/*                                                                 */
/* Set logon assignments for users.                                */
/* The procedure sets the logon assignments for users listed in a  */
/* file togother with another file specifing the assignments. The  */
/* files must be in ASCII. It is recommended to pipe the output of */
/* SETLASN to a file.                                              */
/*                                                                 */
/* The arguments are:                                              */
/*                                                                 */
/*  userList        The file name of the users                     */
/*                  The file contains a user name in each line.    */
/*                                                                 */
/*  assignments     The file containing the assignments. Supported */
/*                  line options are:                              */
/*                                                                 */
/*                  type device aliasname                          */
/*                                                                 */
/*                  type A= for public application assignment      */
/*                       D= for device assignment, such as         */
/*                          G:, COM5, LPT3 or none                 */
/*                  Examples:                                      */
/*                                                                 */
/*                  D=   G:     FILESDIR                           */
/*                  D=          FILESDIR                           */
/*                  D=   COM3   COMPORT                            */
/*                  D=   LPT6   LPTPRT                             */
/*                  A=          PUBLICAP                           */
/*                                                                 */
/*******************************************************************/

parse upper arg userList assignments

call LoadFuncs; /* Load DLL utility functions */

/* Must have all arguments with values */
if ((userList='') | (assignments='')) then do
 call dspHelp;
 exit 9
end

/* Ready to read user list file */
userid = linein(userList,1,1)
if userid = '' then do
 say 'Could not obtain User List File'
 exit 9
end

/* Ready to read assignment list file */
parse upper value linein(assignments,1,1) with type device aliasname
if type = '' then do
 say 'Could not obtain correct assignment list file'
 exit 9
end

do while type <> ''

 userid = linein(userList,1,1)
 do while userid <> ''
  /* Do the work here */
  if aliasname = '' then do
   aliasname = device
   device = ''
  end

  rc = SetLogonAsn(userid, type, device, STRIP(aliasname) )
  if rc <> '0' then do
   say 'SetLogonAsn returned ' rc
  end

  userid = linein(userList)
 end
 parse upper value linein(assignments) with type device aliasname
end

call DropFuncs
exit 0

/*************************************************************/
/* Load DLL Functions                                        */
/*************************************************************/
LoadFuncs:

 /* Load LS30UT.DLL functions */
 call RxFuncAdd 'LoadLs30utFuncs', 'LS30UT', 'LoadLs30utFuncs'
 call LoadLs30utFuncs

return

/*************************************************************/
/* Drop DLL Functions                                        */
/*************************************************************/
DropFuncs:

 /* Drop LS30UT.DLL functions */
 call DropLs30utFuncs

return

/*************************************************************/
/* Display help                                              */
/*************************************************************/
dspHelp:

  say 'Use SETLASN userFile assignmentsFile'

return

