/**************************************************************************
 *  File name  :  osatest.h
 *
 *  Description:  The OSATEST include file defines symbolic constants
 *                and data structures used by the OSA test pgm.
 *
 *************************************************************************/

/* string definitions */

/*************************************************************/
/* Name of Application that will be used in AEInit call                      */
/*************************************************************/

#define APP_NAME            "OSA Tests"

#define CLASSNAME            "OSATEST_CLIENTWINDOW"
#define OBJECTCLASSNAME      "OSATEST_OBJECTWINDOW"


/* length definitions */

#define LEN_STACK             6*4096
#define LEN_WORKSTRING        256

/* user-defined messages */

#define WM_USER_CLOSE                  (  WM_USER +  0 )
#define WM_USER_ACK                    (  WM_USER +  1 )
#define MSG_PROCESS_COMMAND (WM_USER+20)  /* Internally used message */

/* user-defined macros */
#define STRSAME(str1,str2) (!strcmpi(str1,str2))

/* user-defined structures */

/* Structure to describe each menu item supported by this application.  Some of */
/* the data here (e.g. action strings) are duplicated in the .RC file.  Only    */
/* selectable items are listed here (e.g. not submenu placeholders).            */

typedef struct {
                 USHORT CommandID;   /* Unique ID for each menu item */
                 PSZ    Action;      /* Unique 'action' string */
                 PSZ    Desc;        /* Description for this item (used in status bar and customization dlgs) */
               } AppItemStruct;


/*
 * Structure of program settings that are saved in the INI.
 * They are used as default setting upon the next invocation of the program.
 */

typedef struct _OSATEST_PROFILE {

    ULONG       flOptions;
    ULONG       cy;
    ULONG       cx;
    ULONG       y;
    ULONG       x;
    ULONG       testevnt_cy;
    ULONG       testevnt_cx;
    ULONG       testevnt_y;
    ULONG       testevnt_x;
    ULONG       scredit_cy;
    ULONG       scredit_cx;
    ULONG       scredit_y;
    ULONG       scredit_x;
    ULONG       screditStat_cy;
    ULONG       screditStat_cx;
    ULONG       screditStat_y;
    ULONG       screditStat_x;
    ULONG       screditStatTxt_cy;
    ULONG       screditStatTxt_cx;
    ULONG       screditStatTxt_y;
    ULONG       screditStatTxt_x;
    ULONG       screditScr_cy;
    ULONG       screditScr_cx;
    ULONG       screditScr_y;
    ULONG       screditScr_x;
    CHAR        scredit_path[CCHMAXPATH];
    ULONG       testrec_cy;
    ULONG       testrec_cx;
    ULONG       testrec_y;
    ULONG       testrec_x;
    ULONG       testrecStat_cy;
    ULONG       testrecStat_cx;
    ULONG       testrecStat_y;
    ULONG       testrecStat_x;
} OSATEST_PROFILE;
typedef OSATEST_PROFILE POSATEST_PROFILE;


/*
 * block of main program parameters
 * the variable that points to this block is usually named pmp
 */

typedef  struct _MAIN_PARM {
    HAB        hab;               /* hab obtained on thread 1                */

    HWND       hwndFrame;         /* frame handle                            */
    HWND       hwndClient;        /* client window handle                    */
    HWND       hwndMenubar;       /* menu handle, child of frame             */
    HWND       hwndTitlebar;      /* title bar handle, child of frame        */
    HWND       hwndCommandBar;
    HWND       hwndObject;        /* object window handle on thread 2        */
    HWND       hwndScriptEditor;  /* Script Editor dialog handle             */
    HWND       hwndTestEvent;     /* Test Events dialog handle               */
    HWND       hwndRecordEvent;   /* Record Events dialog handle             */

    BOOL       fCancel;           /* cancel                                  */
    BOOL       fBusy;             /* object window is busy with a task       */

    OSATEST_PROFILE Profile;    /* settings read/written from/to os2.ini     */

    PSZ         pszTitle;       /* app title initialized in prtcreat.c       */

    TID         tidObjectThread;      /* thread id for object window         */
} MAIN_PARM;
typedef MAIN_PARM *PMAIN_PARM;

/* Function prototypes used throughout OSA test program */
void _Optlink threadmain( void * );
MRESULT EXPENTRY ObjectWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY WinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT Menu( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY OSAOpendocEventsDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY OSARecordEventsDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY OSAScriptEditorDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY OSAListAETEDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
/*  Apple Event Interface Prototypes */
void InitOSA(HWND hwnd);
void TerminateOSA(HWND hwnd);

/* Responding to Apple Events */
ULONG ProcessSemanticEvent( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

OSErr APIENTRY RecordingProc(const OSAEvent   *theEvent,
                             OSAEvent     *theReply,
                             long          refCon );
OSErr APIENTRY RawEventRecordingProc(const OSAEvent   *theEvent,
                                     OSAEvent     *theReply,
                                     long          refCon );
int FindActionInItemList(PSZ Action);

VOID GetProfile(PMAIN_PARM pmp);
VOID SaveProfile(PMAIN_PARM pmp);
/***************************  End of osatest.h ****************************/
