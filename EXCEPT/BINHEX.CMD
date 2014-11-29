/******/
arg file .
if stream(file ,'C', open)<>'READY:' then exit
line=''
do while chars(file)>0
   char=charin(file)
   line=line || c2x(char)
   if length(line)>=72 then do
      call lineout "HEX.DAT", line
      line=''
   end
end
call lineout "HEX.DAT", line
if stream(file ,'C', close)<>'READY:' then exit
if stream("HEX.DAT" ,'C', close)<>'READY:' then exit
