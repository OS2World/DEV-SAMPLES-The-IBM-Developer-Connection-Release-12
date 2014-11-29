// rxdraw.h

#define CLASSNAME              "REXX Sample Client Window"
#define OBJECTCLASSNAME        "REXX Sample Object Window"
#define CAPTION                "REXX Drawing"
#define OUTPUTCAPTION          "REXX Diagnostics"
#define LEN_STACK                 32000

// global variables are kept in this structure
struct _globals {
  BOOL           fBusy;               // there is an outstanding task on the object window
  BOOL           fCloseMsgSent;       // false until client gets a WM_CLOSE msg
  HAB            hab;                 // anchor block handle
  HWND           hwndClient;          // graphics window
  HWND           hwndFrame;           // graphics frame window
  HWND           hwndMenubar;         // graphics window menu bar
  HWND           hwndTitlebar;        // graphics window title bar
  HWND           hwndOutputFrame;     // diagnostics frame window
  HWND           hwndOutputListbox;   // diagnostics window listbox
  HWND           hwndObject;          // object window handle on thread 2
  ULONG          tidObject;           // thread id of object thread
  HDC            hdcScreen;           // screen device context
  HPS            hpsScreen;           // screen presentation space
  CHAR           szCaption[ 256 ];    // title bar caption
  CHAR           szRexxFileName[ CCHMAXPATH ];   // rexx file name from command line
};
typedef struct _globals GLOBALS, *PGLOBALS;

// variable globals is declared in rxdraw.c
extern GLOBALS globals;

// user-defined messages
#define WM_USER_ACK                     (WM_USER+0)
#define WM_USER_DISABLE_CLIENT          (WM_USER+1)
#define WM_USER_ENABLE_CLIENT           (WM_USER+2)
#define WM_USER_START_REXX              (WM_USER+3)

// function prototypes; REXX-related prototypes are in object.c
MRESULT APIENTRY ClientWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT APIENTRY ObjectWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
void _Optlink threadmain( void * pv );
