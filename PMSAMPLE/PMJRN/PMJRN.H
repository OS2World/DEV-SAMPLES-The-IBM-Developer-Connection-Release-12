/*---------------------------------------------------------------------------+
| JOURNAL common include file
|
| Change History:
| ---------------
|
+---------------------------------------------------------------------------*/

#define INCL_BASE
#define INCL_GPI
#define INCL_WIN
#define INCL_AVIO

#include <os2.h>                        /* the PM header file           */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>                     /* C variable parm functions  */

#define ID_MAINWND       256

/* Menu Action Bar items */

#define ID_AB_JOURNALON  260
#define ID_SELECT        261
#define ID_DESELECT      262

#define ID_AB_TRACE     1291
#define ID_T_RECORD     1292
#define ID_T_PLAYBACK   1293
#define ID_T_VIOGET     1294
#define ID_T_TESTCASE   1295

#define ID_AB_EXIT       290

#define ID_AB_NEWS       295

#define ID_HELP          300

#define ID_JOURNAL_POINTER   310
#define IDD_LB           311


#define IDD_HELP        380





#define IDT_HELP        256         /* Help Resource Type */
#define IDT_AB_HELP       1
#define IDT_NEWS          2

#define VERSION         100         /* 1.00 */
#define DLLVERSION      000         /* 0.00 */


/******************************************************************************
*
* #### T E M P O R A R Y ####
*
* Help Manager message base.
*
******************************************************************************/

#if !defined(PMHELP_H)


 /******************************************************************************/
 /* Messages applications can send to the Help Manager.                        */
 /******************************************************************************/

 #define HM_DISMISS_WINDOW              HM_MSG_BASE+0x0001
 #define HM_DISPLAY_HELP                HM_MSG_BASE+0x0002
 #define HM_EXT_HELP                    HM_MSG_BASE+0x0003
 #define HM_SET_ACTIVE_WINDOW           HM_MSG_BASE+0x0004
 #define HM_LOAD_HELP_TABLE             HM_MSG_BASE+0x0005
 #define HM_CREATE_HELP_TABLE           HM_MSG_BASE+0x0006
 #define HM_SET_HELP_WINDOW_TITLE       HM_MSG_BASE+0x0007
 #define HM_SET_SHOW_PANEL_ID           HM_MSG_BASE+0x0008
 #define HM_REPLACE_HELP_FOR_HELP       HM_MSG_BASE+0x0009
 #define HM_HELP_INDEX                  HM_MSG_BASE+0x000a
 #define HM_HELP_CONTENTS               HM_MSG_BASE+0x000b
 #define HM_KEYS_HELP                   HM_MSG_BASE+0x000c
 #define HM_SET_HELP_LIBRARY_NAME       HM_MSG_BASE+0x000d

 /******************************************************************************/
 /* Messages the Help Manager sends to the applications active window          */
 /* as defined by the Help Manager.                                            */
 /******************************************************************************/

 #define HM_ERROR                       HM_MSG_BASE+0x000e
 #define HM_HELPSUBITEM_NOT_FOUND       HM_MSG_BASE+0x000f
 #define HM_QUERY_KEYS_HELP             HM_MSG_BASE+0x0010
 #define HM_TUTORIAL                    HM_MSG_BASE+0x0011
 #define HM_EXT_HELP_UNDEFINED          HM_MSG_BASE+0x0012
 #define HM_ACTIONBAR_COMMAND           HM_MSG_BASE+0x0013
 #define HM_INFORM                      HM_MSG_BASE+0x0014

#endif

/*************************************************************************
*
* STRINGTABLE items
*
*************************************************************************/

#define IDS_TITLE                         0
#define IDS_MAX_MESSAGES                  1
#define IDS_MAX_MSG_PERCENT               2
#define IDS_MAX_PM_Q_SIZE                 3
#define IDS_DLL_NAME                      4
#define IDS_FMT_LEVEL                     5
#define IDS_FMT_TITLE                      6
#define IDS_FMT_OK_TO_EXIT                 7
#define IDS_FMT_OK_TO_HOOK                 8
#define IDS_HELP_TITLE_MAIN                9
#define IDS_HELP_TITLE_NEWS               10
#define IDS_MSG_TOO_MANY_SPIES            11

                                     /* === */

#define IDS_TOTAL                        12  /* must be last IDS_ item + 1 */

/************************************************************************/

#define PMSPY_QUIT_NOTICE    (WM_USER              + 1)

/************************************************************************/

#define IGNORE_THIS_MSG 0xFFFF
#define END_OF_ML       0xFFFF          /* @C6A - End of Msg List marker */
#define MAX_USER_DEF    8               /* Max # of User Defined messages */

/************************************************************************/

#define GDataScope           near    /* @C11A 'scope' of global data items */

/*---------------------------------------------------------------------------+
| Typedef's                                                                  |
+---------------------------------------------------------------------------*/

typedef USHORT         MSG,                     /* PM message */
                       IDS;                     /* @C10A STRINGTABLE index */




typedef struct {                                /* @C4A - HELP dialog data */

                 ULONG            HelpID,       /* HELP resource ID  */
                                  TitleID;      /* STRING index for HELP dialog title */
                 PCHAR            pHelpText;    /* working Resource pointer */

               } HELP_DATA, * PHELP_DATA;


/*---------------------------------------------------------------------------+
| Function declarations                                                      |
+---------------------------------------------------------------------------*/

MRESULT EXPENTRY SpyWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

MRESULT EXPENTRY HelpWindowProc(         HWND    hwnd,
                                register ULONG   message,
                                         MPARAM  lParam1,
                                         MPARAM  lParam2 );

extern USHORT MsgDisplay( HWND   hWnd,
                          PSZ    strCap,
                          PSZ    strFormat,
                          USHORT mb_id,
                          USHORT style,
                          ...);


BOOL    EXPENTRY SpyJrnRecordHookProc (HAB habSpy, PQMSG pQmsg);
ULONG   EXPENTRY SpyJrnPlayHookProc (HAB habSpy,BOOL fSkip, PQMSG pQmsg);
INT     EXPENTRY SpyInstalled(INT bSet);
INT     EXPENTRY SpyDllVersion(VOID);
BOOL    EXPENTRY PlayOnQueue(BOOL value, INT bSet);
BOOL    EXPENTRY RecordOnQueue(BOOL value, INT bSet);
BOOL    EXPENTRY TransferMessage(PQMSG pQmsg, INT bSet);
HWND    EXPENTRY SpyWndHandle(HWND hwnd, INT bSet);
HAB     EXPENTRY SpyHabHandle(HAB  hab , INT bSet);
MRESULT EXPENTRY SubclassWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
PFNWP   EXPENTRY SpySubclassProcaddr(PFNWP DefaultProc , INT bSet);
BOOL    EXPENTRY SpyGetVioData(BOOL GetIt , INT bSet);
HWND    EXPENTRY TargetWindow( HWND value, INT bSet);


/*---------------------------------------------------------------------------+
| Variable declarations                                                      |
+---------------------------------------------------------------------------*/

/* determine SCOPE to use on global variables */


#if defined(DEFINE_VARIABLES)

   #define xSCOPE
   #define iGLOBAL(init) = init         /*  specify initialization            */

#else

   #define xSCOPE extern
   #define iGLOBAL(init)                /*  do not specify initialization     */

#endif


xSCOPE CHAR     szText[256],
                szMsgText[256],
                szWndTitle[64];

xSCOPE HAB      hab;

xSCOPE HWND     hwndFrame,            /* FRAME window handle */
                hwndClient,           /* CLIENT area window handle */
                hwndMenu,

                hwndHook;             /* 0xFFFF = no trace                            */

xSCOPE HMQ      hmq,
                hwndQueue            iGLOBAL( 0 ),
                hwndQSave            iGLOBAL( 0 );

xSCOPE HSWITCH  hSwitch;

xSCOPE SWCNTRL  swcntrl;

xSCOPE BOOL     bSelecting           iGLOBAL( FALSE );

xSCOPE INT      Installed            iGLOBAL( 0 ),  /* 0 = not installed, 1 = instance 1, 2 = instance 2 */
                NbrOfMessages        iGLOBAL( 0 ),
                MaxNbrOfMessages     iGLOBAL( 0 );

xSCOPE HPOINTER hSpy,
                hOld;

xSCOPE HMODULE  hSpyDll;

xSCOPE POINTL   ptrPos;

xSCOPE MSG      CurrMsg  iGLOBAL(IGNORE_THIS_MSG); /* Current Message being processed */

xSCOPE PSZ      Strings[IDS_TOTAL];   /* pointer to loaded STRINGTABLE items */

xSCOPE USHORT   CurUserDef    iGLOBAL(0);
xSCOPE PSZ      pszFmtStd     iGLOBAL(0);

/* Command line support */
xSCOPE char     **ArgV;
xSCOPE int      ArgC;

/* Journal file variables  */
xSCOPE CHAR     szJournalFile[CCHMAXPATH] iGLOBAL({""});
xSCOPE CHAR     szJournalPattern[CCHMAXPATH] iGLOBAL({"*.JRN"});
xSCOPE CHAR     szTestCasePattern[CCHMAXPATH] iGLOBAL({"*.CAS"});

