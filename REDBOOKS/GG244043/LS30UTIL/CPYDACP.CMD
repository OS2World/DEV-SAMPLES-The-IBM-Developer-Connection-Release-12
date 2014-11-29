/*******************************************************************/
/* CPYDACP.CMD                                                     */
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
/*  sourceDir       The source directory of the operation          */
/*                                                                 */
/*  destinationDir  The destination directory of the operation     */
/*                                                                 */
/*******************************************************************/

parse upper arg option sourceDir destinationDir

call LoadFuncs; /* Load DLL utility functions */

/* Must have all arguments with values */
if ((option='') | (sourceDir='') | (destinationDir='')) then do
 call dspHelp;
 exit 9
end

/* If anything else than 'D' and 'R' mae it a 'D' */
if ((option<>'D') & (option<>'R')) then
 option = 'D'

if option = 'D' then do
 file.0 = 1
 file.1 = sourceDir
end

if option = 'R' then
 call SysFileTree sourceDir||'\*.*', 'file', 'SDO'

if file.0 = 0 then do
 call DropFuncs
 exit 9
end

/* Have the directory specifications. Ready to do ACP copy */
do i=1 to file.0
 if length(sourceDir) = length(file.i) then do
  rc = CopyDirAcls(file.i, destinationDir)
 end
 else do
  rc=CopyDirAcls(file.i, destinationDir||substr(file.i,length(sourcepath)))
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

  say 'Use CPYDACP [D|R] sourceDir destinationDir'

return

