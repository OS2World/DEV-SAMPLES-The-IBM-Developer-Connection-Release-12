/******************************************************************************/
/* The following code fragments were published as part of                     */
/* The Developer Connection News  Volume II                                   */
/*                                                                            */
/* COPYRIGHT:                                                                 */
/* ----------                                                                 */
/* Copyright (C) International Business Machines Corp., 1993.                 */
/*                                                                            */
/* DISCLAIMER OF WARRANTIES:                                                  */
/* -------------------------                                                  */
/* The following [enclosed] code is sample code created by IBM                */
/* Corporation.  This sample code is not part of any standard IBM product     */
/* and is provided to you solely for the purpose of assisting you in the      */
/* development of your applications.  The code is provided "AS IS",           */
/* without warranty of any kind.  IBM shall not be liable for any damages     */
/* arising out of your use of the sample code, even if they have been         */
/* advised of the possibility of such damages.                                */
/*                                                                            */
/******************************************************************************/


#define INCL_DOSMVDM
#define INCL_BASE
#define INCL_DOSPROCESS
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "sty_main.h"
#include "sty_xtrn.h"
#include "sty_dlg.h"
#include "common.h"


/*
 *Any data types that are private to this module
 */


/*
 *any data declarations that are private to this module
 */
ULONG  HandleToVDD;
MESSAGEBUFFER MessageBuffer;

/****************************************************************************/
/*The following code fragments were enclosed due to erros in The Developer  */
/*Connection News that we were not able to correct before press time.       */
/*The following routines will be expanded and incoporated into the final    */
/*release of the source code that will be released in a future release of   */
/*The Developer Connection News - David Kenner                              */
/****************************************************************************/
/****************************************************************************/
/*OpenVirtQueue                                                             */
/*                                                                          */
/*                                                                          */
/*A service routine that will allow us to do our initilization with the vdd */
/****************************************************************************/
APIRET OpenVirtQueue(VOID)
{
     APIRET apiRet;

     /*
      *see if our Vdd is alive and if so get
      *a handle that we may use later
      */
     if(apiRet = DosOpenVDD(VDDNAME,&HandleToVDD) )
     {
          return(apiRet);
     }

     /*
      *Call into our Vdd so we perform any initialization
      *we need to do at this time.
      */
     if(apiRet  = DosRequestVDD(HandleToVDD,
                                0,
                                INIT_COMMAND,
                                0,
                                NULL,
                                0,
                                NULL) )
     {
          return(apiRet);
     }
     return(apiRet);
}
/****************************************************************************/
/*WriteVirtQueue                                                            */
/*                                                                          */
/*                                                                          */
/*A service routine that will allow us to send our message data to the  vdd */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

APIRET WriteVirtQueue(PVOID pvMessageToSend,
                      ULONG ulCount,
                      SGID  SessionId)
{
     APIRET apiRet;
     MESSAGEBUFFER MessageBuffer;
     PTIB pptib;
     PPIB ppib;

     memset(&MessageBuffer,0,sizeof(MessageBuffer));
     DosGetInfoBlocks(&pptib,&ppib);

     MessageBuffer.ulMessageType  = TEXT_MESSAGE;
     MessageBuffer.ulBufferSize   = strlen(pvMessageToSend);
     MessageBuffer.pvMessageData  = pvMessageToSend;
     MessageBuffer.ulSrcSessionId = ppib->pib_ulppid;

     if(apiRet = DosRequestVDD(HandleToVDD,
                               SessionId,
                               POST_MESSAGE,
                               ulCount,
                               pvMessageToSend,
                               0,
                               NULL) )
     {
          return(apiRet);
     }
     return(apiRet);
}

/****************************************************************************/
/*ReadVirtQueue                                                             */
/*                                                                          */
/*                                                                          */
/*This routine is set up as separate thread that will read messages,process */
/*them,and then continue to block until additional messages come in.The     */
/*block mechansim is accomplished through the semaphore in our VDD          */
/*                                                                          */
/*Note:                                                                     */
/*There is no error return. All error messages are handled before the       */
/*thread terminates                                                         */
/*                                                                          */
/****************************************************************************/
VOID ReadVirtQueue(VOID)
{
     APIRET apiRet;
     PVOID  pvMessage;
     CHAR   CommandToSend[SIZ_COMMAND_BUF];



     do
     {
          /*
           *block until we get any message
           *data
           */
          if(apiRet = DosRequestVDD(HandleToVDD,
                                    0,
                                    READ_QUEUE,
                                    strlen(CommandToSend),
                                    CommandToSend,
                                    sizeof(MessageBuffer),
                                    &MessageBuffer) )
          {
               break;
          }
          /*
           *do all of our message processing
           */
           apiRet = ProcessMessageData(MessageBuffer);

     }while(!apiRet);
     /*
      *handle any error that was  encountered
      *during our polling
      */


     /*
      *do any other clean up we need to do
      *and get out
      */

     _endthread();
}

/****************************************************************************/
/*Provide a stub to keep the linker from complaining                        */
/*                                                                          */
/*                                                                          */
/*This is currently just a stub routine                                     */
/****************************************************************************/
#if 0
INT main(void)
{
     APIRET apiRet;


     do
     {

          if(OpenVirtQueue() )
          {
               break;
          }
          /*
           *start off our read thread
           */
           apiRet = StartReadThread();

     }ONCE;
     return(0);

}
#endif

/****************************************************************************/
/*Process any message data we get from our vdd                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
APIRET ProcessMessageData(MESSAGEBUFFER MessageBuffer)
{

     APIRET apiRet;
     CHAR   szBuffer[512];

     do
     {
          memset(szBuffer,0,sizeof(szBuffer) );
          sprintf(szBuffer,"MessageType:%ul,MessageSubCommand:%ul,Message:%s",
                            MessageBuffer.ulMessageType,
                            MessageBuffer.ulMessageSubCommand,
                            (PSZ)MessageBuffer.pvMessageData);

          /*
           *display the message
           *to the mle
           */
          DisPlayInComingMessage(szBuffer);

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
APIRET StartReadThread(VOID)
{
    APIRET ApiRet;


     if( (ApiRet = _beginthread(ReadVirtQueue,
                          NULL,
                          8192,
                          NULL)) == -1)
     {
          return(TRUE);
     }


     return(FALSE);
}

/****************************************************************************/
/****************************************************************************/
/*OpenVirtQueue                                                             */
/*                                                                          */
/*                                                                          */
/*A service routine that will allow us to do our initilization with the vdd */
/****************************************************************************/
APIRET CloseVirtQueue(VOID)
{
     APIRET apiRet;

     /*
      *Call into our Vdd so we perform any initialization
      *we need to do at this time.
      */
     if(apiRet  = DosRequestVDD(HandleToVDD,
                                0,
                                TERMINATE_COMMAND,
                                0,
                                NULL,
                                0,
                                NULL) )
     {
          return(apiRet);
     }
     return(apiRet);
}

