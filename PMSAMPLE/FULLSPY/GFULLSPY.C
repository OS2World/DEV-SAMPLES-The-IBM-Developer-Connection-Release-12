/*---------------------------------------------------------------------------+
+----------------------------------------------------------------------------+
|                                                                            |
| GFULLSPY    This is the GlobalReg DLL source                                |
|                                                                            |
| Program to demonstrate VioGlobalReg usage                                  |
+-------------------------------------+--------------------------------------+
| Version: 1.0                        |   Marc Fiammante (FIAMMANT at LGEVM2)|
+-------------------------------------+--------------------------------------+
|                                                                            |
+-------------------------------------+--------------------------------------+
| History:                                                                   |
| --------                                                                   |
|                                                                            |
| created: Marc Fiammante July 1991                                          |
+---------------------------------------------------------------------------*/
#include  "GFULLSPY.h"
/*----------------------------------------------------------*/
BOOL      ThreadStarted=FALSE;
#define   STACKSIZE   1000
CHAR      FAR *StackPtr;
SEL       SelStack;
USHORT    ThreadID;
void      SpyThread(void);
/*----------------------------------------------------------*/
/*- Ax Must be returned as on entry so this function does---*/
/*- Nothing but returns Ax to us it is declared as void  ---*/
/*- and used as returning an int                          --*/
void near AxDxFunction(void);
void near _loadds ReadScreen(void);
/*----------------------------------------------------------*/
/*- This function is not loadds and not /Aw because Ax needs*/
/*- to be preserved on entry                                */
int far _saveregs VIOSERVICE(unsigned temp2,/* note only enough here to get to index */
                      unsigned temp1,
                      USHORT   index)  {
      ULONG         AxDxOnEntry;
      long int  (near *PAxDxFunction) (void);
      PAxDxFunction=(void near *)AxDxFunction;
      /* Get the AX value on Entry this is why this function is not _loadds*/
      AxDxOnEntry=(*PAxDxFunction)();
      /* Get the screen content in a shared zone this function is _loadds*/
      ReadScreen();
      /* ------------ Returns with the AX on entry  ---------------------*/
      return (USHORT) AxDxOnEntry;
}
      /* ------------ This function does nothing but gets the AX and DX -*/
      /* ------------ because it will be used as returning LONG    ------*/
void near AxDxFunction(void) {return;}
void near _loadds ReadScreen(void) {
      /*---------------------------------------------------------------- */
      DosEnterCritSec();
      if (ThreadStarted) {
          DosExitCritSec();
      } else {
          USHORT rc;
          ThreadStarted=TRUE;
          DosExitCritSec();
          DosAllocSeg(STACKSIZE,&SelStack,1);
          StackPtr=MAKEP(SelStack,STACKSIZE-2);
          rc=DosCreateThread(SpyThread,&ThreadID,StackPtr);
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
      }
      return;
}
typedef struct {
                 PID   pid;
                 CHAR  Directory[256];
               } DIRINFO;

void SpyThread(void) {
    USHORT rc;
    HSYSSEM   Sem1,Sem2;
    DIRINFO * DirInfo;
    SEL       DirSel;
    PIDINFO   PidInfo;
    USHORT    Drive;
    ULONG     Map;
    USHORT    PathLen;
    static const CHAR DriveLetter[]="?ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    DosBeep(1400,50);
    DosBeep(1500,50);
    DosBeep(1550,50);
    DosBeep(1450,50);
    DosBeep(1350,50);
    VioWrtCharStr("Thread Started   ",15,24,1,0);
    /*** Create first semaphore to wait on requests */
    rc=DosCreateSem(1,&Sem1,"\\SEM\\FULLSPY.SEM1");
    if (rc==ERROR_ALREADY_EXISTS) {
       rc=DosOpenSem(&Sem1,"\\SEM\\FULLSPY.SEM1");
    } else {
       if (rc==NO_ERROR) {
          DosSemSet((HSEM)Sem1);
       } else {
          DosExit(EXIT_THREAD,0);
          return;
       } /* endif */
    }
    /* endif */
    /*** Create second semaphore to tell request complete */
    rc=DosCreateSem(1,&Sem2,"\\SEM\\FULLSPY.SEM2");
    if (rc==ERROR_ALREADY_EXISTS) {
       rc=DosOpenSem(&Sem2,"\\SEM\\FULLSPY.SEM2");
    } /* endif */
    /*** Create Shared memory for request data            */
    rc=DosAllocShrSeg(sizeof(DIRINFO),"\\SHAREMEM\\FULLSPY.DAT",&DirSel);
    if (rc==ERROR_ALREADY_EXISTS) {
       rc=DosGetShrSeg("\\SHAREMEM\\FULLSPY.DAT",&DirSel);
    }
    DirInfo=MAKEP(DirSel,0);
    DosGetPID(&PidInfo);
    /*********** Wait indefinitely for Requests  ****************/
    for (; ; ) {
        VioWrtCharStr("Waiting            ",15,24,1,0);
        DosSemWait((HSEM)Sem1,SEM_INDEFINITE_WAIT);
        if (DirInfo->pid==PidInfo.pid) {
            VioWrtCharStr("Good PID           ",15,24,1,0);
            DosBeep(1000,20);
            DosBeep(900,20);
            DosBeep(1000,20);
            DosSemSet((HSEM)Sem1);
            memset(DirInfo->Directory,'\0',sizeof(DirInfo->Directory));
            DosQCurDisk(&Drive,&Map);
            PathLen=200;
            DosQCurDir(Drive,(DirInfo->Directory)+3,&PathLen);
            if (Drive>26) Drive=0;
            DirInfo->Directory[0]        = DriveLetter[Drive];
            DirInfo->Directory[1]        =':';
            DirInfo->Directory[2]        ='\\';
            DirInfo->Directory[PathLen+3]='\0';
            DosSemClear((HSEM)Sem2);
        } else {
            VioWrtCharStr("Bad  PID           ",15,24,1,0);
            DosSemWait((HSEM)Sem2,SEM_INDEFINITE_WAIT);
        } /* endif */
    } /* endfor */

    DosBeep(200,20);
    DosBeep(300,20);
    DosBeep(200,100);
    VioWrtCharStr("Thread Ended       ",15,24,1,0);
}
