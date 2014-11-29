/***************************************************************************
 *
 * File name   :  prtevent.c
 *
 * Description :  This program contains the OSA code
 *
 *                This source file contains the following functions:
 *                InitOSA
 *                TerminateOSA
 *                GenerateOSAEvent
 *                CreateAEAddrDescr
 *                CreateAERecord
 *                GetFileNames
 *                CreateAEFileList
 *                SendOSAEvent
 *                ProcessSemanticEvent
 *                OpenAppProc
 *                QuitAppProc
 *                OpenDocProc
 *                PrintDocProc
 *
 *
 * Required
 *    files    :
 *
 *  Concepts   :  OSA Events
 *
 *  API's      :
 *
 *    Files    :
 *
 * Required
 *   libraries :  OS2386.LIB     - Presentation Manager/OS2 library
 *                C library
 *
 * Required
 *   programs  :  C Compiler
 *                Linker
 *                Resource Compiler
 *
 *  Copyright (C) 1991-1993 IBM Corporation
 *
 *      DISCLAIMER OF WARRANTIES.  The following [enclosed] code is
 *      sample code created by IBM Corporation. This sample code is not
 *      part of any standard or IBM product and is provided to you solely
 *      for  the purpose of assisting you in the development of your
 *      applications.  The code is provided "AS IS", without
 *      warranty of any kind.  IBM shall not be liable for any damages
 *      arising out of your use of the sample code, even if they have been
 *      advised of the possibility of such damages.                                                    *
 *
 ******************************************************************************/

/* os2 includes */
#define INCL_DEV
#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS
#define INCL_GPIBITMAPS
#define INCL_GPICONTROL
#define INCL_GPICORRELATION
#define INCL_GPIERRORS
#define INCL_GPILCIDS
#define INCL_GPIMETAFILES
#define INCL_GPIPATHS
#define INCL_GPIPRIMITIVES
#define INCL_GPISEGMENTS
#define INCL_GPITRANSFORMS
#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_WINDIALOGS
#define INCL_WINERRORS
#define INCL_WINMLE
#define INCL_WINSTDFILE
#define INCL_WINSTDFONT
#define INCL_WINWINDOWMGR
#define INCL_OSAAPI
#define INCL_OSA
#include <os2.h>

/* c language includes */
#include <ctype.h>
#include <memory.h>
#include <process.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\stat.h>
#include <sys\types.h>

/* application includes */
#include "pmassert.h"
#include "prtsamp.h"
#include "prtsdlg.h"
#include "prtevent.h"

/***************************************************************************/
/*    OSA Event Interface Routines                                                              */
/***************************************************************************/

/**************************************************************************
 *
 *  Name       : InitOSA
 *
 *  Description: Perform Initialization of OSA
 *
 *  Concepts:    Initialization of OSA and installation of Event Handlers
 *
 *  API's      : AEInit
 *               AEInstallEventHandlers
 *
 *  Parameters :  hwnd = client window handle
 *
 *  Return     :  success or failure
 *************************************************************************/
void InitOSA(HWND hwnd)
{
   OSErr              rc = noErr;
   AEEventClass       aeEventClass;
   AEEventID          aeEventId;
   PMAIN_PARM       pmp;

   /* obtain the main parameter pointer from window words */
   pmp = (PMAIN_PARM) WinQueryWindowULong( hwnd, QWL_USER );

   /* Do Apple Event Manager Initialization */
   rc = AEInit (hwnd,APP_NAME,FALSE);
   /*
    * Ensure AEInit worked ok; if not, present a message box.
    * ( See pmassert.h. )
    */
   pmassert(pmp-> hab, rc == noErr );

   /*
    * Install the required (basic) 4 OSAEvent handlers first
    * Ensure AEInstallEventHandler worked ok; if not, present a message box.
    * ( See pmassert.h. )
    */

   aeEventClass = kCoreEventClass;
   aeEventId = kAEOpenApplication;
   rc = AEInstallEventHandler( aeEventClass,
                               aeEventId,
                               (AEEventHandlerUPP)OpenAppProc,
                               0L,
                               FALSE );
   pmassert(pmp-> hab, rc == noErr );

   aeEventId = kAEQuitApplication;
   rc = AEInstallEventHandler( aeEventClass,
                               aeEventId,
                               (AEEventHandlerUPP)QuitAppProc,
                               0L,
                               FALSE );
   pmassert(pmp-> hab, rc == noErr );

   aeEventId = kAEOpenDocuments;
   rc = AEInstallEventHandler( aeEventClass,
                               aeEventId,
                               (AEEventHandlerUPP)OpenDocProc,
                               0L,
                               FALSE );
   pmassert(pmp-> hab, rc == noErr );

   aeEventId = kAEPrintDocuments;
   rc = AEInstallEventHandler( aeEventClass,
                               aeEventId,
                               (AEEventHandlerUPP)PrintDocProc,
                               0L,
                               FALSE );
   pmassert(pmp-> hab, rc == noErr );

}

/**************************************************************************
 *
 *  Name       : TerminateOSA
 *

 *  Description: Perform OSA Termination
 *
 *  Concepts:    Remove Event Handlers and terminate OSA
 *
 *  API's      : AETerminate
 *               AERemoveEventHandlers
 *
 *  Parameters :  hwnd = client window handle
 *
 *  Return     :  success or failure
 *************************************************************************/
void TerminateOSA(HWND hwnd)
{
   OSErr              rc = noErr;
   AEEventClass       aeEventClass;
   AEEventID          aeEventId;

/* Remove the required (basic) 4 OSAEvent handlers */

   aeEventClass = kCoreEventClass;
   aeEventId = kAEOpenApplication;
   rc = AERemoveEventHandler( aeEventClass,
                               aeEventId,
                               (AEEventHandlerUPP)OpenAppProc,
                               FALSE );

   aeEventId = kAEQuitApplication;
   rc = AERemoveEventHandler( aeEventClass,
                               aeEventId,
                               (AEEventHandlerUPP)QuitAppProc,
                               FALSE );

   aeEventId = kAEOpenDocuments;
   rc = AERemoveEventHandler( aeEventClass,
                               aeEventId,
                               (AEEventHandlerUPP)OpenDocProc,
                               FALSE );

   aeEventId = kAEPrintDocuments;
   rc = AERemoveEventHandler( aeEventClass,
                               aeEventId,
                               (AEEventHandlerUPP)PrintDocProc,
                               FALSE );

   /* Terminate Apple Event Processing */
   rc = AETerminate();
}

/***************************************************************************/
/*    Apple Event Generator and related functions                          */
/***************************************************************************/


/**************************************************************************
 *
 *  Name       : GenerateOSAEvent
 *
 *  Description: Generates Apple Events(depending on menu item selected)
 *               to itself
 *  Concepts:    Apple Events
 *
 *  API's      : AEPutParamPtr
 *                AEDisposeDesc
 *
 *  Parameters :  hwnd = window handle
 *                msg  = message code
 *                mp1  = first message parameter
 *                mp2  = second message parameter
 *
 *  Return     :  depends on message sent
 *************************************************************************/

ULONG GenerateOSAEvent( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  OSAEvent theOSAEvent;   /* the apple event you are generating       */
  AEEventClass aeEventClass;
  AEEventID    aeEventID;
  AEAddressDesc targetaddr;    /* the address descriptor for target app */
                              /* for Creating an Address Descriptor */

  CHAR  pFilename[ CCHMAXPATH ];
  ULONG uNumFiles;

  PMAIN_PARM pmp;

  OSErr    rc = noErr;

  /***********************************************************/
  /*Call CreateAEAddrDescr to get an address descriptor record*/
  /* (needed when supplying the target address for the       */
  /* call to AECreateOSAEvent Function) to ourselves       */
  /***********************************************************/
  rc = CreateAEAddrDescr( &targetaddr);

  /***********************************************************/
  /* Call CreateAERecord to create the Apple Event Rec( which*/
  /* calls AECreateOSAEvent function creates an Apple Event*/
  /* with only the specified attributes and no parameters).  */
  /***********************************************************/

  rc = CreateAERecord(SHORT1FROMMP(mp1),  /* The Event Type */
                      &targetaddr,         /* Target application - The one  */
                                          /* to receive the Apple Event    */
                      &theOSAEvent);

  /***********************************************************/
  /* Add additional parameters if necessary                  */
  /*                                                         */
  /***********************************************************/

   switch(SHORT1FROMMP(mp1))
   {
     case IDM_AEOPENAPP:
     case IDM_AEQUITAPP:
        ; /*  no additional parms needed */
        break;

     case IDM_AEOPENDOC:

       /****************************************************************/
       /* Add the additional parameter (the list of filenames to open) */
       /* to the Apple Event Record.                                   */
       /****************************************************************/

       /* Put up a dialog to get the list of file names (for now just 1) */
       GetFileNames(hwnd,pFilename, uNumFiles);
       CreateAEFileList (hwnd,&theOSAEvent,pFilename,uNumFiles);
       break;

     case IDM_AEPRINTDOC:

       /****************************************************************/
       /* Add the direct parameter (the filename to print)             */
       /* to the Apple Event Record.                                   */
       /****************************************************************/

       /* obtain the main parameter pointer from window words */
       pmp = (PMAIN_PARM)WinQueryWindowULong( hwnd, QWL_USER );
       /* Print the presently loaded file  */
       strcpy(pFilename,pmp->szFilename);

       /**********************************************************************/
       /* Use AEPutParamPtr  to place the filename into the direct parameter */
       /* of the Apple Event.                                                */
       /**********************************************************************/

       rc = AEPutParamPtr(&theOSAEvent,
                      keyDirectObject,
                      typeChar,
                      pFilename,
                      strlen(pFilename)+1);


        break;

   }   /* End of switch on mp1 */

  /***********************************************************/
  /* Call SendOSAEvent to send the Apple Event which was   */
  /* set up. SendOSAEvent will call AESend to send it.     */
  /* For now SendOSAEvent will always set the send mode    */
  /* to kAENoReply (client doesn't want reply)               */
  /***********************************************************/

  rc = SendOSAEvent( &theOSAEvent);


  /***********************************************************/
  /* Call AEDisposeDesc to dispose of the Apple Event Record */
  /* and it's associated descriptor lists and records. If    */
  /* the descriptor record you pass to AEDisposeDesc (such as*/
  /* this Apple Event Record) includes other nested          */
  /* descriptor records, one call to AEDisposeDesc will      */
  /* dispose of them all.                                    */
  /***********************************************************/
  /* Dispose of the all Descriptor Records of the Apple Event Record */
    AEDisposeDesc(&theOSAEvent);

     return rc;

} /* End of Procedure - GenerateOSAEvent */


/**************************************************************************
 *
 *  Name       : CreateAEAddrDescr
 *
 *  Description: Create an Apple Event Address Descriptor
 *               for a target address to yourself or to another application
 *
 *  Concepts:    Use of AECreateDesc
 *
 *  API's      : AECreateDesc
 *               AEGetPID
 *
 *  Parameters :
 *                targetaddr =  Address Descriptor for target app
 *
 *  Return     :  success or failure
 *************************************************************************/

ULONG CreateAEAddrDescr (AEAddressDesc * targetaddr)   /* Address Descriptor for */
                                                     /* target application     */
{
  OSErr    rc = noErr;
  PID thePID ;
  /**************************************************************/
  /* Call AECreateDesc to get an address descriptor record  (needed when      */
  /* supplying the target address for the call to AECreateOSAEvent Function)  */
  /* We are requesting an Address Descriptor Record of type typePID to send  */
  /* Apple Events to yourself or typeApplSignature to send to another app      */
  /**************************************************************/


#ifdef OSA_CLIENT
  CHAR  pAppName[ CCHMAXPATH ] = TARGET_APP;

  /* If you are the client, you will be sending osa events to another application */

  rc = AEGetPID(pAppName, &thePID);


#else
/* If you are the server, you are sending osa events to yourself */
  thePID =kCurrentProcess;
#endif

  rc = AECreateDesc( typePID,
                     &thePID,
                     sizeof(thePID),
                     targetaddr);

  /* Return the result */
  return rc;
}

/**************************************************************************
 *
 *  Name       : CreateAERecord
 *
 *  Description: Create an Apple Event Record
 *               Create an Apple Event Record with input parameters
 *               supplied for the event class, event type, and target address.
 *               The return ID with always default to kAutoGenerateReturnID
 *               (requesting the AE Manager to generate a unique id.
 *               The transaction id will always default to kAnyTransactionID
 *               indicating that the Apple Event is not part of a transaction.            *
 *
 *  Concepts:    Use of AECreateOSAEvent
 *
 *  API's      : AECreateOSAEvent
 *
 *  Parameters :  EventType = Event type
 *                targetaddr =  Address Descriptor for target application
 *                theOSAEvent = the buffer which contains the apple event
 *
 *  Return     :  success or failure
 *************************************************************************/


ULONG CreateAERecord(short EventType,        /* Event type */
                         AEAddressDesc * targetaddr,   /* Address Descriptor for */
                                                     /* target application     */
                         OSAEvent * theOSAEvent)  /* apple event buffer */
{
  OSErr    rc = noErr;
  short returnID;  /* AE MGR should generate unique ID */
  long transactionID;  /* not part of a transaction */
  AEEventClass aeEventClass;
  AEEventID    aeEventID;

   switch(EventType)
   {
     case IDM_AEOPENAPP:
       aeEventClass = kCoreEventClass;  /* Core Event Class */
       aeEventID = kAEOpenApplication; /* Open Application Event */

     break;

     case IDM_AEQUITAPP:
       aeEventClass = kCoreEventClass;  /* Core Event Class */
       aeEventID = kAEQuitApplication; /* Quit Application Event */
     break;

     case IDM_AEOPENDOC:
       aeEventClass = kCoreEventClass;  /* Core Event Class */
       aeEventID = kAEOpenDocuments; /* Open Documents Event */
     break;

     case IDM_AEPRINTDOC:
       aeEventClass = kCoreEventClass;  /* Core Event Class */
       aeEventID = kAEPrintDocuments; /* Print Documents Event */
     break;

   }   /* End of switch on mp1 */

  /***********************************************************/
  /* Call AECreateOSAEvent to create the Apple Event Record*/
  /* AECreateOSAEvent function creates an Apple Event with */
  /* only the specified attributes and no parameters.        */
  /***********************************************************/

  returnID = kAutoGenerateReturnID;  /* AE MGR should generate unique ID */
  transactionID = kAnyTransactionID;  /* not part of a transaction */

  rc = AECreateOSAEvent(aeEventClass,  /* Attribute that identifies a   */
                                         /* group of related Apple events */
                         aeEventID,      /* Attribute that identifies the */
                                         /* particular Apple event within */
                                         /* it's event class              */
                         targetaddr,    /* Target application - The one  */
                                         /* to receive the Apple Event    */
                         returnID,       /* the return ID of the Apple Event */
                                         /* which associates this Apple Event*/
                                         /* with the server's reply          */
                         transactionID,  /* transaction Id - use to indicate */
                                         /* that an Apple Event is one of a  */
                                         /* sequence of Apple Events related */
                                         /* to a single transaction          */
                         theOSAEvent);/* Buffer for the returned Apple Event */

  /* Return the result */
  return(rc);
}

/**************************************************************************
 *
 *  Name       : GetFileNames
 *
 *  Description: Bring up standard file dialog to obtain a list of filenames.
 *
 *  Concepts:    Use of Standard File Dialog
 *
 *  API's      : WinFileDlg
 *
 *  Parameters : PSZ pfilelist = pointer to a fully-qualified filename.
 *               ULONG uNumFiles
 *
 *  Return     :  success or failure
 *************************************************************************/

ULONG GetFileNames (HWND hwnd, PSZ pfilelist,ULONG uNumFiles)
{
   PMAIN_PARM pmp;

   /* obtain the main parameter pointer from window words */
   pmp = (PMAIN_PARM)WinQueryWindowULong( hwnd, QWL_USER );

  WinFileDlg( HWND_DESKTOP, hwnd, &pmp->filedlg );

  /* Return new file name  */
  strcpy(pfilelist ,pmp->szNextFilename );
  uNumFiles = 1;
  return TRUE;
}

/**************************************************************************
 *
 *  Name       : CreateAEFileList
 *
 *  Description: Create a descriptor record containing list of filenames
 *               NOTE.. Apple uses a descriptor record containing a list
 *               alias records and uses a desc type of "typeAlias".
 *               OS/2 does not have alias records, typeAlias, or an Alias
 *               Manager, and therefore we differ in the implementation.
 *               SUGGESTED WORKAROUND:
 *               In OS/2 land, we will use typeFSS instead of typeAlias.
 *               Apple has defined typeFSS as File system specification
 *               and Apple's File Manager uses this structure for specifying
 *               files and directories.
 *               We will not use FSSpec structure for typeFSS but instead
 *               follow the DosOpen filenaming convention of PSZ (pointer
 *               to a string) as OS/2's FSSpec.
 *
 *  Concepts:    Use of AECreateDesc
 *
 *  API's      : AECreateDesc
 *               AECreateList
 *               AEPutDesc
 *               AEPutParamDesc
 *               AEDisposeDesc
 *
 *  Parameters :
 *                theOSAEvent = the buffer which contains the apple event
 *
 *
 *  Return     :  success or failure
 *************************************************************************/

ULONG CreateAEFileList (HWND hwnd,
            OSAEvent * theOSAEvent,  /* apple event buffer */
            PSZ pFilename,
            ULONG uNumFiles)
{
  OSErr    rc = noErr;
  AEDescList FileDescList; /* The descriptor list of Filenames */
  AEDesc aeDescrRec;   /* the descriptor record containing the filename */
  DescType typeCode;

  /**********************************************************************/
  /* Step 1: Use AECreateList to create an empty descriptor list        */
  /**********************************************************************/

  rc = AECreateList(NULL,               /* No isolation of common data */
                     /* the book says NIL should be used */
                     /* but include file does not have this */
                    0,                   /* must be 0, if no common data */
                    FALSE,               /* Create a descriptor list */
                    &FileDescList);      /* The Descriptor list */



  /**********************************************************************/
  /* Step 2: Use AECreateDesc for each filename (in our simple case,    */
  /* there is only one filename). AECreateDesc takes a descriptor type  */
  /* and a pointer to data and converts them into a descriptor record.  */
  /**********************************************************************/

  if (rc == noErr) {

    /***********************************************************/
    /* Call AECreateDesc to get an descriptor record of typeFSS*/
    /***********************************************************/
    typeCode = typeFSS;
    rc = AECreateDesc( typeCode,
                       pFilename,
                       CCHMAXPATH,
                       &aeDescrRec);

  /**********************************************************************/
  /* Step 3: Use AEPutDesc for each filename ( in our simple case,      */
  /* there is only one filename). AEPutDesc adds a descriptor record    */
  /* to a descriptor list.                                              */
  /**********************************************************************/

   AEPutDesc(&FileDescList,
             (ULONG)1,       /* an index of 1 */
             &aeDescrRec);

  /***********************************************************/
  /* Call AEDisposeDesc to dispose of the descriptor record  */
  /* (for the filename).                                     */
  /***********************************************************/
    AEDisposeDesc(&aeDescrRec);

  /**********************************************************************/
  /* Step 4: Use AEPutParamDesc to place the descriptor list containing */
  /* the filenames into the direct parameter of the Apple Event.        */
  /**********************************************************************/

  AEPutParamDesc(theOSAEvent,
                 keyDirectObject,
                 &FileDescList);

  /***********************************************************/
  /* Call AEDisposeDesc to dispose of the descriptor list    */
  /***********************************************************/
    AEDisposeDesc(&FileDescList);


  }
  /* Return the result */
  return rc;
}


/**************************************************************************
 *
 *  Name       : SendOSAEvent
 *
 *  Description: Send an Apple Event Record
 *               Send an Apple Event Record by calling AESend
 *               The sendMode parm will always default to kAENoReply (client
 *               doesn't want reply)
 *
 *  Concepts:    Use of AESend
 *
 *  API's      : AESend
 *
 *  Parameters :
 *                targetaddr =  Address Descriptor for target application
 *                theOSAEvent = the buffer which contains the apple event
 *
 *  Return     :  success or failure
 *************************************************************************/


ULONG SendOSAEvent( OSAEvent * theOSAEvent)  /* apple event buffer */
{
  OSErr    rc = noErr;
  OSAEvent *reply = 0;
  AESendMode sendMode;
  AESendPriority sendPriority;
  long timeOutInTicks;

   /***********************************************************/
   /* Call AESend to send the Apple Event Record              */
   /*                                                         */
   /***********************************************************/

   sendMode = kAENoReply;         /* Client doesn't want reply */
   sendPriority = kAENormalPriority; /* Apple event is put at the back of event queue */
   timeOutInTicks = kAEDefaultTimeout;  /* tell AE Mgr to provide an appropriate timeout duration */

   rc = AESend (theOSAEvent,
                reply,
                sendMode,
                sendPriority,
                timeOutInTicks,
                NULL,
                NULL);

  /* Return the result */
  return(rc);

}   /* end of procedure - SendOSAEvent */

/***************************************************************************/
/*           R E S P O N D I N G   T O   A P P L E   E V E N T S           */
/***************************************************************************/



/**************************************************************************
 *
 *  Name       : ProcessSemanticEvent
 *
 *  Description: Process the Apple Semantic Event
 *
 *  Concepts:    Semantic Events
 *
 *  API's      : AEProcessOSAEvent
 *
 *  Parameters :  hwnd = window handle
 *                msg  = message code
 *                mp1  = first message parameter
 *                mp2  = second message parameter
 *
 *  Return     :  depends on message sent
 *************************************************************************/

ULONG ProcessSemanticEvent( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  /* The pointer to the SemanticEvent is in mp1 */
  return AEProcessOSAEvent((SemanticEvent *)mp1);

} /* End of Procedure - ProcessSemanticEvent */



/***************************************************************************/
/*                    E V E N T    H A N D L E R S                         */
/***************************************************************************/


/**************************************************************************
 *
 *  Name       : OpenAppProc
 *
 *  Description: Process the Open Application Event
 *
 *  Concepts:     Event Handler
 *
 *  API's      :
 *
 *  Parameters :  theEvent = pointer to the Apple Event
 *                 theReply = pointer to the reply event
 *                 refCon = reference constant
 *  Return     :  depends on message sent
 *
 *************************************************************************/

OSErr APIENTRY OpenAppProc( const OSAEvent   *theEvent,
                            OSAEvent     *theReply,
                            long          refCon )
{
   OSErr                           rc = noErr;


#ifdef OSA_DEBUG
   WinMessageBox( HWND_DESKTOP,
                  HWND_DESKTOP,
                  "received Open Application OSA Event",
                  "Open Application Event Handler",
                  (USHORT)0,
                  MB_OK | MB_NOICON);
#endif
   return(rc);
}
/**************************************************************************
 *
 *  Name       : QuitAppProc
 *
 *  Description: Process the Quit Application Event
 *
 *  Concepts:     Event Handler
 *
 *  API's      :
 *
 *  Parameters :  theEvent = pointer to the Apple Event
 *                 theReply = pointer to the reply event
 *                 refCon = reference constant
 *  Return     :  depends on message sent
 *
 *************************************************************************/

OSErr APIENTRY QuitAppProc( const OSAEvent   *theEvent,
                            OSAEvent     *theReply,
                            long          refCon )
{
   OSErr                           rc = noErr;
   PTIB  ptib;  /* Address of pointer to the Thread Info Block */
   PPIB  ppib;  /* Address of pointer to the Process Info Block */
   PID ourPID;  /* Our PID */
   HWND hwnd;
   PMAIN_PARM       pmp;


#ifdef OSA_DEBUG
   WinMessageBox( HWND_DESKTOP,
                  HWND_DESKTOP,
                  "received Quit Application OSA Event",
                  "Quit Application Event Handler",
                  (USHORT)0,
                  MB_OK | MB_NOICON);
#endif

   /**********************************************************************/
   /* Call AEGetHWND to get our window handle so we can access our data.              */
   /**********************************************************************/
   DosGetInfoBlocks (&ptib,&ppib);
   ourPID = ppib->pib_ulpid;
   rc = AEGetHWND(ourPID,  &hwnd);
   pmp = (PMAIN_PARM) WinQueryWindowULong( hwnd, QWL_USER );

   /* Tell Client window(Thread 1) to start closing down the application */
   WinPostMsg( pmp->hwndClient,WM_CLOSE, (MPARAM)(pmp->hwndClient), (MPARAM)0 );

   return(rc);
}

/**************************************************************************
 *
 *  Name       : OpenDocProc
 *
 *  Description: Process the Open Document Event
 *
 *  Concepts:     Event Handler
 *
 *  API's      :
 *
 *  Parameters :  theEvent = pointer to the Apple Event
 *                 theReply = pointer to the reply event
 *                 refCon = reference constant
 *  Return     :  depends on message sent
 *
 *************************************************************************/

OSErr APIENTRY OpenDocProc( const OSAEvent   *theEvent,
                            OSAEvent     *theReply,
                            long          refCon )
{
   AEDescList FileDescList; /* The descriptor list of Filenames */
   long  numDocs;
   ULONG i;
   AEKeyword keyword;
   DescType returnType;
   Size  maxPath = CCHMAXPATH;
   Size  sizePath = 0;
   CHAR  pFilename[ CCHMAXPATH ];
   OSErr                           rc = noErr;
   PTIB  ptib;  /* Address of pointer to the Thread Info Block */
   PPIB  ppib;  /* Address of pointer to the Process Info Block */
   PID ourPID;  /* Our PID */
   HWND hwnd;
   PMAIN_PARM       pmp;

   pFilename[0] = 0;

#ifdef OSA_DEBUG
   WinMessageBox( HWND_DESKTOP,
                  HWND_DESKTOP,
                  "received Open Document OSA Event",
                  "Open Document Event Handler",
                  (USHORT)0,
                  MB_OK | MB_NOICON);
#endif

   /**********************************************************************/
   /* Call AEGetHWND to get our window handle so we can access our data.              */
   /**********************************************************************/
   DosGetInfoBlocks (&ptib,&ppib);
   ourPID = ppib->pib_ulpid;
   rc = AEGetHWND(ourPID,  &hwnd);
   pmp = (PMAIN_PARM) WinQueryWindowULong( hwnd, QWL_USER );

   /**********************************************************************/
   /* Call AEGetParamDesc to extract the Descriptor list for the list of filename records */
   /* specified in the direct parameter of the Open Documents event.                      */
   /**********************************************************************/

   rc= AEGetParamDesc(theEvent,
                          keyDirectObject,
                          typeAEList,
                          &FileDescList);

   if (rc==noErr)
   {

     /**********************************************************************/
     /* Call AECountItems to return the number of filename records in the descriptor list    */
     /**********************************************************************/

     rc = AECountItems(&FileDescList, &numDocs);

     /**********************************************************************/
     /* Call AEGetNthPtr to return a copy of the filename record from the desriptor list,    */
     /* coerce the returned data to an FSSpec record, and open the associated file          */
     /**********************************************************************/
     for (i =1; i <= numDocs; i++)
     {

       rc = AEGetNthPtr(&FileDescList, i, typeFSS, &keyword, &returnType, pFilename, maxPath, &sizePath);
       pmp->ulNextMode =  MODE_UNKNOWN;
       ValidateFilename(pmp, pFilename ,pmp->hwndClient);


     } /* end of for loop */


    } else {
      WinMessageBox( HWND_DESKTOP,
                       HWND_DESKTOP,
                       "FAILED",
                       "AEGetParamDesc",
                       (USHORT)0,
                       MB_OK | MBID_ERROR);

    } /* endif */


  /***********************************************************/
  /* Call AEDisposeDesc to dispose of the descriptor list    */
  /***********************************************************/
   AEDisposeDesc(&FileDescList);


   return(rc);
}

/**************************************************************************
 *
 *  Name       : PrintDocProc
 *
 *  Description: Process the Print Document Event
 *
 *  Concepts:     Event Handler
 *
 *  API's      :
 *
 *  Parameters :  theEvent = pointer to the Apple Event
 *                 theReply = pointer to the reply event
 *                 refCon = reference constant
 *  Return     :  depends on message sent
 *
 *************************************************************************/

OSErr APIENTRY PrintDocProc(const  OSAEvent   *theEvent,
                            OSAEvent     *theReply,
                            long          refCon )
{
   DescType returnType;
   Size  sizePath;
   CHAR  pFilename[ CCHMAXPATH ];
   OSErr                           rc = noErr;
   PTIB  ptib;  /* Address of pointer to the Thread Info Block */
   PPIB  ppib;  /* Address of pointer to the Process Info Block */
   PID ourPID;  /* Our PID */
   HWND hwnd;
   PMAIN_PARM       pmp;

#ifdef OSA_DEBUG
   WinMessageBox( HWND_DESKTOP,
                  HWND_DESKTOP,
                  "received Print Document OSA Event",
                  "Print Document Event Handler",
                  (USHORT)0,
                  MB_OK | MB_NOICON);
#endif

     /**********************************************************************/
     /* Call AEGetHWND to get our window handle so we can access our data.              */
     /**********************************************************************/
     DosGetInfoBlocks (&ptib,&ppib);
     ourPID = ppib->pib_ulpid;
     rc = AEGetHWND(ourPID,  &hwnd);
     pmp = (PMAIN_PARM) WinQueryWindowULong( hwnd, QWL_USER );


   rc= AEGetParamPtr(theEvent,
                     keyDirectObject,
                     typeChar,
                     &returnType,
                     pFilename,
                     CCHMAXPATH,
                     &sizePath);

   if (strcmp(pFilename,pmp->szFilename) == 0) {
     /* The request is to print the currently loaded file */
     /* Call the Print routine */
     Print(pmp->hwndClient, pmp);

   } else {
     /* The request is to print another file other then the currently loaded file */
     /*  This is not supported at this time, so return errAEEventNotHandled    */
     rc = errAEEventNotHandled;
   } /* endif */

   return(rc);
}



