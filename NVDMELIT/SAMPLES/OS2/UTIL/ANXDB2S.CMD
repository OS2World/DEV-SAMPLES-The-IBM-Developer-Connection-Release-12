/******************************************************************************/
/* ANXDB2S.CMD                                                                */
/*----------------------------------------------------------------------------*/
/* REXX procedure to allow DB/2 unattended installation                       */
/* on a NetView DM/2 Server Workstation                                       */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Associated Files:                                                          */
/* ----------------                                                           */
/*                                                                            */
/* Description:                                                               */
/* -----------                                                                */
/*  This procedure is required to upgrade ES Data Base Manager to DB2/2 on a  */
/*  NetView DM/2 Server workstation. This procedure is based on the ANXCM2    */
/*  command file developed for similar function for CM/2 migration on a       */
/*  NetView DM/2 Server.                                                      */
/*                                                                            */
/* Installation:                                                              */
/* ------------                                                               */
/*  The ANXDB2S.CMD procedure should be installed in the DB2/2 images         */                      
/*  directory with the product code images.                                   */
/*                                                                            */
/*                                                                            */
/* What This Procedure Performs:                                              */
/* ----------------------------                                               */
/*  Basically the procedure ANXDB2S.CMD is called instead of the              */
/*  DB2CID.EXE program. The parameters passed to the procedure are            */
/*  the same as DB2CID plus the /BD:<b> (Boot Drive) and /L5:<path\file>      */
/*  and DBName which are used by the procedure itself.                        */
/*  The procedure calls DB2CID, passing it the                                */
/*  same parameters but the /BD:, /L5:, and /DBNAME: which are filtered out.  */
/*                                                                            */
/*  This is the Procedure Flow:                                               */
/******************************************************************************/
/*                            =============                                   */
/* ...........................! CDM AGENT !                                   */
/* .                          =============                                   */
/* .                                !                                         */
/* .                                !                                         */
/* .                                !                                         */
/* .                                !                                         */
/* .                                V                                         */
/* .      +------------+-------------+-----------------+---------+            */
/* .      !            !             !                 !         !            */
/* .      !(RIS=00)    !(RIS=FD)     !(RIS=FE)         !(RIS=FF) !(RIS=Others)*/
/* .      !            !             !                 !         !            */
/* .      !            !             !            Make Database  !            */
/* .      !            !             !             consistent    !            */
/* .      !            !             !                 !         !            */
/* .   Change          !          Restore         Set RIS=00     !            */
/* . STARTUP.CMD       !           CIDrc               !         !            */
/* .      !         STARTDBM         !                 +<--------+            */
/* .      !          LOGON           !                 !                      */
/* . Return FFFF    MIGRATE          !           Call DB2CID.EXE              */
/* .      .            !             !                 !                      */
/* .      .            !             !                 !                      */
/* .      .            !             V     (yes)       V                      */
/* ........            !             +<--------- CIDrc==FFxx ?                */
/* .                Restore          !                 !                      */
/* .                 CIDrc           !                 ! (no)                 */
/* .                   !             !                 !                      */
/* .                   !             !             Save CIDrc                 */
/* .                   !             !                 !                      */
/* .                   !             !                 !                      */
/* .                   !             !     (yes)       V                      */
/* .           Restore STARTUP.CMD   !    +------CIDrc==FExx ?                */
/* .                   !             !    !            !                      */
/* .                   +<------------!    !            ! (no)                 */
/* .                   !                  !            !                      */
/* .                   !                  !      Restore StartUp.CMD          */
/* .                   !                  !            !                      */
/* .                   !                  !            !                      */
/* .             Return CIDrc       Return FFFD  Return FFFE                  */
/* .                   .                  .            .                      */
/* .                   .                  .            .                      */
/* .....................................................                      */
/*                                                                            */
/*                                                                            */
/*  Note:                                                                     */
/*    RIS = REMOTE_INSTALL_STATE environment variable                         */
/*                                                                            */
/*  REQUIRED USER CUSTOMIZATION :                                             */
/*  At this point, the program assumes that a local LOGON can be              */
/*  accomplished using USERID and PASSWORD.  If another USERID is             */
/*  preferred, this can be specified at the beginning of the Command          */
/*  File.  (Immediately following these comments).  A user procedure          */
/*  could also be developed to determine the userid and password to           */
/*  use based on local standards.                                             */
/*                                                                            */
/******************************************************************************/
/* Change the following lines if a different userid and password */
/* combination are preferred                                     */

userid = 'USERID'
password = 'PASSWORD'


/****************/
/* Trace option */
/****************/
ActTrace = 0         /* 1 Trace ON; 0 Trace OFF              */
TraceOpt = '?r'      /* Put here one of the REXX trace parms */


arg Params

/*******************************/
/* Global variable definitions */
/*******************************/
CIDProgName   = "DB2CID.EXE"
ChkPtFile     = "IBMNVDM2.DB2"
CidRc         = 0

/**************************************************/
/* Retrieve the NetView DM/2 Remote Install State */
/**************************************************/
RemoteS = RemoteState()
if RemoteS = "" then
   /***************************************/
   /* The Remote Install State is not set */
   /***************************************/
   return X2D('1604')             /* Error: Unexpected condition        */
                                  /*   The REMOTE_INSTALL_STATE is      */
                                  /*   inconsistent                     */

/**********************************************************/
/* Get the Boot Drive to know where modify the STARTUP.CMD*/
/**********************************************************/
parse value Params with P1 '/BD:' BootDrive P2
Params = P1||P2
parse value Params with P3 '/L5:' ANXDB2log P4
Params = P3||P4
parse value Params with P5 '/DBNAME:' DBName P6
Params = P5||P6
If DBName = ' ' then 
   DBName = 'IBMNVDM2'
/*****************************************************/
/* Get the Directory of the running Command File     */
/*    and build the fully specified CID program name */
/*****************************************************/
parse source . . CmdFile
RunningPath = filespec("drive",CmdFile)||filespec("path",CmdFile)
ExeFile     = RunningPath||CIDProgName
if ActTrace = 1 then
   trace VALUE(TraceOpt)

select

/*************************************/
/* If the Remote Install State is 00 */
/*************************************/
when RemoteS = '00' then do

   /***********************************/
   /* Save the production STARTUP.CMD */
   /***********************************/
   '@COPY '||BootDrive||':\STARTUP.CMD '||BootDrive||':\STARTUP.ANX'

   /*******************************/
   /* Build a minimal STARTUP.CMD */
   /*******************************/
   '@DEL '||BootDrive||':\STARTUP.CMD'
   call lineout BootDrive||':\STARTUP.CMD', "REM *** Minimal STARTUP.CMD ***"
   call lineout BootDrive||':\STARTUP.CMD', "NET START SERVER"
   call lineout BootDrive||':\STARTUP.CMD', "LOGON CIDA056"
   call lineout BootDrive||':\STARTUP.CMD', "NET USE I: \\CIDA099S\IMG"
   call lineout BootDrive||':\STARTUP.CMD', "START ANXCMCLS.EXE"
   call lineout BootDrive||':\STARTUP.CMD', "EXIT" 
   call lineout BootDrive||':\STARTUP.CMD'

   /***************************************************/
   /* Return to the NetView DM/2 Agent an architected */
   /*    return code to reboot and call back          */
   /***************************************************/
   return X2D('FFFF')
   end /* end when RemoteS = '00' */

/*************************************/
/* If the Remote Install State is FD */
/* Need to migrate data base and     */
/* restore startup.cmd               */
/*************************************/
when RemoteS = 'FD' then do

   rcfinal = x2d(FE00)
   STARTDBM

   /*****************************************************/
   /* Leave some record of failure if not rc = 0 or 1   */
   /*****************************************************/
   dbmrc = rc
   if dbmrc <> 0 & dbmrc <> 1 then
      do
      outline = date() time() 'STARTDBM failed with RC = ' dbmrc
      call lineout ANXDB2log, outline
      rcfinal = x2d(FE08) /* could not start DBM */
      end
    else /* dbm started - try to migrate */
      do
      /* add your own logon logic here  if desired */

     'LOGON '||userid||' /P:'||password||' /L'

      logonrc = rc
      if logonrc <> 0 then
         do
         outline = date() time() 'UPM LOGON failed with RC = ' logonrc
         call lineout ANXDB2log, outline
         rcfinal = x2d(FE08) /* could not start logon */
         end
      else /* dbm started - try to migrate */
         do
         MIGRATE1 DBName 
         migrc = rc
         if migrc <> 0 then
            do
            outline = date() time() 'DB Migration failed with RC ' migrc
            call lineout ANXDB2log, outline
            rcfinal = x2d(FE08) /* could not start DBM */
            end
          else
            do
              /**********************************************************/
              /* Call DelIndex to delete NvDM/2 database indexes        */
              /* This is done as a precaution to insure the databases   */
              /* are consistent.  In this way, DB2/2 will rebuild the   */
              /* index files the first time it opens the database.      */
              /**********************************************************/
              Call DelIndex DBName
            end   
          end
        end

   if d2x(rcfinal,4) = 'FE00' then /* everything worked */
      do
      outline = date() time() DBName 'DB Migration Complete'
      call lineout ANXDB2log, outline
      end

   /**************************************/
   /* Restore the production STARTUP.CMD */
   /**************************************/
   '@COPY '||BootDrive||':\STARTUP.ANX '||BootDrive||':\STARTUP.CMD'
   '@DEL '||BootDrive||':\STARTUP.ANX'

   return rcfinal
   end /* end when RemoteS = 'FD' */

/*************************************/
/* If the Remote Install State is FE */
/*************************************/
when RemoteS = 'FE' then do

   /******************************************/
   /* Get the NetView DM/2 installation path */
   /******************************************/
   NvDMPath = GetNvDMPath()

   /*******************************************/
   /* Read the CID Program return code in the */
   /* checkpoint file                         */
   /*******************************************/
   CIDrc = ReadLine( NvDMPath||'\'||ChkPtFile, "CM2PGMRC")
   if CIDrc = "" | CIDrc = "ERR" then
      CIDrc = X2D('2200')    /* Error: Unable to report CID rc   */
                             /* Unable to Read the checkpoint    */
                             /* file or the CIDPGMRC key doesn't */
                             /* exist                            */

   /*******************************/
   /* Deletes the checkpoint file */
   /*******************************/
   '@DEL '||NvDMPath||'\'||ChkPtFile

   return CIDrc
   end /* end when RemoteS = 'FE' */

/*************************************/
/* If the Remote Install State is FF */
/*************************************/
when RemoteS = 'FF' then do

   /**************************************/
   /* Set the REMOTE_INSTALL_STATE to 00 */
   /**************************************/
   call value 'REMOTE_INSTALL_STATE', '00', 'OS2ENVIRONMENT'

   /***************************************/
   /* make the database consistent        */
   /* before reinstalling                 */
   /***************************************/

   call fixdb DBName userid password
   /***************************************/
   /* Call the CID installation procedure */
   /***************************************/

   if result = 0 then
      do
      '@'||ExeFile Params
      CIDrc = rc
      end
    else CIDrc = x2d('1604') 
   end /* end when RemoteS = 'FF' */

/*****************************************************/
/* If the Remote Install State is not 00 or FE or FF */
/*****************************************************/

otherwise
   /***************************************/
   /* Call the CID installation procedure */
   /***************************************/
   '@'||ExeFile Params
   CIDrc = rc

end /* end when RemoteS = 'FF' */

/* if the db/2 install completed successfully, we want to be recalled */
/* to migrate the data base.  We'll use RIS=FD                        */

say 'CIDrc = ' CIDrc
if d2x(CIDrc,4) = 'FE00' | d2x(CIDrc,4) = 'FE04' then
   do
   retrc = x2d(FFFD)
   return retrc
   end

/************************************************/
/* If CM/2 has not to reboot and be called back */
/* we can restore the production STARTUP.CMD    */
/************************************************/
if ( ANDFF00(CIDrc) \= 'FF00' ) then do
   /**************************************/
   /* Restore the production STARTUP.CMD */
   /**************************************/
   '@COPY '||BootDrive||':\STARTUP.ANX '||BootDrive||':\STARTUP.CMD'
   '@DEL '||BootDrive||':\STARTUP.ANX'

   if ( ANDFF00(CIDrc) \= 'FE00' ) then do
      /******************************************/
      /* Get the NetView DM/2 installation path */
      /******************************************/
      NvDMPath = GetNvDMPath()

      /*************************************************************/
      /* Write the CID Program return code in a checkpoint file.   */
      /* If this statement fails we will not be able to return the */
      /* CID Program return code                                   */
      /*************************************************************/
      call WriteLine NvDMPath||'\'||ChkPtFile, "CM2PGMRC", CIDrc

      /***************************************************/
      /* Return to the NetView DM/2 Agent an architected */
      /*    return code to reboot and call back          */
      /***************************************************/
      return X2D('FFFE')
   end
end

return CIDrc

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/* ----------------------- Utility procedures -----------------------------*/
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/


/******************************************************************/
/* isRebootRC                                                     */
/*================================================================*/
/* This procedure returns TRUE if the passed value is in the      */
/* range 0xFE00 - 0xFFFF otherwise return false                   */
/*----------------------------------------------------------------*/
/* Arguments:                                                     */
/*   ORD     TYPE      MEANING                                    */
/*    1     integer    A decimal return code value                */
/*----------------------------------------------------------------*/
/* Return codes                                                   */
/*   VALUE      MEANING                                           */
/*   TRUE       The passed RC is in the range 0xFE00 - 0xFFFF     */
/*   FALSE      The passed RC is not in the range 0xFE00 - 0xFFFF */
/******************************************************************/
isRebootRC: procedure
  return ( ANDFF00(arg(1)) == 'FF00' ) | ( ANDFF00(arg(1)) == 'FE00' )


/*******************************************************************/
/* AndFF00                                                         */
/*=================================================================*/
/* This procedure returns the hex string of the value passed ANDed */
/* with 0xFF00                                                     */
/*-----------------------------------------------------------------*/
/* Arguments:                                                      */
/*   ORD     TYPE      MEANING                                     */
/*    1     integer    A decimal value                             */
/*-----------------------------------------------------------------*/
/* Return codes                                                    */
/*   VALUE               MEANING                                   */
/*   arg(1) AND 0xFF00   The resulted hex string                   */
/*******************************************************************/
AndFF00: procedure
  return LEFT( LEFT( RIGHT( D2X(arg(1), 4), 4, '0'), 2), 4, '0')

/*******************************************************************/
/* GetNvDMPath                                                     */
/*=================================================================*/
/* This procedure returns fully qualified path where the           */
/* NetView DM/2 is installed                                       */
/*-----------------------------------------------------------------*/
/* Arguments: none                                                 */
/*-----------------------------------------------------------------*/
/* Return codes                                                    */
/*   VALUE        MEANING                                          */
/*   a path       The fully qualified path where the NetView DM/2  */
/*                   is installed                                  */
/*   ""           If not able to find the NetView DM/2 path        */
/*******************************************************************/
GetNvDMPath: procedure
EnvDPATH = value( 'DPATH', , 'OS2ENVIRONMENT' )
do until DirDPATH = ""
   parse value EnvDPATH with DirDPATH ';' EnvDPATH
   if DirDPATH \= "" then do
      if stream( DirDPATH||"\SYSLEVEL.NDM", 'C', 'QUERY EXISTS' ) \= "" then
         return DirDPATH
   end
end
return DirDPATH

/*******************************************************************/
/* RemoteState                                                     */
/*=================================================================*/
/* This procedure returns the NetView DM/2 Remote Install State    */
/*-----------------------------------------------------------------*/
/* Arguments: none                                                 */
/*-----------------------------------------------------------------*/
/* Return codes                                                    */
/*   VALUE        MEANING                                          */
/*   2 chars      The REMOTE_INSTALL_STATE value                   */
/*******************************************************************/
RemoteState: procedure
return value( 'REMOTE_INSTALL_STATE', , 'OS2ENVIRONMENT' )

/*******************************************************************/
/* ReadLine                                                        */
/*=================================================================*/
/* This procedure reads a the value of a keyword contained in the  */
/* NetView DM/2 Recovery text file                                 */
/*-----------------------------------------------------------------*/
/* Arguments:                                                      */
/*   ORD     TYPE      MEANING                                     */
/*    1     string     Fully qualified Filename of the file where  */
/*                        to search the keyword                    */
/*    2     string     The keyword to search                       */
/*-----------------------------------------------------------------*/
/* Return codes                                                    */
/*   VALUE        MEANING                                          */
/*   string       The value of the supplied keyword                */
/*   "ERR"        If not able to open the file                     */
/*   ""           If not able to find the keyword                  */
/*******************************************************************/
ReadLine: procedure

call stream arg(1),C,'open read'
if result <> "READY:"
   then return "ERR"

do until ( lines(arg(1)) = 0 )
   parse value linein(arg(1)) with InKey "=" KeyValue
   InKey = strip( InKey, 'B' )
   KeyValue = strip( KeyValue, 'B' )
   if InKey = arg(2) then do
      call stream arg(1),C,'close'
      return KeyValue
   end
end
call stream arg(1),C,'close'
return ""

/*******************************************************************/
/* WriteLine                                                       */
/*=================================================================*/
/* This procedure writes/changes a keyword/value pair in the       */
/* NetView DM/2 Recovery text file                                 */
/*-----------------------------------------------------------------*/
/* Arguments:                                                      */
/*   ORD     TYPE      MEANING                                     */
/*    1     string     Fully qualified Filename of the file where  */
/*                        to write the keyword/value pair          */
/*    2     string     The keyword to write                        */
/*    3     string     The value to write                          */
/*-----------------------------------------------------------------*/
/* Return codes                                                    */
/*   VALUE        MEANING                                          */
/*   TRUE         Operation successfully executed                  */
/*   FALSE        Unable to write the keyword/value pair           */
/*******************************************************************/
WriteLine: procedure

Found   = 0
LineNum = 1

call stream arg(1),C,'query exists'
if result = "" then
   call lineout arg(1), arg(2)||" = "||arg(3)
else do

   do until ( lines(arg(1)) = 0 )
      QUEUE linein(arg(1))
   end

   call stream arg(1),C,'close'
   '@DEL' arg(1)

   do QUEUED()
      parse pull InKey "=" KeyValue
      InKey = strip( InKey, 'B' )
      if InKey = arg(2) then do
         Found = 1
         call lineout arg(1), InKey||" = "||arg(3)
         end
      else do
         KeyValue = strip( KeyValue, 'B' )
         call lineout arg(1), InKey||" = "||KeyValue
      end
   end

   if Found = 0 then
      call lineout arg(1), arg(2)||" = "||arg(3)
end
call lineout arg(1)
return 1

/*******************************************************************/
/* DelIndex                                                                                             */
/*=================================================================*/
/* This procedure locates the NetView DM/2 Database and erases all */
/* the Index (.INX) files within it. Therefore, any index files    */
/* corrupted by the migration will be removed.                     */
/* The indexes will be recreated automatically when accessed by NvDM/2 */
/*=================================================================*/
/* Arguments:                                                      */
/*   ORD     TYPE      MEANING                                     */
/*    1     string     NvDM/2 database name        */
/* Return codes                                                    */
/*   VALUE        MEANING                                          */
/*******************************************************************/
DelIndex:Procedure
Parse Arg dbname

/* The following statements must always precede any calls to the Database Mgr */
if Rxfuncquery('SQLDBS' <> 0) then
   rcy = Rxfuncadd('SQLDBS','SQLAR','SQLDBS')
if Rxfuncquery('SQLEXEC' <> 0) then
   rcy = Rxfuncadd('SQLEXEC','SQLAR','SQLEXEC')
/* The preceding statements must always precede any calls to the Database Mgr */

/* The Database Manager is already running as a result of the previous        */
/* Migrate Database from ES 1.0 to DB2/2 format operation so just ensure that */
/* we are not connected to any database.                                      */
call SQLDBS 'STOP USING DATABASE'

/* Access the System database directory (ie. \SQLLIB\SQLDBDIR\) on this       */
/* workstation.                                                               */
call SQLDBS 'OPEN DATABASE DIRECTORY ON 0 USING :scan'
if SQLCA.SQLCODE \=0 then do
   call error 'OPEN DATABASE SYSTEM DIRECTORY SCAN FAILED'
   exit
end

/* Search the System database directory for the NVDM/2 database.              */
foundflag = 0
do while SQLCA.SQLCODE = 0  & foundflag = 0
   call SQLDBS 'GET DATABASE DIRECTORY ENTRY :scan.1 USING :entry'
   if SQLCA.SQLCODE = 0 then do
      if entry.1 = dbname then do
         foundflag = 1
      end
   end
end

if foundflag \= 1 then do
   call SQLDBS 'CLOSE DATABASE DIRECTORY :scan.1'
   call error 'ERROR - DATABASE' dbname 'NOT FOUND IN SYSTEM DIRECTORY'.
   exit
end

/* End the successful scan of the System database directory.                  */
call SQLDBS 'CLOSE DATABASE DIRECTORY :scan.1'

/* Capture the logical drive on which the NVDM/2 database resides.            */
parse var entry.3 drive ':'
drivecolon = entry.3

/* Access the Volume database directory (ie. \SQLDBDIR\) on this drive.       */
call SQLDBS 'OPEN DATABASE DIRECTORY ON' drive 'USING :scan'
if SQLCA.SQLCODE \=0 then do
   call error 'OPEN DATABASE VOLUME DIRECTORY SCAN FAILED'
   exit
end

/* Search the Volume database directory for the NVDM/2 database.              */
foundflag = 0
do while SQLCA.SQLCODE = 0  & foundflag = 0
   call SQLDBS 'GET DATABASE DIRECTORY ENTRY :scan.1 USING :entry'
   if SQLCA.SQLCODE = 0 then do
      if entry.1 = dbname then do
         foundflag = 1
      end
   end
end

if foundflag \= 1 then do
   call SQLDBS 'CLOSE DATABASE DIRECTORY :scan.1'
   call error 'ERROR - DATABASE' dbname 'NOT FOUND IN VOLUME DIRECTORY ON DRIVE' drivecolon.
   exit
end

/* End the successful scan of the Volume database directory.                  */
call SQLDBS 'CLOSE DATABASE DIRECTORY :scan.1'

/* Get the subdirectory name containing all of the files for the NVDM/2       */
/* database, concatenate with the logical disk drive on which it resides and  */
/* erase all of the index (ie. .INX) files in it.                             */
'@echo off'
erase drivecolon'\'entry.4'\*.inx'
echo on

return

error:
parse arg stmt
say  stmt
say 'SQLCODE = ' sqlca.sqlcode
return

/*******************************************************************/
/* Fixdb                                                           */                                       */
/*=================================================================*/
/* This procedure locates the database and makes it consistent.    */
/* This is necessary because NVDM/2 has made the database          */		
/* inconsistent by rebooting the machine without committing all    */
/* changes to the database.                                        */
/*=================================================================*/
/* Arguments:                                                      */
/*   ORD     TYPE      MEANING                                     */
/*    1     string     NvDM/2 database name                        */
/* Return codes                                                    */
/*   VALUE        MEANING                                          */
/*******************************************************************/
Fixdb:Procedure
Parse Arg dbname user pw
   STARTDBM

   /*****************************************************/
   /* Leave some record of failure if not rc = 0 or 1   */
   /*****************************************************/
   dbmrc = rc
   if dbmrc <> 0 & dbmrc <> 1 then
      do
      outline = date() time() 'STARTDBM failed with RC = ' dbmrc
      call lineout ANXDB2log, outline
      rcfinal = x2d(FE08) /* could not start DBM */
      end
    else /* dbm started - try to migrate */
      do
      /* add your own logon logic here  if desired */

     'LOGON '||user||' /P:'||pw||' /L'

      logonrc = rc
      if logonrc <> 0 then
         do
         outline = date() time() 'UPM LOGON failed with RC = ' logonrc
         call lineout ANXDB2log, outline
         rcfinal = x2d(FE08) /* could not start logon */
         end
      else /* dbm started - try to make consistent */
         do
         /* The following statements must always precede any calls to the Database Mgr */
         if Rxfuncquery('SQLDBS' <> 0) then
            rcy = Rxfuncadd('SQLDBS','SQLAR','SQLDBS')
         if Rxfuncquery('SQLEXEC' <> 0) then
            rcy = Rxfuncadd('SQLEXEC','SQLAR','SQLEXEC')
         /* The preceding statements must always precede any calls to the Database Mgr */

         call SQLDBS 'START USING DATABASE ' dbname 
         
         if SQLCA.SQLCODE = 0 then   
            call SQLDBS 'STOP USING DATABASE'
         if SQLCA.SQLCODE = 0 then
            do 
            'STOPDBM'
            rcfinal = rc
            end
         else rcfinal = SQLCA.SQLCODE
         end                
      end
return rcfinal



