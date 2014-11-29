/***********************************************************************/
/* OPENDOC SAMPLE INSTALL                                              */
/*                                                                     */
/* Purpose: Uses XCOPY to install all files from a sub directory       */
/*          into a user specified destination directory.               */
/*                                                                     */
/* Parameters: Options: any options (WPS or FROM)                      */
/*             Path:    full path to install from                      */
/***********************************************************************/
/*            (C) Copyright IBM Corporation 1994-1995                  */
/***********************************************************************/

/* MAP - add command line options for no copying of files */
parse upper arg Ctl.!Option Ctl.!InstallPath .

Ctl.!BDr = 'C:'
/* MAP - query boot drive (needed later) */
Ctl.!BDr = left(value('comspec',,'os2environment'), 2)
CDtemp   = directory()

/* MAP - added ability to reinstall from CD) */
if Ctl.!Option = 'FROM' then CDtemp = Ctl.!InstallPath

Ctl.!CDr = substr(CDtemp,1,3)
Ctl.!ProgName = 'OpenDoc Samples'
Ctl.!Source = CDtemp
Ctl.!Destination = Ctl.!BDr||substr(CDtemp,3)
Ctl.!FolderLocation = '<WP_DESKTOP>'
Ctl.!FolderTitle = 'OpenDoc for OS/2'
Ctl.!ExeFile = '\bin\docshell.exe'

trace 'o'
FolderFlag = 1

call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

call on halt
/* MAP - if Ctl.!BDr = '?' */
if Ctl.!Option = '?'
  then call Tell
Globals = 'Ctl.' opendocpath
Ctl.!Quiet = 0
opendocpath = Ctl.!Destination

/* Version Check -- If not WARP, exit     */
/*                                        */
if SysOS2Ver() < 2.3
  then call MyExit 304

call Initialize

if substr(Ctl.!BDr,2, 1) <> ':' | substr(Ctl.!CDr,2, 1) <> ':'
  then call MyExit 302


SpaceRequired = DirSize(Ctl.!Source'\*.*')
if \Ctl.!Quiet
  then call PromptDestination Ctl.!Destination SpaceRequired

/* MAP - added condition */
if Ctl.!Option <> 'WPS' then do

Resp = MakeDestination(Ctl.!Destination)
Ctl.!CurDir = directory()

temp = directory(Ctl.!Destination)
if TRANSLATE(temp) <> TRANSLATE(Ctl.!Destination)
  then call MyExit 303
call directory(Ctl.!CurDir)
call CopyFiles

/* MAP - added */
end

say 'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿'
say '³ These OpenDoc samples can be compiled and run using the OpenDoc Toolkit    ³'
say 'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ'
'pause'

call MyExit 0

exit

Initialize: procedure expose (Globals)
CurDir = directory()
return 0

CopyFiles: procedure expose (Globals)
/* '@ECHO OFF' */
if \Ctl.!Quiet
  then do
    say 'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿'
    say '³'center('Copying files for 'Ctl.!Progname'. Please wait...',76)'³'
    say 'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ'
  end /* Do */
do while queued() <> 0
  pull .
end /* do */

'XCOPY' Ctl.!Source'\*' Ctl.!Destination'\ /s /e >nul 2>nul'
Resp = rc
if Resp \= 0
  then call MyExit 301
if \Ctl.!Quiet
  then do
/* MAP - updated message */
    say 'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿'
    say '³'center('All files copied successfully.', 76)'³'
    say 'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ'
  end
return 0

PromptDestination: procedure expose (Globals)
parse arg path space_required
'@ECHO OFF'
do until answer = 'Y'
  'cls'
  say
  say 'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿'
  say '³                       The Developer Connection                             ³'
  say '³            'right(Ctl.!ProgName,20)' Installation                               ³'
  say '³                  (C) Copyright IBM Corporation 1995                        ³'
  say 'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ'
  say
  say 'The default installation path for 'Ctl.!ProgName' is 'path
  say
  if Ctl.!Option <> 'WPS' then
  do
    say Ctl.!ProgName 'installation requires' space_required 'bytes on your hard drive.'
    say
  end
  say 'To ONLY change the drive, enter d: (where d is the new drive letter)'
  say 'To ONLY change the path, enter \path (where path is the new path)'
  say
  say 'Enter a new destination for 'Ctl.!ProgName', hit enter for default path, or Q to QUIT:'

  tmppath = translate(linein(STDIN))
  if tmppath = 'Q'
    then call MyExit 300
  if tmppath = "" then tmppath='C:'
  colon =':'

  Ctl.!Destination = tmppath
  opendocpath = tmppath
  if tmppath \= ''
      then ppath = substr(tmppath,1,1)||colon
      else ppath = substr(path,1,1)||colon

  freebytes = FreeSpace(ppath)

  if (freebytes < space_required)
  then
     do
        if ( freebytes < 0 ) & Ctl.!Option <> 'WPS'
        then
        do
          if ( freebytes == -1 )
            then
             call MessageBox ('Drive '||ppath||' is not available.')
            else
             call MessageBox ('Drive '||ppath||' is write-protected.')
        end
        else
        do
           answer = 'Y'
           if Ctl.!Option <> 'WPS'
           then
           do
             string1 = 'Not enough space on drive '||ppath||' to hold the '||Ctl.!ProgName||' files.'
             string2 = Ctl.!ProgName 'requires' space_required 'bytes of free space.'
             say 'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿'
             say '³                                                                            ³'
             say '³'center(string1,76)'³'
             say '³'center(string2,76)'³'
             say '³                                                                            ³'
             say 'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ'
             say ' '
             say 'Hit enter to continue or Q to QUIT:'
             ans = translate(linein(STDIN))
             if ans = 'Q'
             then call MyExit 300
           end
        end
     end
  else
   do
     call SysFileTree tmppath||'\BIN\PARTS.DAT', 'filename', 'F'
     if filename.0 <> 0 & Ctl.!Option <> 'WPS'
     then
     do
       answer = 'N'
       string1 = 'A previous version of OpenDoc for OS/2 has already been installed.'
       string2 = 'Please delete the files or install in a different directory.'
       say ' '
       say 'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿'
       say '³                                                                            ³'
        say '³'center(string1,76)'³'
       if string2 <> ''
        then
        say '³'center(string2,76)'³'
       say '³                                                                            ³'
       say 'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ'
       say ' '
       'pause'
     end
     else
     do
       if tmppath \= ''
         then do
           if (length(tmppath)=2) & (substr(tmppath,2,1)=':') then
             tmppath = tmppath||substr(path,3)
           else if substr(tmppath,1,1)='\' then
             tmppath=substr(path,1,2)||tmppath
           say
           say tmppath' is the new path.  Is this correct (Yes/No)?'
           answer = translate(linein(STDIN))
           answer = substr(answer, 1, 1)

           do while answer \= 'Y' & answer \= 'N'
             say beep(450,150)'Please enter Y(es) or N(o)'
             answer = translate(linein(STDIN))
             answer = substr(answer, 1, 1)
           end /* do */

           parse value reverse(RxCadd.ProgInfo.4) with fname'\'.
           RxCadd.ProgInfo.4='EXE='tmppath'\'reverse(fname)
           if FolderFlag = 1
             then
              do
                place = LASTPOS('\', Ctl.!ExeFile)
                exeString = substr(Ctl.!ExeFile,place)
                Ctl.!ExeFile=tmppath||exeString
              end
           if right(tmppath,1) = '\'
             then tmppath = strip(tmppath, 'T', '\')
           Ctl.!Destination = tmppath
           opendocpath = tmppath
         end

         else
              return path
     end
   end
end /* Do */
return

MakeDestination: procedure expose (Globals)

parse arg subdir
dirty = 0
'@ECHO OFF'
do forever
  'mkdir 'subdir' >nul 2>nul'
  if rc = 0 | dirty = 1 | length(subdir) <= 2
    then return rc
    else do
      call MakeDestination(substr(subdir,1,(lastpos('\',subdir)-1)))
      Dirty = 1
    end  /* Do */
end /* do */
return rc

MyExit: procedure expose (Globals)
parse arg Resp

select
  when Resp = 300 & \Ctl.!Quiet
    then do
      call MessageBox 'Installation aborted by user.'
    end /* Do */
  when Resp = 301 & \Ctl.!Quiet
    then do
      msg = 'Installation failed when copying files to '||Ctl.!Destination||'.'
      call MessageBox (msg)
    end /* Do */
  when Resp = 303 & \Ctl.!Quiet
    then do
      call beep 250, 150
      msg = 'Could not create destination directory '||Ctl.!Destination
      call MessageBox (msg)
    end  /* Do */
  when Resp = 304 & \Ctl.!Quiet
    then do
      call beep 250, 150
      msg = 'Installation failed.  Your system is not running OS/2 Warp.'
      call MessageBox (msg)
    end  /* Do */
  otherwise
    nop
end  /* select */

exit Resp

/** DirSize - this procedure evaluates the size in bytes of directory dd\*.* ***/
DirSize: procedure
 arg dd
 stars = '\*.*'
 directory = dd
 size=0
 call SysFileTree directory, 'file', 'SB'

 do i=1 to file.0
   parse var file.i  17 file_size x
   size = size + file_size
 end

 return size

/** FreeSpace - this procedure returns the total free space in drive dd     **/
/**             If drive dd is not valid, it will return 0                  **/
FreeSpace: procedure
 arg dd

 type=DATATYPE(substr(dd,1,1),'M')
                                  /*** drive is a letter               ***/
 if type <> 1
   then return -1
                                  /*** second character is a colon     ***/
 if (substr(dd,2,1) <> ':')
   then return -1

 info = SysDriveInfo(dd)
 if info = ''
        then return -1
 parse var info letter free total

                                  /*** drive is not write protected ?  ***/
 result = WriteDrive(dd)
 if result <> 0
   then free = -2

 return free

halt:

call MyExit 300
return

MessageBox: Procedure
  parse arg string1 ',' string2

  'cls'
  say ' '
  say 'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿'
  say '³                                                                            ³'
  say '³'center(string1,76)'³'
  if string2 <> ''
    then
  say '³'center(string2,76)'³'
  say '³                                                                            ³'
  say 'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ'
  say ' '

'pause'

return

/**************************************************************************/
/******   WriteDrive procedure - It tests if a drive is write        ******/
/******   protected - Returns 0 if it is not, returns 1 otherwise    ******/
/**************************************************************************/
WriteDrive: procedure

 arg drivenumber

filename=substr(drivenumber,1,1)||":\xyz0_zyx.1_2"

array = "0123456789abcdefghijklmnopqrstuvwxyz"

len = LENGTH(array)
switch =1

i=0
index=4

do while ( switch = 1 )

 if lines(filename) == 0
    then
      switch = 0
    else
      do
        i = i+1
        if ( i = (len-1) )
          then
           do
             i=1
             index=index+1
           end
        char = substr(array, i, 1)
        filename = OVERLAY(char, filename, index)
      end
end

result = lineout(filename,"xxx")

if ( result == 0 )
  then
    do
      result = stream(filename, 'c', close)
      'del' filename '1>&2>nul'
      rc = 0
    end
  else
    do
      rc = 1
    end

return rc

/* MAP - added help function */
Tell:
  say 'This is the Developer Connection install program for Opendoc Sample Code.'
exit
return
