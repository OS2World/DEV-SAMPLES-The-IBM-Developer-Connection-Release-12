
/**************************************************************************
 *  File name  :  prtevent.h
 *
 *  Description:  The PRTEVENT include file defines symbolic constants
 *                and data structures used to send and receive osa events.
 *
 *  Concepts   :  [none]
 *
 *  API's      :  [none]
 *
 *  Copyright (C) 1991 IBM Corporation
 *
 *      DISCLAIMER OF WARRANTIES.  The following [enclosed] code is
 *      sample code created by IBM Corporation. This sample code is not
 *      part of any standard or IBM product and is provided to you solely
 *      for  the purpose of assisting you in the development of your
 *      applications.  The code is provided "AS IS", without
 *      warranty of any kind.  IBM shall not be liable for any damages
 *      arising out of your use of the sample code, even if they have been
 *      advised of the possibility of such damages.                                                    *
 *************************************************************************/

/*************************************************************/
/* Compile Options                                                             */
/*************************************************************/
#define OSA_AWARE   /* specifies you want to make OSA calls */ 
#define OSA_DEBUG    /* specifies you want to display OSA debugging messages */

/*************************************************************/
/* Name of Application that will be used in AEInit call                      */
/*************************************************************/

#ifdef OSA_CLIENT
#define APP_NAME             "PRTSAMPC.EXE"  
#define TARGET_APP          "PRTSAMP.EXE"    
#else
#define APP_NAME             "OSA PRINT SAMPLE"  
#define TARGET_APP          NULL    
#endif

/*************************************************************/
/* Defines for Apple Event Types                             */
/*************************************************************/

#define IDM_OSAEvent             40
#define IDM_AEOPENAPP              41
#define IDM_AEOPENDOC              42
#define IDM_AEPRINTDOC             43
#define IDM_AEQUITAPP              44


/*  Apple Event Interface Prototypes */
void InitOSA(HWND hwnd);
void TerminateOSA(HWND hwnd);

/* Apple Event Generator and related functions */
ULONG GenerateOSAEvent( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
ULONG CreateAEAddrDescr (AEAddressDesc *targetaddr);   /* Address Descriptor for */
ULONG CreateAERecord(short aeEventID,        /* Event type */
                         AEAddressDesc *targetaddr,   /* Address Descriptor for */
                                                     /* target application     */
                         OSAEvent *theOSAEvent);  /* apple event buffer */
ULONG CreateAEFileList (HWND hwnd,
            OSAEvent * theOSAEvent, /* apple event buffer */
            PSZ pFilename,
            ULONG uNumFiles);
ULONG GetFileNames (HWND hwnd, PSZ pfilelist,ULONG uNumFiles);
ULONG SendOSAEvent( OSAEvent * theOSAEvent); 

/* Responding to Apple Events */
ULONG ProcessSemanticEvent( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

/* Apple Event Handlers Prototypes */
OSErr APIENTRY  OpenAppProc( const OSAEvent     *theEvent,
                             OSAEvent     *theReply,
                             long          refCon);

OSErr APIENTRY QuitAppProc( const OSAEvent   *theEvent,
                            OSAEvent     *theReply,
                            long          refCon );

OSErr APIENTRY OpenDocProc( const OSAEvent   *theEvent,
                            OSAEvent     *theReply,
                            long          refCon );

OSErr APIENTRY PrintDocProc(const OSAEvent   *theEvent,
                            OSAEvent     *theReply,
                            long          refCon );


/* Idle Function */
BOOL EXPENTRY IdleProc( SemanticEvent *event,
                         long *sleepTime,
                         AERgnHandle *mouseRgn);

/* Reply Filter Function */

BOOL EXPENTRY ReplyFilterProc(SemanticEvent *event,
                          long returnID,
                          long transactionID,
                          const AEAddressDesc *sender);


