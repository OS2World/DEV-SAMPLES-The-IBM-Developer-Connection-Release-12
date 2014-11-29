/*******************************************************************/
/* PRTSTAT.CMD                                                     */
/*                                                                 */
/* Display the current status of server printer ports on a         */
/* LAN Server 3.0 server.                                          */
/*                                                                 */
/* If a server name is provided, such as \\OURSRV then only        */
/* information for this will be displayed. In no argument is       */
/* provided, all server devices within the current logged on       */
/* domain will be displayed.                                       */
/*                                                                 */
/*******************************************************************/

parse upper arg serverId dummyArg

call LoadFuncs; /* Load DLL utility functions */

if serverId = '' then do
 rc = NetEnum('servers',,'servers.', 1)
 if rc = '0' then do
  do i=1 to servers.0
   say servers.i
   rc = NetEnum('printdev',servers.i,'prtStatus.', 'STATUS')

   if rc = '0' then do
    do j=1 to prtStatus.0
     say prtStatus.j
    end
   end
   else do
    say 'Error occured obtaining printer status. Returncode from NetEnum() was:'
    say rc
    exit 9
   end

  end
 end
 else do
  say 'Error obtaining server names occured. Returncode from NetEnum() was:'
  say rc
  exit 9
 end

end
else do
 rc = NetEnum('printdev',serverId,'prtStatus.', 'STATUS')
 if rc = '0' then do
  say serverId
  do i=1 to prtStatus.0
   say prtStatus.i
  end
 end
 else do
  say 'Error occured obtaining printer status. Returncode from NetEnum() was:'
  say rc
  exit 9
 end
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