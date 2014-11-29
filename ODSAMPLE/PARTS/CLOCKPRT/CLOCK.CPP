/******************************************************************************/
/*                                                                            */
/* COPYRIGHT:                                                                 */
/* ----------                                                                 */
/* Copyright (C) International Business Machines Corp., 1994.                 */
/* Copyright:(C) Apple Computer, Inc., 1994                                   */
/*                                                                            */
/* DISCLAIMER OF WARRANTIES:                                                  */
/* -------------------------                                                  */
/* The following [enclosed] code is sample code created by IBM                */
/* Corporation.  This sample code is not part of any standard IBM product     */
/* and is provided to you solely for the purpose of assisting you in the      */
/* development of your applications.  The code is provided "AS IS",           */
/* without warranty of any kind.  IBM shall not be liable for any damages     */
/* arising out of your use of the sample code, even if they have been         */
/* advised of the possibility of such damages.                                */
/*                                                                            */
/******************************************************************************/
/*
  File:    ClockPart.cp

  Contains:  Source code for the sample "Clock" part

  Original PM App. Written by:  Steve Smith


  Change History (most recent first):

   123251     8/08/95 jso      Added DrawDigitalTime and SetMode
   122425     6/08/95 ced      Fixes for displaying properly when root part
   123407     5/24/95 ced      Display highlight state by drawing inverse color
   117472     4/01/95 ced      Fixes for printing and non-linear transforms.
      <1>     5/20/94 ced      Fixes to externalize properly

*/

#define INCL_WIN
#define INCL_GPI
#define INCL_DOSDATETIME
#define INCL_DOSPROCESS
#include <os2.h>

#include <time.h>
#include <string.h>

#include "Clock.h"

// flag marking changes/additions for transparency
#define ACWSTUFF


POINTL aptlFillet1[]= {{53L ,63L },{ 70L ,50L },{68L,47L}} ;
POINTL aptlFillet2[]= {{68L,47L},{ 70L, 50L },{56L ,67L }} ;
POINTL aptlFillet3[]= {{53L ,64L },{ 56L ,62L },{60L,58L}} ;
POINTL aptlFillet4[]= {{60L,58L},{ 60L, 63L },{56L ,67L }} ;
POINTL ptlLight= { -1L , 1L } ;
POINTL ptlShade= {  2L, -2L } ;
POINTL ptlFace = { 0L, 0L};

static FIXED fxSin [60] =
{
    0x00000000, 0x00001ac2, 0x00003539, 0x00004f1b, 0x0000681f, 0x00007fff,
    0x00009679, 0x0000ab4c, 0x0000be3e, 0x0000cf1b, 0x0000ddb3, 0x0000e9de,
    0x0000f378, 0x0000fa67, 0x0000fe98, 0x0000ffff, 0x0000fe98, 0x0000fa67,
    0x0000f378, 0x0000e9de, 0x0000ddb3, 0x0000cf1b, 0x0000be3e, 0x0000ab4c,
    0x00009679, 0x00008000, 0x00006820, 0x00004f1b, 0x00003539, 0x00001ac2,
    0x00000000, 0xffffe53e, 0xffffcac7, 0xffffb0e5, 0xffff97e1, 0xffff8001,
    0xffff6988, 0xffff54b5, 0xffff41c2, 0xffff30e5, 0xffff224d, 0xffff1622,
    0xffff0c88, 0xffff0599, 0xffff0168, 0xffff0001, 0xffff0167, 0xffff0599,
    0xffff0c88, 0xffff1622, 0xffff224d, 0xffff30e5, 0xffff41c2, 0xffff54b4,
    0xffff6987, 0xffff8000, 0xffff97e0, 0xffffb0e4, 0xffffcac6, 0xffffe53e
} ;


MATRIXLF vmatlfDateTrans = {
     MAKEFIXED ( 1 , 0 ) ,       MAKEFIXED ( 0 , 0 ) ,       0L ,
     MAKEFIXED ( 0 , 0 ) ,       MAKEFIXED ( 1 , 0 ) ,       0L ,
     0L ,                      0L ,                      1L } ;
MATRIXLF vmatlfDateScale  = {
     MAKEFIXED ( 1 , 0 ) ,       MAKEFIXED ( 0 , 0 ) ,       0L ,
     MAKEFIXED ( 0 , 0 ) ,       MAKEFIXED ( 1 , 0 ) ,       0L ,
     0L ,                      0L ,                      1L } ;

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:Clock()
 *
 *  Purpose:Intialize a newly created client window
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns:
 *          1 - if sucessful execution completed
 *          0 - if error
\****************************************************************/
Clock:: Clock()
{
    SIZEL sizl = { 200 , 200 };

    /* init the Data structure */

    /*
     * Create our off-screen 'buffer'.
     */

    data.hdcBuffer = DevOpenDC ( HAB(0), OD_MEMORY, (PSZ) "*", 0L, 0, 0);

    data.hpsBuffer = GpiCreatePS (HAB(0), data.hdcBuffer, &sizl, PU_ARBITRARY |
                               GPIT_MICRO | GPIA_ASSOC);

    // Set the PS into RGB mode!
    GpiCreateLogColorTable (data.hpsBuffer, 0, LCOLF_RGB, 0, 0, (PLONG)NULL);

    // if we don't read this in from storage unit.
    {
        data.cp.usMajorTickPref = CLKTM_ALWAYS;
        data.cp.usMinorTickPref = CLKTM_NOTICONIC;
        data.cp.clrBackground = 0x00008080;
        data.cp.clrFace = 0x00008080;
        data.cp.clrHourHand = RGB_RED;
        data.cp.clrMinuteHand = RGB_RED;
        data.cp.fControlsHidden = FALSE;
        data.cp.usDispMode = DM_TIME | DM_ANALOG | DM_SECONDHAND;
        // data.cp.usDispMode = DM_TIME | DM_DIGITAL | DM_SECONDHAND;
        data.cp.alarm.uchHour = 0;
        data.cp.alarm.uchMinutes = 0;
        data.cp.alarm.usMode = 0;
        SetRGBColors();
        strcpy(data.szTimeSep,":");
        strcpy(data.szAnteMeridian,"AM");
        strcpy(data.szPostMeridian,"PM");
        data.bTwelveHourFormat = TRUE;
        data.fIconic = FALSE;
    }

    /* get the time in a format for dislaying */
    DosGetDateTime(&data.dt);
    data.dt.hours = (UCHAR )(data.dt.hours * (UCHAR) 5) %
                                           (UCHAR) 60 + data.dt.minutes / (UCHAR)12;


}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name: destructor
 *
 *  Purpose: Destroy clock face.
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns:VOID
 *
 *
\****************************************************************/
Clock:: ~Clock ()
{
    HBITMAP hbm;

    hbm = GpiSetBitmap (data.hpsBuffer, NULLHANDLE);

    if (hbm != NULLHANDLE)
        GpiDeleteBitmap (hbm);

    GpiDestroyPS (data.hpsBuffer);
    DevCloseDC (data.hdcBuffer);
}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name: CreateRegion
 *
 *  Purpose: Creates a region.
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns:VOID
 *
 *
\****************************************************************/
HRGN Clock:: CreateRegion( HPS hps )
{
    BOOL f ;
    static POINTL ptlLight= { -2L , 2L } ;
    static POINTL ptlShade= {  1L, -1L } ;
    static POINTL ptlFace = { 0L, 0L};
    static POINTL ptlShadeIn= { -3L , 3L } ;
    static POINTL ptlLightIn= {  1L, -1L } ;
    HRGN hRegion, hRegionTemp;
    RECTL rclBounds;

    /*        1         0      0     *\
    *         0         1      0      *
    \*      100       100      1     */

    static MATRIXLF matlfModel = {
    MAKEFIXED ( 1 , 0 ) ,       MAKEFIXED ( 0 , 0 ) ,       0L ,
    MAKEFIXED ( 0 , 0 ) ,       MAKEFIXED ( 1 , 0 ) ,       0L ,
    100L ,                      100L ,                      1L } ;

    /* center at (100, 100) and draw the clock face */
    f = GpiSetModelTransformMatrix ( hps , ( LONG ) MATLF_SIZE ,
                                     & matlfModel , TRANSFORM_REPLACE ) ;

    DrawFullCircle(hps,
                   &ptlShade,
                   MAKEFIXED(99 ,0));

    hRegion = GpiPathToRegion(hps, 1, FPATH_ALTERNATE);
        GpiQueryRegionBox(hps, hRegion, &rclBounds);

    DrawFullCircle(hps,
                   &ptlLight,
                   MAKEFIXED(98 ,0));

    hRegionTemp = GpiPathToRegion(hps, 1, FPATH_ALTERNATE);
    GpiCombineRegion(hps, hRegion, hRegion, hRegionTemp, CRGN_OR);
    GpiDestroyRegion(hps, hRegionTemp);

    DrawFullCircle(hps,
                   &ptlLightIn,
                   MAKEFIXED(94,0));

    hRegionTemp = GpiPathToRegion(hps, 1, FPATH_ALTERNATE);
    GpiCombineRegion(hps, hRegion, hRegion, hRegionTemp, CRGN_OR);
    GpiDestroyRegion(hps, hRegionTemp);

    DrawFullCircle(hps,
                   &ptlShadeIn,
                   MAKEFIXED(93,0));

    hRegionTemp = GpiPathToRegion(hps, 1, FPATH_ALTERNATE);
    GpiCombineRegion(hps, hRegion, hRegion, hRegionTemp, CRGN_OR);
    GpiDestroyRegion(hps, hRegionTemp);

    DrawFullCircle(hps,
                    &ptlFace ,
                    MAKEFIXED(98 ,0));

    hRegionTemp = GpiPathToRegion(hps, 1, FPATH_ALTERNATE);
    GpiCombineRegion(hps, hRegion, hRegion, hRegionTemp, CRGN_OR);
    GpiDestroyRegion(hps, hRegionTemp);

    return hRegion;
}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name: Size()
 *
 *  Purpose:When the window has been sized, we calculate  a page
 *          rectangle which: (a) fills the window rectangle in either
 *          the x or y dimension, (b) appears square, and (c) is centered
 *          in the window rectangle
 *  Usage:
 *
 *  Method:
 *
 *  Returns:
 *
 *
\****************************************************************/
VOID Clock:: Size( RECTL& rcl, HRGN *hrgnUsed)
{
    SIZEF            sizef;

    LONG             cxSquare;
    LONG             cySquare;

    LONG             cxEdge;
    LONG             cyEdge;

    LONG             cyHeight;
    LONG             cxWidth;

    HBITMAP          hbm;
    BITMAPINFOHEADER bmp;

    HRGN             hOldRegion;
    RECTL            rclBounds;

    LONG             cxRes;
    LONG             cyRes;

    LONG             cColorPlanes;
    LONG             cColorBitcount;

    BOOL             f;
    SWP              swp;

    this-> rcl = rcl;

    /*
     * First get rid of any buffer bitmap already there.
     */
    hbm = GpiSetBitmap (data.hpsBuffer, NULLHANDLE);

    if (hbm != NULLHANDLE)
        GpiDeleteBitmap (hbm);

    /*
     * Get the width and height of the window rectangle.
     */
    cxWidth = rcl.xRight - rcl.xLeft;
    cyHeight = rcl.yTop - rcl.yBottom;

    /*
     * Now create a bitmap the size of the window.
     */

    //KLS Get device info from desktop window.
    HPS hps;                /* presentation space handle            */
    HDC hdc;                /* device context handle                */
    LONG lWidth, lHeight;

    DevQueryCaps (data.hdcBuffer, CAPS_COLOR_PLANES, 1L, &cColorPlanes);
    DevQueryCaps (data.hdcBuffer, CAPS_COLOR_BITCOUNT, 1L, &cColorBitcount);
    DevQueryCaps (data.hdcBuffer, (LONG)CAPS_VERTICAL_RESOLUTION,(LONG) 1L, &cyRes);
    DevQueryCaps (data.hdcBuffer, CAPS_HORIZONTAL_RESOLUTION, 1L, &cxRes);

    bmp.cbFix = sizeof(BITMAPINFOHEADER);
    bmp.cx = (SHORT)cxWidth;
    bmp.cy = (SHORT)cyHeight;
    bmp.cPlanes = (SHORT)cColorPlanes;
    bmp.cBitCount = (SHORT)cColorBitcount;
    hbm = GpiCreateBitmap(data.hpsBuffer, (PBITMAPINFOHEADER2)&bmp,
                          0x0000, (PBYTE)NULL, (PBITMAPINFO2)NULL);
    GpiSetBitmap (data.hpsBuffer, hbm);

    /*
     * Assume the size of the page rectangle is constrained in the y
     * dimension,compute the x size which would make the rectangle appear
     * square, then check the assumption and do the reverse calculation
     * if necessary.
     */
    cySquare = cyHeight;
    cxSquare = ( cyHeight * cxRes ) / cyRes;

    if (cxWidth < cxSquare)
    {
        cxSquare = cxWidth;
        cySquare = (cxWidth * cyRes) / cxRes;
    }

    /*
     * Fill in the page rectangle and set the page viewport.
     */
    cxEdge = (cxWidth - cxSquare ) / 2;
    cyEdge = (cyHeight - cySquare ) / 2;
    rclPage.xLeft = cxEdge;
    rclPage.xRight = cxWidth - cxEdge;
    rclPage.yBottom = cyEdge;
    rclPage.yTop = cyHeight - cyEdge;

    f = GpiSetPageViewport (data.hpsBuffer, &rclPage);


    *hrgnUsed = CreateRegion(data.hpsBuffer);

//  if (data.hrgnFace != 0)
//     GpiDestroyRegion(data.hpsBuffer, data.hrgnFace);

//  data.hrgnFace = CreateFaceRegion(data.hpsBuffer);

    GpiQueryCharBox(data.hpsBuffer, &sizef);
    GpiSetCharBox(data.hpsBuffer, &sizef);
    data.fBufferDirty = TRUE;
}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name: DrawAnalog()
 *
 *  Purpose:Draw the clock face ,hand and minute hand,
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns: void
 *
 *
\****************************************************************/
void Clock:: DrawAnalog(HPS hpsDraw, BOOL fTransparent)
{
    HPS hps;
    if (hpsDraw)
       hps = hpsDraw;
    else
       hps = data.hpsBuffer;

    /* draw the face, the hour hand, and the minute hand */

    if (!fTransparent) {
      POINTL ptl = {-101, -101};
      GpiSetColor ( hps , data.vclrFace[SURFACE]);
      GpiMove(hps, &ptl);
      ptl.x = ptl.y = 100;
      GpiBox(hps, DRO_FILL, &ptl, 0, 0);

      DrawRing (hps, data.vclrRing, data.vclrFace);

      DrawFace (hps, data.vclrFace);
    }

    DrawHand (hps, HT_HOUR_SHADE, data.dt.hours, data.vclrFace, data.vclrHands);

    DrawHand (hps, HT_MINUTE_SHADE, data.dt.minutes, data.vclrFace, data.vclrHands);

    DrawHand (hps, HT_HOUR, data.dt.hours, data.vclrFace, data.vclrHands);

    DrawHand (hps, HT_MINUTE, data.dt.minutes, data.vclrFace, data.vclrHands);

    /* draw the tick marks */
    DrawTicks (hps, CLK_MAJORTICKS, data.vclrMajorTicks, data.vclrMinorTicks);

    DrawTicks (hps, CLK_MINORTICKS, data.vclrMajorTicks, data.vclrMinorTicks);
}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:DrawDigitalTime()
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *          -
 *
 *  Returns: void
 *
\****************************************************************/

VOID Clock::DrawDigitalTime(HPS hpsDraw, RECTL & rcl, BOOL fTransparent)
{
// jso    RECTL rcl;
    RECTL rclChar;
    RECTL rclCharOld;
    RECTL rclTime;
    RECTL rclAmPm;
    RECTL rclDate;
    SIZEF sizef;
    USHORT usi;
    ULONG  ulCharWidth,ulCharModulu;
    char achTime[9];
    char achFinalDate[9];
    time_t     tTime;
    struct tm  *pLocalTime;

//  jso  WinQueryWindowRect (hwnd, &rcl);

    memset(achTime,0,sizeof(achTime) );
    memset(achFinalDate,0,sizeof(achFinalDate) );

    GpiCreateLogColorTable (hpsDraw, LCOL_RESET, LCOLF_RGB, 0, 0,
                                 (PLONG) NULL);

    /*
     *if black hands and black background
     *selected force the background to
     *blue
     */

    if( !data.cp.clrMinuteHand && !data.cp.clrBackground )
    {
          data.cp.clrBackground = RGB_BLUE;
    }



    switch (data.cp.usDispMode & (DM_TIME | DM_DATE))
    {
    case DM_DATE:
        rclDate = rcl;
        break;

    case DM_TIME | DM_DATE:
        if (!data.fIconic)
        {
            rclTime = rclDate = rcl;
            rclTime.yBottom = rclDate.yTop = rcl.yTop / 2;
            break;
        } /*else fall through*/

    case DM_TIME:
        rclTime = rcl;
        break;
    }

    if (data.cp.usDispMode & DM_TIME)
    {

        rclAmPm = rclTime;
        time(&tTime);
        pLocalTime = localtime(&tTime);
        if (data.bTwelveHourFormat)
        {
           strftime(achTime, sizeof(achTime), "%I %M %S", pLocalTime);
        }
        else
        {
           strftime(achTime, sizeof(achTime), "%H %M %S", pLocalTime);
        }

        /*insert country time separator*/
        achTime[2] = achTime[5] = data.szTimeSep[0];
        achTime[8] = '\000';
        /*Process 12 hours mode*/
        if (data.bTwelveHourFormat)
        {

            if (data.fIconic)
            {
                rclTime.yBottom = rclAmPm.yTop = rclTime.yTop / 2;
            }
            else
            {
                rclTime.xRight = rcl.xRight * 8 / 11;
                rclAmPm.xLeft = rclTime.xRight;
            }

            strcpy(data.achAmPm,data.szAnteMeridian);

            if (pLocalTime->tm_hour >= 12)
            {
                strcpy(data.achAmPm,data.szPostMeridian);
            }

        }

        GpiSetCharMode(hpsDraw, CM_MODE3);

        if (data.fIconic)
        {
            sizef.cx = MAKEFIXED(LOUSHORT((rclTime.xRight - rclTime.xLeft)
                    / 3), 0);
            sizef.cy = MAKEFIXED(LOUSHORT((rclTime.yTop - rclTime.yBottom)
                    * 1400 / 1000), 0);
        }
        else
        {
            sizef.cx = MAKEFIXED(LOUSHORT((rclTime.xRight - rclTime.xLeft)
                    / 6), 8000);
            sizef.cy = MAKEFIXED(LOUSHORT((rclTime.yTop - rclTime.yBottom)
                    * 1000 / 1500), 0);
        }

        GpiSetCharBox(hpsDraw, &sizef);

        if (data.bTwelveHourFormat)
        {

            if( strcmp(data.achAmPm,data.achOldAmPm) )
            {
                //WinFillRect(hpsDraw, &rclAmPm, data.cp.clrBackground);
#ifdef ACWSTUFF
                if (!fTransparent)
#endif
                  FillRect(hpsDraw, rclAmPm);
                WinDrawText(hpsDraw, sizeof(data.achAmPm) - 1, (PSZ)data.achAmPm,
                        (PRECTL)&rclAmPm, data.cp.clrMinuteHand, data.cp.clrBackground,
                        DT_CENTER | DT_VCENTER );
            }
        }

        if (!data.fIconic)
        {

            WinDrawText(hpsDraw, sizeof(achTime) - 1 , (PSZ)achTime,
                    (PRECTL)&rclTime, data.cp.clrMinuteHand, data.cp.clrBackground,
                    DT_CENTER | DT_VCENTER | DT_QUERYEXTENT);

            ulCharWidth = (rclTime.xRight - rclTime.xLeft) / (sizeof(achTime) - 1 );
            ulCharModulu = (rclTime.xRight - rclTime.xLeft) % (sizeof(achTime) - 1 );
            rclCharOld.xRight = rclTime.xLeft;

            rclChar.yTop = rclTime.yTop;
            rclChar.yBottom = rclTime.yBottom;

            for (usi = 0; usi < (sizeof(achTime)); usi++)
            {
                rclChar.xLeft = rclCharOld.xRight + (ULONG)1;
                rclChar.xRight = rclChar.xLeft + ulCharWidth +
                        ((ulCharModulu > 0L) ? 1L : 0L);

                if (ulCharModulu)
                    ulCharModulu--;

                if (achTime[usi] == data.szTimeSep[0])
                {
                    rclChar.xRight -= 3;
                }
                else
                {
                    rclChar.xRight += 1;
                }

                rclCharOld = rclChar;

                if (achTime[usi] != data.achOldTime[usi] )
                {
                    // WinFillRect (hpsDraw, &rclChar, data.cp.clrBackground);
#ifdef ACWSTUFF
                    if (!fTransparent)
#endif
                      FillRect(hpsDraw, rclChar);

                    if (!((usi == 0) && (achTime[0] == '0') &&
                            (data.bTwelveHourFormat)))
                        WinDrawText (hpsDraw, 1, (PSZ)&achTime[usi], &rclChar,
                                data.cp.clrMinuteHand, data.cp.clrBackground,
                                DT_CENTER | DT_VCENTER);
                }
            }
        }
        else
        {   /*Iconic. just draw if minute changed*/

            if (strncmp(achTime,data.achOldTime,5))
            {
                //WinFillRect(hpsDraw,&rclTime,data.cp.clrBackground);
#ifdef ACWSTUFF
                if (!fTransparent)
#endif
                  FillRect(hpsDraw, rclTime);

                WinDrawText(hpsDraw, sizeof(achTime) - 4 , (PSZ)achTime,
                        (PRECTL)&rclTime, data.cp.clrMinuteHand, data.cp.clrBackground,
                        DT_CENTER | DT_VCENTER);
            }
        }
    }

    if ((!(data.cp.usDispMode & DM_TIME)) || ((data.cp.usDispMode & DM_DATE) &&
            (!data.fIconic)))
    {
        GetArrangedDate(achFinalDate);
        if (strncmp(achFinalDate, data.achOldDate,
                sizeof(achFinalDate) - data.fIconic ? 4 : 1))
        {

#ifdef ACWSTUFF
            if (!fTransparent)
#endif
              WinFillRect (hpsDraw, &rclDate, data.cp.clrBackground);

            GpiSetCharMode (hpsDraw, CM_MODE3);

            sizef.cx = MAKEFIXED(LOUSHORT(
                            (rclDate.xRight - rclDate.xLeft) / 5), 0);
            sizef.cy = MAKEFIXED(LOUSHORT(
                            (rclDate.yTop - rclDate.yBottom) * 1000 / 1500), 0);

            GpiSetCharBox(hpsDraw, &sizef);
            WinDrawText(hpsDraw,
                        (sizeof(achFinalDate) - (data.fIconic ? 4 : 1) ),
                        (PSZ)achFinalDate,(PRECTL)&rclDate,
                        (LONG)data.cp.clrMinuteHand,
                        (LONG)data.cp.clrBackground,
                        (ULONG )(DT_CENTER | DT_VCENTER ) );
        }
    }

    strncpy(data.achOldTime,achTime,sizeof(data.achOldTime));
    strncpy(data.achOldAmPm,data.achAmPm,sizeof(data.achOldAmPm));
    strncpy(data.achOldDate,achFinalDate,sizeof(data.achOldDate));
}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name: Draw
 *
 *  Purpose: Paint the clock client window.
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns:  void
 *
 *
\****************************************************************/
void Clock:: Draw( HPS hps, PRECTL rclFrame, BOOL toScreen, BOOL fTransparent)
{

   RECTL rcl = *rclFrame;
   ULONG cxFrame = rclFrame->xRight - rclFrame->xLeft;
   ULONG cyFrame = rclFrame->yTop - rclFrame->yBottom;
   if (cxFrame > cyFrame)
   {
      rcl.xLeft += ((cxFrame - cyFrame) / 2);
      rcl.xRight = rcl.xLeft + cyFrame;
   }
   else if (cxFrame < cyFrame)
   {
      rcl.yBottom += ((cyFrame - cxFrame) / 2);
      rcl.yTop = rcl.yBottom + cxFrame;
   }

   // Set the PS into RGB mode!
   GpiCreateLogColorTable (hps, 0, LCOLF_RGB, 0, 0, (PLONG)NULL);

   GpiSetColor(hps, data.vclrFace[SURFACE]);

   if (!fTransparent) {
     POINTL ptl = {0, 0};
     GpiMove(hps, &ptl);
     ptl.x = rclFrame->xRight;
     ptl.y = rclFrame->yTop;
     GpiBox(hps, DRO_FILL, &ptl, 0, 0);
   }

   MATRIXLF mtlf, mtlfSave;
   FIXED afxScale[2] = { (rcl.xRight - rcl.xLeft) * 0x10000 / 200,
                         (rcl.yTop - rcl.yBottom) * 0x10000 / 200 };
   POINTL ptlOffset = {rcl.xLeft, rcl.yBottom}, ptlScaleOffset = {0, 0};
   GpiScale(hps, &mtlf, TRANSFORM_REPLACE, afxScale, &ptlScaleOffset);
   GpiTranslate(hps, &mtlf, TRANSFORM_ADD, &ptlOffset);
   GpiQueryDefaultViewMatrix(hps, 9, &mtlfSave);
   GpiSetDefaultViewMatrix(hps, 9, &mtlf, TRANSFORM_PREEMPT);

   if (data.cp.usDispMode & DM_ANALOG)
   {
//#ifndef ACWSTUFF
       if (toScreen && !fTransparent)
       {
          if ( data. fBufferDirty )
          {
             DrawAnalog(0,fTransparent);
             data. fBufferDirty = FALSE;
          }
          GpiSetDefaultViewMatrix(hps, 9, &mtlfSave, TRANSFORM_REPLACE);
          UpdateScreen (hps, &rcl);
          GpiSetDefaultViewMatrix(hps, 9, &mtlf, TRANSFORM_PREEMPT);
       }
       else
//#endif
       {
#ifdef ACWSTUFF
           DrawAnalog(hps,fTransparent);
#else
           DrawAnalog(hps);
#endif
       }

       DrawHand( hps, HT_SECOND, data.dt.seconds, data.vclrFace, data.vclrHands );
   }
   else
   { /*For now, if it is not Analog, it must be digital*/


       // WinFillRect (hps, &rcl, data.cp.clrBackground);
       // FillRect(hps, rcl);
       memset (data.achOldTime, 0, sizeof(data.achOldTime));
       memset (data.achOldAmPm, 0, sizeof(data.achOldAmPm));
       memset (data.achOldDate, 0, sizeof(data.achOldDate));

       // DrawDigitalTime (hps, rcl);

   }

}

/****************************************************************\
 *  Name:Timer()
 *
 *  Purpose: Handles window timer events
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns:
\****************************************************************/
BOOL Clock:: Timer ( HPS hps, PRECTL rclFrame)
{
    HRGN hrgnInvalid = 0;

   RECTL rcl = *rclFrame;
   ULONG cxFrame = rclFrame->xRight - rclFrame->xLeft;
   ULONG cyFrame = rclFrame->yTop - rclFrame->yBottom;
   if (cxFrame > cyFrame)
   {
      rcl.xLeft += ((cxFrame - cyFrame) / 2);
      rcl.xRight = rcl.xLeft + cyFrame;
   }
   else if (cxFrame < cyFrame)
   {
      rcl.yBottom += ((cyFrame - cxFrame) / 2);
      rcl.yTop = rcl.yBottom + cxFrame;
   }

    // Set the PS into RGB mode!
    GpiCreateLogColorTable (hps, 0, LCOLF_RGB, 0, 0, (PLONG)NULL);

    DATETIME  dtNew;
    static LONG lTimeChangeCheck = 0L;
    LONG  lTime;
    BOOL invalidate = FALSE;

    if (data.cp.usDispMode & DM_ANALOG)
    {
      /* get the new time */
      DosGetDateTime (&dtNew);

      /* adjust the hour hand */
      dtNew.hours = (dtNew.hours * (UCHAR) 5 ) % (UCHAR) 60
                    + dtNew.minutes / (UCHAR)12;

      /* if we must move the hour and minute hands, redraw it all */
      if (dtNew.minutes != data. dt.minutes)
      {
          DrawFace (data. hpsBuffer, data.vclrFace);
          DrawHand (data. hpsBuffer, HT_HOUR_SHADE, dtNew.hours, data.vclrFace, data.vclrHands);
          DrawHand (data. hpsBuffer, HT_MINUTE_SHADE, dtNew.minutes, data.vclrFace, data.vclrHands);
          DrawHand (data. hpsBuffer, HT_HOUR, dtNew.hours, data.vclrFace, data.vclrHands);
          DrawHand (data. hpsBuffer, HT_MINUTE, dtNew.minutes, data.vclrFace, data.vclrHands);
          invalidate = TRUE;
      }
      else /* otherwise just undraw the old second hand and draw the new */
      {
          MATRIXLF mtlf;
          FIXED afxScale[2] = { (rcl.xRight - rcl.xLeft) * 0x10000 / 200,
                                (rcl.yTop - rcl.yBottom) * 0x10000 / 200 };
          POINTL ptlOffset = {rcl.xLeft, rcl.yBottom}, ptlScaleOffset = {0, 0};
          GpiScale(hps, &mtlf, TRANSFORM_REPLACE, afxScale, &ptlScaleOffset);
          GpiTranslate(hps, &mtlf, TRANSFORM_ADD, &ptlOffset);
          GpiSetDefaultViewMatrix(hps, 9, &mtlf, TRANSFORM_PREEMPT);

          GpiSetMix(hps, FM_INVERT);
          DrawHand( hps, HT_SECOND, data. dt.seconds, data.vclrFace, data.vclrHands);
          DrawHand( hps, HT_SECOND, dtNew.seconds, data.vclrFace, data.vclrHands);
      }
      data. dt = dtNew ;
    }
    else /*Must be Digital*/
    {
      if(data.fBufferDirty)
      {
        FillRect(hps, rcl);
        memset (data.achOldTime, 0, sizeof(data.achOldTime));
        memset (data.achOldAmPm, 0, sizeof(data.achOldAmPm));
        memset (data.achOldDate, 0, sizeof(data.achOldDate));
        data. fBufferDirty = FALSE;
      }
      DrawDigitalTime(hps, rcl, FALSE);
      invalidate = TRUE;
    }
    return invalidate;
}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:UpdateScreen()
 *
 *  Purpose: Update the screen area.
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns:
 *
 *
\****************************************************************/
VOID Clock:: UpdateScreen (HPS hps, PRECTL rclFrame)
{
    POINTL aptl[4];
    HBITMAP hbm;

    aptl[0].x = rclFrame->xLeft;
    aptl[0].y = rclFrame->yBottom;
    aptl[1].x = rclFrame->xRight;
    aptl[1].y = rclFrame->yTop;
    aptl[2].x = this->rclPage.xLeft;
    aptl[2].y = this->rclPage.yBottom;
    aptl[3].x = this->rclPage.xRight;
    aptl[3].y = this->rclPage.yTop;

    hbm = GpiSetBitmap(data.hpsBuffer, 0);      // Get the bitmap from the presentation space

    IMAGEBUNDLE ibundle;
    LONG lRop;
    GpiQueryDefAttrs(hps, PRIM_IMAGE, IBB_MIX_MODE, &ibundle);
    lRop = (ibundle.usMixMode == FM_NOTCOPYSRC) ? ROP_NOTSRCCOPY : ROP_SRCCOPY;

    GpiWCBitBlt (hps, hbm, 4L, aptl, lRop, BBO_IGNORE);
    GpiSetBitmap(data.hpsBuffer, hbm);          // Set bitmap back into pres space
}

/**************************************************************************\
*                                                                          *
*       ROUTINE:    DrawHand                                            *
*                                                                          *
*       COMMENT:    Draws specified hand at specified hour in given PS     *
*                                                                          *
\**************************************************************************/

void Clock:: DrawHand ( HPS hps , SHORT sHandType , SHORT sAngle, LONG vclrFace[], LONG vclrHands[] )
{
    static POINTL aptlHour [ ] = { { 6 , 0 } , { 0 , 62 } , { -6 , 0 } ,
                                   { 0 , -14 } , { 6 , 0 } } ;
    static POINTL aptlHourLine1 [] = {{0L,-10L},{0L,56}};

    static POINTL aptlMinute [ ] = { { 5 , 0 } , { 0 , 77 } , { -5 , 0 } ,
                                     { 0 , -14 } , { 5 , 0 } } ;
    static POINTL aptlMinuteLine1 [] = {{0L,-15L},{0L,72}};

    static POINTL aptlSecond [ ] = { { 0 , -15 } , { 0 , 74 } } ;
    static POINTL ptlOrigin = {0L,0L};

    static LONG cptlHour = sizeof ( aptlHour ) / sizeof ( POINTL ) ;
    static LONG cptlMinute = sizeof ( aptlMinute ) / sizeof ( POINTL ) ;
    static LONG cptlSecond = sizeof ( aptlSecond ) / sizeof ( POINTL ) ;
    BOOL f ;

    static MATRIXLF matlfModel =
    {
        MAKEFIXED ( 1 , 0 ) ,       MAKEFIXED ( 0 , 0 ) ,       0L ,
        MAKEFIXED ( 0 , 0 ) ,       MAKEFIXED ( 1 , 0 ) ,       0L ,
        100L ,                      100L ,                      1L } ;

    static MATRIXLF matlfShade =
    {
        MAKEFIXED ( 1 , 0 ) ,       MAKEFIXED ( 0 , 0 ) ,       0L ,
        MAKEFIXED ( 0 , 0 ) ,       MAKEFIXED ( 1 , 0 ) ,       0L ,
        3L ,                     -3L ,                      1L } ;

    /* prepare a rotation transform and set it into the ps */
    /*      cos x    - sin x    0     *\
    |       sin x      cos x    0      |
    \*      100        100      1     */

    matlfModel.fxM11 =
    matlfModel.fxM22 = fxSin[(sAngle + 15) % 60];
    matlfModel.fxM12 = fxSin[(sAngle + 30) % 60];
    matlfModel.fxM21 = fxSin[sAngle];

    f = GpiSetModelTransformMatrix(hps, (LONG)MATLF_SIZE, &matlfModel, TRANSFORM_REPLACE);

    /* draw the specified hand */

    switch ( sHandType )
    {

        case HT_HOUR:
            GpiSetColor ( hps , vclrHands[SURFACE] ) ;
            GpiBeginPath ( hps , 1L ) ;
            GpiSetCurrentPosition ( hps , aptlHour ) ;
            GpiPolyLine ( hps , cptlHour , aptlHour ) ;
            GpiEndPath ( hps ) ;
            GpiFillPath ( hps , 1L , FPATH_ALTERNATE ) ;
            GpiSetColor ( hps , vclrHands[SHADE]   ) ;
            GpiSetCurrentPosition ( hps , aptlHour ) ;
            GpiPolyLine ( hps , cptlHour , aptlHour ) ;
            GpiSetColor ( hps , vclrHands[SHADE]   ) ;
            GpiSetCurrentPosition ( hps , aptlHourLine1 ) ;
            GpiPolyLine ( hps , 1L , &(aptlHourLine1[1]) ) ;
            break;
        case HT_HOUR_SHADE:
            GpiSetModelTransformMatrix ( hps , ( LONG ) MATLF_SIZE ,
                                              & matlfShade , TRANSFORM_ADD     ) ;
            GpiSetColor ( hps , vclrFace [SHADE]   ) ;
            GpiBeginPath ( hps , 1L ) ;
            GpiSetCurrentPosition ( hps , aptlHour ) ;
            GpiPolyLine ( hps , cptlHour , aptlHour ) ;
            GpiEndPath ( hps ) ;
            GpiFillPath ( hps , 1L , FPATH_ALTERNATE ) ;
            break;

        case HT_MINUTE:
            GpiSetColor ( hps , vclrHands[SURFACE] ) ;
            GpiBeginPath ( hps , 1L ) ;
            GpiSetCurrentPosition ( hps , aptlMinute ) ;
            GpiPolyLine ( hps , cptlMinute , aptlMinute ) ;
            GpiEndPath ( hps ) ;
            GpiFillPath ( hps , 1L , FPATH_ALTERNATE ) ;

            GpiSetColor ( hps , vclrHands[SHADE]   ) ;
            GpiSetCurrentPosition ( hps , aptlMinute ) ;
            GpiPolyLine ( hps , cptlMinute , aptlMinute ) ;
            GpiSetColor ( hps , vclrHands[SHADE]   ) ;
            GpiSetCurrentPosition ( hps , aptlMinuteLine1 ) ;
            GpiPolyLine ( hps , 1L , &(aptlMinuteLine1[1]) ) ;
            GpiSetCurrentPosition ( hps , & ptlOrigin) ;
            GpiFullArc ( hps , DRO_OUTLINEFILL , MAKEFIXED ( 2 , 0 ) ) ;
            break;
        case HT_MINUTE_SHADE:
            GpiSetModelTransformMatrix ( hps , ( LONG ) MATLF_SIZE ,
                                              & matlfShade , TRANSFORM_ADD     ) ;
            GpiSetColor ( hps , vclrFace [SHADE]   ) ;
            GpiBeginPath ( hps , 1L ) ;
            GpiSetCurrentPosition ( hps , aptlMinute ) ;
            GpiPolyLine ( hps , cptlMinute , aptlMinute ) ;
            GpiEndPath ( hps ) ;
            GpiFillPath ( hps , 1L , FPATH_ALTERNATE ) ;
            break;

        case HT_SECOND:
            /* draw in XOR mixmode, so we can undraw later */
            GpiSetMix ( hps , FM_INVERT ) ;
            GpiSetCurrentPosition ( hps , aptlSecond ) ;
            GpiPolyLine ( hps , cptlSecond , aptlSecond ) ;
            GpiSetMix ( hps , FM_OVERPAINT ) ;
            break;
    }
}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:DrawFace()
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns: void
 *
 *
\****************************************************************/
void Clock:: DrawFace ( HPS hps, LONG vclrFace[] )
{
    BOOL f ;
    static POINTL ptlLight= { -2L , 2L } ;
    static POINTL ptlShade= {  2L, -2L } ;
    static POINTL ptlFace = { 0L, 0L};
    /*        1         0      0     *\
    *         0         1      0      *
    \*      100       100      1     */

    static MATRIXLF matlfModel = {
        MAKEFIXED ( 1 , 0 ) ,       MAKEFIXED ( 0 , 0 ) ,       0L ,
        MAKEFIXED ( 0 , 0 ) ,       MAKEFIXED ( 1 , 0 ) ,       0L ,
        100L ,                      100L ,                      1L } ;

    /* center at (100, 100) and draw the clock face */
    f = GpiSetModelTransformMatrix ( hps , ( LONG ) MATLF_SIZE ,
                                     & matlfModel , TRANSFORM_REPLACE ) ;


    GpiSetColor ( hps , vclrFace[SURFACE]);
    GpiSetCurrentPosition ( hps , & ptlFace ) ;
    GpiFullArc ( hps , DRO_OUTLINEFILL , MAKEFIXED ( 80, 0 ) ) ;

    FATTRS fat;
    POINTL grad = { 1, 4 };
    POINTL aptlPoints[TXTBOX_COUNT];
    LONG lcid = 101;
    memset(&fat, 0, sizeof(FATTRS));
    fat.usRecordLength = sizeof(FATTRS);
    strcpy(fat.szFacename, "Helv Bold");
    fat.fsFontUse = FATTR_FONTUSE_OUTLINE;
    if (FONT_MATCH == GpiCreateLogFont( hps, 0, lcid, &fat))
    {
       FONTMETRICS fm;
       GpiSetCharSet(hps, lcid);
       GpiQueryFontMetrics(hps, sizeof(fm), &fm);
       GpiSetCharShear(hps, &grad);
       SIZEF sizfxBox ;
       sizfxBox.cy = MAKEFIXED(20,0);
       sizfxBox.cx = 20 * fm.lEmHeight * 0x10000 / fm.lEmInc;
       GpiSetCharBox(hps, &sizfxBox);
    }
    POINTL ptlString ;
    GpiSetColor ( hps, vclrFace[SHADE]);
    GpiSetTextAlignment(hps, TA_CENTER, TA_HALF);
    ptlString.x = 0;
    ptlString.y = 40;
    GpiCharStringAt ( hps, &ptlString, 7, (PCH) "OpenDoc");
    ptlString.y = -45;
    GpiCharStringAt ( hps, &ptlString, 4, (PCH) "OS/2");

    GpiDeleteSetId(hps, lcid);
}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:CreateFaceRegion()
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns: void
 *
 *
\****************************************************************/
HRGN Clock:: CreateFaceRegion ( HPS hps )
{
    BOOL f ;
    static POINTL ptlFace = { 0L, 0L};
    /*        1         0      0     *\
    *         0         1      0      *
    \*      100       100      1     */

    static MATRIXLF matlfModel = {
        MAKEFIXED ( 1 , 0 ) ,       MAKEFIXED ( 0 , 0 ) ,       0L ,
        MAKEFIXED ( 0 , 0 ) ,       MAKEFIXED ( 1 , 0 ) ,       0L ,
        100L ,                      100L ,                      1L } ;

    /* center at (100, 100) and draw the clock face */
    f = GpiSetModelTransformMatrix ( hps , ( LONG ) MATLF_SIZE ,
                                     & matlfModel , TRANSFORM_REPLACE ) ;

    GpiBeginPath(hps, 1);
    GpiSetCurrentPosition ( hps , & ptlFace ) ;
    GpiFullArc ( hps , DRO_OUTLINE , MAKEFIXED ( 80, 0 ) ) ;
    GpiEndPath (hps);
    return GpiPathToRegion(hps, 1, FPATH_ALTERNATE);
}

void Clock:: DrawRing ( HPS hps, LONG vclrRing[], LONG vclrFace[] )
{
    BOOL f ;
    static POINTL ptlLight= { -2L , 2L } ;
    static POINTL ptlShade= {  1L, -1L } ;
    static POINTL ptlFace = { 0L, 0L};
    static POINTL ptlShadeIn= { -3L , 3L } ;
    static POINTL ptlLightIn= {  1L, -1L } ;
    /*        1         0      0     *\
    *         0         1      0      *
    \*      100       100      1     */

    static MATRIXLF matlfModel = {
        MAKEFIXED ( 1 , 0 ) ,       MAKEFIXED ( 0 , 0 ) ,       0L ,
        MAKEFIXED ( 0 , 0 ) ,       MAKEFIXED ( 1 , 0 ) ,       0L ,
        100L ,                      100L ,                      1L } ;

    /* center at (100, 100) and draw the clock face */
    f = GpiSetModelTransformMatrix ( hps , ( LONG ) MATLF_SIZE ,
                                     & matlfModel , TRANSFORM_REPLACE ) ;

    DrawFullRing(hps,
                    &ptlShade,
                    MAKEFIXED(95,0),
                    MAKEFIXED(99 ,0),
                    vclrRing[SHADE]);

    DrawFullRing(hps,
                    &ptlLight,
                    MAKEFIXED(95,0),
                    MAKEFIXED(98 ,0),
                    vclrRing[LIGHT]   );

    DrawFullRing(hps,
                    &ptlLightIn,
                    MAKEFIXED(88,0),
                    MAKEFIXED(94,0),
                    vclrRing[LIGHT]   );


    DrawFullRing(hps,
                    &ptlShadeIn,
                    MAKEFIXED(86,0),
                    MAKEFIXED(93,0),
                    vclrRing[SHADE]);

    DrawFullRing(hps,
                    &ptlFace ,
                    MAKEFIXED(94,0),
                    MAKEFIXED(98 ,0),
                    vclrRing[SURFACE]);

    GpiSetColor ( hps , vclrFace[SURFACE]);
    GpiSetCurrentPosition ( hps , & ptlFace ) ;
    GpiFullArc ( hps , DRO_OUTLINEFILL , MAKEFIXED ( 91, 0 ) ) ;
}

void Clock:: DrawFullCircle(HPS hps, PPOINTL pptlCenter, FIXED fxRad)
{
    GpiSetCurrentPosition ( hps , pptlCenter );
    GpiBeginPath(hps,1L);
    GpiFullArc ( hps , DRO_OUTLINE ,  fxRad  ) ;
    GpiEndPath(hps);
}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:GetArrangedDate()
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns: VOID
 *
\****************************************************************/
VOID Clock:: GetArrangedDate(CHAR achFinalDate[])
{
    time_t     tTime;
    struct tm  *pLocalTime;

    time(&tTime);
    pLocalTime = localtime(&tTime);
    strftime(achFinalDate, sizeof(achFinalDate), "%m %d %y", pLocalTime);

    /*put date separators*/
    achFinalDate[2] = achFinalDate[5] = '/';
    achFinalDate[8] = '\0';
}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:DrawTicks()
 *
 *  Purpose:Draws Clock Ticks()
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns: void
 *
 *
\****************************************************************/
void Clock:: DrawTicks ( HPS hps , USHORT usTicks, LONG vclrMajorTicks[], LONG vclrMinorTicks[] )
{
    BOOL f ;
    USHORT usAngle,usTrapez ;

    /* prepare a transform to use when rotating the ticks */
    /*      cos x    - sin x    0     *\
    |       sin x      cos x    0      |
    \*      100        100      1     */

    static MATRIXLF matlfModel = {
        MAKEFIXED ( 1 , 0 ) ,       MAKEFIXED ( 0 , 0 ) ,       0L ,
        MAKEFIXED ( 0 , 0 ) ,       MAKEFIXED ( 1 , 0 ) ,       0L ,
        100L ,                      100L ,                      1L } ;

    /* define what the tick marks look like */
    static POINTL aptlMajorTick [ ] = { { -3 , 94 } , { 3 , 100 } } ;
    static BYTE   aclr [12][4] = {
            /*12*/        {SHADE,LIGHT,LIGHT,SHADE},
            /*1*/         {LIGHT,LIGHT,LIGHT,SHADE},
            /*2*/         {LIGHT,LIGHT,SHADE,SHADE},
            /*3*/         {LIGHT,LIGHT,SHADE,SHADE},
            /*4*/         {LIGHT,LIGHT,SHADE,LIGHT},
            /*5*/         {LIGHT,SHADE,SHADE,LIGHT},
            /*6*/         {LIGHT,SHADE,SHADE,LIGHT},
            /*7*/         {LIGHT,SHADE,SHADE,LIGHT},
            /*8*/         {LIGHT,SHADE,LIGHT,LIGHT},
            /*9*/         {SHADE,SHADE,LIGHT,LIGHT},
            /*9*/         {SHADE,SHADE,LIGHT,LIGHT},
            /*11*/        {SHADE,LIGHT,LIGHT,SHADE}
                                 };
    static  POINTL aptlMT  [4][4] = {
                                  { {-3,81},{-1,83},{1,83},{3,81  }} ,
                                  { {-3,81},{-1,83},{-1,87},{-3,89 }} ,
                                  { {-3,89},{-1,87},{1,87},{3,89 }} ,
                                  { {3,89},{1,87},{1,83},{3,81  }}
                                 };
    static POINTL aptlMajorTickShadow [ ] = { { -1 , 83 } , { 1 , 87  } } ;
    static POINTL aptlMinorTick [ ] = { { 0 , 83 } , { 0 , 85 } } ;

    /* have we been asked to draw the major ticks? */
    if ( usTicks & CLK_MAJORTICKS )
    {
        for ( usAngle = 0 ; usAngle < 60 ; usAngle += 5 )
        {
            /* set the rotation transform */
            matlfModel . fxM11 =
            matlfModel . fxM22 = fxSin [ ( usAngle + 15 ) % 60 ] ;
            matlfModel . fxM12 = fxSin [ ( usAngle + 30 ) % 60 ] ;
            matlfModel . fxM21 = fxSin [ usAngle ] ;
            f = GpiSetModelTransformMatrix ( hps , ( LONG ) MATLF_SIZE ,
                                             & matlfModel ,
                                             TRANSFORM_REPLACE ) ;

            /* draw a major tick mark */
            for (usTrapez = 0; usTrapez < 4; usTrapez++)
            {
                DrawTrapez(hps,aptlMT[usTrapez],vclrMajorTicks[aclr[usAngle/5][usTrapez]]);
            }
            GpiSetColor ( hps , vclrMajorTicks[SURFACE]) ;
            GpiSetCurrentPosition ( hps , & aptlMajorTickShadow [ 0 ] ) ;
            GpiBox ( hps , DRO_FILL , & aptlMajorTickShadow [ 1 ] , 0L , 0L ) ;
        }
    }

    /* have we been asked to draw the minor ticks? */
    /* draw in the default color */
    GpiSetColor ( hps ,vclrMinorTicks[SHADE]  ) ;
    if ( usTicks & CLK_MINORTICKS )
    {
        for ( usAngle = 0 ; usAngle < 60 ; usAngle ++ )
        {
            if ((usAngle % 5) != 0)
            {
                matlfModel . fxM11 =
                matlfModel . fxM22 = fxSin [ ( usAngle + 15 ) % 60 ] ;
                matlfModel . fxM12 = fxSin [ ( usAngle + 30 ) % 60 ] ;
                matlfModel . fxM21 = fxSin [ usAngle ] ;

                /* set the transform */
                f = GpiSetModelTransformMatrix ( hps , ( LONG ) MATLF_SIZE ,
                                                 & matlfModel ,
                                                 TRANSFORM_REPLACE ) ;

                /* draw a minor tick mark */
                GpiSetCurrentPosition ( hps , & aptlMinorTick [ 0 ] ) ;
                GpiBox ( hps , DRO_OUTLINEFILL , & aptlMinorTick [ 1 ] , 0L , 0L ) ;
            }
       }
    }
}
/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:DrawTrapez()
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns:
 *
 *
\****************************************************************/
void Clock:: DrawTrapez(HPS hps,POINTL *aptl,LONG color)
{
    GpiSetColor ( hps,color) ;
    GpiBeginPath(hps, 1L);                  /* start the path bracket */
    GpiSetCurrentPosition ( hps ,  aptl  ) ;
    GpiPolyLine(hps, 3L,&(aptl[1]) );      /* draw the three sides   */
    GpiCloseFigure(hps);                    /* close the triangle     */
    GpiEndPath(hps);                        /* end the path bracket   */
    GpiFillPath(hps, 1L, FPATH_ALTERNATE);  /* draw and fill the path */
}
/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:DrawFullRing()
 *
 *  Purpose: This routine draws the ring for the clock face.
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns:
 *
 *
\****************************************************************/
void Clock:: DrawFullRing(HPS hps,PPOINTL pptlCenter,FIXED fxRadIn, FIXED fxRadOut,
                     LONG lColor)
{
    GpiSetColor(hps,lColor);
    GpiSetCurrentPosition ( hps , pptlCenter );
    GpiBeginPath(hps,1L);
    GpiFullArc ( hps , DRO_OUTLINE ,  fxRadIn  ) ;
    GpiFullArc ( hps , DRO_OUTLINE ,  fxRadOut  ) ;
    GpiCloseFigure(hps);
    GpiEndPath(hps);
    GpiFillPath ( hps , 1L , FPATH_ALTERNATE ) ;
}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name: SetRGBColors
 *
 *  Purpose: Set the RGB schema so that each time a user changes the
 *           color we update it here.
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns:VOID
 *
 *
\****************************************************************/
VOID Clock:: SetRGBColors()
{

    data.vclrFace[SURFACE]       = data.cp.clrFace;
    data.vclrBG[SURFACE]         = data.cp.clrBackground;
    data.vclrHands[SURFACE]      = data.cp.clrMinuteHand;
    data.vclrMajorTicks[SURFACE] = data.cp.clrHourHand;

    /*Fill color tables*/
    ShadeLight(data.vclrMajorTicks);

    data.vclrMinorTicks[SURFACE] = data.vclrFace[SURFACE];
    ShadeLight(data.vclrMinorTicks);

    ShadeLight(data.vclrFace);

    data.vclrRing[SURFACE] = data.vclrFace[SURFACE];
    ShadeLight(data.vclrRing);

    ShadeLight(data.vclrHands);
    data.vclrHands[SHADE] = RGB_BLACK;

    ShadeLight(data.vclrBG);
}
/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:ShadeLight()
 *
 *  Purpose: Find the shade and light color index values and
 *           install them in the colors   array of the element
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns:
 *
 *
\****************************************************************/
VOID Clock:: ShadeLight(PLONG nplColors)
{
   typedef  union  _RGBLONG
   {
        RGB rgb;
        LONG lng;
   } RGBLONG ;
   RGBLONG  rgbSurface,rgbShade,rgbLight;

   rgbSurface.lng = rgbShade.lng = rgbLight.lng = 0L;

   rgbSurface.lng = nplColors[SURFACE];
   rgbShade.rgb.bBlue = ShadeRGBByte(rgbSurface.rgb.bBlue);
   rgbShade.rgb.bRed = ShadeRGBByte(rgbSurface.rgb.bRed);
   rgbShade.rgb.bGreen = ShadeRGBByte(rgbSurface.rgb.bGreen);
   rgbLight.rgb.bBlue = LightRGBByte(rgbSurface.rgb.bBlue);
   rgbLight.rgb.bRed = LightRGBByte(rgbSurface.rgb.bRed);
   rgbLight.rgb.bGreen = LightRGBByte(rgbSurface.rgb.bGreen);
   nplColors[SHADE] = rgbShade.lng;
   nplColors[LIGHT] = rgbLight.lng;
}


/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:ShadeRGBByte
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns:
 *
 *
\****************************************************************/

BYTE Clock:: ShadeRGBByte(BYTE brgb)
{
  #define SHADER   ( (BYTE) 0x50)

  if (brgb > SHADER)
  {
     return (brgb - SHADER);
  }
  else
  {
     return (0);
  }

}
/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:LightRGBByte
 *
 *  Purpose:
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns:
 *
 *
\****************************************************************/
BYTE Clock:: LightRGBByte(BYTE brgb)
{

  #define LIGHTER  ( (BYTE) 0x40)

  if (brgb < (0xff - LIGHTER) )
  {
     return (brgb + LIGHTER);
  }

  else return (0xff);

}

/****************************************************************\
 *
 *--------------------------------------------------------------
 *
 *  Name:  FillRect
 *
 *  Purpose: Fills background space when win calls cannot be made
 *
 *
 *
 *  Usage:
 *
 *  Method:
 *
 *  Returns:
 *
 *
\****************************************************************/
void Clock::FillRect(HPS hpsDraw, RECTL & rcl)
{

  long lColor = GpiQueryColor(hpsDraw);
  GpiSetColor(hpsDraw, data.cp.clrBackground);
  POINTL ptl1, ptl2;
  ptl1.x = rcl.xLeft;
  ptl1.y = rcl.yBottom;
  ptl2.x = rcl.xRight;
  ptl2.y = rcl.yTop;
  GpiMove(hpsDraw, &ptl1);
  GpiBox(hpsDraw, DRO_FILL, &ptl2, 0, 0);
  GpiSetColor(hpsDraw, lColor);
}

void Clock::SetMode(USHORT dispMode)
{
  data.cp.usDispMode = dispMode;
  data.fBufferDirty = TRUE;
}
