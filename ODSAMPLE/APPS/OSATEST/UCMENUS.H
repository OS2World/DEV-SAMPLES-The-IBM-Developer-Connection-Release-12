/***************************************************************************/
/***************************************************************************/
/*                        DISCLAIMER OF WARRANTIES.                        */
/***************************************************************************/
/***************************************************************************/
/*                                                                         */
/*  Copyright (C) 1995 IBM Corporation                                     */
/*                                                                         */
/*      DISCLAIMER OF WARRANTIES.  The following [enclosed] code is        */
/*      sample code created by IBM Corporation. This sample code is not    */
/*      part of any standard or IBM product and is provided to you solely  */
/*      for  the purpose of assisting you in the development of your       */
/*      applications.  The code is provided "AS IS", without               */
/*      warranty of any kind.  IBM shall not be liable for any damages     */
/*      arising out of your use of the sample code, even if they have been */
/*      advised of the possibility of such damages.                        */                           
/***************************************************************************/
/*----------------------------------------------------------------------------*/
/* */
/* UCMENUS.H */
/* */

/*--- Version of the dll, eg 2 stands for 2.00 and 100 for 1.00 */
#define UCMENUS_VERSION 211  /* Version 2.11 */

#ifndef UCMENUS_h
#define UCMENUS_h

/*(MAM) If UCMenus is complied as part of a larger product where resource */
/* IDs have to be coordinated, the following can be changed to set the */
/* base ID number of all UCMenu resources.  Note this requires recompile */
/* of the UCMenus code and application code to make them match. */

#define UCMBASE_ID                   3100           /* Reserve this value plus 100 */


/*----------------------------------------------------------------------------- */
/* Style flag bits        */
/*----------------------------------------------------------------------------- */
   #define UCS_FORCESIZE                   0x00000004  /* -- Force the size of the items ( see UCMinfo.cx and UCMInfo.cy ) */
   #define UCS_FRAMED                      0x00000008  /* -- Draw a 3D frame around the items */
   #define UCS_NOTEXT                      0x00000010  /* -- Do not display the text of the items */
   #define UCS_SHRINK_BLEND                0x00000020  /* -- When the bmp is shrunk, do not use BBO_IGNORE */
   #define UCS_SHRINK_OR                   0x00000040  /* -- set : use BBO_OR, else : use BBO_AND */
   #define UCS_NO_DM_ALT                   0x00000002  /* -- no drg drp copy to submenu when over the menu */
   #define UCS_NO_DM_M_TO_OTHER            0x00000080  /* -- no drg drp move to another ucmenu             */
   #define UCS_NO_DM_M_FROM_OTHER          0x00000100  /* -- no drg drp move from another ucmenu           */
   #define UCS_NO_DM_M_INSIDE              0x00000200  /* -- no drg drp move inside this ucmenu            */
   #define UCS_NO_DM_C_TO_OTHER            0x00000400  /* -- no drg drp copy to another ucmenu             */
   #define UCS_NO_DM_C_FROM_OTHER          0x00000800  /* -- no drg drp copy from another ucmenu           */
   #define UCS_NO_DM_C_INSIDE              0x00001000  /* -- no drg drp copy inside this ucmenu            */
   #define UCS_NO_DM_DISCARD               0x00002000  /* -- no drg drp to the shredder                    */
   #define UCS_NO_DM_RENDER_TO_BMP         0x00004000  /* -- no drg drp rendering as a BMP file            */
   #define UCS_NO_DM_RENDER_FROM_BMP       0x00008000  /* -- no drg drp from a BMP file                    */
   #define UCS_NO_DM                       0x00010000  /* -- no direct manipulation at all                 */

   #define UCS_NO_CM_SEPARATOR             0x00000001  /* -- no separator in the CM        */
   #define UCS_NO_CM_MENU_STYLE            0x00020000  /* -- no "change style" in the CM   */
   #define UCS_NO_CM_MENU_IMPORT           0x00040000  /* -- no "import" in the CM         */
   #define UCS_NO_CM_MENU_EXPORT           0x00080000  /* -- no "export" in the CM         */
   #define UCS_NO_CM_MENU_DEFAULT          0x00100000  /* -- no "load default" in the CM   */
   #define UCS_NO_CM_ITEM_EDIT             0x00200000  /* -- no "edit item" in the CM      */
   #define UCS_NO_CM_ITEM_CREATE           0x00400000  /* -- no "create item" in the CM    */
   #define UCS_NO_CM_ITEM_DELETE           0x00800000  /* -- no "delete item" in the CM    */
   #define UCS_NO_CM                       0x01000000  /* -- no context menu               */

   #define UCS_NOREORDER                   0x00C0BB82  /* --   UCS_NO_DM_M_TO_OTHER */
                                                       /* -- | UCS_NO_DM_M_FROM_OTHER */
                                                       /* -- | UCS_NO_DM_M_INSIDE */
                                                       /* -- | UCS_NO_DM_C_FROM_OTHER */
                                                       /* -- | UCS_NO_DM_C_INSIDE */
                                                       /* -- | UCS_NO_DM_DISCARD */
                                                       /* -- | UCS_NO_DM_RENDER_FROM_BMP */
                                                       /* -- | UCS_NO_CM_ITEM_CREATE */
                                                       /* -- | UCS_NO_CM_ITEM_DELETE */
                                                       /* -- | UCS_NO_DM_ALT */

   #define UCS_REORDERONLY                 0x00C0FD82  /* --   UCS_NO_DM_M_TO_OTHER */
                                                       /* -- | UCS_NO_DM_M_FROM_OTHER */
                                                       /* -- | UCS_NO_DM_C_TO_OTHER */
                                                       /* -- | UCS_NO_DM_C_FROM_OTHER */
                                                       /* -- | UCS_NO_DM_C_INSIDE */
                                                       /* -- | UCS_NO_DM_DISCARD */
                                                       /* -- | UCS_NO_DM_RENDER_TO_BMP */
                                                       /* -- | UCS_NO_DM_RENDER_FROM_BMP */
                                                       /* -- | UCS_NO_CM_ITEM_CREATE */
                                                       /* -- | UCS_NO_CM_ITEM_DELETE */
                                                       /* -- | UCS_NO_DM_ALT */

   #define UCS_STATIC                      0x01010000  /* --   UCS_NO_DM | UCS_NO_CM */

   #define UCS_NOITEMSMODIF                0x00E0F982  /* --   UCS_NO_DM_M_TO_OTHER */
                                                       /* -- | UCS_NO_DM_M_FROM_OTHER */
                                                       /* -- | UCS_NO_DM_C_FROM_OTHER */
                                                       /* -- | UCS_NO_DM_C_INSIDE */
                                                       /* -- | UCS_NO_DM_DISCARD */
                                                       /* -- | UCS_NO_DM_RENDER_TO_BMP */
                                                       /* -- | UCS_NO_DM_RENDER_FROM_BMP */
                                                       /* -- | UCS_NO_CM_ITEM_EDIT */
                                                       /* -- | UCS_NO_CM_ITEM_CREATE */
                                                       /* -- | UCS_NO_CM_ITEM_DELETE */
                                                       /* -- | UCS_NO_DM_ALT */

   #define UCS_CHNGBMPBG                   0x10000000  /* -- Change all bitmap pixels of the BmpBg color (specified in UCMINFO structure) */
                                                       /* --   to the background color (ItemBgColor) */
   #define UCS_CUSTOMHLP                   0x20000000  /* -- The help is provided by the user */
   #define UCS_PROMPTING                   0x40000000  /* -- Post the UCN_MOUSEMOVE notifications */
   #define UCS_NODEFAULTACTION             0x80000000  /* -- No execute action by default */
   #define UCS_BUBBLEHELP                  0x02000000  /* -- Bubble help (added V2.04) */
   #define UCS_CHNGBMPBG_AUTO              0x04000000  /* -- Change all pixels of the bitmap that are the same as */
                                                       /* --   the color of the lower-leftmost pixel to the background color (V2.04) */

/*----------------------------------------------------------------------------- */
/* Defaults */
/*----------------------------------------------------------------------------- */

   #define UCM_DEFAULTCOLOR     0x00C9C9C9L   /* -- light grey on 8514 */
   #define UCM_DEFAULTFONT      "8.Helv"

/*----------------------------------------------------------------------------- */
/* Data structures */
/*----------------------------------------------------------------------------- */

   typedef struct {
      LONG    hBmp;            /* Bitmap handle */
      PSZ     pszBmp;          /* Bitmap Identifier or file name */
      PSZ     pszText;         /* Bitmap Text */
      PSZ     pszAction;       /* Action : for instance the name of a macro */
      PSZ     pszParameters;   /* Parameters for the action */
      PSZ     pszData;         /* Data associated to the item, not visible for the user */
      USHORT  usId;            /* Id of the item */
  /*  LONG    lCurBmpBgRGB;    // ---> Obsolete (V2.04) */
      ULONG   ulFlags;         /* Upper word is bitmap resource ID, lower is UCMI_* flags */
   } UCMITEM, *PUCMITEM;

   #define ITEM(a,b) (((PUCMITEM)((a)->hItem))->b)      /* extract field from UCMINFO struct */
   #define UCMFIELD(a,b) (((PUCMITEM)((a)->hItem))->b)  /* better name */

   typedef struct {
      ULONG ID;
      PSZ   pszItemText;
   } CMITEMS,  *PCMITEMS;

   typedef struct {
      ULONG  cb;                   /* size of this structure */
      HMODULE hModule;             /* Module where the application resources are. */
      USHORT  NbOfCols;            /* Number of Columns (used for a matrix UCM) */
      USHORT  NbOfRows;            /* Number of Rows    (used for a matrix UCM) */
      PSZ     pszBitmapSearchPath; /* Path(s) to search for bitmap files. */
                                   /* Environment variables separated by spaces. */
      PSZ     pszDefaultBitmap;    /* Bitmap to use if bitmap specified for */
                                   /*  a menu item can't be resolved or loaded. */
      ULONG   Style;               /* Style of item (combination of UCS_ flags ) */
      ULONG   cx;                  /* Size to give to all the items if UCS_FORCESIZE */
      ULONG   cy;                  /* */
      LONG    BgBmp;               /* RGB value of the color of the bmp which has to be replaced with ItemBgColor */
      LONG    BgColor;             /* RGB color of the UCMenu background */
      LONG    ItemBgColor;         /* RGB color of the UCMenu items background */
      PSZ     pszFontNameSize;     /* String describing the font of the UCMenu */
      /*--------- Added V2.04 -----------*/
      ULONG   BubbleDelay;         /* Initial bubble delay time in msec (1/100 sec). */
      ULONG   BubbleRead;          /* Initial bubble read-time in msec. */
      HMODULE hModUCM;             /* Module where UCMenu resources are. */
      char    Spares[64];          /* Spare space for future use so we don't do this again! */
   } UCMINFO, *PUCMINFO;        

   typedef struct {  /* Structure passed to application in UCN_QRYBUBBLEHELP control message */
      MENUITEM     *MenuItem           ;             /* Item information supplied by UCMenus */
      char         *BubbleText         ;             /* Bubble text supplied by application */
   } QBUBBLEDATA, *PQBUBBLEDATA;


/*--- Flags for an ucmenu item */
#define  UCMI_BMPFROMFILE 0x0001       /* Bitmap was a file specification (else resource ID) */

/*--- UCMenu template constants */
#define UCMTMPLVERSION ( (ULONG)( 0x00000001 ) )
#define UCMTMPLSIG     ( (ULONG)( 0x53C1A09B ) )

/*--- Context menu items ids */
#define IDM_UCM_EDIT                 0x00E0
#define IDM_UCM_CREATE               0x00E1
#define IDM_UCM_DELETE               0x00E2
#define IDM_UCM_SEPARATOR            0x00E3
#define IDM_UCM_STYLE                0x00E4
#define IDM_UCM_DEFAULT              0x00E5
#define IDM_UCM_LOAD                 0x00E6
#define IDM_UCM_SAVEAS               0x00E7

/*--- Styles to use in WinCreateWindow when creating a UCMenu */
#define CMS_HORZ          0x0001
#define CMS_VERT          0x0002
#define CMS_MATRIX        0x0004     /* Matrix style, no prefered orientation (UCMenus sets H and V size) */
#define CMS_MATRIX_VERT   0x0014     /* Vertical-oriented matrix (app sets vertical size) */
#define CMS_MATRIX_HORZ   0x0024     /* Horz-oriented matirx (app sets horz size) */

/*--- Actions already defined in ucmenu.dll */
#define UCM_ACTION_EXEC "ExecuteProgram"   /* -- If the action of the item begins with this */
                                           /* -- string, the rest of it is interpreted as a */
                                           /* -- program name to be started in another thread. */

/*--- Type defined for the drag and drop of ucmenu items */
#define DRT_UCMITEM  "UCMenu item"

/*--- New menu item style defined by the ucmenus */
#define MIS_SPACER        0x4000           /* -- for spacer type items */

/*--- Class name of the ucmenus */
#define UCMENUCLASS "UCMenu"

/*--- New CMDSRC_* value defined */
#define CMDSRC_UCMENU 55234

/*----------------------------------------------------------------------------- */
/* Messages */
/*----------------------------------------------------------------------------- */

/*--- Notification codes associated to the WM_CONTROL messages */
#define UCN_SIZE                1
#define UCN_STYLE               2
#define UCN_FONT                3
#define UCN_COLOR               4
#define UCN_DELETEDITEM         5
#define UCN_ADDEDITEM           6
#define UCN_BITMAP              7
#define UCN_ACTION              8
#define UCN_TEXT                9
#define UCN_QRYTEMPLATEMODULE  10
#define UCN_QRYDEFTEMPLATEID   11
#define UCN_QRYACTIONLIST      12
#define UCN_CMITEM             13
#define UCN_HLP_NB_BMP         14
#define UCN_HLP_NB_CREATE      15
#define UCN_HLP_NB_ACTION      16
#define UCN_HLP_STYLE          17
#define UCN_HLP_DM             18
#define UCN_QRYRESBMP          19
#define UCN_MOUSEMOVE          20
#define UCN_ITEMSELECTED       21
#define UCN_HLP_BUFFET         22
#define UCN_QRYDEFTEMPLATE     23
#define UCN_QRYBUBBLEHELP      24   /* Added V2.04 */
#define UCN_QRYCONTEXTHWND     25   /* Added V2.04 */
#define UCN_CHANGEDITEM        26   /* Added V2.04 */

/*--- User messages */

#define UCMENU_ACTIONSINSERTED  WM_USER + 0x2000
#define UCMENU_QUERYSIZE        WM_USER + 0x2001
#define UCMENU_QUERYCOLOR       WM_USER + 0x2002
#define UCMENU_QUERYFONT        WM_USER + 0x2003
#define UCMENU_QUERYSTYLE       WM_USER + 0x2004
#define UCMENU_QUERYFORCEDSIZE  WM_USER + 0x2005
#define UCMENU_ADDITEMSTOCM     WM_USER + 0x2006
#define UCMENU_DELETECMITEM     WM_USER + 0x2007
#define UCMENU_QUERYVERSION     WM_USER + 0x2008
#define UCMENU_INSERTACTION     WM_USER + 0x2009
#define UCMENU_SETSTYLE         WM_USER + 0x200A
#define UCMENU_SETBGCOLOR       WM_USER + 0x200B
#define UCMENU_QUERYUCMINFO     WM_USER + 0x200C
#define UCMENU_UPDATE           WM_USER + 0x200D
#define UCMENU_SETFONT          WM_USER + 0x200E
#define UCMENU_SETBUBBLETIMERS  WM_USER + 0x200F   /* Added V2.04 */
#define UCMENU_ACTIVECHG        WM_USER + 0x2011   /* Added V2.04 */
#define UCMENU_QUERYRECT        WM_USER + 0x2012   /* Added V2.04 */
#define UCMENU_DISABLEUPDATE    WM_USER + 0x2013   /* Added V2.04 */


/*----------------------------------------------------------------------------- */
/* Callable interfaces */
/*----------------------------------------------------------------------------- */

HWND    APIENTRY UCMenuCreateFromResource(HAB hab, HWND hParent, HWND hOwner,
                                          ULONG ulStyle, LONG x, LONG y, LONG cx, LONG cy,
                                          HWND hInsertBehind, ULONG ulID,
                                          HMODULE hmodResource, USHORT usMenuID,
                                          PUCMINFO pUCMInfo, PHWND phTextMenu );
HWND    APIENTRY UCMenuCreateFromTemplate(HAB hab, HWND hParent, HWND hOwner,
                                          ULONG ulStyle, LONG x, LONG y, LONG cx, LONG cy,
                                          HWND hInsertBehind, ULONG ulID,
                                          PVOID pmtMenuTemplate,
                                          PUCMINFO pUCMInfo, PHWND phTextMenu );
HWND    APIENTRY UCMenuCreateFromHMenu(HAB hab, HWND hParent, HWND hOwner,
                                       ULONG ulStyle, LONG x, LONG y, LONG cx, LONG cy,
                                       HWND hInsertBehind, ULONG ulID,
                                       HWND hMenu, PUCMINFO pUCMInfo );
LONG    APIENTRY UCMenuLoadBitmap(PSZ pszFileName);
PVOID   APIENTRY UCMenuLoadTemplate(PSZ pszFileName, PULONG pBufLen);
BOOL    APIENTRY UCMenuSaveTemplate(HWND hwndUCMenu, PSZ pszFileName);
PVOID   APIENTRY UCMenuMakeTemplate(HWND hwndUCMenu, PULONG pBufLen);
PVOID   APIENTRY UCMenuLoadTemplateIni(PSZ pszFileName, PSZ pszKeyName, PULONG pBufLen);
BOOL    APIENTRY UCMenuSaveTemplateIni(HWND hwndUCMenu, PSZ pszFileName, PSZ pszKeyName);
PVOID   APIENTRY UCMenuAlloc(size_t size);
PSZ     APIENTRY UCMenuStrdup(const char *Src);
VOID    APIENTRY UCMenuFree(VOID * p);
VOID    APIENTRY UCMenuItemFree(UCMITEM *Source);
UCMITEM * APIENTRY UCMenuItemDup(UCMITEM *Source);
VOID    APIENTRY UCMenuFreeMenuData(HWND hwndMenu);
BOOL    APIENTRY UCMenuNew( HWND hwndUCMenu, PVOID pTemplate );
BOOL    APIENTRY UCMenuLoadDefault( HWND hwndUCMenu );
SHORT   APIENTRY UCMenuGetActionID(HWND hwndUCM, PSZ pszAction, BOOL bSubmenus, SHORT sStartID);
VOID    APIENTRY UCMenuSetActionAttr(HWND hwndUCM, PSZ pszAction, USHORT usAttrMask, USHORT usAttrValue );
BOOL    APIENTRY UCMenuSaveStyle(HWND hwndUCMenu, PSZ pszFileName);
BOOL    APIENTRY UCMenuLoadStyle(PSZ pszFileName, PUCMINFO pUCMInfo);
BOOL    APIENTRY UCMenuSaveStyleIni(HWND hwndUCMenu, PSZ pszFileName, PSZ pszKeyName);
BOOL    APIENTRY UCMenuLoadStyleIni(PSZ pszFileName, PSZ pszKeyName, PUCMINFO pUCMInfo);
ULONG   APIENTRY UCMenuResourceBuffetDlg( HWND hParent, HWND hUCMenu, ULONG ulBuffetID, HMODULE hmodBuffet );
ULONG   APIENTRY UCMenuTemplateBuffetDlg( HWND hParent, HWND hUCMenu, PVOID pTemplate );
ULONG   APIENTRY UCMenuGetVersion(void);
USHORT  APIENTRY UCMenuIdFromCoord(HWND hwndMenu, PPOINTL pptrPos);

#endif /* UCMENUS_h */ 
