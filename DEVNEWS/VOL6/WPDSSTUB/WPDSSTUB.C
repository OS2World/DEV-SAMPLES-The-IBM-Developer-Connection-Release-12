/*
 * File.....: WPDSTTUB.C
 * Author...: Eric E. Osmann
 * Purpose..: Demonstrate calling Workplace Shell methods using DSOM.
 *
 * Workplace Shell methods called:
 *        _wpclsQueryFolder()
 *        _wpclsNew()
 *        _wpOpen()
 *
 * Copyright (c) International Business Machines Corporation 1994
 * All rights reserved.
 */
#pragma info(nogen)

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define INCL_DOS
#define INCL_WINWORKPLACE
#define INCL_PM
#include <os2.h>

#include <somd.h>
#include <somtcnst.h>

#include <wpobject.h>
#include <wpdesk.h>
#include <wppgm.h>
#include <wpabs.h>
#include <wpfolder.h>
#include <wpclrpal.h>
#include <wpclsmgr.h>

enum { STOP, START };

WPClassManager *vWPClassManagerObject;

BOOL       checkForError( Environment *, char *, ... );
SOMClass * getClassObject( SOMClass *, Environment *, string );

int main( int argc, char *argv[] )
{
   Environment *ev;
   SOMClass   *Server;
   SOMObject  *objDesktop;
   SOMObject  *objProgram;
   SOMObject  *objFolder;
   M_WPFolder *clsFolder;
   APIRET     apiRtnCd;
   BOOL       fDaemonUp;
   BOOL       fServerUp;
   ULONG      ulCount;

   if( argc < 2 )
   {
      printf( "Usage: %s <count>\n", argv[0] );
      printf( "\twhere <count> is the number of times to run test case\n" );
      exit( 0 );
   }

   ulCount = atoi( argv[1] );
   if( !ulCount )
   {
      printf( "\aInvalid count\n" );
      exit( 1 );
   }

   /*
    * Check if the DSOM daemon active.
    * if not, we need to start it before we can continue.
    */
   fDaemonUp = WinIsSOMDDReady();
   if( !fDaemonUp )
   {
      apiRtnCd = WinRestartSOMDD( START );
      if( apiRtnCd )
      {
         somPrintf( "Unable to start the DSOM dameon, rc=%ld\n", apiRtnCd );
         exit( apiRtnCd );
      }
   }

   /*
    * Check if the Workplace Shell DSOM server is up and running,
    * if not, we need to start it before we can continue.
    */
   fServerUp = WinIsWPDServerReady();
   if( !fServerUp )
   {
      apiRtnCd = WinRestartWPDServer( START );
      if( apiRtnCd )
      {
         somPrintf( "Unable to start the DSOM server, rc=%ld\n", apiRtnCd );
         exit( apiRtnCd );
      }
   }

   /*
    * Create our local DSOM environment.
    */
   ev = SOM_CreateLocalEnvironment();
   SOMD_Init( ev );

   /*
    * Merge the Workplace Shell Class manager with the SOM Class manager.
    */
   vWPClassManagerObject = WPClassManagerNew();
   _somMergeInto( SOMClassMgrObject, vWPClassManagerObject );

   /*
    * Initialize all the Workplace Shell Classes we are interested in.
    */
   WPObjectNewClass( 1, 1 );
   WPAbstractNewClass( 1, 1 );
   WPProgramNewClass( 1, 1 );
   WPFolderNewClass( 1, 1 );
   WPDesktopNewClass( 1, 1 );

   /*
    * Find the Workplace Shell DSOM server by name.
    */
   Server = _somdFindServerByName( SOMD_ObjectMgr, ev, "wpdServer" );
   checkForError( ev, "Could not find server 'wpdServer'\n" );

   do
   {
      /* ------------------ Start user code section ----------------- */

      clsFolder  = _somdGetClassObj( Server, ev, "WPFolder" );
      objDesktop = _wpclsQueryFolder( clsFolder, "<WP_DESKTOP>", TRUE );
      if( objDesktop )
      {
         /*
          * Create a WPFolder object on the desktop.
          */
         somPrintf( "Creating a opened WPFolder object on desktop: " );
         objFolder = _wpclsNew( clsFolder, "WoW Folder",
                                "OPEN=ICON;ALWAYSSORT=YES",
                                objDesktop, TRUE );
         somPrintf( "%s\n", objFolder ? "Succeeded" : "\aFailed" );

         /*
          * Create a WPProgram object in folder.
          */
         if( objFolder )
         {
            somPrintf( "Creating a WPProgram object in folder: " );
            objProgram = _wpclsNew( _somdGetClassObj( Server, ev, "WPProgram" ),
                                    "My Config File",
                                    "EXENAME=C:\\OS2\\E.EXE;PARAMETERS=C:\\CONFIG.SYS",
                                    objFolder, TRUE );
            somPrintf( "%s\n", objProgram ? "Succeeded" : "\aFailed" );

            /*
             * Let open our WPProgram object.
             */
            _wpOpen( objProgram, NULLHANDLE, OPEN_DEFAULT, 0L );
         }
      }
      else
      {
         somPrintf( "\aUnable to acquire the desktop object pointer\n" );
      }

      /* ------------------ End user code section ----------------- */
      ulCount--;
   } while( ulCount );

   /*
    * Clean up our DSOM environment.
    */
   _somdReleaseObject( SOMD_ObjectMgr, ev, Server );
   checkForError( ev, "_somdReleaseObject failed\n" );
   SOMD_Uninit( ev );
   SOM_DestroyLocalEnvironment( ev );

#ifdef SHUTDOWNSERVER
   /*
    * If we started the Workplace Shell DSOM server,
    * then, lets stop it before going away.
    */
   if( !fServerUp )
   {
      apiRtnCd = WinRestartWPDServer( STOP );
      if( apiRtnCd )
         somPrintf( "Stopping the DSOM server was unsucessful, rc=%ld\n", apiRtnCd );
   }

   if( !fDaemonUp )
   {
      ULONG count = 0;

      /*
       * Loop until server thread ends or we time out.
       */
      while( WinIsWPDServerReady() )
      {
         HEV hev;

         /*
          * First create a private, reset, event semaphore.
          * Wait for 1 seconds. Then try again for a max. of 30 sec.
          */
         DosCreateEventSem( (PSZ) NULL, &hev, 0, FALSE );
         DosWaitEventSem( hev, 1000 );
         if( count++ > 30 )
            break;
      }
      apiRtnCd = WinRestartSOMDD( STOP );
      if( apiRtnCd )
         somPrintf( "Stopping the DSOM daemon was unsucessful, rc=%ld\n", apiRtnCd );
   }
#endif

   return 0;
} /* End of main() */

/*
 * getClassObject
 *
 */
SOMClass *getClassObject( SOMClass *Server, Environment *ev, string sClassName )
{
   SOMClass *ClassObj = NULL;

   ClassObj = _somdGetClassObj( Server, ev, sClassName );
   if( checkForError( ev, NULL ) || ClassObj == NULL )
   {
      somPrintf( "\aCould not find class '%s'\n", sClassName );
      ClassObj = NULL;
   }
   return ClassObj;
}

/*
 *  Check for error
 */
BOOL checkForError( Environment *ev, char *pchFmt, ... )
{
   va_list  vargs;
   char *exId;
   APIRET apiRtnCd = FALSE;

   switch( ev->_major )
   {
      case SYSTEM_EXCEPTION:
      {
         StExcep *params;

         exId   = somExceptionId( ev );
         params = somExceptionValue( ev );

         somPrintf( "\a** Error Occurred **\n" );
         somPrintf( "** exception string: %s\n", exId );
         somPrintf( "** minor error code: %u\n", params->minor );
         somPrintf( "** completion code: %s\n", (params->completed == YES ? "YES" :
                          params->completed == NO ? "NO": "MAYBE" ) );
         somExceptionFree(ev);

         if( pchFmt )
         {
            char achBuf[1024];
            va_start( vargs, pchFmt );
            vsprintf( achBuf, pchFmt, vargs );
            somPrintf( achBuf );
            va_end( vargs );

            SOMD_Uninit( ev );
            SOM_DestroyLocalEnvironment( ev );
            exit( 1 );
         }
         apiRtnCd = TRUE;
         break;
      }

      case USER_EXCEPTION:
      {
         if( pchFmt )
         {
            char achBuf[1024];
            va_start( vargs, pchFmt );
            vsprintf( achBuf, pchFmt, vargs );
            somPrintf( achBuf );
            va_end( vargs );
         }

         apiRtnCd = TRUE;
         break;
      }

      default:
      case NO_EXCEPTION:
      {
         apiRtnCd = FALSE;
         break;
      }
   }

   return(apiRtnCd);
}
#pragma info(nouse)

