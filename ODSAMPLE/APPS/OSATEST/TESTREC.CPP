
#define INCL_WINSTDFILE  /* Window Standard File Functions       */
#define INCL_WINBUTTONS
#define INCL_WINDIALOGS
#define INCL_WINMLE
#define INCL_WINERRORS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINSTDFILE
#define INCL_WINSTDFONT
#define INCL_WINWINDOWMGR
#define INCL_WINENTRYFIELDS
#define INCL_WINLISTBOXES
#define INCL_WINSWITCHLIST
#define INCL_WINSYS
#define INCL_ODAPI
#define INCL_ODOSA
#define INCL_OSAAPI
#define INCL_OSA
#include <os2.h>

/* c language includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <process.h>
#include <memory.h>
#include <sys\types.h>
#include <sys\stat.h>

/* application includes */
#include "osatest.h"
#include "osadlg.h"
#include "pmassert.h"


typedef struct _RECORDER_PARM {
  PMAIN_PARM pmp;
  HWND       hwnd;
  HWND       hwndStatus;
  CHAR       szTitle[CCHMAXPATH];
  BOOL       recording;
} RECORDER_PARM;
typedef RECORDER_PARM *PRECORDER_PARM;

void InitRecorder(PRECORDER_PARM prec);
void StartRecorder(PRECORDER_PARM prec);
void StopRecorder(PRECORDER_PARM prec);
void AddRecordedEvents(PRECORDER_PARM prec, OSAEvent *ptheEvent);

void InitRecorder(PRECORDER_PARM prec)
{
   SWP             swp;
   CHAR            szTemp[CCHMAXPATH];
   ULONG           dummy = errAEBadParm;

   if(prec->pmp->Profile.testrec_cx != 0)
   {
     WinSetWindowPos(prec->hwnd,
                     HWND_TOP,
                     prec->pmp->Profile.testrec_x,
                     prec->pmp->Profile.testrec_y,
                     prec->pmp->Profile.testrec_cx,
                     prec->pmp->Profile.testrec_cy,
                     SWP_MOVE | SWP_SIZE);
     WinSetWindowPos(prec->hwndStatus,
                     HWND_TOP,
                     prec->pmp->Profile.testrecStat_x,
                     prec->pmp->Profile.testrecStat_y,
                     prec->pmp->Profile.testrecStat_cx,
                     prec->pmp->Profile.testrecStat_cy,
                     SWP_MOVE | SWP_SIZE);
   }
   else
   {
    WinQueryTaskSizePos(WinQueryAnchorBlock(prec->hwnd),0,&swp);
    WinSetWindowPos(prec->hwnd,
                    HWND_TOP,
                    swp.x,
                    swp.y,
                    swp.cx,
                    swp.cy,
                    SWP_MOVE );
   }
   WinSetWindowULong(prec->hwnd, QWL_USER, (ULONG) prec );

   prec->pmp->hwndRecordEvent = prec->hwnd;

   prec->hwndStatus = WinWindowFromID(prec->hwnd, IDD_ODRECSTATUS);
   WinSetWindowText(prec->hwndStatus, "idle");
   prec->recording = FALSE;
   WinShowWindow(prec->hwnd,TRUE);
}

MRESULT EXPENTRY OSARecordEventsDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   Environment    *ev;
   PRECORDER_PARM     prec;
   HWND            hwndButton;
   CHAR            szTemp[CCHMAXPATH];
   PSZ             pszBuffer,pszTemp;
   SWP             swp;
   PSWP            pswpNew, pswpOld;
   MRESULT         result;
   OSAError        retVal = noErr;

   switch(msg)
   {
      case WM_INITDLG:
         prec = (PRECORDER_PARM) calloc(1,sizeof(RECORDER_PARM));
         prec->pmp =  (PMAIN_PARM) mp2;
         prec->hwnd = hwnd;
         prec->pmp->hwndRecordEvent = hwnd;
         InitRecorder(prec);
         break;

      case WM_ADD_RECORDEDEVENT:
         prec = (PRECORDER_PARM) WinQueryWindowULong( hwnd, QWL_USER );
         AddRecordedEvents(prec, (OSAEvent *) mp1);
         break;

      case WM_WINDOWPOSCHANGED:
         pswpNew = (PSWP) mp1;
         pswpOld = pswpNew + 1;
         hwndButton = WinWindowFromID(hwnd, IDD_ODRECSTATUS);
         WinQueryWindowPos(hwndButton, &swp);
         WinSetWindowPos(hwndButton,
                         HWND_TOP,
                         swp.x,
                         swp.y+(pswpNew->cy - pswpOld->cy),
                         0,
                         0,
                         SWP_MOVE );
         hwndButton = WinWindowFromID(hwnd, IDD_ODDISPLAYEVENTS);
         WinQueryWindowPos(hwndButton, &swp);
         WinSetWindowPos(hwndButton,
                         HWND_TOP,
                         0,
                         0,
                         swp.cx+(pswpNew->cx - pswpOld->cx),
                         swp.cy+(pswpNew->cy - pswpOld->cy),
                         SWP_SIZE );
         result = WinDefDlgProc(hwnd, msg, mp1, mp2);
         WinInvalidateRect(hwnd, NULL, TRUE);
         return result;
         break;

      case WM_PAINT: {
         SWP Pos, DlgPos;
         HPS Hps;
         RECTL Rect;

         /* First let dialog do normal painting */
         WinDefDlgProc(hwnd, msg, mp1, mp2);

         /* Paint recessed frame around the status line */
         Hps = WinGetPS(hwnd);
         WinQueryWindowPos(WinWindowFromID(hwnd, IDD_ODRECSTATUS), &Pos);
         WinQueryWindowPos(hwnd, &DlgPos);
         Rect.xLeft = WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME);
         Rect.xRight= DlgPos.cx - Rect.xLeft;
         Rect.yBottom = Pos.y - 1;
         Rect.yTop    = Pos.y + Pos.cy + 1;
         #define DB_RAISED    0x0400  // Undocumented borders
         #define DB_DEPRESSED 0x0800
         WinDrawBorder(Hps, &Rect, 1, 1, 0, 0, DB_DEPRESSED);
         WinReleasePS(Hps);
         return 0;
         }

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1)){
            case IDD_ODRECSTART:
               prec = (PRECORDER_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               StartRecorder(prec);
               WinSetWindowText(prec->hwndStatus, "recording...");
               break;
            case IDD_ODRECSTOP:
               prec = (PRECORDER_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               StopRecorder(prec);
               strcpy(szTemp,prec->szTitle);
               WinSetWindowText(prec->hwndStatus, "idl...");
               break;
            case IDD_ODRECCANCEL:
               prec = (PRECORDER_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               if(prec->recording)
                 StopRecorder(prec);
               // Save window size/pos info
               WinQueryWindowPos(hwnd, &swp);
               prec->pmp->Profile.testrec_cx = swp.cx;
               prec->pmp->Profile.testrec_cy = swp.cy;
               prec->pmp->Profile.testrec_x = swp.x;
               prec->pmp->Profile.testrec_y = swp.y;
               WinQueryWindowPos(prec->hwndStatus, &swp);
               prec->pmp->Profile.testrecStat_cx = swp.cx;
               prec->pmp->Profile.testrecStat_cy = swp.cy;
               prec->pmp->Profile.testrecStat_x = swp.x;
               prec->pmp->Profile.testrecStat_y = swp.y;
               prec->pmp->hwndRecordEvent = NULL;
               free((PVOID)prec);
               WinDismissDlg(hwnd, TRUE);
               break;
            default:
               ;
         } /* endswitch */
         break;

      case WM_CLOSE:
         WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDD_ODRECCANCEL), NULL);
         break;
      default:
         return(WinDefDlgProc(hwnd, msg, mp1, mp2));
   }
   return (MRESULT)NULL;
}   /*  end of OSAScriptEditorDlgProc()  */


void StartRecorder(PRECORDER_PARM prec)
{

   OSAEvent theOSAEvent;        /* the apple event you are generating       */
   AEAddressDesc targetaddr;    /* the address descriptor for target app */
   PID           thePID;

   OSErr              rc = noErr;

   if (!prec->recording)
   {
     rc = AEInstallEventHandler( kCoreEventClass,
                                 kAENotifyRecording,
                                 (AEEventHandlerUPP)RawEventRecordingProc,
                                 0L,
                                 FALSE);
     pmassert(prec->pmp->hab, rc == noErr );
     prec->recording = TRUE;

     rc = AEGetPID(APP_NAME, &thePID);
     rc = AECreateDesc(typePID, &thePID, sizeof(thePID), &targetaddr);
     rc = AECreateOSAEvent(kCoreEventClass,
                           kAEStartRecording,
                           &targetaddr,
                           kAutoGenerateReturnID,
                           kAnyTransactionID,
                           &theOSAEvent);
     rc = AESend(&theOSAEvent,
                 NULL,
                 kAENoReply,
                 kAENormalPriority,
                 kAEDefaultTimeout,
                 NULL,
                 NULL);
   }
}


void StopRecorder(PRECORDER_PARM prec)
{
   OSAEvent theOSAEvent;        /* the apple event you are generating       */
   AEAddressDesc targetaddr;    /* the address descriptor for target app */
   PID           thePID;
   OSErr              rc = noErr;

   if (prec->recording)
   {
     rc = AERemoveEventHandler( kCoreEventClass,
                                kAENotifyRecording,
                                (AEEventHandlerUPP)RawEventRecordingProc,
                                FALSE );
     pmassert(prec->pmp->hab, rc == noErr );
     prec->recording = FALSE;
     rc = AEGetPID(APP_NAME, &thePID);
     rc = AECreateDesc(typePID, &thePID, sizeof(thePID), &targetaddr);
     rc = AECreateOSAEvent(kCoreEventClass,
                           kAEStopRecording,
                           &targetaddr,
                           kAutoGenerateReturnID,
                           kAnyTransactionID,
                           &theOSAEvent);
     rc = AESend(&theOSAEvent,
                 NULL,
                 kAENoReply,
                 kAENormalPriority,
                 kAEDefaultTimeout,
                 NULL,
                 NULL);
   }
}

void AddRecordedEvents(PRECORDER_PARM prec, OSAEvent *ptheEvent)
{

   HWND  hwndMle = WinWindowFromID(prec->hwnd, IDD_ODDISPLAYEVENTS);
   PCHAR  pszMleBuf = NULL;
   IPT   lOffset = 0;
   ULONG cbCopied;
   ULONG ulAction;
   ULONG cbLength;
   ULONG bufSize = 1024;
   ULONG dataSize = 0;
   AEDesc result;
   PID   pid;
   Size  size;
   CHAR  szAppName[CCHMAXPATH];
   ULONG cbAppName = CCHMAXPATH;
   ULONG eventClass;
   ULONG eventID;
   PSZ  pszTemp;
   DescType dtype;
   OSErr err;

   pszMleBuf = (PCHAR) calloc(1,bufSize);

   /* Zero-fill the buffer using memset, a C run-time function */
   memset(pszMleBuf, 0, bufSize);

   /* Set the MLE import-export buffer */
   WinSendMsg(hwndMle,
              MLM_SETIMPORTEXPORT,
              MPFROMP(pszMleBuf),
              MPFROMSHORT((USHORT) bufSize));

   /* Find out how much text is in the MLE */
   cbLength = (ULONG) WinSendMsg(hwndMle,
                                MLM_QUERYFORMATTEXTLENGTH,
                                MPFROMLONG(lOffset),
                                MPFROMLONG((-1)));

   lOffset = cbLength + 1; /* insert text at end */

   /* format event into buffer */
   //Get the applications PID
   err = AEGetAttributeDesc(ptheEvent,keyAddressAttr,typeWildCard,&result);
   size = sizeof(PID);
   err = AEGetDescData(&result, &dtype, (Ptr) &pid, (Size) sizeof(PID), &size);
   AEDisposeDesc(&result);
   err = AEGetAppName(pid, szAppName, &cbAppName);

   //Get Event class
   err = AEGetAttributeDesc(ptheEvent,keyEventClassAttr,typeWildCard,&result);
   size = sizeof(ULONG);
   err = AEGetDescData(&result, &dtype, (Ptr) &eventClass, (Size) sizeof(ULONG), &size);
   AEDisposeDesc(&result);
   //Get Event id
   err = AEGetAttributeDesc(ptheEvent,keyEventIDAttr,typeWildCard,&result);
   size = sizeof(ULONG);
   err = AEGetDescData(&result, &dtype, (Ptr) &eventID, (Size) sizeof(ULONG), &size);
   AEDisposeDesc(&result);

   strcpy(pszMleBuf,"******************************\n");
   strcat(pszMleBuf,"\n");
   strcat(pszMleBuf, "Application: ");
   strcat(pszMleBuf,szAppName);
   strcat(pszMleBuf,"\n");
   strcat(pszMleBuf,"Event Class: ");
   pszTemp = pszMleBuf+strlen(pszMleBuf);
   memcpy(pszTemp,&eventClass,sizeof(ULONG));
   pszTemp+=sizeof(ULONG);
   pszTemp = NULL;
   strcat(pszMleBuf,"\n");
   strcat(pszMleBuf,"Event ID: ");
   pszTemp = pszMleBuf+strlen(pszMleBuf);
   memcpy(pszTemp,&eventID,sizeof(ULONG));
   pszTemp+=sizeof(ULONG);
   pszTemp = NULL;
   strcat(pszMleBuf,"\n");
   strcat(pszMleBuf,"******************************\n");
   dataSize = strlen(pszMleBuf);
   cbCopied = (ULONG) WinSendMsg(hwndMle,
                                 MLM_IMPORT,
                                 MPFROMP( &lOffset),
                                 MPFROMLONG(dataSize));
   free((PVOID) pszMleBuf);
}

