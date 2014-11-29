//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
// FILEDLG.H
//
// File selection dialog DLL module for OS/2 Presentation Manager applications
// Application program include file
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
// updated: feb  5 1990 by J. von Kaenel - CUA compatiblity added
//                                       - print purpose added
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
#define FILEDLG_PATTERN_LENGTH 13
#define FILEDLG_DRIVE_LENGTH    3
#define FILEDLG_PATH_LENGTH   128
#define FILEDLG_FILE_LENGTH    13

#define FILEDLG_OPEN            0
#define FILEDLG_SAVE            1
#define FILEDLG_PRINT           2

//----------------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------------
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
INT EXPENTRY GetFileSelection(HWND hwndParent, INT  purpose, PSZ pattern,
                              PSZ drive, PSZ path, PSZ file);
//--- end of file ------------------------------------------------------------
