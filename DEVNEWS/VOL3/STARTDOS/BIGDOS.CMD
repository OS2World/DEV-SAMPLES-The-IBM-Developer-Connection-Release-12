/* creates a larger conventional memory DOS session */
parse arg szCommandComArgs

if 'STARTDOS' <> address()  then do
  say 'Expected STARTDOS environment'
  return 2
end

rc = AddDosSetting( 'DOS_HIGH=1'  )
rc = AddDosSetting( 'DOS_UMB=1'  )

/* the 12 blanks seem to be required by vdm mgr in os2 */
rc = AddDosSetting( 'VIDEO_MODE_RESTRICTION=CGA            '  )
return 0
