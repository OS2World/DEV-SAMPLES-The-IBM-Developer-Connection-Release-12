/*********************************************************************/
/*                                                                   */
/*  File Name:          READPM.C                                     */
/*                                                                   */
/*  Description:        Provides sample exit routine to READ         */
/*                      a PM field in a Rexx Variable                */
/*                                                                   */
/*  Entry Points:       GetObjectText - Get Given object current     */
/*                      text.                                        */
/*                                                                   */
/*  Notes:              This Rexx function changes the clipboard     */
/*                      content it might change thus testcases       */
/*                      relying on the clipboard content may be      */
/*                      affected                                     */
/*                                                                   */
/*  Return Codes:        0 == Function completed without error       */
/*                      -1 == Illegal call to function               */
/*                                                                   */
/*  Usage Notes:        result will contain the requested value      */
/*                      Use this code as a sample to write your      */
/*                      own user exits                               */
/*********************************************************************/

#define INCL_DOS
#define INCL_BASE
#define INCL_WIN
#define INCL_AVIO                        /* the PM header file           */
#define INCL_GRE_DCS
#include <os2.h>                       /* OS/2    header information */
#define INCL_RXSHV                     /* include shared variable    */
#define INCL_RXMACRO                   /* include macrospace info    */
#include <rexxsaa.h>                   /* REXXSAA header information */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pmddi.h"
#define  ID_SPYWIN 0x4321
TID       ThreadId=NULL;
#define   STACKSIZE   0x1000
CHAR      FAR *StackPtr;
SEL       SelStack;
static    void   SpyThread(void);
ULONG     MemSem;
HSEM      Sem=&MemSem;
ULONG     MemSemHK;
HSEM      SemHK=&MemSemHK;
ULONG     Length;
HWND      Field;
ERRORID ErrorId;

/*tobuf (static) internal function */
static VOID tobuf(b,s,l)               /* copy RXSTRING to buffer    */
PUCHAR b;                              /* address of destination buf */
RXSTRING s;                            /* string to be copied        */
USHORT l;                              /* length of destination buf  */
  {                                    /*                            */
  memset(b,'\0',l);                    /* clear destination buffer   */
  if (s.strlength < l) {               /* if length < buffer size    */
    memcpy(                            /* copy string to buffer      */
        b,                             /*                            */
        s.strptr,                      /*                            */
        s.strlength);                  /*                            */
    }                                  /*                            */
  return;                              /*                            */
  }                                    /*                            */



HAB hab;
HWND        hSpyWindow;
PSZ         Buffer;
PSZ         Buffer1;
SEL         CBP_Sel;
#define     PARMLENGTH      80
#define     LENGTH_CLASS    20
static CHAR Class[PARMLENGTH];
static CHAR Index[PARMLENGTH];
/* GetObjectText */
SHORT APIENTRY GetObjectText(PSZ func,
                             USHORT ac,
                             PRXSTRING av,
                             PSZ que,
                             PRXSTRING sel)
  {
    SHORT    rc = -1;                      /* return code from function  */
    SHVBLOCK RexxVar;
    CHAR     VarName[20];
    /*-------------------------------------------------------------*/
    /*- Create a thread at first call to be the less intrusive ----*/
    /*- possible and to allow function to work with PMREXX too ----*/
    /*-------------------------------------------------------------*/
    if (ThreadId==NULL) {
       DosSemSet(Sem);
       DosAllocSeg(STACKSIZE,&SelStack,1);
       StackPtr=MAKEP(SelStack,STACKSIZE-2);
       rc=DosCreateThread(SpyThread,&ThreadId,StackPtr);
       if (rc!=0) {
            DosBeep(200,50);
            if (rc!=8)  {
                DosBeep(250,50);
                if (rc!=89) {
                  DosBeep(300,50);
                  if (rc!=212) {
                    DosBeep(350,50);
                  }
                }
            }
       } /* endif */
       DosSemWait(Sem,10000L); /* Wait for thread to be ready */
    } /* endif */
    /*-------------------------------------------------------------*/
    Buffer=NULL;
    DosSemSet(Sem);
    /*-------------------------------------------------------------*/
    /*- Ask thread for Data ---------------------------------------*/
    WinPostMsg(hSpyWindow,WM_USER+1,NULL,NULL);
    DosSetPrty(PRTYS_THREAD,PRTYC_TIMECRITICAL,PRTYD_MAXIMUM,ThreadId);
    /*-------------------------------------------------------------*/
    /*- Wait for answer       -------------------------------------*/
    DosSemWait(Sem,10000L);
    rc=1;
    if (Buffer!=NULL) {
       strcpy(VarName,"TEXT");
       RexxVar.shvnext           =(struct shvnode far *)NULL;
       RexxVar.shvname.strptr    =(PCH)VarName;
       RexxVar.shvname.strlength =strlen((PSZ)VarName);
       RexxVar.shvnamelen        =RexxVar.shvname.strlength;
       RexxVar.shvvalue.strlength=(USHORT)Length;
       RexxVar.shvvalue.strptr   =Buffer;
       RexxVar.shvvaluelen       =RexxVar.shvvalue.strlength;
       RexxVar.shvcode=RXSHV_SET;
       RxVar((PSHVBLOCK)&RexxVar);
       if (CBP_Sel!=NULL) DosFreeSeg(CBP_Sel);/* Release all memory     */
       DosFreeSeg(SELECTOROF(Buffer)); /*                               */
       /* Give rc                                                       */
       strcpy(sel->strptr,"Texte Found");/*                            */
       sel->strlength=strlen(sel->strptr);/*                            */
       rc = 0;                            /* no errors in call          */
    }
    if (Buffer1!=NULL) {
       strcpy(VarName,"VIOTEXT");
       RexxVar.shvnext           =(struct shvnode far *)NULL;
       RexxVar.shvname.strptr    =(PCH)VarName;
       RexxVar.shvname.strlength =strlen((PSZ)VarName);
       RexxVar.shvnamelen        =RexxVar.shvname.strlength;
       RexxVar.shvvalue.strlength=(USHORT)strlen(Buffer1);
       RexxVar.shvvalue.strptr   =Buffer1;
       RexxVar.shvvaluelen       =RexxVar.shvvalue.strlength;
       RexxVar.shvcode=RXSHV_SET;
       RxVar((PSHVBLOCK)&RexxVar);
       DosFreeSeg(SELECTOROF(Buffer1)); /*                               */
       /* Give rc                                                       */
       strcpy(sel->strptr,"Texte Found");/*                            */
       sel->strlength=strlen(sel->strptr);/*                            */
       rc = 0;                            /* no errors in call          */
    }
    /* Give rc                                                       */
    if (rc!=0) {
       ltoa((ULONG)ErrorId,sel->strptr,16);
       strcat(sel->strptr,"  No Text found");/*                         */
       sel->strlength=strlen(sel->strptr);/*                            */
       rc = 0;                            /* no data in clipboard       */
    }
    DosSemSet(Sem);
    WinPostMsg(hSpyWindow,WM_QUIT,NULL,NULL);
    DosSemWait(Sem,10000L);
    return rc;                         /*                            */
  }                                    /*                            */
/*-------------------------------------------------------------*/
/*- Create a thread at first call to be the less intrusive ----*/
/*- possible and to allow function to work with PMREXX too ----*/
#define   WC_LEN     20
#define   CLASSLEN   80
HATOMTBL  AtomTable ;
CHAR      WCFrame     [WC_LEN];
CHAR      WCButton    [WC_LEN];
CHAR      WCListBox   [WC_LEN];
CHAR      WCEntryField[WC_LEN];
CHAR      WCStatic    [WC_LEN];
CHAR      WCComboBox  [WC_LEN];
CHAR      WCScrollBar [WC_LEN];
CHAR      WCMle       [WC_LEN];
#define   WM_ATOMFIRST 0xC000
USHORT    pascal      QUERYTEXT=0;
HMODULE   hDll;                           /* Spy module handle            */

BOOL      EXPENTRY InputHookProc(HAB habSpy, PQMSG pQmsg, BOOL bRemove);
MRESULT   EXPENTRY SpyWindowProc(HWND  hwnd,USHORT msg,MPARAM mp1,MPARAM mp2 );

static void SpyThread(void) {
   HMQ  hmq;                             /* Message queue handle         */
   QMSG qmsg;
   hab = WinInitialize( NULL );          /* Initialize PM                */
   if (hab==NULL) { return; } /* endif */
   AtomTable=WinQuerySystemAtomTable();
   /*-------------------------------------------------------------*/
   /*- Get System Name for PM Classes                         ----*/
   WinQueryAtomName(AtomTable,(ATOM)WC_FRAME     ,WCFrame     ,WC_LEN);
   WinQueryAtomName(AtomTable,(ATOM)WC_BUTTON    ,WCButton    ,WC_LEN);
   WinQueryAtomName(AtomTable,(ATOM)WC_LISTBOX   ,WCListBox   ,WC_LEN);
   WinQueryAtomName(AtomTable,(ATOM)WC_ENTRYFIELD,WCEntryField,WC_LEN);
   WinQueryAtomName(AtomTable,(ATOM)WC_STATIC    ,WCStatic    ,WC_LEN);
   WinQueryAtomName(AtomTable,(ATOM)WC_COMBOBOX  ,WCComboBox  ,WC_LEN);
   WinQueryAtomName(AtomTable,(ATOM)WC_SCROLLBAR ,WCScrollBar ,WC_LEN);
   WinQueryAtomName(AtomTable,(ATOM)WC_MLE       ,WCMle       ,WC_LEN);
   ErrorId=WinGetLastError(hab);
   /* -------------------------------------------------------------------*/
   /* Add a user message to get vio content -----------------------------*/
    DosGetModHandle( "GETTEXT", (PHMODULE)&hDll);
   if (QUERYTEXT==0) {
     QUERYTEXT=WinAddAtom(AtomTable,"WITT_QUERY_TEXT");
     if (QUERYTEXT<WM_ATOMFIRST) {
        DosBeep(1400,20);
        DosBeep(1200,20);
        DosBeep(1000,20);
        DosBeep(1200,20);
        DosBeep(1400,20);
     } else {
       WinRegisterUserMsg(hab,QUERYTEXT,
                              DTYP_LONG,RUM_IN,
                              DTYP_LONG,RUM_IN,
                              DTYP_LONG);
       WinSetHook(hab, NULL, HK_INPUT  , (PFN)InputHookProc, hDll);
     } /* endif */
   } /* endif */
   /*-------------------------------------------------------------*/
   /*- Normal PM Thread                                       ----*/
   hmq = WinCreateMsgQueue( hab,1000 );    /* Create a message queue       */
   WinRegisterClass(                     /* Register window class        */
     hab,                                /* Anchor block handle          */
     "SpyWindow",                        /* Window class name            */
      SpyWindowProc,                     /* Address of window procedure  */
      CS_SIZEREDRAW,                     /* Class style                  */
      0                                  /* No extra window words        */
      );
    hSpyWindow=WinCreateWindow(        /* Create an object window     */
                    HWND_OBJECT,          /* Parent window               */
                    "SpyWindow",               /* Class of window             */
                    "SpyWindow",               /* Name of window              */
                     0L ,                  /* No particular style         */
                     0, 0, 0, 0,           /* Size and position           */
                     HWND_OBJECT,         /* Window owner                */
                     HWND_BOTTOM,          /* Where she is in the screen  */
                     ID_SPYWIN,           /* Identifier                  */
                     NULL,                 /* No particular datas and no  */
                     NULL);                /* parameters                  */
   DosSemClear(Sem);
   if (hSpyWindow==NULL) {
      DosBeep(400,100);
      ErrorId=WinGetLastError(hab);
      return;
   } /* endif */
   while( WinGetMsg( hab, &qmsg, NULL, 0, 0 ) )
      WinDispatchMsg( hab, &qmsg );
   WinReleaseHook(hab , NULL, HK_INPUT  , (PFN)InputHookProc, hDll);
   WinDestroyWindow( hSpyWindow);
   DosBeep(1300,50);
   DosBeep(1400,50);
   DosBeep(1200,50);
   DosBeep(1100,50);
   DosBeep(1000,50);
   DosBeep( 900,50);
   DosBeep( 800,50);
   DosSemClear(Sem);
   ThreadId=NULL;
   WinDestroyMsgQueue( hmq );            /* and                          */
   WinTerminate( hab );                  /* terminate the application    */
}
MRESULT EXPENTRY SpyWindowProc(HWND  hwnd,USHORT msg,MPARAM mp1,MPARAM mp2 )
{
   USHORT rc;
   CHAR Warn[80];
   switch( msg )
   {
     case WM_USER+1:
       {
          USHORT usCB_FmtInfo,rc;
          POINTL PointerPos;
          CHAR ClassName[CLASSLEN];
          /*------------------------------------------------------------*/
          /* Now find the spy window with the good class and good PID   */
          WinQueryPointerPos( HWND_DESKTOP, &PointerPos);
          Field=WinWindowFromPoint(HWND_DESKTOP,&PointerPos,TRUE,FALSE);
          WinQueryClassName(Field,CLASSLEN,ClassName);
          /*------------------------------------------------------------*/
          /* If is it a help simulate system menu command to send data  */
          /* to clipboard                                               */
          if (strcmp(ClassName,"CLASS_PAGE")==0) {
             HWND TheDesktop,Parent;
             Parent=Field;
             TheDesktop=WinQueryDesktopWindow(hab,NULL);
             /* Loop up to the desktop to get main window */
             for (; ; ) {
                Parent=WinQueryWindow(Parent,QW_PARENT,FALSE);
                if ((Parent==NULL)||(Parent==TheDesktop)||(Parent==HWND_DESKTOP)) {
                   break; /* Found the desktop */
                } /* endif */
                Field=Parent;
             } /* endfor */
             /* Send data to the clipboard                */
             WinSendMsg(Field,WM_COMMAND,(MPARAM)0x7F11,MPFROM2SHORT(CMDSRC_MENU,TRUE));
             /* Now get the clipboard Data                */
             WinOpenClipbrd(hab);
             if (WinQueryClipbrdFmtInfo (hab, CF_TEXT, &usCB_FmtInfo)) {
                  /*********************************************************/
                  /* if there is, then get the selector to the text from   */
                  /* the CB. You can not use this selector directly - copy */
                  /* the text into a local array. You will have to use a   */
                  /* far pointer version of copy, depending on the model   */
                  /* being used you may have to write your own.            */
                  /*********************************************************/
                 CBP_Sel = (SEL)WinQueryClipbrdData (hab, CF_TEXT);
                 if (CBP_Sel==NULL) {
                     WinCloseClipbrd(hab);
                     DosSemClear(Sem);
                     break;
                 } /* endif */
                  /*********************************************************/
                  /* get control of the segment pointed to be selector.    */
                  /*********************************************************/
                 rc=DosGetSeg (CBP_Sel);
                 if (rc==NO_ERROR) rc=DosSizeSeg(CBP_Sel,&Length);
                 ErrorId=(ERRORID)rc;
                 if (rc==NO_ERROR) {
                    SEL NewSel;
                    PSZ CBuffer;
                    CBuffer=MAKEP(CBP_Sel,0);
                    DosAllocSeg((USHORT)Length,&NewSel,1);
                    Buffer=MAKEP(NewSel,0);
                    memcpy(Buffer,CBuffer,(USHORT)Length);
                    WinCloseClipbrd(hab);
                 }
             }
          } else if (strcmp(ClassName,WCListBox)==0) {
             USHORT Count,Item;
             Length=0;
             Count=(SHORT)WinSendMsg(Field, LM_QUERYITEMCOUNT, NULL, NULL);
             for (Item=0;Item<Count;Item++) {
                  SHORT CurLen;
                  CurLen=(SHORT)
                     WinSendMsg(Field,LM_QUERYITEMTEXTLENGTH,
                                      MPFROMSHORT(Item),NULL);
                  if (CurLen!=LIT_ERROR) {
                     Length+=CurLen+1;  /* Give space for \0 */
                  } /* endif */
             }
             if (Length>0) {
                    PID Pid;
                    TID Tid;
                    SEL GivedSel;
                    PSZ GivedBuf;
                    PSZ LocalBuf;
                    USHORT CurLoc;
                    SEL NewSel;
                    DosAllocSeg((USHORT)Length,&NewSel,SEG_GIVEABLE | SEG_GETTABLE);
                    WinQueryWindowProcess(Field,&Pid,&Tid); /* Who's that girl        */
                    DosGiveSeg(NewSel,Pid,&GivedSel);
                    Buffer=MAKEP(NewSel,0);
                    LocalBuf=Buffer;
                    for (Item=0;Item<Count;Item++) {
                       CurLoc=LocalBuf-Buffer; /* where are we in the Buffer */
                       GivedBuf=MAKEP(GivedSel,CurLoc);
                       WinSendMsg(Field,LM_QUERYITEMTEXT,
                                        MPFROM2SHORT(Item,Length-CurLoc),
                                        GivedBuf);
                       /* GivedBuf and LocalBuf point to the same memory  */
                       /* area but one is valid to the current process    */
                       /* the other to the target process                 */
                       LocalBuf+=strlen(LocalBuf);
                       LocalBuf++; /* skip the \0 */
                    }
             } /* endif */
          } else {
             /*------------------------------------------------------------*/
             /* Read all what is readable through WinQueryWindowText       */
              SEL NewSel;
              Length= (ULONG)WinQueryWindowTextLength(Field);
              if (Length>0) {
                    PID Pid;
                    TID Tid;
                    SEL GivedSel;
                    PSZ GivedBuf;
                    CHAR LocBuf[80];
                    Length++;
                    DosAllocSeg((USHORT)Length,&NewSel,SEG_GIVEABLE | SEG_GETTABLE);
                    WinQueryWindowProcess(Field,&Pid,&Tid); /* Who's that girl        */
                    DosGiveSeg(NewSel,Pid,&GivedSel);
                    Buffer=MAKEP(NewSel,0);
                    GivedBuf=MAKEP(GivedSel,0);
                    WinQueryWindowText(Field,(USHORT)Length,GivedBuf);
                    Buffer[(USHORT)Length-1]='\0'; /* Ensures Null termination */
              } /* endif */
          }
          DosSemSet(SemHK);
          Buffer1=NULL;
          rc=WinPostMsg(Field,QUERYTEXT,(MPARAM)hwnd,(MPARAM)Field);
          rc=WinMsgSemWait(SemHK,500L);
          if (rc!=0) {
            itoa((ULONG)rc,Warn,16);
            WinMessageBox (
              HWND_DESKTOP,             /* Parent window              */
              HWND_DESKTOP,
              (PSZ)Warn,
              (PSZ)"SemWait",
              0, MB_OK );
              Buffer1=NULL;
              DosSemClear(SemHK);
          } /* endif */
          DosSemClear(Sem);
       }
       break;
     case WM_USER+2:
       Buffer1=(PSZ)mp1;
       DosSemClear(SemHK);
       break;
     default:
/**********************************************************************/
/*     Everything else comes here.  This call MUST exist              */
/*     in your window procedure.                                      */
/**********************************************************************/
     return WinDefWindowProc(
              hwnd,
              msg,
              mp1,
              mp2 );
  }
  return FALSE;
}
/*---------------------------------------------------------------------------+
| Input Hook procedure                                                       |
+---------------------------------------------------------------------------*/
BOOL EXPENTRY InputHookProc(HAB habSpy, PQMSG pQmsg, BOOL bRemove)
{
   HMODULE hSpyDll;
   if (QUERYTEXT==0) {
     QUERYTEXT=WinFindAtom(WinQuerySystemAtomTable(),"WITT_QUERY_TEXT");
   }
   if ((pQmsg->msg>0) &&
       (pQmsg->msg==QUERYTEXT)) {
     static HDC     PtedDC;
     static SHORT   hvio;                         /* For the hvio handle         */
     static SHORT   Lgth;                         /* For the hvio handle         */
     static PID     Pid;
     static TID     Tid;
     static SEL     NewSel;
     static SEL     GivedSel;
     static PSZ     GivedBuf;
     static PSZ     LocalBuf;
     int rc;
     static struct {
               LONG Cols;
               LONG Rows;
              } RowCol;
     PtedDC  = WinQueryWindowDC((HWND)pQmsg->mp2);
     hvio    = (SHORT)GreGetHandle ( PtedDC, 1L );
     if (hvio!=NULL) {
        DevQueryCaps( PtedDC ,(LONG)CAPS_WIDTH_IN_CHARS ,1L,(PLONG)&RowCol.Cols);
        DevQueryCaps( PtedDC ,(LONG)CAPS_HEIGHT_IN_CHARS,1L,(PLONG)&RowCol.Rows);
        WinQueryWindowProcess((HWND)pQmsg->mp1,&Pid,&Tid); /* Who's that girl        */
        Lgth    = (RowCol.Rows*RowCol.Cols);
        if (Lgth==0) {
           Lgth=80;
           DosBeep(1400,50);
           DosBeep(1200,50);
           DosBeep(1400,50);
        }
        if (Lgth<200) Lgth=200;
        DosAllocSeg(Lgth+1,&NewSel,SEG_GIVEABLE | SEG_GETTABLE);
        DosGiveSeg(NewSel,Pid,&GivedSel);
        LocalBuf=MAKEP(NewSel,0);
        GivedBuf=MAKEP(GivedSel,0);
        rc=VioReadCharStr(LocalBuf, &Lgth, 0, 0, (HVIO)hvio );
        *(LocalBuf+Lgth)='\0';
        if (Lgth==0) {
           itoa(rc,LocalBuf,10);
           strcat(LocalBuf," <-- rc read , ");
           itoa(hvio,LocalBuf+strlen(LocalBuf),10);
           strcat(LocalBuf," <--  hvio , ");
           ltoa(RowCol.Rows,LocalBuf+strlen(LocalBuf),10);
           strcat(LocalBuf," <--  Rows , ");
           ltoa(RowCol.Cols,LocalBuf+strlen(LocalBuf),10);
           strcat(LocalBuf," <--  Cols , ");
           DosBeep(1200,50);
           DosBeep(1500,50);
           DosBeep(1200,50);
        }
        WinSendMsg( (HWND)pQmsg->mp1,WM_USER+2,(MPARAM)GivedBuf,NULL);
        DosFreeSeg(NewSel); /*                               */
     } else {
        WinSendMsg( (HWND)pQmsg->mp1,WM_USER+2,NULL,NULL); /* No Vio text */
     }
   }
   return(FALSE); /* pass the message to the next hook or to the application */
}

