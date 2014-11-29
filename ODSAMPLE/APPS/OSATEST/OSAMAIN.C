/***************************************************************************
 *
 * File name   :  osamain.c
 *
 ******************************************************************************/

/* Include the required sections from the PM header files.  */
#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS
#define INCL_WINDIALOGS
#define INCL_WINFRAMEMGR
#define INCL_WINERRORS
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINPOINTERS
#define INCL_WINSTDDRAG
#define INCL_WINSTDFILE
#define INCL_WINSTDFONT
#define INCL_WINSYS
#define INCL_WINTIMER
#define INCL_WINWINDOWMGR
#define INCL_OSAAPI
#define INCL_OSA
#include <os2.h>

/* c language includes */
#include <ctype.h>
#include <memory.h>
#include <process.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\stat.h>
#include <sys\types.h>

/* application includes */
#include "pmassert.h"
#include "osatest.h"
#include "osadlg.h"
#include "ucmenus.h"
#include "ucmutils.h"

/* Create static table of all supported menu items.  End of list indicated by ID=0.*/
/* We use this list to map action string to IDs when the user selects an action    */
/* on a UC menu.  It is also used to track the check status of checkable items (we */
/* cannot depend on the menu to keep track since the item can be deleted from the  */
/* menu by the user).                                                              */

AppItemStruct ItemList[] = {
   /* Item ID                UCMenu Action String    Description                                 */
   /*--------------------    ----------------------  --------------------------------------------*/
   {IDM_OSASENDEVENTS   ,   "OSATest: Send Events"       ,"Send Events to OpenDoc or other OSA apps"           },
   {IDM_OSARECORDEVENTS ,   "OSATest: Record Events"     ,"Receive and display recorded events"                },
   {IDM_OSASCRIPTEDITOR ,   "OSATest: Script Editor"     ,"Sample Script Editor to execute and record scripts" },
   {0, "", ""}}; /* End of list */

/**************************************************************************
 *
 *  Name       : main(argc, argv)
 *
 *************************************************************************/
INT main(INT argc, CHAR **argv)
{
                                        /* Define variables by type     */
   BOOL       bOK;                      /* Boolean used for return code */
   HAB        hab;                      /* PM anchor block handle       */
   HMQ        hmq;                      /* Message queue handle         */
   QMSG       qmsg;                     /* Message from message queue   */
   ULONG      ulCtlData;                /* Standard window create flags */
   HWND       hwndFrame=(HWND)NULL;     /* Frame window handle          */
   HWND       hwndClient=(HWND)NULL;    /* Client area window handle    */
   PMAIN_PARM pmp;                      /* Main parameters structure    */
   SWCNTRL    swctl;                    /* Struct to add to window list */
   HSWITCH    hsw;                      /* Window list handle ret'd     */
   CHAR       szWork[ LEN_WORKSTRING ]; /* General use string work area */
   PSZ        pszTitle;                 /* Pointer to program title     */
   ERRORID    errid;

   /* normal PM application startup */
   hab = WinInitialize( 0 );
   hmq = WinCreateMsgQueue( hab, 0 );

   /*
    * Register a class for my client window's behavior.
    * This class has enough extra window words to hold a pointer.
    */
   bOK = WinRegisterClass(
                     hab,
                     CLASSNAME,
                     WinProc,
                     CS_SIZEREDRAW,
                     sizeof( PMAIN_PARM ));
   /*
    * Ensure WinRegisterClass worked ok; if not, present a message box.
    * ( See pmassert.h. )
    */
   pmassert( hab, bOK );

   /* Load program title and allocate a local copy. Use it in help creation. */
   WinLoadString( hab, (HMODULE)NULLHANDLE,
                  PROGRAM_TITLE, LEN_WORKSTRING, szWork );
   pszTitle = strdup( szWork );


   /* flags to control creation of window; use on call to WinCreateStdWindow */
   ulCtlData = FCF_SYSMENU | FCF_TITLEBAR | FCF_BORDER;


   hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
/*                                   WS_VISIBLE | FS_ICON | FS_SHELLPOSITION, */
                                   FS_ICON | FS_SHELLPOSITION,
                                   &ulCtlData,
                                   CLASSNAME,
                                   NULL,    /* title text set in prtcreat.c */
                                   0,       /* client style                 */
                                   (HMODULE)NULLHANDLE,  /* resources in exe */
                                   ID_OSATEST,
                                   &hwndClient  );
   pmassert( hab, hwndFrame );
   pmassert( hab, hwndClient );

   /* create.c placed pointer to main params in client's window words; get it */
   pmp = (PMAIN_PARM) WinQueryWindowULong( hwndClient, QWL_USER );

   /* Add program to task list. */
   memset( &swctl, 0, sizeof( SWCNTRL ));
   strcpy( swctl.szSwtitle, pmp->pszTitle );
   swctl.hwnd          = hwndFrame;
   swctl.uchVisibility = SWL_VISIBLE;
   swctl.fbJump        = SWL_JUMPABLE;
   hsw = WinAddSwitchEntry( &swctl );

   /* message loop */
   while( WinGetMsg( hab, &qmsg, (HWND)NULLHANDLE, 0, 0 ))
   {
     WinDispatchMsg( hab, &qmsg );
   }

   /* clean up */
   WinRemoveSwitchEntry( hsw );
   WinDestroyWindow( hwndFrame );
   WinDestroyMsgQueue( hmq );
   WinTerminate( hab );

   DosWaitThread( &pmp->tidObjectThread, DCWW_WAIT );
   return 0;
}  /* End of main() */


/*************************************************************************
 *
 * Name       : WinProc(hwnd, msg, mp1, mp2)
 *
 * Description: Processes the messages sent to the main client
 *              window.  This routine processes the basic
 *              messages all client windows should process.
 *
 * Result     : MRESULT      message result
 *
 *************************************************************************/
MRESULT EXPENTRY WinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   BOOL                bOK;
   PMAIN_PARM          pmp;
   HPS                 hps;
   RECTL               rectl;
   PSZ                 psz;
   CHAR                szWork[ LEN_WORKSTRING ];
   ULONG               ulWork;
   ULONG               rc;
   UCMINFO             UCMInit;      /* UC menu initialization data */
   UCMITEM            *UCItem;       /* Ptr to UCMenu item data       */
   HWND                DummyHwnd;    /* Text menu handle (unused) */
   USHORT              CmdIndex,i;
   SWP                 swp,swp2;

   switch(msg)
   {
   case WM_CLOSE:
      /* obtain the main parameter pointer from window words */
      pmp = (PMAIN_PARM) WinQueryWindowULong( hwnd, QWL_USER );
      /* Do OSA termination */
      TerminateOSA(hwnd);
      if( pmp->fBusy )
      {
            /* close down the application in spite of being busy */
            pmp->fCancel = TRUE;
            /* Tell object window to close, quit, and post me a WM_QUIT */
            WinPostMsg( pmp->hwndObject, WM_USER_CLOSE, (MPARAM)hwnd, (MPARAM)0 );
      }
      else
      {
         /* not busy, so initiate closure by telling object window to close */
         WinPostMsg( pmp->hwndObject, WM_USER_CLOSE, (MPARAM)hwnd, (MPARAM)0 );
      }
      return (MRESULT)NULL;

   case WM_CONTROL:

      switch(SHORT1FROMMP(mp1))
      {
        case ID_COMMANDBAR:
          switch (SHORT2FROMMP(mp1))
          {
            case UCN_QRYBUBBLEHELP:
            {  /* CommandBar needs bubble help text */
               /* Toolbar needs to know what bubble help text to display. */
               /* We keep bubble help in the pszData field of the UCMITEM */
               /* structure, passed to us via mp2.  Note we must return   */
               /* a COPY which UCMenus will free when it is done with it. */
               QBUBBLEDATA *BubbleInfo;
               BubbleInfo = (QBUBBLEDATA *)mp2;

               if (UCMFIELD(BubbleInfo->MenuItem, pszData) != NULL)
                 BubbleInfo->BubbleText = UCMenuStrdup(UCMFIELD(BubbleInfo->MenuItem, pszData));
               return 0;
            }

            case UCN_QRYDEFTEMPLATEID:
              /* User wants to reload default menu config.  Tell UC menus */
              /* what menu resource to load and replace current menu.     */
              /* This is the ID we originally loaded, which is the same   */
              /* as the ID of the UC menu window itself.                  */
              *(USHORT *)mp2 = SHORT1FROMMP(mp1);
              return (MRESULT)TRUE;
            case UCN_QRYTEMPLATEMODULE:
              /* Need to tell UCMenu where to module to load resources    */
              /* from.  Return NULLHANDLE for EXE-bound resources.        */
              *(HMODULE *)mp2 = NULLHANDLE;
              return (MRESULT)TRUE;
            case UCN_ITEMSELECTED: /* User made a selection on a toolbar */

              UCItem = (UCMITEM *)mp2;
              if (UCItem->pszAction != NULL)
              { /* Make sure there is an action string */

                /* See if we recognize this action.  If so, send ourselves */
                /* a message to process the command index.                 */

                CmdIndex = FindActionInItemList(UCItem->pszAction);
                if (CmdIndex >= 0)
                  WinSendMsg(hwnd, MSG_PROCESS_COMMAND, MPFROMSHORT(CmdIndex), 0L);
                 return 0;
              }
           }
      }
      break;

   case WM_COMMAND:
      return Menu( hwnd, msg, mp1, mp2 );

   case WM_USER_ACK:
      break;

   case MSG_PROCESS_COMMAND:
      {
        USHORT CmdIndex;
        USHORT Attr;
        /* A menu command needs to be processed, either from a toolbar  */
        /* or the normal PM text menu.  The index of the command in the */
        /* ItemList[] array is in mp1.  This is where a lot of          */
        /* application specific code would go if this were a real       */
        /* drawing program.                                             */
        CmdIndex = SHORT1FROMMP(mp1);

        switch (ItemList[CmdIndex].CommandID)
        {
           case IDM_OSASENDEVENTS:   WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDM_OSASENDEVENTS), NULL);
              return (MRESULT)NULL;
           case IDM_OSARECORDEVENTS: WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDM_OSARECORDEVENTS), NULL);
              return (MRESULT)NULL;
           case IDM_OSASCRIPTEDITOR: WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDM_OSASCRIPTEDITOR), NULL);
              return (MRESULT)NULL;
           case WM_CLOSE: WinPostMsg(hwnd, WM_CLOSE, NULL, NULL);
              return (MRESULT)NULL;
        }

      } /* end of MSG_PROCESS_COMMAND */
    case WM_ACTIVATE:
      /* Tell toolbars we are getting/losing focus */
      pmp = (PMAIN_PARM) WinQueryWindowULong( hwnd, QWL_USER );
      WinSendMsg(pmp->hwndCommandBar, UCMENU_ACTIVECHG, mp1, MPVOID);
      break;

   case WM_CREATE:

      /* allocate main block of parameters. see osatest.h */
      pmp = (PMAIN_PARM)calloc( 1, sizeof( MAIN_PARM ));

      /* set main parmeters pointer into client window words */
      bOK = WinSetWindowULong( hwnd, QWL_USER, (ULONG) pmp );

      /* store hab and important window handles in the pmp */
      pmp->hab          = WinQueryAnchorBlock( hwnd );
      pmp->hwndClient   = hwnd;
      pmp->hwndFrame    = WinQueryWindow( hwnd, QW_PARENT );
      pmp->hwndTitlebar = WinWindowFromID( pmp->hwndFrame, FID_TITLEBAR );
      pmp->hwndMenubar  = WinWindowFromID( pmp->hwndFrame, FID_MENU );

      /* Create graphic toolbars for edges of frame */

      memset(&UCMInit, 0x00, sizeof(UCMINFO));    /* Setup initialization data */
      UCMInit.cb = sizeof(UCMINFO);
      UCMInit.hModule = NULLHANDLE;         /* All resources are bound to EXE */
      UCMInit.BgBmp   = 0x00CCCCCC;         /* Light grey is bkgnd color in bitmaps */
      UCMInit.BgColor = 0x00B0B0B0;         /* Darker grey is toolbar background */
      UCMInit.ItemBgColor = WinQuerySysColor(HWND_DESKTOP,SYSCLR_MENU,0L); /* Items are menu color */
      UCMInit.BubbleDelay = 1000L;          /* Instant bubble help */
      UCMInit.BubbleRead = 3500L;           /* 3.5 seconds to autodismiss bubble help */

      UCMInit.Style   = UCS_FRAMED |        /* Use 3D effects */
                        UCS_CHNGBMPBG_AUTO |/* Auto-detect bitmap background color */
                        UCS_NODEFAULTACTION|/* Don't put 'exec pgm' on list of actions */
                        UCS_CUSTOMHLP |     /* We will provide all help */
                        UCS_BUBBLEHELP;     /* Enable bubble help */
      pmp->hwndCommandBar = UCMenuCreateFromResource(pmp->hab,
                    pmp->hwndFrame,         /* Frame is parent of its controls */
                    pmp->hwndClient,        /* Client will get messages */
                    CMS_HORZ | WS_VISIBLE,  /* Horizontal orientation */
                    0,0,0,0,                /* Size/position will be done by frame */
                    HWND_TOP,               /* Put on top of siblings */
                    ID_COMMANDBAR,          /* ID new menu will have */
                    NULLHANDLE,             /* Resources come from EXE */
                    ID_COMMANDBAR,          /* ID of menu template in resource file */
                    &UCMInit,               /* Initialization data structure */
                    &DummyHwnd);            /* Returned text menu handle (not used) */

      UCMUtilsAddToFrame(pmp->hwndFrame, pmp->hwndCommandBar, UCMUTILS_PLACE_TOP);


      /* get program title; put in pmp and title bar */
      bOK = WinLoadString( pmp->hab, (HMODULE)NULLHANDLE,
                           PROGRAM_TITLE, LEN_WORKSTRING, szWork );
      pmp->pszTitle = strdup( szWork );
      bOK = WinSetWindowText( pmp->hwndTitlebar, pmp->pszTitle );

      /* Do OSA Initialization */
      InitOSA(hwnd);

      /* do more initialization on thread 2. See WM_CREATE case in prtobj.c */
      /* create object window which operates on thread 2 */
      /* see prtobj.c for thread 2 code and object window procedure */
      pmp->tidObjectThread = (TID)_beginthread( threadmain, NULL, LEN_STACK, pmp );
      pmassert( pmp->hab, pmp->tidObjectThread != 0 );
      return (MRESULT)NULL;


   case WM_MOUSEMOVE:
      return (MRESULT) 1;

   case WM_PAINT:
      pmp = (PMAIN_PARM) WinQueryWindowULong( hwnd, QWL_USER );

      /* do not rely on client window rectangle being correct */
      WinQueryUpdateRect( hwnd, &rectl );
      WinQueryWindowRect( hwnd, &rectl );

      hps = WinBeginPaint( hwnd, (HPS) 0, &rectl );
      bOK = WinFillRect( hps, &rectl, SYSCLR_WINDOW );
      WinEndPaint( hps );
      break;

   case WM_SEMANTICEVENT:
      /* Handle Apple Event Manager Semantic Event */
      /* Call ProcessSemanticEvent to process the Apple Event */
      ProcessSemanticEvent( hwnd, msg, mp1, mp2 );
      return (MRESULT) 0;


   case WM_TIMER:
      return (MRESULT) 0;

   }
   return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}  /* End of WinProc */


/**************************************************************************
 *
 *  Name       : Menu()
 *
 *  Description: Processes commands initiated from the menu bar.
 *
 *  Return     :  depends on message sent
 *************************************************************************/
MRESULT Menu( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   PMAIN_PARM pmp;
   ULONG      rc;
   BOOL       bOK;

   /* obtain the main parameter pointer from window words */
   pmp = (PMAIN_PARM)WinQueryWindowULong( hwnd, QWL_USER );

   switch( msg )
   {
   case WM_COMMAND:
      switch(SHORT1FROMMP(mp1))
      {
      case IDM_OSASENDEVENTS:
         if(pmp->hwndTestEvent)
           WinSetActiveWindow(HWND_DESKTOP,pmp->hwndTestEvent);
         else
           WinLoadDlg( HWND_DESKTOP,
                        hwnd,
                        (PFNWP)OSAOpendocEventsDlgProc,
                        (HMODULE)NULLHANDLE,
                        IDD_ODSENDEVENTS,
                        (PVOID)pmp );
         return (MRESULT)NULL;

      case IDM_OSARECORDEVENTS:
         if(pmp->hwndRecordEvent)
           WinSetActiveWindow(HWND_DESKTOP,pmp->hwndRecordEvent);
         else
           WinLoadDlg( HWND_DESKTOP,
                        hwnd,
                        (PFNWP)OSARecordEventsDlgProc,
                        (HMODULE)NULLHANDLE,
                        IDD_ODRECORDEVENTS,
                        (PVOID)pmp );
         return (MRESULT)NULL;

      case IDM_OSASCRIPTEDITOR:
         if(pmp->hwndScriptEditor)
           WinSetActiveWindow(HWND_DESKTOP,pmp->hwndScriptEditor);
         else
           WinLoadDlg( HWND_DESKTOP,
                        hwnd,
                        (PFNWP)OSAScriptEditorDlgProc,
                        (HMODULE)NULLHANDLE,
                        IDD_SCRED,
                        (PVOID)pmp );
         return (MRESULT)NULL;
      }
      break;

   }
   return (MRESULT)NULL;
}   /*  end of Menu()  */

/*----------------------------------------------------------------------------*/
int FindActionInItemList(PSZ Action)
/*----------------------------------------------------------------------------*/
/* Look through the list of all the 'actions' this application supports and   */
/* return the index into the ItemList array of the matching entry.  If the    */
/* specified action is not supported, return -1.                              */
/*----------------------------------------------------------------------------*/
{
int i;
  if (Action == NULL)  /* Check for nonexistant action string */
    return -1;

  for (i=0; ItemList[i].CommandID != 0; i++) {
    if (STRSAME(ItemList[i].Action, Action)) /* We found the action in the list */
      return i;
  } /* for each item in list */

  return -1;  /* Did not find the action in the list */
}
/***************************  End of osamain.c ****************************/
