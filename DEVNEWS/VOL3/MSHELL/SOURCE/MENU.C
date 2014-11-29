// menu.c

// menu commands get processed here. Called from WM_COMMAND case in mshell.c


#define INCL_PM
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "def.h"
#include "mshell.h"
#include "pmassert.h"


MRESULT Command( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  PGLOBALS    pg;
  ULONG       rc;


  pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );

  switch( SHORT1FROMMP( mp1 )) {
  case IDM_ABOUT:

#define ABOUT_TEXT "\
MShell is an alternative, simple shell for OS/2 2.X that uses the \
replaceable shell architecture of the Workplace Shell.\n\
\n\
To configure MShell, name MSHELL.EXE on the RUNWORKPLACE setting in CONFIG.SYS and reboot. \
MShell uses a plain text INI file called MSHELL.INI located in the root of the \
boot drive.  If it does not exist, MShell will create one.\n\
\n\
(c) Copyright International Business Machines Corporation 1992.\n\
All rights Reserved.\n\
\n\
Monte Copeland, IBM Boca Raton."

    WinMessageBox( HWND_DESKTOP, pg->hwndFrame, ABOUT_TEXT, CAPTION, 0, MB_CANCEL | MB_INFORMATION );
    break;



  case IDM_CMD:
    WinSendMsg( hwnd, WM_USER_DISABLE_CLIENT, 0, 0 );
    WinPostMsg( pg->hwndObject, WM_USER_START_CMD, (MPARAM) hwnd, 0 );
    break;

  case IDM_REFRESH:
    WinSendMsg( hwnd, WM_USER_DISABLE_CLIENT, 0, 0 );
    WinPostMsg( pg->hwndObject, WM_USER_ADD_PROGRAMS, (MPARAM) hwnd, 0 );
    break;

  case IDM_SAVE:
    WinBroadcastMsg( HWND_DESKTOP, WM_SAVEAPPLICATION, 0, 0, BMSG_POST );
    break;

  case IDM_SHUTDOWN:
    rc = WinMessageBox( HWND_DESKTOP, pg->hwndFrame, "Shutdown now?", CAPTION, 0, MB_YESNOCANCEL );
    if( MBID_YES == rc ) {
      pg->fShutdownCalled = TRUE;
      WinShutdownSystem( pg->hab, pg->hmq );
    }
    break;


  case IDM_SPOOLER:
    // show the spooler dialog
    WinSetWindowPos( pg->hwndSpooler, HWND_TOP, 0,0,0,0, SWP_ACTIVATE | SWP_SHOW | SWP_ZORDER );
    // punch the refresh button for the user
    WinPostMsg( pg->hwndSpooler, WM_COMMAND, (MPARAM)IDC_REFRESH, 0 );
    break;

  }
  return (MRESULT) 0;
}

