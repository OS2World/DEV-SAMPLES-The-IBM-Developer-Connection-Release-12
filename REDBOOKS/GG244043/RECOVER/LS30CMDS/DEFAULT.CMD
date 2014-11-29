/*******************************************************************/
/* REXX CID procedure for LAN CID Utility processing               */
/*                                                                 */
/* This procedure is called in front of the standard LAN CID       */
/* utility DEFAULT.CMD procedure. This procedure creates a CLIENT  */
/* procedure after user input has been obtained and chains the     */
/* created procedure.                                              */
/*                                                                 */
/* Options are:                                                    */
/* Restore Domain Related data files                               */
/* Reinstall OS/2 2.0 Service Package                              */
/* Reinstall OS/2 2.0 and add OS/2 2.0 Service Package             */
/* Reinstall OS/2 2.0, add Service Package and reinstall server    */
/* OS/2 Command Line for manual operations on server               */
/* New installation of OS/2, Service Package and LAN Server 3.0    */
/*                                                                 */
/* A switch is added for the 386 HPFS based server, which          */
/* requires special considerations                                 */
/*******************************************************************/

parse ARG client logfile additional

/********************************************************************/
/* Assignments used                                                 */
/********************************************************************/
d1 = "x:"                  /* The LAN CID utility base path         */
d2 = "y:"
d3 = "z:"                  /* The Log file path read/write          */
d4 = "w:"
bootdrive = "c:"
maintdrive = "c:"
exepath = d1"\exe"
maint_dir = maintdrive"\service"
ifs_dir = bootdrive"\srvifsrq"
dll_dirs = d1"\dll;"d1"\img\lcu;"
rsp_dir = d1"\rsp"
log_dir = d3
img_dir = d1"\img"
thisIsDone = 0                     /* Menu variable                 */

call resetstars;                   /* Reset all asterics markers    */
'cls'
/********************************************************************/
/* Begin of main loop                                               */
/********************************************************************/
do forever;
 call QUERYINPUT;                        /* Get user input */
 'cls'
 if thisIsDone = 1 then do               /* Break the loop */
  exit
 end

 if thisIsDone = 2 then do               /* Check for options */
  do i=1 to 6
   if star.i = '*' then do               /* Got an option */
    call performit;                      /* Process the option */
    exit
   end
  end
  thisIsDone = 0                         /* More to be done */
 end
end
exit

/********************************************************************/
/* Get input key                                                    */
/********************************************************************/
GETKEY:

KEY=''
DO WHILE KEY=''
 PARSE UPPER PULL KEY;
END
RETURN

/********************************************************************/
/* Query options                                                    */
/********************************************************************/
QUERYINPUT:
'cls'
say ' Select one or more of the options below:'
say
say ' Environment options: If 386 HPFS is used option A must be selected'
say
say star.0 'A Is OS/2 LAN Server 3.0 Advanced installed and using 386 HPFS?'
say
say ' Actions required:'
say
say star.1 '1 Restore Domain Related data files?'
say star.2 '2 Reinstall OS/2 2.0 Service Package'
say star.3 '3 Reinstall OS/2 2.0 and add OS/2 2.0 Service Package'
say star.4 '4 Reinstall OS/2 2.0, add OS/2 2.0 Service Package and reinstall server'
say star.5 '5 OS/2 Command Line for manual operations on server'
say star.6 '6 New installation of OS/2, Service Package and LAN Server 3.0'
say
say ' R Reset options'
say ' X Execute selected options'
say ' Q Quit, do not execute selected options'
say

CALL GETKEY;
SELECT
 WHEN KEY='A' THEN call setstars4a;
 WHEN KEY='1' THEN call setstars;
 WHEN KEY='2' THEN call setstars;
 WHEN KEY='3' THEN call setstars;
 WHEN KEY='4' THEN call setstars;
 WHEN KEY='5' THEN call setstars;
 WHEN KEY='6' THEN call setstars;
 WHEN KEY='R' THEN call resetstars;
 WHEN KEY='Q' THEN thisIsDone = 1;
 WHEN KEY='X' THEN thisIsDone = 2;
 OTHERWISE DO
  SAY ' Enter a valid option'
  'PAUSE'
 END
END
RETURN

/********************************************************************/
/* Build client cmd file from options provided                      */
/* CASPREP options                                                  */
/********************************************************************/
performit:

/* Set used variables for CASPREP install section */
casprp.inst.0 = 'semaint+laps_prep'
casprp.inst.1 = 'thinifs1+thinifs2+thinifs3+casinstl'
casprp.inst.2 = 'semaint+'
casprp.inst.3 = 'new_seinst+'
casprp.inst.4 = 're_seinst+'
casprp.inst.5 = 'LAPS'
casprp.inst.6 = 'laps_prep'
casprp.inst.7 = 'fservice'
casprp.inst.8 = 'ifsdel+casdelet'
casprp.inst.9 = 'sec386'
casprp.inst.10= 'thin386'
casprp.inst.11= 'entry_srv_reinst'
casprp.inst.12= 'entry_srv_instr'
casprp.inst.13= 'advanced_srv_reinst'
casprp.inst.14= 'advanced_srv_instr'
casprp.inst.15= 'xcopySdf+restNetAcc'
casprp.inst.16= 'cmdprompt'
casprp.inst.17= 'removeOS2Acls'
casprp.inst.18= 'removeAcls'
casprp.inst.19= 'restoreAcls'
casprp.inst.20= 'restoreAcls+ifsdel+casdelet'
casprp.inst.21= 're_seinst'
casprp.inst.22= 'new_seinst'
casprp.inst.23= 'as_srv_386hpfs+remarkStatement'
casprp.inst.24= 'remarkStatement'
error_string =  'Could not prepare' client 'procedure. Will reboot.'
casprp.add.0 = 'semaint+laps_prep+thinifs1+thinifs2+thinifs3+casinstl'
casprp.add.1 = 'semaint+thin386+laps_prep+thinifs1+thinifs2+thinifs3+casinstl'
casprp.add.2 = 'laps_prep+thinifs1+thinifs2+thinifs3+casinstl'
casprp.add.3 = 'sec386+laps_prep+thinifs1+thinifs2+thinifs3+casinstl'
casprp.add.4 = 'laps+thinifs1+thinifs2+thinifs3+casinstl'
casprp.add.5 = 'sec386+laps+thinifs1+thinifs2+thinifs3+casinstl'
casprp.add.6 = 'removeAcls+laps_prep+thinifs1+thinifs2+thinifs3+casinstl+'
casprp.add.7 = 'laps_prep+thinifs1+thinifs2+thinifs3+casinstl+'
casprp.add.8 = 'removeAcls+semaint+laps_prep+thinifs1+thinifs2+thinifs3+casinstl+'
casprp.add.9 = 'removeOS2Acls+semaint+laps_prep+thinifs1+thinifs2+thinifs3+casinstl+'
casprp.add.10= 'sec386+laps+thinifs1+thinifs2+thinifs3+casinstl'
casprp.add.11= 'laps+thinifs1+thinifs2+thinifs3+casinstl'
casprp.add.12= 'thin386+thinifs1+thinifs2+thinifs3+casinstl'
casprp.usrl.0 = 'userline= Call RebootAndGotoState(2)'

ef = d4 || '\ls30cmds\'    /* Short cut notification */
/* Vaidate that required files exist */
if stream(ef||'casprep.cmd','c','query exists') <> '' then do
 if stream(ef||'lsrdbase.fil','c','query exists') <> '' then do
  'copy ' ef||'lsrdbase.fil ' ef||client ||'.fil'

  /* Option 1, restore server data files */
  if star.1 = '*' then do        /* Restore server data files */
   if star.0 = '*' then do       /*   386 HPFS installed */
    add.lines.1=casprp.add.1||'+'/* 0 Install THIN386 */
                                 /*   Add thinifs statements + laps_prep */
    add.lines.2 = casprp.usrl.0  /*   Reboot and goto state 2 */
    add.lines.3 = casprp.add.12  /* 1 Install THIN386 + laps_prep + ifs */
    add.lines.4 = casprp.inst.15 /* 2 Restore server data files */
    add.lines.5 = casprp.inst.23 /* 3 reinstall server 386 hpfs */
    add.lines.6 = casprp.inst.8  /* 4 Remove cas */
    add.lines.0 = 6
   end
   else do                       /*   No 386 HPFS installed */
    add.lines.1 = casprp.add.2   /* 0 Add thinifs + laps_prep */
    add.lines.2 = casprp.inst.15 /* 1 Restore server data files */
    add.lines.3 = casprp.inst.8  /* 2 Remove cas */
    add.lines.0 = 3
   end

  end

  /* Option 2, reinstall OS/2 Service package */
  if star.2 = '*' then do
   if star.0 = '*' then do       /*   386 HPFS installed */
    add.lines.1=casprp.add.1||'+'/* 0 Install THIN386 */
                                 /*   Add thinifs statements + laps_prep */
    add.lines.2 = casprp.usrl.0  /*   Reboot and goto state 2 */
    add.lines.3 = casprp.add.12  /* 1 Install THIN386 + laps_prep + ifs */
    add.lines.4 = casprp.inst.7  /* 2 fservice */

    if star.1 = '*' then do      /*   Restore server data files */
     add.lines.5= casprp.inst.15 /* 3 restore server data files */
     add.lines.6= casprp.inst.23 /* 4 reinstall 386 hpfs server */
     add.lines.7= casprp.inst.8  /* 5 remove cas */
     add.lines.0= 7
    end
    else do
     add.lines.5= casprp.inst.23 /* 3 reinstall 386 hpfs server */
     add.lines.6= casprp.inst.8  /* 4 remove cas */
     add.lines.0= 6
    end

   end
   else do                       /*   No 386 HPFS selected */
    add.lines.1 = casprp.add.0   /* 0 se_maint+ thinifs statements */
    add.lines.2 = casprp.inst.7  /* 1 fservice */

    if star.1 = '*' then do      /*   Restore server data files */
     add.lines.3= casprp.inst.15 /* 2 Restore server data files */
     add.lines.4= casprp.inst.8  /* 3 Remove cas */
     add.lines.0 = 4
    end
    else do                      /*   Do not restore server data files */
     add.lines.3= casprp.inst.8  /* 2 Remove cas */
     add.lines.0 = 3
    end

   end
  end

  /* Option 3, reinstall OS/2 and add service package */
  if star.3 = '*' then do
   if star.0 = '*' then do       /*   If 386 HPFS server */
    add.lines.1 = casprp.add.1   /* 0 Install thin386 + se_maint */
                                 /*   Add thinifs statements + laps_prep */
                                 /* 1 re_seinst + sec386 */

    add.lines.2 = casprp.inst.4 || casprp.add.10

    add.lines.3 = casprp.add.1   /* 2 Install thin386 + se_maint */
                                 /*   Add thinifs statements + laps_prep */
    add.lines.4 = casprp.inst.7  /* 3 fservice */

    if star.1 = '*' then do      /*   Restore server data files */
     add.lines.5= casprp.inst.15 /* 4 Restore server data files */
     add.lines.6= casprp.inst.23 /* 5 advanced_srv 386 hpfs reinstall */
     add.lines.7= casprp.inst.8  /* 6 Remove cas */
     add.lines.0 = 7
    end
    else do                      /*   Do not restore server data files */
     add.lines.5= casprp.inst.23 /* 4 advanced_srv 386 hpfs reinstall */
     add.lines.6= casprp.inst.8  /* 5 Remove cas */
     add.lines.0 = 6
    end

   end
   else do                       /*   No 386 HPFS selected */
    add.lines.1 = casprp.add.0   /* 0 se_maint+ thinifs statements */
                                 /* 1 re_seinst + cas */
    add.lines.2 = casprp.inst.21 || '+' || casprp.add.11
    add.lines.3 = casprp.add.0   /* 2 make new maintenance SE */
    add.lines.4 = casprp.inst.7  /* 3 fservice */

    if star.1 = '*' then do      /*   Restore server data files */
     add.lines.5= casprp.inst.15 /* 4 Restore server data files */
     add.lines.6= casprp.inst.8  /* 5 Remove cas */
     add.lines.0 = 6
    end
    else do                      /*   Do not restore server data files */
     add.lines.5= casprp.inst.8  /* 4 Remove cas */
     add.lines.0 = 5
    end

   end
  end

  /* Option 4, reinstall OS/2, service package and LAN Server 3.0 */
  if star.4 = '*' then do
   if star.0 = '*' then do       /*   If 386 HPFS server */
    add.lines.1 = casprp.add.1   /* 0 Install thin386 + se_maint */
                                 /*   Add thinifs statements + laps_prep */
                                 /* 1 re_seinst + sec386 */

    add.lines.2 = casprp.inst.4 || casprp.add.10

    add.lines.3 = casprp.add.1   /* 2 Install thin386 + se_maint */
                                 /*   Add thinifs statements + laps_prep */
    add.lines.4 = casprp.inst.7  /* 3 fservice */

    if star.1 = '*' then do      /*   Restore server data files */
     add.lines.5= casprp.inst.15 /* 4 Restore server data files */
     add.lines.6= casprp.inst.13 /* 5 advanced_srv reinstall */
     add.lines.7= casprp.inst.8  /* 6 Remove cas */
     add.lines.0 = 7
    end
    else do                      /*   Do not restore server data files */
     add.lines.5= casprp.inst.13 /* 4 advanced_srv reinstall */
     add.lines.6= casprp.inst.8  /* 5 Remove cas */
     add.lines.0 = 6
    end

   end
   else do                       /*   No 386 HPFS selected */
    add.lines.1 = casprp.add.0   /* 0 se_maint+ thinifs statements */
                                 /* 1 re_seinst + cas */
    add.lines.2 = casprp.inst.21 || '+' || casprp.add.11

    add.lines.3 = casprp.add.0   /* 2 make new maintenance SE */
    add.lines.4 = casprp.inst.7  /* 3 fservice */

    if star.1 = '*' then do      /*   Restore server data files */
     add.lines.5= casprp.inst.15 /* 4 Restore server data files */
     add.lines.6= casprp.inst.11 /* 5 entry_srv_reinst */
     add.lines.7= casprp.inst.8  /* 6 Remove cas */
     add.lines.0 = 7
    end
    else do                      /*   Do not restore server data files */
     add.lines.5= casprp.inst.11 /* 4 entry_srv_reinst */
     add.lines.6= casprp.inst.8  /* 5 Remove cas */
     add.lines.0 = 6
    end

   end
  end


  /* Option 5, OS/2 Command Line */
  if star.5 = '*' then do
   if star.0 = '*' then do
    add.lines.1=casprp.add.1||'+'/* 0 Install thin386 + se_maint */
    add.lines.2 = casprp.usrl.0  /*   Reboot and goto state 2 */
    add.lines.3 = casprp.add.12  /* 1 thin386 + casinstl */
    add.lines.4 = casprp.inst.16 /* 2 command prompt */
    add.lines.5 = casprp.inst.23 /* 3 reinst 386 HPFS server */
    add.lines.6 = casprp.inst.8  /* 4 remove cas */
    add.lines.0 = 6
   end
   else do
    add.lines.1 = casprp.inst.16 /* 0 command prompt */
    add.lines.0 = 1
   end
  end

  /* Option 6, new install of all */
  if star.6 = '*' then do
   if star.0 = '*' then do       /*   If 386 HPFS server */
    add.lines.1 = casprp.add.1   /* 0 Install thin386 + se_maint */
                                 /*   Add thinifs statements + laps_prep */
                                 /* 1 new_seinst + sec386 */

    add.lines.2 = casprp.inst.3 || casprp.add.10

    add.lines.3 = casprp.add.1   /* 2 Install thin386 + se_maint */
                                 /*   Add thinifs statements + laps_prep */
    add.lines.4 = casprp.inst.7  /* 3 fservice */

    if star.1 = '*' then do      /*   Restore server data files */
     add.lines.5= casprp.inst.15 /* 4 Restore server data files */
     add.lines.6= casprp.inst.14 /* 5 advanced_srv_instr */
     add.lines.7= casprp.inst.8  /* 6 Remove cas */
     add.lines.0 = 7
    end
    else do                      /*   Do not restore server data files */
     add.lines.5= casprp.inst.14 /* 4 advanced_srv_instr */
     add.lines.6= casprp.inst.8  /* 5 Remove cas */
     add.lines.0 = 6
    end

   end
   else do                       /*   No 386 HPFS selected */
    add.lines.1 = casprp.add.0   /* 0 se_maint+ thinifs statements */
                                 /* 1 new_seinst + cas */
    add.lines.2 = casprp.inst.22 || '+' || casprp.add.11
    add.lines.3 = casprp.add.0   /* 2 make new maintenance SE */
    add.lines.4 = casprp.inst.7  /* 3 fservice */

    if star.1 = '*' then do      /*   Restore server data files */
     add.lines.5= casprp.inst.15 /* 4 Restore server data files */
     add.lines.6= casprp.inst.12 /* 5 entry_srv_instr */
     add.lines.7= casprp.inst.8  /* 6 Remove cas */
     add.lines.0 = 7
    end
    else do                      /*   Do not restore server data files */
     add.lines.5= casprp.inst.12 /* 4 entry_srv_instr */
     add.lines.6= casprp.inst.8  /* 5 Remove cas */
     add.lines.0 = 6
    end

   end
  end

 end
end

/* Make CASPREP input file ready for processing */
fn = ef || client || '.fil'
if rc = 0 then do
 call LINEOUT fn, add.lines.1
 do i=2 to add.lines.0
  call LINEOUT fn, add.lines.i
 end
 call LINEOUT fn, ':endinstall'
 call LINEOUT fn

 '@call' ef||'casprep.cmd' ef||client||'.fil' ef||client||'.cmd' ef||'casbase.fil'
 'erase ' ef || client || '.fil'
 '@call 'ef || client client logfile  /* created OK, call it */
end
else do                         /* else log the error  */
 lineout logfile, error_string
 say error_string                            /* display error      */
 call RxFuncAdd 'AskRemoveDiskIfFloppy', 'CASAGENT',
                 'ASKREMOVEDISKIFFLOPPY'
 rc = AskRemoveDiskIfFloppy( ) /* Remove diskette if one is in drive */
 exepath || '\setboot /ibd:c:'               /* reboot */
end

return
/********************************************************************/
/* Set asterics                                                     */
/********************************************************************/
setstars:
do i=2 to 6
 star.i = ' '
end
star.KEY = '*'
if star.5 = '*' then
 star.1 = ' '
return

/********************************************************************/
/* Reset for 386 HPFS selections                                    */
/********************************************************************/
setstars4a:
do i=1 to 6
 star.i = ' '
end
star.0 = '*'
return

/********************************************************************/
/* Reset all asterics                                               */
/********************************************************************/
resetstars:
do i=0 to 6
 star.i = ' '
end
return

