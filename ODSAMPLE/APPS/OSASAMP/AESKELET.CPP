
/***************************************************************************
 *
 * File name   :  aeskelet.c
 *
 * Description :  This program contains the Fake Apple Event skeleton
 *
 *                This source file contains the following functions:
 *
 *
 * Required
 *    files    :
 *
 *  Concepts   :  Apple Events
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

/* Include the required sections from the PM header files.  */
#define INCL_DEV
#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS
#define INCL_GPIMETAFILES
#define INCL_GPISEGMENTS
#define INCL_GPITRANSFORMS
#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_WINDIALOGS
#define INCL_WINFRAMEMGR
#define INCL_WINERRORS
#define INCL_WINHELP
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINPOINTERS
#define INCL_WINSCROLLBARS
#define INCL_WINSTDDRAG
#define INCL_WINSTDFILE
#define INCL_WINSTDFONT
#define INCL_WINSYS
#define INCL_WINTIMER
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

/* application includes  */
#include "pmassert.h"
#include "prtsamp.h"
#include "prtevent.h"

/**********************************************/
/* The following is defined in aemutil.h      */
/*  OSErr, Linkage, and noErr                 */
/* Note.. aemutil.h is included in aemproto.h */
/**********************************************/

   AEEventID ProcessThisAEEventID;

OSErr APIENTRY AEInit(ULONG windowHandle, char* appName, BOOL acceptRemoteEvents)

{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AEInit was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);
  return (noErr);
}

OSErr APIENTRY AETerminate(void)
{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AETerminate was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);
  return (noErr);
}

/*컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/



OSErr APIENTRY AEInstallEventHandler    (AEEventClass theAEEventClass,
                                        AEEventID theAEEventID,
                                        AEEventHandlerUPP handler,
                                        long handlerRefcon,
                                        BOOL isSysHandler)
{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AEInstallEventHandler was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);
  return (noErr);
}
OSErr APIENTRY AERemoveEventHandler     (AEEventClass theAEEventClass,
                                        AEEventID theAEEventID,
                                        AEEventHandlerUPP handler,
                                        BOOL isSysHandler)
{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AERemoveEventHandler was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);
  return (noErr);
}

OSErr APIENTRY AEProcessAppleEvent(const  SemanticEvent *theSemanticEvent)

{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AEProcessAppleEvent was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);
  return (noErr);
}

OSErr APIENTRY AESend      (const AppleEvent *theAppleEvent,
                           AppleEvent *reply,
                           AESendMode sendMode,
                           AESendPriority sendPriority,
                           long timeOutInTicks,
                           AEIdleUPP idleProc,
                           AEFilterUPP filterProc)
{

 BOOL       bOK;
 long handlerRefcon = 1;

   switch( ProcessThisAEEventID )
   {
     case kAEOpenApplication:


      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AESend - Open App Event Received ",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);
/* TEST for the client processing WM_SEMANTICEVENT
   bOK = WinPostMsg( globalPMP->hwndClient, WM_SEMANTICEVENT, (MPARAM)0L, (MPARAM)0L ); */

       OpenAppProc( theAppleEvent,
                     reply,
                     handlerRefcon);
       break;

     case kAEOpenDocuments:


      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AESend - Open Doc Event Received ",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);

       OpenDocProc( theAppleEvent,
                     reply,
                     handlerRefcon);
       break;

     case kAEPrintDocuments:
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AESend - Print Doc Event Received ",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);

       PrintDocProc( theAppleEvent,
                     reply,
                     handlerRefcon);
       break;

     case kAEQuitApplication:

      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AESend - Quit App Event Received ",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);

       QuitAppProc( theAppleEvent,
                     reply,
                     handlerRefcon);
       break;

   }

  return (noErr);
}


OSErr APIENTRY AECreateAppleEvent   (AEEventClass theAEEventClass,
                                    AEEventID theAEEventID,
                                    const AEAddressDesc *target,
                                    short returnID,
                                    long transactionID,
                                    AppleEvent *result)
{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AECreateAppleEvent was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);

  /* Save the Event ID for use in processing AESend requests */
  /* This is more faking code                                */
  ProcessThisAEEventID = theAEEventID;

  return (noErr);
}

OSErr APIENTRY AEPutParamDesc        (const AppleEvent *theAppleEvent,
                                     AEKeyword theAEKeyword,
                                     const AEDesc *theAEDesc)
{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AEPutParamDesc was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);
  return (noErr);
}

OSErr APIENTRY AEPutParamPtr   (const AppleEvent *theAppleEvent,
                               AEKeyword theAEKeyword,
                               DescType typeCode,
                               const void *dataPtr,
                               Size dataSize)
{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AEPutParamPtr was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);
  return (noErr);
}

OSErr APIENTRY AECreateDesc   (DescType typeCode,
                              const void *dataPtr,
                              Size dataSize,
                              AEDesc *result)

{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AECreateDesc was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);
  return (noErr);
}

OSErr APIENTRY AECreateList(const void *factoringPtr,
                            Size factoredSize,
                            BOOL isRecord,
                            AEDescList *resultList)
{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AECreateList was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);
  return (noErr);
}

OSErr APIENTRY AEPutDesc(const AEDescList *theAEDescList,
                         long index,
                         const AEDesc *theAEDesc)
{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AEPutDesc was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);
  return (noErr);
}

OSErr APIENTRY AEDisposeDesc(AEDesc *theAEDesc);
OSErr APIENTRY AEDisposeDesc(AEDesc *theAEDesc)

{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AEDisposeDesc was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);
  return (noErr);
}


OSErr APIENTRY AEGetParamDesc(const AppleEvent *theAppleEvent,
                              AEKeyword        theAEKeyword,
                              DescType         desiredType,
                              AEDesc           *result);


OSErr APIENTRY AEGetParamDesc(const AppleEvent *theAppleEvent,
                              AEKeyword        theAEKeyword,
                              DescType         desiredType,
                              AEDesc           *result)

{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AEGetParamDesc was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);
  return (noErr);
}



OSErr APIENTRY AECountItems(const AEDescList *theAEDescList, long *theCount);
OSErr APIENTRY AECountItems(const AEDescList *theAEDescList, long *theCount)

{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AECountItems was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);

  /* Set the count of records to 1 */
  *theCount = 1;
  return (noErr);
}


OSErr APIENTRY AEGetNthPtr(const AEDescList *theAEDescList, long index, DescType desiredType, AEKeyword *theAEKeyword, DescType *typeCode, void *dataPtr, Size maximumSize, Size *actualSize);
OSErr APIENTRY AEGetNthPtr(const AEDescList *theAEDescList, long index, DescType desiredType, AEKeyword *theAEKeyword, DescType *typeCode, void *dataPtr, Size maximumSize, Size *actualSize)


{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AEGetNthPtr was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);

   switch( ProcessThisAEEventID )
   {

     case kAEOpenDocuments:
       /* FAKE OUT- use the pmp->sznextfilename as the next file to open (dq1017) */
       strcpy((char *) dataPtr ,globalPMP->szNextFilename);          /* (dq1017) */
       break;

     case kAEPrintDocuments:
       /* FAKE OUT- use the pmp->szfilename as the file to print (dq1017) */
       strcpy((char *) dataPtr , globalPMP->szFilename);      /* (dq1017) */
       break;

   }

  return (noErr);
}

OSErr APIENTRY AEGetParamPtr(const AppleEvent *theAppleEvent,
                              AEKeyword       theAEKeyword,
                              DescType        desiredType,
                              DescType        *typeCode,
                              void            *dataPtr,
                              Size            maximumSize,
                              Size            *actualSize);

OSErr APIENTRY AEGetParamPtr(const AppleEvent *theAppleEvent,
                              AEKeyword       theAEKeyword,
                              DescType        desiredType,
                              DescType        *typeCode,
                              void            *dataPtr,
                              Size            maximumSize,
                              Size            *actualSize)


{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AEGetParamPtr was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);

   switch( ProcessThisAEEventID )
   {

     case kAEOpenDocuments:
       /* FAKE OUT- use the pmp->sznextfilename as the next file to open (dq1017) */
       strcpy((char *) dataPtr , globalPMP->szNextFilename);          /* (dq1017) */
       break;

     case kAEPrintDocuments:
       /* FAKE OUT- use the pmp->szfilename as the file to print (dq1017) */
       strcpy((char *) dataPtr , globalPMP->szFilename);      /* (dq1017) */
       break;

   }

  return (noErr);
}


/*---------------------------------------------------------------------------*/
/*   This call is used to obtain the PID for a running process.              */
/*---------------------------------------------------------------------------*/
OSErr APIENTRY AEGetPID(char* appName, PID* thePID);
OSErr APIENTRY AEGetPID(char* appName, PID* thePID)
{
      WinMessageBox( HWND_DESKTOP,
                     HWND_DESKTOP,
                     (PSZ)"AEGetPID was received",
                     (PSZ)"AE MANAGER",
                     (USHORT)0,
                     MB_OK | MB_NOICON);


  return (noErr);
}
