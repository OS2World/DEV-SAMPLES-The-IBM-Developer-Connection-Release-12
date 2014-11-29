/* clock.cmd for rxdraw.exe */

szEnvironment = address()
if szEnvironment <> "RXDRAW" then do
  say "Expected RXDRAW environment."
  return
end

say 'clock.cmd'

rc = Move( 50, 250 )
rc = Text( "The date is" date() )

rc = Move( 50, 200 )
rc = Text( "The time is" time() )

return

