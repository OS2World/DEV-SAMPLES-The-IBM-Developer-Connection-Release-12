/*---------------------------------------------------------------------------+
| PMJRN   HELP Dialog Procedure
|
| Change History:
| ---------------
+---------------------------------------------------------------------------*/

#include "pmjrn.h"

 /***************************************************************************************
  *
  *                       Entry
  *
  ***************************************************************************************/

 MRESULT EXPENTRY HelpWindowProc(         HWND    hwnd,
                                 register ULONG   message,
                                          MPARAM  lParam1,
                                          MPARAM  lParam2 )

  {

   /***********************************************************************************
    *
    *                   Local Variables
    *
    ***********************************************************************************/

     ULONG       retcode;

     PHELP_DATA  pData;                         /* @C1A - unique dialog data */

 /****************************************************************************
  *
  * get pointer to unique data for this instance of the dialog
  *
  ****************************************************************************/

 pData = (PHELP_DATA) WinQueryWindowULong(hwnd, QWL_USER);   /* @C1A */

 /***************************************************************************************
  *
  *  Process the message
  *
  ***************************************************************************************/

    switch (message)                                    /* Switch off of the message ID */
      {

        case WM_COMMAND:

          switch( LOUSHORT( lParam1 ) )
          {
              /**************************************************************************
               *
               *  User cancelled exclusion/inclusion
               *
               **************************************************************************/

              case DID_CANCEL:                          /* CANCEL was selected          */

                  WinDismissDlg( hwnd,                  /* Close dialog */
                                 0 );                /* WinDlgBox return */

                break;                                  /* Break DID_CANCEL             */
          }

            retcode = 0;                             /* retcode for any WM_COMMAND   */
            break;                                      /* Break WM_COMMAND             */

        /********************************************************************************/
        /**                                                                            **/
        /** WM_CLOSE:                                                                  **/
        /** - Call WinDismissDlg to close the dialog box                               **/
        /**                                                                            **/
        /********************************************************************************/

        case WM_CLOSE:                                  /* Close Dialog Box.            */

          WinDismissDlg( hwnd, 0);
          retcode = TRUE;

          break;                                                                           /* DB  @P4A */

        /*******************************************************************************
         *
         *  WM_INITDLG:
         *  - Init pointer to data pointed to by WinDlgBox
         *  - Init global handle of this dlg
         *
         ********************************************************************************/

        case WM_INITDLG:
             {
               CHAR    szHelpLine[256];
               PCHAR   pWork, pLine;

              /******************************************************************************
               *
               * save instance's unique data for all subsequeunt dialog calls
               *
               ******************************************************************************/

               WinSetWindowULong(hwnd,
                                 QWL_USER,
                                 (ULONG) (pData = (PHELP_DATA)lParam2));

              /******************************************************************************
               *
               *  Load HELP text resource
               *
               ******************************************************************************/

               DosGetResource(0,                   /* get from .EXE */
                              IDT_HELP,               /* Resource Type */
                              pData->HelpID,          /* Resource ID */
                              (PPVOID)&(pData->pHelpText));     /* @C1C */


              /******************************************************************************
               *
               *   Set Window Title As so instructed
               *
               ******************************************************************************/

               WinSetWindowText( WinWindowFromID(hwnd, FID_TITLEBAR),
                                 Strings[pData->TitleID]);

              /******************************************************************************
               *
               *   Add all Help text to list box
               *
               ******************************************************************************/

               for( /* Initialize */  pWork = pData->pHelpText;  /* @C1C start @ first char */
                    /* Terminate  */  *pWork != 0x1A;     /* stop @ End-Of-File */
                    /* Iterate    */  pWork++             /* try the next char */
                  )
               {
                 /* get next line of HELP stuff */

                 for( /* Initialize */  pLine = szHelpLine;
                      /* Terminate  */  *pWork != 0xD;    /* quit @ CR */
                      /* Iterate    */  pWork++, pLine++
                    )
                   *pLine = *pWork;

                   pWork++;         /* skip CR */
                   *pLine = 0;   /* make 0 terminated.... */

                  WinSendDlgItemMsg(                /* Send a message to the list   */
                              hwnd,                 /*   Window Handle              */
                              IDD_HELP,
                              LM_INSERTITEM,        /*   Insert item                */
                              (MPARAM) LIT_END,     /*   @ end                      */
                              (MPARAM) szHelpLine); /*   Text                       */
               }
              /******************************************************************************
               *
               *   Free the HELP resource
               *
               ******************************************************************************/

               /*DosFreeSeg(pData->selHelp);*/
             }
          retcode = FALSE;
          break;                                        /* WM_INITDLG                   */

        default:                                        /* Any other message            */
          retcode =  (ULONG)WinDefDlgProc(              /* Return message to default prc*/
                        hwnd,                  /* Dialog Handle                */
                        message,                        /* Message                      */
                        lParam1,                        /* First parameter for message. */
                        lParam2);                       /* Second parameter for message.*/
      }                                                 /* End switch                   */

 /***************************************************************************************
  *
  *                       Exit
  *
  ***************************************************************************************/

   return((MRESULT) retcode);
}
