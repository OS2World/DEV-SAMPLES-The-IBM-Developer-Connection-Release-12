/***************************************************************************
 *
 * File name   :  osaevnt.c
 *
 * Description :  This program contains the OSA code
 *
 *                This source file contains the following functions:
 *                InitOSA
 *                TerminateOSA
 *                ProcessSemanticEvent
 *                RecordingPRoc
 ******************************************************************************/

/* os2 includes */
#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS
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
#include "osatest.h"
#include "osadlg.h"

/***************************************************************************/
/*    OSA Event Interface Routines                                         */
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
    * Install the recording handler
    * Ensure AEInstallEventHandler worked ok; if not, present a message box.
    * ( See pmassert.h. )
    */

   rc = AEInstallEventHandler( kOSASuite,
                               kOSARecordedText,
                               (AEEventHandlerUPP)RecordingProc,
                               0L,
                               FALSE);
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

/* Remove the recording handler */

   rc = AERemoveEventHandler( kOSASuite,
                              kOSARecordedText,
                              (AEEventHandlerUPP)RecordingProc,
                              FALSE );
   /* Terminate Apple Event Processing */
   rc = AETerminate();
}

/***************************************************************************/
/*    Apple Event Generator and related functions                          */
/***************************************************************************/




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
 *  Name       : RecordingProc
 *
 *  Description: Process the Recorded Text Event
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

OSErr APIENTRY RecordingProc(const  OSAEvent   *theEvent,
                             OSAEvent     *theReply,
                             long          refCon )
{
   DescType returnType;
   Size  size;
   CHAR  pRecText[ CCHMAXPATH ];
   OSErr rc = noErr;
   PTIB  ptib;  /* Address of pointer to the Thread Info Block */
   PPIB  ppib;  /* Address of pointer to the Process Info Block */
   PID ourPID;  /* Our PID */
   HWND hwnd;
   PMAIN_PARM       pmp;

   /**********************************************************************/
   /* Call AEGetHWND to get our window handle so we can access our data.              */
   /**********************************************************************/
   DosGetInfoBlocks (&ptib,&ppib);
   ourPID = ppib->pib_ulpid;
   rc = AEGetHWND(ourPID,  &hwnd);
   pmp = (PMAIN_PARM) WinQueryWindowULong( hwnd, QWL_USER );

   if (pmp->hwndScriptEditor)
   {
     rc= AEGetParamPtr(theEvent,
                     keyDirectObject,
                     typeChar,
                     &returnType,
                     pRecText,
                     CCHMAXPATH,
                     &size);

     /* get window handle and post message */
     WinSendMsg(pmp->hwndScriptEditor, WM_ADD_RECORDEDTEXT,MPFROMP(pRecText),MPFROMLONG(size));
   }
   return(rc);
}



/**************************************************************************
 *
 *  Name       : RawEventRecordingProc
 *
 *  Description: Process the Raw Recorded Event
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

OSErr APIENTRY RawEventRecordingProc(const  OSAEvent   *theEvent,
                                     OSAEvent     *theReply,
                                     long          refCon )
{
   DescType returnType;
   Size  size;
   CHAR  pRecText[ CCHMAXPATH ];
   OSErr rc = noErr;
   PTIB  ptib;  /* Address of pointer to the Thread Info Block */
   PPIB  ppib;  /* Address of pointer to the Process Info Block */
   PID ourPID;  /* Our PID */
   HWND hwnd;
   PMAIN_PARM       pmp;

   /**********************************************************************/
   /* Call AEGetHWND to get our window handle so we can access our data.              */
   /**********************************************************************/
   DosGetInfoBlocks (&ptib,&ppib);
   ourPID = ppib->pib_ulpid;
   rc = AEGetHWND(ourPID,  &hwnd);
   pmp = (PMAIN_PARM) WinQueryWindowULong( hwnd, QWL_USER );

   if (pmp->hwndRecordEvent)
   {
     /* get window handle and post message */
     WinSendMsg(pmp->hwndRecordEvent, WM_ADD_RECORDEDEVENT,MPFROMP(theEvent),MPFROMLONG(NULL));
   }
   return(rc);
}
