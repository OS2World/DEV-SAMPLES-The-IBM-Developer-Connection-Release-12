/*******************************************************************/
/* LASNACC.CMD This procedure returns the alias access control     */
/*             profiles for logon assignments and assigned         */
/*             applications. For external resources access control */
/*             profiles are not returned.                          */
/*                                                                 */
/* Return value examples:                                          */
/*                                                                 */
/*               D=  ALIAS    RWC                                  */
/*               D=  ALIAS8   external resource                    */
/*               A=  ALIAS1   RWC                                  */
/*               A=  ALIAS2     N                                  */
/*               A=  ALIAS3     R                                  */
/*                                                                 */
/* The complete path is not displayed, only the alias name.        */
/*                                                                 */
/*******************************************************************/

parse upper arg userId assignmentType dummyArg

if userId = '' then do
 call dspHelp;
 exit 9
end

if assignmentType = '' then
 assignmentType = 'ALL'

call LoadFuncs; /* Load DLL utility functions */

rc = GetLogonAsnAcp(userId, assignmentType, 'appList.')

if rc = '0' then do
 if appList.0 = 0 then do /* No ACPs found */
  say
  say 'No assignments found'
 end
 else do
  say
  do i=1 to appList.0
   say appList.i
  end
 end
end
else do
 say 'Error occured. Returncode from GetLogonAsnAcp() was: '
 say rc
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

 say 'Use LASNACC userId assignmentType'
 say
 say 'The userId must be supplied and the assignmentType values can be:'
 say
 say 'ALIAS  List access control profiles for alias'
 say 'APPS   List access control profiles for applications'
 say '       required alias resources'
 say 'ALL    List all of above'

return

