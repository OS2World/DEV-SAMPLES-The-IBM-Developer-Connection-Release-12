/* table of sizes of parms (number of words) used for each function */
#define INCL_BASE
#include "viospy.h"
#include <stdlib.h>
static CHAR DefaultShell[]="C:\\OS2\\CMD.EXE";
void cdecl main(int argc,char **argv)  {
   int         rc,i;
   RESULTCODES rcbuf;
   CHAR        *CommandShell;
   CHAR        SpiedPgm[255];
   PGINFOSEG   PGInfoSeg;
   PLINFOSEG   PLInfoSeg;
   SEL         GInfoSel,LInfoSel;
   USHORT      Session;
   /*---------------------------------------------------------------- */
   /* Get the current session id                                      */
   DosGetInfoSeg( &GInfoSel,&LInfoSel);
   PGInfoSeg=MAKEP(GInfoSel,0);
   PLInfoSeg=MAKEP(LInfoSel,0);
   Session  =PLInfoSeg->sgCurrent;
   /*---------------------------------------------------------------- */
   if (Session<=16)  {
       rc=VioRegister("VIOSPY","VIOSERVICE",
                       VR_VIOWRTNCHAR | VR_VIOWRTNATTR | VR_VIOWRTNCELL | VR_VIOWRTTTY |
                       VR_VIOWRTCHARSTR | VR_VIOWRTCHARSTRATT | VR_VIOWRTCELLSTR |
                       VR_VIOSCROLLUP | VR_VIOSCROLLDN | VR_VIOSCROLLLF | VR_VIOSCROLLRT |
                       VR_VIOSETCURPOS | VR_VIOSHOWBUF
                       ,
                       0L);
       if (rc!=0) {
          printf("Register Failed rc=%d\n",rc);
          exit(rc);
          return;
       } else DosBeep(400,50);
       CommandShell=getenv("COMSPEC");
       if (!CommandShell) CommandShell=DefaultShell;
       SpiedPgm[0]='\0';
       for(i=1;i<argc;i++) {
          strcat(SpiedPgm+1," ");
          strcat(SpiedPgm+1,argv[i]);
       }
       DosBeep(400,50);
       rc=DosExecPgm((PSZ)NULL,                 /* no buffer           */
                      0,                        /* no buffer length    */
                      0,                        /* synchronous exec    */
                      (char far *)SpiedPgm,     /* pass arguments      */
                      (char far *)NULL,         /* copy parent env     */
                      &rcbuf,                   /* return codes        */
                      CommandShell);            /* pgm name            */
       VioDeRegister();
       printf("Spying ended rc=%d\n",rc);
   } /* End if session is under 16 */
}
