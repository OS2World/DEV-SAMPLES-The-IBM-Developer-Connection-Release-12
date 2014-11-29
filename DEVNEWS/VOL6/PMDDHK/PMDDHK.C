
#define  INCL_BASE
#define  INCL_GPIERRORS
#include <os2.h>
#define  INCL_GRE_CLIP
#define  INCL_GRE_DCS
#define  INCL_GRE_STRINGS               /* include GRE strings        */
#include <pmddim.h>                     /* in Graphic engine include  */

#pragma data_seg(GLOBDATA)

PFN MyOld32CharStringPos;

/*--------------------------------------------------------------------*/
/*       Structure definitions                                        */

typedef struct _FLAGS  {
   ULONG version;
   USHORT fEngine32Bit;
   USHORT fDriver32Bit;
} FLAGS;
typedef FLAGS * PFLAGS;

typedef struct _Param2 {
        PLONG   pEngHooks;
        PLONG   pDrvHooks;
}  PARAM2;
typedef PARAM2 *PPARAM2;

extern APIENTRY My32CharStringPos(VOID);
/*--------------------------------------------------------------------*/
/* The OS2_PM_DRV_ENABLE entry point is called at ring 3 whether the  */
/* device driver is 16- or 32-bit.                                    */
/*--------------------------------------------------------------------*/
LONG APIENTRY OS2_PM_DRV_ENABLE (ULONG    Subfunc,
                                 PFLAGS   pParam1,
                                 PPARAM2  pParam2) 
{

    /* Subfunction 0x0C is the Install Monitors subfunction used at boot time to */
    /* install pm monitor routines.                                              */

    if (Subfunc==0x0C) {

       /* Check for 16bit or 32bit Display Driver */
       if (pParam1->fDriver32Bit == 0) {

         /* if we have a 16bit driver then return error */
         return(PMERR_DEV_FUNC_NOT_INSTALLED);
       } else {

         /* otherwise setup 32bit addresses in the dispatch table */
         MyOld32CharStringPos = (PFN)pParam2->pDrvHooks[NGreCharStringPos & 0xFF];
         pParam2->pDrvHooks[NGreCharStringPos & 0xFF] = (LONG)&My32CharStringPos;
       }
       return 0;
    }
}

