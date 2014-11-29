//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
// FILEDLG.C
//
// File selection dialog DLL module for OS/2 Presentation Manager applications
// Application program static link code
//-------------------------------------+--------------------------------------
//                                     |   Juerg von Kaenel (JVK at ZURLVM1)
// Version: 0.13                       |   IBM Research Laboratory
//                                     |   Saeumerstrasse 4
//                                     |   CH - 8803 Rueschlikon
//                                     |   Switzerland
//-------------------------------------+--------------------------------------
// History:
// --------
//
// created: dec 16 1988 by J. von Kaenel
// updated: mar  1 1989 by J. von Kaenel - added runtime linking to dll
// updated: mar  9 1989 by J. von Kaenel - initial release on pctools
// updated: jun  6 1989 by J. von Kaenel - fixed declaration of pJVK_GetF...
// updated: feb  5 1990 by J. von Kaenel - CUA compatiblity added
//                                       - print purpose added
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Debugging on/off
//----------------------------------------------------------------------------
#undef  DEBUG

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#define INCL_BASE
#define INCL_WIN
#include <os2.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "_filedlg.h"

//----------------------------------------------------------------------------
// run time DLL resolving
//----------------------------------------------------------------------------
INT (EXPENTRY *pJVK_GetFileSelection)(HWND, INT, PSZ, PSZ, PSZ, PSZ, HMODULE);

//----------------------------------------------------------------------------
// GetFileSelection
//   INPUT:  Parent window handle
//           0 = Open, 1 = Save, 2 = Print
//           file search pattern, default = *.*
//           drive              , default = current drive
//           path               , default = current path
//           filename           , default = *.*
//   OUTPUT: 0 = selection OK
//           1 = selected file does not exist
//          -1 = selection canceled
//          -2 = invalid purpose
//          -3 = invalid pattern
//          -4 = drive does not exist
//          -5 = drive\path does not exist
//          -6 = file pointer may not be NULL
//          -7 = dll file not found, or dll procedure not found
//
//----------------------------------------------------------------------------
INT EXPENTRY GetFileSelection(hwndParent, purpose, pattern, drive, path, file)
  HWND hwndParent;
  INT  purpose;   /* 0 = OPEN, 1 = SAVE, 2 = PRINT, 3 = SCHEDULED           */
  PSZ  pattern;   /* file search pattern, default = *.*                     */
  PSZ  drive;     /* drive              , default = current drive           */
  PSZ  path;      /* path               , default = current path            */
  PSZ  file;      /* filename           , default = *.CMD                   */
{
  INT     rc;
  HMODULE hFileDlg;

  rc = DosLoadModule(NULL, 0, "FILEDLG", &hFileDlg);
  if (rc != 0) {
    WinMessageBox(HWND_DESKTOP,
                  hwndParent,
                  (PSZ)"The file FILEDLG.DLL could not be found",
                  (PSZ)"FileDlg:  DosLoadModule  Error",
                  0,
                  MB_CUAWARNING | MB_CANCEL | MB_MOVEABLE );
    return(-7);
  } else {
    rc = DosGetProcAddr(hFileDlg, "JVK_GETFILESELECTION",
                        (PFN FAR *)&pJVK_GetFileSelection);
    if (rc != 0) {
      WinMessageBox(HWND_DESKTOP,
                    hwndParent,
                    (PSZ)"The procdure GetFileSelection could not be found",
                    (PSZ)"FileDlg:  DosGetProcAddr  Error",
                    0,
                    MB_CUAWARNING | MB_CANCEL | MB_MOVEABLE );
      return(-7);
    } else {
    } /* endif */
  } /* endif */

  rc = pJVK_GetFileSelection(hwndParent, purpose, pattern, drive,
                             path, file, hFileDlg);
  DosFreeModule(hFileDlg);
  return(rc);
}
//--- end of file ------------------------------------------------------------
