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
/*³ PMSPYNOT.C                                                               ³*/
/*³                                                                          ³*/
/*³ Notes dialog procedure                                                   ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

#include "pmspy.h"

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Internal Function: DismissDialog                                         ³*/
/*³                                                                          ³*/
/*³                                                                          ³*/
/*³ Function:                                                                ³*/
/*³                                                                          ³*/
/*³   This module: 1) sets the flag indicating if entered data should        ³*/
/*³                   be used by invoker of dialog                           ³*/
/*³                2) POSTs message to invoker of dialog                     ³*/
/*³                3) Dismisses the dialog                                   ³*/
/*³                                                                          ³*/
/*³ Input:                                                                   ³*/
/*³       DlgHwnd.......dialog handle to dismiss                             ³*/
/*³       DlgData.......pointer to dialog's PNOTES_DATA structure            ³*/
/*³       DlgUse........indicator if entered NOTES data should be used       ³*/
/*³                                                                          ³*/
/*³ Output:                                                                  ³*/
/*³                                                                          ³*/
/*³   - N O N E                                                              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

static void DismissDialog(HWND              hwndDlg,
                          PNOTES_DATA       pDlgData,
                          register BOOL     fDlgUse)

{
  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Set the flag for caller's interogation                                 ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

  pDlgData->fUseData = fDlgUse;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Post REPLY message to caller                                           ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

  WinPostMsg(pDlgData->hwndNotify,
             pDlgData->uNotifyMsg,
             MPFROMP(pDlgData),
             MPFROMP(NULL) );

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Dismiss the dialog                                                     ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

  WinDismissDlg(hwndDlg, fDlgUse);        /* Close the Dialog box         */
}


MRESULT NotesDlgProc(HWND    hwndClientDlg, /* Dialog handle */
                     ULONG   message,       /* Message ID */
                     MPARAM  lParam1,       /* Parameter 1 */
                     MPARAM  lParam2)       /* Parameter 2 */
{
  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Local Variables                                                        ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

  MRESULT   dpResult = NULL;         /* Return code */

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Obtain pointer to this dialog's instance data                          ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

  PNOTES_DATA pData = (PNOTES_DATA)
                       WinQueryWindowPtr(hwndClientDlg, QWL_USER);

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Process this message                                                   ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

  switch (message)                      /* Switch off of the message ID       */
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ One of the pushbuttons was pressed....                               ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

    case WM_COMMAND:

         switch( LOUSHORT( lParam1 ) )
         {
          /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
          /*³ DID_OK:  (User clicked on OK button)                           ³*/
          /*³                                                                ³*/
          /*³ - Get text from edit field                                     ³*/
          /*³ - Validate user's entry                                        ³*/
          /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

          case DID_OK:                              /* OK was selected          */

               /* Disable input into this button */

               WinEnableWindow( WinWindowFromID( hwndClientDlg, DID_OK),
                                BOOL_FALSE);

               /* Get text from input fields   */

               WinQueryDlgItemText(hwndClientDlg,              /* Dialog handle. */
                                   DLG_NOTES_EF_TEXT,          /* Dlg field ID. */
                                   sizeof(pData->szText) - 1,  /* Max # chars */
                                   pData->szText);             /* Return text here */

               if ( pData->szText[0] != 0 )     /* just a NULL string? */
               {
                  DismissDialog( hwndClientDlg, /* which Dialog */
                                 pData,         /* Dialog data */
                                 TRUE);         /* DO use the notes */
               }
               else
               {
                  WinAlarm( WinQueryAnchorBlock(hwndClientDlg), WA_ERROR);

                  WinEnableWindow( WinWindowFromID(hwndClientDlg, DID_OK),
                                   BOOL_TRUE);
               }
          break;

          /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
          /*³ DID_CANCEL:                                                    ³*/
          /*³ - Dismiss dialog                                               ³*/
          /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
          case DID_CANCEL:                          /* CANCEL was selected    */

               DismissDialog( hwndClientDlg, /* which Dialog */
                              pData,         /* Dialog data */
                              FALSE);        /* DO NOT use the name... */
          break;
         }
         break;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ WM_CLOSE:                                                            ³*/
    /*³ - Call WinDismissDlg to close the dialog box                         ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    case WM_CLOSE:                                  /* Close Dialog Box.      */

         DismissDialog( hwndClientDlg, /* which Dialog */
                        pData,         /* Dialog data */
                        FALSE);        /* DO NOT use the name... */
    break;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ WM_INITDLG:                                                          ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

    case WM_INITDLG:                                /* Initialize Dialog Box. */

      /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
      /*³ Save Dialog's instance data pointer (passed in MP2)                ³*/
      /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

      WinSetWindowPtr(hwndClientDlg,
                      QWL_USER,
                      pData = (PNOTES_DATA)lParam2);

      /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
      /*³ Set TEXT entry field width and default values                      ³*/
      /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

      WinSendDlgItemMsg(hwndClientDlg,      /* which Dlg */
                        DLG_NOTES_EF_TEXT,  /* which Field */
                        EM_SETTEXTLIMIT,    /* cmd: set field width */
                        MPFROMSHORT(sizeof(pData->szText)-1), /* mp1: width */
                        NULL);                                /* mp2: ignored */

      WinSetDlgItemText(hwndClientDlg,      /* which Dlg */
                        DLG_NOTES_EF_TEXT,  /* which Field */
                        pData->szText);     /* Default Field text */

      CenterDialog(hwndClientDlg);          /* Center the Dialog */
    break;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ We don't need to handle any other messages...                          ³*/
  /*³                                                                        ³*/
  /*³ If this isn't an IPF message, let PM do it's default "thing"           ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

  default:
       if ( !HandleIPF(hwndClientDlg, message, lParam1, lParam2, &dpResult) )
         dpResult = WinDefDlgProc(hwndClientDlg, message, lParam1, lParam2 );

  }                                                 /* End switch                   */

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Exit                                                                   ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

  return( dpResult );
}
