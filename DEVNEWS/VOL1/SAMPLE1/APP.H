// start of file app.h -------------------------------------------------------

// strings
#define APP_TITLE               "Object Window Application"
#define APP_CLASS_CLIENT        "APPClient"
#define APP_CLASS_OBJECT        "APPObject"

// identifiers
#define ID_APP             3
#define IDM_SLEEP         303
#define IDM_ACTIONS       304

// lengths
#define LEN_WORKSTRING              256
#define LEN_STACK                 32768

// structure to hold globals variables common to both threads
struct _globals {
  BOOL           fBusy;
  HAB            hab;
  HWND           hwndClient;
  HWND           hwndFrame;
  HWND           hwndTitlebar;
  HWND           hwndMenubar;
  HWND           hwndObject;
  TID            tidObject;
};
typedef struct _globals GLOBALS, *PGLOBALS;

// user-defined messages for work items and acknowlegements
#define WM_USER_ACK                      (WM_USER+0)
#define WM_USER_SLEEP                    (WM_USER+1)
#define WM_USER_ENABLE                   (WM_USER+2)
#define WM_USER_DISABLE                  (WM_USER+3)

// function prototypes -- _Optlink is a IBM C SET/2 modifier
void _Optlink threadmain( void *  );
PGLOBALS Create( HWND hwnd );
MRESULT EXPENTRY ObjectWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY ClientWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// end of file app.h ----------------------------------------------------------
