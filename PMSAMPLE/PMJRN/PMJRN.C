/*---------------------------------------------------------------------------+
+----------------------------------------------------------------------------+
|                                                                            |
| PMJRN                                                                      |
|                                                                            |
| Program to record,playback and play testcases on PM applications           |
+-------------------------------------+--------------------------------------+
| Version: 1.0                        |   Marc Fiammante (FIAMMANT at LGEVM2)|
+-------------------------------------+--------------------------------------+
| Inspired from PMSPY on OS2TOOLS                                            |
+-------------------------------------+--------------------------------------+
| History:                                                                   |
| --------                                                                   |
|                                                                            |
| created: Marc Fiammante August 1989                                        |
+---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
| Includes                                                                   |
+---------------------------------------------------------------------------*/

#define DEFINE_VARIABLES

#include "pmjrn.h"                      /* Resource symbolic identifiers*/

/*---------------------------------------------------------------------------+
| Main                                                                       |
+---------------------------------------------------------------------------*/

VOID main(int argc, char *argv[], char * envp[])              /* @C1C */
{
  register BOOL   get_another_msg;             /* control PM msg loop */
           QMSG   qmsg;
  register USHORT i,
                  length;
           CHAR   nlsString[256];
           ULONG  CtrlData = FCF_STANDARD & ~FCF_SHELLPOSITION;

  hab = WinInitialize(0);
  ArgC = argc;                                                      /* @C1C */
  ArgV = argv;                                                      /* @C1C */

  /* load all of our strings, once and for all */
  for (/* init */ i = 0, length = 1;
       /* term */ (i < IDS_TOTAL) && (length != 0);  /* Loop for all strings */
       /* iter */ i++) {
      /* Load the string into a local variable */
      if ( (length = WinLoadString(                     /* Load the string              */
                                    hab,                /*  Required anchor block handle*/
                                    0,               /*  Resource module             */
                                    i,                  /*  Resource ID                 */
                                    sizeof(nlsString),  /*  Max length                  */
                                    nlsString) )        /*  Addr of string buffer       */
           != 0) {
        /* duplicate string of subsequent use */
        if ( ( Strings[i] = strdup( nlsString)) == 0)                           /*  Addr of string buffer       */
          length = 0;   /* fail if unable to DUP it... */
      } /* endif */
  } /* endfor */

  if (length == 0)                                     /* Error in Loop                */
    exit(1);

  if ( (hmq = WinCreateMsgQueue(hab, atoi(Strings[IDS_MAX_PM_Q_SIZE]))) != 0) {
    strcpy(szWndTitle, Strings[IDS_TITLE]);
    DosLoadModule(0, 0, Strings[IDS_DLL_NAME], &hSpyDll);
    if (DLLVERSION != SpyDllVersion()) {
      sprintf(szText, Strings[IDS_FMT_LEVEL], VERSION, DLLVERSION, SpyDllVersion());
      WinMessageBox(HWND_DESKTOP,
                    HWND_DESKTOP,
                    (PSZ)szText,
                    (PSZ)szWndTitle,
                    0,
                    MB_CUAWARNING | MB_CANCEL | MB_MOVEABLE );
    } else {
      if (Installed=SpyInstalled(3)) {
        if (SpyInstalled((Installed%2)+1)) {
          /* there is allready an installed hook */
        } else {
          /* we are the first ones so eventually install hooks */
        } /* endif */

        hSpy = WinLoadPointer(HWND_DESKTOP, 0, ID_JOURNAL_POINTER);
        sprintf(szWndTitle, Strings[IDS_FMT_TITLE], Strings[IDS_TITLE], Installed);

        WinRegisterClass(                   /* Register Window Class        */
            hab,                            /* Anchor block handle          */
            (PSZ)"MyWindow",                /* Window Class name            */
            (PFNWP)SpyWindowProc,           /* Address of Window Procedure  */
            CS_SIZEREDRAW,                  /* No special class style       */
            0);                             /* No extra window words        */
        hwndFrame = WinCreateStdWindow(
            (HWND)HWND_DESKTOP,             /* Desktop Window is parent     */
            FS_ICON,
            (PVOID) &CtrlData,
            (PSZ)"MyWindow",                 /* Window Class name            */
            szWndTitle,
            0 /*WS_VISIBLE*/,             /* Client style - visible       */
            0,                            /* Module handle                */
            (USHORT)ID_MAINWND,              /* Window ID                    */
            (HWND FAR *)&hwndClient);        /* Client Window handle         */
        if (hwndFrame) {
          WinSetWindowPos( hwndFrame,   /* Shows and activates frame    */
                   HWND_TOP,            /* window at position 150, 250, */
                   150, 250, 550, 200,  /* and size 450, 160.           */
                   SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW
                 );
          WinSetWindowText(hwndFrame, szWndTitle);
          swcntrl.hwnd = hwndFrame;
          strcpy(swcntrl.szSwtitle, szWndTitle);
          hSwitch = WinAddSwitchEntry((PSWCNTRL)&swcntrl);

          /*<f>25*****************************************************************
          *
          * Process the PM Message queue
          *
          * - get the next Msg
          *
          ***********************************************************************/

          do
          {
            if (get_another_msg = WinGetMsg(hab,       /* Required anchor block */
                                            &qmsg,     /* Addr of msg structure */
                                            0,      /* Filter window (none)  */
                                            0,      /* Filter begin    "     */
                                            0)   )  /* Filter end      "     */

            /**********************************************************************
            *
            * is this our notice to ourself that we're really ready to Quit?
            *
            * if so,  leave 'get_another_msg' set FALSE
            * if not, leave 'get_another_msg' set TRUE  & dispatch to proper window
            *
            ***********************************************************************/

            {
              if (get_another_msg = (qmsg.msg != PMSPY_QUIT_NOTICE) )
                WinDispatchMsg( hab, &qmsg );
            } /* endif */

            /***********************************************************************
            *
            * we've got a WM_QUIT message....
            *
            * - it can only come from a Task Manager CLOSE operation
            *
            * - tell PM we're going to ignore this WM_QUIT
            *
            * - prompt the user to see if he really does want to terminate
            *   (done by simulating a EXIT menu item selection)
            *
            * Note: at this point, we know that 'get-another-msg' is FALSE...
            *
            ***********************************************************************/

            else
            {
              WinCancelShutdown(hmq, FALSE);           /* ignoring WM_QUIT */

              get_another_msg = TRUE;                  /* must get more MSGs! */

              WinPostMsg( hwndFrame,                   /* Window Handle */
                          WM_COMMAND,                  /* Message ID */
                          (MPARAM) ID_AB_EXIT,         /* P1 = fake EXIT selection */
                          (MPARAM) 0);              /* P2 = NONE */
            } /* endif */
          } while ( get_another_msg );                 /* obey what he says... */

          WinRemoveSwitchEntry(hSwitch);
          WinDestroyWindow(hwndFrame);
        } else {
          /* main window creation failed */
        } /* endif */
      } else {
        /* no more installations possible */
        WinMessageBox(HWND_DESKTOP,
                      HWND_DESKTOP,
                      Strings[IDS_MSG_TOO_MANY_SPIES],
                      szWndTitle,
                      0,
                      MB_CUAWARNING | MB_CANCEL | MB_MOVEABLE );
      } /* endif */
    } /* endif */
    WinDestroyMsgQueue(hmq);
  } else {
    /* queue not created */
  } /* endif */

  if (Installed) {
    if (SpyInstalled((Installed%2)+1)) {
      /* another spy active so don't release the hooks */
    } else {
    } /* endif */
    SpyInstalled(Installed+4);
  } /* endif */

  DosFreeModule(hSpyDll);
  WinTerminate(hab);
  DosExit(EXIT_PROCESS, 0);
}

/*--- end of file ----------------------------------------------------------*/
