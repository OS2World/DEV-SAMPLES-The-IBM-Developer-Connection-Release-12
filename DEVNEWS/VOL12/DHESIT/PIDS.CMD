/* PIDS.CMD

REXX utility to parse PSTAT.EXE output and show just process IDs and
EXE names.

Expects a TMP environment variable.


*/


szTmp = value( 'TMP',,'OS2ENVIRONMENT' )
if length( szTmp ) = 0 then do
  say 'Sorry, expected a TMP environment variable.'
  signal halt
end

/* file name where pstat can write */
szInput = szTmp'\'pstat.out

'pstat >'szInput

szRC = stream( szInput, 'C', 'OPEN READ' )
if szRC <> 'READY:' then signal halt


do while lines( szInput )

  szLine = linein( szInput )

  if 8 = words( szLine ) then do
    say word( szLine, 1 )  word( szLine, 4 )
  end

  if pos( 'Semaphore Information', szLine ) > 0  then leave
end

szRC = stream( szInput, 'C', 'CLOSE' )
if szRC <> 'READY:' then signal halt

return


halt:
say 'Error exit.'
return
