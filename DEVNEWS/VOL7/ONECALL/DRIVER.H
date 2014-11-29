// driver.h
// include after setup.h
// Monte Copeland for DevCon 7

#define ID_DRIVER              101
#define IDM_PRINT              102
#define IDM_PRINTERSETUP       103

#define CLASSNAME              "Driver Client Window"
#define OBJECTCLASSNAME        "Driver Object Window"
#define CAPTION                "Test Program"
#define LEN_STACK                 32000

// global variables are kept in this structure
struct _globals {
  BOOL          fBusy;               // object window is busy
  BOOL          fCloseMsgSent;       // false until client gets a WM_CLOSE msg
  HAB           hab;                 // anchor block handle
  HWND          hwndClient;          // client window
  HWND          hwndFrame;           // frame window
  HWND          hwndMenu;            // menu bar
  HWND          hwndTitle;           // title bar
  HWND          hwndObject;          // object window handle on thread 2
  ULONG         tidObject;           // thread id of object thread
  PRINTERSETUP  Setup;               // printer setup data
  CHAR          szFileName[ CCHMAXPATH ];  // filename given on command line
};
typedef struct _globals GLOBALS, *PGLOBALS;

// globals is declared in driver.c
extern GLOBALS globals;

// user-defined messages
#define WM_USER_ACK                     (WM_USER+0)
#define WM_USER_DISABLE_CLIENT          (WM_USER+1)
#define WM_USER_ENABLE_CLIENT           (WM_USER+2)
#define WM_USER_PRINT                   (WM_USER+3)

void _Optlink threadmain( void * pv );
MRESULT APIENTRY ClientWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT APIENTRY ObjectWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
