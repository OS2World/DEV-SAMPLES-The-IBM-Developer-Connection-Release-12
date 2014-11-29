/* ******************************************************************** *  
 * Copyright International Business Machines Corporation 1995, 1996     *
 * DISCLAIMER OF WARRANTIES.                                            *  
 * The following [enclosed] code is sample code created by IBM          *  
 * Corporation. This sample code is not part of any standard or IBM     *  
 * product and is provided to you solely for the purpose of assisting   *  
 * you in the development of your applications.  The code is provided   *  
 * "AS IS". IBM MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT  *  
 * NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS *  
 * FOR A PARTICULAR PURPOSE, REGARDING THE FUNCTION OR PERFORMANCE OF   *  
 * THIS CODE.  IBM shall not be liable for any damages arising out of   *  
 * your use of the sample code, even if they have been advised of the   *  
 * possibility of such damages.                                         *  
 *                                                                      *  
 * DISTRIBUTION.                                                        *  
 * This sample code can be freely distributed, copied, altered, and     *  
 * incorporated into other software, provided that it bears the above   *  
 * Copyright notice and DISCLAIMER intact.                              *  
 * -------------------------------------------------------------------- *  
 * This file is part of the source for OpenDoc parts document in:       *
 *  IBM International Technical Support Organization Bulletin (Redbook) *  
 *  "OpenDoc : An Introduction to Part Development" (SG24-4673)         *
 *                                                                      *
 * ******************************************************************** *  
 */   
#define INCL_BASE
#define INCL_GPI

#include <os2.h>
#include <stdlib.h>
#include <string.h>
/****************************************************************
 DefaultOutlineFont: Set an outline font which is sizeable.
****************************************************************/
void SetOutlineFont(HPS   hps,
                    short height)

{FONTMETRICS    fm[80];
 static FATTRS  fat;
 static LONG    cFonts=0;
 LONG           ltemp=0;
 SIZEF          size;
 int            i;
 
 if (height<=2)
   return;

 if (cFonts==0) {
    GpiLoadFonts(WinQueryAnchorBlock(HWND_DESKTOP),"helv");
    cFonts= GpiQueryFonts(hps,QF_PUBLIC|QF_PRIVATE,"Helv",&ltemp,
                  sizeof(FONTMETRICS),(PFONTMETRICS)0);
    if (cFonts>sizeof(fm)/sizeof(FONTMETRICS))
      cFonts=sizeof(fm)/sizeof(FONTMETRICS);
    GpiQueryFonts(hps,QF_PUBLIC|QF_PRIVATE,"Helv",&cFonts,
                  sizeof(FONTMETRICS),fm);
    for (i=0; (!(fm[i].fsDefn&FM_DEFN_OUTLINE)) &&     //find outline and
              (!(fm[i].fsType&FM_TYPE_KERNING))  &&    //kerned if can
              (i<cFonts); i++);
    if (i==cFonts)
      for (i=0; (!(fm[i].fsDefn&FM_DEFN_OUTLINE))&&  // Find outline atleast
                (i<cFonts); i++);
    if (i==cFonts)
      i=0;

    fat.usRecordLength =sizeof(FATTRS);
    fat.lMatch         =fm[i].lMatch;
    fat.lMatch         =0;
    fat.fsFontUse      = FATTR_FONTUSE_OUTLINE;
    strcpy(fat.szFacename,fm[i].szFacename);
   }

 GpiCreateLogFont(hps,(PSTR8)0,1,&fat);
 GpiSetBackMix(hps,(LONG)BM_LEAVEALONE);
 GpiSetCharSet(hps,1);
 size.cx = MAKEFIXED(height,0);
 size.cy = MAKEFIXED(height,0);
 GpiSetCharBox(hps,&size);
 GpiSetTextAlignment(hps,TA_NORMAL_HORIZ,TA_BOTTOM);
}
