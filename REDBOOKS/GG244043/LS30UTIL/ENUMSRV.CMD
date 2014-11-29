/*******************************************************************/
/* ENUMSRV.CMD                                                     */
/*******************************************************************/
/* SV_TYPE_WORKSTATION        1 Workstation                   */
/* SV_TYPE_SERVER             2 Server                        */
/* SV_TYPE_SQLSERVER          4 SQL server                    */
/* SV_TYPE_DOMAIN_CTRL        8 Domain controller             */
/* SV_TYPE_DOMAIN_BAKCTRL    16 Backup domain controller      */
/* SV_TYPE_TIME_SOURCE       32 Time server                   */
/* SV_TYPE_AFP               64 Apple** File Protocol service */
/* SV_TYPE_NOVELL           128 Novell** service              */
/* SV_TYPE_ALL               -1 All types of servers          */

parse upper arg serverId dummy

call LoadFuncs; /* Load DLL utility functions */

rc = NetEnum('servers',,'abc', 1)

/* rc = NetEnum('servers','\\ls3srv02','abc','LS3DOM02') */

if rc = '0' then do
 do i=1 to abc.0
  say abc.i
 end
end
else do
 say 'Error occured. Returncode from NetEnum() was: ' rc
end

rc = NetEnum('servers','\\ls3srv02','abc')

if rc = '0' then do
 do i=1 to abc.0
  say abc.i
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