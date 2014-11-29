/*--------------------------------------------------------------------*/
/*-- DMPWIN sample program : dumps all active window and childs ------*/
/*--                         information to Rexx variables      ------*/
/*-- Author : Marc Fiammante 20/07/1993                         ------*/
/*-- See comments in code for explanations                      ------*/
#define  INCL_DOS
#define  INCL_PM
#define  INCL_NLS
#include <os2.h>
#define  INCL_RXSHV                     /* include shared variable    */
#define  INCL_RXMACRO                   /* include macrospace info    */
#include <rexxsaa.h>                    /* REXXSAA header information */
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
/*----------------------------------------*/
/*- Global data                  ---------*/
HWND    Active =0;          /* Active Window                   */
SHORT   Current=-1;         /* Current number of childs found  */
USHORT  MaxObj =0;          /* Max allocated objects in memory */
PID     pid;                /* Active window Process id        */
#define ID_WC_FRAME             0x0001
#define ID_WC_COMBOBOX          0x0002
#define ID_WC_BUTTON            0x0003
#define ID_WC_MENU              0x0004
#define ID_WC_STATIC            0x0005
#define ID_WC_ENTRYFIELD        0x0006
#define ID_WC_LISTBOX           0x0007
#define ID_WC_SCROLLBAR         0x0008
#define ID_WC_TITLEBAR          0x0009
#define ID_WC_MLE               0x000A
#define ID_WC_APPSTAT           0x0010
#define ID_WC_KBDSTAT           0x0011
#define ID_WC_PECIC             0x0012
#define ID_WC_DBE_KKPOPUP       0x0013
#define ID_WC_SPINBUTTON        0x0020
#define ID_WC_CONTAINER         0x0025
#define ID_WC_SLIDER            0x0026
#define ID_WC_VALUESET          0x0027
#define ID_WC_NOTEBOOK          0x0028
#define ADDSTYLE(a,b) if ((Style & (a))==(a)) {strcat(StyleText,(b));Set=TRUE;}
/*----------------------------------------*/
/*- structure to dump menu Items ---------*/
typedef struct _MENUDUMP {
  struct _MENUDUMP * NextMenu;
  USHORT IdParent;
  USHORT IdItem;
  SHORT  TextLength;
  CHAR   ItemText[1];
} MENUDUMP;
MENUDUMP **LastMenu=NULL;
MENUDUMP * NewMenu =NULL;
/*----------------------------------------*/
/*- structure to dump list Items ---------*/
typedef struct _LISTDUMP {
  struct _LISTDUMP * NextList;
  SHORT  TextLength;
  CHAR   ItemText[1];
} LISTDUMP;
LISTDUMP **LastList=NULL;
LISTDUMP * NewList =NULL;
/*----------------------------------------*/
/*- structure to hold dumped information -*/
typedef struct _OBJECT {
   HWND     Handle;
   RXSTRING ClassName;
   RXSTRING Text;
   RXSTRING CtlData;
   RXSTRING StyleText;
   int    x;
   int    y;
   int    cx;
   int    cy;
   USHORT Id;
   ULONG  Style;
   BOOL   Subclassed;
   BOOL   IsFrame;
   USHORT Flags;
   USHORT xRestore;
   USHORT yRestore;
   USHORT cxRestore;
   USHORT cyRestore;
   USHORT xMinimize;
   USHORT yMinimize;
   LONG   Model;
   MENUDUMP * NextMenu;
   LISTDUMP * NextList;
} OBJECT;
OBJECT *Objects=0;
/*----------------------------------------*/
/*- declares for shared memory           -*/
#define ITEMTEXTSIZE 256
typedef union _SHRDATA {
  char      ItemText[ITEMTEXTSIZE];
  MENUITEM  MenuItem;
  WNDPARAMS WndParams;
} SHRDATA;
SHRDATA   *pShrData;
PSZ        ItemText;
MENUITEM  *MenuItem;
WNDPARAMS *WndParams;
/*----------------------------------------*/
/*- local function prototypes            -*/
int     AddVar(PSZ Name,int numvar,PRXSTRING Data);
int     EnumChilds(HWND Parent);
int     _Optlink SortChildsByPos(  );
int     SetRexxVars(void);
void    DumpWindow(HWND HwndChild);
/*----------------------------------------*/
/*- RxListObjects Rexx function to dump  -*/
LONG APIENTRY RxListObjects(
   PUCHAR    func,
   ULONG     ac,
   PRXSTRING av,
   PSZ       que,
   PRXSTRING ret)
  {
     APIRET rc;
     HAB  hab=0;
     HMQ  hmq=0;
     TID  tid;
     Current=-1;
     MaxObj =16;
     /*----------------------------------------------*/
     /*- Wait for user to select window to activate -*/
     DosSleep(2000L);
     /*----------------------------------------------*/
     /*- In case no PM registration try one         -*/
     hab = WinInitialize( 0 );                    /* Initialize PM               */
     if (hab) {
        hmq = WinCreateMsgQueue( hab, 0 );           /* Create a message queue      */
     } /* endif */
     /*----------------------------------------------*/
     /*- Allocate initial heap memory               -*/
     Objects=malloc(MaxObj*sizeof(OBJECT));
     /*----------------------------------------------*/
     /*- Get active window handle and process       -*/
     Active =WinQueryActiveWindow(HWND_DESKTOP);
     WinQueryWindowProcess(Active,&pid,&tid);
     /*----------------------------------------------*/
     /*- Allocate Shared memory for communication   -*/
     pShrData=NULL;
     rc=DosAllocSharedMem((PVOID)&pShrData,NULL,sizeof(SHRDATA),fALLOCSHR);
     if (rc==0) {
       rc=DosGiveSharedMem(pShrData,pid,fGIVESHR);
     } /* endif */
     if (rc==0) {
        /*----------------------------------------------*/
        /*- Initialise pointer to shared memory        -*/
        ItemText=(PSZ)pShrData;
        MenuItem=(MENUITEM *)pShrData;
        WndParams=(WNDPARAMS *)pShrData;
        /*----------------------------------------------*/
        /*- Dump Data for Active window itself         -*/
        DumpWindow(Active);
        /*----------------------------------------------*/
        /*- Dump Data for all childs                   -*/
        EnumChilds(Active);
        /*----------------------------------------------*/
        /*- If data was found sort it by position      -*/
        if (Current>=0) {
           qsort(Objects,Current+1,sizeof(OBJECT),SortChildsByPos);
           /*----------------------------------------------*/
           /*- Initialise Rexx Variables                  -*/
           SetRexxVars();
        } /* endif */
        /*----------------------------------------------*/
        /*- Free the allocated shared memory           -*/
        DosFreeMem(pShrData);
     }
     /*----------------------------------------------*/
     /*- Free local heap memory                     -*/
     free(Objects);
     /*--------------------------------------------------------------------*/
     /*-- Return the count of elements in result    -----------------------*/
     itoa(Current,ret->strptr,10);
     ret->strlength=strlen(ret->strptr);     /*                            */
     return 0;
}
/*--------------------------------------------------------------------*/
/*-- Dumps given listbox handle Items          -----------------------*/
void DumpListBox(HWND HwndList) {
   SHORT ItemCount,ItemPos;
   SHORT TextLength;
   int   i;
   /*----------------------------------------------*/
   /*- Get Item count                             -*/
   ItemCount =(SHORT)WinSendMsg(HwndList, LM_QUERYITEMCOUNT,0,0);
   for ( ItemPos =0;ItemPos<ItemCount ;ItemPos++ ) {
      /*---------------------------------------------------*/
      /*- Get Its text (here I limit it to ITEMTEXTSIZE    */
      /*- MM_QUERYITEMTEXTSIZE could be used to get length */
      TextLength =(SHORT)WinSendMsg(HwndList, LM_QUERYITEMTEXT,
                                   MPFROM2SHORT(ItemPos,(SHORT)ITEMTEXTSIZE),
                                   MPFROMP((ItemText)));
      NewList=(LISTDUMP *)malloc(sizeof(LISTDUMP)+TextLength);
      if (NewList) {
         *LastList=NewList;
         NewList->NextList=NULL;
         LastList=&NewList->NextList;
         NewList->TextLength= TextLength;
         /*----------------------------------*/
         /* suppress control chars and copy--*/
         for (i=0;i<TextLength;i++) {
            if (ItemText[i]>=0x20) {
               NewList->ItemText[i]=ItemText[i];
            } /* endif */
         } /* endfor */
         NewList->ItemText[TextLength]=0x00;
      } /* endif */
   }
}
/*--------------------------------------------------------------------*/
/*-- Dumps styme in readable form ------------------------------------*/
void DumpStyle(int Atom,ULONG Style) {
   static char StyleText[700];
   BOOL Set;
   StyleText[0]=0x00;
   switch (Atom) {
      case ID_WC_COMBOBOX:
          ADDSTYLE(CBS_SIMPLE      ,"| CBS_SIMPLE ");
          ADDSTYLE(CBS_DROPDOWN    ,"| CBS_DROPDOWN ");
          ADDSTYLE(CBS_DROPDOWNLIST,"| CBS_DROPDOWNLIST ");
          ADDSTYLE(CBS_COMPATIBLE  ,"| CBS_COMPATIBLE ");
          /*
           * The following edit and listbox styles may be used in conjunction
           * with CBS_ styles */
           ADDSTYLE(ES_AUTOTAB     ,"| ES_AUTOTAB ");
           Set=FALSE;
           ADDSTYLE(ES_SBCS        ,"| ES_SBCS ");
           ADDSTYLE(ES_DBCS        ,"| ES_DBCS ");
           ADDSTYLE(ES_MIXED       ,"| ES_MIXED ");
           if (Set==FALSE) {
              ADDSTYLE(ES_ANY        ,"| ES_ANY ");
           } /* endif */
           ADDSTYLE(LS_HORZSCROLL  ,"| LS_HORZSCROLL ");
         break;
      case ID_WC_BUTTON:
           Set=FALSE;
           ADDSTYLE(BS_CHECKBOX       ,"| BS_CHECKBOX ");
           ADDSTYLE(BS_AUTOCHECKBOX   ,"| BS_AUTOCHECKBOX ");
           ADDSTYLE(BS_RADIOBUTTON    ,"| BS_RADIOBUTTON ");
           ADDSTYLE(BS_AUTORADIOBUTTON,"| BS_AUTORADIOBUTTON ");
           ADDSTYLE(BS_3STATE         ,"| BS_3STATE ");
           ADDSTYLE(BS_AUTO3STATE     ,"| BS_AUTO3STATE ");
           ADDSTYLE(BS_USERBUTTON     ,"| BS_USERBUTTON ");
           if (Set==FALSE) {
              ADDSTYLE(BS_PUSHBUTTON     ,"| BS_PUSHBUTTON ");
           }
           ADDSTYLE(BS_BITMAP         ,"| BS_BITMAP ");
           ADDSTYLE(BS_ICON           ,"| BS_ICON ");
           ADDSTYLE(BS_HELP           ,"| BS_HELP ");
           ADDSTYLE(BS_SYSCOMMAND     ,"| BS_SYSCOMMAND ");
           ADDSTYLE(BS_DEFAULT        ,"| BS_DEFAULT ");
           ADDSTYLE(BS_NOPOINTERFOCUS ,"| BS_NOPOINTERFOCUS ");
           ADDSTYLE(BS_NOBORDER       ,"| BS_NOBORDER ");
           ADDSTYLE(BS_NOCURSORSELECT ,"| BS_NOCURSORSELECT ");
           ADDSTYLE(BS_AUTOSIZE       ,"| BS_AUTOSIZE ");
         break;
      case ID_WC_MENU:
           ADDSTYLE(MS_ACTIONBAR         ,"| MS_ACTIONBAR ");
           ADDSTYLE(MS_TITLEBUTTON       ,"| MS_TITLEBUTTON ");
           ADDSTYLE(MS_VERTICALFLIP      ,"| MS_VERTICALFLIP ");
           ADDSTYLE(MS_CONDITIONALCASCADE,"| MS_CONDITIONALCASCADE ");
         break;
      case ID_WC_STATIC:
           ADDSTYLE(SS_TEXT            ,"| SS_TEXT ");
           ADDSTYLE(SS_GROUPBOX        ,"| SS_GROUPBOX ");
           ADDSTYLE(SS_ICON            ,"| SS_ICON ");
           ADDSTYLE(SS_BITMAP          ,"| SS_BITMAP ");
           ADDSTYLE(SS_FGNDRECT        ,"| SS_FGNDRECT ");
           ADDSTYLE(SS_HALFTONERECT    ,"| SS_HALFTONERECT ");
           ADDSTYLE(SS_BKGNDRECT       ,"| SS_BKGNDRECT ");
           ADDSTYLE(SS_FGNDFRAME       ,"| SS_FGNDFRAME ");
           ADDSTYLE(SS_HALFTONEFRAME   ,"| SS_HALFTONEFRAME ");
           ADDSTYLE(SS_BKGNDFRAME      ,"| SS_BKGNDFRAME ");
           ADDSTYLE(SS_SYSICON         ,"| SS_SYSICON ");
           ADDSTYLE(SS_AUTOSIZE        ,"| SS_AUTOSIZE ");
           if ((Style&SS_TEXT)==SS_TEXT) {
              ADDSTYLE(DT_QUERYEXTENT    ,"| DT_QUERYEXTENT ");
              ADDSTYLE(DT_UNDERSCORE     ,"| DT_UNDERSCORE ");
              ADDSTYLE(DT_STRIKEOUT      ,"| DT_STRIKEOUT ");
              ADDSTYLE(DT_TEXTATTRS      ,"| DT_TEXTATTRS ");
              ADDSTYLE(DT_EXTERNALLEADING,"| DT_EXTERNALLEADING ");
              ADDSTYLE(DT_LEFT           ,"| DT_LEFT ");
           } /* endif */
         break;
      case ID_WC_ENTRYFIELD:
           ADDSTYLE(ES_CENTER    ,"| ES_CENTER ");
           ADDSTYLE(ES_RIGHT     ,"| ES_RIGHT ");
           Set=FALSE;
           if (Set==FALSE) {
              ADDSTYLE(ES_LEFT    ,"| ES_LEFT ");
           }
           ADDSTYLE(ES_AUTOSCROLL,"| ES_AUTOSCROLL ");
           ADDSTYLE(ES_MARGIN    ,"| ES_MARGIN ");
           ADDSTYLE(ES_AUTOTAB   ,"| ES_AUTOTAB ");
           ADDSTYLE(ES_READONLY  ,"| ES_READONLY ");
           ADDSTYLE(ES_COMMAND   ,"| ES_COMMAND ");
           ADDSTYLE(ES_UNREADABLE,"| ES_UNREADABLE ");
           ADDSTYLE(ES_AUTOSIZE  ,"| ES_AUTOSIZE ");
           Set=FALSE;
           ADDSTYLE(ES_SBCS      ,"| ES_SBCS ");
           ADDSTYLE(ES_DBCS      ,"| ES_DBCS ");
           ADDSTYLE(ES_MIXED     ,"| ES_MIXED ");
           if (Set==FALSE) {
              ADDSTYLE(ES_ANY        ,"| ES_ANY ");
           }
         break;
      case ID_WC_LISTBOX:
           ADDSTYLE(LS_MULTIPLESEL,"| LS_MULTIPLESEL ");
           ADDSTYLE(LS_OWNERDRAW  ,"| LS_OWNERDRAW ");
           ADDSTYLE(LS_NOADJUSTPOS,"| LS_NOADJUSTPOS ");
           ADDSTYLE(LS_HORZSCROLL ,"| LS_HORZSCROLL ");
           ADDSTYLE(LS_EXTENDEDSEL,"| LS_EXTENDEDSEL ");
         break;
      case ID_WC_SCROLLBAR:
           Set=FALSE;
           ADDSTYLE(SBS_VERT     ,"| SBS_VERT ");
           if (Set==FALSE) {
              ADDSTYLE(SBS_HORZ     ,"| SBS_HORZ ");
           }
           ADDSTYLE(SBS_THUMBSIZE,"| SBS_THUMBSIZE ");
           ADDSTYLE(SBS_AUTOTRACK,"| SBS_AUTOTRACK ");
           ADDSTYLE(SBS_AUTOSIZE ,"| SBS_AUTOSIZE ");
         break;
      case ID_WC_MLE:
           ADDSTYLE(MLS_WORDWRAP   ,"| MLS_WORDWRAP ");
           ADDSTYLE(MLS_BORDER     ,"| MLS_BORDER ");
           ADDSTYLE(MLS_VSCROLL    ,"| MLS_VSCROLL ");
           ADDSTYLE(MLS_HSCROLL    ,"| MLS_HSCROLL ");
           ADDSTYLE(MLS_READONLY   ,"| MLS_READONLY ");
           ADDSTYLE(MLS_IGNORETAB  ,"| MLS_IGNORETAB ");
           ADDSTYLE(MLS_DISABLEUNDO,"| MLS_DISABLEUNDO ");
         break;
      case ID_WC_SPINBUTTON:
           Set=FALSE;
           ADDSTYLE(SPBS_NUMERICONLY  ,"| SPBS_NUMERICONLY ");
           ADDSTYLE(SPBS_READONLY     ,"| SPBS_READONLY ");
           if (Set==FALSE) {
              ADDSTYLE(SPBS_ALLCHARACTERS,"| SPBS_ALLCHARACTERS ");
           }
           Set=FALSE;
           ADDSTYLE(SPBS_SERVANT      ,"| SPBS_SERVANT ");
           if (Set==FALSE) {
              ADDSTYLE(SPBS_MASTER       ,"| SPBS_MASTER ");
           }
           Set=FALSE;
           ADDSTYLE(SPBS_JUSTLEFT     ,"| SPBS_JUSTLEFT ");
           ADDSTYLE(SPBS_JUSTRIGHT    ,"| SPBS_JUSTRIGHT ");
           ADDSTYLE(SPBS_JUSTCENTER   ,"| SPBS_JUSTCENTER ");
           if (Set==FALSE) {
               ADDSTYLE(SPBS_JUSTDEFAULT,"| SPBS_JUSTDEFAULT ");
           }
           ADDSTYLE(SPBS_NOBORDER     ,"| SPBS_NOBORDER ");
           ADDSTYLE(SPBS_FASTSPIN     ,"| SPBS_FASTSPIN ");
           ADDSTYLE(SPBS_PADWITHZEROS ,"| SPBS_PADWITHZERO ");
         break;
      case ID_WC_CONTAINER:
           ADDSTYLE(CCS_EXTENDSEL     ,"| CCS_EXTENDSEL ");
           ADDSTYLE(CCS_MULTIPLESEL   ,"| CCS_MULTIPLESEL ");
           ADDSTYLE(CCS_SINGLESEL     ,"| CCS_SINGLESEL ");
           ADDSTYLE(CCS_AUTOPOSITION  ,"| CCS_AUTOPOSITION ");
           ADDSTYLE(CCS_VERIFYPOINTERS,"| CCS_VERIFYPOINTERS ");
           ADDSTYLE(CCS_READONLY      ,"| CCS_READONLY ");
           ADDSTYLE(CCS_MINIRECORDCORE,"| CCS_MINIRECORDCORE ");
         break;
      case ID_WC_SLIDER:
           Set=FALSE;
           ADDSTYLE(SLS_VERTICAL,"| SLS_VERTICAL ");
           if (Set==FALSE) {
              ADDSTYLE(SLS_HORIZONTAL,"| SLS_HORIZONTAL ");
              Set=FALSE;
              ADDSTYLE(SLS_LEFT  ,"| SLS_LEFT ");
              ADDSTYLE(SLS_RIGHT ,"| SLS_RIGHT ");
              if (Set==FALSE) {
                 ADDSTYLE(SLS_CENTER,"| SLS_CENTER ");
              }
              ADDSTYLE(SLS_BUTTONSLEFT ,"| SLS_BUTTONSLEFT ");
              ADDSTYLE(SLS_BUTTONSRIGHT,"| SLS_BUTTONSRIGHT ");
              Set=FALSE;
              ADDSTYLE(SLS_HOMERIGHT,"SLS_HOMERIGHT ");
              if (Set==FALSE) {
                 ADDSTYLE(SLS_HOMELEFT ,"SLS_HOMELEFT ");
              }
           } else { /* Vertical Slider */
              Set=FALSE;
              ADDSTYLE(SLS_BOTTOM,"| SLS_BOTTOM ");
              ADDSTYLE(SLS_TOP   ,"| SLS_TOP ");
              if (Set==FALSE) {
                 ADDSTYLE(SLS_CENTER,"| SLS_CENTER ");
              }
              ADDSTYLE(SLS_BUTTONSBOTTOM,"| SLS_BUTTONSBOTTOM ");
              ADDSTYLE(SLS_BUTTONSTOP   ,"| SLS_BUTTONSTOP ");
              Set=FALSE;
              ADDSTYLE(SLS_HOMEBOTTOM,"| SLS_HOMEBOTTOM ");
              if (Set==FALSE) {
                 ADDSTYLE(SLS_HOMETOP,"| SLS_HOMETOP ");
              }
           }
           ADDSTYLE(SLS_SNAPTOINCREMENT,"| SLS_SNAPTOINCREMENT ");
           ADDSTYLE(SLS_OWNERDRAW      ,"| SLS_OWNERDRAW ");
           ADDSTYLE(SLS_READONLY       ,"| SLS_READONLY ");
           ADDSTYLE(SLS_RIBBONSTRIP    ,"| SLS_RIBBONSTRIP ");
           Set=FALSE;
           ADDSTYLE(SLS_PRIMARYSCALE2  ,"| SLS_PRIMARYSCALE2 ");
           if (Set==FALSE) {
              ADDSTYLE(SLS_PRIMARYSCALE1  ,"| SLS_PRIMARYSCALE1 ");
           }
         break;
      case ID_WC_VALUESET:
           ADDSTYLE(VS_BITMAP      ,"| VS_BITMAP ");
           ADDSTYLE(VS_ICON        ,"| VS_ICON ");
           ADDSTYLE(VS_TEXT        ,"| VS_TEXT ");
           ADDSTYLE(VS_RGB         ,"| VS_RGB ");
           ADDSTYLE(VS_COLORINDEX  ,"| VS_COLORINDEX ");
           ADDSTYLE(VS_BORDER      ,"| VS_BORDER ");
           ADDSTYLE(VS_ITEMBORDER  ,"| VS_ITEMBORDER ");
           ADDSTYLE(VS_SCALEBITMAPS,"| VS_SCALEBITMAPS ");
           ADDSTYLE(VS_RIGHTTOLEFT ,"| VS_RIGHTTOLEFT ");
           ADDSTYLE(VS_OWNERDRAW   ,"| VS_OWNERDRAW ");
         break;
      case ID_WC_NOTEBOOK:
           ADDSTYLE(BKS_BACKPAGESBR     ,"| BKS_BACKPAGESBR ");
           ADDSTYLE(BKS_BACKPAGESBL     ,"| BKS_BACKPAGESBL ");
           ADDSTYLE(BKS_BACKPAGESTR     ,"| BKS_BACKPAGESTR ");
           ADDSTYLE(BKS_BACKPAGESTL     ,"| BKS_BACKPAGESTL ");
           ADDSTYLE(BKS_MAJORTABRIGHT   ,"| BKS_MAJORTABRIGHT ");
           ADDSTYLE(BKS_MAJORTABLEFT    ,"| BKS_MAJORTABLEFT ");
           ADDSTYLE(BKS_MAJORTABTOP     ,"| BKS_MAJORTABTOP ");
           ADDSTYLE(BKS_MAJORTABBOTTOM  ,"| BKS_MAJORTABBOTTOM ");
           Set=FALSE;
           ADDSTYLE(BKS_ROUNDEDTABS     ,"| BKS_ROUNDEDTABS ");
           ADDSTYLE(BKS_POLYGONTABS     ,"| BKS_POLYGONTABS ");
           if (Set==FALSE) {
               ADDSTYLE(BKS_SQUARETABS      ,"| BKS_SQUARETABS ");
           }
           Set=FALSE;
           ADDSTYLE(BKS_SPIRALBIND      ,"| BKS_SPIRALBIND ");
           if (Set==FALSE) {
               ADDSTYLE(BKS_SOLIDBIND       ,"| BKS_SOLIDBIND ");
           }
           Set=FALSE;
           ADDSTYLE(BKS_STATUSTEXTRIGHT ,"| BKS_STATUSTEXTRIGHT ");
           ADDSTYLE(BKS_STATUSTEXTCENTER,"| BKS_STATUSTEXTCENTER ");
           if (Set==FALSE) {
              ADDSTYLE(BKS_STATUSTEXTLEFT  ,"| BKS_STATUSTEXTLEFT ");
           }
           Set=FALSE;
           ADDSTYLE(BKS_TABTEXTRIGHT    ,"| BKS_TABTEXTRIGHT ");
           ADDSTYLE(BKS_TABTEXTCENTER   ,"| BKS_TABTEXTCENTER ");
           if (Set==FALSE) {
              ADDSTYLE(BKS_TABTEXTLEFT     ,"| BKS_TABTEXTLEFT ");
           }
         break;
      default:
      if (Objects[Current].IsFrame) {
           ADDSTYLE(FS_ICON           ,"| FS_ICON ");
           ADDSTYLE(FS_ACCELTABLE     ,"| FS_ACCELTABLE ");
           ADDSTYLE(FS_SHELLPOSITION  ,"| FS_SHELLPOSITION ");
           ADDSTYLE(FS_TASKLIST       ,"| FS_TASKLIST ");
           ADDSTYLE(FS_NOBYTEALIGN    ,"| FS_NOBYTEALIGN ");
           ADDSTYLE(FS_NOMOVEWITHOWNER,"| FS_NOMOVEWITHOWNER ");
           ADDSTYLE(FS_SYSMODAL       ,"| FS_SYSMODAL ");
           ADDSTYLE(FS_DLGBORDER      ,"| FS_DLGBORDER ");
           ADDSTYLE(FS_BORDER         ,"| FS_BORDER ");
           ADDSTYLE(FS_SCREENALIGN    ,"| FS_SCREENALIGN ");
           ADDSTYLE(FS_MOUSEALIGN     ,"| FS_MOUSEALIGN ");
           ADDSTYLE(FS_SIZEBORDER     ,"| FS_SIZEBORDER ");
           ADDSTYLE(FS_AUTOICON       ,"| FS_AUTOICON ");
      }
   } /* endswitch */
   ADDSTYLE(WS_VISIBLE     ,"| WS_VISIBLE ");
   ADDSTYLE(WS_DISABLED    ,"| WS_DISABLED ");
   ADDSTYLE(WS_CLIPCHILDREN,"| WS_CLIPCHILDREN ");
   ADDSTYLE(WS_CLIPSIBLINGS,"| WS_CLIPSIBLINGS ");
   ADDSTYLE(WS_PARENTCLIP  ,"| WS_PARENTCLIP ");
   ADDSTYLE(WS_SAVEBITS    ,"| WS_SAVEBITS ");
   ADDSTYLE(WS_SYNCPAINT   ,"| WS_SYNCPAINT ");
   ADDSTYLE(WS_MINIMIZED   ,"| WS_MINIMIZED ");
   ADDSTYLE(WS_MAXIMIZED   ,"| WS_MAXIMIZED ");
   ADDSTYLE(WS_ANIMATE     ,"| WS_ANIMATE ");
   ADDSTYLE(WS_GROUP       ,"| WS_GROUP ");
   ADDSTYLE(WS_TABSTOP     ,"| WS_TABSTOP ");
   ADDSTYLE(WS_MULTISELECT ,"| WS_MULTISELECT ");
   Objects[Current].StyleText.strptr   =malloc(strlen(StyleText));
   if ((Objects[Current].StyleText.strptr)&&
       (strlen(StyleText)>2)) {
      /* remove heading '|' and trailing space */
      Objects[Current].StyleText.strlength=strlen(StyleText)-2;
      strcpy(Objects[Current].StyleText.strptr,StyleText+1);
   } else {
      Objects[Current].StyleText.strlength=0;
      Objects[Current].StyleText.strptr=StyleText;
   } /* endif */
}
/*--------------------------------------------------------------------*/
/*-- Dumps given menu handle & submenu Items   -----------------------*/
void DumpMenu(HWND HwndMenu,int IdParent) {
   SHORT ItemCount,ItemPos,ItemID;
   SHORT TextLength;
   int   i;
   /*----------------------------------------------*/
   /*- Get Item count                             -*/
   ItemCount =(SHORT)WinSendMsg(HwndMenu, MM_QUERYITEMCOUNT,0,0);
   for ( ItemPos =0;ItemPos<ItemCount ;ItemPos++ ) {
      /*----------------------------------------------*/
      /*- Get Item ID from its ordinal position      -*/
      ItemID =(SHORT)WinSendMsg(HwndMenu, MM_ITEMIDFROMPOSITION,
                                MPFROM2SHORT(ItemPos,TRUE),0);
      /*---------------------------------------------------*/
      /*- Get Its text (here I limit it to ITEMTEXTSIZE    */
      /*- MM_QUERYITEMTEXTSIZE could be used to get length */
      TextLength =(SHORT)WinSendMsg(HwndMenu, MM_QUERYITEMTEXT,
                                   MPFROM2SHORT(ItemID,(SHORT)ITEMTEXTSIZE),
                                   MPFROMP((ItemText)));
      /*----------------------------------------------*/
      /*- If some text was found (not separator or   -*/
      /*- bitmap) then store it                      -*/
      /*if (TextLength)*/ {
         NewMenu=(MENUDUMP *)malloc(sizeof(MENUDUMP)+TextLength);
         if (NewMenu) {
            *LastMenu=NewMenu;
            NewMenu->NextMenu=NULL;
            LastMenu=&NewMenu->NextMenu;
            NewMenu->IdParent=IdParent;
            NewMenu->IdItem  =ItemID;
            NewMenu->TextLength= TextLength;
            /*----------------------------------*/
            /* suppress control chars and copy--*/
            for (i=0;i<TextLength;i++) {
               if (ItemText[i]>=0x20) {
                  NewMenu->ItemText[i]=ItemText[i];
               } /* endif */
            } /* endfor */
            NewMenu->ItemText[TextLength]=0x00;
         } /* endif */
      }
      /*----------------------------------------------*/
      /*- Lets see if some submenu are present       -*/
      if ((BOOL)WinSendMsg(HwndMenu, MM_QUERYITEM,
                                     MPFROM2SHORT(ItemID, TRUE),
                                     MPFROMP(MenuItem))
         ) {
         if (MenuItem->hwndSubMenu) {
            /*----------------------------------------------*/
            /*- found submenu so recurse to dump it        -*/
            DumpMenu(MenuItem->hwndSubMenu,ItemID);
         } /* endif */
      }
   } /* endfor */
}
/*----------------------------------------------*/
/*- Dump the given handle information          -*/
void DumpWindow(HWND HwndChild) {
   static CHAR ClassName[256];
   int         Length;
   POINTL      WinPt;
   ULONG       WUlong;
   CLASSINFO   ClassInfo;
   APIRET      rc;
   ATOM        Atom;
   LONG        Model;
   SWP         Swp;
   /*-----------------------------*/
   /*- One more Child          ---*/
   Current++;
   if (Current>=MaxObj) {
      MaxObj+=16;
      Objects=realloc(Objects,MaxObj*sizeof(OBJECT));
   } /* endif */
   /*----------------------------------------------*/
   /*- Is target 32 or 16:16 model window         -*/
   Model=WinQueryWindowModel(HwndChild);
   Objects[Current].Model      = Model;
   /*-----------------------------*/
   /*- Save Child Class & Handle -*/
   Objects[Current].Handle     = HwndChild;
   Length=WinQueryClassName(HwndChild,sizeof(ClassName), ClassName);
   Objects[Current].ClassName.strptr    = (PSZ)malloc(Length+1);
   Objects[Current].ClassName.strlength = Length;
   strcpy(Objects[Current].ClassName.strptr  ,ClassName);
   Objects[Current].ClassName.strptr[Length]=0x00;
   /*----------------------------------------------*/
   /*- Check if it has a frame's behaviour        -*/
   WUlong=(ULONG)WinSendMsg (HwndChild, WM_QUERYFRAMEINFO, NULL, NULL );
   if (FI_FRAME & WUlong) {
      Objects[Current].IsFrame=TRUE;
      Objects[Current].Flags    =WinQueryWindowUShort(HwndChild,QWS_FLAGS    );
      Objects[Current].xRestore =WinQueryWindowUShort(HwndChild,QWS_XRESTORE );
      Objects[Current].yRestore =WinQueryWindowUShort(HwndChild,QWS_YRESTORE );
      Objects[Current].cxRestore=WinQueryWindowUShort(HwndChild,QWS_CXRESTORE);
      Objects[Current].cyRestore=WinQueryWindowUShort(HwndChild,QWS_CYRESTORE);
      Objects[Current].xMinimize=WinQueryWindowUShort(HwndChild,QWS_XMINIMIZE);
      Objects[Current].yMinimize=WinQueryWindowUShort(HwndChild,QWS_YMINIMIZE);
   } else {
      Objects[Current].IsFrame=FALSE;
      Objects[Current].Flags=0;
      Objects[Current].xRestore=0;
      Objects[Current].yRestore=0;
      Objects[Current].cxRestore=0;
      Objects[Current].cyRestore=0;
      Objects[Current].xMinimize=0;
      Objects[Current].yMinimize=0;
   }
   /*-----------------------------*/
   /*- Save Child Text           -*/
   Length=WinQueryWindowTextLength(HwndChild);
   Objects[Current].Text.strptr    = (PSZ)malloc(Length+1);
   Objects[Current].Text.strlength = Length;
   WinQueryWindowText(HwndChild,Length+1,Objects[Current].Text.strptr);
   Objects[Current].Text.strptr[Length]=0x00;
   /*-----------------------------*/
   /*- Save Child Style and else -*/
   Objects[Current].Id=WinQueryWindowUShort(HwndChild,QWS_ID);
   WinQueryClassInfo( WinQueryAnchorBlock(HwndChild),ClassName,&ClassInfo);
   WUlong = WinQueryWindowULong(HwndChild,QWP_PFNWP);
   if (WUlong==(ULONG)ClassInfo.pfnWindowProc) {
       Objects[Current].Subclassed=FALSE;
   } else {
       Objects[Current].Subclassed=TRUE;
   } /* endif */
   Objects[Current].Style= WinQueryWindowULong(HwndChild,QWL_STYLE);
   /*----------------------------------------------*/
   /*- See if it is a known Class                 -*/
   Atom=WinFindAtom(WinQuerySystemAtomTable(),ClassName);
   Objects[Current].NextMenu=NULL;  /* May be not a menu or no Items in Menu */
   Objects[Current].NextList=NULL;  /* May be not a list or no Items in List */
   switch (Atom) {
      /*----------------------------------------------*/
      /*- If it is a menu specific dump              -*/
      /*- Sorry case WC_MENU: does not compile here  -*/
      case ID_WC_MENU:
         LastMenu=&Objects[Current].NextMenu;
         DumpMenu(HwndChild,0);
         DumpStyle(Atom,Objects[Current].Style);
         break;
      /*----------------------------------------------*/
      /*- If it is a listbox or combo specific dump  -*/
      case ID_WC_COMBOBOX:
      case ID_WC_LISTBOX:
         LastList=&Objects[Current].NextList;
         DumpListBox(HwndChild);
      default:
         DumpStyle(Atom,Objects[Current].Style);
         break;
   } /* endswitch */
   /*-----------------------------*/
   /*- Save Child Control Data   -*/
   Objects[Current].CtlData.strlength = 0;
   Objects[Current].CtlData.strptr    = (PUCHAR)WndParams;
   if (Model==PM_MODEL_2X) {
      WndParams->fsStatus = WPM_CBCTLDATA;
      WndParams->cbCtlData=0;
      if (WinSendMsg(HwndChild,WM_QUERYWINDOWPARAMS,MPFROMP(WndParams),0)) {
           PUCHAR Data;
           rc=DosAllocSharedMem((PVOID)&Data,NULL,WndParams->cbCtlData,fALLOCSHR);
           if (rc==0) {
              rc=DosGiveSharedMem(Data,pid,fGIVESHR);
           }
           if (rc==0) {
              WndParams->pCtlData=Data;
              Objects[Current].CtlData.strptr  = (PSZ)malloc(WndParams->cbCtlData);
              Objects[Current].CtlData.strlength = WndParams->cbCtlData;
              WndParams->fsStatus = WPM_CTLDATA;
              WinSendMsg(HwndChild,WM_QUERYWINDOWPARAMS,MPFROMP(WndParams),0);
              memcpy(Objects[Current].CtlData.strptr,Data,WndParams->cbCtlData);
              DosFreeMem(Data);
           } /* endif */
      }
   }
   /*-----------------------------*/
   /* Now get the position        */
   /* relative to Frame window    */
   WinPt.x=0; WinPt.y=0;
   WinMapWindowPoints(Active, HwndChild, &WinPt,1);
   Objects[Current].x=WinPt.x;
   Objects[Current].y=WinPt.y;
   WinQueryWindowPos(HwndChild,&Swp);
   Objects[Current].cx=Swp.cx;
   Objects[Current].cy=Swp.cy;
}
/*----------------------------------------------*/
/*- Enumerate Child of given window            -*/
int EnumChilds(HWND Parent) {
   HENUM ChildEnum;
   HWND  HwndChild;
   ChildEnum=WinBeginEnumWindows(Parent);
   while( ( HwndChild =WinGetNextWindow(ChildEnum))!=0) {
     /*----------------------------------------------*/
     /*- Found one child dump data                  -*/
     DumpWindow(HwndChild);
     /*-----------------------------*/
     /* Get childs of child if any  */
     EnumChilds(HwndChild);
   }
   if (!WinEndEnumWindows(ChildEnum)) WinAlarm(HWND_DESKTOP,WA_ERROR);
}
/*----------------------------------------------*/
/*- Sort routine for qsort by position         -*/
int     _Optlink SortChildsByPos( OBJECT * El1,OBJECT * El2) {
  if ((El2->y)==(El1->y))
  {
    if ((El2->x)==(El1->x)) return  0;
    if ((El2->x)> (El1->x)) return  1;
    else                return -1;
  } else {
    if ((El2->y)< (El1->y))  return  1;
    else                return -1;
  }
}
/*----------------------------------------------*/
/*- Rexx variables setting for all objects     -*/
int     SetRexxVars() {
   int i,j;
   RXSTRING    RexxData;
   MENUDUMP *  DoneMenu;
   LISTDUMP *  DoneList;
   static char DataBuffer[ITEMTEXTSIZE];
   static char MenuBuffer[ITEMTEXTSIZE];
   RexxData.strptr=DataBuffer;
   for (i=0;i<=Current;i++) {
            if (Active==Objects[i].Handle) {
                sprintf(DataBuffer,"%d",i);
                RexxData.strlength=strlen(DataBuffer);
                AddVar("Active"  ,-1,  &RexxData);
            } /* endif */
            AddVar("Class"    ,i,  &Objects[i].ClassName);
            if (Objects[i].ClassName.strlength>0) {
               free(Objects[i].ClassName.strptr);
            } /* endif */
            AddVar("Text"     ,i,  &Objects[i].Text);
            if (Objects[i].Text.strlength>0) {
               free(Objects[i].Text.strptr);
            } /* endif */
            AddVar("CtlData"  ,i,  &Objects[i].CtlData);
            if (Objects[i].CtlData.strlength>0) {
               free(Objects[i].CtlData.strptr);
            } /* endif */
            AddVar("StyleText"  ,i,  &Objects[i].StyleText);
            if (Objects[i].StyleText.strlength>0) {
               free(Objects[i].StyleText.strptr);
            } /* endif */
            sprintf(DataBuffer,"%p",Objects[i].Handle);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("Handle"  ,i,  &RexxData);
            if (Objects[i].Model==PM_MODEL_1X) {
                strcpy(DataBuffer,"16:16 segmented Model Window");
            } else {
                strcpy(DataBuffer,"32    flat Model Window");
            }
            RexxData.strlength=strlen(DataBuffer);
            AddVar("model"  ,i,  &RexxData);
            sprintf(DataBuffer,"%d",-Objects[i].x);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("x"  ,i,  &RexxData);
            sprintf(DataBuffer,"%d",-Objects[i].y);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("y"  ,i,  &RexxData);
            sprintf(DataBuffer,"%d",Objects[i].cx);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("cx"  ,i,  &RexxData);
            sprintf(DataBuffer,"%d",Objects[i].cy);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("cy"  ,i,  &RexxData);
            sprintf(DataBuffer,"%hd",Objects[i].Id);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("Id"  ,i,  &RexxData);
            sprintf(DataBuffer,"%8.8X",Objects[i].Style);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("Style"  ,i,  &RexxData);
            sprintf(DataBuffer,"%d",Objects[i].Subclassed);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("Subclassed"  ,i,  &RexxData);
            sprintf(DataBuffer,"%d",Objects[i].IsFrame);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("IsFrame"  ,i,  &RexxData);
            sprintf(DataBuffer,"%hd",Objects[i].Flags);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("Flags"  ,i,  &RexxData);
            sprintf(DataBuffer,"%hd",Objects[i].xRestore);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("xRestore"  ,i,  &RexxData);
            sprintf(DataBuffer,"%hd",Objects[i].yRestore);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("yRestore"  ,i,  &RexxData);
            sprintf(DataBuffer,"%hd",Objects[i].cxRestore);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("cxRestore"  ,i,  &RexxData);
            sprintf(DataBuffer,"%hd",Objects[i].cyRestore);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("cyRestore"  ,i,  &RexxData);
            sprintf(DataBuffer,"%hd",Objects[i].xMinimize);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("xMinimize"  ,i,  &RexxData);
            sprintf(DataBuffer,"%hd",Objects[i].yMinimize);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("yMinimize"  ,i,  &RexxData);
            /*----------------------------------------------*/
            /*- Add one level for menu Items               -*/
            NewMenu=Objects[i].NextMenu;
            j=0;
            while (NewMenu!=NULL) {
               sprintf(DataBuffer,"%hd",NewMenu->IdParent);
               RexxData.strlength=strlen(DataBuffer);
               sprintf(MenuBuffer,"ItemParentId.%d",i);
               AddVar(MenuBuffer  ,j+1,  &RexxData);
               sprintf(DataBuffer,"%hd",NewMenu->IdItem);
               RexxData.strlength=strlen(DataBuffer);
               sprintf(MenuBuffer,"ItemId.%d",i);
               AddVar(MenuBuffer  ,j+1,  &RexxData);
               strcpy(DataBuffer,NewMenu->ItemText);
               RexxData.strlength=strlen(DataBuffer);
               sprintf(MenuBuffer,"ItemText.%d",i);
               AddVar(MenuBuffer  ,j+1,  &RexxData);
               DoneMenu=NewMenu;
               NewMenu=NewMenu->NextMenu;
               free(DoneMenu);
               j++;
            } /* endwhile */
            sprintf(DataBuffer,"%d",j);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("MenuItems"  ,i,  &RexxData);
            /*----------------------------------------------*/
            /*- Add one level for Listbox Items            -*/
            NewList=Objects[i].NextList;
            j=0;
            while (NewList!=NULL) {
               strcpy(DataBuffer,NewList->ItemText);
               RexxData.strlength=strlen(DataBuffer);
               sprintf(MenuBuffer,"ListItemText.%d",i);
               AddVar(MenuBuffer  ,j+1,  &RexxData);
               DoneList=NewList;
               NewList=NewList->NextList;
               free(DoneList);
               j++;
            } /* endwhile */
            sprintf(DataBuffer,"%d",j);
            RexxData.strlength=strlen(DataBuffer);
            AddVar("ListItems"  ,i,  &RexxData);
   } /* endfor */
}
/*----------------------------------------------*/
/*- call to the Rexx variable API for setting  -*/
SHVBLOCK RexxVar;
int     AddVar(PSZ Name,int numvar,PRXSTRING Data) {
   static char FullName[80];
   /*----------------------------------------------*/
   /*- Add the required stem                      -*/
   if (numvar>=0) {
      sprintf(FullName,"%s.%d",strupr(Name),numvar);
   } else {
      strcpy(FullName,strupr(Name));
   } /* endif */
   RexxVar.shvnext           =(struct _SHVBLOCK *)0;
   RexxVar.shvname.strptr    =FullName;
   RexxVar.shvname.strlength =strlen(FullName);
   RexxVar.shvnamelen        =strlen(FullName);
   RexxVar.shvvalue          =*Data;
   RexxVar.shvvaluelen       =RexxVar.shvvalue.strlength;
   RexxVar.shvcode           =RXSHV_SET;
   RexxVariablePool(&RexxVar);
}
