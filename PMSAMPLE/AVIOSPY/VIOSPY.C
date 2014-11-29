#include  "viospy.h"
typedef   CHAR   SLINE[80];
typedef   SLINE   SCREEN[25];
SCREEN    pascal Sessions[16];
static    BOOL InRegister[16]={FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,
                               FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};
USHORT    pascal  Session;              /* DEBUG Session number              */
USHORT    pascal  XCursor;              /* Cursor X coordinate               */
USHORT    pascal  YCursor;              /*    "   Y     "                    */
HMQ       pascal  AvioHmq=NULL; /* Spying queue                      */

/*----------------------------------------------------------*/
/*- Typedefs for the different parms -----------------------*/
typedef struct  _SVIOSHOWBUF { HVIO    hVio;
                         USHORT  Length;
                         USHORT  Offset;} SVIOSHOWBUF;
typedef   SVIOSHOWBUF  * PSVIOSHOWBUF;
typedef struct  _SVIOSETCURPOS { HVIO    hVio;
                           USHORT  Ycur;
                           USHORT  Xcur;} SVIOSETCURPOS;
typedef   SVIOSETCURPOS  * PSVIOSETCURPOS;
typedef struct  _SVIOWRTNCHAR  { HVIO    hVio;
                           USHORT  Colonne;
                           USHORT  Ligne;
                           USHORT  Nombre;
                           PCH     PChParm;} SVIOWRTNCHAR;
typedef   SVIOWRTNCHAR  * PSVIOWRTNCHAR;
typedef struct  _SVIOWRTTTY    { HVIO    hVio;
                           USHORT  Nombre;
                           PCH     PChParm;} SVIOWRTTTY;
typedef   SVIOWRTTTY    * PSVIOWRTTTY;
typedef struct  _SVIOWRTCHARSTRATT  { HVIO    hVio;
                                PCH     PAttr;
                                USHORT  Colonne;
                                USHORT  Ligne;
                                USHORT  Nombre;
                                 PCH     PChParm;} SVIOWRTCHARSTRATT;
typedef   SVIOWRTCHARSTRATT    * PSVIOWRTCHARSTRATT;
typedef struct  _SVIOWRTCHARSTR     { HVIO    hVio;
                                USHORT  Colonne;
                                USHORT  Ligne;
                                USHORT  Nombre;
                                PCH     PChParm;} SVIOWRTCHARSTR;
typedef   SVIOWRTCHARSTR       * PSVIOWRTCHARSTR;
typedef struct  _SVIOSCROLL         { HVIO    hVio;
                                PCH     PChParm;
                                USHORT  Nombre;
                                USHORT  Right;
                                USHORT  Bottom;
                                USHORT  Left;
                                USHORT  Top; } SVIOSCROLL;
typedef   SVIOSCROLL           * PSVIOSCROLL;
/*----------------------------------------------------------*/
int far _loadds _saveregs VIOSERVICE(unsigned temp2,/* note only enough here to get to index */
                      unsigned temp1,
                      USHORT   index)  {
      PGINFOSEG   PGInfoSeg;
      PLINFOSEG   PLInfoSeg;
      SEL         GInfoSel,LInfoSel;
      PUSHORT     Parms;
      USHORT      Length;
      /*---------------------------------------------------------------- */
      /* Get the current session id                                      */
      DosGetInfoSeg( &GInfoSel,&LInfoSel);
      PGInfoSeg=MAKEP(GInfoSel,0);
      PLInfoSeg=MAKEP(LInfoSel,0);
      Session  =PLInfoSeg->sgCurrent;
      if (Session>16) return -1;
      if (InRegister[Session]) return -1;
      /*---------------------------------------------------------------- */
      Parms       =   &index+3;
      /*---------------------------------------------------------------- */
      InRegister[Session]=TRUE;
      switch (index) {
    case VIOSHOWBUF:
        VioShowBuf(((PSVIOSHOWBUF)Parms)->Offset,((PSVIOSHOWBUF)Parms)->Length,0);
      break;
    case VIOSETCURPOS:
        VioSetCurPos(((PSVIOSETCURPOS)Parms)->Xcur,((PSVIOSETCURPOS)Parms)->Ycur,0);
      break;
      /* ------------------------------------------------- */
      /* Buffer1 , USParm1 , USParm2 , USParm3 , VioHandle */
   case VIOWRTNCHAR:
        VioWrtNChar(((PSVIOWRTNCHAR)Parms)->PChParm,
                     ((PSVIOWRTNCHAR)Parms)->Nombre,
                     ((PSVIOWRTNCHAR)Parms)->Ligne,
                     ((PSVIOWRTNCHAR)Parms)->Colonne,0);
      break;
   case VIOWRTNCELL:
        VioWrtNCell(((PSVIOWRTNCHAR)Parms)->PChParm,
                     ((PSVIOWRTNCHAR)Parms)->Nombre,
                     ((PSVIOWRTNCHAR)Parms)->Ligne,
                     ((PSVIOWRTNCHAR)Parms)->Colonne,0);
      break;
      /* ------------------------------------------------- */
      /* Buffer1 , USParm1 , USParm2 , USParm3 , VioHandle */
    case VIOWRTTTY:
        VioWrtTTY(((PSVIOWRTTTY)Parms)->PChParm,((PSVIOWRTTTY)Parms)->Nombre,0);
        break;
     case VIOWRTCHARSTRATT:
        VioWrtCharStrAtt( ((PSVIOWRTCHARSTRATT)Parms)->PChParm,
                          ((PSVIOWRTCHARSTRATT)Parms)->Nombre,
                          ((PSVIOWRTCHARSTRATT)Parms)->Ligne,
                          ((PSVIOWRTCHARSTRATT)Parms)->Colonne,
                          ((PSVIOWRTCHARSTRATT)Parms)->PAttr,
                          0);
        break;
     case VIOWRTCHARSTR:
        VioWrtCharStr( ((PSVIOWRTCHARSTR)Parms)->PChParm,
                       ((PSVIOWRTCHARSTR)Parms)->Nombre,
                       ((PSVIOWRTCHARSTR)Parms)->Ligne,
                       ((PSVIOWRTCHARSTR)Parms)->Colonne,
                       0);
        break;
   case VIOWRTCELLSTR:
        VioWrtCellStr( ((PSVIOWRTCHARSTR)Parms)->PChParm,
                       ((PSVIOWRTCHARSTR)Parms)->Nombre,
                       ((PSVIOWRTCHARSTR)Parms)->Ligne,
                       ((PSVIOWRTCHARSTR)Parms)->Colonne,
                       0);
        break;
   case VIOSCROLLUP:
        VioScrollUp( ((PSVIOSCROLL)Parms)->Top,
                     ((PSVIOSCROLL)Parms)->Left,
                     ((PSVIOSCROLL)Parms)->Bottom,
                     ((PSVIOSCROLL)Parms)->Right,
                     ((PSVIOSCROLL)Parms)->Nombre,
                     ((PSVIOSCROLL)Parms)->PChParm,
                     0);
        break;
     case VIOSCROLLDN:
        VioScrollDn( ((PSVIOSCROLL)Parms)->Top,
                     ((PSVIOSCROLL)Parms)->Left,
                     ((PSVIOSCROLL)Parms)->Bottom,
                     ((PSVIOSCROLL)Parms)->Right,
                     ((PSVIOSCROLL)Parms)->Nombre,
                     ((PSVIOSCROLL)Parms)->PChParm,
                     0);
        break;
     case VIOSCROLLLF:
        VioScrollLf( ((PSVIOSCROLL)Parms)->Top,
                     ((PSVIOSCROLL)Parms)->Left,
                     ((PSVIOSCROLL)Parms)->Bottom,
                     ((PSVIOSCROLL)Parms)->Right,
                     ((PSVIOSCROLL)Parms)->Nombre,
                     ((PSVIOSCROLL)Parms)->PChParm,
                     0);
        break;
     case VIOSCROLLRT:
        VioScrollRt( ((PSVIOSCROLL)Parms)->Top,
                     ((PSVIOSCROLL)Parms)->Left,
                     ((PSVIOSCROLL)Parms)->Bottom,
                     ((PSVIOSCROLL)Parms)->Right,
                     ((PSVIOSCROLL)Parms)->Nombre,
                     ((PSVIOSCROLL)Parms)->PChParm,
                     0);
        break;
   default:
        break;
      } /* endswitch */
      Length=2000;
      VioReadCharStr((PCH)&(Sessions[Session][0][0]),&Length,0,0,0);
      VioGetCurPos(&XCursor,&YCursor,0);
      if (AvioHmq!=NULL) {
          WinPostQueueMsg(AvioHmq,WM_USER+1,NULL,NULL);
      } /* endif */
      InRegister[Session]=FALSE;
      return  0;
}
