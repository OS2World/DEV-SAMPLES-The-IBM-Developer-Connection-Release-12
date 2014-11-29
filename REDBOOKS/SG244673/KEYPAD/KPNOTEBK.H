/* ******************************************************************** *  
 * Copyright International Business Machines Corporation 1995, 1996     *
 * DISCLAIMER OF WARRANTIES.                                            *  
 * The following [enclosed] code is sample code created by IBM          *  
 * Corporation. This sample code is not part of any standard or IBM     *  
 * product and is provided to you solely for the purpose of assisting   *  
 * you in the development of your applications.  The code is provided   *  
 * "AS IS". IBM MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT  *  
 * NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS *  
 * FOR A PARTICULAR PURPOSE, REGARDING THE FUNCTION OR PERFORMANCE OF   *  
 * THIS CODE.  IBM shall not be liable for any damages arising out of   *  
 * your use of the sample code, even if they have been advised of the   *  
 * possibility of such damages.                                         *  
 *                                                                      *  
 * DISTRIBUTION.                                                        *  
 * This sample code can be freely distributed, copied, altered, and     *  
 * incorporated into other software, provided that it bears the above   *  
 * Copyright notice and DISCLAIMER intact.                              *  
 * -------------------------------------------------------------------- *  
 * This file is part of the source for OpenDoc parts document in:       *
 *  IBM International Technical Support Organization Bulletin (Redbook) *  
 *  "OpenDoc : An Introduction to Part Development" (SG24-4673)         *
 *                                                                      *
 * ******************************************************************** *  
 */   
#ifndef _KPNOTEBKH_
#define _KPNOTEBKH_

#define INCL_WINHELP
#define INCL_DOS
#define INCL_WINPOINTERS
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WIN
#define INCL_GPI
#define INCL_WINDIALOGS
#define INCL_WPCLASS
#define INCL_ODAPI
#define INCL_ODFRAME
#define INCL_ODPART
#include <os2.h>
//#include <pmhelp.h>

#ifndef _KEYPAD_
#include "keypad.xih"
#endif

#define WM_FOCUS_PROCESSING   WM_USER + 1

typedef struct {
        SHORT width;
        SHORT height;
} TABSIZE;
typedef TABSIZE FAR *PTABSIZE;


typedef struct _PAGE {
        PPAGEINFO ppageinfo;
        ULONG    ulPageId;
        HWND     hwndFocus;
        struct  _PAGE FAR *ppageNext;
} PAGE;
typedef PAGE FAR *PPAGE;

typedef struct {
        PFNWP   pfnwpNoteBookSubclassProc;
        HWND    hwndHelpInstance;
        PHELPTABLE phelptable;
        ULONG   ulHTsize;
        TABSIZE tabsize;
        PPAGE   ppageFirst;
        PPAGE   ppageCurrent;
        PFNWP   pfnwpPageSubclassProc;
        PVOID thisNotebook;
        ODPart * thisPart;
} BOOKINFO; // may want to add
typedef BOOKINFO FAR *PBOOKINFO;



#endif // _KPNOTEBKH_
