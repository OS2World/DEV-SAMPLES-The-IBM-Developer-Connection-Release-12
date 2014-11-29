/*---------------------------------------------------------------------------+
+----------------------------------------------------------------------------+
|                                                                            |
| _FILEDLG.C                                                                 |
|                                                                            |
| File selection dialog DLL module for OS/2 Presentation Manager applications|
| DLL code                                                                   |
+-------------------------------------+--------------------------------------+
|                                     |   Juerg von Kaenel (JVK at ZURLVM1)  |
| Version: 0.09                       |   IBM Research Laboratory            |
|                                     |   Saeumerstrasse 4                   |
|                                     |   CH - 8803 Rueschlikon              |
|                                     |   Switzerland                        |
+-------------------------------------+--------------------------------------+
| History:                                                                   |
| --------                                                                   |
|                                                                            |
| created: dec 16 1988 by J. von Kaenel                                      |
| updated: mar  1 1989 by J. von Kaenel - added runtime linking to dll       |
| updated: mar  5 1989 by J. von Kaenel - added small help message           |
| updated: mar  9 1989 by J. von Kaenel - initial release on pctools         |
| updated: mar 14 1989 by J. von Kaenel - fixed a bug in the JVK_ExistFile   |
|                                         function (close file !)            |
| updated: apr  5 1989 by J. von Kaenel - sorted subdirs and files listboxes |
| updated: mai  8 1989 by J. von Kaenel - corrected existance test for files |
|                                         now correctly returns 1 if the     |
|                                         file does not exist                |
| updated: mai 23 1989 by J. von Kaenel - fixed code to find subdirs with    |
|                                         extensions                         |
| updated: jun  6 1989 by J. von Kaenel - fixed a bug in the JVK_ExistFile   |
|                                         function (null path test)          |
+---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
| Debugging on/off                                                           |
+---------------------------------------------------------------------------*/
#undef  DEBUG
/*---------------------------------------------------------------------------+
| Includes                                                                   |
+---------------------------------------------------------------------------*/
#define INCL_BASE
#define INCL_WIN
#include <os2.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "_filedlg.h"
/*---------------------------------------------------------------------------+
| Constants                                                                  |
+---------------------------------------------------------------------------*/
#define   FILENAMELEN     13
#define   CHAR_A_CONST    64
/*---------------------------------------------------------------------------+
| Global Presentation Manager Variables                                      |
+---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------+
| Global Application Variables                                               |
+---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------+
| Local Application Variables                                                |
+---------------------------------------------------------------------------*/
/* file search pattern, default = *.*          */
static CHAR JVK_pattern[FILEDLG_PATTERN_LENGTH];
static CHAR JVK_def_pattern[FILEDLG_PATTERN_LENGTH];
/* drive              , default = current drive*/
static CHAR JVK_drive[FILEDLG_DRIVE_LENGTH];
/* path               , default = current path */
static CHAR JVK_path[FILEDLG_PATH_LENGTH];
/* filename           , default = *.*          */
static CHAR JVK_file[FILEDLG_FILE_LENGTH];
static CHAR JVK_selDir[FILEDLG_FILE_LENGTH];
static CHAR szButtonText[] = "Open";
static CHAR szTitleText[] = "Save file as:";
static BOOL DlgEditChanged;
/*---------------------------------------------------------------------------+
| Macros                                                                     |
+---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------+
| Local Function declarations                                                |
+---------------------------------------------------------------------------*/
static VOID JVK_FillDrives(HWND);
static INT  JVK_FillSubdirs(HWND);
static VOID JVK_FillFiles(HWND);
static VOID JVK_FillEdit(HWND, PSZ);
static VOID JVK_GetDrive(HWND);
static VOID JVK_SelectDrive(HWND);
static INT  JVK_ExistDrive(HWND, CHAR);
static VOID JVK_SelectDir(HWND);
static INT  JVK_SplitPath(HWND, PSZ, PSZ, PSZ, PSZ);
static INT  JVK_ExistFile(HWND);
static INT  JVK_HardErrHandler(HWND, USHORT);
/*---------------------------------------------------------------------------+
| Exported functions                                                         |
+---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
| GetFileSelection                                                           |
|   INPUT:  Parent window handle                                             |
|           0 = Open, 1 = Save                                               |
|           file search pattern, default = *.*                               |
|           drive              , default = current drive                     |
|           path               , default = current path                      |
|           filename           , default = *.*                               |
|   OUTPUT: 0 = selection OK                                                 |
|           1 = selected file does not exist                                 |
|          -1 = selection canceled                                           |
|          -2 = invalid purpose                                              |
|          -3 = invalid pattern                                              |
|          -4 = drive does not exist                                         |
|          -5 = drive\path does not exist                                    |
|          -6 = file pointer may not be NULL                                 |
|          -7 = dll file not found, or dll procedure not found               |
|                                                                            |
+---------------------------------------------------------------------------*/
INT EXPENTRY JVK_GetFileSelection(hwndParent, purpose, pattern, drive,
                                  path, file, hFileDlg)
  HWND hwndParent;
  INT  purpose;   /* 0 = OPEN, 1 = SAVE                                     */
  CHAR pattern[]; /* file search pattern, default = *.*                     */
  CHAR drive[];   /* drive              , default = current drive           */
  CHAR path[];    /* path               , default = current path            */
  CHAR file[];    /* filename           , default = *.*                     */
  HMODULE hFileDlg;
{
  INT     i,rc,len;
  USHORT  curDisk;
  ULONG   bDisks;

  /* test the parameters */
  if (purpose == 0) {
    /* OPEN request */
    strcpy(szButtonText,"Open");
    strcpy(szTitleText,"Open file:");
  } else {
    if (purpose == 1) {
      /* SAVE request */
      strcpy(szButtonText,"Save");
      strcpy(szTitleText,"Save file as:");
    } else {
      /* invalid purpose */
      return(-2);
    } /* endif */
  } /* endif */
  if ((pattern == NULL) || (pattern[0] == '\0')) {
    /* get default pattern */
    strcpy(JVK_pattern, "*.*");
  } else {
    strncpy(JVK_pattern, pattern, FILEDLG_PATTERN_LENGTH-1);
  } /* endif */
  strncpy(JVK_def_pattern, JVK_pattern, FILEDLG_PATTERN_LENGTH-1);
  if ((drive == NULL) || (drive[0] == '\0')) {
    /* get default drive */
    DosQCurDisk((PUSHORT)&curDisk, (PULONG)&bDisks);
    JVK_drive[0] = (CHAR)(CHAR_A_CONST + curDisk);
    JVK_drive[1] = ':';
    JVK_drive[2] = '\0';
    if (drive != NULL) {
      strcpy(drive, JVK_drive);
    } else {
    } /* endif */
  } else {
    if (JVK_ExistDrive(hwndParent, drive[0]) == -1) {
      return(-4);
    } else {
      strncpy(JVK_drive, drive, FILEDLG_DRIVE_LENGTH-1);
    } /* endif */
  } /* endif */
  if ((path == NULL) || (path[0] == '\0')) {
    /* get default path */
    len = FILEDLG_PATH_LENGTH-1;
    DosQCurDir(curDisk, (PBYTE)JVK_path, (PUSHORT)&len);
    if (path != NULL) {
      strcpy(path, JVK_path);
    } else {
    } /* endif */
  } else {
    strncpy(JVK_path, path, FILEDLG_PATH_LENGTH-1);
  } /* endif */
  if (file == NULL) {
    /* file pointer may not be NULL */
    return(-6);
  } else {
    if (file[0] == '\0') {
      /* get default file */
      strcpy(JVK_file, "*.*");
    } else {
      strncpy(JVK_file, file, FILEDLG_FILE_LENGTH-1);
    } /* endif */
  } /* endif */

  rc = WinDlgBox(HWND_DESKTOP,
                 hwndParent,
                 (PFNWP)JVK_FileDlg,
                 (HMODULE)hFileDlg,
                 IDD_FILE_DLG,
                 (PCH)NULL);

  switch (rc) {
    case 0:
    case 1:
      /* OK */
      if (drive != NULL) strcpy(drive, JVK_drive);
      if (path  != NULL) strcpy(path, JVK_path);
      if (file  != NULL) strcpy(file, JVK_file);
      break;
    case -1:
      /* CANCEL */
      rc = -1;
      break;
  } /* endswitch */
  return(rc);
}

/*---------------------------------------------------------------------------+
| JVK_FileDlg                                                                |
| Dialog procedure for the Open/Save file dialog box                         |
+---------------------------------------------------------------------------*/
MRESULT EXPENTRY JVK_FileDlg(hwnd, msg, mp1, mp2)
  HWND    hwnd;
  USHORT  msg;
  MPARAM  mp1;
  MPARAM  mp2;
{
  INT    SelItem;
  INT    len,i, rc;
  USHORT curDisk;
  CHAR   szTxtBuf[256];
  CHAR   szDriveBuf[FILEDLG_DRIVE_LENGTH];
  CHAR   szPathBuf[FILEDLG_PATH_LENGTH];
  CHAR   szFileBuf[FILEDLG_FILE_LENGTH];

  switch( msg ) {
    case WM_INITDLG:
      rc = (USHORT)WinSendDlgItemMsg(hwnd, IDD_EDIT, EM_SETTEXTLIMIT,
                                     MPFROMSHORT(256), (MPARAM)NULL);
      DlgEditChanged = FALSE;
      JVK_FillDrives(hwnd);
      JVK_FillSubdirs(hwnd);
      WinSetDlgItemText(hwnd, IDD_OK, (PSZ)szButtonText);
      WinSetWindowText(hwnd, (PSZ)szTitleText);
      break;

    case WM_HELP:
      WinMessageBox(HWND_DESKTOP,
                    hwnd,
                    (PSZ)"This 'File Dialog' panel was written by J. von K„nel (JVK @ ZURLVM1)",
                    (PSZ)"FileDlg Help (0.09)",
                    0,
                    MB_CUANOTIFICATION | MB_CANCEL | MB_MOVEABLE );
      break;
    case WM_COMMAND:
      switch (LOUSHORT(mp1)) {
        case IDD_OK:
          /* test for recent changes to the controls */
          i = FALSE;
          if (DlgEditChanged) {
            WinQueryDlgItemText(hwnd,
                                IDD_EDIT,
                                sizeof(szTxtBuf),
                                szTxtBuf);
            /* test if pattern or valid filename */
            if (szTxtBuf[0] == '\0') {
              if (JVK_pattern[0] == '\0') {
                strncpy(JVK_pattern, JVK_def_pattern, FILEDLG_FILE_LENGTH-1);
              } else {
              } /* endif */
              JVK_FillEdit(hwnd, (PSZ)JVK_pattern);
            } else {
              if ((strchr(szTxtBuf, '*') != NULL) ||
                  (strchr(szTxtBuf, '?') != NULL)) {
                /* a pattern */
                JVK_file[0] = '\0';
                rc = JVK_SplitPath(hwnd, szTxtBuf, szDriveBuf,
                                   szPathBuf, JVK_pattern);
                if (rc == -1) {
                  /* invalid input from user, reselect files */
                  if (JVK_pattern[0] == '\0') {
                    strncpy(JVK_pattern, JVK_def_pattern, FILEDLG_FILE_LENGTH-1);
                  } else {
                  } /* endif */
                  JVK_FillEdit(hwnd, (PSZ)JVK_pattern);
                } else {
                  if (strcmp(JVK_drive, szDriveBuf) != 0) {
                    strcpy(JVK_drive, szDriveBuf);
                    strcpy(JVK_path, szPathBuf);
                    JVK_SelectDrive(hwnd);
                    JVK_FillSubdirs(hwnd);
                  } else {
                    if (strcmp(JVK_path, szPathBuf) != 0) {
                      strcpy(JVK_path, szPathBuf);
                      JVK_FillSubdirs(hwnd);
                    } else {
                      JVK_FillFiles(hwnd);
                    } /* endif */
                  } /* endif */
                } /* endif */
                i = TRUE;
              } else {
                if (strcmp(szTxtBuf, JVK_file) == 0) {
                  /* an already selected file name */
                } else {
                  /* a new file name */
                  rc = JVK_SplitPath(hwnd, szTxtBuf, szDriveBuf,
                                     szPathBuf, szFileBuf);
                  if (rc == -1) {
                    /* invalid input from user, reselect files */
                    JVK_file[0] = '\0';
                    JVK_FillFiles(hwnd);
                    i = TRUE;
                  } else {
                    strcpy(JVK_file, szFileBuf);
                    if (strcmp(JVK_drive, szDriveBuf) != 0) {
                      strcpy(JVK_drive, szDriveBuf);
                      strcpy(JVK_path, szPathBuf);
                      JVK_SelectDrive(hwnd);
                      JVK_FillSubdirs(hwnd);
                    } else {
                      if (strcmp(JVK_path, szPathBuf) != 0) {
                        strcpy(JVK_path, szPathBuf);
                        JVK_FillSubdirs(hwnd);
                      } else {
                      } /* endif */
                    } /* endif */
                    if (szFileBuf[0] != '\0') {
                      /* a proper file name */
                      WinDismissDlg(hwnd,JVK_ExistFile(hwnd));
                      return( (MRESULT)NULL );
                    } else {
                      i = TRUE;
                    } /* endif */
                  } /* endif */
                } /* endif */
              } /* endif */
            } /* endif */
            DlgEditChanged = FALSE;
          } else {
          } /* endif */
          if (i) {
            return(MPFROMLONG(NULL));
          } else {
          } /* endif */
          /*------------------------------------------------------------+
          |  A file has been selected from the list, so the full name   |
          |  of the requested file has to be retrieved.                 |
          +------------------------------------------------------------*/
          SelItem = (USHORT)(WinSendDlgItemMsg(hwnd,
                                             IDD_FILE_LSTBOX,
                                             LM_QUERYSELECTION,
                                             MPFROMSHORT( -1 ),
                                             MPFROMSHORT( 0 ) ));
          if (SelItem == LIT_NONE) {
            return(MPFROMLONG(NULL));
          } else {
          } /* endif */
          WinSendDlgItemMsg(hwnd,
                            IDD_FILE_LSTBOX,
                            LM_QUERYITEMTEXT,
                            MPFROM2SHORT((USHORT)SelItem, FILEDLG_FILE_LENGTH),
                            MPFROMP(JVK_file)
                            );

          /*------------------------------------------------------------+
          |  When no file was selected just ignore this request         |
          +------------------------------------------------------------*/

            WinDismissDlg(hwnd,0);
          break;


        case IDD_CANCEL:
          WinDismissDlg(hwnd,-1);
          break;

        default:
          ;
      } /* endswitch */
      break;
    case WM_CONTROL:
      switch (SHORT1FROMMP(mp1)) {
        case IDD_DRIVE_LSTBOX:
          switch (SHORT2FROMMP(mp1)) {
            case LN_SELECT:
              JVK_GetDrive(hwnd);
              break;
          } /* endswitch */
          break;
        case IDD_DIR_LSTBOX:
          switch (SHORT2FROMMP(mp1)) {
            case LN_SELECT:
              DosSleep(100L);
              JVK_SelectDir(hwnd);
              break;
          } /* endswitch */
          break;
        case IDD_FILE_LSTBOX:
          switch (SHORT2FROMMP(mp1)) {
            case LN_SELECT:
              SelItem = (USHORT)(WinSendDlgItemMsg(hwnd,
                                                   IDD_FILE_LSTBOX,
                                                   LM_QUERYSELECTION,
                                                   MPFROMSHORT(-1),
                                                   MPFROMSHORT(0)));
              WinSendDlgItemMsg(hwnd,
                                IDD_FILE_LSTBOX,
                                LM_QUERYITEMTEXT,
                                MPFROM2SHORT((USHORT)SelItem, FILEDLG_FILE_LENGTH),
                                MPFROMP(JVK_file)
                                );
              JVK_FillEdit(hwnd, (PSZ)JVK_file);
              break;
            case LN_ENTER:
              WinPostMsg(hwnd, WM_COMMAND,
                         MPFROMSHORT(IDD_OK), MPFROMSHORT(0));
              break;
          } /* endswitch */
          break;
        case IDD_EDIT:
          switch (SHORT2FROMMP(mp1)) {
            case EN_CHANGE:
              DlgEditChanged = TRUE;
              break;
          } /* endswitch */
          break;
      } /* endswitch */
      break;

    default:
      return(WinDefDlgProc(hwnd, msg, mp1, mp2));
  } /* endswitch */
  return( (MRESULT)NULL );
}

/*---------------------------------------------------------------------------+
| Local functions                                                            |
+---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
| JVK_FillFiles                                                              |
| Local procedure to fill the file list box                                  |
+---------------------------------------------------------------------------*/
static VOID JVK_FillFiles(HWND hwnd)
{
  struct  _FILEFINDBUF ResultBuf;    /* DOS file-search results buffer */
  USHORT  SearchCount;                         /* and input parameters */
  HDIR    DirHandle;
  USHORT  FileAttrib;
  CHAR    szFindPattern[FILEDLG_PATH_LENGTH];
  INT     i,rc;

  /*--------------------------------------------------------------------+
  |  The DOS file-search parameters are defined, and files of the       |
  |  required type (and only normal file entries) are sought.           |
  |  If no files are found, a message is displayed.                     |
  +--------------------------------------------------------------------*/
  DirHandle   = 0xffff;           /* allocate a handle to caller       */
  FileAttrib  = 0;                /* search for normal file entries    */
  SearchCount = 1;                /* one at a time                     */

  WinSendDlgItemMsg(hwnd, IDD_FILE_LSTBOX,
                    LM_DELETEALL, MPFROMSHORT(0), MPFROMSHORT(0));

  strcpy(szFindPattern, JVK_drive);
  strcat(szFindPattern, "\\");
  if (JVK_path[0] != '\0') {
    strcat(szFindPattern, JVK_path);
    strcat(szFindPattern, "\\");
  } /* endif */
  strcat(szFindPattern, JVK_pattern);
  DosError(0);  /* disable hard error popups to handle them internal */
  Retry:
  rc = DosFindFirst(szFindPattern,
                    &DirHandle,
                    FileAttrib,
                    &ResultBuf,
                    (USHORT)(FILENAMELEN + 23),
                    &SearchCount,
                    (ULONG)NULL);
  if (rc != 0) {
    switch (JVK_HardErrHandler(hwnd, rc)) {
      case MBID_RETRY:
        goto Retry;
        break;
      case MBID_OK:
      default:
        SearchCount = 0;
        break;
    } /* endswitch */
  } else {
  } /* endif */
  DosError(1);  /* enable hard error popups to be handled by OS/2 */

  /*--------------------------------------------------------------------+
  |  If at least one file name is found, an entry is made for that      |
  |  file in the list box, and the search continues until the path is   |
  |  exhausted.                                                         |
  +--------------------------------------------------------------------*/
  while (SearchCount == 1) {
    WinSendDlgItemMsg(hwnd,
                      IDD_FILE_LSTBOX,
                      LM_INSERTITEM,
                      MPFROMSHORT(LIT_SORTASCENDING),
                      MPFROMP((PSZ)ResultBuf.achName)
                      );
    DosFindNext(DirHandle,
                &ResultBuf,
                (USHORT)(FILENAMELEN + 23),
                &SearchCount);
  } /* endwhile */

  DosFindClose(DirHandle);

  if (JVK_file[0] != '\0') {
    i = WinSendDlgItemMsg(hwnd,
                          IDD_FILE_LSTBOX,
                          LM_SEARCHSTRING,
                          MPFROM2SHORT(LSS_SUBSTRING,LIT_FIRST),
                          MPFROMP(JVK_file));
    if (i == LIT_NONE) {
      /* file not found */
      JVK_FillEdit(hwnd, (PSZ)JVK_pattern);
    } else {
      WinSendDlgItemMsg(hwnd,
                        IDD_FILE_LSTBOX,
                        LM_SELECTITEM,
                        MPFROMSHORT(i),
                        MPFROMSHORT(TRUE));
      JVK_FillEdit(hwnd, (PSZ)JVK_file);
    } /* endif */
    DlgEditChanged = TRUE;
  } else {
    i = 0;
    JVK_FillEdit(hwnd, (PSZ)JVK_pattern);
  } /* endif */
}

/*---------------------------------------------------------------------------+
| JVK_FillSubdirs                                                            |
| Local procedure to fill the subdirectory listbox                           |
| OUTPUT: 0 no error                                                         |
|         1 drive not ready abbort                                           |
+---------------------------------------------------------------------------*/
static INT JVK_FillSubdirs(HWND hwnd)
{
  struct  _FILEFINDBUF ResultBuf;    /* DOS file-search results buffer */
  USHORT  SearchCount;                         /* and input parameters */
  HDIR    DirHandle;
  USHORT  FileAttrib;
  CHAR    buf[FILEDLG_FILE_LENGTH];
  CHAR    szFindPattern[FILEDLG_PATH_LENGTH];
  INT     len,rc;

  /*--------------------------------------------------------------------+
  |  The DOS file-search parameters are defined, and files of the       |
  |  required type (and only normal file entries) are sought.           |
  |  If no files are found, a message is displayed.                     |
  +--------------------------------------------------------------------*/
  DirHandle   = 0xffff;           /* allocate a handle to caller       */
  FileAttrib  = 0x10;             /* search for directory   entries    */
  SearchCount = 1;                /* one at a time                     */

  WinSendDlgItemMsg(hwnd, IDD_DIR_LSTBOX,
                    LM_DELETEALL, MPFROMSHORT(0), MPFROMSHORT(0));
  WinSendDlgItemMsg(hwnd, IDD_DIR_LSTBOX,
                    LM_SELECTITEM, MPFROMSHORT(LIT_NONE), MPFROMSHORT(TRUE));

  strcpy(szFindPattern, JVK_drive);
  if (JVK_path[0] != '\0') {
    strcat(szFindPattern, "\\");
    strcat(szFindPattern, JVK_path);
  } /* endif */
  strcat(szFindPattern, "\\*.*");
  DosError(0);  /* disable hard error popups to handle them internal */
  Retry:
  rc = DosFindFirst(szFindPattern,
                    &DirHandle,
                    FileAttrib,
                    &ResultBuf,
                    (USHORT)(FILENAMELEN + 23),
                    &SearchCount,
                    (ULONG)NULL);
  if (rc != 0) {
    switch (JVK_HardErrHandler(hwnd, rc)) {
      case MBID_RETRY:
        goto Retry;
        break;
      case MBID_OK:
        SearchCount = 0;
        break;
      default:
        DosError(1);  /* enable hard error popups to be handled by OS/2 */
        return(1);
        break;
    } /* endswitch */
  } else {
  } /* endif */
  DosError(1);  /* enable hard error popups to be handled by OS/2 */

  /*--------------------------------------------------------------------+
  |  Test for timeout on drives A and B                                 |
  +--------------------------------------------------------------------*/
  /*--------------------------------------------------------------------+
  |  If at least one subdirectory is found, an entry is made for that   |
  |  subdirectory in the list box, and the search continues.            |
  +--------------------------------------------------------------------*/
  while(SearchCount == 1) {
    if (ResultBuf.attrFile == 0x0010) {
      if (strcmp(ResultBuf.achName, ".") == 0) {
        /* ignore [.] directory */
      } else {
        buf[0] = '[';
        buf[1] = '\0';
        strcat(buf, ResultBuf.achName);
        len = strlen(buf);
        buf[len++] = ']';
        buf[len] = '\0';
        WinSendDlgItemMsg(hwnd,
                          IDD_DIR_LSTBOX,
                          LM_INSERTITEM,
                          MPFROMSHORT(LIT_SORTASCENDING),
                          MPFROMP((PSZ)buf)
                          );
      } /* endif */
    } /* endif */
    DosFindNext(DirHandle,
                &ResultBuf,
                (USHORT)(FILENAMELEN + 23),
                &SearchCount);
  } /* endwhile */

  DosFindClose(DirHandle);
  JVK_FillFiles(hwnd);
  return(0);
}

/*---------------------------------------------------------------------------+
| JVK_FillDrives                                                             |
| Local procedure to fill the path list box                                  |
+---------------------------------------------------------------------------*/
static VOID JVK_FillDrives(HWND hwnd)
{
  USHORT  curDisk;
  ULONG   bDisks,mask;
  INT     i;
  CHAR    szLogicalDrive[5];
  USHORT  len;
  BYTE    nbrDrives;

  WinSendDlgItemMsg(hwnd, IDD_DRIVE_LSTBOX,
                    LM_DELETEALL, MPFROMSHORT(0), MPFROMSHORT(0));

  DosQCurDisk((PUSHORT)&curDisk, (PULONG)&bDisks);
  /* if this machine has only one disk drive supress the drive B */
  DosDevConfig((PVOID)&nbrDrives, 2, 0);
  if (nbrDrives == 1) {
    bDisks &= ~0x02L;
  } /* endif */

  strcpy(szLogicalDrive, "[A:]");
  for (i=0, mask=1L; i<26; i++, mask<<=1) {
    if (bDisks & mask) {
      szLogicalDrive[1] = (CHAR)(CHAR_A_CONST+1+i);
      WinSendDlgItemMsg(hwnd,
                        IDD_DRIVE_LSTBOX,
                        LM_INSERTITEM,
                        (MPARAM)LIT_SORTASCENDING,
                        (MPARAM)(PSZ)szLogicalDrive);
    } /* endif */
  } /* endfor */
  JVK_SelectDrive(hwnd);
}

/*---------------------------------------------------------------------------+
| JVK_FillEdit                                                               |
| Local procedure to set the Edit control                                    |
+---------------------------------------------------------------------------*/
static VOID JVK_FillEdit(HWND hwnd, PSZ name)
{
  CHAR szText[FILEDLG_DRIVE_LENGTH+FILEDLG_PATH_LENGTH+FILEDLG_FILE_LENGTH];

  strcpy(szText, JVK_drive);
  strcat(szText, "\\");
  if (JVK_path[0] != '\0') {
    strcat(szText, JVK_path);
    strcat(szText, "\\");
  } /* endif */
  strcat(szText, name);
  WinSetDlgItemText(hwnd, IDD_EDIT, (PSZ)szText);
}

/*---------------------------------------------------------------------------+
| JVK_GetDrive                                                               |
+---------------------------------------------------------------------------*/
static VOID JVK_GetDrive(HWND hwnd)
{
  INT    SelItem,len,curDisk,rc;
  CHAR   szDriveBuf[FILEDLG_DRIVE_LENGTH+2];
  CHAR   OldDrive;

  SelItem = (USHORT)(WinSendDlgItemMsg(hwnd,
                                     IDD_DRIVE_LSTBOX,
                                     LM_QUERYSELECTION,
                                     MPFROMSHORT( -1 ),
                                     MPFROMSHORT( 0 ) ));
  WinSendDlgItemMsg(hwnd,
                    IDD_DRIVE_LSTBOX,
                    LM_QUERYITEMTEXT,
                    MPFROM2SHORT((USHORT)SelItem, FILEDLG_DRIVE_LENGTH),
                    MPFROMP((PSZ)szDriveBuf)
                    );
  if (JVK_drive[0] == szDriveBuf[1]) {
    /* already selected drive, no action */
  } else {
    OldDrive = JVK_drive[0];
    JVK_drive[0] = szDriveBuf[1];
    JVK_drive[1] = ':';
    JVK_drive[2] = '\0';
    len = FILEDLG_PATH_LENGTH-1;
    curDisk = (UCHAR)(JVK_drive[0]- CHAR_A_CONST);
    DosError(0);  /* disable hard error popups to handle them internal */
    Retry:
    rc = DosQCurDir(curDisk, (PBYTE)JVK_path, (PUSHORT)&len);
    if (rc != 0) {
      switch (JVK_HardErrHandler(hwnd, rc)) {
        case MBID_RETRY:
          goto Retry;
          break;
        case MBID_OK:
          JVK_path[0] = '\0'; /* path not found */
          break;
        default:
          JVK_drive[0] = OldDrive;
          JVK_SelectDrive(hwnd);
          DosError(1);  /* enable hard error popups to be handled by OS/2 */
          return;
          break;
      } /* endswitch */
    } else {
    } /* endif */
    DosError(1);  /* enable hard error popups to be handled by OS/2 */
    JVK_FillSubdirs(hwnd);
  } /* endif */
}

/*---------------------------------------------------------------------------+
| JVK_SelectDrive                                                            |
+---------------------------------------------------------------------------*/
static VOID JVK_SelectDrive(HWND hwnd)
{
  INT  i;

  i = WinSendDlgItemMsg(hwnd,
                        IDD_DRIVE_LSTBOX,
                        LM_SEARCHSTRING,
                        MPFROM2SHORT(LSS_SUBSTRING, LIT_FIRST),
                        MPFROMP(JVK_drive) );
  WinSendDlgItemMsg(hwnd,
                    IDD_DRIVE_LSTBOX,
                    LM_SELECTITEM,
                    MPFROMSHORT(i),
                    MPFROMSHORT(TRUE));
}

/*---------------------------------------------------------------------------+
| JVK_ExistDrive                                                             |
+---------------------------------------------------------------------------*/
static INT JVK_ExistDrive(HWND hwnd, CHAR drive)
{
  USHORT  curDisk;
  BYTE    nbrDrives;
  ULONG   bDisks,mask;
  static  CHAR szText[] = {"Drive x: does not exist on this machine!"};

  DosQCurDisk((PUSHORT)&curDisk, (PULONG)&bDisks);
  /* if this machine has only one disk drive supress the drive B */
  DosDevConfig((PVOID)&nbrDrives, 2, 0);
  if (nbrDrives == 1) {
    bDisks &= ~0x02L;
  } /* endif */
  if ((1L << (USHORT)drive-CHAR_A_CONST-1) & bDisks) {
    return(0);
  } else {
    szText[6] = drive;
    WinMessageBox(HWND_DESKTOP,
                  hwnd,
                  (PSZ)szText,
                  (PSZ)"FileDlg Error",
                  0,
                  MB_CUANOTIFICATION | MB_CANCEL | MB_MOVEABLE );
    return(-1);
  } /* endif */
}

/*---------------------------------------------------------------------------+
| JVK_SelectDir                                                              |
+---------------------------------------------------------------------------*/
static VOID JVK_SelectDir(HWND hwnd)
{
  INT SelItem,i,len;

  SelItem = (USHORT)(WinSendDlgItemMsg(hwnd,
                                     IDD_DIR_LSTBOX,
                                     LM_QUERYSELECTION,
                                     MPFROMSHORT( -1 ),
                                     MPFROMSHORT( 0 ) ));
  if (SelItem == LIT_NONE) {
    /* no item selected, ignore request */
  } else {
    WinSendDlgItemMsg(hwnd,
                      IDD_DIR_LSTBOX,
                      LM_QUERYITEMTEXT,
                      MPFROM2SHORT((USHORT)SelItem, FILEDLG_PATH_LENGTH),
                      MPFROMP((PSZ)JVK_selDir)
                      );
    i = strlen(JVK_path);
    if (strcmp(JVK_selDir, "[..]") == 0) {
      /* parent directory */
      while ((i > 0) && (JVK_path[i] != '\\')) {
        JVK_path[i--] = '\0';
      } /* endwhile */
      JVK_path[i] = '\0';
    } else {
      len = i;
      if (i == 0) {
        /* root directory */
        len--; /* trick to correct for no \ */
      } else {
        JVK_path[i++] = '\\';
      } /* endif */
      while (JVK_selDir[i-len] != ']') {
        JVK_path[i] = JVK_selDir[i-len];
        i++;
      } /* endwhile */
      JVK_path[i] = '\0';
    } /* endif */
    JVK_FillSubdirs(hwnd);
  } /* endif */
}

/*---------------------------------------------------------------------------+
| JVK_SplitPath                                                              |
|   OUTPUT    0: no error                                                    |
|            -1: an error occured (message box will be shown)                |
+---------------------------------------------------------------------------*/
static INT JVK_SplitPath(HWND hwnd, PSZ buffer, PSZ drive, PSZ path, PSZ file)
{
  INT  i,j,len;
  CHAR szTemp[10];
  BOOL done;

  drive[0] = '\0';
  path[0] = '\0';
  file[0] = '\0';
  strupr(buffer);
  /* test drive */
  if (buffer[1] == ':') {
    if (JVK_ExistDrive(hwnd, buffer[0]) == -1) {
       return(-1);
    } else {
    } /* endif */
    drive[0] = buffer[0];
    drive[1] = ':';
    drive[2] = '\0';
    i = 2;
  } else {
    strcpy(drive, JVK_drive);
    i = 0;
  } /* endif */
  if (buffer[i] == '\\') {
    /* remove first leading \ if any */
    i++;
  } else {
    strcpy(path, JVK_path);   /* ???? cur dir ? */
  } /* endif */
  done = FALSE;
  while (!done) {
    j=0;
    while (buffer[i] != '\\' && buffer[i] !='.' && buffer[i] != '\0' && j<8) {
      szTemp[j] = buffer[i];
      i++; j++;
    } /* endwhile */
    szTemp[j] = '\0';
    if (buffer[i] == '.' || buffer[i] == '\0') {
      /* a file name or a pattern */
      strcpy(file, szTemp);
      j = 0;
      len = strlen(file);
      while (j < 4 && buffer[i] != '\0') {
        file[len++] = buffer[i++];
      } /* endwhile */
      file[len++] = '\0';
      done = TRUE;
    } else {
      if (buffer[i] == '\\') {
        /* a subdirectory */
        if (path[0] == '\0') {
          /* first entry */
        } else {
          strcat(path, "\\");
        } /* endif */
        strcat(path, szTemp);
      } else {
        /* an error */
        WinMessageBox(HWND_DESKTOP,
                      hwnd,
                      (PSZ)"Subdirectory or Filename may not be longer than 8 characters",
                      (PSZ)"FileDlg Error",
                      0,
                      MB_CUANOTIFICATION | MB_CANCEL | MB_MOVEABLE );
        drive[0] = '\0';
        path[0] = '\0';
        file[0] = '\0';
        return(-1);
      } /* endif */
    } /* endif */
    i++;
  } /* endwhile */
  return(0);
}

/*---------------------------------------------------------------------------+
| JVK_ExistFile                                                              |
|   OUTPUT    0: the file exists                                             |
|             1: the file does not exist                                     |
+---------------------------------------------------------------------------*/
static INT JVK_ExistFile(HWND hwnd)
{
  INT file,rc;
  CHAR szTemp[FILEDLG_DRIVE_LENGTH+FILEDLG_PATH_LENGTH+FILEDLG_FILE_LENGTH];

  strcpy(szTemp, JVK_drive);
  strcat(szTemp, "\\");
  if (JVK_path[0] != '\0') {
    strcat(szTemp, JVK_path);
    strcat(szTemp, "\\");
  } else {
  } /* endif */
  strcat(szTemp, JVK_file);
  file = open(szTemp, O_RDONLY);
  if (file == -1) {
    /*
    WinMessageBox(HWND_DESKTOP,
                  hwnd,
                  (PSZ)"The selected file does not exist",
                  (PSZ)"FileDlg Error",
                  0,
                  MB_CUANOTIFICATION | MB_CANCEL | MB_MOVEABLE );
    */
    rc = 1;
  } else {
    rc = 0;
  } /* endif */
  close(file);
  return(rc);
}

/*---------------------------------------------------------------------------+
| JVK_HardErrHandler                                                         |
|   OUTPUT    MBID_RETRY:   retry operation                                  |
|             MBID_CANCEL:  abort operation                                  |
+---------------------------------------------------------------------------*/
static INT JVK_HardErrHandler(HWND hwnd, USHORT err)
{
  USHORT Class, Action, Locus, rc;
  CHAR   szText[256];

  DosErrClass(err, &Class, &Action, &Locus);

  switch (err) {
    case ERROR_NOT_READY:  /* drive not ready */
      #if defined(DEBUG)
       sprintf(szText, "Error: %d -> ", err);
        strcat(szText, "Please insert Disk in drive ");
      #else
        strcpy(szText, "Please insert Disk in drive ");
      #endif
      strcat(szText, JVK_drive);
      rc = WinMessageBox(HWND_DESKTOP,
                         hwnd,
                         (PSZ)szText,
                         (PSZ)"File Dialog Warning",
                         0,
                         MB_CUAWARNING | MB_RETRYCANCEL | MB_MOVEABLE );
      return(rc);
      break;
    case ERROR_DISK_CHANGE:   /* change disk drive */
      #if defined(DEBUG)
       sprintf(szText, "Error: %d -> ", err);
        strcat(szText, "Please make sure that the diskette in drive ");
      #else
        strcpy(szText, "Please make sure that the diskette in drive ");
      #endif
      strcat(szText, JVK_drive);
      strcat(szText, " is properly inserted, and that the drive door is locked.");
      rc = WinMessageBox(HWND_DESKTOP,
                         hwnd,
                         (PSZ)szText,
                         (PSZ)"File Dialog Warning",
                         0,
                         MB_CUAWARNING | MB_RETRYCANCEL | MB_MOVEABLE );
      return(rc);
      break;
    case ERROR_SECTOR_NOT_FOUND:   /* bad diskette */
      #if defined(DEBUG)
       sprintf(szText, "Error: %d -> ", err);
        strcat(szText, "Bad or not formatted disk in drive ");
      #else
        strcpy(szText, "Bad or not formatted disk in drive ");
      #endif
      strcat(szText, JVK_drive);
      rc = WinMessageBox(HWND_DESKTOP,
                         hwnd,
                         (PSZ)szText,
                         (PSZ)"File Dialog Warning",
                         0,
                         MB_CUAWARNING | MB_RETRYCANCEL | MB_MOVEABLE );
      return(rc);
      break;
    default:
      ;
  } /* endswitch */

  #if defined(DEBUG)
    sprintf(szText, "Error: %d, Class: %d, Action: %d, Locus: %d",
            err, Class, Action, Locus);
    rc = WinMessageBox(HWND_DESKTOP,
                       hwnd,
                       (PSZ)szText,
                       (PSZ)"File Dialog Warning",
                       0,
                       MB_CUAWARNING | MB_OK | MB_MOVEABLE );
  #endif
  return(MBID_OK);
}
/*--- end of file ----------------------------------------------------------*/
