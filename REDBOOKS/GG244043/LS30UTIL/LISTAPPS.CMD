/*******************************************************************/
/* LISTAPPS.CMD  List applications defined in the current domain.  */
/*                                                                 */
/* The argument is the type of application to be listed. The       */
/* following values are supported:                                 */
/*                                                                 */
/*  DOS       List the names of all public DOS applications        */
/*  OS2       List the names of all public OS/2 applications       */
/*  PRIVATE   List all private applications with user IDs          */
/*  ALL       List all of above                                    */
/*                                                                 */
/*******************************************************************/

parse upper arg appType dummyArg

call LoadFuncs; /* Load DLL utility functions */

if appType = '' then do
 call dspHelp;
 exit 9
end

parse value GetDCName() with rc dcName
if ( (rc <> '0') | (dcName = '') ) then do
 say 'Could not obtain domain controller machine ID'
 exit 99
end

select
 when appType = 'DOS' then do
  rc = NetEnum('applications', dcName, 'appList', 'PUBLICDOS')
 end
 when appType = 'OS2' then do
  rc = NetEnum('applications', dcName, 'appList', 'PUBLICOS2')
 end
 when appType = 'PRIVATE' then do
  rc = NetEnum('applications', dcName, 'appList', 'PRIVATE')
 end
 when appType = 'ALL' then do
  rc = NetEnum('applications', dcName, 'appList', 'ALL')
 end
 otherwise
  say 'Unsupported option'
  call dspHelp;
  exit 9
end

if rc = '0' then do
 if appList.0 = 0 then do /* No apps found */
  say
  say 'No application found'
 end
 else do
  do i=1 to appList.0
   say appList.i
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

/*************************************************************/
/* Display help                                              */
/*************************************************************/
dspHelp:

 say 'Use LISTAPPS application_type'
 say
 say 'where application_type can be:'
 say
 say 'DOS       List the names of all public DOS applications'
 say 'OS2       List the names of all public OS/2 applications'
 say 'PRIVATE   List all private applications with user IDs'
 say 'ALL       List all of above'

return

