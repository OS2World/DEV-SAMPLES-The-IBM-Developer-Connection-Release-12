/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                                          ³*/
/*³ PROGRAM NAME: PMSPY                                                      ³*/
/*³ -------------                                                            ³*/
/*³  A PM program that is used to look at or 'spy' on the message queue of   ³*/
/*³  other PM applications windows.                                          ³*/
/*³                                                                          ³*/
/*³ COPYRIGHT:                                                               ³*/
/*³ ----------                                                               ³*/
/*³  Copyright (C) International Business Machines Corp., 1992               ³*/
/*³                                                                          ³*/
/*³ DISCLAIMER OF WARRANTIES:                                                ³*/
/*³ -------------------------                                                ³*/
/*³  The following [enclosed] code is sample code created by IBM Corporation.³*/
/*³  This sample code is not part of any standard IBM product and is provided³*/
/*³  to you solely for the purpose of assisting you in the development of    ³*/
/*³  your applications.  The code is provided "AS IS", without warranty of   ³*/
/*³  any kind.  IBM shall not be liable for any damages arising out of your  ³*/
/*³  use of the sample code, even if they have been advised of the           ³*/
/*³  possibility of such damages.                                            ³*/
/*³                                                                          ³*/
/*³ For details on what this program does etc., please see the PMSPY.C file. ³*/
/*³                                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ PMSPYHK.C                                                                ³*/
/*³                                                                          ³*/
/*³ Program to spy on other windows message queues                           ³*/
/*³                                                                          ³*/
/*³                                                                          ³*/
/*³ Notes:                                                                   ³*/
/*³                                                                          ³*/
/*³ 1. This code MUST be in a DLL since we "hook" into the system queue!     ³*/
/*³                                                                          ³*/
/*³ 2. There are TWO types of routines in this module:                       ³*/
/*³                                                                          ³*/
/*³    a) Routines called directly by PM...these are the "hook" routines     ³*/
/*³                                                                          ³*/
/*³       SpyInputHookProc                                                   ³*/
/*³       SpySendMsgHookProc                                                 ³*/
/*³                                                                          ³*/
/*³    b) Routines called only by PMSPY instances                            ³*/
/*³                                                                          ³*/
/*³       SpyRegister                                                        ³*/
/*³       SpyDeRegister                                                      ³*/
/*³                                                                          ³*/
/*³       SpySetTarget                                                       ³*/
/*³       SpyUnSetTarget                                                     ³*/
/*³       SpyQueryTargetWindow                                               ³*/
/*³       SpyQueryTargetQueue                                                ³*/
/*³                                                                          ³*/
/*³       SpySetTargetIsWindow                                               ³*/
/*³       SpyQueryTargetIsWindow                                             ³*/
/*³                                                                          ³*/
/*³       SpySetTargetMsgRange                                               ³*/
/*³                                                                          ³*/
/*³       SpyDllVersion                                                      ³*/
/*³       SpyQueryDllRegisterMax                                             ³*/
/*³                                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Includes                                                                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define INCL_BASE
#define INCL_GPI
#define INCL_WIN

#include <os2.h>                        /* the PM header file           */

#include <stdio.h>
#include "pmspyhk.h"

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Local data structures, macros, etc                                       ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
typedef struct
{
  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Control Flag: TRUE --> this item is actively SPYing                    ³*/
  /*³               FALSE--> this item marks end of the variable length list ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  BOOL      fSpyingActive;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Define who we're SPYing on                                             ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  HWND      hwndTarget;                 /* which Window */
  HMQ        hmqTarget;                 /* which Queue */

  BOOL      fTargetIsWindow;            /* SPYing on Window (vs. Queue) */

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Additional control data                                                ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  HWND      hwndSpy;                    /* destination SPY window */
  MSG        MsgSpy;                    /* PM message to send with SPY data */

  MSG       msgLow,                     /* MSG range to pass on to SPYee */
            msgHigh;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ QMSG data for passing onto PMSPY instances                             ³*/
  /*³                                                                        ³*/
  /*³ - 'normalized' data passed regardless of "hook" source                 ³*/
  /*³ - NOTE: we can do this because each message is really 'single-threaded'³*/
  /*³         through this DLL because WinSendMsg is used to:                ³*/
  /*³         (a) invoke the "hook" routines and                             ³*/
  /*³         (b) invoke PMSPY to 'spy' the MSG                              ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  QMSG      qMsg;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Define who we're SPYing on                                             ³*/
  /*³                                                                        ³*/
  /*³ - used to not "spy" on PMSPY agents which causes PM recursion          ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  HMQ        hmqSpy;                    /* PM queue of destination SPY window */

}   dllSPY,
  *PdllSPY;

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Macros to generate initialized Spys[] data elements                      ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define spyEOT {BOOL_FALSE,                            \
                SPY_BAD_HWND,SPY_BAD_HMQ,BOOL_FALSE,   \
                SPY_BAD_HWND,WM_NULL,                  \
                0,0xFFFF}

#define spyDEF {BOOL_TRUE,                             \
                SPY_BAD_HWND,SPY_BAD_HMQ,BOOL_FALSE,   \
                SPY_BAD_HWND,WM_NULL,                  \
                0,0xFFFF}

#define ID_NOT_DEFINED   0xFFFF

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Macro to validate a specified SPY instance ID                            ³*/
/*³                                                                          ³*/
/*³ - if the ID is not valid, a RETURN is generated using the specified value³*/
/*³ - if the ID is     valid, set pointer to ID's data                       ³*/
/*³                                                                          ³*/
/*³ - Tests for Validity:                                                    ³*/
/*³   a) ID is less than current 1-origin count of PMPSY instances           ³*/
/*³   b) ID is defined in Id2Spys[]                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define IF_BAD_ID_RETURN_ELSE_SET(idSpy,retValue,pSpy)               \
if ( (idSpy < uSpyInstances) && (Id2Spys[idSpy] != ID_NOT_DEFINED) ) \
  pSpy = &Spys[ Id2Spys[idSpy] ];                                    \
else                                                                 \
  return(retValue);

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Localized DLL data for controlling using PMSPY instances                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static USHORT  uSpyInstances = 0;             /* # active SPYee(s) */
static HMODULE hmodDLL       = 0;             /* 'handle' of PMSPYHK.DLL */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Translate a "PMSPY instance ID" (0..n) to it's supporting data           ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static USHORT Id2Spys[] =
{
  ID_NOT_DEFINED,       /* Instance #1 data */
  ID_NOT_DEFINED,       /* Instance #2 data */
  ID_NOT_DEFINED,       /* Instance #3 data */
  ID_NOT_DEFINED,       /* Instance #4 data */
  ID_NOT_DEFINED,       /* Instance #5 data */
  ID_NOT_DEFINED,       /* Instance #6 data */
  ID_NOT_DEFINED,       /* Instance #7 data */
  ID_NOT_DEFINED        /* Instance #8 data */
};

#define MAX_SPYEES    ( sizeof(Id2Spys) / sizeof(Id2Spys[0]) )

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Supporting SPY data                                                      ³*/
/*³                                                                          ³*/
/*³ Implementation Notes:                                                    ³*/
/*³ ---------------------                                                    ³*/
/*³ 1) because this table is searched sequentially each time one of our      ³*/
/*³    "hook" procedures is called to handle a MSG, we want to only search   ³*/
/*³    as many instances as we have currently active SPYees.  So, we keep    ³*/
/*³    table 'packed' as tight as possible.  By that, i mean that any time   ³*/
/*³    a PMSPY instance DeRegisters, we move all data elements below it      ³*/
/*³    up one slot.  Within a "hook" procedure we only 'look' at the first   ³*/
/*³    N items (where N is the number of active SPYees)                      ³*/
/*³                                                                          ³*/
/*³ 2) to maintain the SPYEE value returned when a PMPSY instance Registers, ³*/
/*³    we also maintain Id2Spys[].                                           ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static dllSPY Spys[] =
{
  spyEOT,               /* room for Instance #1 */
  spyEOT,               /* room for Instance #2 */
  spyEOT,               /* room for Instance #3 */
  spyEOT,               /* room for Instance #4 */
  spyEOT,               /* room for Instance #5 */
  spyEOT,               /* room for Instance #6 */
  spyEOT,               /* room for Instance #7 */
  spyEOT,               /* room for Instance #8 */

  spyEOT                /* permanent End-Of-Table marker */
};

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Initialized dllSPY items for adding/deleting Spys[] items                ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static const dllSPY defSPY = spyDEF;
static const dllSPY defEOT = spyEOT;

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Input Hook procedure                                                     ³*/
/*³                                                                          ³*/
/*³ This procedure gets calls after a MSG is removed from an application's   ³*/
/*³ queue before being returned by a WinGetMsg or WinPeekMsg                 ³*/
/*³                                                                          ³*/
/*³ Notes:                                                                   ³*/
/*³ ------                                                                   ³*/
/*³ 1) we are called by PM, not a PMSPY instance.  This implies that we      ³*/
/*³    don't know which PMSPY instance, if any, should SPY this message.     ³*/
/*³    That's why we have to scan the complete Spys[] table and route it     ³*/
/*³    to all interested SPYees...                                           ³*/
/*³ 2) we always return FALSE so that any other hooks "downstream"           ³*/
/*³    can also have a chance at this message                                ³*/
/*³                                                                          ³*/
/*³ Reference: PM Programming Reference: Volume Two,                         ³*/
/*³            Chapter 10 - "Functions Supplied by Application"              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

#pragma linkage (SpyInputHookProc, system)

BOOL SpyInputHookProc(HAB   habSpy_NotUsed,
                      PQMSG pQmsg,
                      BOOL  bRemove_NotUsed)
{
  register PdllSPY pSpy;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Check each of the 'active' SPYees to see if interested in this message ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  for(/* Initialize */ pSpy = &Spys[0];      /* Start @ first Spyee */
      /* While      */ pSpy->fSpyingActive;  /* Not at End-Of-List */
      /* Iterate    */ pSpy++)               /* Move to next Spyee */
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Is this MSG within the active range?                                 ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    if ( (pQmsg->msg < pSpy->msgLow) || (pQmsg->msg > pSpy->msgHigh) )
      continue;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Is this MSG destined for the correct Window?                         ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    if ( pSpy->fTargetIsWindow )               /* SPYing a specific window?   */
    {
      if (pSpy->hwndTarget != pQmsg->hwnd)     /* correct window?             */
        continue;                              /* - ignore if not             */
    }
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Is this MSG destined for the correct Queue?                          ³*/
    /*³                                                                      ³*/
    /*³ - because we now can "hook" the PM system queue, we must ensure      ³*/
    /*³   that this isn't one of our own WinSendMsg() calls that is passing  ³*/
    /*³   "hooked" data to the registered SPY "catcher" window.  Failure     ³*/
    /*³   to test for this caused PM recursion that eventually overflowed    ³*/
    /*³   the stack (trust me, it happened!)                                 ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    else
    {
      if (pSpy->hmqTarget == NULLHANDLE)       /* SPYIng the SYSTEM queue?    */
      {
                                               /* Yes: for SPY "catcher"?     */
        if (pSpy->hmqSpy == (HMQ) WinQueryWindowULong(pQmsg->hwnd,QWL_HMQ) )
          continue;                            /*      - ignore if so         */
      }
      else                                     /* No:  for specified QUEUE?   */
      {
        if (pSpy->hmqTarget != (HMQ) WinQueryWindowULong(pQmsg->hwnd,QWL_HMQ))
          continue;                            /*      - ignore if not        */
      }
    }

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Minimum eligibility requirements met...                              ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    pSpy->qMsg = *pQmsg;                    /* copy QMSG data */

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Forward to window that will handle "hooked" item                     ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    WinSendMsg(pSpy->hwndSpy,
               pSpy->MsgSpy,
               MPFROMP(&pSpy->qMsg),
               MPFROMSHORT(HK_INPUT) );     /* indicate HK source */
  }

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Indicate "pass the message to the next hook"                           ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  return(BOOL_FALSE);
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SendMsg Hook procedure                                                   ³*/
/*³                                                                          ³*/
/*³ This procedure gets calls whenever a window procedure is called          ³*/
/*³ via WinSendMsg                                                           ³*/
/*³                                                                          ³*/
/*³ Notes:                                                                   ³*/
/*³ ------                                                                   ³*/
/*³ 1) we are called by PM, not a PMSPY instance.  This implies that we      ³*/
/*³    don't know which PMSPY instance, if any, should SPY this message.     ³*/
/*³    That's why we have to scan the complete Spys[] table and route it     ³*/
/*³    to all interested SPYees...                                           ³*/
/*³                                                                          ³*/
/*³ Reference: PM Programming Reference: Volume Two,                         ³*/
/*³            Chapter 10 - "Functions Supplied by Application"              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

#pragma linkage (SpySendMsgHookProc, system)

VOID SpySendMsgHookProc(HAB        habSpy_NotUsed,
                        PSMHSTRUCT pSmh,
                        BOOL       bTask_NotUsed)
{
  register PdllSPY pSpy;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Check each of the 'active' SPYees to see if interested in this message ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  for(/* Initialize */ pSpy = &Spys[0];      /* Start @ first Spyee */
      /* While      */ pSpy->fSpyingActive;  /* Not at End-Of-List */
      /* Iterate    */ pSpy++)               /* Move to next Spyee */
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Is this MSG within the active range?                                 ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    if ( (pSmh->msg < pSpy->msgLow) || (pSmh->msg > pSpy->msgHigh) )
      continue;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Is this MSG destined for the correct Window?                         ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    if ( pSpy->fTargetIsWindow )               /* SPYing a specific window?   */
    {
      if (pSpy->hwndTarget != pSmh->hwnd)      /* ignore if wrong window?     */
        continue;
    }
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Is this MSG destined for the correct Queue?                          ³*/
    /*³                                                                      ³*/
    /*³ - because we now can "hook" the PM system queue, we must ensure      ³*/
    /*³   that this isn't one of our own WinSendMsg() calls that is passing  ³*/
    /*³   "hooked" data to the registered SPY "catcher" window.  Failure     ³*/
    /*³   to test for this caused PM recursion that eventually overflowed    ³*/
    /*³   the stack (trust me, it happened!)                                 ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    else
    {
      if (pSpy->hmqTarget == NULLHANDLE)       /* SPYIng the SYSTEM queue?    */
      {
                                               /* Yes: for SPY "catcher"?     */
        if (pSpy->hmqSpy == (HMQ) WinQueryWindowULong(pSmh->hwnd,QWL_HMQ) )
          continue;                            /*      - ignore if so         */
      }
      else                                     /* No:  for specified QUEUE?   */
      {
        if (pSpy->hmqTarget != (HMQ) WinQueryWindowULong(pSmh->hwnd,QWL_HMQ))
          continue;                            /*      - ignore if not        */
      }
    }

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Minimum eligibility requirements met...                              ³*/
    /*³                                                                      ³*/
    /*³ - standardize to QMSG data                                           ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    pSpy->qMsg.hwnd = pSmh->hwnd;
    pSpy->qMsg.msg  = pSmh->msg;
    pSpy->qMsg.mp1  = pSmh->mp1;
    pSpy->qMsg.mp2  = pSmh->mp2;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Forward to window that will handle "hooked" item                     ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    WinSendMsg(pSpy->hwndSpy,
               pSpy->MsgSpy,
               MPFROMP(&pSpy->qMsg),
               MPFROMSHORT(HK_SENDMSG) );     /* indicate HK source */
  }
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SpyHooks: set/clear PMPSY instance "hooks"                               ³*/
/*³ -------------------------------------------------------------            ³*/
/*³                                                                          ³*/
/*³ Parms:   hMQ............message queue to (un)"hook"                      ³*/
/*³          hAB............anchor block                                     ³*/
/*³          fSet...........True=set "Hooks", False=clear "hooks"            ³*/
/*³                                                                          ³*/
/*³ Returns: TRUE...........successful                                       ³*/
/*³          FALSE..........unsuccessful                                     ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static BOOL SpyHooks(HMQ   hMQ,  /* message queue to "HOOK" */
                     HAB   hAB,  /* required HAB */
                     BOOL  bSet) /* tells whether to set or clear "hooks" */

{
  register BOOL   bResult = BOOL_TRUE; /* start as a OPTIMIST... */
  register int    i;                   /* loop counter */

  static   struct {
                    SHORT  sHookID;
                    PFN    pfnHook;
                  } HookList[] =
                  {
                    {HK_INPUT,   (PFN)SpyInputHookProc   }, /* Input   "hook" */
                    {HK_SENDMSG, (PFN)SpySendMsgHookProc }  /* SendMsg "hook" */
                  };

#define NO_OF_HOOKS ( sizeof(HookList) / sizeof(HookList[0]) )

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Process each of the "Hook" items                                       ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  for(/* Initialize */  i = 0;               /* @ first array element */
      /* While      */  i < NO_OF_HOOKS;     /* while more "hooks" to process */
      /* Iterate    */  i++)
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Perform Set or Clear operation based on input parameter              ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    if ( bSet )
    {
      if ( ! WinSetHook(hAB,
                        hMQ,                      /* add "hook" on QUEUE */
                        HookList[i].sHookID,      /* "hook" type */
                        HookList[i].pfnHook,      /* "hook" procedure */
                        hmodDLL) )                /* "hook" procedure DLL handle */
        bResult = BOOL_FALSE;
    }
    else
    {
      if (! WinReleaseHook(hAB,
                           hMQ,                  /* remove "hook" from QUEUE */
                           HookList[i].sHookID,  /* "hook" type */
                           HookList[i].pfnHook,  /* "hook" procedure */
                           hmodDLL) )            /* "hook" procedure DLL handle */
        bResult = BOOL_FALSE;
    }
  }

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Return the calculated result                                           ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  return( bResult );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SpyRegister: register a new PMPSY instance for SPYing                    ³*/
/*³ -------------------------------------------------------------------------³*/
/*³                                                                          ³*/
/*³ Parms:   hwndSpy.........window to be sent MSGs of SPY activity          ³*/
/*³           MsgSpy.........PM message to send to SPY window with "hooked"  ³*/
/*³                                                                 data     ³*/
/*³ Returns: 0-n.............registered SPY instance (used on other 'Spy'    ³*/
/*³                                                                 calls)   ³*/
/*³          SPY_BAD_SPYEE...unable to register new SPY instance (all        ³*/
/*³                          instances already in use)                       ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
SPYEE  SpyRegister(HWND hwndSpy,  /* window to handle SPY data */
                  MSG   MsgSpy)   /* PM message for "hooked" data */

{
  register SPYEE idSpy = SPY_BAD_SPYEE;    /* start as a PESSEMIST... */

  char           errbuf[80];

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Action taken is based on how SPYee are already active                  ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  switch ( uSpyInstances )
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ No SPYee is active yet:                                              ³*/
    /*³                                                                      ³*/
    /*³ - load PMSPYHK.DLL for PM-OS/2 usage while "hooking"                 ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    case 0:

         if (DosLoadModule(errbuf, sizeof(errbuf), SPY_HOOK_DLL, &hmodDLL) != 0 )
           break;

         /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
         /*³ ** fall through to add new instance **                          ³*/
         /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Some SPYees are already active, just add to table                    ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    default:

         idSpy = uSpyInstances;     /* Save this instance # (0-origin) */

         uSpyInstances++;           /* Bump # active SPYees */

         /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
         /*³ Add to Instance-to-SPY data translation table                   ³*/
         /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
         Id2Spys[idSpy] = idSpy;

         /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
         /*³ Add to SPY data table                                           ³*/
         /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
         Spys[idSpy]         = defSPY;     /* default all fields */

         Spys[idSpy].hwndSpy = hwndSpy;    /* who to communicate "hooked" */
         Spys[idSpy].MsgSpy  = MsgSpy;     /* items to                    */

                                           /* agent's HMQ                 */
         Spys[idSpy].hmqSpy  = (HMQ) WinQueryWindowULong(hwndSpy, QWL_HMQ);
    break;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ All SPYee instances are already active                               ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    case MAX_SPYEES:

         idSpy = SPY_BAD_SPYEE;
    break;
  }

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Return the registration result                                         ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  return( idSpy );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SpyDeRegister: de-register a existing PMPSY instance from SPYing         ³*/
/*³ --------------------------------------------------------------------     ³*/
/*³                                                                          ³*/
/*³ Parms:   idSpy...........ID of the PMSPY instance to de-register         ³*/
/*³                                                                          ³*/
/*³ Returns: TRUE............instance successfully de-registered             ³*/
/*³          FALSE...........instance NOT          de-registered (bad ID     ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL SpyDeRegister(SPYEE   idSpy)    /* which SPY instance */

{
  register PdllSPY pSpy;
  register USHORT  i;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Validate that the ID is valid - if not immediate return is performed   ³*/
  /*³                               - if so, pSpy will locate it's data      ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  IF_BAD_ID_RETURN_ELSE_SET(idSpy,BOOL_FALSE,pSpy);

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ First, insure SPYing "hook" is removed...                              ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  SpyUnSetTarget(idSpy);

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Action taken is based on how many SPYee are already active             ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  switch ( uSpyInstances )
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ This is the last SPYee....so free "hook" DLL (we loaded on registering)*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    case 1:

         DosFreeModule(hmodDLL);
         hmodDLL = 0;

         /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
         /*³ ** fall through to delete instance **                           ³*/
         /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Some SPYees will remain active, just delete this ID from tables      ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    default:

         uSpyInstances--;           /* Decrement # active SPYees (1-origin) */

         /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
         /*³ Pack all SPY data table items below this one up one slot        ³*/
         /*³                                                                 ³*/
         /*³ Note: we're intentionally moving the MAX_SPYEES element since   ³*/
         /*³       if exists and is always an EOT element                    ³*/
         /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
         for(/* Initialize */ i = idSpy;       /* Start at this ID */
             /* While      */ i <= MAX_SPYEES; /* While all below not moved */
             /* Iterate    */ i++)             /* keep marching... */
         {
            Spys[i] = Spys[i + 1];
         }

         /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
         /*³ Remove from Instance-to-SPY data translation table              ³*/
         /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
         Id2Spys[idSpy] = ID_NOT_DEFINED;

         /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
         /*³ Adjust Id2Spy[] to locate 'Packed' SPY data table               ³*/
         /*³                                                                 ³*/
         /*³ - we only do this if Spys[] data element was below the          ³*/
         /*³   SPYee we've just deleted and packed up a slot                 ³*/
         /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
         for(/* Initialize */ i = 0;           /* Start at first ID */
             /* While      */ i < MAX_SPYEES;  /* While all not processed */
             /* Iterate    */ i++)             /* keep marching... */
         {
            if ( (Id2Spys[i] != ID_NOT_DEFINED)  &&   /* defined? */
                 (Id2Spys[i] >  idSpy) )              /*below deleted ID? */
              Id2Spys[i]--;                           /*yes: adjust 1 slot */
         }

    break;
  }

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Return that the ID was successfully de-registered                      ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  return( BOOL_TRUE );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SpySetTarget: set new target window and queue (and begin SPYing)         ³*/
/*³ ----------------------------------------------------------------         ³*/
/*³                                                                          ³*/
/*³ Parms:   idSpy...........ID of desired PMSPY instance                    ³*/
/*³          hwndTarget......target Window to now SPY                        ³*/
/*³           hmqTarget......target Queue  to now SPY                        ³*/
/*³                                                                          ³*/
/*³ Returns: TRUE............target successfully set                         ³*/
/*³          FALSE...........target NOT          set (bad ID)                ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL SpySetTarget(register SPYEE idSpy,      /* SPY instance */
                           HWND  hwndTarget, /* Window to SPY */
                           HMQ   hmqTarget)  /* Queue  to SPY */

{
  register PdllSPY pSpy;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Validate that the ID is valid - if not immediate return is performed   ³*/
  /*³                               - if so, pSpy will locate it's data      ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  IF_BAD_ID_RETURN_ELSE_SET(idSpy,BOOL_FALSE,pSpy);

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Set instance's new Window and Queue                                    ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  pSpy->hwndTarget = hwndTarget;
  pSpy-> hmqTarget =  hmqTarget;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Set "hook" on this Queue (and return result to caller)                 ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  return( SpyHooks(hmqTarget,                          /* msg queue to "HOOK" */
                   WinQueryAnchorBlock(pSpy->hwndSpy), /* little ol' HAB */
                   BOOL_TRUE) );                       /* set them hooks... */
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³SpyUnSetTarget: unset instance's target window and queue (and free "hook")³*/
/*³--------------------------------------------------------------------------³*/
/*³                                                                          ³*/
/*³Parms:   idSpy...........ID of the PMSPY instance to Un-Set               ³*/
/*³                                                                          ³*/
/*³Returns: TRUE............target successfully Un-Set                       ³*/
/*³         FALSE...........target NOT          Un-Set (bad ID)              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL SpyUnSetTarget(register SPYEE idSpy) /* SPY instance */

{
  register BOOL    bResult = BOOL_TRUE;        /* default to OPTIMIST result */
  register PdllSPY pSpy;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Validate that the ID is valid - if not immediate return is performed   ³*/
  /*³                               - if so, pSpy will locate it's data      ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  IF_BAD_ID_RETURN_ELSE_SET(idSpy,BOOL_FALSE,pSpy);

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Clear "hook" on this instance's QUEUE (if one was actively being SPYed)³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  if ( pSpy->hmqTarget != SPY_BAD_HMQ )
    bResult = SpyHooks(pSpy->hmqTarget,                    /* msg queue to "HOOK" */
                       WinQueryAnchorBlock(pSpy->hwndSpy), /* little ol' HAB */
                       BOOL_FALSE);                        /* clear them hooks... */

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Set instance's new Window and Queue to "invalid" values                ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  pSpy->hwndTarget = SPY_BAD_HWND;
  pSpy->hmqTarget  = SPY_BAD_HMQ;

  return( bResult );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SpyQueryTargetWindow: query instance's target window                     ³*/
/*³ -------------------------------------------------------------            ³*/
/*³                                                                          ³*/
/*³ Parms:   idSpy...........ID of desired PMSPY instance                    ³*/
/*³                                                                          ³*/
/*³ Returns: !SPY_BAD_HWND...window successfully queried                     ³*/
/*³          SPY_BAD_HWND....window NOT          queried (bad ID)            ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
HWND SpyQueryTargetWindow(register SPYEE idSpy) /* SPY instance */

{
  register PdllSPY pSpy;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Validate that the ID is valid - if not immediate return is performed   ³*/
  /*³                               - if so, pSpy will locate it's data      ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  IF_BAD_ID_RETURN_ELSE_SET(idSpy,SPY_BAD_HWND,pSpy);

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Return instance's Window                                               ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  return( pSpy->hwndTarget );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SpyQueryTargetQueue: query instance's target queue                       ³*/
/*³ -----------------------------------------------------------              ³*/
/*³                                                                          ³*/
/*³ Parms:   idSpy...........ID of desired PMSPY instance                    ³*/
/*³                                                                          ³*/
/*³ Returns: !SPY_BAD_HMQ....queue successfully queried                      ³*/
/*³          SPY_BAD_HMQ.....queue NOT          queried (bad ID)             ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
HMQ SpyQueryTargetQueue(register SPYEE idSpy) /*SPY instance */

{
  register PdllSPY pSpy;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Validate that the ID is valid - if not immediate return is performed   ³*/
  /*³                               - if so, pSpy will locate it's data      ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  IF_BAD_ID_RETURN_ELSE_SET(idSpy,SPY_BAD_HMQ,pSpy);

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Return instance's Queue                                                ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  return( pSpy->hmqTarget );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SpySetTargetIsWindow: set that this instance is SPYing the Window/Queue  ³*/
/*³ -----------------------------------------------------------------------  ³*/
/*³                                                                          ³*/
/*³ Parms:   idSpy...........ID of desired PMSPY instance                    ³*/
/*³          fTargetIsWindow.TRUE=SPY on window, FALSE=SPY on queue          ³*/
/*³                                                                          ³*/
/*³ Returns: TRUE............successful                                      ³*/
/*³          FALSE...........NOT successful (bad ID)                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL SpySetTargetIsWindow(register SPYEE idSpy,      /* SPY instance */
                                            BOOL  fTargetIsWindow)  /* SPYing on window? */

{
  register PdllSPY pSpy;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Validate that the ID is valid - if not immediate return is performed   ³*/
  /*³                               - if so, pSpy will locate it's data      ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  IF_BAD_ID_RETURN_ELSE_SET(idSpy,BOOL_FALSE,pSpy);

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Return instance's Queue                                                ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  pSpy->fTargetIsWindow = fTargetIsWindow;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Return that is worked!                                                 ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  return( BOOL_TRUE );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SpyQueryTargetIsWindow: query if this instance is SPYing the Window/Queue³*/
/*³ -------------------------------------------------------------------------³*/
/*³                                                                          ³*/
/*³ Parms:   idSpy...........ID of desired PMSPY instance                    ³*/
/*³                                                                          ³*/
/*³ Returns: TRUE............SPYing window                                   ³*/
/*³          FALSE...........SPYing queue -or- bad ID                        ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL SpyQueryTargetIsWindow(register SPYEE idSpy) /* SPY instance */

{
  register PdllSPY pSpy;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Validate that the ID is valid - if not immediate return is performed   ³*/
  /*³                               - if so, pSpy will locate it's data      ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  IF_BAD_ID_RETURN_ELSE_SET(idSpy,BOOL_FALSE,pSpy);

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Return instance's SPY state                                            ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  return( pSpy->fTargetIsWindow );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SpySetTargetMsgRange: set instance's MSG interest range                  ³*/
/*³ ------------------------------------------------------------------------ ³*/
/*³                                                                          ³*/
/*³ Parms:   idSpy...........ID of desired PMSPY instance                    ³*/
/*³          msgLow..........low  MSG in inclusive range of desired MSGs     ³*/
/*³          msgHigh.........high MSG in inclusive range of desired MSGs     ³*/
/*³                                                                          ³*/
/*³ Returns: TRUE............MSG interest range set                          ³*/
/*³          FALSE...........MSG interest range NOT set (bad ID or MSG range ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL SpySetTargetMsgRange(register SPYEE idSpy,      /* SPY instance */
                                   MSG   msgLow,     /* lowest  MSG in range */
                                   MSG   msgHigh)    /* highest MSG in range */

{
  register PdllSPY pSpy;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Validate that the ID is valid - if not immediate return is performed   ³*/
  /*³                               - if so, pSpy will locate it's data      ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  IF_BAD_ID_RETURN_ELSE_SET(idSpy,BOOL_FALSE,pSpy);

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Validate specified MSG range: low must really be lower or the same!    ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  if ( msgLow <= msgHigh )
  {
    pSpy->msgLow  = msgLow;
    pSpy->msgHigh = msgHigh;

    return( BOOL_TRUE );
  }
  else
    return( BOOL_FALSE );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SpyDllVersion: Return the Version of the SpyDll Module                   ³*/
/*³ ------------------------------------------------------                   ³*/
/*³                                                                          ³*/
/*³ Parms:   NONE                                                            ³*/
/*³                                                                          ³*/
/*³ Returns: numeric version ID of this PMSPY support ID                     ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
USHORT SpyDllVersion(VOID)
{
  return( DLLVERSION );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ SpyQueryDllRegisterMax: Return maximum number of concurrent PMSPY instances³*/
/*³ -------------------------------------------------------------------------³*/
/*³                                                                          ³*/
/*³ Parms:   NONE                                                            ³*/
/*³                                                                          ³*/
/*³ Returns: maximum number of PMSPY instances that can be registered        ³*/
/*³ concurrently                                                             ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
USHORT SpyQueryDllRegisterMax(VOID)
{
  return( MAX_SPYEES );
}
