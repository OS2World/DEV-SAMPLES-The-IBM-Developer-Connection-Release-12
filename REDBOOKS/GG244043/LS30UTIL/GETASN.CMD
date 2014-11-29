/*******************************************************************/
/* GETASN.CMD                                                      */
/*                                                                 */
/* Get logon assignments for a list of users                       */
/* The argument is:                                                */
/*                                                                 */
/*  userIdFile  A ASCII file containing user Ids                   */
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

parse upper arg userIdFile dummy

/* Must have one argument */
if userIdFile='' then do
 say 'GETASN.CMD must have a ASCII file with user IDs'
 say 'to obtain logon assignments.'
 exit 9
end

call LoadFuncs; /* Load DLL utility functions */

userId = LINEIN(userIdFile,1,1)
if userId = '' then do
 say 'Could not open File'
 'pause'
 exit 9
end

do while userId <> ''
 parse upper var userId
 rc = GetLogonAsn(userId, 'listIt')

 if rc = '0' then do
  say 'userid=' userId
  do i=1 to listIt.0
   say listIt.i
  end
 end
 else do
  say 'Error occured. Returncode from GetLogonAsn() was: ' rc
 end
 say
 userId = LINEIN(userIdFile,,1)
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

