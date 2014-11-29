#define INCL_WINSTDFILE  /* Window Standard File Functions       */
#define INCL_WINBUTTONS
#define INCL_WINDIALOGS
#define INCL_WINMLE
#define INCL_WINERRORS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINSTDFILE
#define INCL_WINSTDFONT
#define INCL_WINWINDOWMGR
#define INCL_WINENTRYFIELDS
#define INCL_WINLISTBOXES
#define INCL_WINSWITCHLIST
#define INCL_WINSYS
#define INCL_WINTIMER
#define INCL_ODAPI
#define INCL_ODOSA
#define INCL_OSAAPI
#define INCL_OSA
#define INCL_OSACOMPONENT
#define INCL_ODCOMPONENT
#include <os2.h>

/* c language includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <process.h>
#include <memory.h>
#include <sys\types.h>
#include <sys\stat.h>

/* application includes */
#include "osatest.h"
#include "osadlg.h"
#include "ucmenus.h"
#include "ucmutils.h"
#include "pmassert.h"

typedef struct _SCRED_PARM {
  PMAIN_PARM pmp;
  ComponentManager *cmgr;
  OSAGenericScriptingComponent *gsc;
  OSAID     scriptID;
  BOOL      bRecording;
  CHAR      szScriptEditor[CCHMAXPATH];
  CHAR      szScriptFileName[CCHMAXPATH];
  HWND      hwnd;
  HWND      hwndScriptToolBar;
  HWND      hwndScriptCmdBar;
  HWND      hwndListAppsDlg;
  ULONG     ulListType;
  AEDesc    scriptDesc;
  ULONG     ScriptCmd;
  ULONG     ulTimer;
} SCRED_PARM;
typedef SCRED_PARM *PSCRED_PARM;

#define SCR_COMPILE     0
#define SCR_PLAY        1
#define SCR_RECORD      2
#define SCR_STOP        3

#define ERR_RECORDING_IN_PROCESS   "...already recording"
#define ERR_RECORDING_NOPLAY       "...play not allowed while recording"
#define ERR_RECORDING_NOCOMPILE    "...check syntax not allowed while recording"
#define COMPILE_OK    "Check Syntax was successful..."
#define PLAY_OK       "Play was successful..."

/* Create static table of all supported menu items.  End of list indicated by ID=0.*/
/* We use this list to map action string to IDs when the user selects an action    */
/* on a UC menu.  It is also used to track the check status of checkable items (we */
/* cannot depend on the menu to keep track since the item can be deleted from the  */
/* menu by the user).                                                              */

AppItemStruct ScriptItemList[] = {
   /* Item ID              UCMenu Action String    Description                                 */
   /*--------------------  ----------------------  --------------------------------------------*/
   {IDD_SCRED_NEW        , "Cmd: New"             ,"Create a new script",                        },
   {IDD_SCRED_OPEN       , "Cmd: Open"            ,"Load script from a file."                    },
   {IDD_SCRED_SAVE       , "Cmd: Save"            ,"Save script to a file."                      },
   {IDD_SCRED_SAVEAS     , "Cmd: SaveAs"          ,"Save script to a another file."              },
   {IDD_SCRED_PLAY       , "Cmd: Play"            ,"Play the script."                            },
   {IDD_SCRED_RECORD     , "Cmd: Record"          ,"Start recording events."                     },
   {IDD_SCRED_STOP       , "Cmd: Stop"            ,"Stop recording events."                      },
   {IDD_SCRED_COMPILE    , "Cmd: Compile"         ,"Check syntax of script."                     },
   {IDD_SCRED_CUT        , "Cmd: Cut"             ,"Cut selected text to clipboard."             },
   {IDD_SCRED_COPY       , "Cmd: Copy"            ,"Copy selected text to clipboard."            },
   {IDD_SCRED_PASTE      , "Cmd: Paste"           ,"Paste clipboard text into script."           },
   {IDD_SCRED_UNDO       , "Cmd: Undo"            ,"Undo the last action."                       },
   {IDD_SCRED_FONTS      , "Cmd: Fonts"           ,"Change font."                                },
   {IDD_SCRED_STYLES     , "Styles Submenu"       ,"Style Options (submenu)."                    },
   {IDD_SCRED_BOLD       , "Style: Bold"          ,"Make text bold."                             },
   {IDD_SCRED_ITALIC     , "Style: Italic"        ,"Make text italic."                           },
   {IDD_SCRED_UNDERLINE  , "Style: Underline"     ,"Make text underlined."                       },
   {0, "", ""}}; /* End of list */

void InitScriptEditor(PSCRED_PARM prec);
void ClearScript(PSCRED_PARM prec);
void OpenScript(PSCRED_PARM prec);
void SaveScript(PSCRED_PARM prec);
void SaveAsScript(PSCRED_PARM prec);
void StartRecordingScript(PSCRED_PARM prec);
void StopRecordingScript(PSCRED_PARM prec);
void PlayScript(PSCRED_PARM prec);
void CompileScript(PSCRED_PARM prec);
void BuildScriptDesc(PSCRED_PARM prec);
void AddRecordedText(PSCRED_PARM prec, PCHAR pch, Size size);
void ClipboardCut(PSCRED_PARM prec);
void ClipboardCopy(PSCRED_PARM prec);
void ClipboardPaste(PSCRED_PARM prec);
void Undo(PSCRED_PARM prec);
void SetFont(PSCRED_PARM prec);
void FontBold(PSCRED_PARM prec);
void FontItalic(PSCRED_PARM prec);
void FontUnderline(PSCRED_PARM prec);
MRESULT ProcessUCMenuControlMsg(HWND hwnd, MPARAM mp1, MPARAM mp2);
int FindActionInScriptItemList(PSZ Action);
void ClearStatusText(PSCRED_PARM prec);
void AddStatusText(PSCRED_PARM prec, PCHAR pch, Size size);
void DisplayScriptError(PSCRED_PARM prec);
void PromptUser(PSCRED_PARM prec);
void HandleWindowSizeChange(PSCRED_PARM prec, HWND hwnd, MPARAM mp1, MPARAM mp2);
void _Optlink ScreditThread( void *pv );

void InitScriptEditor(PSCRED_PARM prec)
{
   HWND            hwndTemp;
   HWND            DummyHwnd;   /* Dummy menu window (unused) */
   SWP             swp;
   UCMINFO         UCMInit;     /* Graphic toolbar setup data */
   CHAR            szTemp[CCHMAXPATH];
   ComponentDescription   theComponent;
   Environment    *ev = somGetGlobalEnvironment();

   WinSetWindowULong(prec->hwnd, QWL_USER, (ULONG) prec );

   WinQueryWindowText(prec->hwnd, CCHMAXPATH-1, szTemp);
   strcpy(prec->szScriptEditor,szTemp);
   prec->szScriptFileName[0] = NULL;
   WinSetWindowText(prec->hwnd, strcat(szTemp,"<Untitled>"));
   hwndTemp = WinWindowFromID(prec->hwnd, IDD_SCRED_STATUSTEXT);
   WinSetWindowText(hwndTemp, "Idle");
   prec->cmgr = new ComponentManager;
   theComponent.componentType    = kOSAComponentType;
   theComponent.componentSubType = kOSAGenericScriptingComponentSubtype;
   prec->gsc = (OSAGenericScriptingComponent *) prec->cmgr->OpenComponent(ev, &theComponent);
   prec->scriptDesc.descriptorType = typeNull;
   prec->scriptDesc.dataHandle     = NULL;
   prec->scriptID = kOSANullScript;
   prec->bRecording = FALSE;
   prec->ulTimer    = 0;
   prec->pmp->hwndScriptEditor = prec->hwnd;

   /* First, set all the basic values into the UCMINFO structure */

   memset(&UCMInit, 0x00, sizeof(UCMINFO));    /* Setup initialization data */
   UCMInit.cb = sizeof(UCMINFO);
   UCMInit.hModule = NULLHANDLE;         /* All resources are bound to EXE */
   UCMInit.BgBmp   = 0x00CCCCCC;         /* Light grey is bkgnd color in bitmaps */
   UCMInit.BgColor = 0x00B0B0B0;         /* Darker grey is toolbar background */
   UCMInit.ItemBgColor = WinQuerySysColor(HWND_DESKTOP,SYSCLR_MENU,0L); /* Items are menu color */
   UCMInit.BubbleDelay = 1000L;          /* 1 second hover delay */
   UCMInit.BubbleRead = 3000L;           /* 3 second read time */

   /* Command menu */
   UCMInit.Style   = UCS_FRAMED |        /* Use 3D effects */
                        UCS_CHNGBMPBG_AUTO |/* Auto-detect bitmap background color */
                        UCS_NODEFAULTACTION|/* Don't put 'exec pgm' on list of actions */
                        UCS_CUSTOMHLP |     /* We will provide all help */
                        UCS_STATIC |        /* No direct manipulation */
                        UCS_BUBBLEHELP;     /* Enable bubble help */
   prec->hwndScriptToolBar = UCMenuCreateFromResource(WinQueryAnchorBlock(prec->hwnd),
                 prec->hwnd,             /* Dialog (frame) is parent */
                 prec->hwnd,             /* Dialog is also owner to get msgs */
                 CMS_HORZ,               /* Horizontal orientation */
                 0,0,0,0,                /* Size/position will be done by frame */
                 HWND_TOP,               /* Put on top of siblings */
                 ID_SCRIPTTOOLBAR,           /* ID new menu will have */
                 NULLHANDLE,             /* Resources come from EXE */
                 ID_SCRIPTTOOLBAR,           /* ID of menu template in resource file */
                 &UCMInit,               /* Initialization data structure */
                 &DummyHwnd);            /* Returned text menu handle (not used) */

   prec->hwndScriptCmdBar = UCMenuCreateFromResource(WinQueryAnchorBlock(prec->hwnd),
                 prec->hwnd,             /* Dialog (frame) is parent */
                 prec->hwnd,             /* Dialog is also owner to get msgs */
                 CMS_HORZ,               /* Horizontal orientation */
                 0,0,0,0,                /* Size/position will be done by frame */
                 HWND_TOP,               /* Put on top of siblings */
                 ID_SCRIPTCMDBAR,        /* ID new menu will have */
                 NULLHANDLE,             /* Resources come from EXE */
                 ID_SCRIPTCMDBAR,        /* ID of menu template in resource file */
                 &UCMInit,               /* Initialization data structure */
                 &DummyHwnd);            /* Returned text menu handle (not used) */

   UCMUtilsAddToFrame(prec->hwnd, prec->hwndScriptToolBar, UCMUTILS_PLACE_TOP);
   UCMUtilsAddToFrame(prec->hwnd, prec->hwndScriptCmdBar, UCMUTILS_PLACE_BOTTOM);

   if(prec->pmp->Profile.scredit_cx != 0)
   {
     WinSetWindowPos(prec->hwnd,
                     HWND_TOP,
                     prec->pmp->Profile.scredit_x,
                     prec->pmp->Profile.scredit_y,
                     prec->pmp->Profile.scredit_cx,
                     prec->pmp->Profile.scredit_cy,
                     SWP_MOVE | SWP_SIZE );

     hwndTemp = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);
     WinQueryWindowPos(hwndTemp, &swp);
     WinSetWindowPos(hwndTemp,
                     HWND_TOP,
                     prec->pmp->Profile.screditScr_x,
                     prec->pmp->Profile.screditScr_y,
                     prec->pmp->Profile.screditScr_cx,
                     prec->pmp->Profile.screditScr_cy,
                     SWP_SIZE );
     hwndTemp = WinWindowFromID(prec->hwnd, IDD_SCRED_STATUS);
     WinQueryWindowPos(hwndTemp, &swp);
     WinSetWindowPos(hwndTemp,
                     HWND_TOP,
                     prec->pmp->Profile.screditStat_x,
                     prec->pmp->Profile.screditStat_y,
                     prec->pmp->Profile.screditStat_cx,
                     prec->pmp->Profile.screditStat_cy,
                     SWP_MOVE | SWP_SIZE );
     hwndTemp = WinWindowFromID(prec->hwnd, IDD_SCRED_STATUSTEXT);
     WinQueryWindowPos(hwndTemp, &swp);
     WinSetWindowPos(hwndTemp,
                     HWND_TOP,
                     prec->pmp->Profile.screditStatTxt_x,
                     prec->pmp->Profile.screditStatTxt_y,
                     prec->pmp->Profile.screditStatTxt_cx,
                     prec->pmp->Profile.screditStatTxt_cy,
                     SWP_MOVE | SWP_SIZE );

   }
   else
   {
    WinQueryTaskSizePos(WinQueryAnchorBlock(prec->hwnd),0,&swp);
    WinSetWindowPos(prec->hwnd,
                    HWND_TOP,
                    swp.x,
                    swp.y,
                    swp.cx,
                    swp.cy,
                    SWP_MOVE );
   }

   WinShowWindow(prec->hwnd,TRUE);
}

MRESULT EXPENTRY OSAScriptEditorDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   Environment    *ev;
   PSCRED_PARM     prec = NULL;
   HWND            hwndButton;
   CHAR            szTemp[CCHMAXPATH];
   PSZ             pszBuffer,pszTemp;
   SWP             swp;
   MRESULT         result;
   OSAError        retVal = noErr;

   switch(msg)
   {
      case WM_INITDLG:
         prec = (PSCRED_PARM) calloc(1, sizeof(SCRED_PARM));
         prec->pmp =  (PMAIN_PARM) mp2;
         prec->hwnd = hwnd;
         InitScriptEditor(prec);
         break;

      case WM_ACK_THREADCOMPLETE:
         hwndButton = WinWindowFromID(hwnd, IDD_SCRED_STATUSTEXT);
         WinSetWindowText(hwndButton, "Idle");
         WinSendMsg(hwnd, WM_ENABLE_DIALOG, NULL, NULL);
         break;

      case WM_DISABLE_DIALOG:
         WinEnableWindow(hwnd, FALSE);
         break;

      case WM_ENABLE_DIALOG:
         WinEnableWindow(hwnd, TRUE);
         break;

      case WM_TIMER:
         switch (SHORT1FROMMP(mp1))
         {
            case ID_RECORDTIMER:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               prec->ulTimer++;
               if(prec->ulTimer > 3)
                 prec->ulTimer = 1;
               hwndButton = WinWindowFromID(hwnd, IDD_SCRED_STATUSTEXT);
               switch(prec->ulTimer)
               {
                  case 1:
                     WinSetWindowText(hwndButton, "Recording.");
                     break;
                  case 2:
                     WinSetWindowText(hwndButton, "Recording..");
                     break;
                  case 3:
                     WinSetWindowText(hwndButton, "Recording...");
                     break;
               }
               return 0;
               break;
            case ID_PLAYTIMER:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               prec->ulTimer++;
               if(prec->ulTimer > 3)
                 prec->ulTimer = 1;
               hwndButton = WinWindowFromID(hwnd, IDD_SCRED_STATUSTEXT);
               switch(prec->ulTimer)
               {
                  case 1:
                     WinSetWindowText(hwndButton, "Playing.");
                     break;
                  case 2:
                     WinSetWindowText(hwndButton, "Playing..");
                     break;
                  case 3:
                     WinSetWindowText(hwndButton, "Playing...");
                     break;
               }
               return 0;
               break;
            case ID_COMPILETIMER:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               prec->ulTimer++;
               if(prec->ulTimer > 3)
                 prec->ulTimer = 1;
               hwndButton = WinWindowFromID(hwnd, IDD_SCRED_STATUSTEXT);
               switch(prec->ulTimer)
               {
                  case 1:
                     WinSetWindowText(hwndButton, "Checking Syntax.");
                     break;
                  case 2:
                     WinSetWindowText(hwndButton, "Checking Syntax..");
                     break;
                  case 3:
                     WinSetWindowText(hwndButton, "Checking Syntax...");
                     break;
               }
               return 0;
               break;
         }
         break;

      case WM_ADD_RECORDEDTEXT:
         prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
         AddRecordedText(prec, (PCHAR) mp1, (Size) mp2);
         break;

      case WM_ACTIVATE:
         /* Tell toolbar we are getting/losing focus */
         prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
         WinSendMsg(prec->hwndScriptToolBar, UCMENU_ACTIVECHG, mp1, MPVOID);
         WinSendMsg(prec->hwndScriptCmdBar, UCMENU_ACTIVECHG, mp1, MPVOID);
         break;

      case WM_WINDOWPOSCHANGED:
         prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
         HandleWindowSizeChange(prec, hwnd, mp1, mp2);
         result = WinDefDlgProc(hwnd, msg, mp1, mp2);
         return result;
         break;

      case WM_PAINT: {
         SWP Pos, DlgPos;
         HPS Hps;
         RECTL Rect;

         /* First let dialog do normal painting */
         WinDefDlgProc(hwnd, msg, mp1, mp2);

         /* Paint recessed frame around the status line */
         Hps = WinGetPS(hwnd);
         WinQueryWindowPos(WinWindowFromID(hwnd, IDD_SCRED_STATUSTEXT), &Pos);
         WinQueryWindowPos(hwnd, &DlgPos);
         Rect.xLeft = WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME);
         Rect.xRight= DlgPos.cx - Rect.xLeft;
         Rect.yBottom = Pos.y - 1;
         Rect.yTop    = Pos.y + Pos.cy + 1;
         #define DB_RAISED    0x0400  // Undocumented borders
         #define DB_DEPRESSED 0x0800
         WinDrawBorder(Hps, &Rect, 1, 1, 0, 0, DB_DEPRESSED);
         WinReleasePS(Hps);
         return 0;
         }

      case WM_CONTROL:
         switch (SHORT1FROMMP(mp1))
         {  /* ID of window sending msg */
            case ID_SCRIPTTOOLBAR:
            case ID_SCRIPTCMDBAR:
               return ProcessUCMenuControlMsg(hwnd, mp1, mp2);
         }
         break;

      case WM_PROCESS_SCRIPT_CMD: {
        USHORT CmdIndex;
        USHORT Attr;
        /* A menu command needs to be processed, either from a toolbar  */
        /* or the normal PM text menu.  The index of the command in the */
        /* ItemList[] array is in mp1.  This is where a lot of          */
        /* application specific code would go if this were a real       */
        /* drawing program.                                             */
        CmdIndex = SHORT1FROMMP(mp1);

        /* Now process the selected command. */

        switch (ScriptItemList[CmdIndex].CommandID) {
          case IDD_SCRED_NEW:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_NEW), NULL);
             break;
          case IDD_SCRED_OPEN:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_OPEN), NULL);
             break;
          case IDD_SCRED_SAVE:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_SAVE), NULL);
             break;
          case IDD_SCRED_SAVEAS:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_SAVEAS), NULL);
             break;
          case IDD_SCRED_COMPILE:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_COMPILE), NULL);
             break;
          case IDD_SCRED_PLAY:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_PLAY), NULL);
             break;
          case IDD_SCRED_RECORD:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_RECORD), NULL);
             break;
          case IDD_SCRED_STOP:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_STOP), NULL);
             break;
          case IDD_SCRED_CLOSE:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_CLOSE), NULL);
            break;
          case IDD_SCRED_CUT:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_CUT), NULL);
            break;
          case IDD_SCRED_COPY:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_COPY), NULL);
            break;
          case IDD_SCRED_PASTE:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_PASTE), NULL);
            break;
          case IDD_SCRED_UNDO:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_UNDO), NULL);
            break;
          case IDD_SCRED_FONTS:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_FONTS), NULL);
            break;
          case IDD_SCRED_BOLD:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_BOLD), NULL);
            break;
          case IDD_SCRED_ITALIC:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_ITALIC), NULL);
            break;
          case IDD_SCRED_UNDERLINE:
             WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_UNDERLINE), NULL);
            break;
        }
        return 0;
      } /* end of WM_PROCESS_SCRIPT_CMD */

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1)){
            case IDD_SCRED_NEW:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               PromptUser(prec);
               ClearScript(prec);
               break;
            case IDD_SCRED_OPEN:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               PromptUser(prec);
               OpenScript(prec);
               strcpy(szTemp, prec->szScriptEditor);
               WinSetWindowText(prec->hwnd, strcat(szTemp,prec->szScriptFileName));
               break;
            case IDD_SCRED_SAVE:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               if(prec->szScriptFileName[0] != NULL)
                 SaveScript(prec);
               else
               {
                 SaveAsScript(prec);
                 strcpy(szTemp, prec->szScriptEditor);
                 WinSetWindowText(prec->hwnd, strcat(szTemp,prec->szScriptFileName));
               }
               break;
            case IDD_SCRED_SAVEAS:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               SaveAsScript(prec);
               strcpy(szTemp, prec->szScriptEditor);
               WinSetWindowText(prec->hwnd, strcat(szTemp,prec->szScriptFileName));
               break;
            case IDD_SCRED_COMPILE:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               ClearStatusText(prec);
               if(prec->bRecording)
               {
                 AddStatusText(prec, (PCHAR) ERR_RECORDING_NOCOMPILE,
                               (Size) strlen(ERR_RECORDING_NOCOMPILE));
               }
               else
               {
                 hwndButton = WinWindowFromID(hwnd, IDD_SCRED_STATUSTEXT);
                 WinSetWindowText(hwndButton, "Checking Syntax...");
                 WinSendMsg(hwnd, WM_DISABLE_DIALOG, NULL, NULL);
                 prec->ScriptCmd = SCR_COMPILE;
                 _beginthread( ScreditThread, NULL, LEN_STACK, prec );
               }
               break;
            case IDD_SCRED_PLAY:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               ClearStatusText(prec);
               if(prec->bRecording)
               {
                 AddStatusText(prec, (PCHAR) ERR_RECORDING_NOPLAY,
                               (Size) strlen(ERR_RECORDING_NOPLAY));
               }
               else
               {
                 hwndButton = WinWindowFromID(hwnd, IDD_SCRED_STATUSTEXT);
                 WinSetWindowText(hwndButton, "Playing...");
                 WinSendMsg(hwnd, WM_DISABLE_DIALOG, NULL, NULL);
                 prec->ScriptCmd = SCR_PLAY;
                 _beginthread( ScreditThread, NULL, LEN_STACK, prec );
               }
               break;
            case IDD_SCRED_RECORD:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               ClearStatusText(prec);
               if(prec->bRecording)
               {
                 AddStatusText(prec, (PCHAR) ERR_RECORDING_IN_PROCESS,
                               (Size) strlen(ERR_RECORDING_IN_PROCESS));
               }
               else
               {
                 WinSendMsg(hwnd, WM_DISABLE_DIALOG, NULL, NULL);
                 prec->ScriptCmd = SCR_RECORD;
                 _beginthread( ScreditThread, NULL, LEN_STACK, prec );
               }
               break;
            case IDD_SCRED_STOP:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               ev = somGetGlobalEnvironment();
               ClearStatusText(prec);
               WinSendMsg(hwnd, WM_DISABLE_DIALOG, NULL, NULL);
               prec->ScriptCmd = SCR_STOP;
               _beginthread( ScreditThread, NULL, LEN_STACK, prec );
               break;
            case IDD_SCRED_CUT:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               ClipboardCut(prec);
               break;
            case IDD_SCRED_COPY:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               ClipboardCopy(prec);
               break;
            case IDD_SCRED_PASTE:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               ClipboardPaste(prec);
               break;
            case IDD_SCRED_UNDO:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               Undo(prec);
               break;
            case IDD_SCRED_FONTS:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               SetFont(prec);
               break;
            case IDD_SCRED_BOLD:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               FontBold(prec);
               break;
            case IDD_SCRED_ITALIC:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               FontItalic(prec);
               break;
            case IDD_SCRED_UNDERLINE:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               FontUnderline(prec);
               break;
            case IDD_SCRED_CLOSE:
               prec = (PSCRED_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               PromptUser(prec);
               // Save window size/pos info
               WinQueryWindowPos(hwnd, &swp);
               prec->pmp->Profile.scredit_cx = swp.cx;
               prec->pmp->Profile.scredit_cy = swp.cy;
               prec->pmp->Profile.scredit_x = swp.x;
               prec->pmp->Profile.scredit_y = swp.y;
               WinQueryWindowPos(WinWindowFromID(hwnd, IDD_SCRED_SCRIPT), &swp);
               prec->pmp->Profile.screditScr_cx = swp.cx;
               prec->pmp->Profile.screditScr_cy = swp.cy;
               prec->pmp->Profile.screditScr_x = swp.x;
               prec->pmp->Profile.screditScr_y = swp.y;
               WinQueryWindowPos(WinWindowFromID(hwnd, IDD_SCRED_STATUS), &swp);
               prec->pmp->Profile.screditStat_cx = swp.cx;
               prec->pmp->Profile.screditStat_cy = swp.cy;
               prec->pmp->Profile.screditStat_x = swp.x;
               prec->pmp->Profile.screditStat_y = swp.y;
               WinQueryWindowPos(WinWindowFromID(hwnd, IDD_SCRED_STATUSTEXT), &swp);
               prec->pmp->Profile.screditStatTxt_cx = swp.cx;
               prec->pmp->Profile.screditStatTxt_cy = swp.cy;
               prec->pmp->Profile.screditStatTxt_x = swp.x;
               prec->pmp->Profile.screditStatTxt_y = swp.y;
               prec->pmp->hwndScriptEditor = NULL;
               if(prec->szScriptFileName[0] != NULL)
                 strcpy(prec->pmp->Profile.scredit_path,prec->szScriptFileName);
               /* add all the script cleanup stuff as needed */
               ev = somGetGlobalEnvironment();
               prec->cmgr->CloseComponent(ev,prec->gsc);
               if(prec->hwndListAppsDlg)
                 WinSendMsg(prec->hwndListAppsDlg, WM_CLOSE, NULL, NULL);
               delete prec->cmgr;
               free((PVOID)prec);
               WinDismissDlg(hwnd, TRUE);
               break;
            default:
               ;
         } /* endswitch */
         break;

      case WM_CLOSE:
         WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_SCRED_CLOSE), NULL);
         break;
      default:
         return(WinDefDlgProc(hwnd, msg, mp1, mp2));
   }
   return (MRESULT)NULL;
}   /*  end of OSAScriptEditorDlgProc()  */

void OpenScript(PSCRED_PARM prec)
{

 FILEDLG fild;                          /* File dialog info structure  */
 char pszTitle[10] = "Open File";       /* Title of dialog             */
 char pszFullFile[CCHMAXPATH] = "*.scr";/* File filter string          */
 HWND hwndDlg;                          /* File dialog window          */
 HFILE hf;
 HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);
 CHAR  szMleBuf[512];
 IPT   lOffset = 0;
 ULONG cbCopied;
 ULONG ulAction;
 ULONG cbBytesRead;

 /*****************************************************************/
 /* Initially set all fields to 0                                 */
 /*****************************************************************/

 memset(&fild, 0, sizeof(FILEDLG));

 /*****************************************************************/
 /* Initialize those fields in the FILEDLG structure that are     */
 /* used by the application                                       */
 /*****************************************************************/
 fild.cbSize = sizeof(FILEDLG);       /* Size of structure        */
 fild.fl = FDS_HELPBUTTON | FDS_CENTER | FDS_OPEN_DIALOG      ;
                                      /* FDS_* flags              */
 fild.pszTitle = pszTitle;         /* Dialog title string         */
                                   /* init path,file or mask      */
 if(prec->szScriptFileName[0] != NULL)
   strcpy(fild.szFullFile, prec->szScriptFileName);
 else if(prec->pmp->Profile.scredit_path[0] != NULL)
   strcpy(fild.szFullFile, prec->pmp->Profile.scredit_path);
 else
   strcpy(fild.szFullFile, pszFullFile);
                                                                                                                                     /* file filter       */

 /*****************************************************************/
 /* Display the dialog and get the file                           */
 /*****************************************************************/

 hwndDlg = WinFileDlg(HWND_DESKTOP, prec->hwnd, &fild);

 if ((hwndDlg && (fild.lReturn == DID_OK)))
 {
   /**************************************************************/
   /* Upon successful return of a file, open it for reading and  */
   /* further processing                                         */
   /**************************************************************/


   /* if we already have a file loaded */
   /*   then clear the MLE             */
   if(prec->szScriptFileName[0] != NULL)
   {
     /*
      * disable redrawing of the MLE so the text doesn't "flash" when
      * the MLE is cleared
      */
     WinSendMsg(hwndMle, MLM_DISABLEREFRESH, NULL, NULL);

     /*
      * Clear the MLE by selecting all of the text and clearing it
      */
     WinSendMsg(hwndMle, MLM_SETSEL, MPFROMSHORT(NULL),
               (MPARAM)WinSendMsg(hwndMle, MLM_QUERYTEXTLENGTH, NULL, NULL));

     WinSendMsg(hwndMle, MLM_CLEAR, NULL, NULL);

     /*
      * Reset the changed flag
      */
     WinSendMsg(hwndMle, MLM_SETCHANGED, MPFROMSHORT((BOOL)FALSE), NULL);

     /*
      * Enable redrawing of the MLE
      */
     WinSendMsg(hwndMle, MLM_ENABLEREFRESH, NULL, NULL);
   }

   /* Save the file name */
   strcpy(prec->szScriptFileName,fild.szFullFile);

   /* Open the file */

   DosOpen(fild.szFullFile,
          &hf,
          &ulAction,
          0,
          FILE_NORMAL,
          FILE_OPEN |
          FILE_CREATE,
          OPEN_ACCESS_READONLY |
          OPEN_SHARE_DENYNONE,
          NULL);

   /* Zero-fill the buffer using memset, a C run-time function */
   memset(szMleBuf, 0, sizeof(szMleBuf));

   /* Set the MLE import-export buffer */
   hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);
   WinSendMsg(hwndMle,
              MLM_SETIMPORTEXPORT,
              MPFROMP(szMleBuf),
              MPFROMSHORT((USHORT) sizeof(szMleBuf)));

   /**********************************************************************/
   /*  Read the text from the file to the buffer,                        */
   /*  then import it to the MLE.                                        */
   /**********************************************************************/

   do {
        DosRead(hf,
                szMleBuf,
                sizeof(szMleBuf),
                &cbBytesRead);

        if(cbBytesRead)
          cbCopied = (ULONG) WinSendMsg(hwndMle,
                                        MLM_IMPORT,
                                        MPFROMP( &lOffset),
                                        MPFROMLONG(cbBytesRead));
      } while (cbBytesRead);

   /*
    * Reset the changed flag
    */
   WinSendMsg(hwndMle, MLM_SETCHANGED, MPFROMSHORT((BOOL)FALSE), NULL);

   /* Close the file */
   DosClose(hf);
 } /* endif */

}

void SaveScript(PSCRED_PARM prec)
{

  HWND  hwndMle;
  CHAR  szMleBuf[512];
  IPT   lOffset = 0;
  HFILE hf;

  ULONG cbCopied;
  ULONG ulAction;
  ULONG cbBytesWritten;
  ULONG cbCopy;

  /* Zero-fill the buffer using memset, a C run-time function */
  memset(szMleBuf, 0, sizeof(szMleBuf));


 /* get MLE handle */
  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);


  /* Set the MLE import-export buffer */
  WinSendMsg(hwndMle,
             MLM_SETIMPORTEXPORT,
             MPFROMP(szMleBuf),
             MPFROMSHORT ((USHORT) sizeof(szMleBuf)));

  /* Open the file */
  DosOpen(prec->szScriptFileName,
          &hf,
          &ulAction,
          0,
          FILE_NORMAL,
          OPEN_ACTION_CREATE_IF_NEW |
          OPEN_ACTION_REPLACE_IF_EXISTS,
          OPEN_ACCESS_WRITEONLY |
          OPEN_SHARE_DENYNONE,
          NULL);

  /* Find out how much text is in the MLE */
  cbCopy = (ULONG) WinSendMsg(hwndMle,
                              MLM_QUERYFORMATTEXTLENGTH,
                              MPFROMLONG(lOffset),
                              MPFROMLONG((-1)));

  do{

    /* Copy the MLE text to the buffer */
    cbCopied = (ULONG) WinSendMsg(hwndMle,
                                MLM_EXPORT,
                                MPFROMP(&lOffset),
                                MPFROMP(&cbCopy));

    /* Write the contents of the buffer to the file */
    DosWrite(hf,
             szMleBuf,
             cbCopied,
             &cbBytesWritten);

  } while (cbCopy); /* enddo */


  /* Close the file */
  DosClose(hf);


}


void SaveAsScript(PSCRED_PARM prec)
{
  FILEDLG fild;                           /* File dialog info structure  */
  char  pszTitle[15] = "Save As File";       /* Title of dialog          */
  char  pszFullFile[CCHMAXPATH] = "*.scr";/* File filter string          */
  HWND  hwndDlg;                          /* File dialog window          */
  HWND  hwndMle;
  CHAR  szMleBuf[512];
  IPT   lOffset = 0;
  HFILE hf;

  ULONG cbCopied;
  ULONG ulAction;
  ULONG cbBytesWritten;
  ULONG cbCopy;

  /* Zero-fill the buffer using memset, a C run-time function */
  memset(szMleBuf, 0, sizeof(szMleBuf));


  /* get MLE handle */
  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);


  /* Set the MLE import-export buffer */
  WinSendMsg(hwndMle,
             MLM_SETIMPORTEXPORT,
             MPFROMP(szMleBuf),
             MPFROMSHORT ((USHORT) sizeof(szMleBuf)));

  /* Get file name from user */

  /*****************************************************************/
  /* Initially set all fields to 0                                 */
  /*****************************************************************/

  memset(&fild, 0, sizeof(FILEDLG));

  /*****************************************************************/
  /* Initialize those fields in the FILEDLG structure that are     */
  /* used by the application                                       */
  /*****************************************************************/
  fild.cbSize = sizeof(FILEDLG);       /* Size of structure        */
  fild.fl = FDS_HELPBUTTON | FDS_CENTER | FDS_SAVEAS_DIALOG      ;
                                       /* FDS_* flags              */
  fild.pszTitle = pszTitle;         /* Dialog title string         */
  if(prec->szScriptFileName[0] != NULL)
    strcpy(fild.szFullFile, prec->szScriptFileName);
  else
    strcpy(fild.szFullFile, pszFullFile);       /* Initial path,   */
                                                /* file name, or   */
                                                                                                                                      /* file filter       */

  /*****************************************************************/
  /* Display the dialog and get the file                           */
  /*****************************************************************/

  hwndDlg = WinFileDlg(HWND_DESKTOP, prec->hwnd, &fild);

  if ((hwndDlg && (fild.lReturn == DID_OK)))
  {
    /**************************************************************/
    /* Upon successful return of a file, open it for reading and  */
    /* further processing                                         */
    /**************************************************************/

    /* Save the file name */
    strcpy(prec->szScriptFileName,fild.szFullFile);

    /* Open the file */
    DosOpen(prec->szScriptFileName,
            &hf,
            &ulAction,
            0,
            FILE_NORMAL,
            OPEN_ACTION_CREATE_IF_NEW |
            OPEN_ACTION_REPLACE_IF_EXISTS,
            OPEN_ACCESS_WRITEONLY |
            OPEN_SHARE_DENYNONE,
            NULL);

    /* Find out how much text is in the MLE */
    cbCopy = (ULONG) WinSendMsg(hwndMle,
                                MLM_QUERYFORMATTEXTLENGTH,
                                MPFROMLONG(lOffset),
                                MPFROMLONG((-1)));

    do{
      /* Copy the MLE text to the buffer */
      cbCopied = (ULONG) WinSendMsg(hwndMle,
                                  MLM_EXPORT,
                                  MPFROMP(&lOffset),
                                  MPFROMP(&cbCopy));

      /* Write the contents of the buffer to the file */
      DosWrite(hf,
             szMleBuf,
             cbCopied,
             &cbBytesWritten);

    } while (cbCopy); /* enddo */

    /* Close the file */
    DosClose(hf);
  }

}

void StartRecordingScript(PSCRED_PARM prec)
{
   Environment    *ev     = somGetGlobalEnvironment();
   OSAError retVal = noErr;

   retVal = prec->gsc->OSAStartRecording(ev, &(prec->scriptID));

   if(retVal != noErr)
     DisplayScriptError(prec);
   else
   {
     WinEnableWindow(WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT), FALSE);
     WinEnableWindow(WinWindowFromID(prec->hwnd, ID_SCRIPTTOOLBAR), FALSE);
     prec->bRecording = TRUE;
     WinSetWindowText(WinWindowFromID(prec->hwnd, IDD_SCRED_STATUSTEXT),
                      "Recording...");
     prec->ulTimer = 0;
     WinStartTimer(prec->pmp->hab, prec->hwnd, ID_RECORDTIMER, 750);
   }
}

void StopRecordingScript(PSCRED_PARM prec)
{
   Environment    *ev     = somGetGlobalEnvironment();
   OSAError retVal = noErr;

   retVal = prec->gsc->OSAStopRecording(ev, prec->scriptID);
   if(retVal != noErr)
     DisplayScriptError(prec);

   WinEnableWindow(WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT), TRUE);
   WinEnableWindow(WinWindowFromID(prec->hwnd, ID_SCRIPTTOOLBAR), TRUE);
   WinStopTimer(prec->pmp->hab, prec->hwnd, ID_RECORDTIMER);
   prec->bRecording = FALSE;

}

void PlayScript(PSCRED_PARM prec)
{
   Environment    *ev     = somGetGlobalEnvironment();
   OSAError        retVal = noErr;
   OSAID           resultID;
   OSAID           cntxtID  = kOSANullScript;
   HWND            hwndMLE = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);

   /* If the script has been changed or not yet compiled, */
   /*   then we need to recompile                         */
   if( (WinSendMsg( hwndMLE, MLM_QUERYCHANGED, NULL,NULL) )   ||
       !(prec->scriptID) )
   {
      /*
       * Reset the changed flag in the MLE
       */
      WinSendMsg(hwndMLE, MLM_SETCHANGED, MPFROMSHORT((BOOL)FALSE), NULL);
      /* BUILD NEW SCRIPT DESC */
      BuildScriptDesc(prec);
      /* COMPILE SCRIPT */
      CompileScript(prec);
   }

   /* If the script has been compiled successfully */
   /*   then play it                               */
   if(prec->scriptID)
   {
     /* PLAY SCRIPT */
     prec->ulTimer = 0;
     WinStartTimer(prec->pmp->hab, prec->hwnd, ID_PLAYTIMER, 750);
     retVal = prec->gsc->OSAExecute(ev, prec->scriptID,cntxtID,0,&resultID);
     if(retVal != noErr)
       DisplayScriptError(prec);
     else
       AddStatusText(prec, (PCHAR) PLAY_OK,
                   (Size) strlen(PLAY_OK));
     WinStopTimer(prec->pmp->hab, prec->hwnd, ID_PLAYTIMER);
   }
}

void CompileScript(PSCRED_PARM prec)
{
   Environment    *ev     = somGetGlobalEnvironment();
   OSAError        retVal = noErr;
   OSAID           scriptID = kOSANullScript;
   HWND            hwndMLE = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);

   /* If the script has been changed, */
   /*   then we need to rebuild the descriptor containing the script */
   if( (WinSendMsg( hwndMLE, MLM_QUERYCHANGED, NULL,NULL) )   ||
       !(prec->scriptID) )
   {
      /*
       * Reset the changed flag in the MLE
       */
      WinSendMsg(hwndMLE, MLM_SETCHANGED, MPFROMSHORT((BOOL)FALSE), NULL);
      BuildScriptDesc(prec);
   }

   /* If we already had a script ID */
   /*   then dispose of it          */
   if(prec->scriptID)
   {
     prec->gsc->OSADispose(ev,prec->scriptID);
     prec->scriptID = kOSANullScript;
   }

   prec->ulTimer = 0;
   WinStartTimer(prec->pmp->hab, prec->hwnd, ID_COMPILETIMER, 750);
   /* COMPILE SCRIPT */
   retVal = prec->gsc->OSACompile(ev, &(prec->scriptDesc), 0, &scriptID);

   if (retVal == noErr)
   {
      /* save script ID */
      prec->scriptID = scriptID;
      AddStatusText(prec, (PCHAR) COMPILE_OK,
                   (Size) strlen(COMPILE_OK));
   }
   else
   {
      prec->scriptID = kOSANullScript;
      DisplayScriptError(prec);
      /* display error from scripting component */
   }

   WinStopTimer(prec->pmp->hab, prec->hwnd, ID_COMPILETIMER);
}

void BuildScriptDesc(PSCRED_PARM prec)
{
   HWND  hwndMle;
   CHAR  szMleBuf[512] = "";
   IPT   lOffset = 0;
   ULONG cbCopied;
   ULONG ulAction;
   ULONG cbBytesWritten;
   ULONG cbCopy;
   PCHAR pBuf,pTemp;
   Size  size;

   AEDisposeDesc(&(prec->scriptDesc));

   /* copy data out of MLE and into our buffer */
   /* Zero-fill the buffer using memset, a C run-time function */
   memset(szMleBuf, 0, sizeof(szMleBuf));

   /* get MLE handle */
   hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);

   /* Set the MLE import-export buffer */
   WinSendMsg(hwndMle,
             MLM_SETIMPORTEXPORT,
             MPFROMP(szMleBuf),
             MPFROMSHORT ((USHORT) sizeof(szMleBuf)));

   /* Find out how much text is in the MLE */
   cbCopy = (ULONG) WinSendMsg(hwndMle,
                              MLM_QUERYFORMATTEXTLENGTH,
                              MPFROMLONG(lOffset),
                              MPFROMLONG((-1)));
   size = (Size) cbCopy;

   /* allocate a buffer to hold it all */
   pBuf = (PCHAR) malloc(cbCopy);
   pTemp = pBuf;

   do{

     /* Copy the MLE text to the buffer */
     cbCopied = (ULONG) WinSendMsg(hwndMle,
                                MLM_EXPORT,
                                MPFROMP(&lOffset),
                                MPFROMP(&cbCopy));

     /* Add to buffer for AEDesc */
     memcpy(pTemp,szMleBuf,cbCopied);
     pTemp = pTemp + cbCopied;

   } while (cbCopy); /* enddo */

   // ensure we have at least one byte in the buffer
   if(!size)
     size++;

   AECreateDesc(typeChar, pBuf, (Size) size, &(prec->scriptDesc));
}

void AddRecordedText(PSCRED_PARM prec, PCHAR pch, Size size)
{

   HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);
   PCHAR  pszMleBuf = NULL;
   IPT   lOffset = 0;
   ULONG cbCopied;
   ULONG ulAction;
   ULONG cbLength;

   pszMleBuf = (PCHAR) malloc(size);

   /* Zero-fill the buffer using memset, a C run-time function */
   memset(pszMleBuf, 0, size);

   /* Set the MLE import-export buffer */
   WinSendMsg(hwndMle,
              MLM_SETIMPORTEXPORT,
              MPFROMP(pszMleBuf),
              MPFROMSHORT((USHORT) size));

   /* Find out how much text is in the MLE */
   cbLength = (ULONG) WinSendMsg(hwndMle,
                                MLM_QUERYFORMATTEXTLENGTH,
                                MPFROMLONG(lOffset),
                                MPFROMLONG((-1)));

   lOffset = cbLength + 1; /* insert text at end */
   memcpy(pszMleBuf, pch, size);
   cbCopied = (ULONG) WinSendMsg(hwndMle,
                                 MLM_IMPORT,
                                 MPFROMP( &lOffset),
                                 MPFROMLONG(size));

}

void ClearScript(PSCRED_PARM prec)
{

 CHAR  szTemp[CCHMAXPATH];
 HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);

 /*
  * disable redrawing of the MLE so the text doesn't "flash" when
  * the MLE is cleared
  */
 WinSendMsg(hwndMle, MLM_DISABLEREFRESH, NULL, NULL);

 /*
  * Clear the MLE by selecting all of the text and clearing it
  */
 WinSendMsg(hwndMle, MLM_SETSEL, MPFROMSHORT(NULL),
           (MPARAM)WinSendMsg(hwndMle, MLM_QUERYTEXTLENGTH, NULL, NULL));

 WinSendMsg(hwndMle, MLM_CLEAR, NULL, NULL);

 /*
  * Reset the changed flag
  */
 WinSendMsg(hwndMle, MLM_SETCHANGED, MPFROMSHORT((BOOL)FALSE), NULL);

 /*
  * Enable redrawing of the MLE
  */
 WinSendMsg(hwndMle, MLM_ENABLEREFRESH, NULL, NULL);

 strcpy(szTemp, prec->szScriptEditor);
 WinSetWindowText(prec->hwnd, strcat(szTemp,"<Untitled>"));
}

void ClipboardCut(PSCRED_PARM prec)
{
 HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);
 WinSendMsg(hwndMle, MLM_CUT, NULL, NULL);
}

void ClipboardCopy(PSCRED_PARM prec)
{
 HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);
 WinSendMsg(hwndMle, MLM_COPY, NULL, NULL);
}

void ClipboardPaste(PSCRED_PARM prec)
{
 HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);
 WinSendMsg(hwndMle, MLM_PASTE, NULL, NULL);
}

void Undo(PSCRED_PARM prec)
{
 HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);
 WinSendMsg(hwndMle, MLM_UNDO, NULL, NULL);
}

void SetFont(PSCRED_PARM prec)
{
 FATTRS fat;
 HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);

 FONTDLG pfdFontdlg;      /* Font dialog info structure         */
 HWND    hwndFontDlg;     /* Font dialog window                 */

 char szFamilyname[FACESIZE];

 /***************************************************************/
 /* Initially set all fields to 0                               */
 /***************************************************************/
 memset(&pfdFontdlg, 0, sizeof(FONTDLG));

 /***************************************************************/
 /* Initialize those fields in the FONTDLG structure that are   */
 /* used by the application                                     */
 /***************************************************************/
 pfdFontdlg.cbSize = sizeof(FONTDLG);  /* Size of structure     */
 pfdFontdlg.hpsScreen = WinGetScreenPS(HWND_DESKTOP);  /* Screen presentation   */
                                       /* space                 */
 szFamilyname[0] = 0;        /* Use default font      */
 pfdFontdlg.pszFamilyname  = szFamilyname; /* Provide buffer    */
 pfdFontdlg.usFamilyBufLen = sizeof(szFamilyname); /* for font  */
                                       /* family name           */
 pfdFontdlg.fxPointSize = MAKEFIXED(10,0); /* Font point size   */
 pfdFontdlg.fl = FNTS_HELPBUTTON | FNTS_CENTER; /* FNTS_* flags */
 pfdFontdlg.clrFore = CLR_BLACK;       /* Foreground color      */
 pfdFontdlg.clrBack = CLR_WHITE;       /* Background color      */
 pfdFontdlg.fAttrs.usCodePage = 437;   /* Code page to select   */
                                       /* from                  */

 /***************************************************************/
 /* Display the font dialog and get the font                    */
 /***************************************************************/
 hwndFontDlg = WinFontDlg(HWND_DESKTOP, prec->hwnd, &pfdFontdlg);

 if (hwndFontDlg && (pfdFontdlg.lReturn == DID_OK))
 {
   WinSendMsg(hwndMle, MLM_SETFONT, (MPARAM) &(pfdFontdlg.fAttrs), (MPARAM) 0);
 }
}
void FontBold(PSCRED_PARM prec)
{
 FATTRS fat;
 HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);

 fat.usRecordLength = sizeof(FATTRS);
 WinSendMsg(hwndMle, MLM_QUERYFONT, (MPARAM) &fat, (MPARAM) 0L);
 fat.fsSelection = FATTR_SEL_BOLD;
 WinSendMsg(hwndMle, MLM_SETFONT, (MPARAM) &fat, (MPARAM) 0);
}

void FontItalic(PSCRED_PARM prec)
{
 FATTRS fat;
 HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);

 fat.usRecordLength = sizeof(FATTRS);
 WinSendMsg(hwndMle, MLM_QUERYFONT, (MPARAM) &fat, (MPARAM) 0L);
 fat.fsSelection = FATTR_SEL_ITALIC;
 WinSendMsg(hwndMle, MLM_SETFONT, (MPARAM) &fat, (MPARAM) 0);
}

void FontUnderline(PSCRED_PARM prec)
{
 FATTRS fat;
 HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_SCRIPT);

 fat.usRecordLength = sizeof(FATTRS);
 WinSendMsg(hwndMle, MLM_QUERYFONT, (MPARAM) &fat, (MPARAM) 0L);
 fat.fsSelection = FATTR_SEL_UNDERSCORE;
 WinSendMsg(hwndMle, MLM_SETFONT, (MPARAM) &fat, (MPARAM) 0);
}


/*----------------------------------------------------------------------------*/
MRESULT ProcessUCMenuControlMsg(HWND hwnd, MPARAM mp1, MPARAM mp2)
/*----------------------------------------------------------------------------*/
/* This routine processes all WM_CONTROL messages from all UCMenu toolbar     */
/* controls.  The return value will be the message result.                    */
/*----------------------------------------------------------------------------*/
{
USHORT   MenuID;        /* Window ID of UCMenu           */
UCMITEM  *UCItem;       /* Ptr to UCMenu item data       */
MENUITEM ItemData;      /* Std PM menu item data         */
int      CmdIndex;      /* Index into ActionList[] array */
char     Msg[100];      /* Message formatting buffer     */
int      i;

  MenuID = SHORT1FROMMP(mp1);  /* ID of UCMenu sending this message */

  switch (SHORT2FROMMP(mp1)) { /* Process each notification code */

  case UCN_ITEMSELECTED: /* User made a selection on a toolbar */

    UCItem = (UCMITEM *)mp2;
    if (UCItem->pszAction != NULL) { /* Make sure there is an action string */

      /* See if we recognize this action.  If so, send ourselves */
      /* a message to process the command index.                 */

      CmdIndex = FindActionInScriptItemList(UCItem->pszAction);
      if (CmdIndex >= 0)
        WinSendMsg(hwnd, WM_PROCESS_SCRIPT_CMD, MPFROMSHORT(CmdIndex), 0L);
    }
    return 0;

  case UCN_QRYDEFTEMPLATEID:
    /* User has asked to reload the default menu for one of the toolbars. */
    /* Tell the UC menu control the resource ID of the menu template,     */
    /* which is the same as the UCMenu window ID set during UCMenu create.*/
    *(USHORT *)mp2 = MenuID;
    return (MRESULT)TRUE;

  case UCN_QRYTEMPLATEMODULE:
    /* Menu needs to know module handle from which to load resources. */
    *(HMODULE *)mp2 = NULLHANDLE;      /* Resources are bound to EXE. */
    return (MRESULT)TRUE;

  case UCN_QRYBUBBLEHELP:{
     /* Toolbar needs to know what bubble help text to display. */
     /* We keep bubble help in the pszData field of the UCMITEM */
     /* structure, passed to us via mp2.  Note we must return   */
     /* a COPY which UCMenus will free when it is done with it. */
     QBUBBLEDATA *BubbleInfo;
     int        Index;

     BubbleInfo = (QBUBBLEDATA *)mp2;

     if (UCMFIELD(BubbleInfo->MenuItem, pszAction) != NULL) {
       Index = FindActionInScriptItemList(UCMFIELD(BubbleInfo->MenuItem, pszAction));
       if (Index >= 0)
         BubbleInfo->BubbleText = UCMenuStrdup(ScriptItemList[Index].Desc);
     }
     return 0;
     }

  } /* switch on notification code */

  return 0;  /* Did not process notifcation */

}

/*----------------------------------------------------------------------------*/
int FindActionInScriptItemList(PSZ Action)
/*----------------------------------------------------------------------------*/
/* Look through the list of all the 'actions' this application supports and   */
/* return the index into the ItemList array of the matching entry.  If the    */
/* specified action is not supported, return -1.                              */
/*----------------------------------------------------------------------------*/
{
int i;
  if (Action == NULL)  /* Check for nonexistant action string */
    return -1;

  for (i=0; ScriptItemList[i].CommandID != 0; i++) {
    if (STRSAME(ScriptItemList[i].Action, Action)) /* We found the action in the list */
      return i;
  } /* for each item in list */

  return -1;  /* Did not find the action in the list */
}


void ClearStatusText(PSCRED_PARM prec)
{
   HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_STATUS);

   /*
    * disable redrawing of the MLE so the text doesn't "flash" when
    * the MLE is cleared
    */
   WinSendMsg(hwndMle, MLM_DISABLEREFRESH, NULL, NULL);

   /*
    * Clear the MLE by selecting all of the text and clearing it
    */
   WinSendMsg(hwndMle, MLM_SETSEL, MPFROMSHORT(NULL),
             (MPARAM)WinSendMsg(hwndMle, MLM_QUERYTEXTLENGTH, NULL, NULL));

   WinSendMsg(hwndMle, MLM_CLEAR, NULL, NULL);

   /*
    * Enable redrawing of the MLE
    */
   WinSendMsg(hwndMle, MLM_ENABLEREFRESH, NULL, NULL);
}

void AddStatusText(PSCRED_PARM prec, PCHAR pch, Size size)
{

   HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_SCRED_STATUS);
   PCHAR  pszMleBuf = NULL;
   IPT   lOffset = 0;
   ULONG cbCopied;
   ULONG ulAction;
   ULONG cbLength;

   ClearStatusText(prec);

   pszMleBuf = (PCHAR) malloc(size);

   /* Zero-fill the buffer using memset, a C run-time function */
   memset(pszMleBuf, 0, size);

   /* Set the MLE import-export buffer */
   WinSendMsg(hwndMle,
              MLM_SETIMPORTEXPORT,
              MPFROMP(pszMleBuf),
              MPFROMSHORT((USHORT) size));

   /* Find out how much text is in the MLE */
   cbLength = (ULONG) WinSendMsg(hwndMle,
                                MLM_QUERYFORMATTEXTLENGTH,
                                MPFROMLONG(lOffset),
                                MPFROMLONG((-1)));

   lOffset = cbLength + 1; /* insert text at end */
   memcpy(pszMleBuf, pch, size);
   cbCopied = (ULONG) WinSendMsg(hwndMle,
                                 MLM_IMPORT,
                                 MPFROMP( &lOffset),
                                 MPFROMLONG(size));

}

void DisplayScriptError(PSCRED_PARM prec)
{
   OSAError retVal = noErr;

   CHAR     szTemp[CCHMAXPATH];
   AEDesc   err_desc;
   PSZ      pszErrMsg = NULL;
   PSZ      pszTemp = NULL;
   ULONG    ulSize = 0;
   DescType err_type;
   USHORT   usMsg = 0;
   Environment  *ev = somGetGlobalEnvironment();

   err_type = typeShortInteger;

   retVal = prec->gsc->OSAScriptError(ev, kOSAErrorNumber, err_type, &err_desc);
   if (retVal == noErr)
   {
     retVal = AESizeOfDescData(&err_desc, (Size *) &ulSize);
     if (retVal == noErr)
     {
        pszTemp = (char *) malloc(ulSize);

        retVal = AEGetDescData(&err_desc, &err_type, pszTemp, (Size) ulSize , (Size *) &ulSize);
        if (retVal == noErr)
        {
           usMsg = *( (USHORT *) pszTemp);

           err_type = typeChar;

           retVal = prec->gsc->OSAScriptError(ev, kOSAErrorMessage, err_type, &err_desc);

           if (retVal == noErr)
           {
             retVal = AESizeOfDescData(&err_desc, (Size *) &ulSize);
             if (retVal == noErr)
             {
                pszErrMsg = (char *) malloc(ulSize);

                retVal = AEGetDescData(&err_desc, &err_type, pszErrMsg, (Size) ulSize , (Size *) &ulSize);
                if (retVal == noErr)
                {
                  AddStatusText(prec, pszErrMsg, ulSize);
                }
             }
           }
        }
     }
   }
}

void PromptUser(PSCRED_PARM prec)
{

}

void HandleWindowSizeChange(PSCRED_PARM prec, HWND hwnd, MPARAM mp1, MPARAM mp2)
{
   HWND            hwndScript,
                   hwndStatusText,
                   hwndStatus;
   SWP             swpScript, swpStatus, swpToolBar, swpCmdBar, swp;
   PSWP            pswpNew, pswpOld;
   LONG            deltaX, deltaY;

   pswpNew = (PSWP) mp1;
   pswpOld = pswpNew + 1;

   deltaY = pswpNew->cy - pswpOld->cy;
   deltaX = pswpNew->cx - pswpOld->cx;

   // this block of code ensures that we don't start sizing
   // until the dialog, toolbars,etc have completely initialized
   if(!deltaX && !deltaY)
     return;
   if(!prec)
     return;
   if( !(hwndScript = WinWindowFromID(hwnd, IDD_SCRED_SCRIPT)) )
     return;
   if( !(hwndStatus = WinWindowFromID(hwnd, IDD_SCRED_STATUS)) )
     return;
   if( !(hwndStatusText = WinWindowFromID(hwnd, IDD_SCRED_STATUSTEXT)) )
     return;
   if(prec && prec->hwndScriptToolBar && prec->hwndScriptCmdBar)
   {
     WinQueryWindowPos(prec->hwndScriptToolBar, &swpToolBar);
     WinQueryWindowPos(prec->hwndScriptCmdBar, &swpCmdBar);
     if((swpToolBar.cx == 0) || (swpCmdBar.cx == 0))
       return;
   }
   else
     return;


   // resize the status and script mle's
   WinQueryWindowPos(hwndScript, &swpScript);
   WinQueryWindowPos(hwndStatus, &swpStatus);
   WinSetWindowPos(hwndScript,
                   HWND_TOP,
                   0,
                   0,
                   swpScript.cx+(deltaX),
                   swpScript.cy+(deltaY),
                   SWP_SIZE );
   WinSetWindowPos(hwndStatus,
                   HWND_TOP,
                   0,
                   0,
                   swpStatus.cx+(deltaX),
                   swpStatus.cy,
                   SWP_SIZE );

   // adjust the positioning of the status windows
   if(deltaY)
   {
     WinSetWindowPos(hwndStatus,
                     HWND_TOP,
                     swpStatus.x,
                     swpStatus.y+(deltaY),
                     0,
                     0,
                     SWP_MOVE );
     WinQueryWindowPos(hwndStatusText, &swp);
     WinSetWindowPos(hwndStatusText,
                     HWND_TOP,
                     swp.x,
                     swp.y+(deltaY),
                     0,
                     0,
                     SWP_MOVE );
   }
}


void _Optlink ScreditThread( void *pv )
{
   HMQ hmq;
   HAB hab;
   PSCRED_PARM prec;

   /* thread initialization */
   hab = WinInitialize( 0 );
   hmq = WinCreateMsgQueue( hab, 0 );

   /* copy and convert pvoid parmeter to a pointer to the main parameter block */
   prec = (PSCRED_PARM) pv;

   switch(prec->ScriptCmd)
   {
      case SCR_COMPILE:
         CompileScript(prec);
         break;
      case SCR_PLAY:
         PlayScript(prec);
         break;
      case SCR_RECORD:
         StartRecordingScript(prec);
         break;
      case SCR_STOP:
         StopRecordingScript(prec);
         break;
   }


   WinPostMsg(prec->hwnd, WM_ACK_THREADCOMPLETE, NULL, NULL);
   WinDestroyMsgQueue( hmq );
   WinTerminate( hab );
   _endthread();
}


