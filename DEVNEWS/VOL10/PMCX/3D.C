#pragma	title("Shadowed Frame Control  --  Version 1.0 -- (3D.C)")
#pragma	subtitle("  Shadowed Frame Control DLL - Interface Definitions")

#pragma	info(noext)

#define	INCL_DOS		   /* Include OS/2 DOS Kernal		*/
#define	INCL_GPI		   /* Include OS/2 PM GPI Interface	*/
#define	INCL_WIN		   /* Include OS/2 PM Windows Interface	*/

#include <malloc.h>
#include <os2.h>
#include <string.h>

#include <pmcx.h>

/* This	module contains	an example of installable controls that	can be	*/
/* used	within applications where additional facilities	are provided	*/
/* that	are not	found within the default controls of OS/2 PM.  The	*/
/* difference of this example from the other PMCX examples is that this	*/
/* particular module contains only the control procedures which	would	*/
/* ultimately be shipped with an application.  The PMCX	interface	*/
/* for each control is contained within	separate DLLs.			*/
/*									*/
/* The ultimate	purpose	is to show how to create one DLL for the	*/
/* controls while still	allowing PMCX interfaces.			*/
/*									*/
/* For complete	details	regarding the PM Control Extensions (PMCX)	*/
/* consult the User's Guide.                                            */
/*									*/
/* The DLL is created using the	following command line invocation:	*/
/*									*/
/*     Icc -G3e- -O+ -Rn -W3 -C	3D.C					*/

/* Filename:   3D.C							*/

/*  Version:   1.0							*/
/*  Created:   1995-12-11						*/
/*  Revised:   1995-12-11						*/

/* Routines:   BOOL EXPENTRY RegisterFrame(HAB hAB);			*/
/*	       BOOL EXPENTRY RegisterLine3D(HAB	hAB);			*/
/*	       BOOL EXPENTRY RegisterText3D(HAB	hAB);			*/
/*	       static VOID CalcFrameSize(PRECTL	prcl, PSRECT psrect);	*/
/*	       static VOID CalcLineSize(PRECTL prcl, PSRECT psrect);	*/
/*	       static VOID SizeText(HWND hWnd, PTEXTFIELD ptf);		*/
/*	       static VOID SetDefaultColours(HWND hWnd,	PTEXTFIELD ptf);*/
/*	       static LONG lGetPresParam(HWND hWnd, ULONG ulID1,	*/
/*					 ULONG ulID2, LONG lDefault);	*/
/*	       static VOID CalcFrameSize(PRECTL	prcl, PSRECT psrect);	*/
/*	       static VOID CalcLineSize(HWND hWnd, PRECTL prcl,		*/
/*					PLINEFIELD plf);		*/
/*	       MRESULT EXPENTRY	FrameWndProc(HWND hWnd,	ULONG msg,	*/
/*					     MPARAM mp1, MPARAM	mp2);	*/
/*	       MRESULT EXPENTRY	Line3DWndProc(HWND hWnd, ULONG msg,	*/
/*					      MPARAM mp1, MPARAM mp2);	*/
/*	       MRESULT EXPENTRY	Text3DWndProc(HWND hWnd, ULONG msg,	*/
/*					      MPARAM mp1, MPARAM mp2);	*/


/* Copyright ¸ 1989-1995  Prominare Inc.  All Rights Reserved.		*/

/* --------------------------------------------------------------------	*/

/************************************************************************/
/************************************************************************/
/*		       DISCLAIMER OF WARRANTIES.			*/
/************************************************************************/
/************************************************************************/
/*     The following [enclosed]	code is	library	code created by		*/
/*     Prominare Inc.  This library code is  provided to you solely	*/
/*     for the purpose of assisting you	in the development of your	*/
/*     applications.  The code is provided "AS IS", without		*/
/*     warranty	of any kind.  Prominare	Inc. shall not be liable	*/
/*     for any damages arising out of your use of the library code,	*/
/*     even if they have been advised of the possibility of such	*/
/*     damages.								*/
/************************************************************************/
/************************************************************************/

/* --- Window Information Structures ----------------------------------	*/

typedef	struct _SRECT		   /* srect */
   {
   POINTL aptl[2];		   /* Group Box	Outline	Points		*/
   POINTL aptlShadow[2];	   /* Group Box	Outline	Points		*/
   LONG	  lClrBackground;	   /* Background Colour			*/
   LONG	  lClrShadow;		   /* Shadow Colour			*/
   BOOL	  fDraw;		   /* Draw Flag				*/
   } SRECT ;

typedef	SRECT *PSRECT;

/* --- Window Information Structures ----------------------------------	*/


typedef	struct _LINEFIELD	   /* lf */
   {
   HWND	  hwndOwner;		   /* Owner Window Handle		*/
   POINTL aptl[4];		   /* Entry Field Outline Points	*/
   } LINEFIELD ;

typedef	LINEFIELD *PLINEFIELD;

/* --- Styles ---------------------------------------------------------	*/

#define	LNS_VERTICAL	  0x0001L
#define	LNS_HORIZONTAL	  0x0002L

/* --- Control Data Structures ----------------------------------------	*/


typedef	struct _TEXTFIELD	   /* tf */
   {
   POINTL aptl[4];		   /* Entry Field Outline Points	*/
   RECTL  rcl;			   /* Frame Rectangle			*/
   LONG	  lClrLeftTop;		   /* Left and Top Edge	Colour		*/
   LONG	  lClrBottomRight;	   /* Right and	Bottom Edge Colour	*/
   BOOL	  fRaised;		   /* Raised Field Flag			*/
   LONG	  lClrText;		   /* Text Colour			*/
   LONG	  lClrBackground;	   /* Background Colour			*/
   LONG	  lClrBorder;		   /* Border Colour			*/
   LONG	  cText;		   /* Text Length Holder		*/
   PSZ	  pszText;		   /* Text Holder			*/
   ULONG  flFormat;		   /* Formatting Flags			*/
   ULONG  flStyle;		   /* Style Flags			*/
   BOOL	  fDraw;		   /* Draw Flag				*/
   POINTL ptlDraw;		   /* Text Drawing Point		*/
   } TEXTFIELD ;

typedef	TEXTFIELD *PTEXTFIELD;

/* --- Styles ---------------------------------------------------------	*/

#define	TS_DEPRESSED	  0x0000L
#define	TS_RAISED	  0x0100L

#define	DS_DEPRESSED	  0x0000L
#define	DS_RAISED	  0x0040L

#define	DT_MASK	(DT_LEFT | DT_CENTER | DT_RIGHT	| DT_TOP | DT_VCENTER |	DT_BOTTOM | DT_HALFTONE	| DT_MNEMONIC |	DT_WORDBREAK)

/* --- Module Prototype	Definitions -----------------------------------	*/

static VOID SizeText(HWND hWnd,	PTEXTFIELD ptf);
static VOID SetFrameDefaultColours(HWND	hWnd, PSRECT psrect);
static VOID SetDefaultColours(HWND hWnd, PTEXTFIELD ptf);
static LONG lGetPresParam(HWND hWnd, ULONG ulID1, ULONG	ulID2, LONG lDefault);
static VOID CalcFrameSize(PRECTL prcl, PSRECT psrect);
static VOID CalcLineSize(HWND hWnd, PRECTL prcl, PLINEFIELD plf);

BOOL	EXPENTRY RegisterLine3D(HAB hAB);
BOOL	EXPENTRY RegisterFrame(HAB hAB);
BOOL	EXPENTRY RegisterText3D(HAB hAB);

MRESULT	EXPENTRY FrameWndProc(HWND hWnd, ULONG msg, MPARAM mp1,	MPARAM mp2);
MRESULT	EXPENTRY Line3DWndProc(HWND hWnd, ULONG	msg, MPARAM mp1, MPARAM	mp2);
MRESULT	EXPENTRY Text3DWndProc(HWND hWnd, ULONG	msg, MPARAM mp1, MPARAM	mp2);

#pragma	subtitle("   Shadowed Frame Control - Control Initialization Function")
#pragma	page ( )

/* --- RegisterFrame ----------------------------------- [ Public ] ---	*/
/*									*/
/*     This function is	used to	register the installable control class	*/
/*     with OS/2 Presentation Manager.	The registration must use the	*/
/*     USER_CWINDOWWORDS to reserve memory for the control to allow for	*/
/*     proper usage by Resource	Editor and for use by the control	*/
/*     dialog and window procedures.  The information for the control	*/
/*     containing the style, presentation parameters and control data	*/
/*     is pointed to by	a pointer that can be referenced by the		*/
/*     control's dialog and window procedure as required.  The memory   */
/*     for the structure is allocated and controlled through Resource	*/
/*     Editor.	 The control can reserve more memory for its use	*/
/*     by adding the memory required to	that of	the USER_CWINDOWWORDS	*/
/*     constant.							*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HAB hAB;	= Application Anchor Block Handle			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     RegisterFrame =	TRUE : Class Registration Successful		*/
/*		     = FALSE : Class Registration Failed		*/
/*									*/
/* --------------------------------------------------------------------	*/

BOOL EXPENTRY RegisterFrame(HAB	hAB)

{
		       /* Register the control class with OS/2		*/
		       /* Presentation Manager and return registration	*/
		       /* result					*/

return(WinRegisterClass(hAB, "3D.Frame", (PFNWP)FrameWndProc, CS_SYNCPAINT | CS_HITTEST	| CS_PARENTCLIP	| CS_SIZEREDRAW,
			USER_CWINDOWWORDS));

}
#pragma	subtitle("   3D Line Control DLL - Control Initialization Function")
#pragma	page ( )

/* --- RegisterLine3D ---------------------------------- [ Public ] ---	*/
/*									*/
/*     This function is	used to	register the installable control class	*/
/*     with OS/2 Presentation Manager.	The registration must use the	*/
/*     USER_CWINDOWWORDS to reserve memory for the control to allow for	*/
/*     proper usage by Resource	Editor and for use by the control	*/
/*     dialog and window procedures.  The information for the control	*/
/*     containing the style, presentation parameters and control data	*/
/*     is pointed to by	a pointer that can be referenced by the		*/
/*     control's dialog and window procedure as required.  The memory   */
/*     for the structure is allocated and controlled through Resource	*/
/*     Editor.	 The control can reserve more memory for its use	*/
/*     by adding the memory required to	that of	the USER_CWINDOWWORDS	*/
/*     constant.							*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HAB hAB;	= Application Anchor Block Handle			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     RegisterLine3D =	 TRUE :	Class Registration Successful		*/
/*		      =	FALSE :	Class Registration Failed		*/
/*									*/
/* --------------------------------------------------------------------	*/

BOOL EXPENTRY RegisterLine3D(HAB hAB)

{
		       /* Register the control class with OS/2		*/
		       /* Presentation Manager and return registration	*/
		       /* result					*/

return(WinRegisterClass(hAB, "3D.Line",	Line3DWndProc, CS_SYNCPAINT | CS_SIZEREDRAW, USER_CWINDOWWORDS));

}
#pragma	subtitle("   3D Text Control DLL - Control Initialization Function")
#pragma	page ( )

/* --- RegisterText3D ---------------------------------- [ Public ] ---	*/
/*									*/
/*     This function is	used to	register the installable control class	*/
/*     with OS/2 Presentation Manager.	The registration must use the	*/
/*     USER_CWINDOWWORDS to reserve memory for the control to allow for	*/
/*     proper usage by Resource	Editor and for use by the control	*/
/*     dialog and window procedures.  The information for the control	*/
/*     containing the style, presentation parameters and control data	*/
/*     is pointed to by	a pointer that can be referenced by the		*/
/*     control's dialog and window procedure as required.  The memory   */
/*     for the structure is allocated and controlled through Resource	*/
/*     Editor.	 The control can reserve more memory for its use	*/
/*     by adding the memory required to	that of	the USER_CWINDOWWORDS	*/
/*     constant.							*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HAB hAB;	= Application Anchor Block Handle			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     RegisterText3D =	 TRUE :	Class Registration Successful		*/
/*		      =	FALSE :	Class Registration Failed		*/
/*									*/
/* --------------------------------------------------------------------	*/

BOOL EXPENTRY RegisterText3D(HAB hAB)

{
		       /* Register the control class with OS/2		*/
		       /* Presentation Manager and return registration	*/
		       /* result					*/

return(WinRegisterClass(hAB, "3D.Text",	Text3DWndProc, CS_SYNCPAINT | CS_SIZEREDRAW, USER_CWINDOWWORDS));

}
#pragma	subtitle("   3D Text Control DLL - Control Window Procedure")
#pragma	page( )

/* --- lGetPresParam ----------------------------------	[ Private } ---	*/
/*									*/
/*     This function is	used to	retrieve a presentation	parameter	*/
/*     that may	be present.  If	the presentation parameter is not,	*/
/*     the default colour passed to the	function will be used.		*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND  hWnd;     = Window	Handle					*/
/*     ULONG ulID1;    = Presentation Parameter	1 ID			*/
/*     ULONG ulID2;    = Presentation Parameter	2 ID			*/
/*     LONG  lDefault; = Default Colour					*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     lGetPresParam = Colour to Use					*/
/*									*/
/* --------------------------------------------------------------------	*/

LONG lGetPresParam(HWND	hWnd, ULONG ulID1, ULONG ulID2,	LONG lDefault)

{
HPS   hPS;			   /* Presentation Space Handle		*/
LONG  lClr;			   /* Presentation Parameter Colour	*/
ULONG ulID;			   /* Presentation Parameter ID		*/

if ( WinQueryPresParam(hWnd, ulID1, ulID2, &ulID, 4UL, (PVOID)&lClr,
		       QPF_NOINHERIT | QPF_ID2COLORINDEX | QPF_PURERGBCOLOR) )
   return(lClr);
else
   if (	(lDefault >= SYSCLR_SHADOWHILITEBGND) &&
	(lDefault <= SYSCLR_HELPHILITE)	)
       return(WinQuerySysColor(HWND_DESKTOP, lDefault, 0L));
   else
       if ( (lClr = GpiQueryRGBColor(hPS = WinGetPS(hWnd),
				     LCOLOPT_REALIZED, lDefault)) == GPI_ALTERROR )
	   {
	   WinReleasePS(hPS);
	   return(lDefault);
	   }
       else
	   {
	   WinReleasePS(hPS);
	   return(lClr);
	   }
}
#pragma	subtitle("   Shadowed Text DLL - Control Window Sizing Procedure")
#pragma	page ( )

/* --- SizeText	---------------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	calculate the size and position		*/
/*     of the text.							*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND	  hWnd;	= Control Window Handle				*/
/*     PTEXTFIELD ptf;	= Text Information Pointer			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Nothing								*/
/*									*/
/* --------------------------------------------------------------------	*/

static VOID SizeText(HWND hWnd,	PTEXTFIELD ptf)

{
FONTMETRICS fm;			   /* Font Metrics			*/
HPS	    hPS;		   /* Presentation Space Handle		*/
POINTL	    rgptl[TXTBOX_COUNT];   /* Text Box Point Array		*/

GpiQueryFontMetrics(hPS	= WinGetPS(hWnd), sizeof(FONTMETRICS), &fm);
GpiQueryTextBox(hPS, (LONG)ptf->cText, ptf->pszText, 5L, rgptl);
WinReleasePS(hPS);

if ( ptf->flStyle & DT_RIGHT )
   ptf->ptlDraw.x = ptf->rcl.xRight - rgptl[TXTBOX_CONCAT].x;
else
   if (	ptf->flStyle & DT_CENTER )
       ptf->ptlDraw.x =	(ptf->rcl.xRight - ptf->rcl.xLeft) / 2L	- rgptl[TXTBOX_CONCAT].x / 2L +	ptf->rcl.xLeft;
   else
       ptf->ptlDraw.x =	ptf->rcl.xLeft;

if ( ptf->flStyle & DT_TOP )
   ptf->ptlDraw.y = ptf->rcl.yTop - (rgptl[TXTBOX_TOPRIGHT].y -	rgptl[TXTBOX_BOTTOMRIGHT].y) + fm.lMaxDescender;
else
   if (	ptf->flStyle & DT_VCENTER )
       ptf->ptlDraw.y =	(ptf->rcl.yTop - ptf->rcl.yBottom) / 2L	- (rgptl[TXTBOX_TOPRIGHT].y - rgptl[TXTBOX_BOTTOMRIGHT].y) / 2L	+
			fm.lMaxDescender + ptf->rcl.yBottom;
   else
       ptf->ptlDraw.y =	ptf->rcl.yBottom + fm.lMaxDescender;
}
#pragma	subtitle("   Shadowed Text DLL - Control Window Sizing Procedure")
#pragma	page ( )

/* --- CalcSize	---------------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	calculate the sizes and	positions	*/
/*     of the various elements that are	used to	make up	a shadowed	*/
/*     text field.							*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     PRECTL	  prcl;	= Control Rectangle Pointer			*/
/*     PTEXTFIELD ptf;	= Group	Box Information	Pointer			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Nothing								*/
/*									*/
/* --------------------------------------------------------------------	*/

static VOID CalcSize(HWND hWnd,	PRECTL prcl, PTEXTFIELD	ptf)

{

if ( (prcl->yTop <= prcl->yBottom) || (prcl->xRight <= prcl->xLeft) )
   ptf->fDraw =	FALSE;
else
   {
   ptf->fDraw =	TRUE;
		       /* Save the rectangle for the control		*/
   ptf->rcl = *prcl;
		       /* Adjust the rectangle size for	the text which	*/
		       /* resides inside the 3D	frame			*/
   ++ptf->rcl.xLeft;
   ++ptf->rcl.yBottom;
   ptf->rcl.yTop -= 2L;
   ptf->rcl.xRight -= 2L;

   if (	!(ptf->flStyle & DT_WORDBREAK) && ptf->cText )
       SizeText(hWnd, ptf);

	       /*  Text	display	points					*/
	       /*							*/
	       /*	  1			      2			*/
	       /*	   +-------------------------+			*/
	       /*	   |			     |			*/
	       /*	   |			     |			*/
	       /*	   |			     |			*/
	       /*	   |			     |			*/
	       /*	   +-------------------------+			*/
	       /*	  0 4			      3			*/

   ptf->aptl[0].x = prcl->xLeft;
   ptf->aptl[0].y = prcl->yTop	 - 1L;
   ptf->aptl[1].x = prcl->xRight - 1L;
   ptf->aptl[1].y = prcl->yTop	 - 1L;
   ptf->aptl[2].x = prcl->xRight - 1L;
   ptf->aptl[2].y = prcl->yBottom;
   ptf->aptl[3].x = prcl->xLeft;
   ptf->aptl[3].y = prcl->yBottom;

   ptf->flFormat = (ptf->flStyle & DT_MASK) | DT_ERASERECT;
   }
}
#pragma	subtitle("   Shadowed Text DLL - Default Colours Procedure")
#pragma	page( )

/* --- SetDefaultColours ------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	set the	default	colours	that the	*/
/*     image button should use within the internal paint routines.	*/
/*     The colour can either be	a presentation parameter that has	*/
/*     been set	or it can be the default colour	as defined within	*/
/*     control.								*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND	  hWnd;	= Window Handle					*/
/*     PTEXTFIELD ptf;	= Text Field Structure Pointer			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Nothing								*/
/*									*/
/* --------------------------------------------------------------------	*/

static VOID SetDefaultColours(HWND hWnd, PTEXTFIELD ptf)

{
		       /* Set up the colours that will be used within	*/
		       /* the painting of the control.			*/

ptf->lClrText	    = lGetPresParam(hWnd, PP_FOREGROUNDCOLOR, PP_FOREGROUNDCOLORINDEX, SYSCLR_OUTPUTTEXT);
ptf->lClrBackground = lGetPresParam(hWnd, PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLORINDEX, SYSCLR_DIALOGBACKGROUND);
ptf->lClrBorder	    = lGetPresParam(hWnd, PP_BORDERCOLOR,     PP_BORDERCOLORINDEX,     SYSCLR_BUTTONDARK);
}
#pragma	subtitle("   Shadowed Text DLL - 3D Text Window Procedure")
#pragma	page ( )

/* --- DrawText	---------------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	draw the text and 3D outline.		*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HPS	  hPS; = Window	Presentation Space Handle		*/
/*     PTEXTFIELD ptf; = Text Field Pointer				*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     hpsDrawText = Window Presentation Space Handle			*/
/*									*/
/* --------------------------------------------------------------------	*/

static HPS hpsDrawText(HPS hPS,	PTEXTFIELD ptf)

{
FONTMETRICS fm;			   /* Font Metrics Holder		*/
RECTL	    rcl;		   /* Rectangle	Holder			*/
register LONG i, k, n;		   /* String Length Counter		*/

		       /* Get the presentation space for the control	*/
		       /* and set the colour table to RGB mode		*/

GpiCreateLogColorTable(hPS, 0L,	LCOLF_RGB, 0L, 0L, (PLONG)NULL);

		       /* Check	to see if any text present and if the	*/
		       /* case,	draw it	within the rectangle		*/
if ( ptf->cText	)
		       /* Check	to see if the text is to be broken over	*/
		       /* more than one	line if	the length of the text	*/
		       /* is greater than the width of the control	*/

   if (	ptf->flStyle & DT_WORDBREAK )
       {
		       /* Word break style specified, set the drawing	*/
		       /* of the text within a loop such that it can	*/
		       /* be drawn on successive lines			*/
       n = ptf->cText;
       GpiQueryFontMetrics(hPS,	sizeof(FONTMETRICS), &fm);
       rcl = ptf->rcl;
       i = 0;
       do
	   {
	   n -=	(k = WinDrawText(hPS, n, &ptf->pszText[i], &rcl, ptf->lClrText,	ptf->lClrBackground, ptf->flFormat));
	   i +=	k;
	   if (	(rcl.yTop -= fm.lMaxBaselineExt) < rcl.yBottom )
	       break;
	   } while ( n > 0 );
       }
   else
       {
       GpiSetColor(hPS,	ptf->lClrText);
       GpiSetBackColor(hPS, ptf->lClrBackground);
       GpiCharStringPosAt(hPS, &ptf->ptlDraw, &ptf->rcl, CHS_OPAQUE | CHS_CLIP,	ptf->cText, ptf->pszText, NULL);
       }
else
   WinFillRect(hPS, &ptf->rcl, ptf->lClrBackground);

GpiSetColor(hPS, ptf->lClrLeftTop);

GpiMove(hPS, &ptf->aptl[3]);
GpiPolyLine(hPS, 2L, ptf->aptl);

GpiSetColor(hPS, ptf->lClrBottomRight);
GpiPolyLine(hPS, 2L, &ptf->aptl[2]);

return(hPS);
}
#pragma	subtitle("   Shadowed Group Box DLL - Control Window Procedure")
#pragma	page ( )

/* --- CalcLineSize -----------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	calculate the sizes and	positions	*/
/*     of the various elements that are	used to	make up	a 3D text	*/
/*     field.								*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND	  hWnd;	= Window Handle					*/
/*     PRECTL	  prcl;	= Control Rectangle Pointer			*/
/*     PLINEFIELD ptf;	= Text Field Information Pointer		*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Nothing								*/
/*									*/
/* --------------------------------------------------------------------	*/

static VOID CalcLineSize(HWND hWnd, PRECTL prcl, PLINEFIELD plf)

{
	       /*  Line	display	points					*/
	       /*							*/
	       /*	  0			      1			*/
	       /*	   ===========================			*/
	       /*	  2			      3			*/

if ( WinQueryWindowULong(hWnd, QWL_STYLE) & LNS_VERTICAL )
   {
   plf->aptl[0].x = plf->aptl[1].x = (prcl->xRight - prcl->xLeft) / 2L +
				      prcl->xLeft;
   plf->aptl[2].x = plf->aptl[3].x = plf->aptl[0].x + 1L;
   plf->aptl[0].y = plf->aptl[2].y = prcl->yBottom;
   plf->aptl[1].y = plf->aptl[3].y = prcl->yTop	  - 1L;
   }
else
   {
   plf->aptl[0].x = plf->aptl[2].x = prcl->xLeft;
   plf->aptl[1].x = plf->aptl[3].x = prcl->xRight;
   plf->aptl[0].y = plf->aptl[1].y = (prcl->yTop - prcl->yBottom) / 2L +
				      prcl->yBottom;
   plf->aptl[2].y = plf->aptl[3].y = plf->aptl[0].y - 1L;
   }
}
#pragma	subtitle("   Shadowed Frame DLL - Control Window Procedure")
#pragma	page ( )

/* --- CalcFrameSize ----------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	calculate the sizes and	positions	*/
/*     of the various elements that are	used to	make up	a shadowed	*/
/*     frame.								*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     PRECTL prcl;   =	Control	Rectangle Pointer			*/
/*     PSRECT psrect; =	Frame Information Pointer			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Nothing								*/
/*									*/
/* --------------------------------------------------------------------	*/

static VOID CalcFrameSize(PRECTL prcl, PSRECT psrect)

{
if ( (prcl->xRight <= prcl->xLeft) || (prcl->yTop <= prcl->yBottom) )
   psrect->fDraw = FALSE;
else
   {
   psrect->fDraw = TRUE;

   psrect->aptl[0].x = prcl->xLeft + 1L;
   psrect->aptl[0].y = prcl->yBottom;
   psrect->aptl[1].x = prcl->xRight - 1L;
   psrect->aptl[1].y = prcl->yTop - 2L;

   psrect->aptlShadow[0].x = prcl->xLeft;
   psrect->aptlShadow[0].y = prcl->yBottom + 1L;
   psrect->aptlShadow[1].x = prcl->xRight - 2L;
   psrect->aptlShadow[1].y = prcl->yTop	- 1L;
   }
}
#pragma	subtitle("   Shadowed Frame DLL - Default Colours Procedure")
#pragma	page( )

/* --- SetFrameDefaultColours -------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	set the	default	colours	that the	*/
/*     image button should use within the internal paint routines.	*/
/*     The colour can either be	a presentation parameter that has	*/
/*     been set	or it can be the default colour	as defined within	*/
/*     control.								*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND   hWnd;   =	Window Handle					*/
/*     PSRECT psrect; =	Frame Information Pointer			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Nothing								*/
/*									*/
/* --------------------------------------------------------------------	*/

static VOID SetFrameDefaultColours(HWND	hWnd, PSRECT psrect)

{
		       /* Set up the colours that will be used within	*/
		       /* the painting of the control.			*/

psrect->lClrBackground = lGetPresParam(hWnd, PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLORINDEX, SYSCLR_DIALOGBACKGROUND);
psrect->lClrShadow     = lGetPresParam(hWnd, PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLORINDEX, SYSCLR_SHADOW);
}
#pragma	subtitle("   Shadowed Frame DLL - Draw Frame Procedure")
#pragma	page ( )

/* --- DrawText	---------------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	draw the 3D outline.			*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HPS    hPS;    =	Window Presentation Space Handle		*/
/*     PSRECT psrect; =	Frame Information Pointer			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Nothing								*/
/*									*/
/* --------------------------------------------------------------------	*/

static HPS hpsDrawFrame(HPS hPS, PSRECT	psrect)

{
		       /* Get the presentation space for the control	*/
		       /* and set the colour table to RGB mode		*/

GpiCreateLogColorTable(hPS, 0L,	LCOLF_RGB, 0L, 0L, (PLONG)NULL);

		       /* Set the colours for the drawing of the text	*/
		       /* and group box	frame				*/

GpiSetColor(hPS, RGB_WHITE);

		       /* Draw the group box frame			*/

GpiMove(hPS, psrect->aptl);
GpiBox(hPS, DRO_OUTLINE, &psrect->aptl[1], 0L, 0L);

		       /* Set the colours for the drawing of the text	*/
		       /* and group box	frame				*/

GpiSetColor(hPS, psrect->lClrShadow);

		       /* Draw the group box frame			*/

GpiMove(hPS, psrect->aptlShadow);
GpiBox(hPS, DRO_OUTLINE, &psrect->aptlShadow[1], 0L, 0L);

return(hPS);
}
#pragma	subtitle("   Shadowed Frame DLL - Control Window Procedure")
#pragma	page ( )

/* --- FrameWndProc -----------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	handle the messages sent to the		*/
/*     installed control.  The window procedure	is designed to		*/
/*     allow for multiple instances and	to be totally re-entrant.	*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND   hWnd; = Window Handle					*/
/*     ULONG  msg;  = PM Message					*/
/*     MPARAM mp1;  = Message Parameter	1				*/
/*     MPARAM mp2;  = Message Parameter	2				*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     FrameWndProc = Message Handling Result				*/
/*									*/
/* --------------------------------------------------------------------	*/

MRESULT	EXPENTRY FrameWndProc(HWND hWnd, ULONG msg, MPARAM mp1,	MPARAM mp2)

{
PCREATESTRUCT pcrst;		   /* Create Structure Pointer		*/
PSRECT	      psrect;		   /* Group Box	Structure Pointer	*/
RECTL	      rcl;		   /* Rectangle	Holder			*/

switch ( msg )
   {
		       /* Perform control initialization when the	*/
		       /* control is created				*/
   case	WM_CREATE :
		       /* Save the address of the text string pointer	*/
		       /* in the control's reserved memory to allow it  */
		       /* to be	referenced as required by the control	*/

       WinSetWindowPtr(hWnd, QWW_CDATA,	(PVOID)(psrect = (PSRECT)malloc(sizeof(SRECT))));
       memset(psrect, 0, sizeof(SRECT));

		       /* Get the control's creation structure address  */
		       /* to copy the default text of the control to	*/
		       /* the memory in	the heap			*/

       pcrst = (PCREATESTRUCT)PVOIDFROMMP(mp2);

       SetFrameDefaultColours(hWnd, psrect);

       if ( ((rcl.xRight = pcrst->cx) >	0L) && ((rcl.yTop = pcrst->cy) > 0L) )
	   {
	   rcl.xLeft = rcl.yBottom = 0L;
	   CalcFrameSize(&rcl, psrect);
	   }
       break;
		       /* Size of control changing, reset the display	*/
		       /* points of the	line				*/
   case	WM_SIZE	:
       if ( ((LONG)(SHORT)SHORT1FROMMP(mp2) > 0L) && ((LONG)(SHORT)SHORT2FROMMP(mp2) > 0L) )
	   {
	   WinQueryWindowRect(hWnd, &rcl);
	   CalcFrameSize(&rcl, (PSRECT)WinQueryWindowPtr(hWnd, QWW_CDATA));
	   }
       else
	   {
	   psrect = (PSRECT)WinQueryWindowPtr(hWnd, QWW_CDATA);
	   psrect->fDraw = FALSE;
	   }
       break;

   case	WM_PRESPARAMCHANGED :

		       /* Check	to see if an individual	presentation	*/
		       /* parameter has	changed	if so, get the new	*/
		       /* colour value for use by the painting routines	*/

       if ( LONGFROMMP(mp1) && (LONGFROMMP(mp1)	< PP_FONTNAMESIZE) )
	   {
		       /* Get the address of the control info from the	*/
		       /* control's reserved memory                     */

	   psrect = (PSRECT)WinQueryWindowPtr(hWnd, QWW_CDATA);

		       /* Get the new presentation parameter colour for	*/
		       /* the presentation parameter that has changed.	*/
		       /* Get the colour as a RGB value	so as to be	*/
		       /* able to get an exact value and not an		*/
		       /* approximation	which could happen if the	*/
		       /* presentation parameter was set as a RGB but	*/
		       /* queried as an	index.	When WinQueryPresParam	*/
		       /* returns a 0, it indicates that no		*/
		       /* presentation parameter set and the default	*/
		       /* colours should be used.			*/

	   switch ( LONGFROMMP(mp1) )
	       {
	       case PP_BACKGROUNDCOLOR :
	       case PP_BACKGROUNDCOLORINDEX :
		   psrect->lClrBackground = lGetPresParam(hWnd,	PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLORINDEX, SYSCLR_DIALOGBACKGROUND);
		   break;

	       case PP_BORDERCOLOR :
	       case PP_BORDERCOLORINDEX	:
		   psrect->lClrShadow =	lGetPresParam(hWnd, PP_BORDERCOLOR, PP_BORDERCOLORINDEX, SYSCLR_BUTTONDARK);
		   break;
	       }
		       /* Invalidate the button	to force to use	the	*/
		       /* new colours just set or removed		*/

	   if (	psrect->fDraw )
	       WinReleasePS(hpsDrawFrame(WinGetPS(hWnd), psrect));
	   }
       else
		       /* Determine if the Scheme Palette has forced a	*/
		       /* global scheme	update in which	case, check all	*/
		       /* of the presentation parameters to see	if they	*/
		       /* have been added or removed			*/

	   if (	LONGFROMMP(mp1)	== 0L )

		       /* Set up the colours that will be used within	*/
		       /* the painting of the control.			*/

	       SetFrameDefaultColours(hWnd, (PSRECT)WinQueryWindowPtr(hWnd, QWW_CDATA));
       break;
		       /* Mouse	being passed over the control, imply	*/
		       /* that the control is transparent to the	*/
		       /* system					*/
   case	WM_HITTEST :
       return(MRFROMLONG(HT_TRANSPARENT));

		       /* Erase	control	background			*/

   case	WM_ERASEBACKGROUND :
       return(MRFROMLONG(TRUE));

		       /* Paint	the Control				*/
   case	WM_PAINT :
		       /* Get the address of the text from the		*/
		       /* control's reserved memory                     */

       psrect =	(PSRECT)WinQueryWindowPtr(hWnd,	QWW_CDATA);

		       /* Get the presentation space for the control	*/
		       /* and set the colour table to RGB mode		*/

       if ( psrect->fDraw )
	   WinEndPaint(hpsDrawFrame(WinBeginPaint(hWnd,	(HPS)NULL, (PRECTL)NULL), psrect));
       break;
		       /* Control being	destroyed			*/
   case	WM_DESTROY :
		       /* Get the address of the text from the		*/
		       /* control's reserved memory and release the     */
		       /* heap allocated for use by the	control		*/

       free((PVOID)WinQueryWindowPtr(hWnd, QWW_CDATA));
       break;
		       /* Default message processing			*/
   default :
       return(WinDefWindowProc(hWnd, msg, mp1, mp2));
   }

return(0L);
}
#pragma	subtitle("   3D Line Control DLL - Control Window Procedure")
#pragma	page ( )

/* --- Line3DWndProc --------------------------------------------------	*/
/*									*/
/*     This function is	used to	handle the messages sent to the		*/
/*     installed control.  The window procedure	is designed to		*/
/*     allow for multiple instances and	to be totally re-entrant.	*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND   hWnd; = Window Handle					*/
/*     ULONG  msg;  = PM Message					*/
/*     MPARAM mp1;  = Message Parameter	1				*/
/*     MPARAM mp2;  = Message Parameter	2				*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Line3DWndProc = Message Handling	Result				*/
/*									*/
/* --------------------------------------------------------------------	*/

MRESULT	EXPENTRY Line3DWndProc(HWND hWnd, ULONG	msg, MPARAM mp1, MPARAM	mp2)

{
HPS	      hPS;		   /* Presentation Space Handle		*/
PCREATESTRUCT pcrst;		   /* Create Structure Pointer		*/
PLINEFIELD    plf;		   /* Text Field Structure Pointer	*/
RECTL	      rcl;		   /* Rectangle	Holder			*/

switch ( msg )
   {
		       /* Perform control initialization when the	*/
		       /* control is created				*/
   case	WM_CREATE :
		       /* Create a heap	for the	control	using a		*/
		       /* segment					*/

       memset(plf = (PLINEFIELD)malloc(sizeof(LINEFIELD)), 0, sizeof(LINEFIELD));

		       /* Save the address of the text string pointer	*/
		       /* in the control's reserved memory to allow it  */
		       /* to be	referenced as required by the control	*/

       WinSetWindowPtr(hWnd, QWW_CDATA,	(PVOID)plf);

		       /* Get the control's creation structure address  */
		       /* to copy the default text of the control to	*/
		       /* the memory in	the heap			*/

       pcrst = (PCREATESTRUCT)PVOIDFROMMP(mp2);

       plf->hwndOwner =	pcrst->hwndOwner;

       rcl.xLeft   =\
       rcl.yBottom = 0L;
       rcl.xRight  = pcrst->cx;
       rcl.yTop	   = pcrst->cy;
       CalcLineSize(hWnd, &rcl,	plf);
       break;

   case	WM_SIZE	:
       WinQueryWindowRect(hWnd,	&rcl);
       CalcLineSize(hWnd, &rcl,	(PLINEFIELD)WinQueryWindowPtr(hWnd, QWW_CDATA));
       break;

   case	WM_HITTEST :
       return(MRFROMLONG(HT_TRANSPARENT));

		       /* Erase	control	background			*/

   case	WM_ERASEBACKGROUND :
       return(MRFROMLONG(TRUE));

		       /* Paint	the Control				*/
   case	WM_PAINT :
		       /* Get the address of the text from the		*/
		       /* control's reserved memory                     */

       plf = (PLINEFIELD)WinQueryWindowPtr(hWnd, QWW_CDATA);

       GpiSetColor(hPS = WinBeginPaint(hWnd, (HPS)NULL,	(PRECTL)NULL), SYSCLR_SHADOW);

       GpiMove(hPS, plf->aptl);
       GpiLine(hPS, &plf->aptl[1]);

       GpiSetColor(hPS,	CLR_WHITE);

       GpiMove(hPS, &plf->aptl[2]);
       GpiLine(hPS, &plf->aptl[3]);

       WinEndPaint(hPS);
       break;
		       /* Control being	destroyed			*/
   case	WM_DESTROY :
		       /* Get the address of the text from the		*/
		       /* control's reserved memory and release the     */
		       /* memory allocated for the use of the control	*/

       free((PVOID)WinQueryWindowPtr(hWnd, QWW_CDATA));
       break;
		       /* Default message processing			*/
   default :
       return(WinDefWindowProc(hWnd, msg, mp1, mp2));
   }

return(0L);
}
#pragma	subtitle("   Shadowed Text DLL - 3D Text Window Procedure")
#pragma	page ( )

/* --- Text3DWndProc ----------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	handle the messages sent to the		*/
/*     installed control.  The window procedure	is designed to		*/
/*     allow for multiple instances and	to be totally re-entrant.	*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND   hWnd; = Window Handle					*/
/*     ULONG  msg;  = PM Message					*/
/*     MPARAM mp1;  = Message Parameter	1				*/
/*     MPARAM mp2;  = Message Parameter	2				*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Text3DWndProc = Message Handling	Result				*/
/*									*/
/* --------------------------------------------------------------------	*/

MRESULT	EXPENTRY Text3DWndProc(HWND hWnd, ULONG	msg, MPARAM mp1, MPARAM	mp2)

{
LONG	       lClr;		   /* Presentation Parameter Colour	*/
PCREATESTRUCT  pcrst;		   /* Create Structure Pointer		*/
PSWP	       pswp;		   /* Size/Window Position Pointer	*/
PTEXTFIELD     ptf;		   /* Text Field Structure Pointer	*/
PWNDPARAMS     pwprm;		   /* Window Parameters	Pointer		*/
RECTL	       rcl;		   /* Rectangle	Holder			*/
ULONG	       flStyle;		   /* Control Style			*/
ULONG	       ulID;		   /* Presentation Parameter ID		*/

switch ( msg )
   {
		       /* Perform control initialization when the	*/
		       /* control is created				*/
   case	WM_CREATE :
		       /* Save the address of the text string pointer	*/
		       /* in the control's reserved memory to allow it  */
		       /* to be	referenced as required by the control	*/

       WinSetWindowPtr(hWnd, QWW_CDATA,	(PVOID)(ptf = (PTEXTFIELD)malloc(sizeof(TEXTFIELD))));
       memset(ptf, 0, sizeof(TEXTFIELD));

		       /* Get the control's creation structure address  */
		       /* to copy the default text of the control to	*/
		       /* the memory in	the heap			*/

       pcrst = (PCREATESTRUCT)PVOIDFROMMP(mp2);

       if ( pcrst->pszText )
	   {
	   ptf->pszText	= (PSZ)malloc((ULONG)(ptf->cText = (LONG)strlen(pcrst->pszText)) + 1UL);
	   memcpy(ptf->pszText,	pcrst->pszText,	(UINT)ptf->cText);
	   }
		       /* Set up the colours that will be used within	*/
		       /* the painting of the control.			*/

       SetDefaultColours(hWnd, ptf);

		       /* Check	to see if the user provided font that	*/
		       /* should override the default font that	would	*/
		       /* be set					*/

       if ( !WinQueryPresParam(hWnd, PP_FONTNAMESIZE, 0L, &ulID, 4L, (PVOID)&lClr, QPF_NOINHERIT) )

		       /* System indicates not set since not data was	*/
		       /* returned, therefore set default font for the	*/
		       /* control					*/

	   WinSetPresParam(hWnd, PP_FONTNAMESIZE, 8L, (PVOID)"10.Helv");

       if ( (ptf->flStyle = pcrst->flStyle) & DS_RAISED	)
	   {
	   ptf->fRaised	= TRUE;
	   ptf->lClrLeftTop	= RGB_WHITE;
	   ptf->lClrBottomRight	= ptf->lClrBorder;
	   }
       else
	   {
	   ptf->fRaised	= FALSE;
	   ptf->lClrLeftTop	= ptf->lClrBorder;
	   ptf->lClrBottomRight	= RGB_WHITE;
	   }

       if ( ((rcl.xRight = pcrst->cx) >	0L) && ((rcl.yTop = pcrst->cy) > 0L) )
	   {
	   rcl.xLeft = rcl.yBottom = 0L;
	   CalcSize(hWnd, &rcl,	ptf);
	   }
       break;

   case	WM_ADJUSTWINDOWPOS :
       pswp = (PSWP)PVOIDFROMMP(mp1);
       if ( (pswp->fl &	SWP_SIZE) && (pswp->cx > 0) && (pswp->cy > 0) )

		       /* Get the address of the control info from the	*/
		       /* control's reserved memory and save the new    */
		       /* size of the control				*/
	   {
	   rcl.xLeft = rcl.yBottom = 0L;
	   rcl.xRight =	pswp->cx;
	   rcl.yTop   =	pswp->cy;
	   CalcSize(hWnd, &rcl,	(PTEXTFIELD)WinQueryWindowPtr(hWnd, QWW_CDATA));
	   }
       break;

   /*********************************************************************/
   /*  Convert position	query						*/
   /*********************************************************************/

   case	WM_QUERYCONVERTPOS :
       return(MRFROMLONG(QCP_NOCONVERT));

		       /* Control size changing, update	the rectangle	*/
		       /* points for the 3D text control		*/
   case	WM_SIZE	:
       if ( ((LONG)(SHORT)SHORT1FROMMP(mp2) > 0L) && ((LONG)(SHORT)SHORT2FROMMP(mp2) > 0L) )
	   {
	   rcl.xLeft = rcl.yBottom = 0L;
	   rcl.xRight =	(LONG)(SHORT)SHORT1FROMMP(mp2);
	   rcl.yTop   =	(LONG)(SHORT)SHORT2FROMMP(mp2);
	   CalcSize(hWnd, &rcl,	(PTEXTFIELD)WinQueryWindowPtr(hWnd, QWW_CDATA));
	   }
       else
	   {
	   ptf = (PTEXTFIELD)WinQueryWindowPtr(hWnd, QWW_CDATA);
	   ptf->fDraw =	FALSE;
	   }
       break;
		       /* Process window parameters setting		*/

   case	WM_SETWINDOWPARAMS :

		       /* Get the address for the windows parameters	*/
		       /* structure					*/

       pwprm = (PWNDPARAMS)PVOIDFROMMP(mp1);

		       /* Check	to see if the text for the control is	*/
		       /* being	set					*/

       if ( pwprm->fsStatus & WPM_TEXT )
	   {
		       /* Text being set, get the address of the text	*/
		       /* string stored	in the heap			*/

	   ptf = (PTEXTFIELD)WinQueryWindowPtr(hWnd, QWW_CDATA);

		       /* Check	to see if any text is being set		*/

	   if (	pwprm->cchText )
	       {
	       if ( ptf->cText )
		   ptf->pszText	= (PSZ)realloc(ptf->pszText, (ULONG)(ptf->cText	= (LONG)pwprm->cchText)	+ 1UL);
	       else
		   ptf->pszText	= (PSZ)malloc((ULONG)(ptf->cText = (LONG)pwprm->cchText) + 1UL);

		       /* Check	to make	sure that the text that	is to	*/
		       /* be set is not	greater	than the memory		*/
		       /* allocated					*/

	       memcpy(ptf->pszText, pwprm->pszText, (UINT)pwprm->cchText);
	       ptf->pszText[ptf->cText]	= (CHAR)0;
	       }
	   else
	       {
	       if ( ptf->cText )
		   free(ptf->pszText);

		       /* No text is being set,	clear any existing text	*/

	       ptf->cText = 0L;
	       }

	   if (	ptf->fDraw )
	       {
	       if ( !(ptf->flStyle & DT_WORDBREAK) && ptf->cText )
		  SizeText(hWnd, ptf);
	       WinReleasePS(hpsDrawText(WinGetPS(hWnd),	ptf));
	       }
	   }
       break;
		       /* Process window parameters query		*/

   case	WM_QUERYWINDOWPARAMS :

		       /* Get the address for the windows parameters	*/
		       /* structure					*/

       pwprm = (PWNDPARAMS)PVOIDFROMMP(mp1);

       if ( (pwprm->fsStatus & (WPM_TEXT | WPM_CCHTEXT)) == (WPM_TEXT |	WPM_CCHTEXT) )
	   {
		       /* Text being asked for,	get the	address	of the	*/
		       /* text string stored in	the heap		*/

	   ptf = (PTEXTFIELD)WinQueryWindowPtr(hWnd, QWW_CDATA);

		       /* Copy the text	from the string	to the		*/
		       /* structure					*/

	   memcpy(pwprm->pszText, ptf->pszText,	(UINT)(ptf->cText + 1));
	   pwprm->cchText = (ULONG)ptf->cText;
	   }
       else
	   {
	   ptf = (PTEXTFIELD)WinQueryWindowPtr(hWnd, QWW_CDATA);
	   if (	pwprm->fsStatus	& WPM_TEXT )

		       /* Copy the text	from the string	to the		*/
		       /* structure					*/

	       memcpy(pwprm->pszText, ptf->pszText, (UINT)(ptf->cText +	1));

	   if (	pwprm->fsStatus	& WPM_CCHTEXT )
	       pwprm->cchText =	(ULONG)ptf->cText;
	   }
		       /* Query	type:  get control data	length		*/

       if ( pwprm->fsStatus & WPM_CBCTLDATA )

		       /* Set the control data length to zero		*/

	   pwprm->cbCtlData = 0;
       return(MRFROMLONG(TRUE));

		       /* Presentation parameters changed, record the	*/
		       /* change internally				*/

   case	WM_PRESPARAMCHANGED :

		       /* Check	to see if an individual	presentation	*/
		       /* parameter has	changed	if so, get the new	*/
		       /* colour value for use by the painting routines	*/

       if ( LONGFROMMP(mp1) && (LONGFROMMP(mp1)	< PP_FONTNAMESIZE) )
	   {
		       /* Get the address of the control info from the	*/
		       /* control's reserved memory                     */

	   ptf = (PTEXTFIELD)WinQueryWindowPtr(hWnd, QWW_CDATA);

		       /* Get the new presentation parameter colour for	*/
		       /* the presentation parameter that has changed.	*/
		       /* Get the colour as a RGB value	so as to be	*/
		       /* able to get an exact value and not an		*/
		       /* approximation	which could happen if the	*/
		       /* presentation parameter was set as a RGB but	*/
		       /* queried as an	index.	When WinQueryPresParam	*/
		       /* returns a 0, it indicates that no		*/
		       /* presentation parameter set and the default	*/
		       /* colours should be used.			*/

	   switch ( LONGFROMMP(mp1) )
	       {
	       case PP_FOREGROUNDCOLOR :
	       case PP_FOREGROUNDCOLORINDEX :
		   ptf->lClrText = lGetPresParam(hWnd, PP_FOREGROUNDCOLOR, PP_FOREGROUNDCOLORINDEX, SYSCLR_OUTPUTTEXT);
		   break;

	       case PP_BACKGROUNDCOLOR :
	       case PP_BACKGROUNDCOLORINDEX :
		   ptf->lClrBackground = lGetPresParam(hWnd, PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLORINDEX, SYSCLR_DIALOGBACKGROUND);
		   break;

	       case PP_BORDERCOLOR :
	       case PP_BORDERCOLORINDEX	:
		   ptf->lClrBorder = lGetPresParam(hWnd, PP_BORDERCOLOR, PP_BORDERCOLORINDEX, SYSCLR_BUTTONDARK);
		   break;
	       }
	   if (	ptf->fRaised )
	       ptf->lClrBottomRight = ptf->lClrBorder;
	   else
	       ptf->lClrLeftTop	    = ptf->lClrBorder;

		       /* Invalidate the button	to force to use	the	*/
		       /* new colours just set or removed		*/

	   if (	ptf->fDraw )
	       WinReleasePS(hpsDrawText(WinGetPS(hWnd),	ptf));
	   }
       else
		       /* Determine if the Scheme Palette has forced a	*/
		       /* global scheme	update in which	case, check all	*/
		       /* of the presentation parameters to see	if they	*/
		       /* have been added or removed			*/

	   if (	LONGFROMMP(mp1)	== 0L )

		       /* Set up the colours that will be used within	*/
		       /* the painting of the control.			*/

	       SetDefaultColours(hWnd, (PTEXTFIELD)WinQueryWindowPtr(hWnd, QWW_CDATA));
       break;
		       /* Mouse	being passed over the control, imply	*/
		       /* that the control is transparent to the	*/
		       /* system					*/
   case	WM_HITTEST :
       return(MRFROMLONG(HT_TRANSPARENT));

		       /* Erase	control	background			*/

   case	WM_ERASEBACKGROUND :
       return(MRFROMLONG(TRUE));

		       /* Paint	the Control				*/
   case	WM_PAINT :
		       /* Get the address of the text from the		*/
		       /* control's reserved memory                     */

       ptf = (PTEXTFIELD)WinQueryWindowPtr(hWnd, QWW_CDATA);

		       /* Before painting, check to see	if the style of	*/
		       /* the control has changed.  When it has	changed	*/
		       /* make sure that the control internal values	*/
		       /* are updated to reflect any changes made.	*/

       if ( ((flStyle =	WinQueryWindowULong(hWnd, QWL_STYLE)) &	(DT_MASK | SS_TEXT)) !=	(ptf->flStyle &	(DT_MASK | SS_TEXT)) )
	   {
	   if (	(ptf->flStyle =	flStyle) & DS_RAISED )
	       {
	       ptf->fRaised = TRUE;
	       ptf->lClrLeftTop	    = RGB_WHITE;
	       ptf->lClrBottomRight = ptf->lClrBorder;
	       }
	   else
	       {
	       ptf->fRaised = FALSE;
	       ptf->lClrLeftTop	    = ptf->lClrBorder;
	       ptf->lClrBottomRight = RGB_WHITE;
	       }

	   WinQueryWindowRect(hWnd, &rcl);
	   CalcSize(hWnd, &rcl,	ptf);
	   }
		       /* Get the presentation space for the control	*/
		       /* and set the colour table to RGB mode		*/
       if ( ptf->fDraw )
	   WinEndPaint(hpsDrawText(WinBeginPaint(hWnd, (HPS)NULL, (PRECTL)NULL), ptf));
       break;
		       /* Control being	destroyed			*/
   case	WM_DESTROY :
		       /* Release the heap allocated for use by	the	*/
		       /* control					*/

       ptf = (PTEXTFIELD)WinQueryWindowPtr(hWnd, QWW_CDATA);
       if ( ptf->cText )
	   free(ptf->pszText);
       free(ptf);
       break;
		       /* Default message processing			*/
   default :
       return(WinDefWindowProc(hWnd, msg, mp1, mp2));
   }

return(0L);
}
