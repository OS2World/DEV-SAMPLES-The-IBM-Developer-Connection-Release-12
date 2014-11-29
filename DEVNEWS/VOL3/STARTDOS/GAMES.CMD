/* for most games */

if 'STARTDOS' <> address()  then do
  say 'Expected STARTDOS environment'
  return 2
end

rc = AddDosSetting( "HW_TIMER=1" )

return 0
