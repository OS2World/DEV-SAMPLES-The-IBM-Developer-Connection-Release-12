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
/*³ PMSPYDEF.C                                                               ³*/
/*³                                                                          ³*/
/*³ Program for building the default PMSPY profile                           ³*/
/*³                                                                          ³*/
/*³                                                                          ³*/
/*³ - A table defines the custom GROUPs and MSGs.                            ³*/
/*³                                                                          ³*/
/*³   >>>> adjust this with your application's MSGs                          ³*/
/*³                                                                          ³*/
/*³ - This program processes the table, formats PROfile statements, and      ³*/
/*³   writes it to STDOUT                                                    ³*/
/*³                                                                          ³*/
/*³ - Just use simple I/O redirection to write to disk file                  ³*/
/*³                                                                          ³*/
/*³   example: PMSPYDEF >PMSPYDEF.PRO                                        ³*/
/*³                                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Standard C #include's                                                    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#include <stdio.h>

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Include your application's #include's here                               ³*/
/*³                                                                          ³*/
/*³ -- it is needed to provide the binary values of your application MSGs    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define INCL_GPI
#define INCL_WIN

#include <os2.h>                        /* the PM header file           */

#include "pmspyMM.h"                    /* MMPM/2 header file           */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Define output strings used in custom profile                             ³*/
/*³                                                                          ³*/
/*³ -- adjust these to meet your application's requirements                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ APPlication name - adjust as necessary                                 ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define APP_NAME   "PMSPY"

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ stderr messages - these NEVER get re-directed                          ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define PGM_TITLE  "Generate Default PMSPY Message Profile\n\n"

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Determine OS/2 2.x Toolkit being used (check for release specific msgs)³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#ifdef WM_MOUSEMAP                      /* OS/2 2.1 or later */
#define PRO_TKT    "2.1"
#else
#define PRO_TKT    "2.0"
#endif

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ PROfile data formating strings                                         ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define PRO_TITLE  "PMSPY Default Profile - Created " __DATE__
#define PRO_TKIT   "Built using the OS/2 " PRO_TKT " Toolkit"

#define PRO_GROUP  "\nGroup   %-30.30s %d\n\n"
#define PRO_MSG      " Define %-30.30s 0x%-4.4X\n"

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Note: you can have each GROUP and/or MSG name automatically prefixed     ³*/
  /*³       with text so that is 'sorts' to the top of the Group and/or Message³*/
  /*³       Selection lists.  This is particularly nice becauses it gets YOUR  ³*/
  /*³       messages in front of all those WM_* messages!                      ³*/
  /*³                                                                          ³*/
  /*³       To enable automatic prefixing, change the following                ³*/
  /*³       #undef to a #define then adjust the prefix string value, if desired³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

#undef   Want_PREFIX    /* <=== change #undef to #define to enable PREFIXes */

#if defined(Want_PREFIX)
   #define PRO_PREFIX_GROUP "@"
   #define PRO_PREFIX_MSG   "*"
#else
   #define PRO_PREFIX_GROUP
   #define PRO_PREFIX_MSG
#endif

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Define data structure and macros used to 'define' Custom PROfile data    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
typedef enum { isGROUP,              /* this item starts a new GROUP     */
               isMSG,                /* this item defines a MSG          */
               isTEXT,               /* this item defines some TEXT      */
               isCOMM,               /* this item defines a    COMMENT   */
               isSKIP,               /* this item defines SKIP 'n' lines */
               isEOL } OPCODE;       /* this item end the PROfile data   */

#define defGROUP(group,color) {isGROUP, PRO_PREFIX_GROUP #group, color }
#define defMSG(msg)           {isMSG  , PRO_PREFIX_MSG   #msg  , msg   }
#define defTEXT(text)         {isTEXT , text                   , 0     }
#define defCOMM(text)         {isCOMM , text                   , 0     }
#define defSKIP(count)        {isSKIP , NULL                   , count }
#define defEOL                {isEOL  , NULL                   , 0     }

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Define Custom PROfile data                                               ³*/
/*³                                                                          ³*/
/*³ Rules:                                                                   ³*/
/*³ ------                                                                   ³*/
/*³ - the GROUP name must be one contiguous string of letters, etc.          ³*/
/*³   NO imbedded blanks are allowed.  use dashes or underscores to          ³*/
/*³   "tie together" words into a single GROUP name                          ³*/
/*³                                                                          ³*/
/*³ - the number specified on the GROUP definition is the COLOR INDEX to be  ³*/
/*³   used for all MSGs in this GROUP.  Only 1..8 are valid (no error        ³*/
/*³   checking is made!)                                                     ³*/
/*³                                                                          ³*/
/*³ - the MSG name must be specified EXACTLY as it is #define in your        ³*/
/*³   #include file (since this is how it's binary value is generated!)      ³*/
/*³                                                                          ³*/
/*³ Standard C #include's                                                    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

struct {
         OPCODE   Op;        /* defines what this item is (GROUP, MSG, etc) */
         PSZ      pszName;   /* name of GROUP or MSG, TEXT string           */
         USHORT   usValue;   /* Color for GROUP, binary MSG value           */
       } sItems[] =
{
  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Define front matter                                                    ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  defCOMM(PRO_TITLE),
  defSKIP(1),
  defCOMM(PRO_TKIT),
  defSKIP(1),

  defCOMM("Color Definitions"),
  defSKIP(1),
  defTEXT("Color   1 SYSCLR_WINDOWTEXT"),
  defTEXT("Color   2 SYSCLR_ACTIVETITLE"),
  defTEXT("Color   3 SYSCLR_WINDOWSTATICTEXT"),
  defTEXT("Color   4 CLR_RED"),
  defTEXT("Color   5 CLR_PINK"),
  defTEXT("Color   6 CLR_DARKGREEN"),
  defTEXT("Color   7 CLR_DARKCYAN"),
  defTEXT("Color   8 CLR_BLUE"),
  defSKIP(1),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(MouseMovement, 1),           /* misc */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(WM_HITTEST),                 /* 0x000c */
    defMSG(WM_CONTROLPOINTER),          /* 0x0038 */
    defMSG(WM_MOUSEMOVE),               /* 0x0070 */

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(StandardWindow, 1),          /* 0x0000 - 0x002f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(WM_NULL),
    defMSG(WM_CREATE),
    defMSG(WM_DESTROY),
    defMSG(WM_ENABLE),
    defMSG(WM_SHOW),
    defMSG(WM_MOVE),
    defMSG(WM_SIZE),
    defMSG(WM_ADJUSTWINDOWPOS),
    defMSG(WM_CALCVALIDRECTS),
    defMSG(WM_SETWINDOWPARAMS),
    defMSG(WM_QUERYWINDOWPARAMS),
    defMSG(WM_ACTIVATE),
    defMSG(WM_SETFOCUS),
    defMSG(WM_SETSELECTION),
    defMSG(WM_COMMAND),
    defMSG(WM_SYSCOMMAND),
    defMSG(WM_HELP),
    defMSG(WM_PAINT),
    defMSG(WM_TIMER),
    defMSG(WM_SEM1),
    defMSG(WM_SEM2),
    defMSG(WM_SEM3),
    defMSG(WM_SEM4),
    defMSG(WM_CLOSE),
    defMSG(WM_QUIT),
    defMSG(WM_SYSCOLORCHANGE),
    defMSG(WM_SYSVALUECHANGED),
    defMSG(WM_APPTERMINATENOTIFY),
    defMSG(WM_PRESPARAMCHANGED),
    defMSG(WM_QUERYCONVERTPOS),         /* 0x00b0 */

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(LanguageSupport, 1),         /* 0x0011 - 0x001f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(WM_PPAINT),
    defMSG(WM_PSETFOCUS),
    defMSG(WM_PSYSCOLORCHANGE),
    defMSG(WM_PSIZE),
    defMSG(WM_PACTIVATE),
    defMSG(WM_PCONTROL),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(Control, 1),                 /* 0x0030 - 0x003f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(WM_CONTROL),
    defMSG(WM_VSCROLL),
    defMSG(WM_HSCROLL),
    defMSG(WM_INITMENU),
    defMSG(WM_MENUSELECT),
    defMSG(WM_MENUEND),
    defMSG(WM_DRAWITEM),
    defMSG(WM_MEASUREITEM),
    defMSG(WM_QUERYDLGCODE),
    defMSG(WM_INITDLG),
    defMSG(WM_SUBSTITUTESTRING),
    defMSG(WM_MATCHMNEMONIC),
    defMSG(WM_SAVEAPPLICATION),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(Frame, 1),                   /* 0x0040 - 0x005f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(WM_FLASHWINDOW),
    defMSG(WM_FORMATFRAME),
    defMSG(WM_UPDATEFRAME),
    defMSG(WM_FOCUSCHANGE),
    defMSG(WM_SETBORDERSIZE),
    defMSG(WM_TRACKFRAME),
    defMSG(WM_MINMAXFRAME),
    defMSG(WM_SETICON),
    defMSG(WM_QUERYICON),
    defMSG(WM_SETACCELTABLE),
    defMSG(WM_QUERYACCELTABLE),
    defMSG(WM_TRANSLATEACCEL),
    defMSG(WM_QUERYTRACKINFO),
    defMSG(WM_QUERYBORDERSIZE),
    defMSG(WM_NEXTMENU),
    defMSG(WM_ERASEBACKGROUND),
    defMSG(WM_QUERYFRAMEINFO),
    defMSG(WM_QUERYFOCUSCHAIN),
    defMSG(WM_OWNERPOSCHANGE),
    defMSG(WM_CALCFRAMERECT),
    defMSG(WM_WINDOWPOSCHANGED),
    defMSG(WM_QUERYFRAMECTLCOUNT),
    defMSG(WM_QUERYHELPINFO),
    defMSG(WM_SETHELPINFO),
    defMSG(WM_ERROR),
    defMSG(WM_REALIZEPALETTE),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(Clipboard, 1),               /* 0x0060 - 0x006f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(WM_RENDERFMT),
    defMSG(WM_RENDERALLFMTS),
    defMSG(WM_DESTROYCLIPBOARD),
    defMSG(WM_PAINTCLIPBOARD),
    defMSG(WM_SIZECLIPBOARD),
    defMSG(WM_HSCROLLCLIPBOARD),
    defMSG(WM_VSCROLLCLIPBOARD),
    defMSG(WM_DRAWCLIPBOARD),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(MouseButtons, 1),            /* 0x0071 - 0x0079 */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(WM_BUTTON1DOWN),
    defMSG(WM_BUTTON1UP),
    defMSG(WM_BUTTON1DBLCLK),

    defMSG(WM_BUTTON2DOWN),
    defMSG(WM_BUTTON2UP),
    defMSG(WM_BUTTON2DBLCLK),

    defMSG(WM_BUTTON3DOWN),
    defMSG(WM_BUTTON3UP),
    defMSG(WM_BUTTON3DBLCLK),

#ifdef WM_MOUSEMAP                      /* OS/2 2.1 or later */
    defMSG(WM_MOUSEMAP),
#endif

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(CharacterIO, 1),             /* 0x007a - 0x007b */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(WM_CHAR),
    defMSG(WM_VIOCHAR),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(Journal, 1),                 /* 0x007c */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(WM_JOURNALNOTIFY),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(DynamicDataExchange, 1),     /* 0x00a0 - 0x00af */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(WM_DDE_INITIATE),
    defMSG(WM_DDE_REQUEST),
    defMSG(WM_DDE_ACK),
    defMSG(WM_DDE_DATA),
    defMSG(WM_DDE_ADVISE),
    defMSG(WM_DDE_UNADVISE),
    defMSG(WM_DDE_POKE),
    defMSG(WM_DDE_EXECUTE),
    defMSG(WM_DDE_TERMINATE),
    defMSG(WM_DDE_INITIATEACK),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(Static Control, 1),          /* 0x0100 - 0x010f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(SM_SETHANDLE),
    defMSG(SM_QUERYHANDLE),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(Button, 1),                  /* 0x0120 - 0x012f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(BM_CLICK),
    defMSG(BM_QUERYCHECKINDEX),
    defMSG(BM_QUERYHILITE),
    defMSG(BM_SETHILITE),
    defMSG(BM_QUERYCHECK),
    defMSG(BM_SETCHECK),
    defMSG(BM_SETDEFAULT),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(EntryField, 1),              /* 0x0140 - 0x014f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(EM_QUERYCHANGED),
    defMSG(EM_QUERYSEL),
    defMSG(EM_SETSEL),
    defMSG(EM_SETTEXTLIMIT),
    defMSG(EM_CUT),
    defMSG(EM_COPY),
    defMSG(EM_CLEAR),
    defMSG(EM_PASTE),
    defMSG(EM_QUERYFIRSTCHAR),
    defMSG(EM_SETFIRSTCHAR),
    defMSG(EM_QUERYREADONLY),
    defMSG(EM_SETREADONLY),
    defMSG(EM_SETINSERTMODE),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(ListBox, 1),                 /* 0x0160 - 0x016f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(LM_QUERYITEMCOUNT),
    defMSG(LM_INSERTITEM),
    defMSG(LM_SETTOPINDEX),
    defMSG(LM_DELETEITEM),
    defMSG(LM_SELECTITEM),
    defMSG(LM_QUERYSELECTION),
    defMSG(LM_SETITEMTEXT),
    defMSG(LM_QUERYITEMTEXTLENGTH),
    defMSG(LM_QUERYITEMTEXT),
    defMSG(LM_SETITEMHANDLE),
    defMSG(LM_QUERYITEMHANDLE),
    defMSG(LM_SEARCHSTRING),
    defMSG(LM_SETITEMHEIGHT),
    defMSG(LM_QUERYTOPINDEX),
    defMSG(LM_DELETEALL),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(ComboBox, 1),                /* 0x0170 - 0x017f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(CBM_SHOWLIST),
    defMSG(CBM_HILITE),
    defMSG(CBM_ISLISTSHOWING),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(Menu, 1),                    /* 0x0180 - 0x019f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(MM_INSERTITEM),
    defMSG(MM_DELETEITEM),
    defMSG(MM_QUERYITEM),
    defMSG(MM_SETITEM),
    defMSG(MM_QUERYITEMCOUNT),
    defMSG(MM_STARTMENUMODE),
    defMSG(MM_ENDMENUMODE),
    defMSG(MM_REMOVEITEM),
    defMSG(MM_SELECTITEM),
    defMSG(MM_QUERYSELITEMID),
    defMSG(MM_QUERYITEMTEXT),
    defMSG(MM_QUERYITEMTEXTLENGTH),
    defMSG(MM_SETITEMHANDLE),
    defMSG(MM_SETITEMTEXT),
    defMSG(MM_ITEMPOSITIONFROMID),
    defMSG(MM_ITEMIDFROMPOSITION),
    defMSG(MM_QUERYITEMATTR),
    defMSG(MM_SETITEMATTR),
    defMSG(MM_ISITEMVALID),
    defMSG(MM_QUERYITEMRECT),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(ScrollBar, 1),               /* 0x01a0 - 0x01af */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(SBM_SETSCROLLBAR),
    defMSG(SBM_SETPOS),
    defMSG(SBM_QUERYPOS),
    defMSG(SBM_QUERYRANGE),
    defMSG(SBM_SETTHUMBSIZE),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(MultiLineEdit, 1),           /* 0x01b0 - 0x01df */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(MLM_SETTEXTLIMIT),
    defMSG(MLM_QUERYTEXTLIMIT),
    defMSG(MLM_SETFORMATRECT),
    defMSG(MLM_QUERYFORMATRECT),
    defMSG(MLM_SETWRAP),
    defMSG(MLM_QUERYWRAP),
    defMSG(MLM_SETTABSTOP),
    defMSG(MLM_QUERYTABSTOP),
    defMSG(MLM_SETREADONLY),
    defMSG(MLM_QUERYREADONLY),
    defMSG(MLM_QUERYCHANGED),
    defMSG(MLM_SETCHANGED),
    defMSG(MLM_QUERYLINECOUNT),
    defMSG(MLM_CHARFROMLINE),
    defMSG(MLM_LINEFROMCHAR),
    defMSG(MLM_QUERYLINELENGTH),
    defMSG(MLM_QUERYTEXTLENGTH),
    defMSG(MLM_FORMAT),
    defMSG(MLM_SETIMPORTEXPORT),
    defMSG(MLM_IMPORT),
    defMSG(MLM_EXPORT),
    defMSG(MLM_DELETE),
    defMSG(MLM_QUERYFORMATLINELENGTH),
    defMSG(MLM_QUERYFORMATTEXTLENGTH),
    defMSG(MLM_INSERT),
    defMSG(MLM_SETSEL),
    defMSG(MLM_QUERYSEL),
    defMSG(MLM_QUERYSELTEXT),
    defMSG(MLM_QUERYUNDO),
    defMSG(MLM_UNDO),
    defMSG(MLM_RESETUNDO),
    defMSG(MLM_QUERYFONT),
    defMSG(MLM_SETFONT),
    defMSG(MLM_SETTEXTCOLOR),
    defMSG(MLM_QUERYTEXTCOLOR),
    defMSG(MLM_SETBACKCOLOR),
    defMSG(MLM_QUERYBACKCOLOR),
    defMSG(MLM_QUERYFIRSTCHAR),
    defMSG(MLM_SETFIRSTCHAR),
    defMSG(MLM_CUT),
    defMSG(MLM_COPY),
    defMSG(MLM_PASTE),
    defMSG(MLM_CLEAR),
    defMSG(MLM_ENABLEREFRESH),
    defMSG(MLM_DISABLEREFRESH),
    defMSG(MLM_SEARCH),
    defMSG(MLM_QUERYIMPORTEXPORT),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(TitleBar, 1),                /* 0x01e0 - 0x01ef */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(TBM_SETHILITE),
    defMSG(TBM_QUERYHILITE),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(SpinButton, 1),              /* 0x0200 - 0x020f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(SPBM_OVERRIDESETLIMITS),
    defMSG(SPBM_QUERYLIMITS),
    defMSG(SPBM_SETTEXTLIMIT),
    defMSG(SPBM_SPINUP),
    defMSG(SPBM_SPINDOWN),
    defMSG(SPBM_QUERYVALUE),
    defMSG(SPBN_UPARROW),
    defMSG(SPBN_DOWNARROW),
    defMSG(SPBN_ENDSPIN),
    defMSG(SPBN_CHANGE),
    defMSG(SPBN_SETFOCUS),
    defMSG(SPBN_KILLFOCUS),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(Help/IPF, 1),                /* 0x0220 - 0x023f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(HM_DISMISS_WINDOW),
    defMSG(HM_DISPLAY_HELP),
    defMSG(HM_EXT_HELP),
    defMSG(HM_SET_ACTIVE_WINDOW),
    defMSG(HM_LOAD_HELP_TABLE),
    defMSG(HM_CREATE_HELP_TABLE),
    defMSG(HM_SET_HELP_WINDOW_TITLE),
    defMSG(HM_SET_SHOW_PANEL_ID),
    defMSG(HM_REPLACE_HELP_FOR_HELP),
    defMSG(HM_HELP_INDEX),
    defMSG(HM_HELP_CONTENTS),
    defMSG(HM_KEYS_HELP),
    defMSG(HM_SET_HELP_LIBRARY_NAME),
    defMSG(HM_SET_OBJCOM_WINDOW),
    defMSG(HM_UPDATE_OBJCOM_WINDOW_CHAIN),
    defMSG(HM_QUERY_DDF_DATA),
    defMSG(HM_INVALIDATE_DDF_DATA),
    defMSG(HM_QUERY),
    defMSG(HM_SET_COVERPAGE_SIZE),
    defMSG(HM_ERROR),
    defMSG(HM_HELPSUBITEM_NOT_FOUND),
    defMSG(HM_QUERY_KEYS_HELP),
    defMSG(HM_TUTORIAL),
    defMSG(HM_EXT_HELP_UNDEFINED),
    defMSG(HM_ACTIONBAR_COMMAND),
    defMSG(HM_INFORM),
    defMSG(HM_NOTIFY),
    defMSG(HM_SET_USERDATA),
    defMSG(HM_CONTROL),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(DragDrop, 1),                /* 0x0310 - 0x032f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(DM_DROP),
    defMSG(DM_DRAGOVER),
    defMSG(DM_DRAGLEAVE),
    defMSG(DM_DROPHELP),
    defMSG(DM_ENDCONVERSATION),
    defMSG(DM_PRINT),
    defMSG(DM_RENDER),
    defMSG(DM_RENDERCOMPLETE),
    defMSG(DM_RENDERPREPARE),
    defMSG(DM_DRAGFILECOMPLETE),
    defMSG(DM_EMPHASIZETARGET),
    defMSG(DM_DRAGERROR),
    defMSG(DM_FILERENDERED),
    defMSG(DM_RENDERFILE),
    defMSG(DM_DRAGOVERNOTIFY),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(Container, 1),               /* 0x0330 - 0x0350 */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(CM_ALLOCDETAILFIELDINFO),
    defMSG(CM_ALLOCRECORD),
    defMSG(CM_ARRANGE),
    defMSG(CM_ERASERECORD),
    defMSG(CM_FILTER),
    defMSG(CM_FREEDETAILFIELDINFO),
    defMSG(CM_FREERECORD),
    defMSG(CM_HORZSCROLLSPLITWINDOW),
    defMSG(CM_INSERTDETAILFIELDINFO),
    defMSG(CM_INSERTRECORD),
    defMSG(CM_INVALIDATEDETAILFIELDINFO),
    defMSG(CM_INVALIDATERECORD),
    defMSG(CM_PAINTBACKGROUND),
    defMSG(CM_QUERYCNRINFO),
    defMSG(CM_QUERYDETAILFIELDINFO),
    defMSG(CM_QUERYDRAGIMAGE),
    defMSG(CM_QUERYRECORD),
    defMSG(CM_QUERYRECORDEMPHASIS),
    defMSG(CM_QUERYRECORDFROMRECT),
    defMSG(CM_QUERYRECORDRECT),
    defMSG(CM_QUERYVIEWPORTRECT),
    defMSG(CM_REMOVEDETAILFIELDINFO),
    defMSG(CM_REMOVERECORD),
    defMSG(CM_SCROLLWINDOW),
    defMSG(CM_SEARCHSTRING),
    defMSG(CM_SETCNRINFO),
    defMSG(CM_SETRECORDEMPHASIS),
    defMSG(CM_SORTRECORD),
    defMSG(CM_OPENEDIT),
    defMSG(CM_CLOSEEDIT),
    defMSG(CM_COLLAPSETREE),
    defMSG(CM_EXPANDTREE),
    defMSG(CM_QUERYRECORDINFO),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(Notebook, 1),                /* 0x0353 - 0x0366 */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(BKM_CALCPAGERECT),
    defMSG(BKM_DELETEPAGE),
    defMSG(BKM_INSERTPAGE),
    defMSG(BKM_INVALIDATETABS),
    defMSG(BKM_TURNTOPAGE),
    defMSG(BKM_QUERYPAGECOUNT),
    defMSG(BKM_QUERYPAGEID),
    defMSG(BKM_QUERYPAGEDATA),
    defMSG(BKM_QUERYPAGEWINDOWHWND),
    defMSG(BKM_QUERYTABBITMAP),
    defMSG(BKM_QUERYTABTEXT),
    defMSG(BKM_SETDIMENSIONS),
    defMSG(BKM_SETPAGEDATA),
    defMSG(BKM_SETPAGEWINDOWHWND),
    defMSG(BKM_SETSTATUSLINETEXT),
    defMSG(BKM_SETTABBITMAP),
    defMSG(BKM_SETTABTEXT),
    defMSG(BKM_SETNOTEBOOKCOLORS),
    defMSG(BKM_QUERYPAGESTYLE),
    defMSG(BKM_QUERYSTATUSLINETEXT),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(Slider, 1),                  /* 0x0369 - 0x0372 */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(SLM_ADDDETENT),
    defMSG(SLM_QUERYDETENTPOS),
    defMSG(SLM_QUERYSCALETEXT),
    defMSG(SLM_QUERYSLIDERINFO),
    defMSG(SLM_QUERYTICKPOS),
    defMSG(SLM_QUERYTICKSIZE),
    defMSG(SLM_REMOVEDETENT),
    defMSG(SLM_SETSCALETEXT),
    defMSG(SLM_SETSLIDERINFO),
    defMSG(SLM_SETTICKSIZE),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(ValueSet, 1),                /* 0x0375 - 0x037d */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(VM_QUERYITEM),
    defMSG(VM_QUERYITEMATTR),
    defMSG(VM_QUERYMETRICS),
    defMSG(VM_QUERYSELECTEDITEM),
    defMSG(VM_SELECTITEM),
    defMSG(VM_SETITEM),
    defMSG(VM_SETITEMATTR),
    defMSG(VM_SETMETRICS),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(ExtMouseButtons, 1),         /* 0x0410 - 0x041f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(WM_CHORD),
    defMSG(WM_BUTTON1MOTIONSTART),
    defMSG(WM_BUTTON1MOTIONEND),
    defMSG(WM_BUTTON1CLICK),
    defMSG(WM_BUTTON2MOTIONSTART),
    defMSG(WM_BUTTON2MOTIONEND),
    defMSG(WM_BUTTON2CLICK),
    defMSG(WM_BUTTON3MOTIONSTART),
    defMSG(WM_BUTTON3MOTIONEND),
    defMSG(WM_BUTTON3CLICK),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(MouseTranslate, 1),          /* 0x0420 - 0x042f */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(WM_BEGINDRAG),
    defMSG(WM_ENDDRAG),
    defMSG(WM_SINGLESELECT),
    defMSG(WM_OPEN),
    defMSG(WM_CONTEXTMENU),
    defMSG(WM_CONTEXTHELP),
    defMSG(WM_TEXTEDIT),
    defMSG(WM_BEGINSELECT),
    defMSG(WM_ENDSELECT),

#ifdef OS2ME_INCLUDED                   /* MMPM/2 stuff defined?          */

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(MediaControlInterface, 1),   /* 0x0500 - 0x0506 */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(MM_MCINOTIFY),
    defMSG(MM_MCIPASSDEVICE),
    defMSG(MM_MCIPOSITIONCHANGE),
    defMSG(MM_MCICUEPOINT),
    defMSG(MM_MCIPLAYLISTMESSAGE),
    defMSG(MM_MCIEVENT),
    defMSG(MM_MCISYNCH),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(SecondaryWindow, 1),         /* 0x0519 */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(WM_INITSECONDARYWINDOW),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(GraphicButton, 1),           /* 0x0524 - 0x0534 */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(GBN_BUTTONUP),
    defMSG(GBN_BUTTONDOWN),
    defMSG(GBN_BUTTONHILITE),
    defMSG(GBM_SETGRAPHICDATA),
    defMSG(GBM_ANIMATE),
    defMSG(GBM_SETANIMATIONRATE),
    defMSG(GBM_QUERYANIMATIONACTIVE),
    defMSG(GBM_QUERYANIMATIONRATE),
    defMSG(GBM_SETBITMAPINDEX),
    defMSG(GBM_QUERYBITMAPINDEX),
    defMSG(GBM_SETSTATE),
    defMSG(GBM_QUERYSTATE),
    defMSG(GBM_SETTEXTPOSITION),
    defMSG(GBM_QUERYTEXTPOSITION),

    defMSG(MM_TABHELP),

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
  defGROUP(CircularSlider, 1),          /* 0x053d - 0x054b */
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    defMSG(CSM_QUERYRANGE),
    defMSG(CSM_SETRANGE),
    defMSG(CSM_QUERYVALUE),
    defMSG(CSM_SETVALUE),
    defMSG(CSM_QUERYRADIUS),
    defMSG(CSM_SETINCREMENT),
    defMSG(CSM_QUERYINCREMENT),
    defMSG(CSM_SETBITMAPDATA),
    defMSG(CSN_SETFOCUS),
    defMSG(CSN_CHANGED),
    defMSG(CSN_TRACKING),
    defMSG(CSN_QUERYBACKGROUNDCOLOR),

#endif

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Define "end matter"                                                    ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  defSKIP(1),
  defCOMM("Standard message status"),
  defSKIP(1),
  defTEXT("Include All"),
  defSKIP(1),
  defTEXT("Exclude MouseMovement"),
  defTEXT("Exclude WM_QUERYFOCUSCHAIN"),

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Define End-Of-List marker                                              ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
 defEOL
};

int valueEOF = 0x1A;      /* EOF character */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Away we go....                                                           ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
INT main( void )
{
  FILE *hProfile;

  register unsigned nItems, nSkips;

  fprintf( stderr, PGM_TITLE);
  hProfile = fopen("pmspydef.pro", "w");

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Process the items until the End-Of-List is detected                    ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  for(nItems = 0;
      sItems[nItems].Op != isEOL;
      nItems++)
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Process based on what type of item it is                             ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    switch( sItems[nItems].Op )
    {
      /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
      /*³ Generate GROUP statement                                           ³*/
      /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
      case isGROUP:
           fprintf(hProfile,
                   PRO_GROUP,
                   sItems[nItems].pszName,           /* GROUP name  */
                   sItems[nItems].usValue );         /* GROUP color */
      break;

      /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
      /*³ Generate MSG statement                                             ³*/
      /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
      case isMSG:
           fprintf(hProfile,
                   PRO_MSG,
                   sItems[nItems].pszName,           /* MSG   name  */
                   sItems[nItems].usValue );         /* MSG   value */
      break;

      /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
      /*³ Generate TEXT statement                                            ³*/
      /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
      case isTEXT:
           fprintf(hProfile,
                   "%s\n",
                   sItems[nItems].pszName);        /* TEXT */
      break;

      /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
      /*³ Generate COMMENT statement                                         ³*/
      /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
      case isCOMM:
           fprintf(hProfile,
                   "/* %s */\n",
                   sItems[nItems].pszName);        /* TEXT */
      break;

      /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
      /*³ Generate SKIPs                                                     ³*/
      /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
      case isSKIP:
           for(nSkips = 0;
               nSkips < sItems[nItems].usValue;
               nSkips++)
           {
            fprintf(hProfile, "\n");
           }
      break;
    }
  }

  fprintf(hProfile, "%c", valueEOF);   /* insure EOF character written!!!! */

  fclose(hProfile);

  return( 0 );       /* return a "clean" return code so no MAKE failures */
}
