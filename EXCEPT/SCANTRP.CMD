/**********************************************************************/
/*                                                                    */
/*  ScanTrp                                                           */
/*                                                                    */
/* CMD scanning the .TRP files for most possible call chains in       */
/* case EBP=0 or has been corrupted                                   */
/* is works only for 32 bit programs                                  */
/**********************************************************************/
/* Version: 1.0             |   Marc Fiammante (FIAMMANT at LGEPROFS) */
/*                          |   La Gaude FRANCE                       */
numeric digits 12
arg trpfile .
if trpfile=''| trpfile='?'|trpfile='-?'|trpfile='/?' then do
   say 'Scans a trapfile for probable stack call chains when Ebp'
   say 'has been corrupted or is invalid'
   say 'Syntax is : Scantrp trapfile'
   say "Output is standard output use '>' to redirect to a file"
   exit
end
if  stream(trpfile,'C','QUERY EXIST')='' then do
   say 'Problem opening file 'trpfile
end
/* get ESP */
do while lines(trpfile)
   line =linein(trpfile)
   parse value line with . 'ESP :' esp .
   if esp<>'' then leave
end
say 'Starting scan at ESP :' esp
if  stream(trpfile,'C','CLOSE')<>'READY:' then do
  say 'Problem closing file' trpfile
end
stack.=''
dll.=''
dll.0=0
/* Get DLL addresses */
dllname=''
do while lines(trpfile)
   line =linein(trpfile)
   if strip(line)="/*----- Stack Bottom ---*/" then leave
   parse value line with 'DLL' name .
   if name<>'' then do
      parse value name with base '.' extent
      dllname=name
   end
   else if extent='DLL' then do
      parse value line with Nbr Addr .
      if datatype(Nbr,'X')&Nbr<>'' then do
          dll.Addr=DllName Nbr
      end
   end
end
/* Parse stack until esp line      */
w.='0000'
do while lines(trpfile)
   line =linein(trpfile)
   parse value line with addr ':' w.1 w.2 w.3 w.4 w.5 w.6 w.7 w.8 .
   if w.1='' then iterate
   addr=strip(addr)
   if x2d(substr(addr,1,7))<x2d(substr(esp,1,7)) then iterate
   leave
end
/* Parse stack after esp line      */
chain.='00000000'
stkwords=0
do while lines(trpfile)
   line =linein(trpfile)
   parse value line with .    ':' w.9 reste
   if w.9='' then iterate
   addr=substr(strip(addr),1,7)
   addr.1=addr || '0'
   addr.2=addr || '2'
   addr.3=addr || '4'
   addr.4=addr || '6'
   addr.5=addr || '8'
   addr.6=addr || 'A'
   addr.7=addr || 'C'
   addr.8=addr || 'E'
   do  i=1 to 8
      addr=addr.i
      j=i+1
      chain.addr=w.j || w.i
      base.addr = w.j || '0000'
      stkwords=stkwords+1
      stkword.stkwords=addr
   end
   parse value line with addr ':' w.1 w.2 w.3 w.4 w.5 w.6 w.7 w.8 .
end
stktop=addr.8
dstktop=x2d(stktop)
dstkbot =x2d(esp)
do i=1 to stkwords
   addr=stkword.i
   newaddr=chain.addr
   do j=0 while x2d(newaddr)>dstkbot & x2d(newaddr)<dstktop
      if j=0 then do
         say '----------------------------------------'
         say '--one chain found ----------------------'
         say ' stack addr  : possible function address '
         tmpaddr=right(d2x(x2d(addr)+4),8,'0')
         fctaddr=chain.tmpaddr
         objaddr=substr(fctaddr,1,4) || '0000'
         object=''
         do k=0 while x2d(objaddr)>x2d(010000)
             if Dll.objaddr<>'' then do
                object=Dll.objaddr
                leave k
             end
             objaddr=right(d2x(x2d(objaddr)-x2d(010000)),8,'0')
         end
         offset=right(d2x(x2d(fctaddr)-x2d(objaddr)),8,'0')
         say '   ' addr   ':' fctaddr object 'Offset:' offset
      end
      object=''
      tmpaddr=right(d2x(x2d(newaddr)+4),8,'0')
      fctaddr=chain.tmpaddr
      objaddr=substr(fctaddr,1,4) || '0000'
      do k=0 while x2d(objaddr)>x2d(010000)
          if Dll.objaddr<>'' then do
             object=Dll.objaddr
             leave k
          end
          objaddr=right(d2x(x2d(objaddr)-x2d(010000)),8,'0')
      end
      offset=right(d2x(x2d(fctaddr)-x2d(objaddr)),8,'0')
      say '   ' newaddr   ':' fctaddr object 'Offset:' offset
      if addr=newaddr then do
        leave j
      end
      chain.addr='00000000'
      addr =newaddr
      newaddr=chain.addr
   end
/*   chain.addr base.addr */
end

/* Close trap file ***/
if  stream(trpfile,'C','CLOSE')<>'READY:' then do
  say 'Problem closing file' trpfile
end
