/*******************************************************************/
/* PRTENUM.CMD                                                     */
/*                                                                 */
/* Display information of the printer ports of a server.           */
/*                                                                 */
/* The server machine ID must be used.                             */
/*                                                                 */
/* For example:  PRTENUM \\OURSRV                                  */
/*                                                                 */
/*******************************************************************/

parse upper arg serverId dummyArg

call LoadFuncs; /* Load DLL utility functions */

if serverId = '' then do
 say 'Specify a server machine ID'
end

rc = NetEnum('printdev',serverId,'prtInfo.')
if rc = '0' then do
 say serverId
 do i=1 to prtInfo.0
  say prtInfo.i
 end
end
else do
 say 'Error while obtaining printer information. Returncode from NetEnum() was:'
 say rc
 exit 9
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