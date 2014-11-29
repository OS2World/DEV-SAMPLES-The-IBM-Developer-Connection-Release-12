/*********** loadicon C Sample Program Include file (.H) ************************/
/*                                                                            */
/*  The loadicon include file defines symbolic constants used                   */
/*  in the loadicon.C and .RC files.                                            */
/*  loadicon local procedure declarations may appear in this file to            */
/*  ensure have been declared before being used.                              */
/*                                                                            */
/*  NOTE: If using the Dialog Editor "DLGBOX.EXE" ALL comments                */
/*          (including this) MUST be removed from this file.                  */
/******************************************************************************/


#include "filedlg.h"                    /* Fileopen dialog            */
#define ID_WINDOW   256
#define ID_WINDOW_S   "#256"

#define ID_FILE     257
#define ID_OPEN     258
#define ID_EXITPROG 261
#define ID_DUMMY    262

CHAR     szIcoDrive[FILEDLG_DRIVE_LENGTH];

CHAR     szIcoPath[FILEDLG_PATH_LENGTH];
CHAR     szIcoFile[FILEDLG_FILE_LENGTH] ={""};
CHAR     szIcoPattern[FILEDLG_PATTERN_LENGTH] ={"*.ICO"};
