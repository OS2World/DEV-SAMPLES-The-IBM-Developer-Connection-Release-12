/*  */
Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

'@echo off'

RetCode = SysDeregisterObjectClass( "PWFolder");

if RetCode then
    say 'Uninstall successfully completed for PWFolder class'

say 'Re-boot NOW in order to release DLL'
'pause'
