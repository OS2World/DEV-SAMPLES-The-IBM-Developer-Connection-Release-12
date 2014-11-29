/*  */
Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

say 'Please enter Boot drive letter'
pull boot
dll = substr(boot,1,1) || ':\os2\dll'

'copy PWFolder.dll' dll

if rc then do
   say 'Error DLL could not be updated please re-boot'
   /* Remove bad entry */
   RetCode = SysDeregisterObjectClass( "PWFolder");
   'pause'
   exit(1)
   end

RetCode = SysRegisterObjectClass( "PWFolder", "pwfolder")

if RetCode then
   say 'PWFolder Class registered'
else do
   say 'Error PWFolder Class failed to register'
   /* Remove false entry */
   RetCode = SysDeregisterObjectClass( "PWFolder");
   exit(1)
   end

