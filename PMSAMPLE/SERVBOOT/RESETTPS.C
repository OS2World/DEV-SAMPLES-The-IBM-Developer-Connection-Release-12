/*  IDSS_Cmp_Opts: -As -Zpe -G2s                                     */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*  First lines must be C compiler options.                          */
/*-------------------------------------------------------------------*/
/*  OPtions -W3 and -DLINT_ARGS are forced by COS2 IDSS process.     */
/*                                                                   */
/*                                                                   */
/*  Warning: If you supply -DLINT_ARGS option or any -Dxxx options   */
/*           twice, the IBM-C2 compiler will loop and  your  COS2    */
/*           IDSS process will abort on time out.                    */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*           G set printer to condensed mode, Double printing      */
/*                                                                   */
#pragma linesize(132)
#pragma pagesize(63)
#pragma title("GOS/2 RESETTPS Version 2.00")
#pragma subtitle("Header")
#pragma page(1)
/*****************************************************************************
*       Author:        P.Guillon - D/0768 CER IBM La Gaude                   *
*                                                                            *
*       Environment:   PCAT, PS/2  OS/2  1.1 or higher                       *
*                                                                            *
*       OS2 Syntax:    RESETTPS                                              *
*                                                                            *
*       This is the companion program of WATCHDOG. It reset  WATCHDOG        *
*       timeout count to an initial value. It's usually invoked by VM        *
*       to make sure that the HOST to PS connection is isn't lost.           *
*                                                                            *
*       The REBOOT function is assumed by the companion bimodal  Cha-        *
*       racter Device Driver SERVBOOT.SYS.                                   *
*                                                                            *
*       The design of this little program was done with in  mind  the        *
*       IDSS PS/2 La Gaude OS/2 servers. It's an OS/2  adaptation  of        *
*       DOS RESETTPS.                                                        *
*----------------------------------------------------------------------------*
*      What you need to Compile this Program:                                *
*                                                                            *
*           Required Files:                                                  *
*                                                                            *
*                RESETTPS.C    - Source code for this C Program.             *
*                WATCHDOG.H    - Watchdog miscellaneous definitions          *
*                RESETTPS.DEF  - Make window compatible Program.             *
*                RESETTPS.MAK  - Make file                                   *
*                                                                            *
*           Required Libraries:                                              *
*                                                                            *
*                SLIBCE.LIB   - This is the Protect mode/standard combined   *
*                               small model run-time library.                *
*                DOSCALLS.LIB - DLL API Import Library.                      *
*                                                                            *
*           Required Programs: IBM C/2 Compiler (Version 1.1 or later)       *
*                              IBM Linker/2 (Version 1.1 or later)           *
*                                                                            *
*****************************************************************************/

#pragma subtitle("DLL Link References - History")
#pragma page(1)
/*****************************************************************************
*                                                                            *
*           Dynamic Link References:                                         *
*                                                                            *
*                DosGetShrSeg   - Obtain access to a "named" shared-memory   *
*                DosFreeSeg     - Deallocate a memory block                  *
*                                                                            *
*                DosOpenSem     - Opens a "named" system semaphore           *
*                DosSemRequest  - Attempt to set (claim) a semaphore         *
*                DosSemClear    - Clear (release) a semaphore                *
*                                                                            *
*                DosBeep        - Make a musical tone on speaker             *
*                DosExit        - Terminate process or one thread            *
*                                                                            *
******************************************************************************
*                                                                            *
*    Initial Release Version 1:                                              *
*                                                                            *
*             - 09/01/89  T.Liethoudt's  Release  Version 1.0                *
*                                                                            *
*    Modified by Pete Guillon D/0768                                         *
*                                                                            *
*             - 06/22/90  Change MRESETTP.CMD to make file RESETTPS.MAK      *
*                         Add main function prototype                        *
*----------------------------------------------------------------------------*
*                                                                            *
*    Release Version 2:                                                      *
*                                                                            *
*             - 09/28/90  Release Version 2.00                               *
*                                                                            *
*                         It's the OS/2 version of DOS RESETTPS. This new    *
*                         program use a "named" system semaphore to share    *
*                         a  "named" shared-memory block.                    *
*                                                                            *
*****************************************************************************/

#pragma subtitle("Include - Define - F prototypes - Global")
#pragma page(1)
#include <stdio.h>

#define INCL_BASE
#define INCL_DOSMEMMGR             /*  Required by the DosGetShrSeg function */
#include <OS2.h>

#include "watchdog.h"              /*  Watchdog Micellaneous definitions     */

VOID main(VOID);                   /*  main function prototype               */


#pragma subtitle("main() function")
#pragma page(1)
VOID main(VOID)
{
   USHORT usReturn_Code;           /*  Dos function Return Code              */

   PSZ    pszName = SHARENAME;              /*  Shareable memory block name  */
   SEL    selMem_Selector;                  /*  Receive Segment selector     */

   PSZ     pszSemName = SEMNAME;            /*  System semaphore name        */
   HSEM    hsemSemHandle;                   /*  System semaphore Handle      */

   WATCH FAR * Watch_Ptr;          /*  WATCH pointer                         */

                                   /* Get Access to Shareable memory Block   */
   usReturn_Code = DosGetShrSeg(pszName, & selMem_Selector);

   if (usReturn_Code == 0) {
      Watch_Ptr = (WATCH FAR *) MAKEP(selMem_Selector,0);

      DosOpenSem( & hsemSemHandle,          /*  Open System semaphore        */
                  pszSemName);

      DosSemRequest(hsemSemHandle,          /*  Waits for shared resources   */
                    SEM_INDEFINITE_WAIT);

                            /* Restore Init value count if WATCHDOG enabled  */
      if (Watch_Ptr->Flag & ENABLE_MASK)
         Watch_Ptr->Counter = Watch_Ptr->Counter_Init_Value ;

      DosSemClear(hsemSemHandle);             /*  Release  shared resources  */

      DosFreeSeg(selMem_Selector);            /*  Freeing Shared memory      */

   } else {
      DosBeep(262,250);DosBeep(330,250);DosBeep(392,250);DosBeep(494,250);
      DosBeep(392,250);DosBeep(330,250);DosBeep(262,250);

#ifdef DEBUG
      printf("\n -*-*- STARTTPS Error Code %04x -*-*-\n",usReturn_Code);
      printf("  -*-    WatchDog not Started    -*-\n");
#endif

   } /* endif */

   DosExit((USHORT) 0,usReturn_Code);         /*  Exit Process              */

} /* end of main program */
