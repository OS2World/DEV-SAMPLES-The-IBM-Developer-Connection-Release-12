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
const ODType kPart            = "KeyPadPart:keypad"; // Used in kODPropPart
const ODType kKind            = "KeyPadPart Kind";   // Our part kind
const ODType kKindDisplayName = "KeyPadPart (ITSO)"; // Used to display our part kind
const ODType kPartHandlerName = "KeyPadPartHandler";
const ODType kCategory        = "KeyPadPart (Cat)";

#define FIXED2LONG(f) (((f) + 0x8000) >> 16)

#define KPBUTBASE 4000
#define KPBUT_0 KPBUTBASE
#define KPBUT_1 KPBUT_0+1
#define KPBUT_2 KPBUT_1+1
#define KPBUT_3 KPBUT_2+1
#define KPBUT_4 KPBUT_3+1
#define KPBUT_5 KPBUT_4+1
#define KPBUT_6 KPBUT_5+1
#define KPBUT_7 KPBUT_6+1
#define KPBUT_8 KPBUT_7+1
#define KPBUT_9 KPBUT_8+1
#define KPBUT_10 KPBUT_9+1
#define KPBUT_11 KPBUT_10+1

#define MAXDISPLEN 12
#define KP_DELAY 500

#define  KP_BASE          (MENUID_ACTIVEPART_USER+500)
#define  KP_POPUP_MENU    KP_BASE
#define  KP_MENU          (KP_BASE + 1)
#define  IDM_CLEAR        (KP_BASE + 2)
#define  IDM_ABOUT        (KP_BASE + 3)
#define  IDM_TRACE        (KP_BASE + 4)
#define  IDM_REVERSE      (KP_BASE + 5)
#define  IDM_SEPARATOR    (KP_BASE + 6)


PFNWP  pButClass;
FILE * stream;
somTD_SOMOutCharRoutine * SavedSOMOutCharRoutine;
//-------------------------------------------------------------------------
// Function Prototypes
//-------------------------------------------------------------------------

MRESULT EXPENTRY SubclassWndProc(HWND hwnd, LONG msg, MPARAM mp1, MPARAM mp2);
void writeSU(ODStorageUnit *aSU, char *text);   
void SetOutlineFont(HPS hps, short height);
