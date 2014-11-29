/************** Dialog2 C Sample Program Source Code File (.C) ****************/
#define INCL_BASE
#define INCL_WIN
#define ON  "On"
#define OFF "Off"
#include <os2.h>
#include "kbdstat.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/***************** Start of main procedure ***************************/
VOID PASCAL THE_SIGNAL_HANDLER(USHORT,USHORT);
BOOL EXPENTRY InputHookProc( HAB habSpy , PQMSG pQmsg , BOOL bRemove);
MRESULT EXPENTRY KbdStatProc( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2 );
ULONG EXPENTRY KeyPlayHookProc(HAB habSpy,BOOL fSkip, PQMSG pQmsg);
HAB hab;
typedef struct _KEYDEF  { USHORT VkValue;USHORT KbdFlag;USHORT OnScan;USHORT OffScan;USHORT VkOffValue;} KEYDEF;
KEYDEF KeyList[]={
   {VK_INSERT,   KBD_INSERT     ,0x68,0x68,VK_INSERT  },
   {VK_NUMLOCK,  KBD_NUMLOCK    ,0x45,0x45,VK_NUMLOCK },
   {VK_SCRLLOCK, KBD_SCROLLLOCK ,0x46,0x46,VK_SCRLLOCK},
   {VK_CAPSLOCK, KBD_CAPSLOCK   ,0x3A,0x2A,VK_SHIFT   }
};
HFILE      hwndkbd;                     /* Keyboard Handle                 */
typedef struct _KEYSHIFT {
                USHORT Shift;
                UCHAR  Nls; } KEYSHIFT;
USHORT   usTemp;
KEYSHIFT Keyshift;
USHORT   ApplType=ID_STATE;
HWND     hwndMenu;
USHORT   Scancode;
USHORT   VkValue;
HMQ      hmq;                        /* Message queue handle         */
static   QMSG cur_msg;             /* Message to simulate                 */
static   BOOL KeyFirstTime=TRUE;      /* The first time in the hook       */
static   BOOL KeyUp      =FALSE;   /* Has the button1 Down been simulated */
static   BOOL KeyEnd     =FALSE;   /* Has the button1 Down been simulated */
static   LONG Time,TimeTillNext,LastTime;   /* Time to wait for key playback */
static   Flags;
/*********************************************************************/
/*                                                                   */
/*  FUNCTION: main                                                   */
/*                                                                   */
VOID cdecl main( VOID )
{
  hab   = WinInitialize( NULL );      /* Initialize PM                */
  hmq   = WinCreateMsgQueue( hab, 0 );/* Create application msg queue */
  DosSetSigHandler(THE_SIGNAL_HANDLER ,
                   NULL,NULL,
                   SIGA_ACCEPT,
                   SIG_KILLPROCESS);
  WinSetHook(hab, HMQ_CURRENT, HK_INPUT,(PFN)InputHookProc,(HMODULE)NULL);
  /* Invoke a modal dialog with the main window frame as owner       */
  WinDlgBox( HWND_DESKTOP,              /* Parent                    */
             HWND_DESKTOP,              /* Owner                     */
             KbdStatProc,                 /* Address of dialog proc    */
             NULL,                      /* Module handle             */
             ID_KBDSTAT,                 /* ID of dialog in resource  */
             NULL );                    /* Initialization data       */
  WinDestroyMsgQueue( hmq );
  WinTerminate( hab );
  DosExit( EXIT_PROCESS, 0 );
}
/*********************************************************************/

MRESULT EXPENTRY KbdStatProc( HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
  HPOINTER Hptr;
  CHAR     Status[256];
  USHORT   iKey;
  switch (msg)
  {
    case WM_INITDLG:
       /*--------------------------------------------------------------------*/
       /* Get the Keyboard Device driver handle                              */
       DosOpen2("KBD$", (PHFILE)&hwndkbd, &usTemp,
                           0L,
//                         FILE_READONLY,
                           FILE_NORMAL,
                           FILE_OPEN,
//                         (ULONG)OPEN_ACCESS_READONLY |
                         (ULONG) OPEN_ACCESS_READWRITE |
                         OPEN_SHARE_DENYREADWRITE |
                         OPEN_FLAGS_FAIL_ON_ERROR,
                         (PEAOP)NULL,
                         0L);
       hwndMenu=WinLoadMenu(hwndDlg,NULL,ID_MENU);
       WinSendMsg(hwndMenu, MM_SETITEMATTR,        /* then check */
               MPFROM2SHORT(ID_STATE, TRUE),
               MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED) );
       WinSendMsg(hwndDlg,WM_UPDATEFRAME,(MPARAM)FCF_MENU,NULL);
       Hptr=WinLoadPointer(HWND_DESKTOP,NULL,ID_KBDSTAT);
       WinSendMsg( hwndDlg ,WM_SETICON,(MPARAM)Hptr,NULL);
       WinSetFocus(HWND_DESKTOP,WinWindowFromID(hwndDlg,ID_EFIELD));
       return ((MRESULT)TRUE);
       break;
    case WM_CHAR:
    case WM_PAINT:
       WinSetKeyboardStateTable(HWND_DESKTOP,Status,FALSE);
       for (iKey=0;iKey<sizeof(KeyList)/sizeof(KEYDEF);iKey++) {
          if (Status[KeyList[iKey].VkValue]&0x01)
              WinSetDlgItemText(hwndDlg,KeyList[iKey].VkValue+ID_VKBASE_STATUS,ON);
          else
              WinSetDlgItemText(hwndDlg,KeyList[iKey].VkValue+ID_VKBASE_STATUS,OFF);
       } /* endfor */
      if ((msg==WM_PAINT)||(msg==WM_CHAR)) return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
      break;
    case WM_COMMAND:
      switch( SHORT1FROMMP( mp1 ) )
      {
        case ID_STATE:
           ApplType=ID_STATE;
           WinSendMsg(hwndMenu, MM_SETITEMATTR,        /* de-check first */
               MPFROM2SHORT(ID_PLAY, TRUE),
               MPFROM2SHORT(MIA_CHECKED, 0) );
          WinSendMsg(hwndMenu, MM_SETITEMATTR,        /* then check */
               MPFROM2SHORT(ID_STATE, TRUE),
               MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED) );
          break;
        case ID_PLAY:
           ApplType=ID_PLAY;
           WinSendMsg(hwndMenu, MM_SETITEMATTR,        /* de-check first */
               MPFROM2SHORT(ID_STATE, TRUE),
               MPFROM2SHORT(MIA_CHECKED, 0) );
           WinSendMsg(hwndMenu, MM_SETITEMATTR,        /* then check */
               MPFROM2SHORT(ID_PLAY, TRUE),
               MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED) );
          break;
        case ID_SCROLLLOCK :    /* pushbutton selected  */
        case ID_CAPSLOCK   :    /* pushbutton selected  */
        case ID_NUMLOCK    :    /* pushbutton selected  */
        case ID_INSERT     :    /* pushbutton selected  */
           WinSetKeyboardStateTable(HWND_DESKTOP,Status,FALSE);
           if (ApplType==ID_STATE) {
              if (Status[SHORT1FROMMP( mp1 )-ID_VKBASE]&0x01) {
                   WinSetDlgItemText(hwndDlg,SHORT1FROMMP( mp1 )-ID_VKBASE+ID_VKBASE_STATUS,OFF);
                   Status[SHORT1FROMMP( mp1 )-ID_VKBASE]&=0xFE;
                   DosDevIOCtl((PCHAR)&Keyshift, 0L, 0x0073, 0x0004, hwndkbd);
                   for (iKey=0;iKey<sizeof(KeyList)/sizeof(KEYDEF);iKey++) {
                     if (KeyList[iKey].VkValue==SHORT1FROMMP( mp1 )-ID_VKBASE) {
                        Keyshift.Shift &=(~KeyList[iKey].KbdFlag);
                        break;
                     }
                   } /* endfor */
                   DosDevIOCtl(0L,(PCHAR)&Keyshift, 0x0053, 0x0004, hwndkbd);
              } else {
                   WinSetDlgItemText(hwndDlg,SHORT1FROMMP( mp1 )-ID_VKBASE+ID_VKBASE_STATUS,ON);
                   Status[SHORT1FROMMP( mp1 )-ID_VKBASE]|=0x01;
                   DosDevIOCtl((PCHAR)&Keyshift, 0L, 0x0073, 0x0004, hwndkbd);
                   for (iKey=0;iKey<sizeof(KeyList)/sizeof(KEYDEF);iKey++) {
                     if (KeyList[iKey].VkValue==SHORT1FROMMP( mp1 )-ID_VKBASE) {
                        Keyshift.Shift |=KeyList[iKey].KbdFlag;
                        break;
                     }
                   } /* endfor */
                   DosDevIOCtl(0L,(PCHAR)&Keyshift, 0x0053, 0x0004, hwndkbd);
              }
              WinSetKeyboardStateTable(HWND_DESKTOP,Status,TRUE);
           } else {
              for (iKey=0;iKey<sizeof(KeyList)/sizeof(KEYDEF);iKey++) {
                if (KeyList[iKey].VkValue==SHORT1FROMMP( mp1 )-ID_VKBASE) {
                   if (Status[SHORT1FROMMP( mp1 )-ID_VKBASE]&0x01) {
                        Scancode=KeyList[iKey].OffScan;
                        VkValue=KeyList[iKey].VkOffValue ;
                        Flags=NULL;
                   } else {
                        Scancode=KeyList[iKey].OnScan;
                        VkValue=KeyList[iKey].VkValue ;
                        Flags=KC_TOGGLE;
                   }
                   WinSetHook(hab, HMQ_CURRENT, HK_JOURNALPLAYBACK,(PFN)KeyPlayHookProc,(HMODULE)NULL);
                   break;
                }
              } /* endfor */
           } /* endif */
          return FALSE;
          break;
        case ID_EXIT:    /* Enter key pressed or pushbutton selected  */
             WinDismissDlg( hwndDlg, TRUE ); /* Finished with dialog box    */
          break;
        default:
          return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
      }
      break;

    default:  /* Pass all other messages to the default dialog proc  */
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
  }
  return FALSE;
}

/*---------------------------------------------------------------------+
| Input Hook procedure                                                 |
+---------------------------------------------------------------------*/
BOOL EXPENTRY InputHookProc( HAB habSpy , PQMSG pQmsg , BOOL bRemove)
{
    if ((pQmsg->msg== WM_CHAR)             &&
        (SHORT1FROMMP(pQmsg->mp1)&KC_CTRL) &&
        ( (SHORT2FROMMP(pQmsg->mp2)==VK_BREAK)||(SHORT1FROMMP(pQmsg->mp2)==(USHORT)'c') ) ) {
         PIDINFO  Process;           /* Process information          */
         DosGetPID(&Process);        /* Who are we                   */
         DosKillProcess(1,Process.pid); /* commit Suicide            */
         DosBeep(400,50);
         return(TRUE);                    /* Do not pass message to next hook
                                               or to the application */
    }
    /**** ------- If it is a character --------****/
    if ((pQmsg->msg== WM_CHAR) &&
        (SHORT1FROMMP(pQmsg->mp1)&KC_CHAR)) {
        /** add 1 to the characterjust for fun *******/
        pQmsg->mp2=(MPARAM) (((ULONG)pQmsg->mp2)+1L);
    }
  return(FALSE);                    /* pass the message to the next hook
                                                or to the application */
}
/******************* End of first dialog procedure *******************/
/*---------------------------------------------------------------------------+
| Key Play Hook                                                              |
+---------------------------------------------------------------------------*/
#define WAITTIME 100L
ULONG EXPENTRY KeyPlayHookProc(HAB habSpy,BOOL fSkip, PQMSG pQmsg) /* 4 */
{
  /*---------------------------------------------------------------------------+
  | First Time called this turn Initialise information                         |
  +---------------------------------------------------------------------------*/
  if (KeyFirstTime==TRUE) {
         KeyUp=FALSE;                       /* Do not play the keyup yet    */
         Time=0L;
         TimeTillNext=0L;
         /*-------------------------------------------------------------*/
         /* get the first character to play and play it with key down   */
         cur_msg.msg=WM_CHAR;
         cur_msg.mp1=MPFROMSH2CH(KC_VIRTUALKEY | KC_SCANCODE | Flags ,
                                    0x01,
                                    Scancode);
         cur_msg.mp2=MPFROM2SHORT(NULL,VkValue);
         cur_msg.msg=WM_MOUSEMOVE;
         cur_msg.mp1=MPFROM2SHORT(100,
                                  100);
         cur_msg.mp2=(MPARAM)HT_NORMAL;
         KeyEnd=FALSE;                   /* Play the key up message next      */
         KeyFirstTime=FALSE;
   }
   if (!fSkip) {                   /* If not going to next msg */
         Time=TimeTillNext-WinGetCurrentTime(habSpy)+LastTime;
         if (Time<0L) Time=0L;
         *pQmsg=cur_msg;   /* <---------  Copy over msg so it is played by PM  */
         return(Time);
   } else { /* Read next record */
         TimeTillNext=WAITTIME;
         LastTime=WinGetCurrentTime(habSpy);
         cur_msg.msg=WM_CHAR;
         if (KeyEnd) {
            WinReleaseHook(habSpy,HMQ_CURRENT, HK_JOURNALPLAYBACK,(PFN)KeyPlayHookProc,(HMODULE)NULL);
            KeyFirstTime=TRUE;
         } else {
             if (KeyUp) {
                cur_msg.mp1=MPFROMSH2CH(KC_VIRTUALKEY |
                                        KC_SCANCODE   |
                                        KC_LONEKEY    |
                                        KC_KEYUP | Flags ,
                                        0x01, Scancode);
                cur_msg.mp2=MPFROM2SHORT(NULL,VkValue);
         cur_msg.msg=WM_MOUSEMOVE;
         cur_msg.mp1=MPFROM2SHORT(120,
                                  120);
         cur_msg.mp2=(MPARAM)HT_NORMAL;
                KeyUp=FALSE;
                KeyEnd=TRUE;
             } else {
                cur_msg.mp2=MPFROM2SHORT(NULL,VkValue);
                cur_msg.mp1=MPFROMSH2CH(KC_VIRTUALKEY | KC_SCANCODE | Flags,
                                        0x01, Scancode);
         cur_msg.msg=WM_MOUSEMOVE;
         cur_msg.mp1=MPFROM2SHORT(110,
                                  110);
         cur_msg.mp2=(MPARAM)HT_NORMAL;
                KeyUp=TRUE;
             } /* endif */
         } /* endif */
   }
   return(0L);
}
/*****************************************************************************/
/*     Signal handling routine                                               */
/*****************************************************************************/
VOID PASCAL THE_SIGNAL_HANDLER(USHORT Flag,USHORT SigNumber) {
    WinReleaseHook(hab, HMQ_CURRENT, HK_INPUT,(PFN)InputHookProc,(HMODULE)NULL);
    WinDestroyMsgQueue( hmq );
    WinTerminate( hab );
    DosBeep(300,50);
    DosBeep(200,50);

#define EXIT_RC 255
    /*****************************************************************************/
    /*     Exit from Process                                                     */
    /*****************************************************************************/
    DosExit(EXIT_PROCESS,EXIT_RC);
}
