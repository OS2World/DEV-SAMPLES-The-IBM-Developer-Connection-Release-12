/*******************************************************************/
/* GET1ASN.CMD                                                     */
/*                                                                 */
/* Get one logon assignments for a user.                           */
/*                                                                 */
/* The argument is:                                                */
/*  user        The user ID                                        */
/*                                                                 */
/* Result:                                                         */
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

parse upper arg userId dummy

call LoadFuncs; /* Load DLL utility functions */

rc = GetLogonAsn(userId, 'asnList')

if rc = '0' then do
 say 'userid=' userId
 do i=1 to asnList.0
  say asnList.i
 end
 say
end
else do
 say 'Error occured. Returncode from GetLogonAsn() was: ' rc
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