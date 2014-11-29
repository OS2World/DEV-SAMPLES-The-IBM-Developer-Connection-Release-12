/*******************************************************************/
/* LISTSRV.CMD Enumerate a server type. Values used are specified  */
/* below. Combination values are allowed. The resulting server     */
/* list contains all servers matching each of the combination      */
/* values. A server may therefore be in the list multible times.   */
/*                                                                 */
/* SV_TYPE_WORKSTATION        1 Workstation                        */
/* SV_TYPE_SERVER             2 Server                             */
/* SV_TYPE_SQLSERVER          4 SQL server                         */
/* SV_TYPE_DOMAIN_CTRL        8 Domain controller                  */
/* SV_TYPE_DOMAIN_BAKCTRL    16 Backup domain controller           */
/* SV_TYPE_TIME_SOURCE       32 Time server                        */
/* SV_TYPE_AFP               64 Apple** File Protocol service      */
/* SV_TYPE_NOVELL           128 Novell** service                   */
/* SV_TYPE_ALL               -1 All types of servers               */
/*******************************************************************/

parse upper arg serverType dummyArg

if serverType = '' then do
 say 'Use the following values to list servers matching matching'
 say
 say '  1 Workstation'
 say '  2 Server'
 say '  4 SQL server'
 say '  8 Domain controller'
 say ' 16 Backup domain controller'
 say ' 32 Time server'
 say ' 64 Apple** File Protocol service'
 say '128 Novell** service'
 say ' -1 All types of servers'
 say
 exit 0
end

call LoadFuncs; /* Load DLL utility functions */

rc = NetEnum('servers',,'srvList', serverType)

if rc = '0' then do
 if srvList.0 = 0 then do /* No server found */
  say
  say 'None'
 end
 else do
  do i=1 to srvList.0
   say srvList.i
  end
 end
end
else do
 say 'Error occured. Returncode from NetEnum() was: ' rc
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