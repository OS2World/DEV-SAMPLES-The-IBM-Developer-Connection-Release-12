// mshell.h

#define LEN_MAXPATH               512
#define LEN_WORKSTRING            256
#define LEN_STACK               32768
#define LEN_STARTLIST              50
#define CLASSNAME              "MSHELL Client Class"
#define OBJECTCLASSNAME        "MSHELL Object Class"
#define CAPTION                "MiniShell"
#define INI_APP                "MSHELL"
#define INIKEY_PROFILE         "PROFILE"


// user-defined messages
#define WM_USER_ACK                     (WM_USER+0)
#define WM_USER_START                   (WM_USER+1)
#define WM_USER_ENABLE_CLIENT           (WM_USER+2)
#define WM_USER_DISABLE_CLIENT          (WM_USER+3)
#define WM_USER_CLOSE                   (WM_USER+4)
#define WM_USER_ADD_PROGRAMS            (WM_USER+5)
#define WM_USER_START_CMD               (WM_USER+6)

// user nacks
#define WM_NACK_NO_INI                  (WM_USER+50)
#define WM_NACK_SYNTAX_ERROR            (WM_USER+51)
#define WM_NACK_NO_SPOOLER              (WM_USER+52)



struct _profile {
  SWP     swp;
  SWP     swpMinimized;
};
typedef struct _profile PROFILE, *PPROFILE;


struct _startem {
  BOOL bAutoStart;
  PSZ  pszTitle;
  PSZ  pszCMD;
};
typedef struct _startem STARTEM, *PSTARTEM;


// globals structure
struct _globals {
  HAB            hab;
  HMQ            hmq;
  HWND           hwndClient;
  HWND           hwndFrame;
  HWND           hwndMenubar;
  HWND           hwndObject;
  HWND           hwndTitlebar;
  HWND           hwndListbox;
  HWND           hwndSpooler;
  HDC            hdcScreen;
  HPS            hpsScreen;
  BOOL           fBusy;
  BOOL           fSpooler;
  BOOL           fShutdownCalled;
  LONG           cStartem;
  STARTEM        aStartem[ LEN_STARTLIST ];
  HFILE          hfReadPipe;
  HFILE          hfWritePipe;
  PID            pidCMD;
  RECTL          rectlDesktop;
  PROFILE        profile;
};
typedef struct _globals GLOBALS, *PGLOBALS;


// function prototypes
MRESULT EXPENTRY ClientWinProc    ( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY ObjectWinProc    ( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY SpoolerDlgProc   ( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT Command( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
PGLOBALS Create( HWND hwnd );
char *trim( char * );
char *ltrim( char * );
void _Optlink threadmain( void * pv  );


