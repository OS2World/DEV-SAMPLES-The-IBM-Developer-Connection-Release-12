/*---------------------------------------------------------------------------+

+----------------------------------------------------------------------------+
|                                                                            |
| _FILEDLG.H                                                                 |
|                                                                            |
| File selection dialog DLL module for OS/2 Presentation Manager applications|
| DLL internal include file                                                  |
+-------------------------------------+--------------------------------------+
|                                     |   Juerg von Kaenel (JVK at ZURLVM1)  |
| Version: 0.03                       |   IBM Research Laboratory            |
|                                     |   Saeumerstrasse 4                   |
|                                     |   CH - 8803 Rueschlikon              |
|                                     |   Switzerland                        |
+-------------------------------------+--------------------------------------+
| History:                                                                   |
| --------                                                                   |
|                                                                            |
| created: dec 16 1988 by J. von Kaenel                                      |
| updated: mar  1 1989 by J. von Kaenel - added runtime linking to dll       |
| updated: mar  9 1989 by J. von Kaenel - initial release on pctools         |
+---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
| Window ID, Class name, Window Title                                        |
+---------------------------------------------------------------------------*/
#define ID_WINDOW           1
#define ID_OK               1
#define ID_CANCEL           2
#define ID_WIN_ICON       301
#define ID_WIN_MENU       302
#define ID_WIN_ACCTABLE   303
#define IDS_WIN_CLASS     304
#define IDS_WIN_TITLE     305

/*---------------------------------------------------------------------------+
| Menu IDs                                                                   |
+---------------------------------------------------------------------------*/
#define IDM_FILE            1000
#define IDM_EXIT            1100
#define IDM_HELP            9000

/*---------------------------------------------------------------------------+
| Dialog IDs                                                                 |
+---------------------------------------------------------------------------*/
/* --- File Selection IDs --- */
#define IDD_FILE_DLG         1080
#define IDD_EDIT             1082
#define IDD_DRIVE_LSTBOX     1084
#define IDD_DIR_LSTBOX       1085
#define IDD_FILE_LSTBOX      1086
#define IDD_HELP             1087
#define IDD_OK               ID_OK
#define IDD_CANCEL           ID_CANCEL
/* --- File Selection Constants --- */
#define FILEDLG_PATTERN_LENGTH 13
#define FILEDLG_DRIVE_LENGTH    3
#define FILEDLG_PATH_LENGTH   128
#define FILEDLG_FILE_LENGTH    13
/*---------------------------------------------------------------------------+
| String IDs                                                                 |
+---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------+
| Function declarations                                                      |
+---------------------------------------------------------------------------*/
MRESULT EXPENTRY JVK_FileDlg(HWND, USHORT, MPARAM, MPARAM);
INT EXPENTRY JVK_GetFileSelection(HWND, INT, PSZ, PSZ, PSZ, PSZ, HMODULE);
/*--- end of file ----------------------------------------------------------*/
