/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                                          ³*/
/*³ PROGRAM NAME: PMSPY                                                      ³*/
/*³ -------------                                                            ³*/
/*³  A PM program that is used to look at or 'spy' on the message queue of   ³*/
/*³  other PM applications windows.                                          ³*/
/*³                                                                          ³*/
/*³ COPYRIGHT:                                                               ³*/
/*³ ----------                                                               ³*/
/*³  Copyright (C) International Business Machines Corp., 1992               ³*/
/*³                                                                          ³*/
/*³ DISCLAIMER OF WARRANTIES:                                                ³*/
/*³ -------------------------                                                ³*/
/*³  The following [enclosed] code is sample code created by IBM Corporation.³*/
/*³  This sample code is not part of any standard IBM product and is provided³*/
/*³  to you solely for the purpose of assisting you in the development of    ³*/
/*³  your applications.  The code is provided "AS IS", without warranty of   ³*/
/*³  any kind.  IBM shall not be liable for any damages arising out of your  ³*/
/*³  use of the sample code, even if they have been advised of the           ³*/
/*³  possibility of such damages.                                            ³*/
/*³                                                                          ³*/
/*³ For details on what this program does etc., please see the PMSPY.C file. ³*/
/*³                                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ PMSPY.H                                                                  ³*/
/*³                                                                          ³*/
/*³ common OS/2 include file                                                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define INCL_BASE
#define INCL_GPI
#define INCL_WIN

#include <os2.h>                        /* the PM header file           */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Additional definitions based on OS/2 definitions                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define  NULLH   NULLHANDLE

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ PMSPY common include files                                               ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#include "pmspyhk.h"                    /* PMSPY "Hook" DLL include is base   */
#include "pmspyrc.h"                    /* PMSPY RC items                     */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ C runtime include files                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>                     /* C variable parm functions          */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ These are the PM messages internally used by PMSPY.EXE                   ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
typedef enum _SPY_EXE_MSGS
{
   PMSPY_MSG_FROM_HOOK = WM_USER,       /* message "hook" DLL sends us        */

   PMSPY_ADD_TO_LIST,                   /* add text message to SPY list       */
   PMSPY_SNAPSHOT,                      /* "snapshot" SPY list to LOG file    */
   PMSPY_STARTUP,                       /* special "Start up" processing      */

   PMSPY_NOTES_RESULT,                  /* results from NOTES dialog          */
   PMSPY_OW_RESULT,                     /* results from OBJECT window dialog  */
   PMSPY_FONT_RESULT                    /* results from FONT window dialog    */

} SPY_EXE_MSGS;

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ PMSPY version definition                                                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define SPY_VERSION      110            /* 1.10 */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Miscellaneous constants                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define IGNORE_THIS_MSG 0xFFFFFFFF

#define END_OF_ML       0xFFFFFFFF      /* End of Msg List marker             */

#define NO_MSG_SLOTS    251             /* PRIME # of MSG slots               */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Typedef's                                                                ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
typedef USHORT       GROUP,                     /* PMSPY message GROUP        */
                       IDS;                     /* STRINGTABLE index          */

typedef enum {                                  /* PMSPY frame titles         */
               tStandard,
               tSpyWho,

               tWindow,                         /* Application items          */
               tObject,
               tQueue,

               tQueueSystem                     /* System Queue               */

             } SPY_TITLES;

#define COLOR_BASE     'A'                      /* must be ASCII!!!!          */

typedef enum {                                  /* logical colors             */
               Color1        = 0,
               Color2,
               Color3,
               Color4,
               Color5,
               Color6,
               Color7,
               Color8,

               Color_Total,                     /* Total # logical colors     */

               Color_Asis,                      /* leave color 'ASIS' value   */
               Color_Error,                     /* used in reporting color errors */
               Color_Default = Color1,          /* Default color              */

               Color_First   = 1,               /* One-origin first color index */
               Color_Last    = Color_Total,     /* One-origin last  color index */

               /* Colors used for special items                               */

               Color_Notes            = Color7,
               Color_Status           = Color8,
               Color_UndefinedMsg     = Color1,
               Color_UndefinedWM_USER = Color2

             } MSG_COLOR;

typedef enum { Standard,                        /* defined MSG                */
               User,                            /* unknown WM_USER message    */
               Unknown                          /* unknown system  message    */

             } MSG_FORMAT;                      /* Msg Format                 */

typedef enum {                                  /* MSG usage values           */
               MSG_TIMES_EVER,
               MSG_TIMES_SINCE,

               MSG_TIMES_TOTAL

             } MSG_TIMES;

typedef enum {
               MP_First  = 0,                  /* Generic 1st MP format value */

               /***************************************************************
               *  32 bit entities
               ****************************************************************/
               MP_32H    = MP_First,
               MP_32P,
               MP_32U,
               MP_32S,

               /***************************************************************
               *  16 bit - 16 bit entities
               ****************************************************************/
               MP_16H_16H,
               MP_16H_16S,
               MP_16H_16U,

               MP_16S_16H,
               MP_16S_16S,
               MP_16S_16U,

               MP_16U_16H,
               MP_16U_16S,
               MP_16U_16U,

               /***************************************************************
               *  16 bit - 8 bit - 8 bit entities
               ****************************************************************/
               MP_16H_8H_8H,
               MP_16H_8U_8U,
               MP_16H_8C_8C,

               /***************************************************************
               *  8 bit - 8 bit - 16 bit entities
               ****************************************************************/
               MP_8H_8H_16H,
               MP_8U_8U_16H,
               MP_8C_8C_16H,

               /***************************************************************
               *  8 bit - 8 bit - 8 bit - 8 bit entities
               ****************************************************************/
               MP_8H_8H_8H_8H,
               MP_8U_8U_8U_8U,
               MP_8C_8C_8C_8C,

               /***************************************************************
               *  Other items
               ****************************************************************/
               MP_Total,                        /* Total # MP formats         */

               MP_EOT,                          /* End-of-Table "marker"      */

               MP_Default     = MP_32H          /* Default MP format          */

             } MP_FORMAT;                       /* MP Formats                 */

typedef struct { MSG              Msg;          /* specific Msg               */

                 PSZ              pDesc;        /* MSG description            */

                 BOOL             Include,      /* include msg?               */
                                  TriggerThaw,  /* trigger thaw?              */
                                  TriggerFreeze;/* trigger freeze?            */

                 MSG_COLOR        ClrFG;        /* logical FG color to DRAW   */

                 GROUP            GroupID;      /* GROUP this msg defined in  */

                 BOOL             SaveProcessed;/* processed during SAVE yet? */

                 ULONG            aulTimes[MSG_TIMES_TOTAL];  /* usage counts */

                 MP_FORMAT        fMP1,         /* MSG's MP1 & MP2 formats    */
                                  fMP2;

               } MSG_ITEM, * PMSG_ITEM;         /* Msg control item           */

typedef struct _MSG_DATA
               {
                 struct _MSG_DATA *pNext;       /* Ptr(Next MSG in this slot) */

                 MSG_ITEM         item;

               }   MSG_DATA,
                * PMSG_DATA;

typedef struct _MSG_SLOT
               {
                 PMSG_DATA        pFirst;       /* Ptr(1st MSG in this slot)
                                                **
                                                ** MUST be first field...
                                                */

                 USHORT           uNoMsgs;      /* # Msgs in this slot        */

                 MSG              MsgLow,       /* Range of messages in this slot */
                                  MsgHigh;

               }   MSG_SLOT,
                * PMSG_SLOT;

typedef struct {                                /* Programmer Defined Text Resource */

                 HMODULE          hModule;      /* handle of module to get PDTR from */

                 ULONG            typePDTR,     /* PDTR resource type ID      */
                                  idPDTR;       /* ID of PDTR to process      */

                 PCHAR            offPDTR;      /* OFFset of loaded Resource */

                 PCHAR            pTOF,         /* Top-Of-File  pointer */
                                  pRead;        /* working READ pointer */

               } PDTR_DATA, * PPDTR_DATA;

typedef struct {                                /* Group List control         */
                 GROUP            GroupID;      /* This GROUP's ID            */

                 PSZ              pDesc;        /* Group's Description        */

                 BOOL             Include;      /* include GROUP?             */

                 MSG_COLOR        ClrFG;        /* logical FG color to DRAW   */

               } GRP_ITEM, * PGRP_ITEM;         /* Grp control item           */

typedef struct _GRP_DATA                        /* Group List control         */
               {
                 struct _GRP_DATA *pNext;       /* Locates next GROUP         */

                 GRP_ITEM          item;        /* Other Group Data           */

               } GRP_DATA, * PGRP_DATA;         /* Grp control item           */

typedef struct {
                LONG lClrValue;              /* binary CLR value */
                PSZ  pszClrName;             /* ascii  CLR value */
               } XCOLOR, * PXCOLOR;

typedef struct {
                USHORT    iExtColor;      /* ExternalColorTranslation index */
                MSG_COLOR pbValue;        /* value to use if Pushbutton selected */

               } LOGCOLOR, * PLOGCOLOR;

typedef LOGCOLOR XLOGCOLOR[ Color_Total ];

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ FONT dialog Initialization/Result data                                   ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
typedef CHAR       FONTNAME[FACESIZE + 8];
typedef FONTNAME *PFONTNAME;

typedef struct _FONT_DATA
{
  /* defines "who" handles the FONT dialog results */

  HWND                hwndNotify;            /* who to contact with results */
  MSG                 uNotifyMsg;            /* PM msg saying "here's results" */

  BOOL                fUseData;              /* use dialog data? (for contact!) */

  PFONTNAME           pfnList,               /* In:  list with all fixed FONTS */
                      pfnCurrent,            /* In:  currently selected FONT */

                      pfnSelected;           /* Out: selected FONT */

} FONT_DATA, *PFONT_DATA;

typedef struct {
                 /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
                 /*³ MSG support data                                        ³*/
                 /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
                 USHORT       TotalMsgs,
                              usMaxMsgNameSize;

                 MSG_SLOT     Msgs[NO_MSG_SLOTS];   /* MSG slot table */

                   /* "Process All Msgs" support data */

                 USHORT       ProcessAllMsgs_Slot;
                 PMSG_DATA    ProcessAllMsgs_pData;

                 /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
                 /*³ GROUP support data                                      ³*/
                 /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
                 GROUP        NextGroupID;  /* next GROUP ID */

                 PGRP_DATA    GroupHead,    /* locates GROUP table */
                              GroupTail;

                 USHORT       usMaxGrpNameSize;

                   /* "Process All Groups" support data */

                 PGRP_DATA    ProcessAllGroups_pGroup;

                 GROUP        ProcessAllMsgsInGroup_ID;

                 BOOL         ProcessAllMsgsInGroup_First,
                              ProcessAllGroups_First;

                 /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
                 /*³ COLOR support data                                      ³*/
                 /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
                 XLOGCOLOR    LogicalColorTranslation;

                 /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
                 /*³ Processing data                                         ³*/
                 /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
                 PFONTNAME     pfnLB;             /* FONT for dialog LB's     */

               } SPY_DATA, * PSPY_DATA;

/* *** Spy List Mode **** */
typedef enum { SLM_THAWED,                    /* "thawed" states */
               SLM_FROZEN                     /* "frozen" states */
             } SLMSTATE;

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ NOTES dialog Initialization/Result data                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

#define L_NOTES_TEXT  (80 + 1)               /* # text chars + NULL */

typedef struct _NOTES_DATA
{
  /* defines "who" handles the NOTES dialog results */

  HWND                hwndNotify;            /* who to contact about NOTES */
  MSG                 uNotifyMsg;            /* PM msg saying "here's NOTES" */

  BOOL                fUseData;              /* use dialog data? (for contact!) */

  CHAR                szText[L_NOTES_TEXT];  /* note text */

} NOTES_DATA, *PNOTES_DATA;

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ OBJECT Window dialog Initialization/Result data                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
typedef struct _OW_DATA
{
  /* defines "who" handles the dialog results */

  HWND                hwndNotify;     /* In:  who to contact about selection  */
  MSG                 uNotifyMsg;     /* In:  PM msg saying "here's selection"*/

  BOOL                fDESKTOP,       /* In/Out: display DESKTOP windows?     */
                      fOBJECT;        /* In/Out: display OBJECT  windows?     */

  PFONTNAME           pfnCurrent;     /* In:  currently selected listbox FONT */
  HMQ                 hmqSPY;         /* In:  HMQ of this agent               */

  BOOL                fUseData;       /* Out: use dialog data? (for contact!) */

  HWND                hwndOW;         /* Out: OBJECT's HWND IFF fUseData=TRUE */
  HMQ                 hmqOW;          /* Out: OBJECT's HMQ  IFF fUseData=TRUE */

  BOOL                fSpyHWND,       /* Out: spy HWND (TRUE) or HMQ (FALSE)  */
                      fHwndIsOBJECT;  /* Out: selected HWND is OBJECT window  */

  PSZ                 pszType;        /* DLG: type "wording" for this window  */
  BOOL                fIsOBJECT;      /* DLG: processing OBJECT windows       */

} OW_DATA, *POW_DATA;

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ MSG Decompositon dialog Initialization data                              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
typedef struct _MD_DATA
{
  PMSG_ITEM   pMsg;                  /* MSG data                              */

  QMSG        qmsgItem;              /* specific MSG's data                   */

} MD_DATA, *PMD_DATA;

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Windown and Dialog Procedures                                            ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
FNWP SpyWindowProc;

FNWP FilterDlgProc;
FNWP FontDlgProc;
FNWP GroupDlgProc;
FNWP MsgDlgProc;
FNWP NotesDlgProc;
FNWP ObjectDlgProc;
FNWP SpyLogoDlgProc;

/* PMSPYIPF.C routines                                                        */
extern HWND InitializeIPF( HWND   hwndFrame,
                           USHORT uActionBarID,
                           PSZ    pszHelpLibs,
                           PSZ    pszRefTitle);

extern BOOL HandleIPF(     HWND     hWnd,         /* current window */
                           MSG      Msg,          /* current message */
                           MPARAM   mp1,          /* current message parm #1 */
                           MPARAM   mp2,          /* current message parm #2 */
                           PMRESULT pmrValue);    /* where to "set" result */

/* PMSPYMSG.C routines                                                        */
extern VOID InitializeSpyData(PSPY_DATA pSpy);
extern VOID ResetSpyData(PSPY_DATA pSpy);
extern VOID InitializeMsg(PSPY_DATA pSpy);
extern USHORT HashMsg(MSG msg);
extern VOID AddMsg(PSPY_DATA pSpy, PMSG_ITEM pMsg);
extern VOID IncludeAllMsgs(PSPY_DATA pSpy, BOOL Include, MSG_COLOR Color);
extern VOID IncludeMsg(PSPY_DATA pSpy, MSG Msg, BOOL Include, MSG_COLOR Color);
extern PMSG_ITEM FindMsg(PSPY_DATA pSpy, MSG Msg);
extern PMSG_ITEM FindMsgName(PSPY_DATA pSpy, PSZ pName);
extern PMSG_ITEM ProcessFirstMsg(PSPY_DATA pSpy);
extern PMSG_ITEM ProcessNextMsg (PSPY_DATA pSpy);
extern BOOL DeleteMsg(PSPY_DATA pSpy, MSG Msg);
extern VOID DeleteAllMsgs(PSPY_DATA pSpy);

/* GROUP support routines                                                     */
extern VOID InitializeGroup(PSPY_DATA pSpy);
extern GROUP AddGroup(PSPY_DATA pSpy, PGRP_ITEM pGrp);
extern VOID IncludeGroup(PSPY_DATA pSpy, PGRP_ITEM pGrp, BOOL Include, MSG_COLOR Color );
extern PGRP_ITEM FindGroup(PSPY_DATA pSpy, GROUP GroupID);
extern PGRP_ITEM FindGroupName(PSPY_DATA pSpy, PSZ pName);
extern PMSG_ITEM ProcessFirstMsgInGroup(PSPY_DATA pSpy, GROUP     GroupID);
extern PMSG_ITEM ProcessNextMsgInGroup(PSPY_DATA pSpy);
extern PGRP_ITEM ProcessFirstGroup(PSPY_DATA pSpy);
extern PGRP_ITEM ProcessNextGroup(PSPY_DATA pSpy);
extern BOOL      DeleteGroup(PSPY_DATA pSpy, GROUP GroupID);
extern VOID      DeleteAllGroups(PSPY_DATA pSpy);

/* PMSPYUTL.C routines                                                        */
extern ULONG      MsgDisplay(HWND hWnd, PSZ strCap, PSZ strFormat, USHORT mb_id, ULONG style, ...);
extern VOID       MsgDisplayShouldEnableHELP(VOID);

extern VOID       UpdateSpyTitle(HWND hwndFrame, SPY_TITLES whichTitle, ...);

extern BOOL       ConvertToNumber(PSZ pAscii, PUSHORT pNumber, USHORT lMin, USHORT lMax);

extern PVOID      AllocateMemory(size_t SizeWanted);
extern VOID       FreeMemory    (PVOID  p);

extern PPDTR_DATA OpenPDTR (PPDTR_DATA pData);
extern BOOL       EofPDTR  (PPDTR_DATA pData);
extern PSZ        GetsPDTR (PPDTR_DATA pData, PSZ pString, USHORT lString);
extern int        ClosePDTR(PPDTR_DATA pData);

typedef struct { PSZ    *ppWord;      /* pointer to PSZ for this WORD */
                 USHORT  vWord;       /* Value if this WORD found */
               } WORD_LIST, *PWORD_LIST;

extern USHORT FindWordInList(PSZ pWord, PWORD_LIST pWordList);
extern PSZ    TranslateClassName(PSZ pszClassName);
extern BOOL   CenterDialog(HWND hwndDialog);            /* center DLG window  */
extern VOID   SetListboxFont(HWND hwndLB, PSZ  pszFont);/* Set new LB font    */

/* PMSPYPRO.C routines                                                        */

BOOL OpenProfile(HWND hwndCur, HWND hwndFrame, PSPY_DATA pSpy, PSZ szFile);
BOOL SaveProfile(HWND hwnd, PSPY_DATA pSpy, PSZ szFile);

/* PMSPYFIL.C routines                                                        */
extern VOID      SetColors(HWND hwnd, PSPY_DATA pSpyData);
extern MSG_COLOR GetColor (HWND hwnd, PSPY_DATA pSpyData);

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Variable declarations                                                    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/* determine SCOPE to use on global variables */

#if defined(DEFINE_VARIABLES)
   #define xSCOPE
   #define iGLOBAL(init) = init         /*  specify initialization            */
#else
   #define xSCOPE extern
   #define iGLOBAL(init)                /*  do not specify initialization     */
#endif

#define genXCT(color)    { color, #color }

xSCOPE XCOLOR  ExternalColorTranslation[]

#if defined(DEFINE_VARIABLES)

= {
   /* These are the 'default' colors */

   genXCT(SYSCLR_WINDOWTEXT             ),         /* Color #1 */
   genXCT(SYSCLR_ACTIVETITLE            ),         /* Color #2 */
   genXCT(SYSCLR_WINDOWSTATICTEXT       ),         /* Color #3 */
   genXCT(   CLR_RED                    ),         /* Color #4 */
   genXCT(   CLR_PINK                   ),         /* Color #5 */
   genXCT(   CLR_DARKGREEN              ),         /* Color #6 */
   genXCT(   CLR_DARKCYAN               ),         /* Color #7 */
   genXCT(   CLR_BLUE                   ),         /* Color #8 */

   /* These are the standard 'SYSCLR' colors */

   genXCT(SYSCLR_ACTIVEBORDER           ),
   genXCT(SYSCLR_ACTIVETITLETEXT        ),
   genXCT(SYSCLR_ACTIVETITLETEXTBGND    ),
   genXCT(SYSCLR_APPWORKSPACE           ),
   genXCT(SYSCLR_BACKGROUND             ),
   genXCT(SYSCLR_BUTTONDARK             ),
   genXCT(SYSCLR_BUTTONDEFAULT          ),
   genXCT(SYSCLR_BUTTONLIGHT            ),
   genXCT(SYSCLR_BUTTONMIDDLE           ),
   genXCT(SYSCLR_DIALOGBACKGROUND       ),
   genXCT(SYSCLR_HELPBACKGROUND         ),
   genXCT(SYSCLR_HELPHILITE             ),
   genXCT(SYSCLR_HELPTEXT               ),
   genXCT(SYSCLR_HILITEBACKGROUND       ),
   genXCT(SYSCLR_HILITEFOREGROUND       ),
   genXCT(SYSCLR_ICONTEXT               ),
   genXCT(SYSCLR_INACTIVEBORDER         ),
   genXCT(SYSCLR_INACTIVETITLE          ),
   genXCT(SYSCLR_INACTIVETITLETEXT      ),
   genXCT(SYSCLR_INACTIVETITLETEXTBGND  ),
   genXCT(SYSCLR_MENU                   ),
   genXCT(SYSCLR_MENUTEXT               ),
   genXCT(SYSCLR_OUTPUTTEXT             ),
   genXCT(SYSCLR_SCROLLBAR              ),
   genXCT(SYSCLR_SHADOW                 ),
   genXCT(SYSCLR_TITLEBOTTOM            ),
   genXCT(SYSCLR_TITLETEXT              ),
   genXCT(SYSCLR_WINDOW                 ),
   genXCT(SYSCLR_WINDOWFRAME            ),

   /* These are the standard 'CLR' colors */

   genXCT(   CLR_BACKGROUND             ),
   genXCT(   CLR_BROWN                  ),
   genXCT(   CLR_BLACK                  ),
   genXCT(   CLR_DEFAULT                ),
   genXCT(   CLR_DARKBLUE               ),
   genXCT(   CLR_DARKCYAN               ),
   genXCT(   CLR_DARKGRAY               ),
   genXCT(   CLR_DARKGREEN              ),
   genXCT(   CLR_DARKPINK               ),
   genXCT(   CLR_DARKRED                ),
   genXCT(   CLR_NEUTRAL                ),
   genXCT(   CLR_PALEGRAY               ),
   genXCT(   CLR_WHITE                  ),
   genXCT(   CLR_YELLOW                 ),

   /* end-of-table marker */

   {    CLR_ERROR             , NULL }
  }
#endif

;

xSCOPE HMODULE  hmodNonNLS    iGLOBAL( 0 ); /* for Non-NLS resources */
xSCOPE HMODULE  hmodNLS       iGLOBAL( 0 ); /* for     NLS resources */

xSCOPE SWCNTRL  swcntrl;
xSCOPE HSWITCH  hSwitch       iGLOBAL( NULLH ); /* switch list handle */

xSCOPE SPYEE    SpyInstance          iGLOBAL( SPY_BAD_SPYEE ); /* Instance # */

xSCOPE PSZ      Strings[IDS_TOTAL];   /* pointers to NLS STRINGs */
xSCOPE PSZ      Controls[CTL_TOTAL];  /* pointers to non-NLS control STRINGs */

xSCOPE SLMSTATE slmState iGLOBAL(SLM_THAWED);

xSCOPE FILE     *pLogFile iGLOBAL(NULL);
xSCOPE BOOL     bCurrentlyLogging iGLOBAL(BOOL_FALSE);

/* Command line support */

xSCOPE char     *(*ArgV);
xSCOPE int      ArgC;

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                                          ³*/
/*³ Macros to implement a loop that executes only ONCE with the ability      ³*/
/*³ to exit the loop at any point                                            ³*/
/*³                                                                          ³*/
/*³ Use:                                                                     ³*/
/*³                                                                          ³*/
/*³       BEGIN_DO_ONCE_BLOCK                                                ³*/
/*³       |                                                                  ³*/
/*³       |  <code to execute once>                                          ³*/
/*³       |                                                                  ³*/
/*³       |  if ( <error detected> )                                         ³*/
/*³       |    EXIT_DO_ONCE_BLOCK; -----+                                    ³*/
/*³       |                             |                                    ³*/
/*³       END_DO_ONCE_BLOCK;            |                                    ³*/
/*³                                     |                                    ³*/
/*³       <-----------------------------+                                    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

#define BEGIN_DO_ONCE_BLOCK  do {

#define   END_DO_ONCE_BLOCK  } while(0)

#define  EXIT_DO_ONCE_BLOCK  break
