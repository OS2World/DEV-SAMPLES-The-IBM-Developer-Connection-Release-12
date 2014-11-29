/*** OS/2 Application Primer "MISC Sample Secondary Window Procedures"*/
/*                                                                    */
/* Program name : MISCEDIT.C                                          */
/* Version : 2.1 (12/24/93)                                           */
/*                                                                    */
/*    << This sample program is only for DBCS OS/2 V2.x. >>           */
/*                                                                    */
/*    Dialog procedure for a simple editor of the Sample              */
/*    Miscellenaeus program.                                          */
/*                                                                    */
/**********************************************************************/

#define INCL_WIN
#define INCL_DOS
#define INCL_GPI
#define INCL_NLS

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <limits.h>
#include <malloc.h>

#include "miscrsc.h"
#include "miscprg.h"

extern HAB hab;
extern QMSG qmsg;
extern HMQ hmq;

/*** Window Procedure for "Simple Editor " ****************************/
/*                                                                    */
/* function name : editorWinProc                                      */
/*                                                                    */
/*    Intercept WM_CHAR and converts all characters into wchar_t      */
/*    data type. Create Cursor and handle up/down/right/left          */
/*    movement. Perform character-to-character based replacement.     */
/*    Perform On-the-spot and Fixed-position conversion.              */
/*                                                                    */
/**********************************************************************/

static HPS hps;

static long countPel( LINEDATA* aLine, USHORT usStartChar );
static wchar_t createChar( MPARAM mp );
static void discardString( LINEDATA* aLine, CURPOS* pCurpos, FONTMETRICS* fm );
static void freeTextData( LINEDATA* pTop );
static long getCharWidth( HPS hps, wchar_t wc );
static LONG getFontInfo( FONTMETRICS* fm );
static BOOL isKeyUp( MPARAM mp1 );
static BOOL isChar( MPARAM mp1, MPARAM mp2 );
static BOOL isEnter( MPARAM mp1, MPARAM mp2 );
static BOOL isLeftArrow( MPARAM mp1, MPARAM mp2 );
static BOOL isRightArrow( MPARAM mp1, MPARAM mp2 );
static BOOL isUpArrow( MPARAM mp1, MPARAM mp2 );
static BOOL isDownArrow( MPARAM mp1, MPARAM mp2 );
static USHORT setBoundary( LINEDATA* aLine, CURPOS* pCurpos );
static void writeStringAt( wchar_t* wsStr, USHORT usNum, POINTL *ptl );

MRESULT EXPENTRY editorWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    HWND hwndFrame;
    HWND hwndMenu;
    static CURPOS curpos;
    static LINEDATA *currentLine = NULL;
    static LINEDATA *topLine = NULL;
    static BOOL flSpot = FALSE;
    static FONTMETRICS fm;
    static LONG lFontHeight = 0L;

    switch(msg)
    {
        case WM_COMMAND:
            hwndFrame = WinQueryWindow(hwnd, QW_PARENT);
            hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );
            switch (SHORT1FROMMP(mp1))
            {
               case MID_ONSPOT:

                  flSpot = (flSpot==TRUE)?FALSE:TRUE;
                  WinSendMsg( hwndMenu, MM_SETITEMATTR,
                              MPFROM2SHORT( MID_ONSPOT, TRUE ),
                              MPFROM2SHORT( MIA_CHECKED,
                                            (flSpot==TRUE)?MIA_CHECKED:~MIA_CHECKED ) );
                  break;
            }
            break;

        case WM_CHAR:

            if( isKeyUp( mp1 ) == TRUE )  break;

            if( isEnter( mp1, mp2 ) == TRUE )
            {
               /* set cursor to the next line */
               curpos.usLineNum++;
               curpos.usCharNum = 0;
               curpos.ptlCursor.y -= lFontHeight;
               curpos.ptlCursor.x = 0;

               /* set next line buffer as current */
               if( currentLine->next == NULL )
               {
                  currentLine->next = malloc( sizeof(LINEDATA) );
                  currentLine->next->pre = currentLine;
                  currentLine = currentLine->next;

                  memset( currentLine->wcChars, '\0', sizeof(currentLine->wcChars) );
                  memset( currentLine->lWidth, 0, sizeof(currentLine->lWidth) );
                  currentLine->next = NULL;
                  currentLine->usNumChars = 0;
               }
               else  currentLine=currentLine->next;

               /* set cursor */
               GpiMove( hps, &curpos.ptlCursor );
               WinCreateCursor( hwnd,
                                curpos.ptlCursor.x, curpos.ptlCursor.y,
                                0, 0,
                                CURSOR_SETPOS,
                                NULL );
               break;
            }

            if( isLeftArrow( mp1, mp2 ) == TRUE )
            {
               if( curpos.usCharNum < 2 )
               {
                  curpos.ptlCursor.x = 0;
                  curpos.usCharNum = 0;
               }
               else
               {
                  curpos.ptlCursor.x -= currentLine->lWidth[--curpos.usCharNum];
               }
               GpiMove( hps, &curpos.ptlCursor );
               WinCreateCursor( hwnd,
                                curpos.ptlCursor.x, curpos.ptlCursor.y,
                                0, 0,
                                CURSOR_SETPOS,
                                NULL );
               break;
            }

            if( isRightArrow( mp1, mp2 ) == TRUE )
            {
               if( curpos.usCharNum < currentLine->usNumChars )
               {
                  curpos.ptlCursor.x += currentLine->lWidth[curpos.usCharNum++];
                  GpiMove( hps, &curpos.ptlCursor );
                  WinCreateCursor( hwnd,
                                   curpos.ptlCursor.x, curpos.ptlCursor.y,
                                   0, 0,
                                   CURSOR_SETPOS,
                                   NULL );
               }
               break;
            }

            if( isUpArrow( mp1, mp2 ) == TRUE )
            {
               if( currentLine->pre != NULL )
               {
                  currentLine = currentLine->pre;

                  curpos.usCharNum = setBoundary( currentLine, &curpos );
                  curpos.usLineNum--;
                  curpos.ptlCursor.y += lFontHeight;

                  GpiMove( hps, &curpos.ptlCursor );
                  WinCreateCursor( hwnd,
                                   curpos.ptlCursor.x, curpos.ptlCursor.y,
                                   0, 0,
                                   CURSOR_SETPOS,
                                   NULL );
               }
               break;
            }

            if( isDownArrow( mp1, mp2 ) == TRUE )
            {
               if( currentLine->next != NULL )
               {
                  currentLine = currentLine->next;

                  curpos.usCharNum = setBoundary( currentLine, &curpos );
                  curpos.usLineNum++;
                  curpos.ptlCursor.y -= lFontHeight;

                  GpiMove( hps, &curpos.ptlCursor );
                  WinCreateCursor( hwnd,
                                   curpos.ptlCursor.x, curpos.ptlCursor.y,
                                   0, 0,
                                   CURSOR_SETPOS,
                                   NULL );
               }
               break;
            }

            if( isChar( mp1, mp2 ) == TRUE )
            {
               if( curpos.usCharNum < MAX_KEY_IN )
               {
                long cx;
                wchar_t wc[2];

                   wc[0] = currentLine->wcChars[curpos.usCharNum] = createChar( mp2 );

                   if( curpos.usCharNum < currentLine->usNumChars )
                   {
                      /* REPLACE */
                      WinShowCursor( hwnd, FALSE );
                      discardString( currentLine, &curpos, &fm );
                      cx = currentLine->lWidth[curpos.usCharNum] = getCharWidth( hps, wc[0] );
                      writeStringAt( &currentLine->wcChars[curpos.usCharNum],
                                     currentLine->usNumChars - curpos.usCharNum,
                                     &curpos.ptlCursor );
                      WinShowCursor( hwnd, TRUE );
                   }
                   else
                   {
                      /* APPEND */
                      wc[1] = L'\0';
                      currentLine->usNumChars++;
                      cx = currentLine->lWidth[curpos.usCharNum] = getCharWidth( hps, wc[0] );
                      writeStringAt( wc, 1, &curpos.ptlCursor );
                   }

                   /* put cursor on the next character */
                   curpos.ptlCursor.x += cx;
                   curpos.usCharNum++;
                   GpiMove( hps, &curpos.ptlCursor );
                   WinCreateCursor( hwnd,
                                    curpos.ptlCursor.x, curpos.ptlCursor.y,
                                    0, 0,
                                    CURSOR_SETPOS,
                                    NULL );
               }
               else
               {
                   /* Maximum input for a line */
                   WinAlarm( HWND_DESKTOP, WA_WARNING );
               }

               break;
            }
            break;

        case WM_QUERYCONVERTPOS:
        {
         PRECTL pCurPos;

            pCurPos = (PRECTL)PVOIDFROMMP(mp1);
            if( flSpot == TRUE )
            {
               /* set on-the-spot conversion if flag is on */
               pCurPos->xLeft   = curpos.ptlCursor.x;
               pCurPos->yBottom = curpos.ptlCursor.y;
               pCurPos->xRight  =  0;
               pCurPos->yTop    =  0;
            }
            else
            {
               /* set fixed-position conversion if flag is off */
               pCurPos->xLeft   = -1;
               pCurPos->yBottom = -1;
               pCurPos->xRight  =  0;
               pCurPos->yTop    =  0;
            }
            return (MRESULT)QCP_CONVERT;
        }

        case WM_SETFOCUS:

            if( SHORT1FROMMP(mp2)==TRUE )
            {
               if( WinCreateCursor( hwnd,
                                    curpos.ptlCursor.x, curpos.ptlCursor.y,
                                    curpos.cx, curpos.cy,
                                    CURSOR_SOLID,
                                    NULL ) == TRUE )
                   WinShowCursor( hwnd, TRUE );
            }
            else   WinDestroyCursor(hwnd);

            break;

        case WM_CREATE:
        {
         SIZEL  sizl;
         HDC    hdc;
         CHARBUNDLE cbnd;

             sizl.cx = 0L;
             sizl.cy = 0L;

             hdc = WinOpenWindowDC( hwnd );
             hps = GpiCreatePS( hab,
                                hdc,
                                (PSIZEL)&sizl,
                                (ULONG)PU_PELS | GPIT_MICRO | GPIA_ASSOC
                              );
             lFontHeight = getFontInfo( &fm );

             curpos.usCharNum = curpos.usLineNum = 0;
             curpos.ptlCursor.y = curpos.ptlCursor.x = 0;
             curpos.cx = 0;
             curpos.cy = lFontHeight;

             /* create the first line buffer */
             currentLine = topLine = malloc( sizeof(LINEDATA) );
             topLine->next = topLine->pre = NULL;
             return(MRESULT)(FALSE);
        }

        case WM_PAINT:
        {
         RECTL rectl;
         SWP   swp;
         POINTL ptlLine = {0,0};
         LINEDATA *aLine;
         int   len;
         char* p;

            WinBeginPaint( hwnd, hps, (PRECTL) &rectl );
            WinFillRect( hps, (PRECTL) &rectl, CLR_BACKGROUND );
            WinQueryWindowPos( hwnd, &swp );

            aLine = topLine;
            ptlLine.y = swp.cy;
            while( aLine != NULL )
            {
               ptlLine.y -= lFontHeight;
               writeStringAt( aLine->wcChars, aLine->usNumChars, &ptlLine );
               aLine = aLine->next;
            }

            /* set the cursor */
            curpos.ptlCursor.y = swp.cy - lFontHeight*(curpos.usLineNum+1);
            GpiMove( hps, &curpos.ptlCursor );
            WinCreateCursor( hwnd,
                             curpos.ptlCursor.x, curpos.ptlCursor.y,
                             0, 0,
                             CURSOR_SETPOS,
                             NULL );
            WinEndPaint( hps );
            break;
        }
        case WM_CLOSE:
            hwndFrame = WinQueryWindow(hwnd, QW_PARENT);
            WinDestroyCursor( hwnd );
            WinDestroyWindow( hwndFrame );
            freeTextData( topLine );
            return(MRESULT)(FALSE);

        case WM_DESTROY:
            GpiDestroyPS( hps );

        default:
            return(WinDefWindowProc(hwnd,msg,mp1,mp2));
    }
    return(FALSE);
}

/*******************************************/
/* countPel:                               */
/*  count the length of the line from the  */
/*  character specified by usStartChar till*/
/*  the end.                               */
/*******************************************/
static long countPel( LINEDATA* aLine, USHORT usStartChar )
{
 int i;
 long lWidth;

   i = aLine->usNumChars;
   lWidth = 0;
   while( i > usStartChar )
   {
      lWidth += aLine->lWidth[--i];
   }
   return( lWidth );
}

/*******************************************/
/* createChar:                             */
/*  create a wide character from a message */
/*  parameter specified by mp.             */
/*******************************************/
static wchar_t createChar( MPARAM mp )
{
 char temp[3];
 wchar_t wc;

   temp[0] = CHAR1FROMMP( mp );
   temp[1] = CHAR2FROMMP( mp );
   temp[2] = '\0';
   mbtowc( &wc, temp, MB_LEN_MAX );
   return( wc );
}

/*******************************************/
/* discardString:                          */
/*  clear the screen from the position     */
/*  pointed to by pCurpos till the end of  */
/*  the line.                              */
/*******************************************/
static void discardString( LINEDATA* aLine, CURPOS* pCurpos, FONTMETRICS* fm )
{
 int i;
 long lWidth;
 POINTL ptl;
 AREABUNDLE abnd;

   lWidth = countPel( aLine, pCurpos->usCharNum );

   abnd.lColor = CLR_BACKGROUND;
   abnd.usBackMixMode = BM_OVERPAINT;
   GpiSetAttrs( hps, PRIM_AREA, ABB_COLOR | ABB_BACK_MIX_MODE, NULLHANDLE, &abnd );
   GpiBeginArea( hps, BA_NOBOUNDARY );
   ptl.x = pCurpos->ptlCursor.x;
   ptl.y = pCurpos->ptlCursor.y - fm->lMaxDescender;
   GpiMove( hps, &ptl );
   ptl.x += lWidth;
   ptl.y += fm->lMaxBaselineExt;
   GpiBox( hps, DRO_OUTLINE, &ptl, 0, 0 );
   GpiEndArea( hps );
   GpiSetAttrs( hps, PRIM_AREA, ABB_COLOR | ABB_BACK_MIX_MODE,
                ABB_COLOR | ABB_BACK_MIX_MODE, &abnd );
}

/*******************************************/
/* freeTextData:                           */
/*  Free the buffer allocated for text     */
/*  data.                                  */
/*******************************************/
static void freeTextData( LINEDATA* pTop )
{
 LINEDATA *cur, *prev;

   cur = pTop;
   while( cur != NULL )
   {
      prev = cur;
      cur = prev->next;
      free( prev );
   }
}

/*******************************************/
/* getCharWidth:                           */
/*  calculate width of a character in pel. */
/*******************************************/
static long getCharWidth( HPS hps, wchar_t wc )
{
 char temp[3];
 int  len;
 POINTL aptl[TXTBOX_COUNT];

   len=wctomb( temp, wc );
   *(temp+len) = '\0';
   GpiQueryTextBox( hps, len, temp, TXTBOX_COUNT, aptl );
   return( aptl[3].x-aptl[1].x );
}

/******************************************/
/* getFontInfo:                           */
/*  get fontmetrics and returns height of */
/*  the current font.                     */
/******************************************/
static LONG getFontInfo( FONTMETRICS* fm )
{
     GpiQueryFontMetrics( hps, sizeof(FONTMETRICS), fm );
     return( fm->lMaxBaselineExt + fm->lExternalLeading );
}

/******************************************/
/* isxxx:                                 */
/*  Check routine for input key value.    */
/******************************************/
static BOOL isChar( MPARAM mp1, MPARAM mp2 )
{
   /* should check virtual key first */
   if( (SHORT1FROMMP( mp1 ) & KC_VIRTUALKEY) != 0 )
   {
      if( SHORT2FROMMP( mp2 ) == VK_SPACE )  return TRUE;   /* space key */
      else  return FALSE;
   }
   else return( (SHORT1FROMMP( mp1 ) & KC_CHAR) != 0 );
}

static BOOL isDownArrow( MPARAM mp1, MPARAM mp2 )
{
   if( (SHORT1FROMMP( mp1 ) & KC_VIRTUALKEY) == 0 )  return FALSE;
   else if( SHORT2FROMMP( mp2 ) == VK_DOWN )    return TRUE;
   else return FALSE;
}

static BOOL isEnter( MPARAM mp1, MPARAM mp2 )
{
   if( (SHORT1FROMMP( mp1 ) & KC_VIRTUALKEY) == 0 )  return FALSE;
   else if( SHORT2FROMMP( mp2 ) == VK_NEWLINE )    return TRUE;
   else return FALSE;
}

static BOOL isKeyUp( MPARAM mp1 )
{
   return( (SHORT1FROMMP( mp1 ) & KC_KEYUP) != 0 );
}

static BOOL isLeftArrow( MPARAM mp1, MPARAM mp2 )
{
   if( (SHORT1FROMMP( mp1 ) & KC_VIRTUALKEY) == 0 )  return FALSE;
   else if( SHORT2FROMMP( mp2 ) == VK_LEFT )    return TRUE;
   else return FALSE;
}

static BOOL isRightArrow( MPARAM mp1, MPARAM mp2 )
{
   if( (SHORT1FROMMP( mp1 ) & KC_VIRTUALKEY) == 0 )  return FALSE;
   else if( SHORT2FROMMP( mp2 ) == VK_RIGHT )    return TRUE;
   else return FALSE;
}
static BOOL isUpArrow( MPARAM mp1, MPARAM mp2 )
{
   if( (SHORT1FROMMP( mp1 ) & KC_VIRTUALKEY) == 0 )  return FALSE;
   else if( SHORT2FROMMP( mp2 ) == VK_UP )    return TRUE;
   else return FALSE;
}

/*******************************************/
/* setBoundary:                            */
/*  find the nearest character boundary to */
/*  the position pointed to by pCurpos and */
/*  reset the position to pCurpos.         */
/*******************************************/
static USHORT setBoundary( LINEDATA* aLine, CURPOS* pCurpos )
{
 int i;
 long totLen, len;

   totLen = countPel( aLine, 0 );

   if( pCurpos->ptlCursor.x > totLen )
   {
      pCurpos->ptlCursor.x = totLen;
      return( aLine->usNumChars );
   }

   for( i=aLine->usNumChars, len=totLen; len>pCurpos->ptlCursor.x; i-- )
   {
      len -= aLine->lWidth[i];
   }

   if( pCurpos->ptlCursor.x - len > len+aLine->lWidth[i+1] - pCurpos->ptlCursor.x )
   {
      pCurpos->ptlCursor.x = len+aLine->lWidth[i+1];
      return( i+2 );
   }
   else
   {
      pCurpos->ptlCursor.x = len;
      return( i+1 );
   }
}

/*******************************************/
/* writeStringAt:                          */
/*  write a wchar_t string on the position */
/*  specified by ptl.                      */
/*******************************************/
static void writeStringAt( wchar_t* wsStr, USHORT usNum, POINTL *ptl )
{
 USHORT len;
 char *p;

     len = usNum*MB_LEN_MAX;
     p = malloc( len+1 );
     len = wcstombs( p, wsStr,len );
     GpiCharStringAt( hps, ptl, len, p );
     free( p );
     return;
}

