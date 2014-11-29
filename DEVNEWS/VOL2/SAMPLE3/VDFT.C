/******************************************************************************/
/*                                                                            */
/*                     IBM Sample Virtual Device Driver                       */
/*                                                                            */
/*                 Copyright (c) IBM Corporation 1993                         */
/*                         All Rights Reserved                                */
/*                                                                            */
/*   DISCLAIMER OF WARRANTIES.  The following [enclosed] code is              */
/*   sample code created by IBM Corporation. This sample code is not          */
/*   part of any standard or IBM product and is provided to you solely        */
/*   for  the purpose of assisting you in the development of your             */
/*   applications.  The code is provided "AS IS", without                     */
/*   warranty of any kind.  IBM shall not be liable for any damages           */
/*   arising out of your use of the sample code, even if they have been       */
/*   advised of the possibility of such damages.                              */
/*                                                                            */
/******************************************************************************/
/**************************** END OF SPECIFICATIONS ***************************/

#define INCL_VDH
#define NULL 0

#include <mvdm.h>                       /* VDH services, etc.        */
#include <basemid.h>                    /* message numbers           */
#include <builtin.h>                    /* builtin functions         */
#include "VDFTP.H"                      /* Local Stuff               */
#include "common.h"

/*                                                                   */
/*                 GLOBAL DATA AREA FOR VDFT                         */
/*                                                                   */
#define DEBUG
HIRQ hirq_DFT = 0;                      /* holds handle for EOI hook */

HVDM HVDM_Owner = 0;                    /* hvdm of owner             */
INT   Status_byte;                      /* Status Register value     */
PMESSAGE pMessage = NULL;

HVDHSEM SemOS2Handle = 0;
HVDHSEM SemWinHandle = 0;
#pragma data_seg(CSWAP_DATA)


#pragma data_seg()

#pragma data_seg(SWAPINSTDATA)

HVDM  HVDM_Current;                     /* Current VDM handle  */
VDFTPDB PDB_Current;                    /* Current PDB         */

#pragma data_seg()

#pragma data_seg(CINIT_DATA)

char szPDDName[]=DFT_NAME;

#pragma data_seg()

#pragma alloc_text(CSWAP_TEXT, VDFTDaIn, VDFTStIn, VDFTDaOut)
/*
 *local function prototypes go here
 */

LONG EXPENTRY OS2Service(SGID sgid,ULONG ulCommand,ULONG ulCountIn,
                         PVOID pvDataIn,ULONG ulCountout,
                         PVOID pvDataOut);
LONG OS2ReadQueue(SGID sgid,ULONG ulCountout,
                    PVOID pvDataOut);

BOOL OS2TerminateService(VOID);
BOOL OS2PostMessage(SGID SessionId,ULONG ulCountIn,PVOID pvDataIn);
BOOL OS2InitSetup(VOID);
BOOL OS2ReadMessageData(SGID sgid,ULONG ulCountout,PVOID pvDataOut);
PMESSAGE GetNextLink(VOID);

#pragma alloc_text(CINIT_TEXT, Start_Here)
//#pragma alloc_text(CINIT_TEXT, VDDInit)

/********************** START OF SPECIFICATIONS ***********************/
/*                                                                    */
/* SUBROUTINE NAME:  VDFTInit (Start_here)                            */
/*                                                                    */
/* DESCRIPTIVE NAME:  Virtual DFT Initialization entry point          */
/*                                                                    */
/* FUNCTION:  The function of this routine is to register the VDM     */
/*            Creation, Termination, PDB Change, and PDB Destroy      */
/*            handlers. This routine also sets up the VDFT Port       */
/*            Address table in the global data area.                  */
/*                                                                    */
/* NOTES:                                                             */
/*                                                                    */
/* CONTEXT: System Initialization                                     */
/*                                                                    */
/* ENTRY POINT:  VDFTInit                                             */
/*    LINKAGE:  CALL NEAR 32                                          */
/*                                                                    */
/* INPUT: psz - pointer to configuration strings                      */
/*                                                                    */
/* EXIT-NORMAL: returns !0                                            */
/*                                                                    */
/* EXIT-ERROR: returns 0                                              */
/*                                                                    */
/* EFFECTS: VDFT Port Address table in the global data area.          */
/*                                                                    */
/* INTERNAL REFERENCES:  NONE                                         */
/*                                                                    */
/* EXTERNAL REFERENCES:  VDHInstallUserHook - Install VDM event hook  */
/*                       VDHOpenPDD         - Get PDD entry point     */
/*                       VDHOpenVIRQ        - register an IRQ for VDM */
/*                       VDHReservePages    - set up some linear      */
/*                                 space for use as the adapter space */
/************************ END OF SPECIFICATIONS ***********************/
#pragma entry(Start_Here)
BOOL EXPENTRY Start_Here(PSZ psz)
{

     ULONG ErrorNumber;                 /* holds error number from getError*/
#ifdef DEBUG
     _interrupt(3);
#endif
     if(!VDHRegisterVDD(VDDNAME,OS2Service,NULL))
     {
       return(FALSE);
     }
     return TRUE;
}
/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:OS2Service
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *          -
 *
 *          -
 *          -
 *
 *          -
 *          -
 *
 *  Returns:
 *          1 - if sucessful execution completed
 *          0 - if error
\****************************************************************/
LONG EXPENTRY OS2Service(SGID sgid,ULONG ulCommand,ULONG ulCountIn,
                         PVOID pvDataIn,ULONG ulCountout,
                         PVOID pvDataOut)

{

     LONG apiRet;

#ifdef DEBUG
     _interrupt(3);
#endif
     do
     {
          switch(ulCommand)
          {
          case INIT_COMMAND:
               apiRet= OS2InitSetup();
               break;
          case POST_MESSAGE:
               apiRet=OS2PostMessage(sgid,ulCountIn,pvDataIn);
               break;

          case READ_QUEUE:
               apiRet= OS2ReadQueue(sgid,ulCountout,pvDataOut);
               break;
          case TERMINATE_COMMAND:
               OS2TerminateService();
               break;
          default:
               apiRet = TRUE;
               break;
          }


     }ONCE;
     return(apiRet);
}
/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *          -
 *
 *          -
 *          -
 *
 *          -
 *          -
 *
 *  Returns:
 *          1 - if sucessful execution completed
 *          0 - if error
\****************************************************************/
LONG OS2ReadQueue(SGID sgid,ULONG ulCountout,
                    PVOID pvDataOut)
{

#ifdef DEBUG
     _interrupt(3);
#endif
     do
     {

          /*
           *wait until we get cleared
           *and if so assume we have a new message waiting for us
           */
          if(!VDHWaitEventSem(SemOS2Handle,
                          SEM_INDEFINITE_WAIT) )
          {
               return(TRUE);
          }
          /*
           *reset the semaphore for the next time
           *through
           */
          VDHResetEventSem(SemOS2Handle);
          /*
           *retrieve the message data
           *and store it in pvDataOut
           */
          if(OS2ReadMessageData(sgid,ulCountout,pvDataOut) )
          {
               return(TRUE);
          }

     }ONCE;
     return(FALSE);

}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *          -
 *
 *          -
 *          -
 *
 *          -
 *          -
 *
 *  Returns:
 *          1 - if sucessful execution completed
 *          0 - if error
\****************************************************************/
BOOL OS2InitSetup(VOID)
{

#ifdef DEBUG
     _interrupt(3);
#endif

     /*
      *if we have not created our structure yet
      */
     if(!pMessage)
     {

         /*
          *allocate our control
          *structure for our messagelist
          */
          if( ( pMessage = VDHAllocMem(sizeof(MESSAGE),
                                        VDHAM_SWAPPABLE) ) == NULL)
          {
               return(TRUE);
          }
     }
     pMessage->SrcSessionIdentifier = -1;
     pMessage->DstSessionIdentifier = -1;
     pMessage->ulMessageType        = 0;
     pMessage->ulMessageSubCommand  = 0;
     pMessage->ulBufferSize         = 0;
     pMessage->pvMessageData    = NULL;
     pMessage->pvNextMessage    = NULL;
     /*
      *create our event semaphore for the
      *read thread from the OS/2 process
      */

     if(!VDHCreateSem(&SemOS2Handle,
                     VDH_EVENTSEM) )
     {
          return(TRUE);
     }

     VDHResetEventSem(SemOS2Handle);
     return(FALSE);

}


/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *          -
 *
 *          -
 *          -
 *
 *          -
 *          -
 *
 *  Returns:
 *          1 - if sucessful execution completed
 *          0 - if error
\****************************************************************/
BOOL OS2TerminateService(VOID)
{
     BOOL fError = FALSE;

#ifdef DEBUG
     _interrupt(3);
#endif

     do
     {

          /*
           *post our semaphore
           */
          VDHPostEventSem(SemOS2Handle);
          /*
           *and get rid of it
           */
          VDHDestroySem(SemOS2Handle);

     }ONCE;
     return(fError);
}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *          -
 *
 *          -
 *          -
 *
 *          -
 *          -
 *
 *  Returns:
 *          1 - if sucessful execution completed
 *          0 - if error
\****************************************************************/
BOOL OS2PostMessage(SGID SessionId,ULONG ulCountIn,PVOID pvDataIn)
{
     BOOL fError = FALSE;
     PMESSAGE pMessage;


     do
     {
          /*
           *get the next link in our list
           */
           pMessage = GetNextLink();


          /*
           *allocate some local storage
           */
           if( (pMessage = VDHAllocMem(ulCountIn,VDHAM_SWAPPABLE))== NULL)
           {
               return(TRUE);
           }

          /*
           *copy our message data in
           *and hold onto our session identifier
           */
           pMessage->SrcSessionIdentifier = SessionId;
           pMessage->ulBufferSize         = ulCountIn;
           memcpy(pMessage->pvMessageData,pvDataIn,
                  ulCountIn);


          /*
           *clear the semaphore on the Windows side
           */
          VDHPostEventSem(SemWinHandle);


     }ONCE;
     return(fError);
}
/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *          -
 *
 *          -
 *          -
 *
 *          -
 *          -
 *
 *  Returns:
 *          1 - if sucessful execution completed
 *          0 - if error
\****************************************************************/
BOOL OS2ReadMessageData(SGID sgid,ULONG ulCountout,PVOID pvDataOut)
{





     return(FALSE);
}
/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *          -
 *
 *          -
 *          -
 *
 *          -
 *          -
 *
 *  Returns:
 *          1 - if sucessful execution completed
 *          0 - if error
\****************************************************************/
PMESSAGE GetNextLink(VOID)
{
     PMESSAGE pMessageNext;

     do
     {
          /*
           *spin through our list looking for the
           *first null link
           *in our list
           */
          for(pMessageNext = (PMESSAGE)pMessage->pvNextMessage;
                         !pMessageNext;
                         pMessageNext = pMessageNext->pvNextMessage)
          {
               ;
          }


     }ONCE;
     return(pMessageNext);
}

