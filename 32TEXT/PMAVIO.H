/***************************************************************************
 *
 * Name:  pmavio.h
 *
 * Function:
 *     Declare the avio calls which a dependent on PM defines
 *
 * Copyright:
 *     Copyright (c) IBM Corp. 1987, 1993
 *
 * Note:
 *     1. INCL_AVIO should be defined to include this file.
 *
 *     2. Many of the Vio declarations are now in bsesub.h
 *
 ****************************************************************************/

/* XLATOFF */
#ifdef __IBMC__
   #pragma checkout( suspend )
   #ifndef __CHKHDR__
      #pragma checkout( suspend )
   #endif
   #pragma checkout( resume )
#endif
/* XLATON */

#define AVIO_INCLUDED

/*
 *  WinDefAVioWindowProc: Window procedure for AVio
 */
MRESULT APIENTRY WinDefAVioWindowProc(HWND hwnd, ULONG msg,
                                      MPARAM mp1, MPARAM mp2);


/* XLATOFF */
#ifdef __IBMC__
   #pragma checkout( suspend )
   #ifndef __CHKHDR__
      #pragma checkout( resume )
   #endif
   #pragma checkout( resume )
#endif
/* XLATON */

