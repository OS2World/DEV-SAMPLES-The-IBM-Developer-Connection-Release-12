#include <os2win.h>
#include "sample.h"


HANDLE hInst;
char szSampleClassName[] = "Sample";
char szHelpFileName[] = "Sample.Hlp";


/***********************************************************************/
/*  Program Overview:                                                  */
/*                                                                     */
/*  This program demonstrates all of the major steps of porting a      */
/*  Windows application to OS/2 using DAPIE.                           */
/***********************************************************************/

BOOL Init(HINSTANCE, int);
BOOL RegisterSampleClass(HINSTANCE);
BOOL CALLBACK AboutWndProc(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI SampleWndProc(HWND, UINT, WPARAM, LPARAM);


/***********************************************************************/
/*  WinMain() - Windows entry point.                                   */
/***********************************************************************/
int WINAPI WinMain(HANDLE hinstThis,
                   HANDLE hinstPrev,
                   LPSTR lpszCmdLine,
                   int iCmdShow)
{
   MSG msg;

   if (!Init(hinstThis, iCmdShow))
   {
      return(FALSE);
   }

   while (GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   return(msg.wParam);
}


/***********************************************************************/
/*  Init() - utility function                                          */
/*  registers classes, creates and displays application windows.       */
/***********************************************************************/
BOOL Init(HANDLE hinstThis, int iCmdShow)
{
   HWND hWnd;

   hInst = hinstThis;

   /********************************************************************/
   /* Register the window class for program's main window.             */
   /********************************************************************/
   if (!RegisterSampleClass(hinstThis))
   {
      return(FALSE);
   }

   /********************************************************************/
   /* Create a "Sample" class window.                                  */
   /********************************************************************/
   hWnd = CreateWindowEx(0,
                         szSampleClassName,
                         "Sample",
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         NULL,
                         NULL,
                         hInst,
                         NULL);

   /********************************************************************/
   /* Create the child container window.                               */
   /********************************************************************/
   CreateContainer(hWnd);

   ShowWindow(hWnd, iCmdShow);
   UpdateWindow(hWnd);

   return(hWnd != NULL);
}

/***********************************************************************/
/*  RegisterSampleClass() - utility function                           */
/*  registers "Sample" window class.                                   */
/***********************************************************************/
BOOL RegisterSampleClass(HANDLE hinstThis)
{
   WNDCLASS wc;

   /********************************************************************/
   /* Fill in the window class structure with information about the    */
   /* Sample window class                                              */
   /********************************************************************/
   wc.lpszClassName  = szSampleClassName;
   wc.hInstance      = hinstThis;
   wc.lpfnWndProc    = SampleWndProc;
   wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wc.hIcon          = LoadIcon(NULL, MAKEINTRESOURCE(IDI_QUESTION));
   wc.lpszMenuName   = MAKEINTRESOURCE(Sample_Menu);
   wc.hbrBackground  = (HANDLE)(COLOR_WINDOW + 1);
   wc.style          = CS_HREDRAW | CS_VREDRAW;
   wc.cbClsExtra     = 0;
   wc.cbWndExtra     = 0;

   /********************************************************************/
   /* Register the Sample class the with the system                    */
   /********************************************************************/
   return(RegisterClass(&wc));
}

/***********************************************************************/
/*  AboutWndProc() - window procedure for the "About" dialog window.   */
/*  called when the "About" dialog receives messages.                  */
/***********************************************************************/
BOOL CALLBACK AboutWndProc(HWND hWndDlg,
                           UINT message,
                           WPARAM wParam,
                           LPARAM lParam)
{
   switch (message)
   {
   case WM_INITDIALOG:
      return TRUE;

   case WM_COMMAND:
      EndDialog(hWndDlg, 0);
      return TRUE;
   }
   return FALSE;
}

/***********************************************************************/
/*  SampleWndProc() - main window procedure for the application.       */
/***********************************************************************/
LONG WINAPI SampleWndProc(HWND hWnd,
                          UINT message,
                          WPARAM wParam,
                          LPARAM lParam)
{
   switch (message)
   {
   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      /*****************************************************************/
      /* User selected "About" from the help menu; display the "About" */
      /* dialog box.                                                   */
      /*****************************************************************/
      case IDM_ABOUT:
         DialogBox(hInst,
                   MAKEINTRESOURCE(Sample_Dialog),
                   hWnd,
                   AboutWndProc);
         break;

      /*****************************************************************/
      /* User selected "Exit" from the file menu; exit the application.*/
      /*****************************************************************/
      case IDM_EXIT:
         PostQuitMessage(0);
         break;

      /*****************************************************************/
      /* User selected one of the help options from the help menu;     */
      /* exit the application.                                         */
      /*****************************************************************/
      case IDM_HELP_OVERVIEW:
         WinHelp(hWnd, szHelpFileName, HELP_CONTEXT, 1);
         break;

      case IDM_HELP_CONTENTS:
         WinHelp(hWnd, szHelpFileName, HELP_CONTENTS, 0);
         break;

      case IDM_HELP_KEYWORD:
         WinHelp(hWnd, szHelpFileName, HELP_PARTIALKEY, (DWORD)"");
         break;

      }
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      break;

   default:
      return(DefWindowProc(hWnd, message, wParam, lParam));
   }

   return(0);
}
