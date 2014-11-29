/*************** whatis C Sample Program Source Code File (.C) ****************/
/*                                                                            */
/* PROGRAM NAME: whatis  DLL part                                             */
/* -------------                                                              */
/*  Presentation Manager Standard Window C Sample Program                     */
/*                                                                            */
/* WHAT THIS PROGRAM DOES:                                                    */
/* -----------------------                                                    */
/* Displays all information about the object under the mouse                  */
/* And the menu item text if any selected (button1 held down)                 */
/*                                                                            */
#define INCL_DOS                        /* Selectively include          */
#define INCL_BASE                       /* Selectively include          */
#define INCL_WIN                        /* the PM header file           */
#define INCL_VIO                         /* the PM header file           */
#define INCL_AVIO                        /* the PM header file           */
#define INCL_GRE_DCS
#define INCL_GRE_JOURNALING
#define INCL_GRE_DEVICE

#include <os2.h>                        /* PM header file               */
#include <stdio.h>                      /* C/2 I/O    functions         */
#include <string.h>                     /* C/2 string functions         */
#include <stdlib.h>                     /* C/2 conver functions         */
#include "whatis.h"                     /* Resource symbolic identifiers*/
#include "pmddi.h"
#pragma data_seg(GLOBDATA)
CREATESTRUCT    CreateStruct;
CHAR     SharedText [80];
CHAR     SharedText1[80];
HWND     TargetWindow=(HWND)0;
CHAR     AvioText[80];
USHORT   xCur;
USHORT   yCur;
CHAR     Directory[256];
ULONG    SegSize;
PVOID    UserPtr;
USHORT   QUERYTEXT=0;
LONG     Handle0;
LONG     Handle1;
LONG     Handle2;
LONG     Handle3;
LHANDLE   Journal1=0;
BOOL      Success1=FALSE;
BOOL      Success2=FALSE;
ERRORID   Err1=0L;
ERRORID   Err2=0L;
ERRORID   Err3=0L;
#pragma data_seg()
static HDC     PointedDC;
static SHORT   Nhvio;                         /* For the hvio handle         */
static USHORT  Length;                        /* For the hvio handle         */
static POINTL  PointerPos;
static HWND    Pointed;
static HWND    LastPointed;
static HWND    Subclassed=0;
static PFNWP   OldProc =0;

#define WM_ATOMFIRST 0xC000
APIRET16 APIENTRY16   DOS16SIZESEG(USHORT,PULONG);
MRESULT EXPENTRY TextWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 ) ;

/*---------------------------------------------------------------------------+
| Input Hook procedure                                                       |
+---------------------------------------------------------------------------*/
BOOL EXPENTRY InputHookProc(HAB habSpy, PQMSG pQmsg, BOOL bRemove)
{
  static  HMODULE hSpyDll;
  ULONG  HHeap;
  ULONG  Drive;
  ULONG  Map;
  ULONG  PathLen;
  static const CHAR DriveLetter[]="?ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  if (TargetWindow!=(HWND)0) {
     if (pQmsg->msg<WM_ATOMFIRST) {
        switch (pQmsg->msg) {
          case WM_BUTTON2DOWN:
           WinSendMsg( TargetWindow,WM_USER+pQmsg->msg,pQmsg->mp1,pQmsg->mp2 );
           if (Subclassed==0) {
               WinQueryPointerPos( HWND_DESKTOP, &PointerPos);
               Subclassed=Pointed   = WinWindowFromPoint( HWND_DESKTOP, &PointerPos, TRUE);
               DosBeep(200,50);
               OldProc=WinSubclassWindow(Subclassed,TextWindowProc);
               if (OldProc==0) DosBeep(1200,50);
                WinInvalidateRect( Subclassed, 0, TRUE );

           } /* endif */
           break;
           case WM_MOUSEMOVE:
               memset(Directory,'\0',sizeof(Directory));
               DosQueryCurrentDisk(&Drive,&Map);
               PathLen=200;
               DosQueryCurrentDir(Drive,Directory+3,&PathLen);
               if (Drive>26) Drive=0;
               Directory[0]        = DriveLetter[Drive];
               Directory[1]        =':';
               Directory[2]        ='\\';
               Directory[PathLen+3]='\0';
               WinQueryPointerPos( HWND_DESKTOP, &PointerPos);
               Pointed   = WinWindowFromPoint( HWND_DESKTOP, &PointerPos, TRUE);
               PointedDC = WinQueryWindowDC( Pointed);
               /* GreGetHandle is documented in I/O Subsystem and device drivers */
               /* Volume 2                                                       */
               Handle0   =        GreGetHandle ( PointedDC, 0L );
               Handle1   =        GreGetHandle ( PointedDC, 1L );
               Handle2   =        GreGetHandle ( PointedDC, 2L );
               Handle3   =        GreGetHandle ( PointedDC, 3L );
               Nhvio     = (SHORT)GreGetHandle ( PointedDC, 1L );
               Length    =79;
               memset(AvioText,'\0',80);
               if (Nhvio!=0) {
                    VioReadCharStr(AvioText,&Length, 0, 0, (HVIO)Nhvio );
                    VioGetCurPos( &xCur , &yCur , (HVIO)Nhvio );
               }
               else {
                  strcpy(AvioText,"Handle 0 ");
                  itoa(Nhvio,AvioText+strlen(AvioText),10);
                  strcat(AvioText,"  ,");
                  ltoa((LONG)PointedDC,AvioText+strlen(AvioText),16);
                  strcat(AvioText," <-DC  ,");
                  ltoa((LONG)Pointed,AvioText+strlen(AvioText),16);
                  strcat(AvioText," <-HWND ");
               }
               if (LastPointed!=Pointed) {
                  HHeap=WinQueryWindowULong(Pointed,QWL_HHEAP);
                  UserPtr= (PVOID) WinQueryWindowULong (Pointed,QWL_USER);
                  if (UserPtr!=0) {
                    DOS16SIZESEG(SELECTOROF(UserPtr),&SegSize);
                  } else {
                    SegSize=0;
                  } /* endif */
               } /* endif */
               WinSendMsg( TargetWindow,WM_USER+pQmsg->msg,pQmsg->mp1,pQmsg->mp2 );
               LastPointed=Pointed;

              break;
          default:
              break;
        } /* endswitch */
     } else {
        if (pQmsg->msg==QUERYTEXT) {
           POINTL  PtrPos;
           HWND    Pted;
           HDC     PtedDC;
           SHORT   hvio;                         /* For the hvio handle         */
           USHORT  Lgth;                         /* For the hvio handle         */


           WinQueryPointerPos( HWND_DESKTOP, &PtrPos);
           Pted   = WinWindowFromPoint( HWND_DESKTOP, &PtrPos, TRUE);
           PtedDC = WinQueryWindowDC( Pted);
           hvio     = (SHORT)GreGetHandle ( PtedDC, 1L );
           Lgth    =79;
           memset(SharedText1,'\0',80);
           if (hvio!=0) VioReadCharStr(SharedText1, &Lgth, 0, 0, (HVIO)hvio );
           else strcpy(AvioText,"Handle 0");
           WinSendMsg( TargetWindow,WM_USER+WM_USER+1,0,0);

         }
     } /* endif */
  } else {
     DosQueryModuleHandle( "WHATISD", (PHMODULE)&hSpyDll);
     WinReleaseHook (habSpy, 0, HK_INPUT  , (PFN)InputHookProc, hSpyDll);
     WinBroadcastMsg(HWND_DESKTOP,WM_NULL,(MPARAM)0,(MPARAM)0,BMSG_POST);
  }
  return(FALSE); /* pass the message to the next hook or to the application */
}

/*---------------------------------------------------------------------------+
| SendMsg Hook procedure                                                     |
+---------------------------------------------------------------------------*/

VOID EXPENTRY SendMsgHookProc(HAB habSpy, PSMHSTRUCT pSmh, BOOL bTask)
{
  static  HMODULE hSpyDll;
  if (TargetWindow!=(HWND)0) {
     switch (pSmh->msg) {
     case WM_FOCUSCHANGE:
     case WM_MENUSELECT:
        WinQueryPointerPos( HWND_DESKTOP, &PointerPos);
        Pointed   = WinWindowFromPoint( HWND_DESKTOP, &PointerPos, TRUE);
        PointedDC = WinQueryWindowDC( Pointed);
        Nhvio     = (SHORT)GreGetHandle ( PointedDC, 1L );
        Length    =79;
        memset(AvioText,'\0',80);
        if (Nhvio!=0) VioReadCharStr(AvioText, &Length, 0, 0, (HVIO)Nhvio );
        else strcpy(AvioText,"Handle 0");
        WinSendMsg( TargetWindow,WM_USER+pSmh->msg,pSmh->mp1,pSmh->mp2);
        break;
     case WM_CREATE:
        CreateStruct=*((PCREATESTRUCT)pSmh->mp2);



     default:
        break;
     } /* endswitch */
  } else {
     DosQueryModuleHandle( "PMTSTHDL", (PHMODULE)&hSpyDll);
     WinReleaseHook (habSpy, 0, HK_SENDMSG, (PFN)SendMsgHookProc, hSpyDll);
     WinBroadcastMsg(HWND_DESKTOP,WM_NULL,(MPARAM)0,(MPARAM)0,BMSG_POST);
  }
}
MRESULT EXPENTRY TextWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 ) {
  HPS hps,hps1;
  RECTL rc;
  HDC lHdc;
  HDC pHdc;
  LHANDLE Journal2;
  switch( msg )
  {
   case WM_PAINT:
      if (Subclassed!=0) {                     /* Obtain a cached micro PS     */
        HWND Subcl;
        HAB hab;
        ERRORID Err;
        Subcl=Subclassed;
        Subclassed=0;
        DosDelete("D:\\JOURNAL1.OUT");
        hab=WinQueryAnchorBlock(Subcl);
        Err=WinGetLastError(hab);
        Journal1 =(LHANDLE)GreCreateJournalFile("D:\\JOURNAL1.OUT",JNL_PERM_FILE,0L);
        Err1=WinGetLastError(hab);
        hps = WinBeginPaint( hwnd, 0, &rc );
        lHdc=GpiQueryDevice(hps);
        Err2=WinGetLastError(hab);
        Success1=GreStartJournalFile(lHdc,Journal1);
        Err2=WinGetLastError(hab);
        WinEndPaint( hps );                      /* Drawing is complete   */
        hps1 = WinBeginPaint( hwnd, 0, &rc );
        if (hps1!=hps) {
           DosBeep(400,10);
           DosBeep(450,10);
           DosBeep(500,10);
           Success2=GreStopJournalFile(lHdc,Journal1);
           lHdc=GpiQueryDevice(hps1);
           DosDelete("D:\\JOURNAL1.OUT");
           Journal1 =(LHANDLE)GreCreateJournalFile("D:\\JOURNAL1.OUT",JNL_PERM_FILE,0L);
           Err1=WinGetLastError(hab);
           Success1=GreStartJournalFile(lHdc,Journal1);
           Err2=WinGetLastError(hab);
        } /* endif */
        WinEndPaint( hps1 );                      /* Drawing is complete   */
        (*OldProc)(hwnd,msg,mp1,mp2);
        Err3=WinGetLastError(hab);
        Success2=GreStopJournalFile(lHdc,Journal1);
        Err3=WinGetLastError(hab);
//      GreDeleteJournalFile(Journal1);
//      pHdc = WinQueryWindowDC(Subcl);
//      if (pHdc!=0) {
//         DosBeep(1800,20);
//           DosDelete("D:\\JOURNAL2.OUT",0L);
//         Journal2 =(LHANDLE)GreCreateJournalFile("D:\\JOURNAL2.OUT",JNL_PERM_FILE,0L);
//         GreStartJournalFile(pHdc,Journal2);
//         (*OldProc)(hwnd,msg,mp1,mp2);
//         GreStopJournalFile(pHdc,Journal2);
//         GreDeleteJournalFile(Journal2);
//      }
        WinSubclassWindow(Subcl,OldProc);
        OldProc=0;
        WinInvalidateRect( hwnd, 0, TRUE );
      }
     break;
   default :
     (*OldProc)(hwnd,msg,mp1,mp2);
     break;
  }
}
