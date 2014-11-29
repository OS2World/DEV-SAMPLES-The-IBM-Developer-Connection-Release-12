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
/*³ PMSPYMD.C                                                                ³*/
/*³                                                                          ³*/
/*³ Dialog procedure to display MSG's field decomposition                    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#include "pmspy.h"                      /* Resource symbolic identifiers      */
#include "pmspyMP.h"                    /* MP externals                       */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ LoadMpFormats                                                            ³*/
/*³                                                                          ³*/
/*³ - common subroutine to 'set up' the MP formats drop-down list            ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static
VOID
LoadMpFormats(HWND      hwndDlg,
              USHORT    usDropDownList,
              MP_FORMAT defMP)
{
  USHORT          usThisItem;                  /* where this MP added to list */
  MP_FORMAT       mpThis;                      /* current MP Format           */

  /**************************************************************************
  * Determine HWND of drop-down list once
  **************************************************************************/
  HWND     hwndDD = WinWindowFromID(hwndDlg, usDropDownList);

  /**************************************************************************
  * Add all MP format items to the specified drop-down list
  **************************************************************************/
 for( mpThis = MP_First;       /* Initialize: start @ first         */
      mpThis < MP_Total;       /* While:      still more MP formats */
      mpThis++                 /* Iterate:    try the next          */
    )
 {
   /************************************************************************
   * Add this MP format item remembering "where" it went
   ************************************************************************/
   usThisItem = SHORT1FROMMR(WinSendMsg(hwndDD,
                                        LM_INSERTITEM,
                                        MPFROMSHORT(LIT_END),
                                        MPFROMP(MpDisplayText(mpThis))) );

   /************************************************************************
   * Set item's "handle" to MP format value
   ************************************************************************/
   WinSendMsg(hwndDD,                    /* Window Handle              */
              LM_SETITEMHANDLE,          /* Message                    */
              MPFROMSHORT(usThisItem),   /* Index    of list item      */
              MPFROMSHORT(mpThis) );     /* "Handle" of list item      */

   /************************************************************************
   * If this is the designated "default" MP format items, select it
   ************************************************************************/
   if (mpThis == defMP)
     WinSendMsg(hwndDD,
                LM_SELECTITEM,                  /* Select item    */
                MPFROMSHORT(usThisItem),        /* Item   = This  */
                MPFROMSHORT(TRUE));             /* Select = Yes   */
 }
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ MSG Decompositon Dialog Procedure                                      ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
MRESULT MsgDlgProc(HWND    hwnd,
                   MSG     message,
                   MPARAM  lParam1,
                   MPARAM  lParam2 )
{
  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Local Variables                                                        ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  MRESULT   dpResult  = NULL;

  CHAR      szFmt [256],                          /* formatting specification */
            szText[256];                          /* formatting area          */

  USHORT    usThisItem;                           /* list item                */

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Locate dialog's instance data                                          ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  PMD_DATA pData = WinQueryWindowPtr(hwnd, QWL_USER);

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Process the message                                                    ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  switch (message)
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ "Done" PUSHBUTTON has been used or dialog close requested          ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    case WM_COMMAND:
    case WM_CLOSE:

         WinDismissDlg(hwnd, 0);

    break;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Notification from one of the dialog controls....                   ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    case WM_CONTROL:
          /******************************************************************
          * One of our drop-down list items have new selection?
          ******************************************************************/
          if (
                 (SHORT2FROMMP(lParam1) == CBN_EFCHANGE) &&
               ( (SHORT1FROMMP(lParam1) == DLG_MD_DD_MP1) ||
                 (SHORT1FROMMP(lParam1) == DLG_MD_DD_MP2) )
             )
          {
            MP_FORMAT mpThis;                     /* current MP Format        */

            /****************************************************************
            * Determine "new" drop-down list item
            ****************************************************************/
            usThisItem = SHORT1FROMMR( WinSendMsg(HWNDFROMMP(lParam2),
                                                  LM_QUERYSELECTION,                  /* Select item    */
                                                  MPFROMSHORT(LIT_FIRST),
                                                  NULL) );

            /****************************************************************
            * Query item's "handle" to MP table item
            *****************************************************************/
            mpThis = (MP_FORMAT)
                     SHORT1FROMMR( WinSendMsg(HWNDFROMMP(lParam2),
                                              LM_QUERYITEMHANDLE,
                                              MPFROMSHORT(usThisItem),
                                              NULL) );

            /****************************************************************
            * Format MP1/MP2 using selected value
            *****************************************************************/
            if ( SHORT1FROMMP(lParam1) == DLG_MD_DD_MP1 )
            {
              WinSetWindowText( WinWindowFromID(hwnd, DLG_MD_ST_MP1),
                                MpFormat(pData->qmsgItem.mp1,
                                         pData->pMsg->fMP1 = mpThis,
                                         szText) );
            }
            else
            {
              WinSetWindowText( WinWindowFromID(hwnd, DLG_MD_ST_MP2),
                                MpFormat(pData->qmsgItem.mp2,
                                         pData->pMsg->fMP2 = mpThis,
                                         szText) );
            }
          }
    break;

     /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
     /*³ WM_INITDLG:                                                       ³*/
     /*³                                                                   ³*/
     /*³ - MP2 is dialog instance's data                                   ³*/
     /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
     case WM_INITDLG:
          /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
          /*³ Save Dialog's instance data pointer (passed in MP2)          ³*/
          /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
          WinSetWindowPtr(hwnd, QWL_USER, pData = PVOIDFROMMP(lParam2));

          /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
          /*³ Setup the Dialog for processing...                           ³*/
          /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
          CenterDialog(hwnd);                                /* Center DLG  */

          /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
          /*³ Set up Dialog's title to include MSG name                    ³*/
          /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
          WinQueryWindowText(hwnd, sizeof(szFmt), szFmt);

          sprintf(szText, szFmt, pData->pMsg->pDesc);

          WinSetWindowText(hwnd, szText);

          /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
          /*³ Display MSG's value                                          ³*/
          /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
          sprintf(szText,
                  "0x%4.4X (%d)",
                  pData->pMsg->Msg,
                  pData->pMsg->Msg);

          WinSetWindowText( WinWindowFromID(hwnd, DLG_MD_ST_MSG), szText);

          /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
          /*³ Display MSG "seen" values                                    ³*/
          /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
          WinQueryWindowText( WinWindowFromID(hwnd, DLG_MD_ST_SEEN),
                              sizeof(szFmt), szFmt);

          sprintf( szText,
                   szFmt,
                   pData->pMsg->aulTimes[MSG_TIMES_EVER],
                   pData->pMsg->aulTimes[MSG_TIMES_SINCE]
                 );

          WinSetWindowText( WinWindowFromID(hwnd, DLG_MD_ST_SEEN), szText);

          /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
          /*³ Set MP format values                                         ³*/
          /*³                                                              ³*/
          /*³ Note: WM_CONTROL messages are generated by the following     ³*/
          /*³       which cause formatting of MP1 / MP2 data (see above)   ³*/
          /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
          LoadMpFormats(hwnd, DLG_MD_DD_MP1, pData->pMsg->fMP1);
          LoadMpFormats(hwnd, DLG_MD_DD_MP2, pData->pMsg->fMP2);

      break;

     /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
     /*³ We don't need to handle any other messages...                     ³*/
     /*³                                                                   ³*/
     /*³ If this isn't an IPF message, let PM do it's default "thing"      ³*/
     /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
     default:
          if ( !HandleIPF(hwnd, message, lParam1, lParam2, &dpResult) )
            dpResult = WinDefDlgProc(hwnd, message, lParam1, lParam2 );
   }

   /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
   /*³ Exit                                                                  ³*/
   /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
   return( dpResult );
}
