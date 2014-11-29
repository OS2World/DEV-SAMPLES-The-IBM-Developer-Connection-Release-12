#define  INCL_WIN
#define  INCL_GPI

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "contain.h"
#include "phones.h"

/***********************************************************************/
/*  Module Overview:                                                   */
/*                                                                     */
/*  This module creates a container window as a child of a given       */
/*  window.                                                            */
/*                                                                     */
/*  After the windows are created successfully, the container          */
/*  window is populated.  First, the container is sent a message to    */
/*  allocate memory for each of the records which will be inserted.    */
/*  After the memory is allocated, we set the values for each record.  */
/*  (This sample program reads data from a static array - you could    */
/*  also load values from a file.) Then, the container is sent a       */
/*  message to insert the records (which makes them visible).          */
/*                                                                     */
/*  This container is read-only, which means the end user cannot       */
/*  change the title text.  It supports single selection.              */
/*                                                                     */
/*  In the message loop, we must check for WM_CONTROL messages,        */
/*  which are generated from the container control.  This sample       */
/*  processes CN_ENTER messages, when an item in the container is      */
/*  selected (either with the mouse or the keyboard), and              */
/*  CN_CONTEXTMENU messages, when a context menu is requested.  The    */
/*  context menu allows the user to change the display mode of the     */
/*  container.  Our container supports Icon, Text, and Name views.     */
/*                                                                     */
/*  When a CN_ENTER message is received, we loop through the array     */
/*  of names until we find a match.  On a match, we pop up a message   */
/*  box which contains the nickname, name, and number of the person    */
/*  selected.                                                          */
/*                                                                     */
/***********************************************************************/

VOID LoadDatabase(HWND);

MRESULT EXPENTRY LocalWndProc(HWND, ULONG, MPARAM, MPARAM);

HWND    hPopupMenu;
HWND    hCnrWnd;
PFNWP   SysWndProc;

/***********************************************************************/
/*  CreateContainer() - PMWIN entry point.                             */
/***********************************************************************/
BOOL CreateContainer(HWND hFrameWnd)
{
   SWP  swp;
   HWND hParentWnd;

   hParentWnd = WinWindowFromID(hFrameWnd, FID_CLIENT);
   WinQueryWindowPos(hParentWnd, &swp); 

   /********************************************************************/
   /*  Create the container.                                           */
   /********************************************************************/
   hCnrWnd = WinCreateWindow(hParentWnd,
                             WC_CONTAINER,
                             NULL,
                             WS_VISIBLE |
                             CCS_AUTOPOSITION |
                             CCS_READONLY |
                             CCS_SINGLESEL,
                             0, 0, swp.cx, swp.cy,
                             hParentWnd,
                             HWND_BOTTOM,
                             1,
                             NULL,
                             NULL);

   /********************************************************************/
   /*  If we got it, fill it up.                                       */
   /********************************************************************/
   if (hCnrWnd)
     LoadDatabase(hCnrWnd);
   else
     return FALSE;

   /********************************************************************/
   /*  We must intercept the frame window's messages                   */
   /*  (to capture any input from the container control).              */
   /*  We save the return value (the current WndProc),                 */
   /*  so we can pass it all the other                                 */
   /*  messages the frame gets.                                        */
   /********************************************************************/
   SysWndProc = WinSubclassWindow(hParentWnd, (PFNWP)LocalWndProc);

   /********************************************************************/
   /*  Load the popup menu from the resources                          */
   /*  and show the frame window.                                      */
   /********************************************************************/
   hPopupMenu = WinLoadMenu(HWND_OBJECT, NULLHANDLE, IDM_DISPLAY);

   return TRUE;
}

/***********************************************************************/
/*  LocalWndProc() - window procedure for the frame window.            */
/*  Called by PM whenever a message is sent to the frame.              */
/***********************************************************************/
MRESULT EXPENTRY LocalWndProc(HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
   char                szBuffer[80];
   CNRINFO             cnrInfo;
   PNOTIFYRECORDENTER  Selected;
   POINTL              pt;
   int                 x;

   switch(msg)
   {
   case WM_CONTROL:
      switch (SHORT2FROMMP(mp1))
      {

      /*****************************************************************/
      /*  Context menu - usually right mouse button clicked            */
      /*  on window. Popup a menu to allow the user to                 */
      /*  select a new view of the container.                          */
      /*****************************************************************/
      case CN_CONTEXTMENU:
         WinQueryPointerPos(HWND_DESKTOP, &pt);
         WinPopupMenu(HWND_DESKTOP,
                      hwnd,
                      hPopupMenu,
                      (SHORT)pt.x,
                      (SHORT)pt.y,
                      IDM_ICON,
                      PU_NONE |
                      PU_MOUSEBUTTON1 |
                      PU_KEYBOARD |
                      PU_SELECTITEM);
         break;

      /*****************************************************************/
      /*  User selected an item - we take the icon text                */
      /*  and spin through the array of Friends, looking for           */
      /*  a match - on match, print out the phone number               */
      /*****************************************************************/
      case CN_ENTER:
         Selected = (PNOTIFYRECORDENTER)mp2;
         for (x = 0; x < MAXFRIENDS; x++)
         {
            if (!strcmpi(Friends[x].NickName,
              Selected->pRecord->pszIcon))
            {
               sprintf(szBuffer,
                       "'%s' (%s) %s",
                       Friends[x].NickName,
                       Friends[x].FullName,
                       Friends[x].Phone);
                       WinMessageBox(HWND_DESKTOP,
                                     HWND_DESKTOP,
                                     szBuffer,
                                     "Phone",
                                     0,
                                     MB_OK);
            }
         }
         break;
      }
      break;

   case WM_COMMAND:
      switch (SHORT1FROMMP(mp1))
      {
      case IDM_ICON:
         cnrInfo.flWindowAttr = CV_ICON;
         break;
      case IDM_NAME:
         cnrInfo.flWindowAttr = CV_NAME;
         break;
      case IDM_TEXT:
         cnrInfo.flWindowAttr = CV_TEXT;
         break;
      default:
         return (*SysWndProc)(hwnd, msg, mp1, mp2);
         break;
      }

      WinSendMsg(hCnrWnd,
                 CM_SETCNRINFO,
                 &cnrInfo,
                 MPFROMLONG(CMA_FLWINDOWATTR));
      break;

   case WM_SIZE:
      WinSetWindowPos(hCnrWnd,
                      NULLHANDLE,
                      0,
                      0,
                      SHORT1FROMMP(mp2),
                      SHORT2FROMMP(mp2),
                      SWP_SIZE|SWP_MOVE);
      break;

   /********************************************************************/
   /*  Send the message to the usual WC_FRAME WndProc.                 */
   /********************************************************************/
   default:
      return (*SysWndProc)(hwnd, msg, mp1, mp2);
      break;
   }
   return (*SysWndProc)(hwnd, msg, mp1, mp2);
}

/***********************************************************************/
/*  LoadDatabase() - utility function                                  */
/*  called after the WC_CONTAINER window is created successfully,      */
/*  allocates and populates container records, and then inserts        */
/*  the records into the container window.                             */
/***********************************************************************/
VOID LoadDatabase (HWND hWnd)
{
   HWND            hIcon;
   PRECORDCORE     Address, FirstRec;
   RECORDINSERT    recsIn;
   ULONG           x;

   /********************************************************************/
   /*  The Icon view for each of the records in the                    */
   /*  container will use the standard File icon,                      */
   /*  so we grab the handle now for reference later.                  */
   /********************************************************************/
   hIcon = WinQuerySysPointer(HWND_DESKTOP, SPTR_FILE, FALSE);

   /********************************************************************/
   /*  Allocate MAXFRIENDS records all at once -                       */
   /*  CM_ALLOCRECORD returns them in a linked list.                   */
   /********************************************************************/
   Address = (PRECORDCORE)WinSendMsg(hWnd,
                                     CM_ALLOCRECORD,
                                     0,
                                     MPFROMLONG(MAXFRIENDS));

   /*******************************************************************/
   /*  We will need the first record's address to                     */
   /*  insert them into the container.                                */
   /*******************************************************************/
   FirstRec = Address;

   /*******************************************************************/
   /*  Loop through the address book, loading as we go.               */
   /*  Because the CM_ALLOCRECORD returns a linked list,              */
   /*  the address of the next record is retrieved                    */
   /*  from each record as we go (preccNextRecord).                   */
   /*******************************************************************/
   for (x = 0; x < MAXFRIENDS; x++)
   {
      Address->cb       = sizeof(RECORDCORE); /* Standard records     */
      Address->hptrIcon = hIcon;              /* File icon            */
      Address->pszIcon  = Friends[x].NickName;
      Address->pszName  = Friends[x].FullName;
      Address->pszText  = Friends[x].FullName;
      Address = Address->preccNextRecord;     /* Next record in list  */
   }

   /*******************************************************************/
   /*  Set up the insert record structure to place                    */
   /*  the records in the container.                                  */
   /*******************************************************************/
   recsIn.cb = sizeof(RECORDINSERT);

   /* Put the records in after any others */
   recsIn.pRecordOrder = (PRECORDCORE)CMA_END;

   /* All the records are top level (not children of other records) */
   recsIn.pRecordParent = NULL;

   /* The icons are top level */
   recsIn.zOrder = (USHORT)CMA_TOP;

   /* Redraw the container */
   recsIn.fInvalidateRecord = TRUE;

   /* Set the number of records to insert */
   recsIn.cRecordsInsert = MAXFRIENDS;

   /*******************************************************************/
   /*  Insert the records into the container.                         */
   /*******************************************************************/
   WinSendMsg(hWnd,
              CM_INSERTRECORD,
              (PRECORDCORE)FirstRec,
              &recsIn);
}
