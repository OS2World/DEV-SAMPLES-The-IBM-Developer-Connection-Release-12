/* */
call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

parse arg libdir

/* change following drive letter to your boot drive */
if (libdir = "") then libdir = "C:\os2\dll"

'copy dragexam.dll' libdir
'copy dragexam.sym' libdir
'copy dragfold.dll' libdir
'copy dragfold.sym' libdir
'copy myfolder.dll' libdir
'copy myfolder.sym' libdir

if SysCreateObject("DRAGEXAM","Drag Example","<WP_DESKTOP>") then
   say 'Drag Example successfully installed.'
else
   say 'Error, Drag Example not installed properly.'

if SysCreateObject("DRAGFOLD","Drag Folder","<WP_DESKTOP>") then
   say 'Drag Folder successfully installed.'
else
   say 'Error, Drag Folder not installed properly.'

if SysCreateObject("MYFOLDER","My Folder","<WP_DESKTOP>") then
   say 'My Folder successfully installed.'
else
   say 'Error, My Folder not installed properly.'


