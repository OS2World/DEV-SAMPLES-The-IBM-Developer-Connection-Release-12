/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*�                                                                          �*/
/*� PROGRAM NAME: PMSPY                                                      �*/
/*� -------------                                                            �*/
/*�  A PM program that is used to look at or 'spy' on the message queue of   �*/
/*�  other PM applications windows.                                          �*/
/*�                                                                          �*/
/*� COPYRIGHT:                                                               �*/
/*� ----------                                                               �*/
/*�  Copyright (C) International Business Machines Corp., 1992               �*/
/*�                                                                          �*/
/*� DISCLAIMER OF WARRANTIES:                                                �*/
/*� -------------------------                                                �*/
/*�  The following [enclosed] code is sample code created by IBM Corporation.�*/
/*�  This sample code is not part of any standard IBM product and is provided�*/
/*�  to you solely for the purpose of assisting you in the development of    �*/
/*�  your applications.  The code is provided "AS IS", without warranty of   �*/
/*�  any kind.  IBM shall not be liable for any damages arising out of your  �*/
/*�  use of the sample code, even if they have been advised of the           �*/
/*�  possibility of such damages.                                            �*/
/*�                                                                          �*/
/*� For details on what this program does etc., please see the PMSPY.C file. �*/
/*�                                                                          �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� PMSPYFON.C                                                               �*/
/*�                                                                          �*/
/*� Font Selection Dialog Procedure                                          �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

#include "pmspy.h"
/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� Internal Function: DismissDialog                                         �*/
/*�                                                                          �*/
/*�                                                                          �*/
/*� Function:                                                                �*/
/*�                                                                          �*/
/*�   This module: 1) sets the flag indicating if entered data should        �*/
/*�                   be used by invoker of dialog                           �*/
/*�                2) POSTs message to invoker of dialog                     �*/
/*�                3) Dismisses the dialog                                   �*/
/*�                                                                          �*/
/*� Input:                                                                   �*/
/*�       DlgHwnd.......dialog handle to dismiss                             �*/
/*�       DlgData.......pointer to dialog's PFONT_DATA structure             �*/
/*�       DlgUse........indicator if entered FONT data should be used        �*/
/*�                                                                          �*/
/*� Output:                                                                  �*/
/*�                                                                          �*/
/*�   - N O N E                                                              �*/
/*�                                                                          �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

static
void DismissDialog(HWND              hwndDlg,
                   PFONT_DATA        pDlgData,
                   register BOOL     fDlgUse)

{
  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Set the flag for caller's interrogation                                �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

  pDlgData->fUseData = fDlgUse;

  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Post REPLY message to caller                                           �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

  WinPostMsg(pDlgData->hwndNotify,
             pDlgData->uNotifyMsg,
             MPFROMP(pDlgData),
             MPFROMP(NULL) );

  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Dismiss the dialog                                                     �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

  WinDismissDlg(hwndDlg, fDlgUse);        /* Close the Dialog box         */
}


 MRESULT  FontDlgProc(          HWND    hWnd,
                       register ULONG   usMsg,
                                MPARAM  mp1,
                                MPARAM  mp2 )

{
  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Local Variables                                                        �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

  MRESULT    dpResult = NULL;          /* result of current MSG processing */
  USHORT     usItem;

  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Obtain pointer to this dialog's instance data                          �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

  PFONT_DATA pData = (PFONT_DATA) WinQueryWindowPtr(hWnd, QWL_USER);

  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Process the message                                                    �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

  switch ( usMsg )
    {
      /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
      /*� One of the pushbuttons has been used....                           �*/
      /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

      case WM_COMMAND:

        switch( LOUSHORT( mp1 ) )
        {
           /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�*/
           /*� Process selected FONT                                         �*/
           /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�*/

           case DID_OK:

                /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
                /*� Determine selected FONT                                  �*/
                /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

                usItem = (USHORT)
                         WinSendDlgItemMsg(hWnd,
                                           DLG_FONT_LB,
                                           LM_QUERYSELECTION,
                                           MPFROMSHORT(LIT_FIRST),
                                           MPFROMP(NULL));

                pData->pfnSelected = (PFONTNAME)
                                     WinSendDlgItemMsg(hWnd,
                                                       DLG_FONT_LB,
                                                       LM_QUERYITEMHANDLE,
                                                       MPFROMSHORT(usItem),
                                                       MPFROMP(NULL));
                DismissDialog(hWnd, pData, TRUE);

           break;

           /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�*/
           /*� User cancelled exclusion/inclusion                            �*/
           /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�*/

           case DID_CANCEL:

                DismissDialog(hWnd, pData, FALSE);

           break;
        }

        break;                                        /* Break WM_COMMAND           */

      /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
      /*� WM_CLOSE:                                                          �*/
      /*� - Dismiss the dialog, but DO NOT use results...                    �*/
      /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

      case WM_CLOSE:
           DismissDialog(hWnd, pData, FALSE);
      break;

      /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
      /*� WM_INITDLG:                                                        �*/
      /*�                                                                    �*/
      /*� - save dialog instance data pointer                                �*/
      /*�                                                                    �*/
      /*� - MP2 = PFONT_DATA                                                 �*/
      /*�                                                                    �*/
      /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

      case WM_INITDLG:
           {
             PFONTNAME  pFont;

             /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�*/
             /*� Save Dialog's instance data pointer (passed in MP2)         �*/
             /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�*/

             WinSetWindowPtr(hWnd, QWL_USER, pData = (PFONT_DATA)mp2);

             /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�*/
             /*� Add all FONTS to list box                                   �*/
             /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�*/

             for( /* Initialize */  pFont = pData->pfnList; /* @ first FONT */
                  /* Terminate  */  *pFont[0] != 0;         /* stop at EOT */
                  /* Iterate    */  pFont++                 /* try next FONT */
                )
             {
               usItem = (USHORT)
                         WinSendDlgItemMsg(hWnd,
                                           DLG_FONT_LB,
                                           LM_INSERTITEM,
                                           MPFROMSHORT(LIT_SORTASCENDING),
                                           MPFROMP(pFont) ); /* FONT Name */
               WinSendDlgItemMsg(hWnd,
                                 DLG_FONT_LB,
                                 LM_SETITEMHANDLE,
                                 MPFROMSHORT(usItem),
                                 MPFROMP(pFont) );
             }

            /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
            /*� See if "current" FONT exists...automatically select and      �*/
            /*� make it the "top" item                                       �*/
            /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

            if ( (pData->pfnCurrent != NULL ) &&
                 ( (usItem = SHORT1FROMMR(
                             WinSendDlgItemMsg(hWnd,
                                               DLG_FONT_LB,
                                               LM_SEARCHSTRING,
                                               MPFROM2SHORT(LSS_CASESENSITIVE |
                                                            LSS_PREFIX,
                                                            LIT_FIRST),
                                               MPFROMP(pData->pfnCurrent))) )
                   != (USHORT)LIT_NONE )
               )
            {
              WinSendDlgItemMsg(hWnd,                    /* select "current" */
                                DLG_FONT_LB,
                                LM_SELECTITEM,
                                MPFROMSHORT(usItem),
                                MPFROMSHORT(TRUE));

              WinSendDlgItemMsg(hWnd,                    /* make "top" item */
                                DLG_FONT_LB,
                                LM_SETTOPINDEX,
                                MPFROMSHORT(usItem),
                                MPFROMP(NULL));
            }

             CenterDialog(hWnd);                        /* Center the Dialog */

           }

        break;

      /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
      /*� We don't need to handle any other messages...                      �*/
      /*�                                                                    �*/
      /*� If this isn't an IPF message, let PM do it's default "thing"       �*/
      /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

      default:
           if ( !HandleIPF(hWnd, usMsg, mp1, mp2, &dpResult) )
             dpResult = WinDefDlgProc(hWnd,       /* Dialog Handle                */
                                      usMsg,      /* Message                      */
                                      mp1,        /* First parameter for message. */
                                      mp2);       /* Second parameter for message.*/

   } /* End MSG switch */

  /*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
  /*� Exit                                                                   �*/
  /*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

  return( dpResult );
}
