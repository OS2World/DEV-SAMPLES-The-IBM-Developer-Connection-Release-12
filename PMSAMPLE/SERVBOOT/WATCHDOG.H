

/* G set printer to condensed mode, Double printing  */

#ifndef WATCHDOG_H

/*****************************************************************************
*       Author:        P.Guillon - D/0768 CER IBM La Gaude                   *
*                                                                            *
*       WATCHDOG definition file  used by:                                   *
*                                                                            *
*              - WATCHDOG.C  Version 2                                       *
*              - RESETTPS.C  Version 2                                       *
*                                                                            *
*----------------------------------------------------------------------------*
*                                                                            *
*    Release Version 2:                                                      *
*                                                                            *
*             - 09/28/90  Release Version 2.00                               *
*                                                                            *
*****************************************************************************/

#define WATCHDOG_H

#define SHARENAME     "\\SHAREMEM\\WATCHDOG.200"
#define SEMNAME       "\\SEM\\WATCHDOG.200"

                                   /* WATCH MASK -------------------- */
#define ENABLE_MASK 0x01                   /*  Enable bit mask        */
#define QUIET_MASK  0x02                   /*  Quiet  bit mask        */
#define U_MASK      0x04                   /*  Uninstall bit mask     */

typedef struct _watch {            /*  Define WATCH structure type    */
        ULONG Counter;
        ULONG Counter_Init_Value;
        UCHAR Flag;
        } WATCH;

#define WATCH_LENGTH sizeof(WATCH) /*  WATCH type length              */

#endif
