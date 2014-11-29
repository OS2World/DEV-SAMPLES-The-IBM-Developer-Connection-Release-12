/*************************************************************************/
/* Licensed Materials - Property of IBM                                  */
/*                                                                       */
/* XXXX-XXX                                                              */
/*                                                                       */
/* (C) Copyright IBM Corp. 1995.  All Rights Reserved.                   */
/*                                                                       */
/* US Government Users Restricted - Use, duplication, or                 */
/* disclosure restricted by GSA ADP Schedule Contract with IBM Corp.     */
/*                                                                       */
/*************************************************************************/

/* First get addressability to the required source directories and target workstation drives */

Call RxFuncAdd SysLoadFuncs, RexxUtil, SysLoadFuncs
Call SysLoadFuncs
/* Replaced following with technique from Address.cmd */
/*  call RxFuncAdd 'GetBootDrive', 'RLANUTIL', 'GetBootDrive' */

Parse upper arg logname CIDPath TargDrv

msgfile = 'OSCINST.MSG'

If CIDPath = '' then CIDPath = 'X:\'
/* If GrpPath = '' then do   */
/*   err1.1 = SysGetMessage(201, msgfile) */
/*   err1.2 = ''      */
/*   call errout 201  */
/*  end    (Do) */

/* *** Following four lines replace getbootdrive **** */
dpathstr = value("DPATH",,"OS2ENVIRONMENT")
dpathstr = translate(dpathstr)
parse var dpathstr first '\OS2\' .
os2drv = right(first,2)
/* os2drv = GetBootDrive() */
CFG_SYS = os2drv'\CONFIG.SYS'

A=0
Do Until Lines(CFG_SYS) = 0
  A = A+1
  CFG_SYS.A = LineIn(CFG_SYS)
End
Call Stream CFG_SYS, 'C', 'CLOSE'

/* Scan for Netware indicator in config.sys */
nwpath = ''
do i = 1 to A
   parse upper var CFG_SYS.i kw'='kwval
   if kw = 'DEVICE' & pos('\LSL.SYS',kwval) > 0 then do /*Netware found in c.sys*/
      nwpath = strip(left(kwval,pos('\LSL.SYS',kwval)-1)) /*Install here even if  */
      nwdrv = left(nwpath,2)               /*  different from user selection */
   end  /* Do */
end


/* For now do not try to upgrade existing installed Netware */
if nwpath \= '' then do
   err1.1 = SysGetMessage(209, msgfile)
   err1.2 = ""
   call errout 209
end  /* Do */

/* Setup NetWare drive and path */
if nwpath = '' then do                        /* Netware not installed here */
/*  drvfil = GrpPath'DRIVES.RSP'                (Use drive selected by user) */
/*  do while lines(drvfil) \= 0                */
/*     this =linein(drvfil)                    */
/*     if lastpos('NWDrv', this) \= 0 then do  */
/*        parse var this .'='nwdrv':'.         */
/*     end */
/*  end  (do) */
  nwdrv=TargDrv
  nwdrv =strip(nwdrv)||':'                    /* Drive selected by user */
  nwpath = nwdrv||'\NETWARE'
/*  call lineout drvfil */
end  /* do */


/* Save the user's Config.sys and load the NetWare image on workstation */
"@Copy" CFG_SYS   os2drv'\CONFIG.NVL  1>nul 2>nul'
 'XCOPY 'CIDPath'IMG\NWREQ\NETWARE 'nwpath'\/S/E/V'

/* Check for NET.CFG file, Don't change an existing one */
net_cfg = SysSearchPath('DPATH', 'NET.CFG')
If net_cfg = '' then do  /* No existing NET.CFG. Install a new default one */
   '@COPY 'CIDPath'IMG\NWREQ\NET.CFG 'os2drv'\NET.CFG  1>nul 2>nul'
   end


/* See if WINOS2 is installed */
winpath = os2drv||'\os2\mdos\winos2'
wininst = 'NO'
if stream(winpath'\win.com','C','query exists') \= '' then wininst = 'YES'


/* Get the additional NetWare config.sys statements */
B=0
NVL_SYS = CIDPath'IMG\NWREQ\NETWARE.DAT'
Do Until Lines(NVL_SYS) = 0
  B = B+1
  NVL_SYS.B= LineIn(NVL_SYS)
End
Call Stream NVL_SYS, 'C', 'CLOSE'


/* Substitute real drives for parameterized drives in NetWare statements */
do i = 1 to B
   if lastpos('&NWPath', NVL_SYS.i) \= 0 then do
      t1 = NVL_SYS.i
      parse var t1 f1'&NWPath'l1
      NVL_sys.i = f1||nwpath||l1
   end  /* Do */
   else if lastpos('&OS2Drv', NVL_SYS.i) \= 0 then do
      t1 = NVL_SYS.i
      parse var t1 f1'&OS2Drv'l1
      NVL_sys.i = f1||os2drv||l1
   end  /* Do */
end /* do */


/* Modify the config.sys file with the NetWare statements */
C=0
VemmFound = 'NO'
BindFound = 'NO'
Do i = 1 to A
  C = C+1
  CFG_SYS.i = Strip(CFG_SYS.i,T)
  Parse Var CFG_SYS.i CFG_Stmt '=' MIDDLE 'OS2\' STMT

  If Strip(CFG_Stmt) =  'LIBPATH'   Then Do
     If Right(CFG_SYS.i,1) \== ';' Then CFG_SYS.i = CFG_SYS.i || ';'
     CFG_SYS.i = CFG_SYS.i ||nwpath';'nwpath'\NLS\ENGLISH;L:\OS2;P:\OS2;'
     End
  If Strip(CFG_Stmt) =  'SET PATH'  Then Do
     If Right(CFG_SYS.i,1) \== ';' Then CFG_SYS.i = CFG_SYS.i || ';'
     CFG_SYS.i = CFG_SYS.i ||nwpath';L:\OS2;P:\OS2;'
     End
  If Strip(CFG_Stmt)=   'SET DPATH' Then Do
     If Right(CFG_SYS.i,1) \== ';' Then CFG_SYS.i = CFG_SYS.i || ';'
     CFG_SYS.i = CFG_SYS.i ||nwpath';'nwpath'\NLS;'nwpath'\NLS\ENGLISH;L:\NLS;P:\NLS;'
     End
  If Strip(CFG_Stmt) =  'SET HELP'  Then Do
     If Right(CFG_SYS.i,1) \== ';' Then CFG_SYS.i = CFG_SYS.i || ';'
     CFG_SYS.i = CFG_SYS.i ||nwpath'\NLS\ENGLISH;'
     End

  If Lastpos('VEMM.SYS',STMT) \= 0 & VemmFound = 'NO' & wininst = 'YES' then do
         /* Insert the NetWare Vipx & Vshell statements ahead of the        */
         /*  DEVICE=d:\OS2\MDOS\VEMM.SYS   statement.                       */
     VemmFound = 'YES'
     Do j = 1 to 2
       New_CFG.C = NVL_SYS.j
       C=C+1
     End
  End  /* if laspos */

  If Lastpos('NETBIND.EXE',MIDDLE) \= 0 & BindFound = 'NO' then do
         /* Insert the NetWare config.sys statements ahead of the           */
         /*  \IBMCOM\PROTOCOL\NETBIND.EXE  statement.                       */
     BindFound = 'YES'
     Do j = 4 to B
       New_CFG.C = NVL_SYS.j
       C=C+1
     End
  End  /* if laspos */

  New_CFG.C = CFG_SYS.i
End /* of do to modify config.sys */

/* Write out the new config.sys */
K=1
Call Lineout CFG_SYS,,1
Do Until New_CFG.k =  'NEW_CFG.' ||k
     Call LineOut CFG_SYS, New_CFG.k
     K=K+1
End
Call Stream CFG_SYS, 'c' , "Close"


/* Add the hidden install file to the root of the OS/2 drive */
'@'CIDpath'exe\os2\attrib -h 'os2drv'\nwinst.dat 1>nul 2>nul'
nwinstin = LineIn(CIDPath||'IMG\NWREQ\nwinst.dat')
Call Stream CIDPath||'IMG\NWREQ\nwinst.dat', 'C', 'CLOSE'
nwinstout = Translate(nwpath)||Right(nwinstin, length(nwinstin)-length(nwpath))
'@del 'os2drv'\nwinst.dat 1>nul 2>nul'
Call Charout os2drv'\nwinst.dat', nwinstout
Call Charout os2drv'\nwinst.dat'
'@'CIDpath'exe\os2\attrib +h 'os2drv'\nwinst.dat 1>nul 2>nul'


/* If WINOS2 is installed finish the installation of Windows support */
If wininst = 'YES' then do
  'XCopy 'CIDpath'IMG\NWREQ\SYSTEM  'OS2drv'\OS2\MDOS\WINOS2\SYSTEM\ /V'


  /* Update the Autoexec.bat with NetWare statements */
  autox = OS2drv||'\autoexec.bat'
  autox.1 = OS2drv||'\OS2\MDOS\WINOS2\SYSTEM\TBMI2.COM'
  autox.2 = nwpath||'\NETX.EXE'
  AA=2
  Do Until Lines(autox) = 0
    AA = AA+1
    autox.AA = LineIn(autox)
  End
  Call Stream autox, 'C', 'CLOSE'

  /* Write the new Autoexec.bat */
  K=1
  Call Lineout autox,,1
  Do Until autox.k =  'AUTOX.'||k
       Call LineOut autox, autox.k
       K=K+1
  End
  Call Stream autox, 'C', "Close"


  /* Update \WINOS2\SYSTEM.INI for NetWare support */

  /* First read in System.ini */
  sys_ini = winpath||'\system.ini'
  A=0
  Do Until Lines(sys_ini) = 0
    A = A+1
    sys_ini.A = LineIn(sys_ini)
  End
  Call Stream sys_ini, 'C', 'CLOSE'


  /* Now insert the NetWare stuff in the appropriate sections of system.ini */
  C=1
  Do i = 1 to A
    new_sys.C = sys_ini.i
    shout = Translate(sys_ini.i)
    C = C+1
    If Lastpos('[BOOT]',shout) \= 0 then do
      new_sys.C = 'network.drv=netware.drv'
      C = C+1
      End
    If Lastpos('[BOOT.DESC',shout) \= 0 then do
      new_sys.C = 'network.drv=Novell NetWare'
      C = C+1
      End
    If Lastpos('[386ENH]',shout) \= 0 then do
      new_sys.C = 'FileSysChange=off'
      C = C+1
      End
  End /* Do */

  /* Write modified system.ini */
  K=1
  Call Lineout sys_ini,,1
  Do Until new_sys.k =  'NEW_SYS.' ||k
       Call LineOut sys_ini, new_sys.k
       K=K+1
  End
  Call Stream sys_ini, 'C' , "Close"

end /* wininst = YES then do */


call nvldsk

err1.1 = SysGetMessage(215, msgfile, 'Novell Update')
err1.2 = ''
call errout 0

RETURN 0

nvldsk:
/* This procedure makes the Netware desktop folder and its included icons */
/*
Call RxFuncAdd SysLoadFuncs, RexxUtil, SysLoadFuncs
Call SysLoadFuncs  */
'@Echo Off'

/* Now create the Netware folder and place objects in it */
Classname= 'WPFolder'
Title =   'Novell'
Location =  '<WP_DESKTOP>'
Setup = "OBJECTID=<NOVELL_FOLDER>;ICONFILE="nwpath"\NVFLDR.ICO"
result=SysCreateObject(classname,Title,Location,Setup)

/* add the client program */
Classname= 'WPProgram'
Title =   'NetWare Client'
Location =  '<NOVELL_FOLDER>'
Setup = "OBJECTID=<NVL_CLIENT>;ICONFILE="nwpath"\NWOS2DOC.ICO;EXENAME="os2drv"\OS2\VIEW.EXE;PARAMETERS="nwpath"\NLS\ENGLISH\OS2BOOK.INF;STARTUPDIR="nwpath
result=SysCreateObject(classname,Title,Location,Setup)

/* add the utilities program */
Classname= 'WPProgram'
Title =   'NetWare Utils'
Location =  '<NOVELL_FOLDER>'
Setup = "OBJECTID=<NVL_UTILS>;ICONFILE="nwpath"\NWOS2DOC.ICO;EXENAME="os2drv"\OS2\VIEW.EXE;PARAMETERS="nwpath"\NLS\ENGLISH\OS2UTIL.INF;STARTUPDIR="nwpath
result=SysCreateObject(classname,Title,Location,Setup)

/* add the install program */
Classname= 'WPProgram'
Title =   'Install'
Location =  '<NOVELL_FOLDER>'
Setup = 'EXENAME='nwpath'\INSTALL.EXE;STARTUPDIR='nwpath';OBJECTID=<NVL_INSTALL> '
result=SysCreateObject(classname,Title,Location,Setup)

/* add the NetWare Printer program */
Classname= 'WPProgram'
Title =   'Network Printer'
Location = '<NOVELL_FOLDER>'
Setup = 'EXENAME='nwpath'\NPRINTER.EXE;STARTUPDIR='nwpath';OBJECTID=<NVL_NPRINTER> '
result=SysCreateObject(classname,Title,Location,Setup)

/* add the TSA program */
Classname='WPProgram'
Title =   'NetWare TSA'
Location = '<NOVELL_FOLDER>'
Setup = 'EXENAME='nwpath'\TSAOS2.EXE;STARTUPDIR='nwpath';OBJECTID=<NVL_TSA> '
result=SysCreateObject(classname,Title,Location,Setup)

/* add the Tools program */
Classname='WPProgram'
Title =   'NetWare Tools'
Location = '<NOVELL_FOLDER>'
Setup = 'EXENAME='nwpath'\NWTOOLS.EXE;STARTUPDIR='nwpath';OBJECTID=<NVL_TOOLS> '
result=SysCreateObject(classname,Title,Location,Setup)


Quit:
RETURN 0

errout:

if logname = '' then logname = 'C:\ERROR.OUT'
parse arg retc

if retc \= 0 then do
   say err1.1
   say err1.2
end  /* Do */

err1.0 = 2
rc =  stream(logname, 'C', 'QUERY EXISTS')
if rc \= '' then '@del' logname '1>nul 2>nul'

do i = 1 to err1.0
   call lineout logname, err1.i
end /* do */

call lineout logname

EXIT retc
