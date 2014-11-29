/* table of sizes of parms (number of words) used for each function */
#define INCL_BASE
#include "GFULLSPY.h"
void cdecl main(int argc,char **argv)  {
   int         rc;
   rc=VioGlobalReg("GFULLSPY","VIOSERVICE",
                   VR_VIOWRTNCHAR | VR_VIOWRTNATTR | VR_VIOWRTNCELL | VR_VIOWRTTTY |
                   VR_VIOWRTCHARSTR | VR_VIOWRTCHARSTRATT | VR_VIOWRTCELLSTR |
                   VR_VIOSCROLLUP | VR_VIOSCROLLDN | VR_VIOSCROLLLF | VR_VIOSCROLLRT |
                   VR_VIOSETCURPOS | VR_VIOSHOWBUF
                   ,
                   0L,0L);
   if (rc!=0) {
      printf("Register Failed rc=%d\n",rc);
      exit(rc);
      return;
   } else DosBeep(400,50);
}
