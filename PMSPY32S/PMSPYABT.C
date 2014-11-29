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
/*³ PMSPYABT.C                                                               ³*/
/*³                                                                          ³*/
/*³ Window Procedure to display the PMSPY Logo                               ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Includes                                                                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

#include "pmspy.h"                      /* Resource symbolic identifiers  */

MRESULT SpyLogoDlgProc(register HWND   hwnd,
                       register ULONG  msg,
                                MPARAM mp1,
                                MPARAM mp2)

{
  MRESULT dpResult = NULL;

  switch(msg)
  {
    case WM_CLOSE:

         WinDismissDlg(hwnd,0);

         break;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ WM_INITDLG:                                                          ³*/
    /*³                                                                      ³*/
    /*³ - Position window in center of Desk Top                              ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

    case WM_INITDLG:                                /* Display Caption & edits      */
         {
                    HWND   abouthwnd;               /* child window handle(for about dialog) */

                    ULONG  style;                   /* window style in reserved window word's memory */

                    HPOINTER hicon;                 /* handle of the program icon */

           /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
           /*³ We will now add the PMSPY icon to the about dialog box area by³*/
           /*³ issuing the following PM commands:                            ³*/
           /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

           abouthwnd=WinWindowFromID(               /* Get the window icon    */
                                hwnd,               /*   Parent window Handle */
                                IDD_ABOUTICON);     /*   Child ID             */

           style=WinQueryWindowULong(               /* Get the window style   */
                                     abouthwnd,     /*   About Window Handle  */
                                     QWL_STYLE);    /*   ULong to get is window style  */

           WinSetWindowULong(                       /* Set the window style   */
                             abouthwnd,             /*   About Window Handle  */
                             QWL_STYLE,             /*   ULong to set         */
                             (style & (ULONG)(~SS_TEXT)) | SS_ICON);

           hicon=WinLoadPointer(                     /* Get the window icon   */
                                HWND_DESKTOP,        /*   DeskTop Window hand */
                                hmodNLS,             /*   Resource Module hand*/
                                ID_MAINWND);         /*   Res ID or ptr to be loaded */

           WinSendDlgItemMsg(                /* Send msg to dialog item, ABOUTICON  */
                             hwnd,           /*   Parent Window Handle(dialog)*/
                             IDD_ABOUTICON,  /*   dialog item ID              */
                             SM_SETHANDLE,   /*   message ID                  */
                             (MPARAM)hicon,  /*   parm1 is ptr from res file  */
                             NULL);          /*   msg parm2 is NULL           */

           CenterDialog(hwnd);               /* Center the Dialog             */

         }
         break;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ WM_COMMAND:                                                          ³*/
    /*³                                                                      ³*/
    /*³ - switch on user command value                                       ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

    case WM_COMMAND:

         switch ( SHORT1FROMMP(mp1) )
         {
           /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
           /*³ OK pressed:                                                   ³*/
           /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

           case DID_OK:

                WinDismissDlg(hwnd, TRUE);

                break;

           default:
                dpResult = WinDefDlgProc( hwnd, msg, mp1, mp2 );
         }

         break;

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ We don't need to handle any other messages...                        ³*/
    /*³                                                                      ³*/
    /*³ If this isn't an IPF message, let PM do it's default "thing"         ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

    default:
         if ( !HandleIPF(hwnd, msg, mp1, mp2, &dpResult) )
           dpResult = WinDefDlgProc( hwnd, msg, mp1, mp2 );

  }

  return( dpResult );
}
