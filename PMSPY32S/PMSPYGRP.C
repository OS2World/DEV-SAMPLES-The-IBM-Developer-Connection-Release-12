/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё                                                                          Ё*/
/*Ё PROGRAM NAME: PMSPY                                                      Ё*/
/*Ё -------------                                                            Ё*/
/*Ё  A PM program that is used to look at or 'spy' on the message queue of   Ё*/
/*Ё  other PM applications windows.                                          Ё*/
/*Ё                                                                          Ё*/
/*Ё COPYRIGHT:                                                               Ё*/
/*Ё ----------                                                               Ё*/
/*Ё  Copyright (C) International Business Machines Corp., 1992               Ё*/
/*Ё                                                                          Ё*/
/*Ё DISCLAIMER OF WARRANTIES:                                                Ё*/
/*Ё -------------------------                                                Ё*/
/*Ё  The following [enclosed] code is sample code created by IBM Corporation.Ё*/
/*Ё  This sample code is not part of any standard IBM product and is providedЁ*/
/*Ё  to you solely for the purpose of assisting you in the development of    Ё*/
/*Ё  your applications.  The code is provided "AS IS", without warranty of   Ё*/
/*Ё  any kind.  IBM shall not be liable for any damages arising out of your  Ё*/
/*Ё  use of the sample code, even if they have been advised of the           Ё*/
/*Ё  possibility of such damages.                                            Ё*/
/*Ё                                                                          Ё*/
/*Ё For details on what this program does etc., please see the PMSPY.C file. Ё*/
/*Ё                                                                          Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё PMSPYGRP.C                                                               Ё*/
/*Ё                                                                          Ё*/
/*Ё "Group Message Filter" Dialog                                            Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

#include "pmspy.h"

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё FormatGRP                                                                Ё*/
/*Ё                                                                          Ё*/
/*Ё - common subroutine to format a list item for a specific GRP             Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
static
PSZ
FormatGRP(PGRP_ITEM   pGrp,
          PSZ         pszFormatArea)
{
  sprintf(pszFormatArea,
          pGrp->Include ? Strings[IDS_FMT_GRP_INCLUDED]
                        : Strings[IDS_FMT_GRP_EXCLUDED],
          pGrp->pDesc       /* Group's mnemonic */
         );

  return(pszFormatArea);
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Group Message Filter Dialog Procedure                                    Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
 MRESULT GroupDlgProc(          HWND    hwnd,
                       register ULONG   message,
                                MPARAM  lParam1,
                                MPARAM  lParam2 )
  {
     /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
     /*Ё Local Variables                                                     Ё*/
     /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
     MRESULT            dpResult = NULL;

     CHAR               ListText[200];

              PGRP_ITEM pGrp;

     register USHORT    usItem,
                        pbSelected;

     static PSPY_DATA   pSpyData = NULL;

     HWND               hwndLB;                /* HWND of group listbox */

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Process the message                                                    Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  switch (message)
  {
    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё A dialog pushbutton was selected                                     Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_COMMAND:

      switch( pbSelected = SHORT1FROMMP(lParam1) )
      {
          /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
          /*Ё Handle all attribute pushbuttons                               Ё*/
          /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
          default:
          {
            register MSG_COLOR color = GetColor(hwnd, pSpyData);

            USHORT             nSelected;  /* Number of items selected */

            /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
            /*Ё Obtain HWND of group listbox once for performance        Ё*/
            /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
            hwndLB = WinWindowFromID(hwnd, ID_GRP_LB_GROUPS);

            /************************************************************
            * "Lock" the ListBox to minimize performance impact
            ************************************************************/
            WinLockWindowUpdate(HWND_DESKTOP,     /* Desktop */
                                hwndLB);          /* Lockee  */

            /************************************************************
            * Process the "selected" ListBox items
            ************************************************************/
            for(nSelected = 0,
                usItem    = SHORT1FROMMR(WinSendMsg(hwndLB,
                                                    LM_QUERYSELECTION,
                                                    MPFROMSHORT(LIT_FIRST),
                                                    NULL) );

                usItem != (USHORT)LIT_NONE;

                nSelected++,
                usItem    = SHORT1FROMMR(WinSendMsg(hwndLB,
                                                    LM_QUERYSELECTION,
                                                    MPFROMSHORT(usItem),
                                                    NULL) )
               )
            {
              pGrp = PVOIDFROMMR( WinSendMsg(hwndLB,
                                             LM_QUERYITEMHANDLE,
                                             MPFROMSHORT(usItem),
                                             NULL) );

              /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
              /*Ё Perform selected pushbutton's action                       Ё*/
              /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
              switch( pbSelected )
              {
                /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
                /*Ё Delete this group                                        Ё*/
                /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
                case ID_GRP_PB_DELETE:

                     DeleteGroup(pSpyData, pGrp->GroupID);

                     WinSendMsg(hwndLB,
                                LM_DELETEITEM,
                                MPFROMSHORT(usItem),
                                MPFROMP(NULL) );

                     usItem = (USHORT)( (usItem == 0)
                                          ? LIT_FIRST
                                          : (usItem - 1) );
                break;

                /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
                /*Ё Exclude this group                                       Ё*/
                /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
                case ID_GRP_PB_EXCLUDE:

                     IncludeGroup(pSpyData, pGrp, BOOL_FALSE, color);

                     WinSendMsg(hwndLB,
                                LM_SETITEMTEXT,
                                MPFROMSHORT(usItem),
                                MPFROMP(FormatGRP(pGrp, ListText)) );
                break;

                /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
                /*Ё Include this group                                       Ё*/
                /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
                case ID_GRP_PB_INCLUDE:

                     IncludeGroup(pSpyData, pGrp, BOOL_TRUE, color);

                     WinSendMsg(hwndLB,
                                LM_SETITEMTEXT,
                                MPFROMSHORT(usItem),
                                MPFROMP(FormatGRP(pGrp, ListText)) );
                break;
              }
            }

            /************************************************************
            * Finally, "unlock" the ListBox so it is only redrawn once
            ************************************************************/
            WinLockWindowUpdate(HWND_DESKTOP, NULLHANDLE);

            /************************************************************
            * Make "noise" if no items were selected...
            ************************************************************/
            if (nSelected == 0)                  /* Any selections? */
              WinAlarm(HWND_DESKTOP, WA_ERROR);
          }
          break;

          /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
          /*Ё User's done with the dialog...                                 Ё*/
          /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
          case DID_CANCEL:

               WinDismissDlg( hwnd, 0 );

          break;
      }
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё WM_CLOSE:                                                            Ё*/
    /*Ё - Call WinDismissDlg to close the dialog box                         Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_CLOSE:

         WinDismissDlg( hwnd, 0);

    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё WM_INITDLG:                                                          Ё*/
    /*Ё - Init pointer to data pointed to by WinDlgBox                       Ё*/
    /*Ё - Init global handle of this dlg                                     Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_INITDLG:

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Locate "SPY" data to utilize                                       Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      pSpyData = PVOIDFROMMP(lParam2);

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Obtain HWND of group listbox once for performance        Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      hwndLB = WinWindowFromID(hwnd, ID_GRP_LB_GROUPS);

      /************************************************************
      * "Lock" the ListBox to minimize performance impact
      ************************************************************/
      WinLockWindowUpdate(HWND_DESKTOP,     /* Desktop */
                          hwndLB);          /* Lockee  */

      /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Set listbox FONT to current non-proportional value                  Ё*/
      /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      SetListboxFont(hwndLB, (PSZ) pSpyData->pfnLB);

      /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
      /*Ё Add all Groups to list box                                         Ё*/
      /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
      for(/* Initialize */ pGrp = ProcessFirstGroup(pSpyData); /* @ 1st GROUP */
          /* Terminate  */ pGrp != NULL;                       /* @ end of list */
          /* Iterate    */ pGrp = ProcessNextGroup(pSpyData)   /* try next GROUP */
         )
      {
        usItem = SHORT1FROMMR(WinSendMsg(hwndLB,
                                         LM_INSERTITEM,
                                         MPFROMSHORT(LIT_SORTASCENDING),
                                         MPFROMP(FormatGRP(pGrp, ListText))) );

        WinSendMsg(hwndLB,
                   LM_SETITEMHANDLE,
                   MPFROMSHORT(usItem),
                   MPFROMP(pGrp));
      }

      /************************************************************
      * Finally, "unlock" the ListBox so it is only redrawn once
      ************************************************************/
      WinLockWindowUpdate(HWND_DESKTOP, NULLHANDLE);

      /************************************************************
      * Other dialog set-up
      ************************************************************/
      SetColors( hwnd, pSpyData );      /* load colors       */

      CenterDialog(hwnd);               /* Center the dialog */

    break;                                          /* WM_INITDLG */

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё This is a MSG we don't care about...                                 Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    default:
         if ( !HandleIPF(hwnd, message, lParam1, lParam2, &dpResult) )
           dpResult = WinDefDlgProc(hwnd, message, lParam1, lParam2);
  }

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Exit                                                                   Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  return( dpResult );
}
