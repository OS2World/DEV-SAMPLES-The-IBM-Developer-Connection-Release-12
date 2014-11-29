/*******************************************************************/
/* ALLASN.CMD                                                      */
/*                                                                 */
/* Get logon assignments for a all defines users                   */
/* No arguments are used                                           */
/*                                                                 */
/* Result:                                                         */
/*                  type A= for public application assignment      */
/*                       D= for device assignment, such as         */
/*                          G:, COM5, LPT3 or none                 */
/*                                                                 */
/* Examples of returned values:                                    */
/*                  userid= DEPT001                                */
/*                  D=   G:     FILESDIR                           */
/*                  D=   COM3   COMPORT                            */
/*                  A=          PUBLICAP                           */
/*                                                                 */
/*                  userid= DEPT002                                */
/*                  D=   O:     FILESDIR                           */
/*                  D=   COM7   COMPORT                            */
/*                  A=          PUBLICAP                           */
/*                                                                 */
/*                  userid= DEPT003                                */
/*                  -none-                                         */
/*******************************************************************/

parse upper arg dummy

call LoadFuncs; /* Load DLL utility functions */

/* Get user names into rxqueue */
'@net users | rxqueue'
pull line ; pull line
pull line ; pull line
do while queued() > 1

 parse pull userId userId1 userId2

 call getLASN STRIP(userId);
 if userId1 <> '' then do
  call getLASN STRIP(userId1);
  if userId2 <> '' then do
   call getLASN STRIP(userId2);
  end
 end

end

call DropFuncs
exit 0

/*************************************************************/
/* get Logon assignments                                     */
/*************************************************************/
getLASN:
 arg userName

 parse upper var userName
 rc = GetLogonAsn(userName, 'listIt')

 if rc = '0' then do
  say 'userid=' userName
  do i=1 to listIt.0
   say listIt.i
  end
 end
 else do
  say 'Error occured. Returncode from GetLogonAsn() was: ' rc
 end
 say

return

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

