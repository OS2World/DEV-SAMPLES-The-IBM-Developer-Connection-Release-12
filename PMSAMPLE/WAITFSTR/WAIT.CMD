/*---------------------------------------------------------------------*/
signal on ERROR
signal on FAILURE
signal on HALT
trace off
call RxFuncAdd 'SysLoadFuncs', 'REXXUTIL', 'SysLoadFuncs'
call SysLoadFuncs
Funct1='RXWAITFORSTRING'
call RxFuncDrop Funct1
if RxFuncQuery( Funct1 ) Then Call RxFuncAdd Funct1, 'WAITFSTR', Funct1
val = SysIni( , 'PM_ED_HOOKS', 'GREHOOK')
if val='ERROR:' then do
   say 'Please read installation WAITFSTR.DOC before'
   exit
end
say 'Now waiting for string . Case Sensitive !!! type "its  me" on any window'
say RxWaitForString('its'||' me',0)
Call RxFuncDrop Funct1
exit rc
FAILURE:
ERROR:
HALT:
Call RxFuncDrop Funct1
say "Error "rc  line sigl":"sourceline(sigl)
say errortext(rc)
Exit rc
