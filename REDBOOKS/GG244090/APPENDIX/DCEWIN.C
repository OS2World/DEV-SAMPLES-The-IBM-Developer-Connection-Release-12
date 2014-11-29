/****************************************************************************

    PROGRAM: DCEWin.h

    PURPOSE: Windows DCE user interface

****************************************************************************/

#include <windows.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dce/pthreadx.h>
#include <dce/rpc.h>
#include "dcewin.h"

#undef  NULL
#define NULL 0

#define argc __argc
#define argv __argv
#define envp _environ

extern int __argc;
extern char **__argv;

static HWND hWnd;
static HWND hDCEWnd;

static BOOL InitInstance( HANDLE);
static BOOL InitApplication( HANDLE);

static int yield( void);
static void shutdown( void);

int PASCAL WinMain( HANDLE, HANDLE, LPSTR, int);
long FAR PASCAL MainWndProc( HWND, UINT, WPARAM, LPARAM);

static char szAppName[13];

static pthread_t far main_thread;

#define SC_CANCEL  1
#define BUFFERSIZE 1024

/****************************************************************************

    FUNCTION: InitApplication( HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL InitApplication( HANDLE hInstance)
{
    HICON hIcon;
    WNDCLASS  wc;

    hIcon = LoadIcon( (HINSTANCE)SendMessage( hDCEWnd, WM_USER + 2,
                                                 NULL, NULL), "DCEIcon");
    wc.style = NULL;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = ( hIcon ) ? hIcon : LoadIcon( NULL, IDI_APPLICATION);
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "DCEWClass";

    return( RegisterClass( &wc));
}

/****************************************************************************

    FUNCTION:  InitInstance( HANDLE)

    PURPOSE:  Saves instance handle and creates main window

****************************************************************************/

BOOL InitInstance( HANDLE hInstance)
{
    char szModName[64];

    GetModuleFileName( hInstance, szModName, sizeof szModName);
    strcpy( szModName, strrchr( szModName, '\\') + 1);
    strtok( strcpy( szAppName, szModName), ".");

    /* lets create our window */
    hWnd = CreateWindow(
        "DCEWClass",
        szAppName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if ( !hWnd )
        return( FALSE);
 
    /* initialize DCE client context */
    if ( DCETaskInit( NULL, 0, NULL) == -1 )
        return( FALSE);

    /* change DCESHELL window title */
    SetWindowText( hDCEWnd, (LPSTR) szModName);

    /* inform DCESHELL about our window handle */
    SendMessage( hDCEWnd, WM_USER + 1, (WPARAM) hWnd, NULL);

    /* display our window as an icon */
    ShowWindow( hWnd, SW_SHOWMINNOACTIVE);

    /* let begin the show */
    SendMessage( hWnd, WM_USER, NULL, NULL);

    return( TRUE);
}

/****************************************************************************

    FUNCTION: MainWndProc( HWND, UINT, WPARAM, LPARAM)

    PURPOSE:  Processes messages

****************************************************************************/

long FAR PASCAL MainWndProc( hWnd, msg, wParam, lParam)
HWND hWnd;
UINT msg;
WPARAM wParam;
LPARAM lParam;
{
    HMENU hMenu;
    pthread_t shutdown_thread;

    switch ( msg ) {

        case WM_CREATE:
           hMenu = GetSystemMenu( hWnd, FALSE);
           DeleteMenu( hMenu, SC_RESTORE,  MF_BYCOMMAND);
           DeleteMenu( hMenu, SC_SIZE,     MF_BYCOMMAND);
           DeleteMenu( hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
           DeleteMenu( hMenu, SC_MINIMIZE, MF_BYCOMMAND);
           InsertMenu( hMenu, SC_CLOSE,    MF_BYCOMMAND,
               SC_CANCEL, "C&ancel\tCtrl+C");
           return( 0);

        case WM_USER:
            main_thread = pthread_self( );
            DCEmain( argc, argv);

        case WM_DESTROY:
            DCETaskDestroy( );
            PostQuitMessage( 0);
            return( 0);

        case WM_SYSCHAR:
            if ( wParam == VK_CANCEL ) {
                SendMessage( hWnd, WM_SYSCOMMAND, SC_CANCEL, NULL);
                return( 0);
            }
            break;

        case WM_SYSCOMMAND:
            if ( wParam == SC_CANCEL ) {
                pthread_create( &shutdown_thread, pthread_attr_default,
                    (pthread_startroutine_t)shutdown, NULL );
                pthread_detach( &shutdown_thread);
                return( 0);
            }
            break;
    }
    return( DefWindowProc( hWnd, msg, wParam, lParam));
}

/****************************************************************************

    FUNCTION: WinMain( HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

****************************************************************************/

int PASCAL WinMain( hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR lpCmdLine;
int nCmdShow;
{
    MSG msg;

    /* save DCESHELL window handle */
    hDCEWnd = nCmdShow > SW_RESTORE ? (HWND) nCmdShow : NULL;

    if ( !hPrevInstance )
        if ( !InitApplication( hInstance) )
            return( FALSE);

    if ( !InitInstance( hInstance) )
        return( FALSE);

    return( yield( ));
}

/****************************************************************************

    FUNCTION: DCEexit( int)

    PURPOSE:  Exit DCE application

****************************************************************************/

void DCEexit( int rtn)
{
    DestroyWindow( hWnd);
    yield( );
#undef exit
    exit( rtn);
#define exit(x) DCEexit(x)
}

/****************************************************************************

    FUNCTION: yield( )

    PURPOSE:  Yields to Windows

****************************************************************************/

static int yield( void)
{
    MSG msg;
    BOOL BackgroundThread = TRUE;

    for (;;) {
        /* lets check our message queue for a message */
        while ( PeekMessage (&msg, NULL, 0, 0, PM_REMOVE )) {

            /* if message is WM_QUIT we are done */
            if ( msg.message == WM_QUIT ) {
                /* since we are done lets tell DCESHELL about it */
                SendMessage( hDCEWnd, WM_USER, NULL, NULL);
                return( TRUE);
            }
            TranslateMessage( &msg);
            DispatchMessage( &msg);
        }
        if ( BackgroundThread )
            /* yield to other DCE threads */
            pthread_yield ();
        else
            /* yield to other Windows applications */
            WaitMessage( );
    }
}

/****************************************************************************

    FUNCTION: shutdown( )

    PURPOSE: Shutdown the main task

****************************************************************************/

void shutdown( void)
{
    pthread_cancel( main_thread);
}

/****************************************************************************

    FUNCTION: printf( )

    PURPOSE:  Formated print

****************************************************************************/

int printf( const char *fmt, ...)
{
    char s[BUFFERSIZE];
    int len; va_list marker;

    va_start( marker, fmt);
    len = wvsprintf( s, fmt, marker);
    va_end( marker);

    LockData( NULL);

    if ( hDCEWnd )
        SendMessage( hDCEWnd, WM_USER, strlen( s), (LPARAM) ((LPSTR) s));
    else
        MessageBox( NULL, s, szAppName, MB_OK);

    UnlockData( NULL);

    return len;
}

/****************************************************************************

    FUNCTION: fprintf( )

    PURPOSE:  Formated print

****************************************************************************/

int fprintf( FILE *file, const char *fmt, ...)
{
    char s[BUFFERSIZE];
    int len; va_list marker;

    va_start( marker, fmt);
    len = wvsprintf( s, fmt, marker);
    va_end( marker);

    if ( file != stderr || file != stdout ) {
        len = fputs( s, file);
    }
    else {

        LockData( NULL);

        if ( hDCEWnd )
            SendMessage( hDCEWnd, WM_USER, strlen( s), (LPARAM) ((LPSTR) s));
        else
            MessageBox( NULL, s, szAppName, MB_OK);

        UnlockData( NULL);
    }

    return len;
}

/****************************************************************************

    FUNCTION: gets( )

    PURPOSE:  Get string

****************************************************************************/

char *gets( char *buffer)
{
    MSG msg;

    *buffer = '\0';

    if ( !hDCEWnd )
        return( buffer);

    SendMessage( hDCEWnd, WM_USER + 3, NULL, (LPARAM) ((LPSTR) buffer));

    while ( GetMessage( &msg, NULL, NULL, NULL) ) {
        TranslateMessage( &msg);
        DispatchMessage( &msg);

        if ( msg.message == WM_USER + 1 )
            return( buffer);
    }
    SendMessage( hDCEWnd, WM_USER, NULL, NULL);

#undef exit
    exit( 0);
#define exit(x) DCEexit(x)
}

/***************************************************************************/
