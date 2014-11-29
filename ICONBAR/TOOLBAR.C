/* ********************************************************************** */
/*                                                                        */
/*   ToolBar  Main Module                                                 */
/*                                                                        */
/*   This sample program implements two menu bars: a standard action bar  */
/*   and a "toolbar". The toolbar consists of "buttons" that can be       */
/*   pressed. The toolbar is actually a regular class menu whose items    */
/*   have the style MIS_BITMAP. The toolbar is positioned in between      */
/*   the standard titlebar and the standard action or menu bar.           */
/*                                                                        */
/*   The purpose of this sample is to demonstrate subclassing the frame   */
/*   window to add frame controls.                                        */
/*                                                                        */
/*   This code was written quickly for demonstration of a technique, not  */
/*   as an example of production level coding. Error checking was left    */
/*   out in many areas to make the code less "cluttered". The code was    */
/*   also written for readability, not for optimal use of resources.      */
/*   In other words, please don't judge it too harshly :-)                */
/*                                                                        */
/*   DISCLAIMER OF WARRANTIES.  The following [enclosed] code is          */
/*   sample code created by IBM Corporation. This sample code is not      */
/*   part of any standard or IBM product and is provided to you solely    */
/*   for  the purpose of assisting you in the development of your         */
/*   applications.  The code is provided "AS IS", without                 */
/*   warranty of any kind.  IBM shall not be liable for any damages       */
/*   arising out of your use of the sample code, even if they have been   */
/*   advised of the possibility of   such damages.                        */
/*                                                                        */
/*   Copyright 1992, IBM Corp                                             */
/*                                                                        */
/*   John D. Webb                                                         */
/*   AUSVM1( V$IJOHNW )                                                   */
/*   CIS: 71075,1117                                                      */
/*   johnw@vnet.ibm.com                                                   */
/*                                                                        */
/* ********************************************************************** */


#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#define INCL_DOSMISC
#define INCL_DOSERRORS
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include "toolbar.h"

HAB     hab;
HMQ     hmq;
HWND    hwndClient;
HWND    hwndFrame;
HWND    hwndToolBar ;
HWND    hwndMenuBar ;
QMSG    qmsg;

PSZ    pszClassName  = "ToolBarClass" ;
PSZ    pszMainTitle  = "ToolBar" ;
PSZ    pszErrorTitle = "ToolBar Error" ;

        /* ----------------  Prototypes  ------------------------ */
MRESULT EXPENTRY MainWindowProc( HWND, USHORT, MPARAM, MPARAM );
MRESULT EXPENTRY NewFrameProc( HWND, USHORT, MPARAM, MPARAM );
VOID             ShowErrorWindow( PSZ, BOOL );



/* ********************************************************************** */
/*                                                                        */
/*   Main                                                                 */
/*                                                                        */
/* ********************************************************************** */

VOID main()
{

  if ( (hab = WinInitialize( 0L )) == (HAB) NULL ){
     DosBeep( 60, 250 );
     DosBeep( 120, 250 );
  }
  else {
     if ( (hmq = WinCreateMsgQueue( hab, 0 )) == (HMQ) NULL ){
        DosBeep( 60, 250 );
        DosBeep( 120, 250 );
        DosBeep( 60, 250 );
     }
     else {

       ULONG fulCreate= FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER |
                        FCF_MINMAX | FCF_SHELLPOSITION | FCF_ICON  ;
               /*
                *  Note: no menu was specificed in create flags
                */

        WinSetPointer( HWND_DESKTOP,
                       WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,TRUE));

        WinRegisterClass(hab, pszClassName, (PFNWP)MainWindowProc, CS_SIZEREDRAW, 0);

        hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                       0L,
                                       (PULONG)&fulCreate,
                                       pszClassName ,
                                       pszMainTitle,
                                       0L,
                                       (HMODULE)NULL,
                                       ID_MAIN_WIN,
                                       &hwndClient);
        if ( hwndFrame == NULLHANDLE ) {
           ShowErrorWindow( "Error creating Main window !", TRUE );
        }
        else {
           PFNWP     pfnwpOldFrameProc ;

             /* ---------  subclass frame proc  ------------------ */
           pfnwpOldFrameProc = WinSubclassWindow( hwndFrame,
                                                  (PFNWP) NewFrameProc );
           if ( pfnwpOldFrameProc == (PFNWP)0L ){
               ShowErrorWindow( "Error subclassing frame window !", TRUE );
           }
           else {
              PID       pid ;
              SWCNTRL   swCntrl;
              HSWITCH   hSwitch ;


                /* -------  store old frame proc with handle  ------- */
              WinSetWindowULong( hwndFrame,
                                 QWL_USER,
                                 (ULONG) pfnwpOldFrameProc );

                /* ------------------ load menus  ------------------- */
              hwndMenuBar = WinLoadMenu( hwndFrame,
                                         (HMODULE)NULL,
                                         MID_MENUBAR );

              hwndToolBar = WinLoadMenu( hwndFrame,
                                         (HMODULE)NULL,
                                         MID_TOOLBAR );
                /*
                 *  Note that the last menu loaded, the toolbar, is the
                 *  one that is associated with the frame as "the" menu.
                 *  this means that hwndMenuBar is the only link to the
                 *  regular action bar, so hang onto it tightly
                 */

                /* ---------  set window size and pos  -------------- */
              WinSetWindowPos( hwndFrame,
                               HWND_TOP,
                               0, 0, 370, 300,
                               SWP_SIZE | SWP_SHOW | SWP_ACTIVATE );

               /* ----------- add program to tasklist  --------------- */
              WinQueryWindowProcess( hwndFrame, &pid, NULL );
              swCntrl.hwnd = hwndFrame ;
              swCntrl.hwndIcon = (HWND) NULL ;
              swCntrl.hprog = (HPROGRAM) NULL ;
              swCntrl.idProcess = pid ;
              swCntrl.idSession = (LONG) NULL ;
              swCntrl.uchVisibility = SWL_VISIBLE ;
              swCntrl.fbJump = SWL_JUMPABLE ;
              strcpy( swCntrl.szSwtitle, pszMainTitle );
              hSwitch = WinAddSwitchEntry((PSWCNTRL)&swCntrl);


              WinSetPointer(HWND_DESKTOP,
                            WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,TRUE));

                 /* ---------- start the main processing loop ----------- */
              while (WinGetMsg(hab, &qmsg,NULLHANDLE,0,0)){
                  WinDispatchMsg(hab, &qmsg);
              }

              WinRemoveSwitchEntry( hSwitch );
           } /* end of else ( pfnwpOldFrameProc ) */

           WinSetPointer(HWND_DESKTOP,
                         WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,TRUE));
           WinDestroyWindow(hwndFrame);
        }  /* end of else (hwndFrame == NULLHANDLE) */

        WinSetPointer(HWND_DESKTOP,
                      WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,TRUE));
        WinDestroyMsgQueue(hmq);
     }  /* end of else ( ...WinCreateMsgQueue() */

   WinTerminate(hab);
   }  /* end of else (...WinInitialize(NULL) */
}  /*  end of main() */

/* ********************************************************************** */
/*                                                                        */
/*   MainWindowProc                                                       */
/*                                                                        */
/* ********************************************************************** */

MRESULT EXPENTRY
MainWindowProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{

  switch (msg) {

    case WM_PAINT:
       {
         RECTL rectl ;
         HPS   hps;

         hps = WinBeginPaint( hwnd, (HPS) NULL, &rectl );
         WinFillRect( hps, (PRECTL)&rectl, SYSCLR_WINDOW);
         WinEndPaint( hps );
      }
      break;

       /* --------  Handle commands from *both* menus ---------- */
    case WM_COMMAND :
        {
        switch ( SHORT1FROMMP( mp1 )){
             /* ----- Toolbar items -------- */
          case  MID_TB_1 :
               DosBeep( 440,250 ) ;
               break;
          case  MID_TB_2 :
               DosBeep( 466,250 ) ;
               break;
          case  MID_TB_3 :
               DosBeep( 494,250 ) ;
               break;
          case  MID_TB_4 :
               DosBeep( 524,250 ) ;
               break;
          case  MID_TB_5 :
               DosBeep( 550,250 ) ;
               break;
          case  MID_TB_6 :
               DosBeep( 588,250 ) ;
               break;
          case  MID_TB_7 :
               DosBeep( 624,250 ) ;
               break;
          case  MID_TB_8 :
               DosBeep( 662,250 ) ;
               break;
          case  MID_TB_9 :
               DosBeep( 701,250 ) ;
               break;
          case  MID_TB_10:
               DosBeep( 743,250 ) ;
               break;
              /* -------- Menu Bar items ----------- */
          case  MID_SUB11 :
                WinMessageBox( HWND_DESKTOP,
                               HWND_DESKTOP,
                               "Menu 1, Item 1 Selected",
                               pszMainTitle ,
                               0,
                               MB_ICONASTERISK | MB_OK );
               break;
          case  MID_SUB21 :
                WinMessageBox( HWND_DESKTOP,
                               HWND_DESKTOP,
                               "Menu 2, Item 1 Selected",
                               pszMainTitle ,
                               0,
                               MB_ICONASTERISK | MB_OK );
               break;
          case  MID_SUB22 :
                WinMessageBox( HWND_DESKTOP,
                               HWND_DESKTOP,
                               "Menu 2, Item 2 Selected",
                               pszMainTitle ,
                               0,
                               MB_ICONASTERISK | MB_OK );
               break;
          case  MID_SUB31 :
                WinMessageBox( HWND_DESKTOP,
                               HWND_DESKTOP,
                               "Menu 3, Item 1 Selected",
                               pszMainTitle ,
                               0,
                               MB_ICONASTERISK | MB_OK );
               break;
          case  MID_SUB32 :
                WinMessageBox( HWND_DESKTOP,
                               HWND_DESKTOP,
                               "Menu 3, Item 2 Selected",
                               pszMainTitle ,
                               0,
                               MB_ICONASTERISK | MB_OK );
               break;
          case  MID_SUB33 :
                WinMessageBox( HWND_DESKTOP,
                               HWND_DESKTOP,
                               "Menu 3, Item 3 Selected",
                               pszMainTitle ,
                               0,
                               MB_ICONASTERISK | MB_OK );
               break;

          default:
               break;
          } /* end of switch */
        }
        break;

    default:
      return WinDefWindowProc(hwnd,msg,mp1,mp2);

  } /*  end of switch () */
  return( FALSE );

} /*  end of MainWindowProc */
/* ********************************************************************** */
/*                                                                        */
/*   NewFrameProc                                                         */
/*                                                                        */
/*       This frame proc subclasses the original frame proc. The two      */
/*   messages of interest are WM_QUERYFRAMECTLCOUNT and WM_FORMATFRAME.   */
/*   By catching WM_QUERYFRAMECTLCOUNT, we can return the count of the    */
/*   total number of frame controls, both original and new. This count    */
/*   is used to allocate SWP structures for the array during frame        */
/*   formating. When we process WM_FORMATFRAME, we first call the         */
/*   the original frame proc to setup the SWP array for the original      */
/*   frame controls. Note that among these "original" frame controls is   */
/*   the toolbar. We will add the regular action bar as a second menu     */
/*   below the toolbar. The original processing of WM_FORMATFRAME will    */
/*   return the count of original frame controls (note that because we    */
/*   modified WM_QUERYFRAMECTLCOUNT the SWP array will actually contain   */
/*   an extra SWP for our second menu). We will setup the SWP for our     */
/*   second menu based upon setting for the first menu. We will also have */
/*   to adjust the client window size to make room for the additional     */
/*   menu. Likewise, we will need to check if there is a vertical scroll  */
/*   bar and adjust it. We will us a simple technique of comparing the    */
/*   hwnd field from the SWP array with our global variable hwnds to      */
/*   determine which SWP structures in the array we are interested in.    */
/*                                                                        */
/* ********************************************************************** */

MRESULT EXPENTRY
NewFrameProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
  PFNWP   pfnwpOldFrameProc ;

     /* ------- get original frame procedure  --------- */
  pfnwpOldFrameProc = (PFNWP) WinQueryWindowULong( hwnd, QWL_USER );

  switch (msg) {
    case WM_QUERYFRAMECTLCOUNT :
         {
          USHORT   usItemCount ;

                /* ---- get count of original frame controls --- */
          usItemCount = SHORT1FROMMR( pfnwpOldFrameProc( hwnd,
                                                         msg,
                                                         mp1, mp2 ));
                /* ------- add 1 for new toolbar control  ------ */
          return ( (MRESULT) ++usItemCount );
         }

    case WM_FORMATFRAME :
       {
         PSWP     pSWP ;               // pointer to array of frame ctl SWPs
         USHORT   usItemCount ;        // count of original frame controls
         USHORT   usNewMenuIndex ;     // index of new menu SWP
         USHORT   usToolBarIndex ;     // index of Tool Bar menu SWP
         USHORT   usClientIndex ;      // index of client window SWP
         USHORT   usVertScrollIndex ;  // index of vertical scrollbar SWP
         HWND     hwndVertScroll ;     // hwnd of vertical scrollbar

            /* ------- get a pointer to the SWP array ---------- */
         pSWP = PVOIDFROMMP( mp1 );

           /* ---- run regular processing for original controls --- */
           /*
            *  Note that the original frame proc will setup all
            *  the SWP sturctures for the standard frame window
            *  controls starting at the beginning of the array.
            *  A count of how many SWP structures were initialized
            *  is returned.
            *  All SWP structures for new controls that are to be
            *  added ( in our case, 1 ) will be uninitialized and
            *  at the end of the array. The start of the uninitialied
            *  SWP structure start at an index equal to the returned
            *  count.
            */
         usItemCount = SHORT1FROMMR( pfnwpOldFrameProc( hwnd,
                                                        msg,
                                                        mp1, mp2 ));

            /* ------------- locate SWP for 1st menu  ---------- */
            /*
             *  We will use the settings of the 1st menu to help initialize
             *  the SWP for the second menu. We look for the proper SWP
             *  by scanning the array for the matching hwnd.
             */
         for ( usToolBarIndex = 0;
               usToolBarIndex < usItemCount;
               usToolBarIndex++) {
            if (pSWP[usToolBarIndex].hwnd == hwndToolBar){
               break;
            }
         } // end of for( usToolBarIndex...

            /* ------------- locate SWP for client window  ---------- */
            /*
             *  We will need to adjust the vertical height of the client
             *  window to make room for the second menu. We look for the
             *  proper SWP by scanning the array for a matching hwnd.
             */
         for ( usClientIndex = 0;
               usClientIndex < usItemCount;
               usClientIndex++){
            if (pSWP[usClientIndex].hwnd == hwndClient ){
               break;
            }
         } // end of for ( usClientIndex...

            /* --- locate SWP for vert scroll (if exists) --- */
            /*
             *  First we will check if this window has a vert. scroll bar.
             *  We will need to adjust the vertical height of the scroll
             *  bar to make room for the second menu. We look for the
             *  proper SWP by scanning the array for a matching hwnd.
             */

         if ( ( hwndVertScroll =
                    WinWindowFromID( hwnd, FID_VERTSCROLL)) != NULLHANDLE ){
            for ( usVertScrollIndex = 0;
                  usVertScrollIndex < usItemCount;
                  usVertScrollIndex++){
               if (pSWP[usVertScrollIndex].hwnd == hwndVertScroll ){
                  break;
               }
            } // end of for ( usClientIndex...
         } // end of if (( hwndVertScroll...


          /* ------ the new SWP starts after standard control SWPs ----- */
         usNewMenuIndex = usItemCount ;

          /* ---- get size values for 2nd menu bar  -------- */
         pSWP[ usNewMenuIndex ].fl = SWP_SIZE;
         pSWP[ usNewMenuIndex ].cx =  pSWP[usToolBarIndex].cx ;  // set some
         pSWP[ usNewMenuIndex ].cy =  pSWP[usToolBarIndex].cy ;  // defaults
         pSWP[ usNewMenuIndex ].hwndInsertBehind = HWND_TOP ;
         pSWP[ usNewMenuIndex ].hwnd = hwndMenuBar ;

           /* -- get the menu code to make the actual size adjustments -- */
         WinSendMsg( hwndMenuBar,
                     WM_ADJUSTWINDOWPOS,
                     MPFROMP( pSWP+usNewMenuIndex ),
                     (MPARAM) 0L );

         /* ------ position menu directly below other menu  ------- */
         pSWP[usNewMenuIndex].x = pSWP[usToolBarIndex].x ;
         pSWP[usNewMenuIndex].y = pSWP[usToolBarIndex].y -
                                               pSWP[usNewMenuIndex].cy ;
         pSWP[usNewMenuIndex].fl = pSWP[usToolBarIndex].fl ;

         /* --------  adjust client window size for 2nd menu ------- */
         pSWP[usClientIndex].cy -= pSWP[usNewMenuIndex].cy ;

         /* -------- adjust vertical scroll size for 2nd menu  ----- */
         if ( hwndVertScroll != NULLHANDLE ){
             pSWP[usVertScrollIndex].cy -= pSWP[usNewMenuIndex].cy ;
         }

         /* ---  return total count of controls ( +1 for 2nd menu ) --- */
         return( MRFROMSHORT( ++usItemCount )  );
       }
       break;

    default:
      return( pfnwpOldFrameProc(hwnd,msg,mp1,mp2) );

  } /* end of switch () */

  return( FALSE );

} /* end of NewFrameProc */
/* ********************************************************************** */
/*                                                                        */
/*   ShowErrorWindow                                                      */
/*                                                                        */
/* ********************************************************************** */
VOID
ShowErrorWindow( PSZ  pszErrorMsg, BOOL bUseLastError )
{
  CHAR      acErrorBuffer[256] ;

  if ( bUseLastError ) {
      ERRORID   errorID = WinGetLastError( hab );

      sprintf( acErrorBuffer,
               "%s \n(code = 0x%lX)",
               pszErrorMsg,
               (ULONG) errorID );
      pszErrorMsg = (PSZ) acErrorBuffer ;
  }  /* end of if ( bUseLastError ) */

  WinMessageBox( HWND_DESKTOP,
                 HWND_DESKTOP,
                 pszErrorMsg ,
                 pszErrorTitle ,
                 0,
                 MB_CUACRITICAL | MB_OK );

}


