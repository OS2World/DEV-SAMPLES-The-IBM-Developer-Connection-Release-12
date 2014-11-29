/*---------------------------------------------------------------------------+
+----------------------------------------------------------------------------+
|                                                                            |
| mousedll                                                                   |
|                                                                            |
| Program to demonstrate Playback hook usage                                 |
+-------------------------------------+--------------------------------------+
| Version: 1.0                        |   Marc Fiammante (FIAMMANT at LGEVM2)|
+-------------------------------------+--------------------------------------+
|                                                                            |
+-------------------------------------+--------------------------------------+
| History:                                                                   |
| --------                                                                   |
|                                                                            |
| created: Marc Fiammante September 1990                                     |
+---------------------------------------------------------------------------*/
#include "mousplay.h"
/*************************************************************************/
/* Move   related variables                                              */
/*************************************************************************/
#pragma data_seg(GLOBDATA)
SHORT   sXCoordinate;   /* DLL definition of coordinates          */
SHORT   sYCoordinate;   /* DLL must be DATA SINGLE SHARED in .DEF */
HMQ     Thmq;                 /* Thmq is defined in  DLL          */
#pragma data_seg()
static  BOOL MouseFirstTime=TRUE;      /* The first time in the hook     */
static  BOOL Button1Down=FALSE;   /* Has the button1 Down been simulated */
static  BOOL Button1Up  =FALSE;   /* Has the button1 up   been simulated */
static  QMSG cur_msg;             /* Message to simulate                 */
static  BOOL KeyFirstTime=TRUE;      /* The first time in the hook       */
static  BOOL KeyUp      =FALSE;   /* Has the button1 Down been simulated */
#pragma data_seg(GLOBDATA)
CHAR    PlayString[80]; /* DLL definition of character string     */
#pragma data_seg()
CHAR    PlayChar;                           /* Current character to play */
USHORT  PlayIndex;                /* Index of char in the string         */
static  LONG Time,TimeTillNext,LastTime;   /* Time to wait for key playback */
/*---------------------------------------------------------------------------+
| Mouse Play Hook                                                            |
+---------------------------------------------------------------------------*/
ULONG EXPENTRY MousePlayHookProc(HAB habSpy,BOOL fSkip, PQMSG pQmsg) /* 4 */
{
  HMODULE hSpyDll;                          /* Dll Module handle              */
  /*---------------------------------------------------------------------------+
  | First Time called this turn Initialise information                         |
  +---------------------------------------------------------------------------*/
  if (MouseFirstTime==TRUE) {
         Button1Down=FALSE;
         Button1Up=FALSE;
         cur_msg.msg=WM_MOUSEMOVE;
         cur_msg.mp1=MPFROM2SHORT(sXCoordinate,
                                  sYCoordinate);
         cur_msg.mp2=(MPARAM)HT_NORMAL;
         MouseFirstTime=FALSE;
   }
   if (!fSkip) {                   /* If not going to next msg */
      *pQmsg=cur_msg;   /* <---------  Copy over msg so it is played by PM  */
   } else { /* Read next record */
         if (Button1Up) {
            DosQueryModuleHandle("MOUSEDLL", (PHMODULE)&hSpyDll);
            WinReleaseHook(habSpy, 0, HK_JOURNALPLAYBACK,(PFN)MousePlayHookProc, hSpyDll);
            MouseFirstTime=TRUE;
            WinPostQueueMsg(Thmq, WM_QUIT, 0L, 0L );/* Cause RAFAEL to resume */
         } else {
            if (Button1Down) {
               cur_msg.msg=WM_BUTTON1UP;
               Button1Up=TRUE;
            } else {
               cur_msg.msg=WM_BUTTON1DOWN;
               Button1Down=TRUE;
            } /* endif */
         } /* endif */
   }
   return(0L);
}
/*---------------------------------------------------------------------------+
| Key Play Hook                                                              |
+---------------------------------------------------------------------------*/
#define WAITTIME 100L
ULONG EXPENTRY KeyPlayHookProc(HAB habSpy,BOOL fSkip, PQMSG pQmsg) /* 4 */
{
  HMODULE hSpyDll;                          /* Dll Module handle              */
  /*---------------------------------------------------------------------------+
  | First Time called this turn Initialise information                         |
  +---------------------------------------------------------------------------*/
  if (KeyFirstTime==TRUE) {
         KeyUp=FALSE;                       /* Do not play the keyup yet    */
         PlayIndex=0;
         Time=0L;
         TimeTillNext=0L;
         /*-------------------------------------------------------------*/
         /* get the first character to play and play it with key down   */
         PlayChar=PlayString[PlayIndex];
         cur_msg.msg=WM_CHAR;
         cur_msg.mp1=MPFROMSH2CH(KC_CHAR | KC_LONEKEY , 0x01 , 0x00);
         cur_msg.mp2=MPFROM2SHORT((USHORT)PlayChar,0);
         KeyUp=TRUE;                   /* Play the key up message next      */
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
         PlayChar=PlayString[PlayIndex];
         if (KeyUp) {
            cur_msg.mp1=MPFROMSH2CH(KC_LONEKEY | KC_KEYUP  , 0x01, 0x00);
            cur_msg.mp2=MPFROM2SHORT((USHORT)PlayChar,0);
            KeyUp=FALSE;
            PlayIndex++;
           if (PlayString[PlayIndex]=='\0') {
               DosQueryModuleHandle("MOUSEDLL", (PHMODULE)&hSpyDll);
               WinReleaseHook(habSpy, 0, HK_JOURNALPLAYBACK,(PFN)KeyPlayHookProc, hSpyDll);
               MouseFirstTime=TRUE;
               WinPostQueueMsg(Thmq, WM_QUIT, 0L, 0L );/* Cause RAFAEL to resume */
            }
         } else {
            cur_msg.mp2=MPFROM2SHORT((USHORT)PlayChar,0);
            KeyUp=TRUE;
            cur_msg.mp1=MPFROMSH2CH(KC_CHAR | KC_LONEKEY , 0x01, 0x00);
         } /* endif */
   }
   return(0L);
}
