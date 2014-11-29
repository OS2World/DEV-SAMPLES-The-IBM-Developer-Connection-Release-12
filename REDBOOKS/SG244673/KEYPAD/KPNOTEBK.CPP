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
#ifndef SOM_Module_kpnotebk_Source
#define SOM_Module_kpnotebk_Source
#endif
#define KPNotebook_Class_Source
#define VARIABLE_MACROS

//#ifndef _ORDCOLL_
//#include "OrdColl.h"
//#endif


#include "kpnotebk.h"
#include "kpnoterc.h"
#include "KPNoteBK.xih"
#include "keypad.xh"

extern "C" {
MPARAM EXPENTRY KPPageDlgProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
}

MPARAM EXPENTRY KPPageDlgProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{


   MRESULT             mresReply;
   PBOOKINFO           pbookinfo;
   static KeyPadPart * myPart;
   static ODPart     * myPartWrapper;
   static ODSettingsExtension * myNotebook;
   Environment       * ev;
   ev = somGetGlobalEnvironment();
   ODFrame           * displayFrame;
   CHAR              * cData;
   CHAR                cBuffer[10];
   static ODUShort     idelay=500;
   static ODUShort     whichColor=IDMA_COLOR_GREEN;
   static ODUShort     ColorPrev=IDMA_COLOR_GRAY;
   ODUShort            usChkState;
   ODBoolean           fbeep=TRUE;
   static KPPartDataStruct  PartData;

   /* Get the data for this notebook
    */

   switch(msg)
   {
      case WM_INITDLG:
          if ( !(pbookinfo = (PBOOKINFO)WinQueryWindowULong(WinQueryWindow(
             hwnd, QW_PARENT), QWL_USER)) )
             return  (MRESULT)FALSE;

          myNotebook    = (ODSettingsExtension *)pbookinfo->thisNotebook;
          myPartWrapper = (ODPart *)pbookinfo->thisPart;
          myPart = (KeyPadPart *)myPartWrapper->GetRealPart(ev);
          myPart->KPGetSetting(ev,(PKPPartDataStruct)&PartData);
          whichColor = PartData.Color;
          fbeep      = PartData.BeepOn;
          idelay     = PartData.LinkDelay;

          WinCheckButton( hwnd, (USHORT)whichColor, TRUE) ;

          WinCheckButton(hwnd,ID_CHECKBOX,(boolean)fbeep);
          cData=_ltoa(idelay,cBuffer,10);
          WinSetDlgItemText(hwnd,ID_ENTRYDATA,cData);

          return (MPARAM)0;

      case WM_COMMAND:

        switch (SHORT1FROMMP(mp1))
        {
        case DID_OK:
            mresReply = WinSendMsg(WinWindowFromID(hwnd, IDMA_COLOR_GRAY),
                           BM_QUERYCHECK, 0, 0 );
                  if(mresReply)
                     whichColor = IDMA_COLOR_GRAY;
            mresReply = WinSendMsg(WinWindowFromID(hwnd, IDMA_COLOR_RED),
                           BM_QUERYCHECK, 0, 0 );
                  if(mresReply)
                     whichColor = IDMA_COLOR_RED;
            mresReply = WinSendMsg(WinWindowFromID(hwnd, IDMA_COLOR_GREEN),
                           BM_QUERYCHECK, 0, 0 );
                  if(mresReply )
                     whichColor = IDMA_COLOR_GREEN;
            mresReply = WinSendMsg(WinWindowFromID(hwnd, IDMA_COLOR_YELLOW),
                           BM_QUERYCHECK, 0, 0 );
                  if(mresReply)
                     whichColor = IDMA_COLOR_YELLOW;
            mresReply = WinSendMsg(WinWindowFromID(hwnd, IDMA_COLOR_BLUE),
                           BM_QUERYCHECK, 0, 0 );
                  if(mresReply)
                     whichColor = IDMA_COLOR_BLUE;
            mresReply = WinSendMsg(WinWindowFromID(hwnd, IDMA_COLOR_MAGENTA),
                           BM_QUERYCHECK, 0, 0 );
                  if(mresReply)
                     whichColor = IDMA_COLOR_MAGENTA;
            mresReply = WinSendMsg(WinWindowFromID(hwnd, IDMA_COLOR_CYAN),
                           BM_QUERYCHECK, 0, 0 );
                  if(mresReply)
                     whichColor = IDMA_COLOR_CYAN;
            mresReply = WinSendMsg(WinWindowFromID(hwnd, IDMA_COLOR_BLACK),
                           BM_QUERYCHECK, 0, 0 );
                  if(mresReply)
                     whichColor = IDMA_COLOR_BLACK;

            WinQueryDlgItemShort(hwnd,ID_ENTRYDATA,(PSHORT)&idelay,TRUE);

            if( WinSendDlgItemMsg(hwnd,ID_CHECKBOX,BM_QUERYCHECK,0,0) > 0)
            {
               fbeep=TRUE;
            }else
            fbeep=FALSE;

            myPart->KPChangeSetting(ev, displayFrame,
                                        whichColor, idelay, fbeep);

            KeyPadPartData * somThis = KeyPadPartGetData(myPart);
            somThis->fFrame->Invalidate(ev,kODNULL,kODNULL);

            return (MPARAM)0;
            }

      case WM_CONTROL:
         switch (SHORT1FROMMP(mp1))
         {
                  return (MPARAM)0;
         }
      case WM_FOCUSCHANGE:
         /* On a focus change, we will post a message to process all of the
          * changes to stuff.  We don't want to do it here, because if a
          * method displays an error message, we can't switch focus to the
          * message and will hang.
          */
         WinPostMsg(hwnd, WM_FOCUS_PROCESSING, mp1, mp2);
         return WinDefDlgProc( hwnd, msg, mp1, mp2);

      case WM_FOCUS_PROCESSING:
                  return (MPARAM)0;
      case WM_DESTROY:
         /* save everything, as the post by the WM_FOCUSCHANGE message
          * will not get processed before this message kills the dialog.
          */
         WinSendMsg(hwnd, WM_FOCUS_PROCESSING, mp1, mp2);

         if(myPart) myPartWrapper->ReleaseRealPart(ev);
         return WinDefDlgProc( hwnd, msg, mp1, mp2);

      default:
         return WinDefDlgProc( hwnd, msg, mp1, mp2);
   }

}

SOM_Scope ODULong  SOMLINK KPNotebookInsertBackgroundPage(KPNotebook *somSelf,
                                                           Environment *ev,
                                                          HWND hwndNotebook)
{
    KPNotebookData *somThis = KPNotebookGetData(somSelf);
    KPNotebookMethodDebug("KPNotebook","KPNotebookInsertBackgroundPage");

    PAGEINFO      pageinfo;

    memset( (PCH)&pageinfo, 0, sizeof( PAGEINFO ) );
    pageinfo.cb                = sizeof(PAGEINFO);
    pageinfo.usPageStyleFlags  = BKA_MAJOR;
    pageinfo.usPageInsertFlags = BKA_FIRST;
    pageinfo.pfnwp             = (PFNWP)KPPageDlgProc;
    pageinfo.resid             = _hmodHandle;


    pageinfo.dlgid             = KP_COLORS;
    pageinfo.pszName           = "KeyPad Setting";
    pageinfo.usSettingsFlags    = SETTINGS_PAGE_NUMBERS;
    pageinfo.pCreateParams     = (PVOID)somSelf; /* Let dialog know what object */


    return somSelf->InsertNotebookPage(ev, hwndNotebook,  &pageinfo );
}

SOM_Scope void  SOMLINK KPNotebookAddNotebookPages(KPNotebook *somSelf,
                                                    Environment *ev,
                                                   HWND hwndNotebook)
{
    KPNotebookData *somThis = KPNotebookGetData(somSelf);
    KPNotebookMethodDebug("KPNotebook","KPNotebookAddNotebookPages");

    CHAR LoadError[100];
    APIRET rc = DosLoadModule(LoadError, sizeof(LoadError),
                   "keypad", &_hmodHandle);

    somSelf->InsertBackgroundPage(ev, hwndNotebook);

    KPNotebook_parent_ODSettingsExtension_AddNotebookPages(somSelf,
                                                           ev,
                                                           hwndNotebook);
}

SOM_Scope void  SOMLINK KPNotebooksomInit(KPNotebook *somSelf)
{
    KPNotebookData *somThis = KPNotebookGetData(somSelf);
    KPNotebookMethodDebug("KPNotebook","KPNotebooksomInit");

    _hmodHandle = 0;
    KPNotebook_parent_ODSettingsExtension_somInit(somSelf);
}

SOM_Scope void  SOMLINK KPNotebooksomUninit(KPNotebook *somSelf)
{
    KPNotebookData *somThis = KPNotebookGetData(somSelf);
    KPNotebookMethodDebug("KPNotebook","KPNotebooksomUninit");

    KPNotebook_parent_ODSettingsExtension_somUninit(somSelf);
}

