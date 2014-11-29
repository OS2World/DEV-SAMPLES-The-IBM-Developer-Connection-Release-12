//----------------------------------------------------------------------------
//                             IBM Confidential
//----------------------------------------------------------------------------
//
// UCMENUS.H
//
//-------------------------------------+--------------------------------------
//                                     |   Advanced Commercial Applications
// Version: 1.00                       |   IBM T.J. Watson Research Center
//                                     |   P.O. Box 218
//                                     |   Yorktown Heights, NY 10598
//-------------------------------------+--------------------------------------
// History:
// --------
//
// created:    Feb 10 1992 by AB
// Release 2 : Dec 06 1993. (EOS2)
//----------------------------------------------------------------------------

//--- Version of the dll, eg 2 stands for 0.02 and 100 for 1.00
#define UCMENUS_VERSION 100

#ifndef UCMENUS_h
#define UCMENUS_h

//----------------------------------------------------------------------------
// ID's for IPF resources usually come right here to allow their use in the
// description part of the source code
//----------------------------------------------------------------------------
//
// //--- Component ID --------------
// #define IDC_UCMENUSCOMP                 MAKERESID(BID_VIEW, 0x0C00)
//
// //--- Type ID's -----------------
// #define IDT_UCMITEM                     MAKERESID(BID_VIEW, 0x0C01)
// #define IDT_UCMINFO                     MAKERESID(BID_VIEW, 0x0C02)
// #define IDT_CMITEMS                     MAKERESID(BID_VIEW, 0x0C03)
//
// //--- Function IDs -------------
// #define IDF_UCMREGIS                    MAKERESID(BID_VIEW, 0x0C10)
// #define IDF_LOADBIT                     MAKERESID(BID_VIEW, 0x0C11)
// #define IDF_SELCOL                      MAKERESID(BID_VIEW, 0x0C12)
// #define IDF_3DFRAM                      MAKERESID(BID_VIEW, 0x0C13)
// #define IDF_MAKETMP                     MAKERESID(BID_VIEW, 0x0C14)
// #define IDF_SAVETMP                     MAKERESID(BID_VIEW, 0x0C15)
// #define IDF_LOADTMP                     MAKERESID(BID_VIEW, 0x0C16)
// #define IDF_LOADMENU                    MAKERESID(BID_VIEW, 0x0C17)
// #define IDF_CLCITSZ                     MAKERESID(BID_VIEW, 0x0C18)
// #define IDF_CLCUNSCITSZ                 MAKERESID(BID_VIEW, 0x0C19)
// #define IDF_CLCBMPPOS                   MAKERESID(BID_VIEW, 0x0C1A)
// #define IDF_CLCTXTSZ                    MAKERESID(BID_VIEW, 0x0C1B)
// #define IDF_FREEMENU                    MAKERESID(BID_VIEW, 0x0C1C)
// #define IDF_CREATUCM                    MAKERESID(BID_VIEW, 0x0C1D)
// #define IDF_UCMenuSize                  MAKERESID(BID_VIEW, 0x0C1E)
// #define IDF_CHKSCROL                    MAKERESID(BID_VIEW, 0x0C1F)
// #define IDF_IDFROMP                     MAKERESID(BID_VIEW, 0x0C20)
// #define IDF_CTXTMENU                    MAKERESID(BID_VIEW, 0x0C21)
// #define IDF_UCMWNDPROC                  MAKERESID(BID_VIEW, 0x0C22)
// #define IDF_HIDEWNDPROC                 MAKERESID(BID_VIEW, 0x0C23)
// #define IDF_SUBMWNDPROC                 MAKERESID(BID_VIEW, 0x0C24)
// #define IDF_MENUWNDPROC                 MAKERESID(BID_VIEW, 0x0C25)
// #define IDF_UCMSETTDLGPROC              MAKERESID(BID_VIEW, 0x0C26)
// #define IDF_PG1DLGPROC                  MAKERESID(BID_VIEW, 0x0C27)
// #define IDF_INITPG1                     MAKERESID(BID_VIEW, 0x0C28)
// #define IDF_PG2DLGPROC                  MAKERESID(BID_VIEW, 0x0C29)
// #define IDF_INITPG2                     MAKERESID(BID_VIEW, 0x0C2A)
// #define IDF_PG0DLGPROC                  MAKERESID(BID_VIEW, 0x0C2B)
// #define IDF_INITPG0                     MAKERESID(BID_VIEW, 0x0C2C)
// #define IDF_INITPG0                     MAKERESID(BID_VIEW, 0x0C2D)
// #define IDF_NBPG1OK                     MAKERESID(BID_VIEW, 0x0C2E)
// #define IDF_NBPG2OK                     MAKERESID(BID_VIEW, 0x0C2F)
// #define IDF_STYLEDLGPROC                MAKERESID(BID_VIEW, 0x0C30)
// #define IDF_INITSTYLDLG                 MAKERESID(BID_VIEW, 0x0C31)
// #define IDF_STYLDLGOK                   MAKERESID(BID_VIEW, 0x0C32)
// #define IDF_UPDMENU                     MAKERESID(BID_VIEW, 0x0C33)
// #define IDF_UNSELALL                    MAKERESID(BID_VIEW, 0x0C34)
// #define IDF_NEWID                       MAKERESID(BID_VIEW, 0x0C35)
// #define IDF_NBPG0OK                     MAKERESID(BID_VIEW, 0x0C36)
// #define IDF_EXECPRGM                    MAKERESID(BID_VIEW, 0x0C37)
// #define IDF_REPMENU                     MAKERESID(BID_VIEW, 0x0C38)
// #define IDF_INITDRAG                    MAKERESID(BID_VIEW, 0x0C39)
// #define IDF_DRGDRPMSG                   MAKERESID(BID_VIEW, 0x0C3A)
// #define IDF_RMVEMPH                     MAKERESID(BID_VIEW, 0x0C3B)
// #define IDF_DRGOVER                     MAKERESID(BID_VIEW, 0x0C3C)
// #define IDF_DROP                        MAKERESID(BID_VIEW, 0x0C3D)
// #define IDF_PPCHG                       MAKERESID(BID_VIEW, 0x0C3E)
// #define IDF_GETEXEPATH                  MAKERESID(BID_VIEW, 0x0C3F)
// #define IDF_BMPPATH                     MAKERESID(BID_VIEW, 0x0C40)
// #define IDF_BMPFULLNAME                 MAKERESID(BID_VIEW, 0x0C41)
// #define IDF_MAXITSZ                     MAKERESID(BID_VIEW, 0x0C42)
// #define IDF_TRNACC                      MAKERESID(BID_VIEW, 0x0C43)
// #define IDF_ADDACC                      MAKERESID(BID_VIEW, 0x0C44)
// #define IDF_REMACC                      MAKERESID(BID_VIEW, 0x0C45)
// #define IDF_BMPFILEDLGPROC              MAKERESID(BID_VIEW, 0x0C46)
// #define IDF_SUBMEMSTRDUP                MAKERESID(BID_VIEW, 0x0C47)
// #define IDF_UCMALLOC                    MAKERESID(BID_VIEW, 0x0C48)
// #define IDF_UCMFREE                     MAKERESID(BID_VIEW, 0x0C49)
// #define IDF_EDITBMPFILE                 MAKERESID(BID_VIEW, 0x0C4A)
// #define IDF_EDITBMPFILET2               MAKERESID(BID_VIEW, 0x0C4B)
// #define IDF_UCMNBP1UPDBMP               MAKERESID(BID_VIEW, 0x0C4C)
// #define IDF_CREATEBMPDLGPROC            MAKERESID(BID_VIEW, 0x0C4D)
// #define IDF_SETBMPBG                    MAKERESID(BID_VIEW, 0x0C4E)
// #define IDF_DISPLAYHLP                  MAKERESID(BID_VIEW, 0x0C4F)
// #define IDF_UCMLOADITEMBMP              MAKERESID(BID_VIEW, 0x0C50)
// #define IDF_CREATEBMPFILE               MAKERESID(BID_VIEW, 0x0C51)
// #define IDF_NLSGETMSG                   MAKERESID(BID_VIEW, 0x0C52)
// #define IDF_MAKETMP2                    MAKERESID(BID_VIEW, 0x0C53)
// #define IDF_TMPLEN                      MAKERESID(BID_VIEW, 0x0C54)
// #define IDF_UCMNEW                      MAKERESID(BID_VIEW, 0x0C55)
// #define IDF_UCMCREATERES                MAKERESID(BID_VIEW, 0x0C56)
// #define IDF_UCMCREATETMPL               MAKERESID(BID_VIEW, 0x0C57)
// #define IDF_UCMCREATE                   MAKERESID(BID_VIEW, 0x0C58)
// #define IDF_RESBMPDLGPROC               MAKERESID(BID_VIEW, 0x0C59)
// #define IDF_UCMCREATEHMENU              MAKERESID(BID_VIEW, 0x0C5A)
// #define IDF_IDFROMACTION                MAKERESID(BID_VIEW, 0x0C5B)
// #define IDF_IDFROMACTION2               MAKERESID(BID_VIEW, 0x0C5C)
// #define IDF_SETACTIONATTR               MAKERESID(BID_VIEW, 0x0C5D)
// #define IDF_LOADRESBMP                  MAKERESID(BID_VIEW, 0x0C5E)
// #define IDF_SAVESTL                     MAKERESID(BID_VIEW, 0x0C5F)
// #define IDF_LOADSTL                     MAKERESID(BID_VIEW, 0x0C60)
// #define IDF_UCMLOADDEF                  MAKERESID(BID_VIEW, 0x0C61)
// #define IDF_UCMDLGPROC                  MAKERESID(BID_VIEW, 0x0C62)
// #define IDF_RESBUFFETDLG                MAKERESID(BID_VIEW, 0x0C63)
// #define IDF_TMPLBUFFETDLG               MAKERESID(BID_VIEW, 0x0C64)
// #define IDF_SHRDDRWNDPROC               MAKERESID(BID_VIEW, 0x0C65)
//
// //--- Messages IDs --------------------------
// #define IDM_UCMQRYMOD                   MAKERESID(BID_VIEW, 0x0C90)
// #define IDM_UCMQRYDEFID                 MAKERESID(BID_VIEW, 0x0C91)
// #define IDM_UCMQRYACTLST                MAKERESID(BID_VIEW, 0x0C92)
// #define IDM_QRYSIZE                     MAKERESID(BID_VIEW, 0x0C93)
// #define IDM_QRYCLR                      MAKERESID(BID_VIEW, 0x0C94)
// #define IDM_QRYFONT                     MAKERESID(BID_VIEW, 0x0C95)
// #define IDM_QRYSTYL                     MAKERESID(BID_VIEW, 0x0C96)
// #define IDM_QRYFRCSZ                    MAKERESID(BID_VIEW, 0x0C97)
// #define IDM_ADDTOCM                     MAKERESID(BID_VIEW, 0x0C98)
// #define IDM_CMSELECT                    MAKERESID(BID_VIEW, 0x0C99)
// #define IDM_DELCMITEM                   MAKERESID(BID_VIEW, 0x0C9A)
// #define IDM_QRYVERSION                  MAKERESID(BID_VIEW, 0x0C9B)
// #define IDM_MOUSEMOVE                   MAKERESID(BID_VIEW, 0x0C9C)
// #define IDM_UCMINSERTACT                MAKERESID(BID_VIEW, 0x0C9D)
// #define IDM_UCMSETSTYLE                 MAKERESID(BID_VIEW, 0x0C9E)
// #define IDM_QRYUCMINFO                  MAKERESID(BID_VIEW, 0x0C9F)
// #define IDM_UCMUPDATE                   MAKERESID(BID_VIEW, 0x0CA0)
// #define IDM_SETBGCOLOR                  MAKERESID(BID_VIEW, 0x0CA1)
// #define IDM_UCMQRYRESBMP                MAKERESID(BID_VIEW, 0x0CA2)
// #define IDM_UCMACTSINSERTED             MAKERESID(BID_VIEW, 0x0CA3)
// #define IDM_UCMCHGSIZE                  MAKERESID(BID_VIEW, 0x0CA4)
// #define IDM_UCMCHGSTYLE                 MAKERESID(BID_VIEW, 0x0CA5)
// #define IDM_UCMCHGFONT                  MAKERESID(BID_VIEW, 0x0CA6)
// #define IDM_UCMCHGCOLOR                 MAKERESID(BID_VIEW, 0x0CA7)
// #define IDM_UCMCHGADDED                 MAKERESID(BID_VIEW, 0x0CA8)
// #define IDM_UCMCHGDELETED               MAKERESID(BID_VIEW, 0x0CA9)
// #define IDM_UCMCHGBITMAP                MAKERESID(BID_VIEW, 0x0CAA)
// #define IDM_UCMCHGACTION                MAKERESID(BID_VIEW, 0x0CAB)
// #define IDM_HLPNBCREATE                 MAKERESID(BID_VIEW, 0x0CAC)
// #define IDM_HLPNBBMP                    MAKERESID(BID_VIEW, 0x0CAD)
// #define IDM_HLPNBACTION                 MAKERESID(BID_VIEW, 0x0CAE)
// #define IDM_HLPSTYLE                    MAKERESID(BID_VIEW, 0x0CAF)
// #define IDM_HLPDM                       MAKERESID(BID_VIEW, 0x0CB0)
// #define IDM_HLPBUFFET                   MAKERESID(BID_VIEW, 0x0CB1)
// #define IDM_UCMQRYDEFTMPLT              MAKERESID(BID_VIEW, 0x0CB2)
// #define IDM_SETFONT                     MAKERESID(BID_VIEW, 0x0CB3)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Description
//----------------------------------------------------------------------------
// :comp res=&IDC_UCMENUSCOMP..User Customizable Menus
// :h1. User-Customizable Menu Control
// :h2. Description
// :p.
// The :hp3.User Customizable Menu:ehp3. is a very powerful, new user interface
// component, designed to overcome the problems with current menu and toolbar
// designs.
// It allows the user to define the ordering of menu items, the
// visual representation (artwork and text) of menu items. New items, like
// "macro assignments" can be added dynamically. A user customizable menu can
// be saved in its present state for later use. Hence it is feasible to create
// several versions of a menu. On demand they can be reloaded (without having
// to quit the application). The user customizable menu changes itself into a
// "scrolling" mode when the number of selectable items exceeds the
// allotted window size.
// .*----------------------------------------------------------------------------
// .* Arrangements
// .*----------------------------------------------------------------------------
// :h2.Arrangements
// :p.
// User customizable menus can be displayed in three different arrangements:
// :ul compact.
// :li.
// First, we have the traditional horizontal menu bar style. If the
// number of items do not fit, scrolling indicators are shown. This is contrary
// to the style of menu bars which would wrap.
// .config section1 on
// .when 'PC' delete
// :fig id=HUCM place=inline frame=box width=column.
// :artwork name=HUCM align=center.
// .when 'PC' insert
// :fig.
// :artwork name='HUCM.BMP'.
// .config section1 off
// :figcap.User Customizable Menu
// :figdesc.Horizontal arrangement with scrolling indicators.
// :efig.
// :p.
// :li.
// Second, display as a vertical column is possible. This is a very useful
// arrangement to display a small set of tools to the left or rightof a
// window.
// .config section1 on
// .when 'PC' delete
// :fig id=VUCM place=inline frame=box width=column.
// :artwork name=VUCM align=center.
// .when 'PC' insert
// :fig.
// :artwork name='VUCM.BMP'.
// .config section1 off
// :figcap.User Customizable Menu
// :figdesc.Vertical arrangement with an open pull-right menu.
// :efig.
// :p.
// :li.
// And third, a matrix arrangement, as it is typically used for tool palettes.
// .config section1 on
// .when 'PC' delete
// :fig id=MUCM place=inline frame=box width=column.
// :artwork name=MUCM align=center.
// .when 'PC' insert
// :fig.
// :artwork name='MUCM.BMP'.
// .config section1 off
// :figcap.User Customizable Menu
// :figdesc.Matrix arrangement.
// :efig.
// :eul.
// .*----------------------------------------------------------------------------
// .* User Manipulations
// .*----------------------------------------------------------------------------
// :h2.User Manipulations
// :p.
// The current version supports the following user interactions:
// :parml compact break=all.
// :pt.:hp2.Select:ehp2.
// :pd.Selection of user customizable menu items work exactly as for normal
// menus.
// :pt.:hp2.Context Menu:ehp2.
// :pd.Invoking context menus (e.g. by clicking the direct-manipulation mouse
// button) is new and has not been done for menus so far. In OS/2, a notebook
// dialog will appear and allow the user to change the artwork and the text of
// the user customizable menu item.
// :pt.:hp2.Drag:ehp2.
// :pd.A user customizable menu item can be dragged to the OS/2 Shredder, which
// will delete it (including possible sub-items). It can also be dragged to
// another place in the same menu or to another user customizable menu, which
// will result in a move of that item to the new position. A move can be augmented
// with the Ctrl key to change it to a copy operation. It can also be augmented
// by pressing the Alt key, this will do a copy to the submenu of the item, which 
// will be created if it did not exist. Items can be dropped right
// or left of another item in an horizontal or matrix UCMenu and top or bottom in
// a vertical UCMenu.
// :pt.:hp2.Presentation parameters:ehp2.
// :pd.Colors and fonts can be dragged from the system palettes and dropped on the
// ucmenu to change its appearance.
// :pt.:hp2.Bitmap files:ehp2.
// :pd. A bitmap file can be dropped on  a ucmenu, this will create a new item using 
//  the bitmap.
// :eparml.
// .*----------------------------------------------------------------------------
// .* Tech Details
// .*----------------------------------------------------------------------------
// :h2.Technical Details
// :p.
// Technically the user customizable menu combines a menu and a scrollbar control.
// The user customizable menu window contains inside a scrollbar which exactly
// fits into it. This scrollbar is shown if the number of items in the menu exceeds
// the user customizable menu window size. The menu itself is shown as the shaft
// of the scrollbar (see below). For the programmer who uses this control the
// knowledge about this window hierarchy is not needed. To the outside the UCMenu
// appears as 'the menu' and messages are routed accordingly by the UCMenu code.
// :pcxmp.
//    UC Menu
//  ÚÄÄÄÄÄÄÄÄÄÄ¿
//  ³ÚÄÄÄÄÄÄÄÄ¿³
//  ³³   /\   ³³
//  ³³ Scroll ³³
//  ³ÃÄÄÄÄÄÄÄÄ´³
//  ³³ÚÄÄÄÄÄÄ¿³³
//  ³³³      ³³³
//  ³³³ Menu ³³³
//  ³³³      ³³³
//  ³³³      ³³³
//  ³³³      ³³³
//  ³³³      ³³³
//  ³³³      ³³³
//  ³³ÀÄÄÄÄÄÄÙ³³
//  ³ÃÄÄÄÄÄÄÄÄ´³
//  ³³  Bar   ³³
//  ³³   \/   ³³
//  ³ÀÄÄÄÄÄÄÄÄÙ³
//  ÀÄÄÄÄÄÄÄÄÄÄÙ
// :epcxmp.
// :p.
// The UCMenu is the owner of the menu window and therefore receives all the messages.
// It forwards the following messages to its owner. They are
// described in PM Reference Guide (Menu Control Notification Messages)&colon.
// :parml compact break=all.
// :pt.:hp2.WM_HELP:ehp2.
// :pd.SHORT2FROMMP(mp1) = UCMenuID, SHORT1FROMMP(mp2) = CMDSRC_UCMENU
// :pt.:hp2.WM_INITMENU:ehp2.
// :pd.
// :pt.:hp2.WM_MENUEND:ehp2.
// :pd.mp2 = hwndUCMenu
// :pt.:hp2.WM_MENUSELECT:ehp2.
// :pd.mp2 = hwndUCMenu
// :pt.:hp2.WM_NEXTMENU:ehp2.
// :pd.Should be handled correctly if you want to put the UCMenu in the same ring as a title
// bar and system menu for instance.
// :eparml.
// :p.
// The items for which a bitmap can not be found are processed like regular menu items. This
// means that the parameters associated to the previous messages are not changed for them.
// :p.
// The following messages are processed by the UCMenu and not forwarded&colon.
// .config section1 on
// .when 'PC' delete
// :fn.This might change in the future!:efn.
// .when 'PC' insert
// :fn id=FN1.This might change in the future!:efn.
// :link reftype=fn refid=FN1.*:elink.
// .config section1 off
// :dl compact.
// :dt.:hp2.WM_DRAWITEM:ehp2.
// :dd.
// :dt.:hp2.WM_MEASUREITEM:ehp2.
// :dd.
// :edl.
// :p.
// The Menu Control Window Messages (MM_*) sent to the UCMenu are forwarded to
// the menu window.
// :hp2.MM_QUERYITEMTEXT:ehp2., :hp2.MM_QUERYITEMTEXTLENGTH:ehp2.
// and :hp2.MM_SETITEMTEXT:ehp2., which usually only work for items with a MIS_TEXT style, can
// be used with the MIS_OWNERDRAW items to query and set the text displayed in the bitmap.
// .*-------------------------------------------------------------------------
// :h2.Styles
// :p.
// A UCMenu can have a number of different styles that can be set on creation in
// the UCMINFO structure and changed dynamically through the context menu &colon.
// :parml compact break=all.
// :pt.:hp2.UCS_NOTEXT:ehp2.
// :pd.The text associated with the item is not displayed on the screen.
// :pt.:hp2.UCS_FRAMED:ehp2.
// :pd.When this is set, a frame is drawn around each item so that it looks like a 3-D
// button.
// :pt.:hp2.UCS_FORCESIZE:ehp2.
// :pd.The size of all items can be forced to a certain rectangle. If this is set, the size
// has to be specified in the UCMINFO structure (UCMInfo.cx and UCMInfo.cx)
// :pt.:hp2.UCS_SHRINK_BLEND:ehp2.
// :pd.When set, either BBO_OR or BBO_AND is used to shrink the bitmaps, else BBO_IGNORE is used
// :pt.:hp2.UCS_SHRINK_OR:ehp2.
// :pd.Used when BBO_IGNORE not selected. If set BBO_OR is used, else BBO_AND
// :pt.:hp2.UCS_NO_DM_ALT:ehp2.
// :pd.No drag and drop copy to a submenu when doing an Alt-drop over the ucmenu
// :pt.:hp2.UCS_NO_DM_M_TO_OTHER:ehp2.
// :pd.No drag and drop move to another ucmenu
// :pt.:hp2.UCS_NO_DM_M_FROM_OTHER:ehp2.
// :pd.No drag and drop move from another ucmenu
// :pt.:hp2.UCS_NO_DM_M_INSIDE:ehp2. 
// :pd.No drag and drop move inside this ucmenu
// :pt.:hp2.UCS_NO_DM_C_TO_OTHER:ehp2.
// :pd.No drag and drop copy to another ucmenu
// :pt.:hp2.UCS_NO_DM_C_FROM_OTHER:ehp2.
// :pd.No drag and drop copy from another ucmenu
// :pt.:hp2.UCS_NO_DM_C_INSIDE:ehp2. 
// :pd.No drag and drop copy inside this ucmenu
// :pt.:hp2.UCS_NO_DM_DISCARD:ehp2.  
// :pd.No drag and drop to the shredder
// :pt.:hp2.UCS_NO_DM_RENDER_TO_BMP:ehp2.
// :pd.No drag and drop rendering as a BMP file
// :pt.:hp2.UCS_NO_DM_RENDER_FROM_BMP:ehp2.
// :pd.No drag and drop from a BMP file
// :pt.:hp2.UCS_NO_DM:ehp2.          
// :pd.No direct manipulation at all
// :pt.:hp2.UCS_NO_CM_MENU_STYLE:ehp2.
// :pd.No "change style" in the context menu
// :pt.:hp2.UCS_NO_CM_MENU_IMPORT:ehp2.
// :pd.No "import" in the context menu
// :pt.:hp2.UCS_NO_CM_MENU_EXPORT:ehp2.
// :pd.No "export" in the context menu
// :pt.:hp2.UCS_NO_CM_MENU_DEFAULT:ehp2.
// :pd.No "load default" in the context menu
// :pt.:hp2.UCS_NO_CM_ITEM_EDIT:ehp2.
// :pd.No "edit item" in the context menu
// :pt.:hp2.UCS_NO_CM_ITEM_CREATE:ehp2.
// :pd.No "create item" in the context menu
// :pt.:hp2.UCS_NO_CM_ITEM_DELETE:ehp2.
// :pd.No "delete item" in the context menu
// :pt.:hp2.UCS_NO_CM:ehp2.          
// :pd.No context menu
// :pt.:hp2.UCS_CHNGBMPBG:ehp2.
// :pd. Allow the changing of the background color of the bitmaps.
// :pt.:hp2.UCS_CUSTOMHLP:ehp2.
// :pd.The help is provided by the user.
// :pt.:hp2.UCS_PROMPTING:ehp2.
// :pd.UCN_MOUSEMOVE notifications are sent to the owner.
// :pt.:hp2.UCS_NODEFAULTACTION:ehp2.
// :pd.The list of actions does not include executeprogram by default.
// :eparml.
// :pcxmp.

// ----------------------------------------------------------
// -- Those styles can be changed by the user (style dialog)
// ----------------------------------------------------------
   #define UCS_FORCESIZE     0x0004       // -- Force the size of the items ( see UCMinfo.cx and UCMInfo.cy )
   #define UCS_FRAMED        0x0008       // -- Draw a 3D frame around the items
   #define UCS_NOTEXT        0x0010       // -- Do not display the text of the items
   #define UCS_SHRINK_BLEND  0x0020       // -- When the bmp is shrunk, do not use BBO_IGNORE
   #define UCS_SHRINK_OR     0x0040       // -- set : use BBO_OR, else : use BBO_AND

// ------------------------------------------------------------------------------
// -- Those styles can not be changed by the user (only used at ucmenu creation)
// ------------------------------------------------------------------------------
   #define UCS_NO_DM_ALT                   0x00000002  // -- no drg drp copy to submenu when over the menu
   #define UCS_NO_DM_M_TO_OTHER            0x00000080  // -- no drg drp move to another ucmenu
   #define UCS_NO_DM_M_FROM_OTHER          0x00000100  // -- no drg drp move from another ucmenu
   #define UCS_NO_DM_M_INSIDE              0x00000200  // -- no drg drp move inside this ucmenu
   #define UCS_NO_DM_C_TO_OTHER            0x00000400  // -- no drg drp copy to another ucmenu
   #define UCS_NO_DM_C_FROM_OTHER          0x00000800  // -- no drg drp copy from another ucmenu
   #define UCS_NO_DM_C_INSIDE              0x00001000  // -- no drg drp copy inside this ucmenu
   #define UCS_NO_DM_DISCARD               0x00002000  // -- no drg drp to the shredder
   #define UCS_NO_DM_RENDER_TO_BMP         0x00004000  // -- no drg drp rendering as a BMP file
   #define UCS_NO_DM_RENDER_FROM_BMP       0x00008000  // -- no drg drp from a BMP file
   #define UCS_NO_DM                       0x00010000  // -- no direct manipulation at all
                                                   
   #define UCS_NO_CM_MENU_STYLE            0x00020000  // -- no "change style" in the CM
   #define UCS_NO_CM_MENU_IMPORT           0x00040000  // -- no "import" in the CM
   #define UCS_NO_CM_MENU_EXPORT           0x00080000  // -- no "export" in the CM
   #define UCS_NO_CM_MENU_DEFAULT          0x00100000  // -- no "load default" in the CM
   #define UCS_NO_CM_ITEM_EDIT             0x00200000  // -- no "edit item" in the CM
   #define UCS_NO_CM_ITEM_CREATE           0x00400000  // -- no "create item" in the CM
   #define UCS_NO_CM_ITEM_DELETE           0x00800000  // -- no "delete item" in the CM
   #define UCS_NO_CM                       0x01000000  // -- no context menu
                                           
   #define UCS_NOREORDER                   0x00C0BB82  // --   UCS_NO_DM_M_TO_OTHER
                                                       // -- | UCS_NO_DM_M_FROM_OTHER
                                                       // -- | UCS_NO_DM_M_INSIDE
                                                       // -- | UCS_NO_DM_C_FROM_OTHER
                                                       // -- | UCS_NO_DM_C_INSIDE
                                                       // -- | UCS_NO_DM_DISCARD
                                                       // -- | UCS_NO_DM_RENDER_FROM_BMP
                                                       // -- | UCS_NO_CM_ITEM_CREATE
                                                       // -- | UCS_NO_CM_ITEM_DELETE
                                                       // -- | UCS_NO_DM_ALT

   #define UCS_REORDERONLY                 0x00C0FD82  // --   UCS_NO_DM_M_TO_OTHER
                                                       // -- | UCS_NO_DM_M_FROM_OTHER
                                                       // -- | UCS_NO_DM_C_TO_OTHER
                                                       // -- | UCS_NO_DM_C_FROM_OTHER
                                                       // -- | UCS_NO_DM_C_INSIDE       
                                                       // -- | UCS_NO_DM_DISCARD        
                                                       // -- | UCS_NO_DM_RENDER_TO_BMP  
                                                       // -- | UCS_NO_DM_RENDER_FROM_BMP
                                                       // -- | UCS_NO_CM_ITEM_CREATE
                                                       // -- | UCS_NO_CM_ITEM_DELETE
                                                       // -- | UCS_NO_DM_ALT

   #define UCS_STATIC                      0x01010000  // --   UCS_NO_DM | UCS_NO_CM

   #define UCS_NOITEMSMODIF                0x00E0F982  // --   UCS_NO_DM_M_TO_OTHER
                                                       // -- | UCS_NO_DM_M_FROM_OTHER          
                                                       // -- | UCS_NO_DM_C_FROM_OTHER          
                                                       // -- | UCS_NO_DM_C_INSIDE              
                                                       // -- | UCS_NO_DM_DISCARD               
                                                       // -- | UCS_NO_DM_RENDER_TO_BMP         
                                                       // -- | UCS_NO_DM_RENDER_FROM_BMP       
                                                       // -- | UCS_NO_CM_ITEM_EDIT             
                                                       // -- | UCS_NO_CM_ITEM_CREATE           
                                                       // -- | UCS_NO_CM_ITEM_DELETE           
                                                       // -- | UCS_NO_DM_ALT

   #define UCS_CHNGBMPBG                   0x10000000  // -- Change the background color of the bitmaps
   #define UCS_CUSTOMHLP                   0x20000000  // -- The help is provided by the user
   #define UCS_PROMPTING                   0x40000000  // -- Post the UCN_MOUSEMOVE notifications
   #define UCS_NODEFAULTACTION             0x80000000  // -- No execute action by default


// :epcxmp.
// :p.The UCM text font and background color can be set on creation, through the UCMINFO
// structure. The default font is Helv 8. Note that the color corresponds to the RGB index
// and can look different on VGAs or 8514 displays. You should use the index returned by
// GpiQueryRGBColor.
// :pcxmp.

   #define UCM_DEFAULTCOLOR     0x00C9C9C9L   // -- light grey on 8514
   #define UCM_DEFAULTFONT      "8.Helv"

// :epcxmp.
// .*-------------------------------------------------------------------------
// :h2.Creation of a UCMenu.
// :p.
// To create a User-Customizable Menu, do the following&colon.
// :ol compact.
// :li.Fill the :link reftype=hd res=&IDT_UCMINFO..UCMINFO structure:elink.
// with the initialization parameters
// :li.Use UCMenuCreateFromResource or UCMenuCreateFromTemplate to create a UCMenu. Give the style and
// a pointer to the :link reftype=hd res=&IDT_UCMINFO..UCMINFO structure:elink.
// :eol.
// Sample&colon.
// :xmp.
//
// :rev refid=R3.
// memset( &amp.UCMInfo, 0, sizeof(UCMInfo));
// UCMInfo.cb       = sizeof(UCMInfo);
// :erev refid=R3.
// UCMInfo.hModule  = (HMODULE)0;        // Look in the exe file
// UCMInfo.Style    = UCS_FRAMED |       // Draw a 3D frame around the items
//                    UCS_CHNGBMPBG;     // Use one of the bitmap colors as a background color
// // The background color of the sample bitmaps is CLR_PALEGRAY
// // But the corresponding RGB value depends on the display adapter
// // So we retrieve it dynamically
// // --
// hps               = WinGetPS( hwndFrame );
// // --  Set the color of the bitmaps which has to be replaced by the items background color
// UCMInfo.BgBmp     = GpiQueryRGBColor( hps, 0, CLR_PALEGRAY ); 
// // -- Set the color of the toolbar background
// UCMInfo.BgColor   = GpiQueryRGBColor( hps, 0, CLR_DARKGRAY );
// // -- Set the color of the items background
// UCMInfo.ItemBgColor = 0x00b0b0b0;
// WinReleasePS( hps );
// UCMInfo.pszFontNameSize = "8.Helv";
// 
// hwndVertUCM = UCMenuCreateFromResource(
//                               hab,
//                               hwndClient,
//                               hwndClient,
//                               CMS_VERT,
//                               0,
//                               0,
//                               0,
//                               0,
//                               HWND_TOP,
//                               ID_MENU,    // ID of the ucmenu, could be different from the menu id
//                               (HMODULE)0, // module of the menu resource
//                               ID_MENU,    // ID of the menu resource
//                               (PVOID)&UCMInfo,
//                               &hTmp );
//
// :exmp.
// The following styles can be used&colon.
// :dl compact.
// :dt.:hp2.CMS_VERT:ehp2.
// :dd.Vertical UCMenu
// :dt.:hp2.CMS_HORZ:ehp2.
// :dd.Horizontal UCMenu
// :dt.:hp2.CMS_MATRIX:ehp2.
// :dd.Matrix UCMenu
// :edl.
// :p.
// A UCMenu is autosized &colon.
// :ol compact.
// :li. The width is set to fit the widest item if the style is CMS_VERT or CMS_MATRIX.
// :li. The height is set to fit the tallest item if the style is CMS_HORZ or CMS_MATRIX.
// :eol.
// :p.
// :note. UCMenus.h contains the necessary types and constant definitions.
// :enote.
// .*-------------------------------------------------------------------------
// :h2 id=DECL.Declaration of a UCMenu in a resource file
// A UCMenu is based on a standard PM Menu control. However, in order to use
// the special features, like the association of an action with each item,
// the declaration is slightly different.
// :p.
// The bitmap items have to be declared with the MIS_TEXT style initially. The
// format for the text field is the following&colon.
// "&lbrk.#Text&rbrk.#BitmapID&lbrk.#Action&rbrk.&lbrk.#Parameters&rbrk.&lbrk.#Data&rbrk.".
// The first character of the string (here #) is used as a delimiter
// :parml compact break=all.
// :pt.:hp2.Text:ehp2.
// :pd.The text displayed in or under the bitmap
// :pt.:hp2.BitmapID:ehp2.
// :pd.Bitmap identifier or file name
// :pt.:hp2.Action:ehp2.
// :pd.Information about the action associated with the item , for example the name of a macro.
// :pt.:hp2.Parameters:ehp2.
// :pd.Parameters associated to the action.
// :pt.:hp2.Data:ehp2.
// :pd.Some text associated with the item , for example the parameters of a macro.
// :eparml.
// :p.
// The MIS_BREAK style is not effective for the first level UCMenu items, but it
// works for the submenu items.
// :p.
// The MIS_SPACER style creates unselectable items  without 3D frames.
// :p.
// Only one level of submenus is supported.
// :p.Sample of resource file
// :pcxmp.
// #define  INCL_PM
// #include <os2.h>
// #include "UCMenus.h"
//
// BITMAP 2000 styles.bmp
// BITMAP 2001 bold.bmp
// BITMAP 2002 italic.bmp
// BITMAP 2003 fonts.bmp
//
// MENU IDD_MENU
// BEGIN
//   SUBMENU "Styles#2000",  IDM_STYLES, MIS_TEXT
//   BEGIN
//      MENUITEM "Bold#2001#Style bold",                   IDM_BOLD,      MIS_TEXT 
//      MENUITEM "Italic#2002#Style italic",               IDM_ITALIC,    MIS_TEXT 
//      MENUITEM "Underline#underlin.bmp#Style underline", IDM_UNDERLINE, MIS_TEXT 
//   END
//   SUBMENU "Fonts#2003",    IDM_FONTS,  MIS_TEXT
//   BEGIN
//      MENUITEM "Helv",      IDM_HELV,   MIS_TEXT      // These are standard text items
//      MENUITEM "Times Rmn", IDM_TMSRMS, MIS_TEXT      // (no bitmap id is given).
//      MENUITEM "Symbols"  , IDM_SYMB,   MIS_TEXT | MIA_DISABLED
//   END
// END
// :epcxmp.
// :note. During the creation of a UCMenu, such items are transformed and become
// ownerdrawn. The :hp1.hItem:ehp1. field of the MENUITEM structure contains a pointer
// to a :link reftype=hd res=&IDT_UCMITEM..UCMITEM data structure:elink.. The MM_QUERYITEM, MM_QUERYITEMHANDLE, MM_QUERYITEMTEXTLENGTH and
// MM_QUERYITEMTEXT messages can still be used to query the information.
// :enote.
// .*---------------------------------------------------------------------
// :h2.Sample
// :p. This sample creates a horizontal UCMenu.
// :pcxmp.
// #define INCL_BASE
// #define INCL_PM
// #include <os2.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "sample.h"
// #include "UCMenus.h"
//
// HWND     hwndHorzUCM;
//
// MRESULT EXPENTRY  ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
//
// :epcxmp.
// :pcxmp.
// void main()
// { HAB    hab;
//   HMQ    hmq;
//   QMSG   qmsg;
//   ULONG  pmStyle;
//   HWND   hwndFrame, hwndClient;
//   HWND   hTmp;
//   RECTL  rcl;
//   SWP    swp;
//   UCMINFO UCMInfo;
// :epcxmp.
// :pcxmp.
//   //--- PM Initialization
//   hab=WinInitialize(0);
//   hmq=WinCreateMsgQueue(hab, 0x100);
//
// :epcxmp.
// :pcxmp.
//   //--- Create Frame window
//   WinRegisterClass(hab, (PSZ)"MENUTEST", (PFNWP)ClientWndProc, CS_SIZEREDRAW, 0);
//   pmStyle = FCF_SIZEBORDER    |
//             FCF_TITLEBAR      |
//             FCF_MINMAX        |
//             FCF_SYSMENU       |
//             FCF_SHELLPOSITION |
//             FCF_MENU          |
//             FCF_TASKLIST;
//
// :epcxmp.
// :pcxmp.
//   hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
//                                  FS_SIZEBORDER | FS_SHELLPOSITION | FS_TASKLIST,
//                                  &amp.pmStyle,
//                                  (PSZ)"MENUTEST",
//                                  (PSZ)"Sample of User-Customizable Menu",
//                                  0L,
//                                  (HMODULE)NULL,
//                                  ID_MAIN,
//                                  (PHWND)&amp.hwndClient);
//
// :epcxmp.
// :pcxmp.
//   //--- Show Frame window
//   WinSetWindowPos (hwndFrame,
//                    HWND_TOP,
//                    0,
//                    0,
//                    0,
//                    0,
//                    SWP_SHOW | SWP_ZORDER);
// :epcxmp.
// :pcxmp.
//   WinQueryWindowPos(hwndClient, &amp.swp);
//
// :epcxmp.
// :pcxmp.
//   //--- Create a UCMenu window
//   memset( &amp.UCMInfo, 0, sizeof(UCMInfo));
//   UCMInfo.hModule         = (HMODULE)0;
//   UCMInfo.BgColor      = GpiQueryRGBColor( hps, 0, SYSCLR_MENU ); 
//   UCMInfo.ItemBgColor  = GpiQueryRGBColor( hps, 0, SYSCLR_MENU ); 
//   UCMInfo.Style        = UCS_FRAMED;  // Draw a 3D frame around the items
//
// :epcxmp.
// :pcxmp.
//   
//   hwndHorzUCM = UCMenuCreateFromResource(
//                                 hab,
//                                 hwndClient,
//                                 hwndClient,
//                                 CMS_HORZ,
//                                 0,
//                                 0,
//                                 0,                 
//                                 0,
//                                 HWND_TOP,
//                                 ID_MENU,    // ID of the ucmenu, could be different from the menu id
//                                 (HMODULE)0, // module of the menu resource
//                                 ID_MENU,    // ID of the menu resource
//                                 (PVOID)&UCMInfo,
//                                 &hTmp );
//   :epcxmp.
//   :pcxmp.
//   //--- Dispatch messages
//   while(WinGetMsg(hab, (PQMSG)&amp.qmsg, (HWND)NULL, 0, 0)) {
//     WinDispatchMsg(hab, (PQMSG)&amp.qmsg);
//   } /* endwhile */
//
// :epcxmp.
// :pcxmp.
//   //--- Terminate
//   WinDestroyWindow(hwndFrame);
//   WinDestroyMsgQueue(hmq);
//   WinTerminate(hab);
// }
// :epcxmp.
// :pcxmp.
// //--------------------------------------------------------------------------
// // Client window procedure
// //--------------------------------------------------------------------------
// MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
// {
//   switch (msg) {
//     case WM_COMMAND:
//       switch (SHORT1FROMMP(mp1)) {
//         case IDM_EXIT:
//           WinSendMsg(hwnd, WM_CLOSE, (MPARAM)0, (MPARAM)0);
//           break;
//       } /* endswitch */
//       break;
//
// :epcxmp.
// :pcxmp.
//     case WM_PAINT:
//       { HPS hPS;
//         hPS = WinBeginPaint(hwnd, (HPS)NULL, (PRECTL)0);
//         GpiErase( hPS );
//         WinEndPaint( hPS );
//       } break;
//
// :epcxmp.
// :pcxmp.
//     case WM_ADJUSTWINDOWPOS:
//       { PSWP pswp = (PSWP)mp1;
//
//         if (pswp->fl &amp. (SWP_MOVE | SWP_SIZE)) {
//           WinPostMsg(hwnd,
//                      WM_SIZE,
//                      (MPARAM)0,
//                      MPFROM2SHORT(pswp->cx, pswp->cy));
//         } /* endif */
//
//         if (pswp->fl &amp. SWP_ACTIVATE) {
//           pswp->fl &amp.= ~SWP_ACTIVATE;
//           WinSetWindowPos(hwnd,
//                           HWND_TOP,
//                           0,
//                           0,
//                           0,
//                           0,
//                           SWP_ZORDER | SWP_ACTIVATE);
//         } /* endif */
//       } break;
//
// :epcxmp.
// :pcxmp.
//     case WM_SIZE:
//       { SWP swp;
//         LONG dx = SHORT1FROMMP(mp2);
//
//         WinQueryWindowPos(hwndHorzUCM, &amp.swp)
//         if (dx) WinSetWindowPos(hwndHorzUCM,
//                                 HWND_TOP,
//                                 0,
//                                 0,
//                                 dx,
//                                 swp.cy,
//                                 SWP_SIZE | SWP_SHOW);
//
//       } break;
// :epcxmp.
// :pcxmp.
//     default:
//       return(WinDefWindowProc(hwnd, msg, mp1, mp2));
//   }
//   return (MRESULT)FALSE;
// }
// :epcxmp.
// :ecomp.
// .*-------------------------------------------------------------------------
// :type res=&IDT_UCMITEM. name='UCMITEM' text='Ownerdrawn Menu Item Structure'.
// This structure is used to store data related to an ownerdrawn menu item, such as
// its bitmap handle, identifier, text and an action string.
// :p.
// :pcxmp.
   typedef struct {
      LONG    hBmp;            // Bitmap handle
      PSZ     pszBmp;          // Bitmap Identifier or file name
      PSZ     pszText;         // Bitmap Text
      PSZ     pszAction;       // Action : for instance the name of a macro
      PSZ     pszParameters;   // Parameters for the action
      PSZ     pszData;         // Data associated to the item, not visible for the user
      USHORT  usId;            // Id of the item
      LONG    lCurBmpBgRGB;    // Current background color set for the bmp (if any)
      ULONG   ulFlags;
   } UCMITEM, *PUCMITEM;

   #define ITEM(a,b) ((PUCMITEM)((a)->hItem))->b
// :epcxmp.
// ulFlags can include &colon.
// :dl compact.
// :dt.:hp2.UCMI_BMPFROMFILE:ehp2.
// :dd.Set if the bitmap comes from afile, not set if it comes from the resources.
// :edl.
// :etype.
//-----------------------------------------------------------------------------
// :type res=&IDT_CMITEMS. name='CMITEMS' text='Context Menu items'.
// This structure is used to add items to the UCMenu's context menu.
// It contains the identifier and text of the item
// :p.
// :pcxmp.
   typedef struct {
      ULONG ID;
      PSZ   pszItemText;
   } CMITEMS,  *PCMITEMS;
// :epcxmp.
// :etype.
//-----------------------------------------------------------------------------
// :type res=&IDT_UCMINFO. name='UCMINFO' text='UCMenu Initialization Structure'.
// This structure is used to create a UCMenu. It must be passed to WinCreateWindow
// as the Control Data.
// :p.
// It contains the window handle of the PM menu used and the
// handle of the Module where the bitmaps are (if any).
// :p.If a matrix UCMenu is created with a null width, NbOfCols must contain the
// number of menu items per line. If it's created with a null height, NbOfRows
// must contain the number of items per column.
// :pcxmp.

   typedef struct {
      ULONG  cb;                   // size of this structure
      HMODULE hModule;             // Module where the bitmaps are
      USHORT  NbOfCols;            // Number of Columns (used for a matrix UCM)
      USHORT  NbOfRows;            // Number of Rows    (used for a matrix UCM)
      PSZ     pszBitmapSearchPath; // Path(s) to search for bitmap files.
                                   // Environment variables separated by spaces.
      PSZ     pszDefaultBitmap;    // Bitmap to use if bitmap specified for
                                   //  a menu item can't be resolved or loaded.
      ULONG   Style;               // Style of item (combination of UCS_ flags )
      ULONG   cx;                  // Size to give to all the items if UCS_FORCESIZE
      ULONG   cy;                  //
      LONG    BgBmp;               // RGB value of the color of the bmp which has to be replaced with ItemBgColor
      LONG    BgColor;             // RGB color of the UCMenu background
      LONG    ItemBgColor;         // RGB color of the UCMenu items background
      PSZ     pszFontNameSize;     // String describing the font of the UCMenu (such as used
   } UCMINFO, *PUCMINFO;           //  in WinSetPresParam), if NULL, UCM_DEFAULTFONT is used

// :epcxmp.
// :etype.

//----------------------------------------------------------------------------
// Messages Definitions
//----------------------------------------------------------------------------
// :message res=&IDM_UCMQRYMOD. name='UCN_QRYTEMPLATEMODULE' .
// This WM_CONTROL notification is sent by the ucmenu to its owner when the default template is loaded
// ("Load default option" in the popup context menu). The owner has to return the module
// handle where we can look for the default template.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_QRYTEMPLATEMODULE
// :param2.
// :item name='pHModule' type='HMODULE *'.
// Pointer to the module to be filled by the owner.
// :returns.
// :item name='Processed' type='BOOL'.
// TRUE if processed, FALSE else.
// :eparams.
// :remarks.
// :defproc.
// :related.
// UCN_QRYDEFTEMPLATEID
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMQRYDEFID. name='UCN_QRYDEFTEMPLATEID' .
// This WM_CONTROL notification is sent by the ucmenu to its owner when the default template is loaded
// ("Load default option" in the popup context menu). The owner has to return the ID
// of the menu in the resource.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_QRYDEFTEMPLATEID
// :param2.
// :item name='pID' type='PUSHORT'.
// Pointer to the ID to be filled by the owner.
// :returns.
// :item name='Processed' type='BOOL'.
// TRUE if processed, FALSE else.
// :eparams.
// :remarks.
// :defproc.
// :related.
// UCN_QRYTEMPLATEMODULE
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMQRYDEFTMPLT. name='UCN_QRYDEFTEMPLATE' .
// This WM_CONTROL notification is sent by the ucmenu to its owner when the default template has to be loaded
// ("Load default option" in the popup context menu). The owner has to return a pointer to the 
// template of the default toolbar, allocated with UCMenuAlloc.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_QRYDEFTEMPLATE
// :param2.
// :item name='ppTemplate' type='PVOID*'.
// Pointer to the pointer to the template.
// :returns.
// :item name='Processed' type='BOOL'.
// TRUE if processed, FALSE else.
// :eparams.
// :remarks.
// If processed, the template has to be allocated with UCMenuAlloc
// The ucmenus will free it with UCMenuFree
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMQRYACTLST. name='UCN_QRYACTIONLIST' .
// This WM_CONTROL notification is posted by the ucmenu to its owner when the settings dialog for the
// action is popped up so that he can fill in the list of actions.
// The owner has to send UCMENU_INSERTACTION messages and to post back a UCMENU_ACTIONSINSERTED
// when it is done.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_QRYACTIONSLIST
// :param2.
// :item name='hwndUCMenu' type='HWND'.
// Handle of the UCMenu.
// :returns.
// nothing
// :eparams.
// :remarks.
// :defproc.
// :related.
// UCMENU_INSERTACTION
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMACTSINSERTED. name='UCMENU_ACTIONSINSERTED' .
// This message is posted to the ucmenu by its owner when it has filled
// the list of actions
// :params.
// :param1.
// :item name='param1' type='ULONG'.
// Reserved.
// :param2.
// :item name='param2' type='ULONG'.
// Reserved
// :returns.
// nothing
// :eparams.
// :remarks.
// :defproc.
// :related.
// UCMENU_INSERTACTION
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMINSERTACT. name='UCMENU_INSERTACTION' .
// This message is sent to the UCMenu by its owner after it received UCMENU_ACTIONLIST
// :params.
// :param1.
// :item name='Action' type='PSZ'.
// String including the name of the action eventually followed by parameters.
// :param2.
// :item name='Description' type='PSZ'.
// A description of the action.
// :returns.
// nothing
// :eparams.
// :remarks.
// This message has to be sent
// :defproc.
// :related.
// UCMENU_ACTIONLIST
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMCHGSIZE. name='UCN_SIZE' .
// This WM_CONTROL notification is posted by the UCMenu to its owner when 
// its size has changed.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_SIZE
// :param1.
// :item name='NotifCode' type='USHORT'.
// :param2.
// :item name='param2' type='ULONG'.
// reserved
// :returns.
// Nothing
// :eparams.
// :remarks.
// After receiving this message, further information can be obtained by using the UCMENU_QUERY* messages.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMCHGSTYLE. name='UCN_STYLE' .
// This WM_CONTROL notification is posted by the UCMenu to its owner when 
// its style has changed.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_STYLE
// :param1.
// :item name='NotifCode' type='USHORT'.
// :param2.
// :item name='param2' type='ULONG'.
// reserved
// :returns.
// Nothing
// :eparams.
// :remarks.
// After receiving this message, further information can be obtained by using the UCMENU_QUERY* messages.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMCHGFONT. name='UCN_FONT' .
// This WM_CONTROL notification is posted by the UCMenu to its owner when 
// its font has changed (eg when a font has been dropped from the palette over the UCMenu ).
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_FONT
// :param2.
// :item name='param2' type='ULONG'.
// reserved
// :returns.
// Nothing
// :eparams.
// :remarks.
// After receiving this message, further information can be obtained by using the UCMENU_QUERY* messages.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMCHGCOLOR. name='UCN_COLOR' .
// This WM_CONTROL notification is posted by the UCMenu to its owner when 
// its color has changed (eg when a color has been dropped from the palette over the UCMenu ).
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_COLOR
// :param2.
// :item name='param2' type='ULONG'.
// reserved
// :returns.
// Nothing
// :eparams.
// :remarks.
// After receiving this message, further information can be obtained by using the UCMENU_QUERY* messages.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMCHGADDED. name='UCN_ADDEDITEM' .
// This WM_CONTROL notification is posted by the UCMenu to its owner when 
// an item has been added.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_ADDEDITEM
// :param2.
// :item name='ItemID' type='USHORT'.
// ID of the deleted item.
// :returns.
// Nothing
// :eparams.
// :remarks.
// After receiving this message, further information can be obtained by using the UCMENU_QUERY* messages.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMCHGDELETED. name='UCN_DELETEDITEM' .
// This WM_CONTROL notification is posted by the UCMenu to its owner when 
// an item has been deleted.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_DELETEDITEM
// :param2.
// :item name='ItemID' type='USHORT'.
// ID of the added item.
// :returns.
// Nothing
// :eparams.
// :remarks.
// After receiving this message, further information can be obtained by using the UCMENU_QUERY* messages.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMCHGBITMAP. name='UCN_BITMAP' .
// This WM_CONTROL notification is posted by the UCMenu to its owner when 
// the bitmap of an item has been changed.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_BITMAP
// :param2.
// :item name='ItemID' type='USHORT'.
// ID of the changed item.
// :returns.
// Nothing
// :eparams.
// :remarks.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMCHGTEXT. name='UCN_TEXT' .
// This WM_CONTROL notification is posted by the UCMenu to its owner when 
// the text of an item has been changed.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_TEXT
// :param2.
// :item name='ItemID' type='USHORT'.
// ID of the changed item.
// :returns.
// Nothing
// :eparams.
// :remarks.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMCHGACTION. name='UCN_ACTION' .
// This WM_CONTROL notification is posted by the UCMenu to its owner when 
// the action of an item has been changed.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_ACTION
// :param2.
// :item name='ItemID' type='USHORT'.
// ID of the changed item.
// :returns.
// Nothing
// :eparams.
// :remarks.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_QRYSIZE. name='UCMENU_QUERYSIZE' .
// This message is sent to the ucmenu to query the size it needs to be be fully displayed.
// :params.
// :param1.
// :item name='pCx' type='PULONG'.
// Pointer to a ULONG in which the width will be put.
// :param2.
// :item name='pCy' type='PULONG'.
// Pointer to a ULONG in which the height will be put.
// :returns.
// :item name='Success' type='BOOL'.
// :dl compact.
// :dt.:hp2.TRUE:ehp2.
// :dd.Query successful
// :dt.:hp2.FALSE:ehp2.
// :dd.Query Unsuccessful
// :edl.
// :eparams.
// :remarks.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_QRYCLR. name='UCMENU_QUERYCOLOR' .
// This message is sent to the ucmenu to query its color.
// :params.
// :param1.
// :item name='pRGBColor' type='LONG *'.
// Pointer to a LONG in which the RGB value of the UCMenu background color will be put.
// :param2.
// :item name='pItemRGBColor' type='LONG *'.
// Pointer to a LONG in which the RGB value of the UCMenu items background color will be put.
// :returns.
// :item name='Success' type='BOOL'.
// :dl compact.
// :dt.:hp2.TRUE:ehp2.
// :dd.Query successful
// :dt.:hp2.FALSE:ehp2.
// :dd.Query Unsuccessful
// :edl.
// :eparams.
// :remarks.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_QRYFONT. name='UCMENU_QUERYFONT' .
// This message is sent to the ucmenu to query its font.
// :params.
// :param1.
// :item name='pszFontNameSize' type='PSZ'.
// String in which the font  name and size will be put, allocated by the sender.
// :p.The string returned has the format used in WinSetPresParam.
// :param2.
// :item name='Size' type='ULONG'.
// Size of the string allocated by the sender
// :returns.
// :item name='Success' type='BOOL'.
// :dl compact.
// :dt.:hp2.TRUE:ehp2.
// :dd.Query successful
// :dt.:hp2.FALSE:ehp2.
// :dd.Query Unsuccessful
// :edl.
// :eparams.
// :remarks.
// :defproc.
// :related.
// WinSetPresParam, WinQueryPresParam.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_QRYSTYL. name='UCMENU_QUERYSTYLE' .
// This message is sent to the ucmenu to query its style.
// :params.
// :param1.
// :item name='pStyle' type='PULONG'.
// Pointer to a ULONG in which the style will be put.
// :p.The style is a combination of the UCS_* values.
// :returns.
// :item name='Success' type='BOOL'.
// :dl compact.
// :dt.:hp2.TRUE:ehp2.
// :dd.Query successful
// :dt.:hp2.FALSE:ehp2.
// :dd.Query Unsuccessful
// :edl.
// :param2.
// :item name='param2' type='ULONG'.
// reserved
// :eparams.
// :remarks.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_QRYFRCSZ. name='UCMENU_QUERYFORCEDSIZE' .
// This message is sent to the ucmenu to query the value of its forced size
// :params.
// :param1.
// :item name='pCx' type='PULONG'.
// Pointer to a ULONG in which the width will be put.
// :param2.
// :item name='pCy' type='PULONG'.
// Pointer to a ULONG in which the height will be put.
// :returns.
// :item name='Success' type='BOOL'.
// :dl compact.
// :dt.:hp2.TRUE:ehp2.
// :dd.Query successful
// :dt.:hp2.FALSE:ehp2.
// :dd.Query Unsuccessful
// :edl.
// :eparams.
// :remarks.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_ADDTOCM. name='UCMENU_ADDITEMSTOCM' .
// This user message can be sent to the UCMenu to add items to its context menu
// :params.
// :param1.
// :item name='NbOfItems' type='ULONG'.
// Number of items to add
// :param2.
// :item name='pCMItems' type='PCMITEMS'.
// Pointer to a CMITEMS structure. It contains an ID and pszText for each item
// added
// :returns.
// nothing
// :item name='' type=''.
// :eparams.
// :remarks.
// This message should be sent, since the caller has to free pszItemText in
// the CMITEMS structure.
// :defproc.
// No default processing
// :related.
// See CMITEMS type.
// See UCMENU_DELETECMITEM
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_DELCMITEM. name='UCMENU_DELETECMITEM' .
// This user message can be sent to the UCMenu to delete items from its context menu
// :params.
// :param1.
// :item name='NbOfItems' type='ULONG'.
// Number of items to delete.
// :param2.
// :item name='pItemId' type='PULONG'.
// Array of items ids.
// :returns.
// nothing
// :eparams.
// :remarks.
// By default, the context menu has the following items &colon.
// :dl compact.
// :dt.:hp2.Edit&period.&period.&period.:ehp2.
// :dd.IDM_UCM_EDIT
// :dt.:hp2.Create&period.&period.&period.:ehp2.
// :dd.IDM_UCM_CREATE
// :dt.:hp2.Delete:ehp2.
// :dd.IDM_UCM_DELETE
// :dt.:hp2.Separator line:ehp2.
// :dd.IDM_UCM_SEPARATOR
// :dt.:hp2.Style&period.&period.&period.:ehp2.
// :dd.IDM_UCM_STYLE
// :dt.:hp2.Load Default:ehp2.
// :dd.IDM_UCM_DEFAULT
// :dt.:hp2.Import&period.&period.&period.:ehp2.
// :dd.IDM_UCM_LOAD
// :dt.:hp2.Export&period.&period.&period.:ehp2.
// :dd.IDM_UCM_SAVEAS
// :edl.
// :defproc.
// No default processing
// :related.
// See CMITEMS type
// See UCMENU_ADDITEMSTOCM
// See UCN_CMITEM
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_CMSELECT. name='UCN_CMITEM' .
// This WM_CONTROL notification is posted to the owner of the UCMenu when the context menu receives
// a WM_COMMAND message for one of the items added with UCMENU_ADDITEMSTOCM.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_CMITEM
// :param2.
// :item name='CMItemID' type='USHORT'.
// ID of the context menu item selected.
// :item name='ItemID' type='USHORT'.
// ID of item over which the context menu was popped up.
// :eparams.
// :remarks.
// The owner should process this message by executing the function associated
// to the item he added.
// :defproc.
// No default processing
// :related.
// See CMITEMS type.
// See UCMENU_ADDITEMSTOCM
// See UCMENU_DELETECMITEM
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_QRYVERSION. name='UCMENU_QUERYVERSION' .
// This user message is sent to the UCMenu to query the version of the UCMenus dll
// :params.
// :param1.
// :item name='param1' type='ULONG'.
// Reserved
// :param2.
// :item name='param2' type='ULONG'.
// Reserved
// :returns.
// :item name='version' type='ULONG'.
// Version number of the dll, eg 2 for 0.02 and 100 for 1.00.
// :eparams.
// :remarks.
// :defproc.
// No default processing
// :related.
// :emessage.
//-----------------------------------------------------------------------------
// :message res=&IDM_HLPNBBMP. name='UCN_HLP_NB_BMP' .
// This WM_CONTROL notification is sent by the ucmenu to its owner when custom help is required
// for the general page of the notebook.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_HLP_NB_BMP
// :param2.
// :item name='param2' type='ULONG'.
// Reserved
// :returns.
// Nothing.
// :eparams.
// :remarks.
// :defproc.
// No default processing
// :related.
// UCS_CUSTOMHLP
// :emessage.
//-----------------------------------------------------------------------------
// :message res=&IDM_HLPNBCREATE. name='UCN_HLP_NB_CREATE' .
// This WM_CONTROL notification is sent by the ucmenu to its owner when custom help is required
// for the create page of the notebook.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_HLP_NB_CREATE
// :param2.
// :item name='param2' type='ULONG'.
// Reserved
// :returns.
// Nothing.
// :eparams.
// :remarks.
// :defproc.
// No default processing
// :related.
// UCS_CUSTOMHLP
// :emessage.
//-----------------------------------------------------------------------------
// :message res=&IDM_HLPNBACTION. name='UCN_HLP_NB_ACTION' .
// This WM_CONTROL notification is sent by the ucmenu to its owner when custom help is required
// for the action page of the notebook.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_HLP_NB_ACTION
// :param2.
// :item name='param2' type='ULONG'.
// Reserved
// :returns.
// Nothing.
// :eparams.
// :remarks.
// :defproc.
// No default processing
// :related.
// UCS_CUSTOMHLP
// :emessage.
//-----------------------------------------------------------------------------
// :message res=&IDM_HLPSTYLE. name='UCN_HLP_STYLE' .
// This WM_CONTROL notification is sent by the ucmenu to its owner when custom help is required
// for the style dialog.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_HLP_NB_STYLE
// :param2.
// :item name='param2' type='ULONG'.
// Reserved
// :returns.
// Nothing.
// :eparams.
// :remarks.
// :defproc.
// No default processing
// :related.
// UCS_CUSTOMHLP
// :emessage.
//-----------------------------------------------------------------------------
// :message res=&IDM_HLPDM. name='UCN_HLP_DM' .
// This WM_CONTROL notification is sent by the ucmenu to its owner when custom help is required
// for the drag and drop.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_HLP_NB_DM
// :param2.
// :item name='param2' type='ULONG'.
// Reserved
// :returns.
// Nothing.
// :eparams.
// :remarks.
// :defproc.
// No default processing
// :related.
// UCS_CUSTOMHLP
// :emessage.
//-----------------------------------------------------------------------------
// :message res=&IDM_HLPBUFFET. name='UCN_HLP_BUFFET' .
// This WM_CONTROL notification is sent by the ucmenu to its owner when custom help is required
// for the buffet toolbar dialog.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_HLP_NB_BUFFET
// :param2.
// :item name='param2' type='ULONG'.
// Reserved
// :returns.
// Nothing.
// :eparams.
// :remarks.
// :defproc.
// No default processing
// :related.
// UCS_CUSTOMHLP
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMSETSTYLE. name='UCMENU_SETSTYLE' .
// This message is sent to the UCMenu to set the style flags and the associated width
// and height values.
// :params.
// :param1.
// :item name='StyleFlags' type='ULONG'.
// Combination of UCS_* flags.
// :param2.
// :item name='cx' type='USHORT'.
// The width of the items if UCS_FORCESIZE is set.
// :item name='cy' type='USHORT'.
// The height of the items if UCS_FORCESIZE is set.
// :returns.
// :item name='Success' type='BOOL'.
// TRUE if successful, FALSE else.
// :eparams.
// :remarks.
// A UCMENU_UPDATE message must be issued in order to reflect the changes.
// :defproc.
// :related.
// UCMENU_UPDATE
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_MOUSEMOVE. name='UCN_MOUSEMOVE' .
// This WM_CONTROL notification code is posted by the ucmenu to its owner when the mouse is moved over
// its menu or submenu.
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_MOUSEMOVE
// :param2.
// :item name='ItemID' type='USHORT'.
// ID of the item over which the pointer is
// :returns.
// Nothing.
// :eparams.
// :remarks.
// :defproc.
// No default processing
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_QRYUCMINFO. name='UCMENU_QUERYUCMINFO' .
// This message is sent to the ucmenu to retrieve the UCMINFO structure
// :params.
// :param1.
// :item name='pUCMInfo' type='PUCMINFO'.
// Pointer to the UCMINFO structure to fill.
// :param2.
// :item name='param2' type='ULONG'.
// reserved.
// :returns.
// :item name='Success' type='BOOL'.
// TRUE if successful, FALSE else.
// :eparams.
// :remarks.
// If the PSZ fields are not null, they have been allocated by
// the ucmenu and have to be freed with UCMenuFree by the sender.
// :defproc.
// No default processing
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMUPDATE. name='UCMENU_UPDATE' .
// This message is sent to the ucmenu to display the changes made.
// :params.
// :param1.
// :item name='param1' type='ULONG'.
// reserved.
// :param2.
// :item name='param2' type='ULONG'.
// reserved.
// :returns.
// :item name='Success' type='BOOL'.
// TRUE if successful, FALSE else.
// :eparams.
// :remarks.
// :defproc.
// No default processing
// :related.
// UCMENU_SETSTYLE UCMENU_SETCOLOR
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_SETBGCOLOR. name='UCMENU_SETBGCOLOR' .
// This message is sent to the ucmenu to set the ucmenu background color
// :params.
// :param1.
// :item name='BgColor' type='ULONG'.
// The RGB color which will be used for the ucmenu background.
// :param2.
// :item name='ItemBgColor' type='ULONG'.
// The RGB color which will be used for the items background.
// :returns.
// :item name='Success' type='BOOL'.
// TRUE if successful, FALSE else.
// :eparams.
// :remarks.
// :defproc.
// No default processing
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_SETFONT. name='UCMENU_SETFONT' .
// This message is sent to the ucmenu to set the ucmenu font.
// :params.
// :param1.
// :item name='FontNameSize' type='PSZ'.
// Font name, such as used in WinSetPresParam.
// :param2.
// :item name='-' type='ULONG'.
// Reserved.
// :returns.
// :item name='Success' type='BOOL'.
// TRUE if successful, FALSE else.
// :eparams.
// :remarks.
// :defproc.
// No default processing
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :message res=&IDM_UCMQRYRESBMP. name='UCN_QRYRESBMP' .
// This WM_CONTROL notification message is sent by the ucmenu to its owner when the list of bmp id is needed.
// ("Predefined..." on the "general" notebook page )
// :params.
// :param1.
// :item name='UCMenuID' type='USHORT'.
// ID of the ucmenu
// :item name='NotifCode' type='USHORT'.
// UCN_QRYRESBMP
// :param2.
// :item name='pArrayId' type='PULONG *'.
// The owner has to return in this pointer an array of the bitmap IDs, allocated with UCMenuAlloc.
// :returns.
// :item name='nbItems' type='ULONG'.
// The number of elements in the returned array or zero.
// :eparams.
// :remarks.
// The array will be freed by the ucmenu.
// :defproc.
// :related.
// :emessage.
//----------------------------------------------------------------------------
// :euserdoc.
//----------------------------------------------------------------------------

//--- Flags for an ucmenu item
#define  UCMI_BMPFROMFILE 0x0001  


//--- Context menu items ids
#define IDM_UCM_EDIT                 0x00E0
#define IDM_UCM_CREATE               0x00E1
#define IDM_UCM_DELETE               0x00E2
#define IDM_UCM_SEPARATOR            0x00E3
#define IDM_UCM_STYLE                0x00E4
#define IDM_UCM_DEFAULT              0x00E5
#define IDM_UCM_LOAD                 0x00E6
#define IDM_UCM_SAVEAS               0x00E7

//--- Styles to use in WinCreateWindow when creating a UCMenu
#define CMS_HORZ          0x0001
#define CMS_VERT          0x0002
#define CMS_MATRIX        0x0004

//--- Actions already defined in ucmenu.dll
#define UCM_ACTION_EXEC "ExecuteProgram"   // -- If the action of the item begins with this
                                           // -- string, the rest of it is interpreted as a
                                           // -- program name to be started in another thread.

//--- Type defined for the drag and drop of ucmenu items
#define DRT_UCMITEM  "UCMenu item"

//--- New menu item style defined by the ucmenus
#define MIS_SPACER        0x4001           // -- includes MIS_TEXT  

//--- Class name of the ucmenus
#define UCMENUCLASS "UCMenu"

//--- New CMDSRC_* value defined
#define CMDSRC_UCMENU 55234

//--- User messages

//--- Notification codes associated to the WM_CONTROL messages
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

//--- User messages

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

//-----------------------------------------------------------------------------
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
PVOID   APIENTRY UCMenuAlloc(size_t size);
VOID    APIENTRY UCMenuFree(VOID * p);
VOID    APIENTRY UCMenuFreeMenuData(HWND hwndMenu);
BOOL    APIENTRY UCMenuNew( HWND hwndUCMenu, PVOID pTemplate );
BOOL    APIENTRY UCMenuLoadDefault( HWND hwndUCMenu );
SHORT   APIENTRY UCMenuGetActionID(HWND hwndUCM, PSZ pszAction, BOOL bSubmenus, SHORT sStartID);
VOID    APIENTRY UCMenuSetActionAttr(HWND hwndUCM, PSZ pszAction, USHORT usAttrMask, USHORT usAttrValue );
BOOL    APIENTRY UCMenuSaveStyle(HWND hwndUCMenu, PSZ pszFileName);
BOOL    APIENTRY UCMenuLoadStyle(PSZ pszFileName, PUCMINFO pUCMInfo);
ULONG   APIENTRY UCMenuResourceBuffetDlg( HWND hParent, HWND hUCMenu, ULONG ulBuffetID, HMODULE hmodBuffet );
ULONG   APIENTRY UCMenuTemplateBuffetDlg( HWND hParent, HWND hUCMenu, PVOID pTemplate );

#endif // UCMENUS_h
//--- end of file ------------------------------------------------------------
