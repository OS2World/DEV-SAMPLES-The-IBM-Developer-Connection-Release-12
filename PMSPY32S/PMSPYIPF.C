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
/*Ё PMSPYIPF.C                                                               Ё*/
/*Ё                                                                          Ё*/
/*Ё IPF interface code                                                       Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

#define INCL_WINMESSAGEMGR

#include "pmspy.h"
#include "pmspyipf.h"


/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё Shared, local data used by IPF routines                                  Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

static HWND hFrame = NULLH,
            hIPF   = NULLH;


/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё InitializeIPF                                                            Ё*/
/*Ё                                                                          Ё*/
/*Ё Returns: NULL........unable to create IPF instance                       Ё*/
/*Ё          Otherwise...IPF help handle                                     Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

HWND InitializeIPF( HWND   hwndFrame,
                    USHORT uActionBarID,
                    PSZ    pszHelpLibs,
                    PSZ    pszRefTitle)
{
  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Define Automatic variables                                             Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

  HELPINIT HM_Struct;                    /* HM Initialization Structure */

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Reset to "No IPF now..."                                               Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

  hIPF = hFrame = NULLH;

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё set up HelpManager intialization structure                             Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

  HM_Struct.cb                        = sizeof(HM_Struct);
  HM_Struct.ulReturnCode              = 0L;
  HM_Struct.pszTutorialName           = NULL;
  HM_Struct.phtHelpTable              = MPFROM2SHORT(uActionBarID,0xFFFF);
  HM_Struct.hmodAccelActionBarModule  = 0;
  HM_Struct.idAccelTable              = 0;
  HM_Struct.idActionBar               = 0;
  HM_Struct.pszHelpWindowTitle        = pszRefTitle;
  HM_Struct.fShowPanelId              = CMIC_HIDE_PANEL_ID;  /* CMIC_SHOW_PANEL_ID */
  HM_Struct.pszHelpLibraryName        = pszHelpLibs;
  HM_Struct.hmodHelpTableModule       = hmodNonNLS;

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Create an instance of the HelpManager                                  Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

  if ( (hIPF = WinCreateHelpInstance(WinQueryAnchorBlock(hwndFrame),
                                     &HM_Struct)) == NULLH)
  {
    MsgDisplay( hwndFrame,
                swcntrl.szSwtitle,
                Strings[IDS_MSG_IPF_INIT_1],
                IDS_MSG_IPF_INIT_1,
                MB_CANCEL | MB_CUACRITICAL,

                hwndFrame,
                uActionBarID,
                HM_Struct.ulReturnCode,
                pszHelpLibs
              );
  }
  else
  {
    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Associate IPF instance with specified FRAME window                   Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

    if ( WinAssociateHelpInstance(hIPF, hwndFrame) == BOOL_FALSE )
    {
      MsgDisplay( hwndFrame,
                  swcntrl.szSwtitle,
                  Strings[IDS_MSG_IPF_INIT_2],
                  IDS_MSG_IPF_INIT_2,
                  MB_CANCEL | MB_CUACRITICAL,

                  hIPF,
                  WinGetLastError(WinQueryAnchorBlock(hwndFrame))
                );

      WinDestroyHelpInstance( hIPF );

      hIPF = NULLH;
    }
    else
    {
      hFrame = hwndFrame;
    }
  }

  return ( hIPF );
}

/*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
/*Ё HandleIPF - handle any required IPF messages/actions                     Ё*/
/*Ё                                                                          Ё*/
/*Ё Returns: TRUE........the message HAS     been processed; application     Ё*/
/*Ё                      should just return "set" MRESULT value              Ё*/
/*Ё                                                                          Ё*/
/*Ё          FALSE.......the message HAS NOT been processed; application     Ё*/
/*Ё                      should do something with it ("default" for window orЁ*/
/*Ё                      dialog)                                             Ё*/
/*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
BOOL HandleIPF(HWND     hWnd,         /* current window */
               MSG      Msg,          /* current message */
               MPARAM   mp1,          /* current message parm #1 */
               MPARAM   mp2,          /* current message parm #2 */
               PMRESULT pmrValue)     /* where to "set" result */

{
  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Define automatic variables                                             Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  USHORT usPanelID  = 0;         /* panel ID to display (default: NONE) */

  BOOL   bHandled   = BOOL_TRUE; /* final "result" (default: we DID handle) */

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Initially, set to "no special result"                                  Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  *pmrValue = NULL;

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Processing is based on the "msg"...                                    Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  switch ( Msg )
  {
    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Some sort of IPF error...                                            Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case HM_ERROR:
    case HM_EXT_HELP_UNDEFINED:

         MsgDisplay( hFrame,
                     swcntrl.szSwtitle,
                     Strings[IDS_MSG_IPF_ERROR_1],
                     IDS_MSG_IPF_ERROR_1,
                     MB_CANCEL | MB_CUACRITICAL,

                     Msg,
                     mp1,
                     mp2,
                     WinGetLastError(WinQueryAnchorBlock(hFrame))
                   );
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Return Panel ID to use for "Keys Help"                               Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case HM_QUERY_KEYS_HELP:

         *pmrValue = MRFROMSHORT(PID_KEYSHELP);
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Check for pulldown selections we're interested in                    Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_COMMAND:

         switch (LOUSHORT(mp1))
         {
           /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
           /*Ё Display the "Help for Help" panel                             Ё*/
           /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
           case ID_H_HELP:

                usPanelID = PID_HELPHELP;
           break;

           /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
           /*Ё Sorry, we just don't care!                                    Ё*/
           /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/

           default:
                bHandled = BOOL_FALSE;          /* we DID NOT handle... */
         }

         /*зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
         /*Ё Now check to see if we should display a Panel...                Ё*/
         /*юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
         if ( usPanelID != 0 )
           WinSendMsg(hIPF,
                      HM_DISPLAY_HELP,
                      MPFROMSHORT(usPanelID),
                      MPFROMSHORT(HM_RESOURCEID) );    /* i've given a Panel ID */
    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё If the "top level" frame window is being destroyed, stop IPF...      Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    case WM_DESTROY:
         if (hWnd == hFrame)
         {
           WinDestroyHelpInstance(hIPF);
           hIPF = hFrame = NULLH;
         }

         bHandled = BOOL_FALSE;            /* we DID NOT handle... */

    break;

    /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
    /*Ё Otherwise, not a message we need to consider for IPF...              Ё*/
    /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
    default:
         bHandled = BOOL_FALSE;            /* we DID NOT handle... */
  }

  /*здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©*/
  /*Ё Return if this is an HM_xxx message we've handled...                   Ё*/
  /*юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды*/
  return( bHandled );
}
