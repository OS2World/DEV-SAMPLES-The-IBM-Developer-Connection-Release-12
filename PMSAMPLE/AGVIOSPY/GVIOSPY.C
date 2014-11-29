/*---------------------------------------------------------------------------+
+----------------------------------------------------------------------------+
|                                                                            |
| GVIOSPY    This is the GlobalReg DLL source                                |
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
| created: Marc Fiammante October 1990                                       |
+---------------------------------------------------------------------------*/
#include  "GVIOSPY.h"
/*----------------------------------------------------------*/
typedef   CHAR   SLINE[80];
typedef   SLINE   SCREEN[25];
SCREEN    pascal  Sessions[16];
USHORT    pascal  XCursor[16];    /* Cursor X coordinate               */
USHORT    pascal  YCursor[16];    /*    "   Y     "                    */
HMQ       pascal  AvioHmq=NULL;   /* Spying queue                      */
BOOL      pascal  WroteToSess[16]={FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,
                                   FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};
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
      PGINFOSEG   PGInfoSeg;
      PLINFOSEG   PLInfoSeg;
      USHORT      Session;              /* DEBUG Session number              */
      SEL         GInfoSel,LInfoSel;
      USHORT      Length;
      /*---------------------------------------------------------------- */
      /* Get the current session id                                      */
      DosGetInfoSeg( &GInfoSel,&LInfoSel);
      PGInfoSeg=MAKEP(GInfoSel,0);
      PLInfoSeg=MAKEP(LInfoSel,0);
      Session  =PLInfoSeg->sgCurrent;
      if (Session>16) return;
      WroteToSess[Session]=TRUE;
      /*---------------------------------------------------------------- */
      /* Get the current session screen                                  */
      Length=2000;
      VioReadCharStr((PCH)&(Sessions[Session][0][0]),&Length,0,0,0);
      VioGetCurPos(&XCursor[Session],&YCursor[Session],0);
      /*---------------------------------------------------------------- */
      /* Inform the AGVIOSPY.EXE                                          */
      if (AvioHmq!=NULL) {
          WinPostQueueMsg(AvioHmq,WM_USER+1,(MPARAM)Session,NULL);
      } /* endif */
      return;
}
