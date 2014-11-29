/*******************************************************************/
/* MOVEDA.CMD                                                      */
/*                                                                 */
/* Move directory alias directory definition.                      */
/* The arguments are:                                              */
/*  alias           The alias name                                 */
/*  newDir          New assigned directory for the alias           */
/*  serverName      The name of the server on which the resource   */
/*                  resides.                                       */
/*******************************************************************/

parse upper arg alias newDirectory serverName

call RxFuncAdd 'LoadLs30utFuncs', 'LS30UT', 'LoadLs30utFuncs'
call LoadLs30utFuncs

if (alias = '') | (newDirectory = '')) then
 say 'Usage: MOVDA aliasName NewPath [ServerName]'
 exit 9
end

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

/* Use Domain Controller name if no serverName was specified */
if serverName = '' then
 serverName = dcName

/* Just doit now */
rc = MoveDirAlias(dcName, alias, dcName, newDir)
say 'MoveDirAlias returned: ' rc

call DropLs30utFuncs
exit 0

