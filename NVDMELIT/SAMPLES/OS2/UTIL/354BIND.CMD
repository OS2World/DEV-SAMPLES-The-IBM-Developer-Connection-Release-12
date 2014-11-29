/* Command to bind NVDM/2 database for CSD install  */
Parse Arg dbname
If dbname='' then
   dbname='IBMNVDM2'
'SQLBIND A:\ANXCSCD1.BND' dbname
If RC=0 then
   Do
'SQLBIND A:\ANXCSCD3.BND' dbname
'SQLBIND A:\ANXCSCD4.BND' dbname
'SQLBIND A:\ANXCSCD5.BND' dbname
'SQLBIND A:\ANXCSCD6.BND' dbname
'SQLBIND A:\ANXCSCD8.BND' dbname
'SQLBIND A:\ANXCSCD9.BND' dbname
'SQLBIND A:\ANXCSCDC.BND' dbname
'SQLBIND A:\ANXCSCDD.BND' dbname
'SQLBIND A:\ANXCSCDG.BND' dbname
'SQLBIND A:\ANXCSCDJ.BND' dbname
'SQLBIND A:\ANXCSCDL.BND' dbname
'SQLBIND A:\ANXCSCDN.BND' dbname
'SQLBIND A:\ANXCSCDP.BND' dbname
'SQLBIND A:\ANXCSCDR.BND' dbname
   end /* do */
Exit
