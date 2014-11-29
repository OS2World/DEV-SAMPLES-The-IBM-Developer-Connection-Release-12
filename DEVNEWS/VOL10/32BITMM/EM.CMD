/*  em.cmd: embeds, numbers, and indents files */
parse arg szLine

szFile = ''

n = words( szLine )
if 0 = n then do
  say 'Usage: em [/n] [/lm<n>] <file>'
  say '<file> may contain .em tag lines.'
  say '/n       number lines'
  say '/lm<n>   left margin of <n> column'
  signal halt
end

/* file to embed is named last */
szFile = word( szLine, n )

/* check for switches */
fNumber = 0
cbIndent = 0
do i = 1 to n
  if i < n then do
    szArg = translate( word( szLine, i ))
    select
      when szArg = '/N' then do
        /* number the line */
        fNumber = 1
      end
      when left( szArg, 3 ) = '/LM' then do
        /* indent left margin */
        szArg = right( szArg, length( szArg ) - 3 )
        if length( szArg ) > 0 then do
          cbIndent = szArg
        end
      end
      otherwise nop
    end
  end
end


rc = stream( szFile, 'C', 'OPEN READ' )
if rc <> 'READY:' then do
  say szFile 'not found.'
  signal halt
end

count = 0

do while lines( szFile )
  szLine = linein( szFile )
  count = count + 1

  if ".EM" = translate( left( szLine, 3 )) then do
    szCMD = "@call em"
    do i=2 to words( szLine )
      szCMD = szCMD word( szLine, i )
    end
    szCMD
  end
  else do
    if fNumber > 0 then do
      szLine = right( count, 5 )':' szLine
    end
    if cbIndent > 0 then do
      szLine = right( ' ', cbIndent )||szLine
    end
    say szLine
  end
end

rc = stream( szFile, 'C', 'CLOSE' )
if rc <> 'READY:' then signal halt

return 0



halt:
if length( szFile ) > 0 then rc = stream( szFile, 'C', 'CLOSE' )
return 2

