/*******************************************************************/
/* CPYAACP.CMD                                                     */
/*                                                                 */
/* Copy directory related Access Control Profiles.                 */
/* The Access Control Profiles from the source directory are       */
/* copied and applied to the destination directory.                */
/* The arguments are:                                              */
/*                                                                 */
/*  option          The option can either be                       */
/*                  D Only the directory Access Control Profiles   */
/*                    is copyed to the destination directory       */
/*                                                                 */
/*                  R The directory Access Control Profiles of     */
/*                    the sourcer directory and all subdirectories */
/*                    are copied in recursive order. The realtive  */
/*                    subdirectory positions will be preserved     */
/*                                                                 */
/*  alias           The directory alias, from with the source      */
/*                  directory is extracted.                        */
/*                                                                 */
/*  destDir         The destination directory of the operation     */
/*                                                                 */
/*******************************************************************/

parse upper arg option alias destDir

call LoadFuncs; /* Load DLL utility functions */

/* Must have all arguments with values */
if ((option='') | (alias='') | (destDir='')) then do
 call dspHelp;
 exit 9
end

/* If anything else than 'D' and 'R' make it a 'D' */
if ((option<>'D') & (option<>'R')) then
 option = 'D'

/* Must have access to domain controller name */
parse value GetDCName() with rc dcName

if rc <> '0' then do
 say 'Could not obtain Domain Controller name'
 exit 9
end

if dcName = '' then do
 say 'Could not obtain Domain Controller name'
 exit 9
end

/* Get alias directory */
parse value QueryDirAliasPath(dcName, alias) with rc aliaspath

if ( (rc <> '0') & (aliaspath <> '') ) then do
 say 'Could not obtain alias directory'
 exit 9
end

if option = 'D' then do
 file.0 = 1
 file.1 = aliaspath
end

if option = 'R' then
 call SysFileTree aliaspath||'\*.*', 'file', 'SDO'

if file.0 = 0 then do
 call DropFuncs
 exit 9
end

/* Have the directory specifications. Ready to do ACP copy */
do i=1 to file.0
 if length(aliaspath) = length(file.i) then do
  rc=CopyDirAcls(file.i, destDir)
 end
 else do
  rc=CopyDirAcls(file.i, destDir||substr(file.i,length(aliaspath)))
 end
 if rc <> '0' then do
  say
  say 'CopyDirAcls() returned: ' rc
  say 'Source Directory:' file.i
 end
end

call DropFuncs
exit 0

/*************************************************************/
/* Load DLL Functions                                        */
/*************************************************************/
LoadFuncs:

 /* Load REXXUTIL.DLL functions */
 call RxFuncAdd SysLoadFuncs, RexxUtil, SysLoadFuncs
 call SysLoadFuncs

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

 /* Drop REXXUTIL.DLL functions */
 call SysDropFuncs

return

/*************************************************************/
/* Display help                                              */
/*************************************************************/
dspHelp:

  say 'Use CPYAACP [D|R] alias destiationDir'

return

