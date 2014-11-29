/*******************************************************************/
/* DELUSR.CMD                                                      */
/*                                                                 */
/* Delete users as specified in the argument ACSII file.           */
/*                                                                 */
/* Arguments:                                                      */
/*  users_list_file A file containing userids that will be         */
/*                  transferred                                    */
/*                                                                 */
/*******************************************************************/

parse upper arg usrListFile

if usrListFile = '' then do
 say 'Specify a user list file'
 exit 9
end

/* Ready to read user list file */
userid = linein(usrListFile,1,1)
if userid = '' then do
 say 'Could not obtain User List File'
 exit 9
end

do while userid <> ''
 'NET USER ' userid '/DELETE '
 userid = linein(usrListFile,,1)
end

exit 0

