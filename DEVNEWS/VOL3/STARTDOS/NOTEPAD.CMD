/* start notepad.exe in a common seamless vdm */
parse source . . szRexxFile .

select
  when 'CMD' = address() then do
    'pmsd' szRexxFile
    if rc > 0 then do
      say 'Problem finding/running pmsd.exe'
      return 2
    end
  end
  when 'STARTDOS' = address() then do
    rc = startseamless( 18, "notepad.exe" )
  end
  otherwise do
    say 'Unexpected environment.'
    return 4
  end
end

return 0
