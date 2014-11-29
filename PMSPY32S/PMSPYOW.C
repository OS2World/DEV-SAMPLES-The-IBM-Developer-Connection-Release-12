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
/*³ PMSPYOW.C                                                                ³*/
/*³                                                                          ³*/
/*³ Dialog procedure to display and select an OBJECT window for SPYing       ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

#include "pmspy.h"                      /* Resource symbolic identifiers*/

#define  SYSTEM_OBJECT_WINDOW_ID '#'    /* first char of SYSTEM object class name */

typedef struct
{
  HWND       hwnd;
  char       szClass[40];
  PID        pidCreator;
  TID        tidCreator;
  CLASSINFO  classInfo;
  HMQ        hmq;
  BOOL       fIsOBJECT;

}    OW_INFO,
  * POW_INFO;

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine to add one window to the LISTBOX                                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static
BOOL                                     /* TRUE = included, FALSE = excluded */
AddOneWindowToList( HWND     hwndLB,     /* window of LISTBOX                 */
                    HWND     hwndWindow, /* window to add                     */
                    POW_DATA pData)      /* Dialog's data                     */
{
  BOOL     fIncluded = BOOL_TRUE;

  USHORT   list_index;

  OW_INFO  objData;

  POW_INFO pOW;

  char     szData  [128],
           szHWND  [20],
           szHMQ   [20],
           szParent[40];

  PSZ      pszClass;

  memset(&objData, 0, sizeof(objData) );

  objData.hwnd      = hwndWindow;
  objData.fIsOBJECT = pData->fIsOBJECT;

  WinQueryClassName(hwndWindow, sizeof(objData.szClass), objData.szClass);

  WinQueryClassInfo(WinQueryAnchorBlock(hwndLB),
                    objData.szClass,
                    &objData.classInfo);

  WinQueryWindowProcess(hwndWindow, &objData.pidCreator, &objData.tidCreator);

  objData.hmq = (HMQ)WinQueryWindowULong(hwndWindow, QWL_HMQ);

  if ( objData.hmq == pData->hmqSPY )         /* this SPY agent's window?     */
  {
    fIncluded = BOOL_FALSE;                   /* Yes: exclude...              */
  }
  else
  {
    pszClass = TranslateClassName( objData.szClass );  /* translate SYSTEM */

    sprintf(szHWND  , "%8.8lX",   hwndWindow);
    sprintf(szHMQ   , "%8.8lX",   objData.hmq);

    sprintf(szParent, "%s.%u.%u", pData->pszType,
                                  objData.pidCreator,
                                  objData.tidCreator );

    if ( pszClass != objData.szClass )  /* Class name translated? */
      sprintf( szData,                  /* yes... */
               "Hmq: %s Hwnd: %s Linage: %-12.12s Class: %s (%s)",
               szHMQ,
               szHWND,
               szParent,
               pszClass,
               objData.szClass
             );
    else                                /* no... */
      sprintf( szData,
               "Hmq: %s Hwnd: %s Linage: %-12.12s Class: %s",
               szHMQ,
               szHWND,
               szParent,
               objData.szClass
             );

    pOW = (POW_INFO) AllocateMemory( sizeof( OW_INFO) );

    *pOW = objData;

    list_index = SHORT1FROMMR(WinSendMsg(hwndLB,
                                         LM_INSERTITEM,    /* Insert LB item    */
                                         MPFROMSHORT(LIT_SORTASCENDING),/* sort */
                                         MPFROMP(szData)) );        /*   Text   */

    WinSendMsg(hwndLB,                    /*   Window Handle              */
               LM_SETITEMHANDLE,          /*   Message                    */
               MPFROMSHORT(list_index),   /*   Index of list item         */
               MPFROMP(pOW) );            /*   Handle of list item        */
  }

  return( fIncluded );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine to recursively add all child windows to the LISTBOX              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static
void
AddWindowChildrenToList(HWND     hwndLB,       /* window of LISTBOX           */
                        HWND     hwndWindow,   /* parent window               */
                        POW_DATA pData)        /* Dialog's data               */
{
  HWND     hwndChild;

  HENUM    hEnum = WinBeginEnumWindows( hwndWindow );

  /* process all child windows of this window */

  for( /* Initialize */ hwndChild  = WinGetNextWindow(hEnum);
       /* While      */ hwndChild != NULLH;
       /* Iterate    */ hwndChild  = WinGetNextWindow(hEnum) )
  {
     AddOneWindowToList( hwndLB,               /* window of LISTBOX */
                         hwndChild,            /* window to add     */
                         pData);               /* Dialog's data     */

     AddWindowChildrenToList( hwndLB,          /* window of LISTBOX    */
                              hwndChild,       /* add child's children */
                              pData);          /* Dialog's data        */

     WinLockWindowUpdate(hwndChild, BOOL_FALSE); /* unLOCK the window */
  }

  WinEndEnumWindows( hEnum );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine to delete all existing OBJECT windows in the listbox             ³*/
/*³                                                                          ³*/
/*³ - for performance and a visually 'nice' appear, we'll delete             ³*/
/*³   list items at once (after dealing with each's HANDLE data)             ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static VOID DeleteList(HWND hwndLB)   /* handle of LISTBOX      */

{
           PSZ    pData;              /* data for current list item */

  register USHORT uItem;              /* deletion loop counter */

  register USHORT uTotalItems = SHORT1FROMMR(WinSendMsg(hwndLB,
                                                        LM_QUERYITEMCOUNT,
                                                        NULL,
                                                        NULL) );

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Delete HANDLE data for every item                                      ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  for( /* Initialization */  uItem = 0;                /* start @ first Item */
       /* While          */  uItem < uTotalItems;      /* while not all processed */
       /* Iterate        */  uItem++)                  /* to the next Item */
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Obtain item's "handlized" data and free allocated memory             ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    pData = (PSZ) WinSendMsg( hwndLB,
                              LM_QUERYITEMHANDLE, /* give me HANDLE */
                              MPFROMSHORT(uItem), /* current Item # */
                              NULL);              /* Reserved */
    if (pData != NULL)
      FreeMemory( pData );
  }

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Delete ALL item(s) in list at once (quick and more visually pleasing   ³*/
  /*³ than do one at a time in the above loop)                               ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  WinSendMsg( hwndLB,
              LM_DELETEALL,     /* Delete ALL Item */
              NULL,             /* Reserved */
              NULL);            /* Reserved */
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routine to generate the list of windows                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static USHORT GenerateList( HWND     hwndDlg,      /* handle of DIALOG        */
                            USHORT   idList,       /* ID of listbox in DIALOG */
                            USHORT   idCount,      /* ID of "count" in DIALOG */
                            POW_DATA pData)        /* Dialog's data           */
{
  register USHORT uNumberOfWindows;
  CHAR            szData[128];

  HWND     hwndLB = WinWindowFromID(hwndDlg,idList);  /* calculate HWND once! */

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ First of all, "lock" the ListBox to minimize bad visual                ³*/
  /*³ effect from the deletion and insertion of the VIEWs...                 ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  WinLockWindowUpdate(HWND_DESKTOP,          /* Desktop */
                      hwndLB);               /* Lockee */

  /* start by erasing any existing windows in the listbox */

  DeleteList( hwndLB );      /* LISTBOX window */

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Add DESKTOP windows if specified                                       ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  if (pData->fDESKTOP)
  {
    pData->pszType   = "DSK";
    pData->fIsOBJECT = BOOL_FALSE;

    if ( AddOneWindowToList( hwndLB,          /* window of LISTBOX */
                             HWND_DESKTOP,    /* window of OBJECT  */
                             pData) )         /* dialog's data     */
    {
      AddWindowChildrenToList( hwndLB,        /* window of LISTBOX */
                               HWND_DESKTOP,  /* parent window     */
                               pData);        /* dialog's data     */
    }
  }

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Add OBJECT  windows if specified                                       ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  if (pData->fOBJECT)
  {
    pData->pszType   = "OBJ";
    pData->fIsOBJECT = BOOL_TRUE;

    if ( AddOneWindowToList( hwndLB,          /* window of LISTBOX */
                             HWND_OBJECT,     /* window of OBJECT  */
                             pData) )         /* dialog's data     */
    {
      AddWindowChildrenToList( hwndLB,        /* window of LISTBOX */
                               HWND_OBJECT,   /* parent window     */
                               pData);        /* dialog's data     */
    }
  }

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Select first item in the ListBox                                       ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  WinSendMsg(hwndLB,                 /* Select 1st item      */
             LM_SELECTITEM,          /* msg: select item     */
             MPFROMSHORT( 0 ),       /* mp1: FIRST item      */
             MPFROMSHORT( TRUE ));   /* mp2: SELECT item     */

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Finally, "unlock" the ListBox so it is only redrawn once               ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  WinLockWindowUpdate(HWND_DESKTOP,  /* Desktop */
                      NULLH);        /* Re-enable ListBox */

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Determine current number of windows in LISTBOX                         ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  uNumberOfWindows = SHORT1FROMMR( WinSendMsg(hwndLB,
                                              LM_QUERYITEMCOUNT,
                                              NULL, NULL) );

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Display the current window "count" in the dialog                       ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  {
    static   CHAR   szCountFmt[128] = "";            /* dialog "count" format   */

    /*************************************************************
    * if necesarry, load Format string from dialog template
    *************************************************************/
    if (szCountFmt[0] == 0)
    {
      WinQueryDlgItemText(hwndDlg, idCount, sizeof(szCountFmt), szCountFmt);
    }

    /*************************************************************
    * Format to include number of windows
    *************************************************************/
    sprintf(szData,                      /* target formating area  */
            szCountFmt,                  /* DLG format string      */
            uNumberOfWindows);           /* Insert: # windows      */

    /*************************************************************
    * Update dialog's visible text
    *************************************************************/
    WinSetDlgItemText(hwndDlg, idCount, szData);
  }

  return( uNumberOfWindows );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Internal Function: DismissDialog                                         ³*/
/*³                                                                          ³*/
/*³                                                                          ³*/
/*³ Function:                                                                ³*/
/*³                                                                          ³*/
/*³   This module: 1) sets the flag indicating if entered data should        ³*/
/*³                   be used by invoker of dialog                           ³*/
/*³                2) POSTs message to invoker of dialog                     ³*/
/*³                3) Delete all LISTBOX items                               ³*/
/*³                4) Dismisses the dialog                                   ³*/
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
                          POW_DATA          pDlgData,
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
  /*³ Delete all list items                                                  ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  DeleteList(  WinWindowFromID( hwndDlg, ID_OW_LB_WINDOWS) );

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Dismiss the dialog                                                     ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  WinDismissDlg( hwndDlg, fDlgUse);        /* Close the Dialog box         */
}

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Dialog Procedure for displaying list of OBJECT windows                 ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

 MRESULT ObjectDlgProc(          HWND    hwnd,
                        register ULONG   message,
                                 MPARAM  lParam1,
                                 MPARAM  lParam2 )

{

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Local Variables                                                        ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

  MRESULT   dpResult  = NULL;
  POW_INFO  pOW;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Obtain pointer to this dialog's instance data                          ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

  POW_DATA pData = (POW_DATA) WinQueryWindowPtr(hwnd, QWL_USER);

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Process the message                                                  ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

    switch (message)                       /* Switch off of the message ID */
      {
        /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
        /*³ One of the PUSHBUTTON has been used...                           ³*/
        /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
        case WM_COMMAND:

          switch( LOUSHORT( lParam1 ) )
          {
              /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
              /*³ process the selected window                                ³*/
              /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
              case ID_OW_PB_SPY_HWND:
              case ID_OW_PB_SPY_HMQ:
              {
                USHORT        curItem;                 /* current list item */

                if ( (curItem = SHORT1FROMMR(
                                WinSendDlgItemMsg(      /* Query 1st selection          */
                                  hwnd,                 /*   Dialog handle              */
                                  ID_OW_LB_WINDOWS,     /*   Listbox ID                 */
                                  LM_QUERYSELECTION,    /*   Query ID of selected       */
                                  MPFROMSHORT(LIT_FIRST),
                                  NULL)) ) != (USHORT)LIT_NONE )
                {
                  pOW = PVOIDFROMMR( WinSendDlgItemMsg(hwnd,
                                                       ID_OW_LB_WINDOWS,
                                                       LM_QUERYITEMHANDLE,
                                                       MPFROMSHORT(curItem),
                                                       NULL) );

                  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
                  /*³ Switch to SPYing this window...                        ³*/
                  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
                  pData->hwndOW        = pOW->hwnd;
                  pData->hmqOW         = pOW->hmq;

                  pData->fHwndIsOBJECT = pOW->fIsOBJECT;

                  pData->fSpyHWND = SHORT1FROMMP(lParam1) == ID_OW_PB_SPY_HWND
                                    ? BOOL_TRUE
                                    : BOOL_FALSE;

                  DismissDialog(hwnd, pData, TRUE);  /* end dialog (do USE data) */
                }
                else
                  WinAlarm(HWND_DESKTOP, WA_ERROR);
              }
              break;

              /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
              /*³ refresh the list with all windows                          ³*/
              /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
              case ID_OW_PB_REFRESH:

                   pData->fDESKTOP =                   /* query current state */
                             SHORT1FROMMR(WinSendDlgItemMsg(hwnd,
                                                            ID_OW_CB_DESKTOP,
                                                            BM_QUERYCHECK,
                                                            NULL,
                                                            NULL));

                   pData->fOBJECT  =                   /* query current state */
                             SHORT1FROMMR(WinSendDlgItemMsg(hwnd,
                                                            ID_OW_CB_OBJECT,
                                                            BM_QUERYCHECK,
                                                            NULL,
                                                            NULL));

                   GenerateList(hwnd,
                                ID_OW_LB_WINDOWS,
                                ID_OW_ST_WINDOWS,
                                pData);            /* Dialog's data           */
              break;

              /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
              /*³ User cancelled exclusion/inclusion                         ³*/
              /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
              case DID_CANCEL:

                   DismissDialog(hwnd, pData, FALSE);  /* DO NOT use USE data */

              break;
            }

          break;                                       /* Break WM_COMMAND    */

        /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
        /*³ Notification from one of the dialog controls....                 ³*/
        /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
        case WM_CONTROL:

          /******************************************************************
          * One of our checkboxes?
          ******************************************************************/
          if ( ( SHORT1FROMMP(lParam1) == ID_OW_CB_DESKTOP ) ||
               ( SHORT1FROMMP(lParam1) == ID_OW_CB_OBJECT  ) )
          {
            /****************************************************************
            * Decide if the "Refresh" pull-down should be:
            *
            * - enabled  if one or both checked
            * - disabled if neither     checked
            ****************************************************************/
            WinEnableWindow(WinWindowFromID(hwnd, ID_OW_PB_REFRESH),
                            SHORT1FROMMR(WinSendDlgItemMsg(hwnd,
                                                           ID_OW_CB_DESKTOP,
                                                           BM_QUERYCHECK,
                                                           NULL,
                                                           NULL)) ||

                             SHORT1FROMMR(WinSendDlgItemMsg(hwnd,
                                                            ID_OW_CB_OBJECT,
                                                            BM_QUERYCHECK,
                                                            NULL,
                                                            NULL))
                             ? BOOL_TRUE
                             : BOOL_FALSE);
          }
          break;                                        /* Break WM_CONTROL   */

        /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
        /*³ WM_CLOSE:                                                        ³*/
        /*³ - Call WinDismissDlg to close the dialog box                     ³*/
        /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
        case WM_CLOSE:

             DismissDialog(hwnd, pData, FALSE); /* DO NOT use USE data... */

             dpResult = MRFROMSHORT( TRUE );

        break;

         /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
         /*³ WM_INITDLG:                                                     ³*/
         /*³                                                                 ³*/
         /*³ - Load list with current set of OBJECT windows                  ³*/
         /*³                                                                 ³*/
         /*³ - MP2 is dialog instance's data                                 ³*/
         /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
         case WM_INITDLG:
              /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
              /*³ Save Dialog's instance data pointer (passed in MP2)        ³*/
              /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
              WinSetWindowPtr(hwnd, QWL_USER, pData = (POW_DATA)lParam2);

              pData->fUseData = BOOL_FALSE;       /* start a pessemist...     */

              /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
              /*³ Setup the Dialog for processing...                         ³*/
              /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
              CenterDialog(hwnd);                              /* Center DLG  */

              /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
              /*³ Set default "Refresh" control checkboxes                   ³*/
              /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
              WinSendDlgItemMsg(hwnd,
                                ID_OW_CB_DESKTOP,
                                BM_SETCHECK,
                                MPFROMSHORT(pData->fDESKTOP),
                                NULL);

              WinSendDlgItemMsg(hwnd,
                                ID_OW_CB_OBJECT,
                                BM_SETCHECK,
                                MPFROMSHORT(pData->fOBJECT),
                                NULL);

              /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
              /*³ Load listbox with current set of windows                   ³*/
              /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
              SetListboxFont(WinWindowFromID(hwnd, ID_OW_LB_WINDOWS),/* set LB font */
                             (PSZ) pData->pfnCurrent);

              GenerateList(hwnd,
                           ID_OW_LB_WINDOWS,
                           ID_OW_ST_WINDOWS,
                           pData);            /* Dialog's data           */
          break;

         /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
         /*³ We don't need to handle any other messages...                   ³*/
         /*³                                                                 ³*/
         /*³ If this isn't an IPF message, let PM do it's default "thing"    ³*/
         /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
         default:
              if ( !HandleIPF(hwnd, message, lParam1, lParam2, &dpResult) )
                dpResult = WinDefDlgProc(hwnd, message, lParam1, lParam2 );

      }                                                 /* End switch                   */

   /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
   /*³ Exit                                                                  ³*/
   /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
   return( dpResult );
}
