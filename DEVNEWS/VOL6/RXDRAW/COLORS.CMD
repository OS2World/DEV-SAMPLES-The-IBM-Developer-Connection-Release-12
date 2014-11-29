/* colors.cmd for rxdraw.exe */

szEnvironment = address()
if szEnvironment <> "RXDRAW" then do
  say "Expected RXDRAW environment."
  return
end

say 'Colors.cmd running...'

x = 0

do i = 0 to 15        /* there are 16 pre-defined colors */

  rc = Color( i )

  do j = 0 to 24           /* draw a 1/4" bar of color */
    rc = Move( x+j, 0 )
    rc = Line( x+j, 200 )
  end

  select                   /* pick a contrasting color to label it */
    when i = 0 | i = 6 then c = 7
    otherwise c = 0
  end

  rc = Move( x+2, 25 )   /* label the number of the color */
  rc = Color( c )
  rc = Text( i )

  x = x + 25
end
return


