/* zigzag.cmd for rxdraw.exe */

szEnvironment = address()
if szEnvironment <> "RXDRAW" then do
  say "Expected RXDRAW environment."
  return 1
end

say 'ZIGZAG.CMD running....'


y = 100
x =   0
rc = Move( x, y )

do i = 1 to 30

  if 0 = i // 2 then do
    y2 = y + 75
  end
  else do
    y2 = y - 75
  end

  rc = Line( x, y2 )

  x = x + 20
end
return

