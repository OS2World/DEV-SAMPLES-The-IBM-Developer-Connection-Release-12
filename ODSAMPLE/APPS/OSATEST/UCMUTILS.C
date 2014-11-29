/************************************************************************/
/* Program name: UCMUTILS.C                                             */
/*                                                                      */
/* OS/2 Developer Magazine, Issue:  Jan '95                             */
/* Author:  Mark McMillan, IBM Corp.                                    */
/*                                                                      */
/* Description: UCMenu utility routines                                 */
/*                                                                      */
/* Program Requirements:  OS/2 2.x                                      */
/*                        IBM C Set/2 (or other OS/2 compiler)          */
/*                        OS/2 Toolkit                                  */
/*                                                                      */
/* Copyright (c)International Business Machines Corp. 1994              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/************************************************************************/
/*                      DISCLAIMER OF WARRANTIES.                       */
/************************************************************************/
/************************************************************************/
/*     The following [enclosed] code is source code created by the      */
/*     author(s).  This source code is provided to you solely           */
/*     for the purpose of assisting you in the development of your      */
/*     applications.  The code is provided "AS IS", without             */
/*     warranty of any kind.  The author(s) shall not be liable         */
/*     for any damages arising out of your use of the source code,      */
/*     even if they have been advised of the possibility of such        */
/*     damages.  It is provided purely for instructional and            */
/*     illustrative purposes.                                           */
/************************************************************************/
/************************************************************************/

/*************************************************************************
Notes for UCMenu utility routines:

  This is a collection of useful routines for applications using
  UCMenu toolbar controls.  In particular, these routines can manage
  the toolbar as a frame control (as opposed to it being part of the
  client window).  All the frame control logic is encapsulated here
  so the application does not have to worry about placement or sizing
  of the toolbar.  The frame control routines also support management
  of toolbar controls on dialog windows.

  See the descriptive comments on each routine.

  The utilities can manage multiple UCMenu controls in a single frame
  (or dialog) window.  They can also manage multiple frames/dialogs
  in an application, each with multiple toolbars.

  Limitations:
      - Linked lists are not semaphore protected.  Therefore cannot use
        these routines from multiple threads in the same process.  This is
        not usually a problem as most applications have only a single
        thread doing user interface (PM) work.  It would be fairly easy
        to add semaphore protection where the linked lists are accessed.

**************************************************************************/

#define  INCL_BASE
#define  INCL_WIN
#define  INCL_DOS
#define  INCL_GPI

#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "UCMenus.h"   /* Basic UCMenus definitions */
#include "UCMUtils.h"  /* My external prototypes and definitions */

/* Data structures used internally here */

typedef struct MENUINST_S {         /* Describe one UCMenu in a subclassed frame */
  struct MENUINST_S  *Next, *Prev;  /* Linked list pointers */
  HWND               MenuHwnd;      /* UC menu window handle */
  PFNWP              MenuProc;      /* Original menu window proc */
  USHORT             Position;      /* Position in frame UCMUTILS_PLACE_xxxx */
  } MENUINST;
#define MENUINST_SIZE sizeof(MENUINST)

typedef struct FRAMEINST_S {        /* Describe one subclassed frame */
  struct FRAMEINST_S *Next, *Prev;  /* Linked list pointers */
  HWND               FrameHwnd;     /* Frame window handle */
  HWND               ClientHwnd;    /* Client window handle */
  PFNWP              FrameProc;     /* Original frame proc address */
  MENUINST           MenuList;      /* Headnode of list of menus in this frame */
  USHORT             MenuCount;     /* Number of menus in this frame */
  BOOL               IsDialog;      /* Is this frame a dialog window? */
  SWP                DlgSize;       /* Minimum size of dialog */
  } FRAMEINST;
#define FRAMEINST_SIZE sizeof(FRAMEINST)

typedef struct GENERICLIST_S {      /* Generic linked-list structure */
  struct GENERICLIST_S *Next, *Prev;/* All must have links as first elements */
  } GENERICLIST;

/* Internal prototypes.  All procedures and global data are named starting */
/* with "UCMU..." to prevent conflicts with application procs and data.    */

MRESULT EXPENTRY UCMU_FrameSubclassProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY UCMU_MenuSubclassProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
PVOID            UCMU_ListInsert(PVOID Element, ULONG Size);
void             UCMU_ListDelete(PVOID Element);

/* Global internally used data */

FRAMEINST UCMUFrameList =                /* First (dummy) node in list of subclassed frames */
  {NULL, NULL, NULLHANDLE, NULLHANDLE, NULL, {NULL, NULL, NULLHANDLE, NULL, 0}, 0};

/******************************************************************************/
void UCMUtilsAddToFrame(HWND TargetHwnd,  /* Handle of frame/dialog window    */
                        HWND ToolHwnd,    /* Handle of toolbar menu           */
                        ULONG Place)      /* UCMUTILS_PLACE_xxx position flag */
/******************************************************************************/
/* This procedure will add the specified toolbar window to the specified      */
/* frame or dialog window.  It will be placed at the top/bottom/right/left    */
/* as determined by the "Place" parameter.  It is assumed that only toolbars  */
/* with a style of CMS_HORZ are place at the top or bottom, and that only     */
/* CMS_VERT menus are place at the left or right.  If CMX_MATRIX is used, the */
/* dimensions should be appropriate for the placement.                        */
/* SWP_SHOW will be set on to make the window visible.                        */
/*                                                                            */
/* This routine will build a linked list of all frame windows that we have    */
/* been asked to add UC menus to.  The FRAMEINST data is used to keep info    */
/* we need to maintain about each frame.  (We cannot use the frame's window   */
/* words because the application may be using them for its own purposes).     */
/* Each FRAMEINST also contains in it a linked list of all the UC menus that  */
/* that particular frame contains.  The MENUINST data keeps info that we      */
/* need for each UC menu in the frame (and again, it is not safe for us to    */
/* use the menu's window words).                                              */
/*                                                                            */
/* The linked list elements are removed when the UCMUtilsRemoveFromFrame()    */
/* routine is called, and when the WM_DESTROY message is received by the      */
/* frame (subclass) procedure.                                                */
/*                                                                            */
/* Both the frame and menu window will be subclassed.  The subclassing is     */
/* removed when UCMUtilsRemoveFromFrame() is called, or the frame window is   */
/* destroyed.                                                                 */
/******************************************************************************/
{
FRAMEINST *FrameInfo;   /* Pointer into linked list of subclassed frames */
MENUINST  *MenuInfo;    /* Pointer into linked list of frame's UC menus  */

  /* See if this frame window is already under our control */

  for (FrameInfo=UCMUFrameList.Next; FrameInfo!=NULL; FrameInfo=FrameInfo->Next) {
    if (FrameInfo->FrameHwnd == TargetHwnd) {  /* Here it is */
      (FrameInfo->MenuCount)++;               /* Add this menu to it */
      MenuInfo = &(FrameInfo->MenuList);      /* Pt to first node in menu list */
      /* Put LEFT/RIGHT menus at end of list */
      if ((Place == UCMUTILS_PLACE_LEFT) || (Place == UCMUTILS_PLACE_RIGHT))
        for (;MenuInfo->Next != NULL; MenuInfo=MenuInfo->Next);

      MenuInfo = UCMU_ListInsert(MenuInfo, MENUINST_SIZE); /* Create new menu node */
      MenuInfo->MenuHwnd = ToolHwnd;                       /* Fill it in */
      MenuInfo->Position = Place;
      /* Subclass the menu */
      MenuInfo->MenuProc = WinSubclassWindow(ToolHwnd, (PFNWP)UCMU_MenuSubclassProc);
      break;
    }
  }

  /* If we have not seen this frame before, create a new entry for it in the */
  /* list of frames and subclass it.                                         */

  if (FrameInfo == NULL) {
    FrameInfo = UCMU_ListInsert(&UCMUFrameList, FRAMEINST_SIZE);
    FrameInfo->FrameHwnd = TargetHwnd;
    FrameInfo->ClientHwnd= WinWindowFromID(TargetHwnd, FID_CLIENT);
    /* Subclass the frame */
    FrameInfo->FrameProc = WinSubclassWindow(TargetHwnd, (PFNWP)UCMU_FrameSubclassProc);
    if (FrameInfo->ClientHwnd == NULLHANDLE) {
      FrameInfo->IsDialog = TRUE;            /* This frame is a dialog */
      UCMUtilsSetMinDlgSize(TargetHwnd);     /* Initial min is current dialog size */
    }
      else FrameInfo->IsDialog = FALSE;
    /* Add this menu to the frame data */
    FrameInfo->MenuCount = 1;
    MenuInfo = UCMU_ListInsert(&(FrameInfo->MenuList), MENUINST_SIZE); /* Create new node */
    MenuInfo->MenuHwnd = ToolHwnd;                                     /* Fill it in */
    MenuInfo->Position = Place;
    /* Subclass the menu */
    MenuInfo->MenuProc = WinSubclassWindow(ToolHwnd, (PFNWP)UCMU_MenuSubclassProc);
  }

  /* If this is a dialog, adjust the frame size to make space for new menu. */
  if (FrameInfo->IsDialog) {
    SWP SwpTool;
    WinQueryWindowPos(ToolHwnd, &SwpTool);
    UCMUtilsAdjustDlgSize(TargetHwnd,
                          SwpTool.cx, SwpTool.cy,
                          Place);
  }

  /* Now that the frame is subclassed and we have stored information about the */
  /* frame and the menu we're   making into a frame control, we tell the frame */
  /* to reformat so the new control is sized and positioned into the frame.    */
  /* (It does not matter what control we tell it was added -- it will format   */
  /* the controls that actually exist).                                        */

  WinSetWindowPos(ToolHwnd, HWND_TOP, 0,0,0,0, SWP_SHOW);
  WinSendMsg(TargetHwnd, WM_UPDATEFRAME, MPFROMSHORT(FCF_VERTSCROLL), 0L);

} /* procedure UCMUtilsAddToFrame() */

/******************************************************************************/
void UCMUtilsRemoveFromFrame(HWND  TargetHwnd, /* Frame handle                */
                             HWND  ToolHwnd)   /* UC menu handle              */
/******************************************************************************/
/* This routine will remove the specified UC menu from the frame (e.g. the    */
/* menu will no longer be sized/moved as a frame control).  The UC menu       */
/* will be hidden (but not destroyed).  The frame will be reformatted.        */
/******************************************************************************/
{
FRAMEINST *FrameInfo;
MENUINST  *MenuInfo;
ULONG     TempPlace;

  /* Locate the frame in our list of subclassed frames */

  for (FrameInfo=UCMUFrameList.Next; FrameInfo!=NULL; FrameInfo=FrameInfo->Next) {
    if (FrameInfo->FrameHwnd == TargetHwnd)
      break;
  }

  if (FrameInfo == NULL)  /* Not there... we don't know about this frame */
    return;

  /* Locate the UC menu from the list of UC menus in this frame */

  for (MenuInfo=(FrameInfo->MenuList).Next; MenuInfo!=NULL; MenuInfo=MenuInfo->Next) {
    if (MenuInfo->MenuHwnd == ToolHwnd)
      break;
  }

  if (MenuInfo == NULL)  /* Not there... we don't know about this menu */
    return;

  TempPlace= MenuInfo->Position;  /* Save some dialog info we need later */

  /* Un-subclass the menu, remove it from list, update count of menus */
  WinSubclassWindow(ToolHwnd, MenuInfo->MenuProc);
  UCMU_ListDelete(MenuInfo);
  (FrameInfo->MenuCount)--;

  /* If this is a dialog, adjust the frame size.   This must be done AFTER */
  /* the menu has been removed from the list, or minimum dialog size will  */
  /* not be calculated correctly by the frame subclass procedure.          */

  if (FrameInfo->IsDialog) {  /* Use temp values since MenuInfo is gone now */
    SWP SwpTool;
    WinQueryWindowPos(ToolHwnd, &SwpTool);
    UCMUtilsAdjustDlgSize(FrameInfo->FrameHwnd,
                          0-SwpTool.cx, 0-SwpTool.cy,
                          TempPlace);
  }

  /* If there are no more UC menus in this frame, un-subclass and remove frame from list */

  if (FrameInfo->MenuCount < 1) {
    WinSubclassWindow(TargetHwnd, FrameInfo->FrameProc);
    UCMU_ListDelete(FrameInfo);
  }

  /* Hide the menu window and reformat the frame */
  WinSetWindowPos(ToolHwnd, HWND_TOP, 0,0,0,0, SWP_HIDE);
  WinSendMsg(TargetHwnd, WM_UPDATEFRAME, MPFROMSHORT(FCF_VERTSCROLL), 0L);

} /* procedure UCMUtilsRemoveFromFrame() */


/******************************************************************************/
void UCMUtilsSetMinDlgSize(HWND DlgHwnd)
/******************************************************************************/
/* This function can be used to set the minimum size of a dialog to           */
/* the current size of the dialog.  This would only need to be called by an   */
/* application program if the application altered the size of the dialog      */
/* window with WinSetWindowPos().  Dialogs that dynamically add/remove        */
/* controls and adjust the dialog size would need to call this function after */
/* altering the dialog size.                                                  */
/*                                                                            */
/* The calculated minimum frame size is stored in the per-frame instance data */
/* where the FrameSubclassProc() will use it during WM_QUERYTRACKINFO message */
/* processing to limit the window size.                                       */
/******************************************************************************/
{
FRAMEINST *FrameInfo;   /* Ptr into list of subclassed frames */
MENUINST  *MenuInfo;    /* Ptr into list of UC menus on the frame */
SWP       MenuSwp;      /* Size of UC menu window */

  /* Find this dialog in our list of subclassed frames. */

  for (FrameInfo=UCMUFrameList.Next; FrameInfo!=NULL; FrameInfo=FrameInfo->Next) {
    if (FrameInfo->FrameHwnd == DlgHwnd)
      break;
  }
  if (FrameInfo==NULL)   /* If not there, just quit */
    return;

  WinQueryWindowPos(DlgHwnd, &(FrameInfo->DlgSize)); /* Current size, inclding any UC menus*/

  /* Subtract the size of all existing UC menus from current frame size */

  for (MenuInfo=(FrameInfo->MenuList).Next; MenuInfo!=NULL; MenuInfo=MenuInfo->Next) {
    WinQueryWindowPos(MenuInfo->MenuHwnd, &MenuSwp);
    switch (MenuInfo->Position) {
      case UCMUTILS_PLACE_TOP:      /* Adjust y dimension of rectangle */
      case UCMUTILS_PLACE_BOTTOM:
        FrameInfo->DlgSize.cy = FrameInfo->DlgSize.cy - MenuSwp.cy;
        break;
      case UCMUTILS_PLACE_LEFT:     /* Adjust x dimension of rectangle */
      case UCMUTILS_PLACE_RIGHT:
        FrameInfo->DlgSize.cx = FrameInfo->DlgSize.cx - MenuSwp.cx;
        break;
    } /* switch */
  } /* for each UC menu in the frame */

} /* end procedure UCMUtilsSetMinDlgSize() */

/******************************************************************************/
void UCMUtilsAdjustDlgSize(HWND DlgHwnd, LONG Cx, LONG Cy, USHORT Where)
/******************************************************************************/
/* Adjust dialog window by Cx or Cy amount (depending on where the adjustment */
/* is made: UCMUTILS_PLACE_TOP/LEFT/RIGHT/BOTTOM).  If necessary, all dialog  */
/* controls are moved to keep in same relative position in the dialog.  An    */
/* application can use this as a general dialog-sizing utility.               */
/******************************************************************************/
{
SWP     DlgSwp;         /* Size of dialog frame */
HENUM   Enum;           /* Handle to enumerate dialog children */
HWND    ChildHwnd;      /* Child of dialgo */
SWP     ChildSwp;       /* Size of child */
char    ChildClass[10]; /* Class name of child */
LONG    ChildID;        /* Window ID of child */

  WinQueryWindowPos(DlgHwnd, &DlgSwp);
  switch (Where) {
    case UCMUTILS_PLACE_TOP:
      /* This is easy... just resize the window, dlg controls stay put */
      WinSetWindowPos(DlgHwnd, HWND_TOP, 0,0, DlgSwp.cx, DlgSwp.cy + Cy, SWP_SIZE);
      break;
    case UCMUTILS_PLACE_RIGHT:
      /* This is easy... just resize the window, dlg controls stay put */
      WinSetWindowPos(DlgHwnd, HWND_TOP, 0,0, DlgSwp.cx + Cx, DlgSwp.cy, SWP_SIZE);
      break;
    case UCMUTILS_PLACE_LEFT:
    case UCMUTILS_PLACE_BOTTOM:
      /* For these positions, we must resize the dialog and move all the */
      /* dialog controls by the Cx or Cy amount.                         */
      if (Where == UCMUTILS_PLACE_LEFT) {
        Cy = 0L;
      }
      else {
        Cx = 0L;
      }
      /* Locate all dialog control windows.  The criteria for a child window */
      /* being a dialog control, is:                                         */
      /*   - the window is not a standard frame ctrl (FID_TITLEBAR, etc).    */
      /*   - the window is not of class "UCMenus"                            */

      Enum = WinBeginEnumWindows(DlgHwnd);
      for (ChildHwnd = WinGetNextWindow(Enum); ChildHwnd!=NULLHANDLE; ChildHwnd=WinGetNextWindow(Enum)) {
        ChildID = WinQueryWindowUShort(ChildHwnd, QWS_ID);
        switch (ChildID) {
          case FID_SYSMENU:
          case FID_MENU:
          case FID_TITLEBAR:
          case FID_VERTSCROLL:
          case FID_HORZSCROLL:
          case FID_MINMAX:
            break;  /* Don't adjust these -- the frame will format them */
          default:
            WinQueryClassName(ChildHwnd, sizeof(ChildClass), ChildClass);
            if (strcmp(ChildClass, "UCMenu")) {  /* Not a UCMenu either, so adjust it */
              WinQueryWindowPos(ChildHwnd, &ChildSwp);
              ChildSwp.x = ChildSwp.x + Cx;
              ChildSwp.y = ChildSwp.y + Cy;
              WinSetWindowPos(ChildHwnd, HWND_TOP, ChildSwp.x, ChildSwp.y, 0,0, SWP_MOVE);
              /* Entry field control is a major pain... it changes it's position   */
              /* when moved.  No way to precalculate how much.  Must move it back. */
              if (!strcmp(ChildClass, "#6")) {  /* WC_ENTRYFIELD */
                SWP NewPos;
                WinQueryWindowPos(ChildHwnd, &NewPos);
                ChildSwp.x = ChildSwp.x + (ChildSwp.x - NewPos.x);
                ChildSwp.y = ChildSwp.y + (ChildSwp.y - NewPos.y);
                WinSetWindowPos(ChildHwnd, HWND_TOP, ChildSwp.x, ChildSwp.y, 0,0, SWP_MOVE);
              }
              /* Controls must repaint or moves will overlay each other */
              WinInvalidateRect(ChildHwnd, NULL, TRUE);
            }
            break;
        } /* switch on ID */
      } /* for all children of dialog window */
      WinEndEnumWindows(Enum);

      if (Where == UCMUTILS_PLACE_LEFT) {
        WinSetWindowPos(DlgHwnd, HWND_TOP, 0,0, DlgSwp.cx + Cx, DlgSwp.cy, SWP_SIZE);
      }
      else {
        WinSetWindowPos(DlgHwnd, HWND_TOP, 0,0, DlgSwp.cx, DlgSwp.cy + Cy, SWP_SIZE);
      }
      break;

  } /* switch on Where */

} /* end procedure UCMUtilsAdjustDlgSize() */

/******************************************************************************/
MRESULT UCMU_FormatFrameMsg(HWND hwnd, MPARAM mp1, MPARAM mp2, FRAMEINST *FrameInfo)
/******************************************************************************/
/* This procedure processes WM_FORMATFRAME messages in the frame subclass     */
/* procedure.  MP1 is a pointer to an array of SWP structures, one for each   */
/* of the frame controls.  Upon entry, none of the structures have been       */
/* filled in yet with real control size/positions (that is the purpose of     */
/* this message).  MP2 is a pointer to a rectangle structure which is the     */
/* size of the client area of the frame window.  We must return the count     */
/* of the number of SWP structures filled in.  (The normal frame window       */
/* proc will fill in most of them, we fill in only those for the UC menus).   */
/******************************************************************************/
{
SHORT CtrlCount;
SHORT IndexTitlebar=-1, IndexMenu=-1, IndexVScroll=-1, IndexHScroll=-1, IndexClient=-1,
      IndexUCTop=-1, IndexUCBot=-1;
SHORT IndexUCMenu;
int    i;
SWP    *SwpArray, SwpFrame;
POINTL BorderSize;
MENUINST *MenuInfo;
LONG   Posn;

  /* Pass this message down to all other frame window procs to fill in */
  /* the SWP structures for all the FID_XXXX controls (e.g. title bar, */
  /* scroll bars, system menu, etc).  Will get back number of SWP      */
  /* structures which have been filled in.                             */

  CtrlCount = SHORT1FROMMR((*(FrameInfo->FrameProc))(hwnd, WM_FORMATFRAME, mp1, mp2));
  SwpArray  = PVOIDFROMMP(mp1);

  /* Go through array of SWP structures and see which entries are for  */
  /* which frame controls that we are interested in.                   */

  for (i=0; i<CtrlCount; i++)
    switch (WinQueryWindowUShort(SwpArray[i].hwnd, QWS_ID)) {
      case FID_CLIENT:      IndexClient  = i; break;
      case FID_MENU:        IndexMenu    = i; break;
      case FID_TITLEBAR:    IndexTitlebar= i; break;
      case FID_VERTSCROLL:  IndexVScroll = i; break;
      case FID_HORZSCROLL:  IndexHScroll = i; break;
    }

  /* Get size of frame border (cannot use WinQuerySysValue() because */
  /* border size can be set with WM_SETBORDERSIZE) and frame itself. */
  WinSendMsg(hwnd, WM_QUERYBORDERSIZE, MPFROMP(&BorderSize), 0L);
  WinQueryWindowPos(hwnd, &SwpFrame);

  /* For each UC menu in this frame, adjust the size/position of other */
  /* frame controls as necessary and then set the size/pos of the menu.*/
  /* Note that menus are in the linked list such that TOP/BOTTOM menus */
  /* are processed before LEFT/RIGHT menus.                            */

  for (MenuInfo=(FrameInfo->MenuList).Next; MenuInfo!=NULL; MenuInfo=MenuInfo->Next) {

    /* Use next entry in SWP array for this menu.  Prefill it with the */
    /* current size/position of the menu window.  Then adjust the      */
    /* size/position information to place the menu as required in frame*/

    WinQueryWindowPos(MenuInfo->MenuHwnd, &SwpArray[CtrlCount]);

    /* If frame is minimized, hide the UC menus so they don't paint    */
    /* over the minimized icon.                                        */

    if (SwpFrame.fl & SWP_MINIMIZE) {
      SwpArray[CtrlCount].fl = SwpArray[CtrlCount].fl | SWP_HIDE;
      CtrlCount++;  /* Use next entry in SWP array for next UC menu */
      continue;     /* Skip remainder and look at next UC menu */
    }
    SwpArray[CtrlCount].fl = SwpArray[CtrlCount].fl | SWP_SHOW;

    /* Now place and size the UCMenu and adjust any other frame        */
    /* controls that are effected by it.                               */
    /* Note we do not need to check to see that there is room for the  */
    /* adjustments -- the frame subclass proc will insure the frame is */
    /* never sized too small to hold all frame controls.               */

    switch (MenuInfo->Position) {
      case UCMUTILS_PLACE_TOP:
        /* Put under menu (or title bar if no menu), spanning from left */
        /* to right edge of the frame (but inside borders).             */
        if (IndexMenu >= 0)
          SwpArray[CtrlCount].y = SwpArray[IndexMenu].y - SwpArray[CtrlCount].cy;
          else SwpArray[CtrlCount].y = SwpArray[IndexTitlebar].y - SwpArray[CtrlCount].cy;
        SwpArray[CtrlCount].x = BorderSize.x;
        SwpArray[CtrlCount].cx= SwpFrame.cx - (2*BorderSize.x);
        /* Adjust vertical scroll bar & client to stop at bottom of menu bar */
        if (IndexVScroll >= 0)
          SwpArray[IndexVScroll].cy = SwpArray[IndexVScroll].cy - SwpArray[CtrlCount].cy;
        if (IndexClient >= 0)
          SwpArray[IndexClient].cy  = SwpArray[IndexClient].cy  - SwpArray[CtrlCount].cy;
        IndexUCTop = CtrlCount;  /* Note index of TOP UC menu for LEFT/RIGHT usage */
        break;
      case UCMUTILS_PLACE_BOTTOM:
        /* Place at bottom of frame, below H scroll bar (if one).  Menu */
        /* will span from left to right edge, inside frame border.      */
        SwpArray[CtrlCount].y = BorderSize.y;
        SwpArray[CtrlCount].x = BorderSize.x;
        SwpArray[CtrlCount].cx= SwpFrame.cx - (2*BorderSize.x);
        /* Move up and shrink size of client and V scroll bars */
        if (IndexVScroll >= 0) {
          SwpArray[IndexVScroll].y  = SwpArray[IndexVScroll].y  + SwpArray[CtrlCount].cy;
          SwpArray[IndexVScroll].cy = SwpArray[IndexVScroll].cy - SwpArray[CtrlCount].cy;
        }
        if (IndexClient >= 0) {
          SwpArray[IndexClient].y   = SwpArray[IndexClient].y   + SwpArray[CtrlCount].cy;
          SwpArray[IndexClient].cy  = SwpArray[IndexClient].cy  - SwpArray[CtrlCount].cy;
        }
        /* Move H scroll bar up */
        if (IndexHScroll >= 0)
          SwpArray[IndexHScroll].y = SwpArray[IndexHScroll].y + SwpArray[CtrlCount].cy;
        IndexUCBot = CtrlCount;  /* Note index of BOTTOM UC menu for LEFT/RIGHT usage */
        break;
      case UCMUTILS_PLACE_LEFT:
        SwpArray[CtrlCount].x = BorderSize.x;
        /* Place menu above BOTTOM UC menu, or lower border. */
        if (IndexUCBot >= 0)
          Posn = SwpArray[IndexUCBot].y   + SwpArray[IndexUCBot].cy;
        else Posn = BorderSize.y;
        SwpArray[CtrlCount].y = Posn;
        /* Size up to TOP UC menu, text menu bar, titlebar, or upper border.  */
        if (IndexUCTop >= 0)
          Posn = SwpArray[IndexUCTop].y - Posn;
        else if (IndexMenu >= 0)
          Posn = SwpArray[IndexMenu].y - Posn;
        else if (IndexTitlebar >= 0)
          Posn = SwpArray[IndexTitlebar].y - Posn;
        else Posn = BorderSize.y - Posn;
        SwpArray[CtrlCount].cy = Posn;
        /* Move & shrink client (x direction) and H scroll. */
        if (IndexClient >= 0) {
          SwpArray[IndexClient].x  = SwpArray[IndexClient].x  + SwpArray[CtrlCount].cx;
          SwpArray[IndexClient].cx = SwpArray[IndexClient].cx - SwpArray[CtrlCount].cx;
        }
        if (IndexHScroll >= 0) {
          SwpArray[IndexHScroll].x  = SwpArray[IndexHScroll].x  + SwpArray[CtrlCount].cx;
          SwpArray[IndexHScroll].cx = SwpArray[IndexHScroll].cx - SwpArray[CtrlCount].cx;
        }
        break;
      case UCMUTILS_PLACE_RIGHT:
        /* Place against right edge of frame, inside border. */
        SwpArray[CtrlCount].x = SwpFrame.cx-BorderSize.x-SwpArray[CtrlCount].cx;
        /* Place menu above BOTTOM UC menu, or lower border. */
        if (IndexUCBot >= 0)
          Posn = SwpArray[IndexUCBot].y   + SwpArray[IndexUCBot].cy;
        else Posn = BorderSize.y;
        SwpArray[CtrlCount].y = Posn;
        /* Size up to TOP UC menu, text menu bar, titlebar, or upper border.  */
        if (IndexUCTop >= 0)
          Posn = SwpArray[IndexUCTop].y - Posn;
        else if (IndexMenu >= 0)
          Posn = SwpArray[IndexMenu].y - Posn;
        else if (IndexTitlebar >= 0)
          Posn = SwpArray[IndexTitlebar].y - Posn;
        else Posn = BorderSize.y - Posn;
        SwpArray[CtrlCount].cy = Posn;
        /* Shrink client (x direction) and H scroll. */
        if (IndexClient >= 0)
          SwpArray[IndexClient].cx = SwpArray[IndexClient].cx - SwpArray[CtrlCount].cx;
        if (IndexHScroll >= 0)
          SwpArray[IndexHScroll].cx = SwpArray[IndexHScroll].cx - SwpArray[CtrlCount].cx;
        /* Move V scroll to left of UC menu */
        if (IndexVScroll >= 0)
          SwpArray[IndexVScroll].x = SwpArray[CtrlCount].x - SwpArray[IndexVScroll].cx;
        break;
    } /* switch on placement */

    CtrlCount++;  /* Use next entry in SWP array for next UC menu */
  } /* for each UC menu in the frame */

  /* Return (updated) number of frame controls */
  return MRFROMSHORT(CtrlCount);

}

/******************************************************************************/
MRESULT EXPENTRY UCMU_FrameSubclassProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
/******************************************************************************/
/* This is the frame subclass procedure.  This procedure is used to subclass  */
/* all frames to which UC menus have been added as frame controls.  It will   */
/* process certain messages relating to the formatting of the frame, and then */
/* pass the messages on to the original frame window procedure.  This         */
/* procedure works for dialog windows as well as PM frame windows.            */
/******************************************************************************/
{
MRESULT Result;        /* Message result to return */
FRAMEINST *FrameInfo;  /* Ptr to our frame instance data */
MENUINST  *MenuInfo;   /* Ptr ot our menu instance data */
USHORT  CtrlCount;     /* Total number of controls in frame */
RECTL   *NewRect;      /* New size of frame/client rectangle */
SWP     MenuSwp;       /* Size information of a UC menu */

  /* We need the per-frame data we stored when the */
  /* frame was subclassed in UCMUtilsAddToFrame(). */

  for (FrameInfo=UCMUFrameList.Next; FrameInfo!=NULL; FrameInfo=FrameInfo->Next) {
    if (FrameInfo->FrameHwnd == hwnd)
      break;
  }

  /* If we did not find the frame data we are in big trouble, becase   */
  /* we don't know what original frame procedure to call.  This should */
  /* never occure if we keep proper track of our subclassing.          */
  if (FrameInfo == NULL) {
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Subclass message detected for "\
                  "unknown frame window.\n\nMessage will be ignored and unexpected "\
                  "results may occur.",
                  "Internal UCMUtils Processing Error:",
                  0, MB_OK|MB_ERROR);
    return 0;  /* Don't know what else to do! */
  }

  /* Now process messages that are of interest for the frame controls */

  switch (msg) {
    case WM_QUERYTRACKINFO: {
      /* Limit how small the window can be made.  Limit is the sum of   */
      /* space needed by all frame controls (including UC menus).  For  */
      /* dialogs, the minimum window size is the original size of the   */
      /* dialog, plus the frame controls (to prevent any dialog ctrls   */
      /* from overlaying any frame controls).                           */

      /* Use the normal window procedure to fill in the TRACKINFO data  */
      /* structure, then modify the structure to set a minimum sizing   */
      /* limit on the tracking rectangle.                               */
      SWP       PosData;
      RECTL     FrameSize;

      Result = (*(FrameInfo->FrameProc))(hwnd, msg, mp1, mp2);  /* Do default processing */

      /* Do this only if we are not minimized (we get this message when */
      /* the icon is moved -- don't mess with it in that case).         */

      WinQueryWindowPos(hwnd, &PosData);
      if (!(PosData.fl & SWP_MINIMIZE)) {
        /* Determine the mimum window size allowed, such that there is  */
        /* always room for all frame controls, and for dialogs, that    */
        /* there is always room for all the dialog controls.            */

        if (FrameInfo->IsDialog) {
          /* For dialogs, we cannot use WinCalcFrameRect() because PM   */
          /* does not handle this for dialogs (the returned values do   */
          /* not account for any frame controls).  So we must take the  */
          /* original dialog size and add space for all UC menus.       */
          FrameSize.xLeft  = 0;
          FrameSize.yBottom= 0;
          FrameSize.xRight = FrameInfo->DlgSize.cx;
          FrameSize.yTop   = FrameInfo->DlgSize.cy;
          for (MenuInfo=(FrameInfo->MenuList).Next; MenuInfo!=NULL; MenuInfo=MenuInfo->Next) {
            WinQueryWindowPos(MenuInfo->MenuHwnd, &MenuSwp);
            switch (MenuInfo->Position) {
              case UCMUTILS_PLACE_TOP:      /* Adjust y dimension of rectangle */
              case UCMUTILS_PLACE_BOTTOM:
                FrameSize.yTop = FrameSize.yTop + MenuSwp.cy;
                break;
              case UCMUTILS_PLACE_LEFT:     /* Adjust x dimension of rectangle */
              case UCMUTILS_PLACE_RIGHT:
                FrameSize.xRight = FrameSize.xRight + MenuSwp.cx;
            } /* switch */
          } /* for each UC menu in the frame */
        }
        else {  /* This frame is NOT a dialog, it has a client window. */
          /* We use WinCalcFrameRect() to determine how big the frame  */
          /* must be to hold a zero-sized client.  This will include   */
          /* space needed for all frame controls (incld UC menus).     */
          FrameSize.xLeft = 0;
          FrameSize.xRight= 0;
          FrameSize.yTop  = 0;
          FrameSize.yBottom=0;
          WinCalcFrameRect(hwnd, &FrameSize, FALSE);
        }

        /* If the frame procedure (or a lower subclass) has set a larger minimum */
        /* size for the frame, use the larger value.                             */
        ((TRACKINFO *)mp2)->ptlMinTrackSize.x = max(FrameSize.xRight - FrameSize.xLeft,
                                                    ((TRACKINFO *)mp2)->ptlMinTrackSize.x);
        ((TRACKINFO *)mp2)->ptlMinTrackSize.y = max(FrameSize.yTop   - FrameSize.yBottom,
                                                    ((TRACKINFO *)mp2)->ptlMinTrackSize.y);
      } /* if not minimized */

      return (Result);
      }

    case WM_FORMATFRAME:
      /* Call function to handle this complex message */
      return UCMU_FormatFrameMsg(hwnd, mp1, mp2, FrameInfo);

    case WM_QUERYFRAMECTLCOUNT:
      /* Frame needs a count of number of frame controls.  Pass this message */
      /* down to other frame procedures, then add our own before returning.  */

      CtrlCount = SHORT1FROMMR(
                  (*(FrameInfo->FrameProc))(hwnd, WM_QUERYFRAMECTLCOUNT, mp1, mp2)
                  );
      return MRFROMSHORT(CtrlCount + FrameInfo->MenuCount);

    case WM_CALCFRAMERECT:
      /* This mesage is to calculate how big the client area is for a given */
      /* frame, or how big a frame is needed for a given client.  We will   */
      /* add or subtract the sizes of the UC menus.                         */
      NewRect = PVOIDFROMMP(mp1);

      if (SHORT1FROMMP(mp2)) { /* Was the frame provided? */
        /* Call lower frame window procs to get their space requirements */
        Result = (*(FrameInfo->FrameProc))(hwnd, msg, mp1, mp2);

        /* Remove space from the client to make room for our UC menus */
        for (MenuInfo=(FrameInfo->MenuList).Next; MenuInfo!=NULL; MenuInfo=MenuInfo->Next) {
          WinQueryWindowPos(MenuInfo->MenuHwnd, &MenuSwp);
          switch (MenuInfo->Position) {
            case UCMUTILS_PLACE_TOP:      /* Adjust y dimension of rectangle */
              NewRect->yTop = NewRect->yTop - MenuSwp.cy;
              break;
            case UCMUTILS_PLACE_BOTTOM:
              NewRect->yBottom = NewRect->yBottom + MenuSwp.cy;
              break;
            case UCMUTILS_PLACE_LEFT:     /* Adjust x dimension of rectangle */
              NewRect->xLeft = NewRect->xLeft + MenuSwp.cx;
              break;
            case UCMUTILS_PLACE_RIGHT:
              NewRect->xRight = NewRect->xRight - MenuSwp.cx;
              break;
          } /* switch */
        } /* for each UC menu in the frame */

      } else { /* Client was provided, so add in space UC menus need */
        for (MenuInfo=(FrameInfo->MenuList).Next; MenuInfo!=NULL; MenuInfo=MenuInfo->Next) {
          WinQueryWindowPos(MenuInfo->MenuHwnd, &MenuSwp);
          switch (MenuInfo->Position) {
            case UCMUTILS_PLACE_TOP:      /* Adjust y dimension of rectangle */
            case UCMUTILS_PLACE_BOTTOM:
              NewRect->yBottom = NewRect->yBottom - MenuSwp.cy;
              break;
            case UCMUTILS_PLACE_LEFT:     /* Adjust x dimension of rectangle */
            case UCMUTILS_PLACE_RIGHT:
              NewRect->xRight = NewRect->xRight + MenuSwp.cx;
          } /* switch */
        } /* for each UC menu in the frame */

        /* Call lower frame window procs to get their space requirements */
        Result = (*(FrameInfo->FrameProc))(hwnd, msg, mp1, mp2);
      }
      return Result;  /* End of WM_CALCFRAMERECT processing */

    case WM_PAINT: {
      /* When there are both horz and vert scroll bars, we must paint the */
      /* small rectangle where they intersect.  We will paint with the    */
      /* color specified in the presentation parms.  Note that we cannot  */
      /* use WinBeginPaint() because the default frame proc may use it    */
      /* and it can be called only once per WM_PAINT.                     */
      HWND HorzHwnd, VertHwnd;
      SWP  HorzSwp,  VertSwp;
      RECTL PaintArea;
      HPS   Pres;
      LONG  PaintColor;

      /* Call lowerlever frame procs to do normal painting */
      Result = (*(FrameInfo->FrameProc))(hwnd, msg, mp1, mp2);

      HorzHwnd = WinWindowFromID(hwnd, FID_HORZSCROLL);
      VertHwnd = WinWindowFromID(hwnd, FID_VERTSCROLL);
      if ((HorzHwnd!=NULLHANDLE) && (VertHwnd!=NULLHANDLE)) {
        WinQueryWindowPos(HorzHwnd, &HorzSwp);
        WinQueryWindowPos(VertHwnd, &VertSwp);
        PaintArea.xLeft = HorzSwp.x + HorzSwp.cx;
        PaintArea.xRight= PaintArea.xLeft + VertSwp.cx;
        PaintArea.yBottom = HorzSwp.y;
        PaintArea.yTop    = PaintArea.yBottom + HorzSwp.cy;
        /* Get PS to draw in, but don't draw over other sibling windows    */
        /* just in case some higher frame subclass has moved the controls. */
        Pres = WinGetClipPS(hwnd, NULLHANDLE, PSF_CLIPCHILDREN | PSF_CLIPSIBLINGS);
        /* Use color specified in this windows pres parms, or system default if none */
        if (WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, 0L, NULL,
                              sizeof(LONG), &PaintColor, 0L) != 0)
          PaintColor = GpiQueryColorIndex(Pres, 0L, PaintColor);  /* Convert from RGB to index */
        else PaintColor = SYSCLR_WINDOW;
        WinFillRect(Pres, &PaintArea, PaintColor);
        WinReleasePS(Pres);
      }
      return Result;
      }

    case WM_DESTROY:
      /* If frame is destroyed while we still have it subclassed, we need */
      /* to cleanup the storage we allocated for the frame and UC menus.  */
      /* First, pass message down for normal processing since a lower     */
      /* window proc may play with the UC menus during WM_DESTROY.        */

      Result = (*(FrameInfo->FrameProc))(hwnd, msg, mp1, mp2);

      /* The above may have made the FrameInfo invalid, if the application */
      /* removed the UC menus from the frame during WM_DESTROY.  Thus we   */
      /* must verify that we still have an entry for this frame.           */

      for (FrameInfo=UCMUFrameList.Next; FrameInfo!=NULL; FrameInfo=FrameInfo->Next) {
        if (FrameInfo->FrameHwnd == hwnd)
          break;
      }

      if (FrameInfo == NULL)  /* App must have removed all UC menus */
        return Result;        /* Cleanup is done. */

      /* Remove each UC menu from the list for this frame */
      for (MenuInfo=(FrameInfo->MenuList).Next; MenuInfo!=NULL; MenuInfo=MenuInfo->Next)
        UCMU_ListDelete(MenuInfo);

      /* Remove subclass and delete frame from our frame list */
      WinSubclassWindow(hwnd, FrameInfo->FrameProc);
      UCMU_ListDelete(FrameInfo);
      return Result;

  } /* switch on msg */

  /* Call original frame procedure */
  return (*(FrameInfo->FrameProc))(hwnd, msg, mp1, mp2);

}


/******************************************************************************/
MRESULT EXPENTRY UCMU_MenuSubclassProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
/******************************************************************************/
/* This procedure is used to subclass all UC menu windows which are frame     */
/* controls.  The only purpose of this procedure is to notice when the        */
/* size of the UC menu window changes (such as when the user adds or removes  */
/* the text under the bitmaps, or changes the sizes of the bitmaps).  When    */
/* a frame control window changes its size, the frame must be reformatted to  */
/* adjust all the other frame controls and client area.                       */
/*                                                                            */
/* If the menu window is destroyed, we will automatically remove it from      */
/* the list of frame controls.                                                */
/******************************************************************************/
{
FRAMEINST *FrameInfo;
MENUINST  *MenuInfo;
HWND      MyFrame;
MRESULT   Result;

  /* Frame window is the parent of control */
  MyFrame = WinQueryWindow(hwnd, QW_PARENT);

  /* Find the frame window in our list of subclassed frames (if we are */
  /* subclassing the UC menu, we must also be subclassing the frame).  */

  for (FrameInfo=UCMUFrameList.Next; FrameInfo!=NULL; FrameInfo=FrameInfo->Next) {
    if (FrameInfo->FrameHwnd == MyFrame)
      break;
  }
  if (FrameInfo==NULL) {
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Menu subclass message detected for "\
                  "unknown frame window.\n\nMessage will be ignored and unexpected "\
                  "results may occur.",
                  "Internal UCMUtils Processing Error:",
                  0, MB_OK|MB_ERROR);
    return 0;  /* Don't know what else to do! */
  }

  /* Now find this particular UC menu in the list of UC menus in the frame */

  for (MenuInfo=(FrameInfo->MenuList).Next; MenuInfo!=NULL; MenuInfo=MenuInfo->Next) {
    if (MenuInfo->MenuHwnd == hwnd)
      break;
  }
  if (MenuInfo==NULL) {
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Menu subclass message detected for "\
                  "unknown menu window.\n\nMessage will be ignored and unexpected "\
                  "results may occur.",
                  "Internal UCMUtils Processing Error:",
                  0, MB_OK|MB_ERROR);
    return 0;  /* Don't know what else to do! */
  }

  /* Finally, if the window size has changed, update the frame, and then */
  /* pass the message on for processing by the original window procedure.*/

  switch (msg) {
    case WM_WINDOWPOSCHANGED:
      /* If frame is a dialog, adjust size based on new and old SWP structures */
      /* since the WM_UPDATEFRAME will not adjust the dialog size.             */
      if (FrameInfo->IsDialog) {
        SWP *SwpOld, *SwpNew;
        SwpNew = (SWP *)mp1;
        SwpOld = SwpNew + 1;
        UCMUtilsAdjustDlgSize(FrameInfo->FrameHwnd,
                              SwpNew->cx - SwpOld->cx,
                              SwpNew->cy - SwpOld->cy,
                              MenuInfo->Position);
      }
      WinSendMsg(MyFrame, WM_UPDATEFRAME, MPFROMSHORT(FCF_VERTSCROLL), 0L);
      break;
    case WM_DESTROY:
      /* Pass message on, then remove menu from frame */
      Result =  (*(MenuInfo->MenuProc))(hwnd, msg, mp1, mp2);
      UCMUtilsRemoveFromFrame(MyFrame, hwnd);
      return Result;
  }

  return (*(MenuInfo->MenuProc))(hwnd, msg, mp1, mp2);
}

/******************************************************************************/
PVOID UCMU_ListInsert(PVOID Element, ULONG Size)
/******************************************************************************/
/* Generic linked-list insert routine.  Given an element of a double-linked   */
/* list, a new element will be inserted after it.  The new element will be    */
/* allocated to the size specified.  The first two pointers of the data       */
/* element are used for forward and backward pointers.  The remainder of the  */
/* new element is initialized to binary zeros.                                */
/******************************************************************************/
{
GENERICLIST *New, *Current;

  Current = (GENERICLIST *)Element;  /* Avoid lots of typcasting */

  /* Allocate new element of specified size and zero it */
  New = malloc(Size);
  memset(New, 0, Size);

  /* Insert new element after current element */
  New->Next = Current->Next;  /* Fixup forward links */
  Current->Next = New;
  if (New->Next != NULL)      /* Fixup backward links */
    (New->Next)->Prev = New;
  New->Prev = Current;
}

/******************************************************************************/
void UCMU_ListDelete(PVOID Element)
/******************************************************************************/
/* Generic linked-list delete routine.  This routine will delete the element  */
/* of the linked list specified.  The storage for the element will be freed.  */
/* Note that there is no head-pointer fixup -- it is assumed the first node   */
/* of the list is never deleted.  This simplifies the parameters and code at  */
/* the slight expense of extra storage for one unused list element.           */
/******************************************************************************/
{
GENERICLIST *Current;

  Current = (GENERICLIST *)Element;  /* Avoid lots of typcasting */

  /* Fixup forward links -- no need to test for head of list */
  (Current->Prev)->Next = Current->Next;

  /* Fixup reverse links -- check for end of list */
  if ((Current->Next) != NULL)
    (Current->Next)->Prev = Current->Prev;

  free(Current);
}

/******************************************************************************/
void UCMUtilsFreeUCMInfoStrings(UCMINFO *UCMData)
/******************************************************************************/
/* Free all valid strings in the UCMINFO structure.  An application can use   */
/* this routine after UCMENU_QUERYUCMINFO to free the strings allocated by    */
/* the UCMenu control.                                                        */
/******************************************************************************/
{
  if (UCMData->pszBitmapSearchPath != NULL)
    UCMenuFree(UCMData->pszBitmapSearchPath);

  if (UCMData->pszDefaultBitmap != NULL)
    UCMenuFree(UCMData->pszDefaultBitmap);

  if (UCMData->pszFontNameSize != NULL)
    UCMenuFree(UCMData->pszFontNameSize);
}

/******************************************************************************/
MRESULT UCMUtilsGetNextMenu(HWND Parent, HWND CurrHwnd, BOOL StartOfMenu, ...)
/******************************************************************************/
/* This routine will return the 'next' or 'previous' menu in a supplied list  */
/* of menu IDs.  The returned value should be returned to PM as the result    */
/* of the WM_NEXTMENU message.  This allows UCMenus to participate in the     */
/* menu ring (e.g. selection cursor moves from menu to menu with the left and */
/* right arrow keys).  The variable list of arguments must be a list of       */
/* USHORT windows IDs in the order to be traversed.  The list must be         */
/* terminated with an ID of zero, and all the windows must be immediate       */
/* children of the 'Parent' window.  'CurrHwnd' is the menu with the current  */
/* selection and StartOfMenu indicates direction of traversal.                */
/*                                                                            */
/* This routine will take care of the fact that UCMenus uses a real PM menu   */
/* 'under the covers' and it is this real PM menu handle that must be         */
/* returned in order to traverse UCMenus in the menu ring.  This routine will */
/* also automatically skip menus (of any kind) which are not visible (e.g.    */
/* have been hidden).                                                         */
/******************************************************************************/
{
USHORT   CurrMenuID, TargetID;
USHORT   *List;
int      ListCnt, Dir, i;
va_list  VarList;
HWND     NextMenu;
char     ClassName[10];

  /* Count number of IDs that were passed and allocate array for them */

  va_start(VarList, StartOfMenu);        /* Start of list */
  TargetID = va_arg(VarList, USHORT);    /* Get 1st USHORT */
  ListCnt  = 0;
  while (TargetID != 0) {                /* Until end of list marker */
    ListCnt++;
    TargetID = va_arg(VarList, USHORT);
  }
  va_end(VarList);
  if (ListCnt == 0)
    return (MRESULT) NULLHANDLE;         /* No list supplied */

  List = malloc(ListCnt * sizeof(USHORT));

  /* Load ID list into array for easy access */

  va_start(VarList, StartOfMenu);        /* Start of list */
  TargetID = va_arg(VarList, USHORT);    /* Get 1st USHORT */
  i = 0;
  while (TargetID != 0) {                /* Until end of list marker */
    List[i] = TargetID;
    i++;
    TargetID = va_arg(VarList, USHORT);
  }
  va_end(VarList);

  /* Now find the current menu in the list of IDs */

  CurrMenuID = WinQueryWindowUShort(CurrHwnd, QWS_ID);
  i = 0;
  while (List[i] != CurrMenuID) {
    i++;
    if (i >= ListCnt) {                  /* Current menu not in list */
      free(List);
      return (MRESULT)NULLHANDLE;
    }
  }

  /* Set direction and find next visible menu */
  if (StartOfMenu)
    Dir = -1;
    else Dir = 1;

  do {
    i = i + Dir;                   /* Next in ID list */
    if (i>=ListCnt) i = 0;         /* Wrap around */
      else if (i<0) i = ListCnt-1;

    if (List[i] == CurrMenuID) {   /* Wrapped to starting point */
      free(List);
      return (MRESULT) NULLHANDLE;
    }

    NextMenu = WinWindowFromID(Parent, List[i]);
    if (WinIsWindowVisible(NextMenu)) {
      /* If this is a UCMenu, look for underlaying PM menu */
      /* which will have same ID as the UCMenu.            */
      WinQueryClassName(NextMenu, sizeof(ClassName), ClassName);
      if (!strcmp(ClassName, "UCMenu"))
        /* We must return not the UCMenu handle, but the handle of the real */
        /* underlaying PM menu.  Currently we just hardcode the internal    */
        /* UCMenu window structure.  To make this more general we should    */
        /* really enumerate all the child and sub-children of the UCMenu    */
        /* window and find the one with an ID that matches the UCMenu ID.   */
        NextMenu = WinWindowFromID(WinWindowFromID(WinWindowFromID(NextMenu, 0),0),List[i]);
      free(List);
      return (MRESULT)NextMenu;
    }
  } while(TRUE);  /* Loop until we find next menu or wrap to starting point */

} /* procedure UCMUtilsGetNextMenu */
