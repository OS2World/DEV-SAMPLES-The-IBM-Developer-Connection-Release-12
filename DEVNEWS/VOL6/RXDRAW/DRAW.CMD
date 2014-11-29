/* rxdraw.cmd, a REXX program for RXDRAW.EXE           */
/* call a box-drawing subroutine for each of 16 colors */

/* trace r shows execution results; good for debugging */
/* trace r */

/* this REXX program expects RXDRAW.EXE to be the host environment */
szEnvironment = address()
say 'Environment is' szEnvironment
if szEnvironment <> "RXDRAW" then do
  say "Expected RXDRAW environment."
  return 1
end

say 'draw.cmd starting...'


/* Start drawing at x,y; there are 100 points per inch. */
x = 50
y = 50

do i = 0 to 15
  /* set color and draw a box (color 0 is white) */
  call Color i
  call Box x, y, x+100, y+100

  /* draw some text in the current color */
  call move x+110, y
  call text 'Color' i

  /* adjust drawing point */
  x = x + 10
  y = y + 15
end
return


/*------------------------------------------------------*/
/* Box-drawing subroutine                               */
/* Parameters are x,y lower-left and x,y upper-right    */
Box:
x1 = arg(1)
y1 = arg(2)
x2 = arg(3)
y2 = arg(4)

rc = Move( x1, y1 )
rc = Line( x2, y1 )
rc = Line( x2, y2 )
rc = Line( x1, y2 )
rc = Line( x1, y1 )
return
