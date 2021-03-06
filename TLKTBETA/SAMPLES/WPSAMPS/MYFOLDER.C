
/*
 *  This file was generated by the SOM Compiler.
 *  Generated using:
 *     SOM incremental update: 2.7
 */

/******************************************************************************
*
*  Module Name: MYFOLDER.C
*
*  OS/2 Work Place Shell IDL File BETA Sample Program
*
*  Copyright (C) 1993 IBM Corporation
*
*      DISCLAIMER OF WARRANTIES.  The following [enclosed] code is
*      sample code created by IBM Corporation. This sample code is not
*      part of any standard or IBM product and is provided to you solely
*      for  the purpose of assisting you in the development of your
*      applications.  The code is provided "AS IS", without
*      warranty of any kind.  IBM shall not be liable for any damages
*      arising out of your use of the sample code, even if they have been
*      advised of the possibility of such damages.                                                    *
*
******************************************************************************/

#define INCL_DOS
#define INCL_PM
#include <os2.h>

#define MYFOLDER_Class_Source
#include "myfolder.ih"

/* global used to set the handle for the FIRST opened view of an object
 * of class MYFOLDER.  This is a quick and dirty way to do this, you would
 * probably want to have an instance variable (i.e. per object) to represent
 * this meaningfully, but this is effective for demonstration purposes.
 */
HWND hwndFirstView = NULLHANDLE;

/*
 * SOM_Scope HWND   SOMLINK myfold_wpOpen(MYFOLDER *somSelf,
 *                 HWND hwndCnr,
 *                 ULONG ulView,
 *                 ULONG param)
 *
 * this wpOpen override does several special things:
 *
 * 1 - if the global hwndFirstView is not set yet, it assumes the
 *     folder hasn't been opened yet.  When this is the case, a
 *     MYFOLDER object will populate itself, and then search through
 *     its contents for WPFolder objects and delete them before
 *     opening the view (calling parent_wpOpen).  This may be useful
 *     if you want a particular class of folder to clean up some of
 *     its contents on a new boot, etc.
 *
 * 2 - After one MYFOLDER object has been opened once, hwndFirstView
 *     will be set to the hwnd for that first view, to be used later
 *     on in CloseViews below
 */

SOM_Scope HWND  SOMLINK myfold_wpOpen(MYFOLDER *somSelf, HWND hwndCnr,
                                      ULONG ulView, ULONG param)
{
    HWND        hwndParent = NULLHANDLE;
    /* MYFOLDERData *somThis = MYFOLDERGetData(somSelf); */
    MYFOLDERMethodDebug("MYFOLDER","myfold_wpOpen");

    if (hwndFirstView == NULLHANDLE)
    {

       /* This will enable multiple views for MYFOLDER objects.
        * Again, this method could be better placed (e.g. in an
        * override for myfold_wpInitData or a similar initialization
        * method)
        */
       _wpSetConcurrentView(somSelf, (USHORT) CCVIEW_ON);

       /* we ONLY want to do this on the FIRST open of the folder.
        * notice that hwndFirstView is a global that we set after
        * the folder has been opened once.
        */
       if (_wpPopulate(somSelf, NULLHANDLE, NULL, FALSE))
        {
           WPObject *Obj;
           WPObject *LastFoundObj = NULL;

           /* loop through the contents of the folder, checking each to
            * see if it is an instance of a class we want to delete
            */
           for ( Obj = _wpQueryContent(somSelf,NULL,(ULONG)QC_First);
                 Obj;
                 Obj = _wpQueryContent(somSelf, Obj, (ULONG) QC_Next ))
           {
              /* delete the last object found on the previous iteration
               * of the loop (we couldn't delete it then, because we
               * needed it at the top of the loop for this iteration)
               */
              if (LastFoundObj)
              {
                 _wpDelete(LastFoundObj,0);
                 LastFoundObj = NULL;
              }

              /* for example, we want to make sure that all previously
               * existing WPFolders are deleted
               */
              if (_somIsA(Obj, _WPFolder))
              {
                 LastFoundObj = Obj;
              }

           }  /* end for */


           /* if there is still one object left to delete, do it now
            */
           if (LastFoundObj)
           {
              _wpDelete(LastFoundObj,0);
              LastFoundObj = NULL;
           }
       } /* end if populate */
    } /* end if first open */

    /* now, show the opened folder */
    hwndParent = parent_wpOpen(somSelf, hwndCnr, ulView, param);

    /* if this is the first open, set our global flag, so everybody knows
     */
    if (hwndFirstView == NULLHANDLE)
    {
        hwndFirstView = hwndParent;
    }

    return hwndParent;
}

/* I put a "close views" option on my context menu, so that I could
 * easily invoke my _CloseViews method
 */

/*
 * SOM_Scope BOOL  SOMLINK myfold_wpModifyPopupMenu(MYFOLDER somSelf,
 *                                                  HWND hwndMenu,
 *                                                  HWND hwndCnr,
 *                                                  ULONG iPosition)
 *
 * this overide adds a new item to the context menu for MYFOLDER objects,
 * one that, when selected, will invoke the _CloseViews method
 */

SOM_Scope BOOL  SOMLINK myfold_wpModifyPopupMenu(MYFOLDER *somSelf,
                                                 HWND hwndMenu,
                                                 HWND hwndCnr,
                                                 ULONG iPosition)
{
    HMODULE    hmod = NULLHANDLE;
    zString    zsPathName;

    /* MYFOLDERData *somThis = MYFOLDERGetData(somSelf); */
    MYFOLDERMethodDebug("MYFOLDER","myfold_wpModifyPopupMenu");

    /* querying the module handle in this manner is very expensive, and
     * placed here for convenience.  it more properly belongs in the
     * class initialization code, where a global hmod could be set once.
     */
    zsPathName =
     _somLocateClassFile( SOMClassMgrObject, SOM_IdFromString("MYFOLDER"),
                          MYFOLDER_MajorVersion, MYFOLDER_MinorVersion);

    DosQueryModuleHandle(zsPathName, &hmod);
    _wpInsertPopupMenuItems( somSelf, hwndMenu, 0, hmod, ID_CLOSEVIEWSMENU, 0);

    return (parent_wpModifyPopupMenu(somSelf,hwndMenu,hwndCnr,iPosition));
}


/*
 * SOM_Scope BOOL   SOMLINK myfold_wpMenuItemSelected(MYFOLDER *somSelf,
 *                 HWND hwndFrame,
 *                 ULONG ulMenuId)
 *
 * this override will invode the _CloseViews method for my new context
 * menu item.
 */
SOM_Scope BOOL  SOMLINK myfold_wpMenuItemSelected(MYFOLDER *somSelf,
                                                  HWND hwndFrame,
                                                  ULONG ulMenuId)
{
    /* MYFOLDERData *somThis = MYFOLDERGetData(somSelf); */
    MYFOLDERMethodDebug("MYFOLDER","myfold_wpMenuItemSelected");

    if (ulMenuId == IDM_CLOSEVIEWS)
    {
       _CloseViews(somSelf);
       return(TRUE);
    }
    else
    {
       return (parent_wpMenuItemSelected(somSelf,hwndFrame,ulMenuId));
    }
}

/*
 * SOM_Scope void  SOMLINK myfold_CloseViews(MYFOLDER somSelf)
 *
 * this will close all open views of somSelf, except for a view with
 * the same hwnd as that set in myfold_wpOpen above (if one of its views
 * was the first one opened, hwndFirstView).
 * I am also assuming that you only are interested in closing open
 * folder views (i.e. OPEN_CONTENTS, OPEN_SETTINGS, OPEN_TREE..., and
 * not OPEN_RUNNING views).
 */
SOM_Scope void  SOMLINK myfold_CloseViews(MYFOLDER *somSelf)
{
    PUSEITEM  pUseItem, pUseItemNext;
    PVIEWITEM pViewItem;
    HWND      hwndView;
    typedef struct _HANDLENODE
    {
       LHANDLE handle;
       struct _HANDLENODE * pNextHandle;
    } HANDLENODE;
    HANDLENODE *pHandles = NULL;
    HANDLENODE *pTemp = NULL;

    /* MYFOLDERData *somThis = MYFOLDERGetData(somSelf); */
    MYFOLDERMethodDebug("MYFOLDER","myfold_CloseViews");

    /* go through use list looking for OPENVIEW use items, as you
     * find one, add view item's handle to a list
     */
    for ( pUseItem = _wpFindUseItem(somSelf , USAGE_OPENVIEW, NULL);
          pUseItem;
          pUseItem = _wpFindUseItem(somSelf , USAGE_OPENVIEW, pUseItem) )
    {
       if (pTemp = (HANDLENODE*)_wpAllocMem(somSelf,sizeof(HANDLENODE),NULL))
       {
          pViewItem = (PVIEWITEM)(pUseItem + 1);
          pTemp->handle = pViewItem->handle;
          pTemp->pNextHandle = pHandles;
          pHandles = pTemp;
       }
    }

    /* go through list of handles we just found and close any
     * views other than the first one
     */
    pTemp = pHandles;
    while (pTemp)
    {
       if (hwndFirstView != (HWND)pTemp->handle )
       {
          WinSendMsg( (HWND)pTemp->handle, WM_CLOSE, 0, 0);
       }
       pTemp = pTemp->pNextHandle;
    }

    /* free our list */
    while (pHandles)
    {
       pTemp = pHandles;
       pHandles = pTemp->pNextHandle;
       _wpFreeMem(somSelf,(PBYTE)pTemp);
    }

}


