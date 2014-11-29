/*******************************************************************/
/* INSASN.CMD                                                      */
/*                                                                 */
/* Insert logon assignemts from a assignment list file.            */
/* The procedure sets the logon assignments for users and logon    */
/* assignments listed in a assignment file. The assignment file    */
/* can be generated as a result from the GETASN.CMD and ALLASN.CMD */
/* procedures. The format of the logon assignment input file is    */
/* for example:                                                    */
/*                                                                 */
/*                  userid= DEPT001                                */
/*                  D=   G:     FILESDIR                           */
/*                  A=          PUBLICAP                           */
/*                                                                 */
/*                  userid= DEPT002                                */
/*                  D=   O:     FILESDIR                           */
/*                  D=   COM7   COMPORT                            */
/*                  A=          PUBLICAP                           */
/*                                                                 */
/* The argument is:                                                */
/*                                                                 */
/*  assignListFile  The file name of the users assignments         */
/*                                                                 */
/* If an logon assignment is already set, a return code 85         */
/* will be received. (ERROR_ALREADY_ASSIGNED)                      */
/*                                                                 */
/*******************************************************************/

parse upper arg assignListFile dummy

call LoadFuncs; /* Load DLL utility functions */

/* Must have all arguments with values */
if userList='' then do
 say 'User assign list file is required for the insert assignments'
 say 'procedure.'
 exit 9
end

/* Ready to read assignment list file */
parse upper value linein(assignListFile,1,1) with type device aliasname
if LINES(assignListFile) = '0' then do
 say 'Could not obtain correct logon assignment list file'
 exit 9
end

do until LINES(assignListFile) = '0'

 if firstTime = 0 then do
  parse upper value linein(assignListFile) with type device aliasname
 end
 else do
  parse upper value linein(assignListFile,1,1) with type device aliasname
  firstTime = 0
 end

 select
  when type = 'USERID=' then do
   userid = device
  end
  when type = 'A=' then do
   if aliasname = '' then do
    aliasname = device
    device = ''
   end
   rc = SetLogonAsn(userid, type, device, STRIP(aliasname) )
   if rc <> '0' then do
    say 'SetLogonAsn returned ' rc
   end
  end
  when type = 'D=' then do
   if aliasname = '' then do
    aliasname = device
    device = ''
   end
   rc = SetLogonAsn(userid, type, device, STRIP(aliasname) )
   if rc <> '0' then do
    say 'SetLogonAsn returned ' rc
   end
  end
  otherwise
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