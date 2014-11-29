/*  */
Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

'@echo off'

RetCode = SysCreateObject( "PWFolder", "My Lockable Folder", "<WP_DESKTOP>", "PASSWORD=wps;OBJECTID=<MyFolder>")

if RetCode then
   say 'PWFolder Object created'
else do
   say 'Error creating object'
   exit(1)
   end

RetCode = SysCreateObject( "WPProgram", "Editor", "<MyFolder>", "PROGTYPE=PM;EXENAME=\OS2\E.EXE;")

say 'A Folder has been created with the password: wps'
say 'select the context menu option LOCK to lock it'
say 'Switch to PM to test object.'

