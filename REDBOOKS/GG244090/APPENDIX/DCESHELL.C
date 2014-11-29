/***************************************************************************

    PROGRAM: DCEShell.c

    PURPOSE: DCE shell for DCE application

****************************************************************************/

#include <windows.h>
#include <string.h>     /* strcpy    */
#include <stdarg.h>     /* va_list   */
#include <direct.h>     /* _chdir    */
#include <stdio.h>      /* vsprintf  */
#include <dos.h>        /* findfirst */

#define MAX_X 80
#define MAX_Y 25

#define SC_CANCEL  1

#define COLOR_BLACK RGB(   0,   0,   0)
#define COLOR_GRAY  RGB( 200, 200, 200)

#define LPMMI ((MINMAXINFO FAR *)lParam)
#define BUFFER( x, y) *(szBuffer + (y) * MAX_X + (x))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

int PASCAL WinMain( HANDLE, HANDLE, LPSTR, int);
long FAR PASCAL MainWndProc( HWND, UINT, WPARAM, LPARAM);

static BOOL InitApplication( HANDLE);
static BOOL InitInstance( HANDLE, int);

static HDC  PASCAL SetDC( HDC);
static void PASCAL ExecCommand( void);
static void PASCAL WriteTTY( LPSTR, int);
static void PASCAL ShowDir( char *dir);
static void PASCAL ReturnGets( void);

static HWND hWnd;
static HWND hDCEWnd;
static HANDLE hInst;
static TEXTMETRIC tm;

static int xCaret, yCaret;
static int cxChar, cyChar;
static int nWidth, nHeight;
static int cxClient, cyClient;
static int nVscrollPos, nHscrollPos;
static int nVscrollMax, nHscrollMax;

static char szBuffer[MAX_X * MAX_Y + MAX_X];
static char szLastCmd[MAX_X];
static char szPrompt[MAX_X] = "\n";
static char szAppName[] = "DCE Shell";

static UINT xGets;
static LPSTR szGetsBuffer;

static BOOL bGets, bPrompt;

static void trace( const char *fmt, ...);

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

    if ( !hPrevInstance )
        if ( !InitApplication( hInstance) )
            return( FALSE);

    if ( !InitInstance( hInstance, nCmdShow) )
        return( FALSE);

    while ( GetMessage( &msg, NULL, NULL, NULL) ) {
        TranslateMessage( &msg);
        DispatchMessage( &msg);
    }

    return( msg.wParam);
}

/****************************************************************************

    FUNCTION: InitApplication( HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL InitApplication( hInstance)
HANDLE hInstance;
{
    WNDCLASS  wc;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, "DCEIcon");
    wc.hCursor = LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject( BLACK_BRUSH);
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "DCEShellWClass";

    return( RegisterClass( &wc));
}

/****************************************************************************

    FUNCTION:  InitInstance( HANDLE, int)

    PURPOSE:  Saves instance handle and creates main window

****************************************************************************/

BOOL InitInstance( hInstance, nCmdShow)
HANDLE hInstance;
int    nCmdShow;
{
    hInst = hInstance;

    /* Create a main window for this application instance.  */

    hWnd = CreateWindow( "DCEShellWClass",szAppName,
        WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    /* If window could not be created, return "failure" */

    if ( !hWnd )
        return( FALSE);

    /* Make the window visible; update its client area; and return "success" */

    ShowWindow( hWnd, nCmdShow);
    UpdateWindow( hWnd);
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
    RECT rc;
    HDC hDC;
    PAINTSTRUCT ps;
    int nVscrollInc, nHscrollInc;
    int i, j, x , y;

    static int xPos, yPos;
    static int cxMax, cyMax;
    static int cxScroll, cyScroll;

    switch ( msg) {

        case WM_CREATE:
            hDC = SetDC( GetDC( hWnd));
            GetTextMetrics( hDC, &tm);
            ReleaseDC( hWnd, hDC);
            cxChar = tm.tmAveCharWidth;
            cyChar = tm.tmHeight + tm.tmExternalLeading;
            cxScroll = GetSystemMetrics( SM_CXVSCROLL);
            cyScroll = GetSystemMetrics( SM_CYHSCROLL);
            cxMax = cxChar * MAX_X + 2 * GetSystemMetrics( SM_CXFRAME);
            cyMax = cyChar * MAX_Y + 2 * GetSystemMetrics( SM_CYFRAME)
                                      + GetSystemMetrics( SM_CYCAPTION);
            xPos = (GetSystemMetrics( SM_CXSCREEN) - cxMax) / 2;
            yPos = (GetSystemMetrics( SM_CYSCREEN) - cyMax) / 2;
            if ( xPos < 0 )
                yPos = 0;
            MoveWindow( hWnd, xPos, yPos,
                cxMax + cxScroll, cyMax + cyScroll, TRUE);
            memset( szBuffer, ' ', sizeof szBuffer);
            break;

        case WM_GETMINMAXINFO:
            LPMMI->ptMaxPosition.x = xPos;
            LPMMI->ptMaxPosition.y = yPos;
            LPMMI->ptMaxSize.x = cxMax + cxScroll;
            LPMMI->ptMaxSize.y = cyMax + cxScroll;
            LPMMI->ptMaxTrackSize.x = cxMax + cxScroll;
            LPMMI->ptMaxTrackSize.y = cyMax + cyScroll;
            break;

        case WM_PAINT:
            hDC = SetDC( BeginPaint( hWnd, &ps));
            for ( i = 0, j = nVscrollPos; i < nHeight + 1 ; ++i, ++j )
                TextOut( hDC, 0, i * cyChar, &BUFFER( nHscrollPos, j), nWidth + 1);
            SetCaretPos( (xCaret - nHscrollPos) * cxChar,
                         (yCaret - nVscrollPos) * cyChar + tm.tmAscent );
            EndPaint( hWnd, &ps);
            break;

        case WM_SETFOCUS:
            if ( !bPrompt && !hDCEWnd )
                SendMessage( hWnd, WM_USER, NULL, NULL);
            CreateCaret( hWnd, NULL, cxChar, 2);
            SetCaretPos( (xCaret - nHscrollPos) * cxChar,
                         (yCaret - nVscrollPos) * cyChar + tm.tmAscent );
            ShowCaret( hWnd);
            break;

        case WM_KILLFOCUS:
            HideCaret( hWnd);
            DestroyCaret( );
            break;

        case WM_CHAR:
            for ( i = 0 ; (WORD) i < LOWORD( lParam) ; ++i ) {
                switch ( wParam ) {
                    case VK_CANCEL:
                        if ( hDCEWnd )
                            SendMessage( hDCEWnd,WM_SYSCOMMAND,SC_CANCEL,NULL);
                        break;
                    case VK_ESCAPE:
                        if ( !bPrompt )
                            SendMessage( hWnd, WM_USER, NULL, NULL);
                        break;
                    case VK_RETURN:
                        if ( bGets )
                            ReturnGets( );
                        else if ( bPrompt )
                            ExecCommand( );
                        break;
                    default:
                        if ( bPrompt || bGets )
                            WriteTTY( (char *) &wParam, 1);
                }
            }
            break;

          case WM_SIZE:
               cxClient = LOWORD( lParam);
               cyClient = HIWORD( lParam);

               nWidth  = cxClient / cxChar;
               nHeight = cyClient / cyChar;

               nVscrollMax = max( 0, MAX_Y - nHeight);
               nVscrollPos = min( nVscrollPos, nVscrollMax);

               nHscrollMax = max( 0, MAX_X - nWidth);
               nHscrollPos = min( nHscrollPos, nHscrollMax);

               SetScrollRange( hWnd, SB_VERT, 0, nVscrollMax, FALSE);
               SetScrollPos( hWnd, SB_VERT, nVscrollPos, TRUE);

               SetScrollRange( hWnd, SB_HORZ, 0, nHscrollMax, FALSE);
               SetScrollPos( hWnd, SB_HORZ, nHscrollPos, TRUE);

               if ( !(nVscrollMax && nVscrollMax) || wParam == SIZE_MAXIMIZED )
               {
                   GetWindowRect( hWnd, &rc);
                   x = rc.right - rc.left;
                   y = rc.bottom - rc.top;
                   MoveWindow( hWnd, rc.left, rc.top,
                       x > cxMax ? (nVscrollMax ? x : cxMax) : x,
                       y > cyMax ? (nHscrollMax ? y : cyMax) : y,
                       TRUE);
               }
               break;

          case WM_VSCROLL:
               switch ( wParam )
                    {
                    case SB_TOP:
                         nVscrollInc = -nVscrollPos;
                         break;

                    case SB_BOTTOM:
                         nVscrollInc = nVscrollMax - nVscrollPos;
                         break;

                    case SB_LINEUP:
                         nVscrollInc = -1;
                         break;

                    case SB_LINEDOWN:
                         nVscrollInc = 1;
                         break;

                    case SB_PAGEUP:
                         nVscrollInc = min( -1, -cyClient / cyChar);
                         break;

                    case SB_PAGEDOWN:
                         nVscrollInc = max(  1,  cyClient / cyChar);
                         break;

                    case SB_THUMBTRACK:
                         nVscrollInc = LOWORD( lParam) - nVscrollPos;
                         break;

                    default:
                         nVscrollInc = 0;
                    }
               nVscrollInc = max( -nVscrollPos,
                             min(  nVscrollInc, nVscrollMax - nVscrollPos));

               if ( nVscrollInc )
                    {
                    nVscrollPos += nVscrollInc;
                    SetScrollPos( hWnd, SB_VERT, nVscrollPos, TRUE);
                    InvalidateRect( hWnd, NULL, FALSE);
                    UpdateWindow( hWnd);
                    }
               break;

          case WM_HSCROLL:
               switch ( wParam )
                    {
                    case SB_LINEUP:
                         nHscrollInc = -1;
                         break;

                    case SB_LINEDOWN:
                         nHscrollInc = 1;
                         break;

                    case SB_PAGEUP:
                         nHscrollInc = -20;
                         break;

                    case SB_PAGEDOWN:
                         nHscrollInc = 20;
                         break;

                    case SB_THUMBPOSITION:
                         nHscrollInc = LOWORD( lParam) - nHscrollPos;
                         break;

                    default:
                         nHscrollInc = 0;
                    }
               nHscrollInc = max( -nHscrollPos,
                             min(  nHscrollInc, nHscrollMax - nHscrollPos));

               if ( nHscrollInc )
                    {
                    nHscrollPos += nHscrollInc;
                    SetScrollPos( hWnd, SB_HORZ, nHscrollPos, TRUE);
                    InvalidateRect( hWnd, NULL, FALSE);
                    UpdateWindow( hWnd);
                    }
               break;

        case WM_USER:
            if ( !lParam ) {
                bGets = FALSE;
                hDCEWnd = NULL;
                SetWindowText( hWnd, (LPSTR) szAppName);
                if ( !bPrompt ) {
                    strcat( _getcwd( szPrompt + 1, sizeof szPrompt - 1), ">");
                    WriteTTY( szPrompt, strlen( szPrompt));
                }
                bPrompt = TRUE;
            }
            else {
                WriteTTY( (LPSTR) lParam, wParam);
            }
            break;

        case WM_USER + 1:
            hDCEWnd = (HWND) wParam;
            break;

        case WM_USER + 2:
            return( hInst);

        case WM_USER + 3:
            if ( hDCEWnd ) {
                xGets = xCaret;
                szGetsBuffer = (LPSTR) lParam;
                bGets = TRUE;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage( 0);
            break;

        case WM_KEYDOWN:
            if ( wParam == VK_F3 && bPrompt ) {
                xCaret = strlen( szPrompt) - 1;
                WriteTTY( szLastCmd, strlen( szLastCmd));
                break;
            }

        default:
            return( DefWindowProc( hWnd, msg, wParam, lParam));
    }
    return( NULL);
}

/****************************************************************************

    FUNCTION: WriteTTY( LPSTR, int)

    PURPOSE:  Put string on the screen

****************************************************************************/

void PASCAL WriteTTY( LPSTR str, int len)
{
    HDC hDC;
    int i, x, y;


    if ( yCaret - nVscrollPos > nHeight ) {
        SetScrollPos( hWnd, SB_VERT, (nVscrollPos = yCaret - nHeight + 1), TRUE);
        InvalidateRect( hWnd, NULL, FALSE);
    }
    if ( yCaret - nVscrollPos < 0 ) {
        SetScrollPos( hWnd, SB_VERT, (nVscrollPos = yCaret), TRUE);
        InvalidateRect( hWnd, NULL, FALSE);
    }
    if ( xCaret - nHscrollPos > nWidth - 2 ) {
        SetScrollPos( hWnd, SB_HORZ, (nHscrollPos = xCaret - nWidth + 2), TRUE);
        InvalidateRect( hWnd, NULL, FALSE);
    }
    if ( xCaret - nHscrollPos < 0 ) {
        SetScrollPos( hWnd, SB_HORZ, (nHscrollPos = xCaret), TRUE);
        InvalidateRect( hWnd, NULL, FALSE);
    }

    HideCaret( hWnd);
    hDC = SetDC( GetDC( hWnd));

    for ( i = 0 ; i < len ; ++i, ++str ) {
        switch ( *str ) {

            case '\n':
                if ( yCaret - nVscrollPos == nHeight - 1) {
                    memcpy( szBuffer, szBuffer + MAX_X, sizeof szBuffer - MAX_X);
                    if ( nVscrollPos < nVscrollMax )
                        SetScrollPos( hWnd, SB_VERT, ++nVscrollPos, TRUE);
                    InvalidateRect( hWnd, NULL, FALSE);
                    UpdateWindow( hWnd);
                }
                else {
                    ++yCaret;
                }

            case '\r':
                xCaret = 0;
                break;

            case '\x7':
                MessageBeep( 0);
                break;

            case '\t':
                do WriteTTY( " ", 1);
                while ( xCaret % 8 != 0 );
                break;

            case '\b':
                if ( xCaret > strlen( szPrompt) - 1) {
                    for ( x = --xCaret ; x < MAX_X - 1 ; ++x )
                        BUFFER( x, yCaret) = BUFFER( x + 1, yCaret);
                    TextOut( hDC, (xCaret - nHscrollPos) * cxChar,
                                  (yCaret - nVscrollPos) * cyChar,
                                  &BUFFER( xCaret, yCaret),
                                  MAX_X - nHscrollPos - xCaret);
                }
                break;

            default:
                if ( *str > '\x1F' ) {
                    BUFFER( xCaret, yCaret) = *str;
                    TextOut( hDC, (xCaret - nHscrollPos) * cxChar,
                                  (yCaret - nVscrollPos) * cyChar, str, 1);
                    if ( xCaret == MAX_X - 1 )
                        WriteTTY( "\n", 1);
                    else
                        ++xCaret;
                }
        }
    }
    ReleaseDC( hWnd, hDC);
    SetCaretPos( (xCaret - nHscrollPos) * cxChar,
                 (yCaret - nVscrollPos) * cyChar + tm.tmAscent );
    ShowCaret( hWnd);
}

/****************************************************************************

    FUNCTION: SetDC( HDC)

    PURPOSE:  Set device context

****************************************************************************/

static HDC PASCAL SetDC( HDC hDC)
{
    SetBkColor( hDC, COLOR_BLACK);
    SetTextColor( hDC, COLOR_GRAY);
    SelectObject( hDC, GetStockObject( OEM_FIXED_FONT));
    return( hDC);
}

/****************************************************************************

    FUNCTION: ExecCommand( void)

    PURPOSE:  Execute command

****************************************************************************/

void PASCAL ExecCommand( void)
{
    WORD wReturn;
    char szMsg[MAX_X];
    char szCmdLine[MAX_X];
    char *cmd, *arg;

    bPrompt = FALSE;

    BUFFER( xCaret, yCaret) = '\0';
    strcpy( szLastCmd, &BUFFER( strlen( szPrompt) - 1, yCaret));
    strcpy( szCmdLine, &BUFFER( strlen( szPrompt) - 1, yCaret));
    BUFFER( xCaret, yCaret) = ' ';
    cmd = strtok( szCmdLine, " ");
    arg = strtok( NULL, "");

    WriteTTY( "\n", 1);

    if ( !(*cmd) )
        --yCaret;
    else if ( strcmp( &cmd[1], ":") == 0 )
        _chdrive( 1 + cmd[0] - (cmd[0] < 'a' ? 'A' : 'a'));
    else if ( _stricmp( cmd, "CD") == 0 ) {
        if ( arg[1] == ':' ) {
            _chdrive( 1 + arg[0] - (arg[0] < 'a' ? 'A' : 'a'));;
            arg += 2;
        }
        _chdir( arg);
    }
    else if ( _stricmp( cmd, "CLS") == 0 ) {
        xCaret = yCaret = 0;
        memset( szBuffer, ' ', sizeof szBuffer);
        SetScrollPos( hWnd, SB_VERT, (nVscrollPos = 0), TRUE);
        SetScrollPos( hWnd, SB_HORZ, (nHscrollPos = 0), TRUE);
        InvalidateRect( hWnd, NULL, FALSE);
        UpdateWindow( hWnd);
    }
    else if ( _stricmp( cmd, "DIR") == 0 )
        ShowDir( arg);
    else if ( _stricmp( cmd, "EXIT") == 0 )
        DestroyWindow( hWnd);
    else {
        if ( (wReturn = WinExec( szLastCmd, (UINT)hWnd)) < 32 ) {
            if ( wReturn == 2 )
                strcpy( szMsg, "Bad command or file name\n");
            else
                wsprintf( szMsg, "WinExec failed error code %d\n", wReturn);
            WriteTTY( szMsg, strlen( szMsg));
         }
         else return;
    }

    SendMessage( hWnd, WM_USER, NULL, NULL);
}

/****************************************************************************

    FUNCTION: ReturnGets( )

    PURPOSE:  Return string to gets function

****************************************************************************/

static void PASCAL ReturnGets( )
{
    bGets = FALSE;
    BUFFER( xCaret, yCaret) = '\0';
    _fstrcpy( szGetsBuffer, &BUFFER( xGets, yCaret));
    BUFFER( xCaret, yCaret) = ' ';
    WriteTTY( "\n", 1);
    PostMessage( hDCEWnd, WM_USER + 1, NULL, NULL);
}
/****************************************************************************

    FUNCTION: printf( )

    PURPOSE:  Formated print

****************************************************************************/

static int printf( const char *fmt, ...)
{
    char s[80];
    int len; va_list marker;

    va_start(marker, fmt);
    len = vsprintf( s, fmt, marker);
    va_end( marker);

    WriteTTY( s, len);
}

/****************************************************************************

    FUNCTION: ShowDir ( char * dir)

    PURPOSE:  Show directory

****************************************************************************/

void PASCAL ShowDir( char *dir)
{
    struct find_t info;
    unsigned long bytes;
    unsigned attrib, files;
    char s[64];

    strcpy( s, dir);
    if ( !strchr( s, '.') )
        strcat( s, "*.*");
    attrib = _A_NORMAL | _A_SUBDIR | _A_RDONLY;

    if ( _dos_findfirst( s, attrib, &info) == 0 )
        do {
            if ( info.attrib & _A_SUBDIR )
                printf( "%-13s\t<DIR>\n", info.name);
            else
                printf( "%-13s\t%9lu\n", info.name, info.size);
            files++;
            bytes += info.size;
        } while ( _dos_findnext( &info) == 0 );

    printf( "%5u File(s)\t%lu bytes\n", files, bytes);
}

/***************************************************************************/
