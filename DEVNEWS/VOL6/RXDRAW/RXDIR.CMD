/* rxdir.cmd for rxdraw.exe */

szEnvironment = address()
if szEnvironment <> "RXDRAW" then do
  say "Expected RXDRAW environment."
  return
end

say 'RXDIR.CMD starting...'

/* add REXXUTIL functions */
rc = rxfuncadd('sysloadfuncs', 'rexxutil', 'sysloadfuncs' )
if rc <> 1 then do
  say 'Expected REXXUTIL.DLL in your LIBPATH.'
  signal halt
end
rc = sysloadfuncs()

rc = SysFileTree(  '*', aFile, 'F' )
if rc <> 0 then do
  say 'Unexpected SysFileTree rc' rc
  signal halt
end

/* compute highest-needed y, then move down */
Leading = 20
y = aFile.0 * Leading  + 10
do i=1 to aFile.0
  rc = Move( 25, y )
  rc = Text( aFile.i )
  y = y - Leading
end
return


halt:
say 'Error exit.'
return
