/*******************************************************************/
/* PRINTDEV.CMD                                                    */
/*                                                                 */
/*******************************************************************/

parse upper arg serverId dummy

call LoadFuncs; /* Load DLL utility functions */

rc = NetEnum('printdev','\\ls3srv02','abc', 'STATUS')

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