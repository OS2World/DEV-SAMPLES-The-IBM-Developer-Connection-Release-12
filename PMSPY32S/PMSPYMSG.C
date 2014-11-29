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
/*³ PMSPYMSG.C                                                               ³*/
/*³                                                                          ³*/
/*³ MSG and GROUP Data structure support routines                            ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ MSG Data Structure Requirements:                                         ³*/
/*³ --------------------------------                                         ³*/
/*³ 1. Fast, Random access by MSG number                                     ³*/
/*³ 2. Sequential access required (BUT not performance critical)             ³*/
/*³ 3. Dynamic Additions and Deletions                                       ³*/
/*³                                                                          ³*/
/*³ MSG Data Structure Implementation:                                       ³*/
/*³ ----------------------------------                                       ³*/
/*³                                                                          ³*/
/*³ Hash Table      Linked List                                              ³*/
/*³ (MSG_SLOT)      (MSG_DATA)                                               ³*/
/*³                 ÚÄÄÄÄÄÄÄÄÄÄÄ¿    ÚÄÄÄÄÄÄÄÄÄÄÄ¿                           ³*/
/*³ ÚÄÄÄÄÄÄÂÄÄÄ¿ ÚÄ>³         *ÄÅÄÄÄ>³         * ³                           ³*/
/*³ ³      ³ *ÄÅÄÙ  ³           ³    ³           ³                           ³*/
/*³ ÃÄÄÄÄÄÄÅÄÄÄ´    ³ÚÄÄÄÄÄÄÄÄÄ¿³    ³ÚÄÄÄÄÄÄÄÄÄ¿³                           ³*/
/*³ ³      ³   ³    ³³         ³³    ³³         ³³                           ³*/
/*³ ÃÄÄÄÄÄÄÅÄÄÄ´    ³³MSG_ITEM ³³    ³³MSG_ITEM ³³                           ³*/
/*³ ³      ³   ³    ³³         ³³    ³³         ³³                           ³*/
/*³ ÃÄÄÄÄÄÄÅÄÄÄ´    ³ÀÄÄÄÄÄÄÄÄÄÙ³    ³ÀÄÄÄÄÄÄÄÄÄÙ³                           ³*/
/*³ ³      ³   ³    ÀÄÄÄÄÄÄÄÄÄÄÄÙ    ÀÄÄÄÄÄÄÄÄÄÄÄÙ                           ³*/
/*³ ÃÄÄÄÄÄÄÅÄÄÄ´                                                             ³*/
/*³ ³      ³   ³                                                             ³*/
/*³ ÀÄÄÄÄÄÄÁÄÄÄÙ                                                             ³*/
/*³                                                                          ³*/
/*³                                                                          ³*/
/*³ * MSG number is hashed to index into MSG_SLOT table                      ³*/
/*³                                                                          ³*/
/*³ * MSG_DATA items are stored sorted in ascending order to reduce          ³*/
/*³   sequential search time                                                 ³*/
/*³                                                                          ³*/
/*³                                                                          ³*/
/*³ GROUP Data Structure Requirements:                                       ³*/
/*³ ----------------------------------                                       ³*/
/*³ 1. Sequential access required (BUT not performance critical)             ³*/
/*³ 2. Dynamic Additions and Deletions                                       ³*/
/*³                                                                          ³*/
/*³ GROUP Data Structure Implementation:                                     ³*/
/*³ ------------------------------------                                     ³*/
/*³                                                                          ³*/
/*³ List Head       Linked List                                              ³*/
/*³                 (GRP_DATA)                                               ³*/
/*³                 ÚÄÄÄÄÄÄÄÄÄÄÄ¿    ÚÄÄÄÄÄÄÄÄÄÄÄ¿                           ³*/
/*³ ÚÄÄÄÄÄÄÂÄÄÄ¿ ÚÄ>³         *ÄÅÄÄÄ>³         * ³                           ³*/
/*³ ³Head  ³ *ÄÅÄÙ  ³           ³  Ú>³           ³                           ³*/
/*³ ÃÄÄÄÄÄÄÅÄÄÄ´    ³ÚÄÄÄÄÄÄÄÄÄ¿³  ³ ³ÚÄÄÄÄÄÄÄÄÄ¿³                           ³*/
/*³ ³Tail  ³ * ³    ³³         ³³  ³ ³³         ³³                           ³*/
/*³ ÀÄÄÄÄÄÄÁÄÂÄÙ    ³³GRP_ITEM ³³  ³ ³³GRP_ITEM ³³                           ³*/
/*³          ³      ³³         ³³  ³ ³³         ³³                           ³*/
/*³          ³      ³ÀÄÄÄÄÄÄÄÄÄÙ³  ³ ³ÀÄÄÄÄÄÄÄÄÄÙ³                           ³*/
/*³          ³      ÀÄÄÄÄÄÄÄÄÄÄÄÙ  ³ ÀÄÄÄÄÄÄÄÄÄÄÄÙ                           ³*/
/*³          ³                     ³                                         ³*/
/*³          ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Includes                                                                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

#include "pmspy.h"                      /* Resource symbolic identifiers*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    InitializeMsg                                                ³*/
/*³                                                                          ³*/
/*³ Function:   Initialize passed MSG table for processing                   ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
VOID InitializeMsg(register PSPY_DATA pSpy )

{
  static MSG_SLOT defSlot = { NULL,             /* none in list yet */

                              0,                /* no messages, yet... */

                              0xFFFF,           /* no LOW  message */
                              0                 /* no HIGH message */
                            };

  register USHORT i;

  for( /* Init */ i =                           /* Start @ 1st slot */
                  pSpy->TotalMsgs = 0;

       /* Term */ i < NO_MSG_SLOTS;             /* Stop after all slots processed */

       /* Iter */ i++)                          /* On to the next slot... */

    pSpy->Msgs[i] = defSlot;
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    HashMsg                                                      ³*/
/*³                                                                          ³*/
/*³ Function:   "hash" specified MSG to determine MSG_SLOT entry             ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#if defined(HashMsgAsSubroutine)

  USHORT HashMsg(register MSG msg )

  {
    return( msg % NO_MSG_SLOTS );
  }

#else

  #define HashMsg(msg) ( msg % NO_MSG_SLOTS )

#endif

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    AddMsg                                                       ³*/
/*³                                                                          ³*/
/*³ Function:   Add specified MSG to database                                ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
VOID AddMsg(PSPY_DATA pSpy,
            PMSG_ITEM pMsg )

{
  PMSG_DATA pLast,
            pCur;

  /* Locate "home" slot for this message */

  PMSG_SLOT pSlot = &(pSpy->Msgs[ HashMsg(pMsg->Msg) ]);

  /* Allocate new slot data instance */

  PMSG_DATA pData = AllocateMemory( sizeof( MSG_DATA) );

  pData->pNext = NULL;
  pData->item  = *pMsg;

  /* Processing is based on number of messages already in list */

  switch(pSlot->uNoMsgs)
  {
    case 0:  /* this is the first message in this here slot... */

             pSlot->pFirst = pData;
             break;

    default: /* there are one or more items in list
             **
             ** decide where this one goes
             */

             for(pCur   = pSlot->pFirst,          /* start @ 1st message */
                 pLast  = (PMSG_DATA)pSlot;       /* fudge as MSG_DATA */

                 /* Terminates within loop below */ ;

                 pLast = pCur,
                 pCur  = pCur->pNext)
             {
               if ( pMsg->Msg < pCur->item.Msg )  /* insert before this message? */
               {
                 pLast->pNext = pData;            /* yes... */
                 pData->pNext = pCur;
                 break;
               }
               else if (pCur->pNext == NULL)      /* no...last in list? */
               {
                 pCur->pNext = pData;
                 break;
               }
             }
  }

  /* Update SLOT information */

  pSlot->uNoMsgs++;

  if ( pMsg->Msg < pSlot->MsgLow )
    pSlot->MsgLow = pMsg->Msg;

  if ( pMsg->Msg > pSlot->MsgHigh )
    pSlot->MsgHigh = pMsg->Msg;

  pSpy->TotalMsgs++;
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    iFindMsg (internal)                                 ³*/
/*³                                                                          ³*/
/*³ Function:   locate the MSG_DATA entry for a specific MSG                 ³*/
/*³                                                                          ³*/
/*³ Returns:     NULL...if message WAS NOT found                             ³*/
/*³             !NULL...if message WAS     found (locates MSG_DATA info)     ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static PMSG_DATA iFindMsg(         PSPY_DATA  pSpy,   /* Control data structure */
                          register MSG        Msg,    /* MSG to find */
                                   PMSG_SLOT *pSlot,  /* IFF found, 'slot' message "lives" in */
                                   PMSG_DATA *pPrev)  /* IFF found, MSG_DATA before this MSG */
{
  PMSG_DATA pCur;

  /* Locate "home" slot for this message */

  *pSlot = &(pSpy->Msgs[ HashMsg(Msg) ]);

  /* Don't even scan if MSG is not within this slot's current values */

  if ( (Msg >= (*pSlot)->MsgLow) && (Msg <= (*pSlot)->MsgHigh) )
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Scan sequentially for this message                                   ³*/
    /*³                                                                      ³*/
    /*³ Note: since MSG's are kept in sorted order, we will stop             ³*/
    /*³       when this MSG is found to not be possible                      ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    for(pCur   = (*pSlot)->pFirst,     /* start @ 1st  message */
        *pPrev = NULL;

        (pCur  != NULL) &&             /* quit  @ last message */
        (Msg   >= pCur->item.Msg);     /* or when MSG no longer possible */

        *pPrev = pCur,
        pCur   = pCur->pNext)          /* try next message */
    {
      if ( Msg == pCur->item.Msg )
        return( pCur );
    }
  }

  return( NULL );                               /* MSG was not found */
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    FindMsg                                                      ³*/
/*³                                                                          ³*/
/*³ Function:   locate the MSG_ITEM entry to be used by a specific MSG       ³*/
/*³                                                                          ³*/
/*³ Returns:     NULL...if message WAS NOT found                             ³*/
/*³             !NULL...if message WAS     found (locates MSG_ITEM info)     ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
PMSG_ITEM FindMsg(         PSPY_DATA pSpy,
                  register MSG       Msg )

{
  PMSG_SLOT pSlot;
  PMSG_DATA pMsg, pPrev;

  pMsg = iFindMsg(pSpy, Msg, &pSlot, &pPrev );

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ MSG in our database?                                                   ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  return( pMsg == NULL ? NULL
                       : &(pMsg->item) );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    FindMsgName                                                  ³*/
/*³                                                                          ³*/
/*³ Function:   locate the MSG_ITEM entry to be used by a specific MSG name  ³*/
/*³                                                                          ³*/
/*³ Returns:     NULL...if message WAS NOT found                             ³*/
/*³             !NULL...if message WAS     found (locates MSG_ITEM info)     ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
PMSG_ITEM FindMsgName(PSPY_DATA pSpy,
                      PSZ       pName)

{
  register PMSG_ITEM pMsg;

  for( /* Init */ pMsg  = ProcessFirstMsg(pSpy);   /* Start @ 1st MSG */

       /* Term */ (pMsg != NULL) &&                /* Until all MSGs processed */
                  (strcmp(pMsg->pDesc,pName)!=0); /* or name found */

       /* Iter */ pMsg = ProcessNextMsg(pSpy) );    /* Try next MSG */

  return( pMsg );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    DeleteMsg                                                    ³*/
/*³                                                                          ³*/
/*³ Function:   Delete the specified MSG                                     ³*/
/*³                                                                          ³*/
/*³ Returns:    FALSE...message WAS NOT deleted                              ³*/
/*³             TRUE....message WAS     deleted                              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL DeleteMsg(         PSPY_DATA pSpy,
               register MSG       Msg )

{
  PMSG_SLOT pSlot;
  PMSG_DATA pMsg, pPrev;

  if ( (pMsg = iFindMsg(pSpy, Msg, &pSlot, &pPrev)) == NULL)
    return( BOOL_FALSE );

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Delete this MSG from this SLOT                                         ³*/
  /*³                                                                        ³*/
  /*³ 1. Delete from this SLOT's linked list                                 ³*/
  /*³ 2. Update SLOT statitistics                                            ³*/
  /*³ 3. Update MSG  statistics                                              ³*/
  /*³ 4. Free Memory (MSG_DATA, Message Description)                         ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  if (pPrev == NULL)                            /* First in SLOT's list? */
    pSlot->pFirst = pMsg->pNext;
  else
    pPrev->pNext  = pMsg->pNext;

  pSlot->uNoMsgs--;

  FreeMemory(pMsg->item.pDesc);
  FreeMemory(pMsg);

  pSpy->TotalMsgs--;

  return( BOOL_TRUE );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    DeleteAllMsgs                                                ³*/
/*³                                                                          ³*/
/*³ Function:   Delete ALL defined MSGs                                      ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
VOID DeleteAllMsgs( PSPY_DATA pSpy )

{
  register PMSG_ITEM pMsg;

  for( /* Init */ pMsg  = ProcessFirstMsg(pSpy);  /* Start @ 1st MSG */

       /* Term */ pMsg != NULL;                   /* Until all MSGs processed */

       /* Iter */ pMsg = ProcessFirstMsg(pSpy) )  /* Restart @ (new) 1st MSG */
  {
    DeleteMsg(pSpy, pMsg->Msg);
  }
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    IncludeMsg                                                   ³*/
/*³                                                                          ³*/
/*³ Function:   mark a specific MSG as Included/Not Included                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
VOID IncludeMsg(         PSPY_DATA pSpy,
                register MSG       Msg,
                         BOOL      Include,
                         MSG_COLOR Color)

{
  register PMSG_ITEM pMsg = FindMsg(pSpy, Msg );

  if (pMsg != NULL)
  {
    pMsg->Include = Include;

    if (Color != Color_Asis)
      pMsg->ClrFG = Color;
  }
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    IncludeGroup                                                 ³*/
/*³                                                                          ³*/
/*³ Function:   process all MSGs in a 'group'; mark as Included/Not Included ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
VOID IncludeGroup  (          PSPY_DATA  pSpy,
                              PGRP_ITEM  pGrp,
                    register  BOOL       Include,
                    register  MSG_COLOR  Color
                   )
{
  PMSG_ITEM  pMsg;

  /* Process each MSG defined in this group */

  for( /* Initialize */ pMsg  = ProcessFirstMsgInGroup(pSpy,pGrp->GroupID);
       /* Terminate  */ pMsg != NULL;     /* stop when all processed */
       /* Iterate    */ pMsg  = ProcessNextMsgInGroup(pSpy)
     )
  {
    IncludeMsg(pSpy, pMsg->Msg, Include, Color);
  }

  /* Maintain GROUP's status */

  pGrp->Include = Include;

  if (Color != Color_Asis)              /* only change if instructed so... */
    pGrp->ClrFG = Color;
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    ProcessFirstMsg                                              ³*/
/*³                                                                          ³*/
/*³ Function:   setup processing to sequentially return all MSG's defined    ³*/
/*³                                                                          ³*/
/*³ Returns:     NULL......there are NO messages                             ³*/
/*³             !NULL......pointer to first MSG_ITEM                         ³*/
/*³                                                                          ³*/
/*³ Note:       - the MSG's WILL NOT be returned in sequential numeric order!³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
PMSG_ITEM ProcessFirstMsg(PSPY_DATA pSpy)
{
  /* Initialize Processing Data */

  pSpy->ProcessAllMsgs_Slot  = (USHORT) -1;  /* start 1 before 1st SLOT */
  pSpy->ProcessAllMsgs_pData = NULL;         /* say 'need to find next MSG' */

  return( ProcessNextMsg(pSpy) );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    ProcessNextMsg                                               ³*/
/*³                                                                          ³*/
/*³ Function:   Continue to sequentially return all MSG's defined            ³*/
/*³                                                                          ³*/
/*³ Returns:     NULL......there are NO messages                             ³*/
/*³             !NULL......pointer to next MSG_ITEM                          ³*/
/*³                                                                          ³*/
/*³ Note:       - the MSG's WILL NOT be returned in sequential numeric order!³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
PMSG_ITEM ProcessNextMsg(PSPY_DATA pSpy)
{
  register PMSG_DATA pData;

  /* Locate next MSG... */

  while( pSpy->ProcessAllMsgs_pData == NULL )
  {
    /* try the next SLOT */

    if ( ++pSpy->ProcessAllMsgs_Slot >= NO_MSG_SLOTS )
      return( NULL );

    pSpy->ProcessAllMsgs_pData = pSpy->Msgs[pSpy->ProcessAllMsgs_Slot].pFirst;
  }

  /* We've found the next MSG.  Prepare for our next invocation */

  pData                      = pSpy->ProcessAllMsgs_pData;
  pSpy->ProcessAllMsgs_pData = pSpy->ProcessAllMsgs_pData->pNext;

  return( &(pData->item) );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    InitializeGroup                                              ³*/
/*³                                                                          ³*/
/*³ Function:   Initialize passed GROUP table for processing                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
VOID InitializeGroup(register PSPY_DATA pSpy )
{
    pSpy->NextGroupID = 0;

    pSpy->GroupHead   =
    pSpy->GroupTail   = NULL;
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    IncludeAllMsgs                                               ³*/
/*³                                                                          ³*/
/*³ Function:   - mark all MSGs   as Included/Not Included                   ³*/
/*³             - mark all GROUPs as Included/Not Included                   ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
VOID IncludeAllMsgs(         PSPY_DATA pSpy,
                    register BOOL      Include,
                             MSG_COLOR Color)
{
  register PMSG_ITEM pMsg;
  register PGRP_ITEM pGrp;

  for( /* Initialize */  pMsg = ProcessFirstMsg(pSpy); /* start @ first MSG */
       /* Terminate  */  pMsg != NULL;                 /* stop when all processed */
       /* Iterate    */  pMsg = ProcessNextMsg(pSpy)   /* try the next MSG */
     )
  {
    IncludeMsg(pSpy, pMsg->Msg, Include, Color);
  }

  /* Update the GROUP status, also                                            */

  for(/* Initialize */ pGrp  = ProcessFirstGroup(pSpy); /* start @ 1st */
      /* Terminate  */ pGrp != NULL;                    /* @ end of list */
      /* Iterate    */ pGrp  = ProcessNextGroup(pSpy)   /* move to next */
     )
  {
    pGrp->Include = Include;
  }
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    AddGroup                                                     ³*/
/*³                                                                          ³*/
/*³ Function:   Add specified GROUP to database                              ³*/
/*³                                                                          ³*/
/*³ Returns:    GroupID of new GROUP                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
GROUP AddGroup(PSPY_DATA pSpy,
               PGRP_ITEM pGrp )
{
  PGRP_DATA pData = AllocateMemory( sizeof(GRP_DATA) );

  pData->pNext        = NULL;

  pData->item         = *pGrp;
  pData->item.GroupID = pSpy->NextGroupID++;

  if (pSpy->GroupHead == NULL)
  {
    pSpy->GroupHead =
    pSpy->GroupTail = pData;
  }
  else
  {
    pSpy->GroupTail->pNext = pData;
    pSpy->GroupTail        = pData;
  }

  return(pData->item.GroupID);
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    iFindGroup (Internal)                                        ³*/
/*³                                                                          ³*/
/*³ Function:   Find specified GROUP in database                             ³*/
/*³                                                                          ³*/
/*³ Returns:     NULL....GroupID not found                                   ³*/
/*³             !NULL....GroupID     found (this is PGRP_DATA)               ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static PGRP_DATA iFindGroup(         PSPY_DATA pSpy,
                            register GROUP     GroupID)
{
  register PGRP_DATA pData;

  /* Sequentially scan the list for this GROUP ID */

  for(/* Init */ pData = pSpy->GroupHead;            /* Start @ 1st GROUP */
      /* Term */ (pData != NULL) &&                  /* Until last GROUP seen */
                 (pData->item.GroupID != GroupID);   /* or GROUP found */
      /* Init */ pData = pData->pNext);              /* try the next one */


  return(pData );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    FindGroup                                                    ³*/
/*³                                                                          ³*/
/*³ Function:   Find specified GROUP in database                             ³*/
/*³                                                                          ³*/
/*³ Returns:     NULL....GroupID not found                                   ³*/
/*³ Returns:    !NULL....GroupID     found (this is PGRP_ITEM)               ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
PGRP_ITEM FindGroup(         PSPY_DATA pSpy,
                    register GROUP     GroupID)
{
  register PGRP_DATA pData = iFindGroup(pSpy, GroupID);

  return(pData == NULL ? NULL : &(pData->item) );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    FindGroupName                                                ³*/
/*³                                                                          ³*/
/*³ Function:   Find specified GROUP in database (by name)                   ³*/
/*³                                                                          ³*/
/*³ Returns:     NULL....Group not found                                     ³*/
/*³ Returns:    !NULL....Group     found (this is PGRP_ITEM)                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
PGRP_ITEM FindGroupName(PSPY_DATA pSpy,
                        PSZ       pName)
{
  register PGRP_DATA pData;

  /* Sequentially scan the list for this GROUP NAME */

  for(/* Init */ pData = pSpy->GroupHead;                 /* Start @ 1st GROUP */
      /* Term */ (pData != NULL) &&                       /* Until last GROUP seen */
                 (strcmp(pData->item.pDesc,pName) != 0);  /* or GROUP found */
      /* Init */ pData = pData->pNext);                   /* try the next one */


  return(pData == NULL ? NULL : &(pData->item) );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    ProcessFirstMsgInGroup                                       ³*/
/*³                                                                          ³*/
/*³ Function:   setup processing to sequentially return all MSG's defined    ³*/
/*³             in this GROUP                                                ³*/
/*³                                                                          ³*/
/*³ Returns:     NULL......there are NO messages                             ³*/
/*³             !NULL......pointer to first MSG_ITEM                         ³*/
/*³                                                                          ³*/
/*³ Note:       - the MSG's WILL NOT be returned in sequential numeric order!³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
PMSG_ITEM ProcessFirstMsgInGroup(         PSPY_DATA pSpy,
                                 register GROUP     GroupID)
{
  /* Initialize Processing Data */

  pSpy->ProcessAllMsgsInGroup_ID    = GroupID;
  pSpy->ProcessAllMsgsInGroup_First = BOOL_TRUE;

  return( ProcessNextMsgInGroup(pSpy) );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    ProcessNextMsgInGroup                                        ³*/
/*³                                                                          ³*/
/*³ Function:   Continue to sequentially return all MSG's defined in this    ³*/
/*³                                                                     GROUP³*/
/*³ Returns:     NULL......there are NO messages                             ³*/
/*³             !NULL......pointer to next MSG_ITEM                          ³*/
/*³                                                                          ³*/
/*³ Note:       - the MSG's WILL NOT be returned in sequential numeric order!³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
PMSG_ITEM ProcessNextMsgInGroup(PSPY_DATA pSpy)
{
  register PMSG_ITEM pMsg;

  for(/* Init */ pMsg = pSpy->ProcessAllMsgsInGroup_First
                        ? ProcessFirstMsg(pSpy)
                        : ProcessNextMsg(pSpy),

                 pSpy->ProcessAllMsgsInGroup_First = BOOL_FALSE;

      /* Term */ (pMsg          != NULL ) &&
                 (pMsg->GroupID != pSpy->ProcessAllMsgsInGroup_ID);
      /* Init */ pMsg = ProcessNextMsg(pSpy)
     );

  return( pMsg );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    ProcessFirstGroup                                            ³*/
/*³                                                                          ³*/
/*³ Function:   setup processing to sequentially return all GROUP's defined  ³*/
/*³                                                                          ³*/
/*³ Returns:     NULL......there are NO groups                               ³*/
/*³             !NULL......pointer to first GRP_ITEM                         ³*/
/*³                                                                          ³*/
/*³ Note:       - the GROUP's WILL be returned in sequential numeric order!  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
PGRP_ITEM ProcessFirstGroup( register PSPY_DATA pSpy )
{
  /* Initialize Processing Data */

  pSpy->ProcessAllGroups_pGroup  = NULL;
  pSpy->ProcessAllGroups_First   = BOOL_TRUE;

  return( ProcessNextGroup(pSpy) );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    ProcessNextGroup                                             ³*/
/*³                                                                          ³*/
/*³ Function:   Continue to sequentially return all GROUP's defined          ³*/
/*³                                                                          ³*/
/*³ Returns:     NULL......there are NO groups                               ³*/
/*³             !NULL......pointer to next PGRP_ITEM                         ³*/
/*³                                                                          ³*/
/*³ Note:       - the GROUP's WILL be returned in sequential numeric order!  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
PGRP_ITEM ProcessNextGroup(register PSPY_DATA pSpy)
{
  register PGRP_DATA pGrp;

  if ( pSpy->ProcessAllGroups_First )
  {
    pGrp = pSpy->GroupHead;
    pSpy->ProcessAllGroups_First  = BOOL_FALSE;
  }
  else
  {
    pGrp = pSpy->ProcessAllGroups_pGroup == NULL
           ? NULL
           : pSpy->ProcessAllGroups_pGroup->pNext;
  }

  pSpy->ProcessAllGroups_pGroup = pGrp;             /* save current location */

  return( pGrp == NULL ? NULL : &(pGrp->item) );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    DeleteGroup                                                  ³*/
/*³                                                                          ³*/
/*³ Function:   Delete specified GROUP and all MSGs defined in it            ³*/
/*³                                                                          ³*/
/*³ Returns:    FALSE......group not found/deleted                           ³*/
/*³             TRUE.......group deleted                                     ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL DeleteGroup(         PSPY_DATA pSpy,
                 register GROUP     GroupID)
{
  register PGRP_DATA pGrp, pCurGrp, pPrevGrp;

           PMSG_ITEM pMsg;

  if ( (pGrp = iFindGroup(pSpy, GroupID)) != NULL )
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Phase 1: Delete Group from List                                      ³*/
    /*³                                                                      ³*/
    /*³ Cases: a) Group is 'head'..................first GROUP               ³*/
    /*³        b) Group is 'head' and 'tail'.......only  GROUP               ³*/
    /*³        c) Group is 'tail'..................last  GROUP               ³*/
    /*³        d) Group is not 'head' or 'tail'....not first or last         ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

    if (pGrp == pSpy->GroupHead)                /* Head? */
    {
      /* Make second item the first */

      pSpy->GroupHead = pGrp->pNext;

      if (pGrp == pSpy->GroupTail)              /* Also Tail? */
        pSpy->GroupTail = NULL;                 /* yes --> was only in list */
    }
    else
    {
      /* Know not first...scan for this GROUP in linked list */

      for(/* Init */ pCurGrp  = pSpy->GroupHead,
                     pPrevGrp = NULL;

          /* Term */ pCurGrp != pGrp;

          /* Iter */ pPrevGrp = pCurGrp,
                     pCurGrp  = pCurGrp->pNext);

      pPrevGrp->pNext = pGrp->pNext;            /* point PREVIOUS to NEXT */

      if (pGrp == pSpy->GroupTail)              /* also Tail? */
        pSpy->GroupTail = pPrevGrp;
    }

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Phase 2: Delete all MSGs defined for this Group                      ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

    for( /* Init */ pMsg  = ProcessFirstMsgInGroup(pSpy, GroupID);

         /* Term */ pMsg != NULL;                 /* Stop after all MSGs processed */

         /* Iter */ pMsg = ProcessNextMsgInGroup(pSpy) )
    {
      DeleteMsg(pSpy, pMsg->Msg );
    }

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Phase 3: Free Memory allocated for this Group                        ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

    FreeMemory(pGrp->item.pDesc);               /* GROUP description */
    FreeMemory(pGrp);                           /* GROUP data */

  }

  return( pGrp == NULL
          ? BOOL_FALSE
          : BOOL_TRUE );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    DeleteAllGroups                                              ³*/
/*³                                                                          ³*/
/*³ Function:   Delete all defined GROUPs                                    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
VOID DeleteAllGroups(register PSPY_DATA pSpy)
{
  register PGRP_ITEM pGrp;

  for( /* Init */ pGrp  = ProcessFirstGroup(pSpy);    /* start at first */

       /* Term */ pGrp != NULL;                       /* until no GROUPS left */

       /* Iter */ pGrp  = ProcessFirstGroup(pSpy) )   /* restart at new first */
  {
    DeleteGroup(pSpy, pGrp->GroupID );
  }
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    InitializeSpyData                                            ³*/
/*³                                                                          ³*/
/*³ Function:   Initialize a SPY_DATA structure to 'default' values          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
VOID InitializeSpyData( register PSPY_DATA pSpy)
{
  static XLOGCOLOR defLogicalColorTranslation = {
                                                   { 0, Color1 },
                                                   { 1, Color2 },
                                                   { 2, Color3 },
                                                   { 3, Color4 },
                                                   { 4, Color5 },
                                                   { 5, Color6 },
                                                   { 6, Color7 },
                                                   { 7, Color8 },
                                                };

  memset(pSpy, 0, sizeof(*pSpy) );              /* set to all ZERO's */

  InitializeMsg(pSpy);                          /* MSG   data */
  InitializeGroup(pSpy);                        /* GROUP data */

  memcpy(pSpy->LogicalColorTranslation,         /* COLOR data */
         defLogicalColorTranslation,
         sizeof(XLOGCOLOR) );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine:    ResetSpyData                                                 ³*/
/*³                                                                          ³*/
/*³ Function:   Reset a SPY_DATA structure to 'default' values               ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
VOID ResetSpyData( register PSPY_DATA pSpy)
{
   /* Delete all existing Groups and Messages */

   DeleteAllMsgs(pSpy);
   DeleteAllGroups(pSpy);

   /* Reinitialize... */

   InitializeSpyData(pSpy);
}
