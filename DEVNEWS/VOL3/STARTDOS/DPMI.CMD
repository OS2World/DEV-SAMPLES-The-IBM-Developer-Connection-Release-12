/* turns DPMI on  */

if 'STARTDOS' <> address()  then do
  say 'Expected STARTDOS environment'
  return 2
end

rc = AddDosSetting( "DPMI_DOS_API=ENABLED" );
rc = AddDosSetting( "DPMI_MEMORY_LIMIT=8" );

return 0
