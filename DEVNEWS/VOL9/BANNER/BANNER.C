#pragma	title("PMCX Example  --  Version 1  --  (Banner.C)")
#pragma	subtitle("   Module Purpose - Interface Definitions")


#define	INCL_DOS		   /* Include OS/2 PM DOS Interface	*/
#define	INCL_GPI		   /* Include OS/2 PM GPI Interface	*/
#define	INCL_WIN		   /* Include OS/2 PM Windows Interface	*/

#include <os2.h>

#include <string.h>
#include <pmcx.h>

#include "banner.h"

/* This	module contains	routine	used to	the routines for the PMCX	*/
/* sample control.							*/

/* Filename:   Banner.C							*/

/*  Version:   1							*/
/*  Created:   1995-08-11						*/
/*  Revised:   1995-10-03						*/

/* Routines:   ULONG _System _Dll_InitTerm(ULONG hModule, ULONG	fl);	*/
/*	       BOOL EXPENTRY BannerRegister(HAB	hAB);			*/
/*	       BOOL EXPENTRY BannerQuery(PUSERINFO pUserInfo);		*/
/*	       MRESULT EXPENTRY	BannerStyles(HWND hWnd,	ULONG msg,	*/
/*					     MPARAM mp1, MPARAM	mp2);	*/
/*	       LONG lGetPresParam(HWND hWnd, ULONG ulID1, ULONG	ulID2,	*/
/*				  LONG lDefault);			*/
/*	       VOID SetDefaultColours(HWND hWnd, PCTLINFO pctli);	*/
/*	       VOID CalcDisplayPoints(HWND hWnd, PCTLINFO pctli,	*/
/*				      LONG cx, LONG cy);		*/
/*	       LONG lSelectFont(CHAR *pszFacename, LONG	lPoints);	*/
/*	       PCTLINFO	pctliGetDataPtr(HWND hWnd);			*/
/*	       MRESULT EXPENTRY	BannerWndProc(HWND hWnd, ULONG msg,	*/
/*					      MPARAM mp1, MPARAM mp2);	*/


/* Copyright ¸ 1995  Prominare Inc.  All Rights	Reserved.		*/

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

/* --- Control Information Structure ----------------------------------	*/

typedef	struct _CTLINFO		   /* ctli */
   {
   HWND	     hWnd;		   /* Window Handle			*/
   HWND	     hwndOwner;		   /* Owner Window Handle		*/
   HWND	     hwndParent;	   /* Parent Window Handle		*/
   CHAR	     szText[512];	   /* Text Holder			*/
   LONG	     cText;		   /* Text Length			*/
   ULONG     flStyle;		   /* Control Style Flags		*/
   LONG	     lClrText;		   /* Text Colour   [Foreground]	*/
   LONG	     lClrTextShadow;	   /* Text Colour   [Shadow]		*/
   LONG	     lClrBanner;	   /* Banner Colour [Background]	*/
   LONG	     cx;		   /* Control Width			*/
   LONG	     cy;		   /* Control Height			*/
   RECTL     rcl;		   /* Display Rectangle			*/
   ARCPARAMS arcp;		   /* Arc Parameters			*/
   POINTL    ptlArc;		   /* Arc Display Points		*/
   POINTL    ptlText;		   /* Text Display Point		*/
   POINTL    ptlTextShadow;	   /* Text Shadow Display Point		*/
   FATTRS    fat;		   /* Font Attributes			*/
   SIZEF     sizfx;		   /* Font Size				*/
   } CTLINFO ;

typedef	CTLINFO	*PCTLINFO;

static GRADIENTL gradl = { 0L, 1L };

MRESULT	EXPENTRY BannerWndProc(HWND hWnd, ULONG	msg, MPARAM mp1, MPARAM	mp2);
BOOL	EXPENTRY BannerRegister(HAB hAB);
BOOL	EXPENTRY BannerQuery(PUSERINFO pUserInfo);
MRESULT	EXPENTRY BannerStyles(HWND hWnd, ULONG msg, MPARAM mp1,	MPARAM mp2);

LONG	 lGetPresParam(HWND hWnd, ULONG	ulID1, ULONG ulID2, LONG lDefault);
VOID	 SetDefaultColours(HWND	hWnd, PCTLINFO pctli);
VOID	 CalcDisplayPoints(HWND	hWnd, PCTLINFO pctli, LONG cx, LONG cy);
LONG	 lSelectFont(CHAR *pszFacename,	LONG lPoints);
PCTLINFO pctliGetDataPtr(HWND hWnd);

#pragma	subtitle("   Custom Control DLL - DLL Initialization/Termination Procedure")
#pragma	page( )

/* --- _Dll_InitTerm ----------------------------------- [ Public ] ---	*/
/*									*/
/*     This function is	used to	provide	the DLL	initialization and	*/
/*     termination.  The function is called by the C startup code	*/
/*     and allows the control to register itself and provide any	*/
/*     necessary startup.						*/
/*									*/
/*     This function is	designed for IBM C Set/2 Version 1.0,		*/
/*     IBM C Set++ Version 2.x and IBM VisualAge C++ Version 3.		*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     ULONG hModule; =	DLL Module Handle				*/
/*     ULONG fl;      =	Startup	/ Termination Flag			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     _Dll_InitTerm =	0 : Error Return				*/
/*		     =	1 : Successful Startup / Termination		*/
/*									*/
/* --------------------------------------------------------------------	*/

#pragma	info(nouse)

ULONG _System _Dll_InitTerm(ULONG hModule, ULONG fl)

{
static HMODULE hmodDLL;		   /* DLL Module Handle			*/

		       /* Determine if in startup or termination mode	*/
if ( fl	== 0 )
   {
		       /* DLL being initialized, save the DLL module	*/
		       /* handle to allow the bitmap loading routines	*/
		       /* routines a means of loading the default	*/
		       /* bitmaps when required				*/
   hmodDLL = hModule;
   }

return(1UL);
}
#pragma	info(use)
#pragma	subtitle("   PMCX Control DLL - Control Initialization Function")
#pragma	page ( )

/* --- BannerRegister ---------------------------------- [ Public ] ---	*/
/*									*/
/*     This function is	used to	register the installable control class	*/
/*     with OS/2 Presentation Manager.	The registration must use the	*/
/*     USER_CWINDOWWORDS to reserve memory for the control to allow for	*/
/*     proper usage by the Universal Resource Editor and for use by the	*/
/*     control dialogue	and window procedures.	The information	for the	*/
/*     control containing the style, presentation parameters and	*/
/*     control data is pointed to by a pointer that can	be referenced	*/
/*     by the control's dialogue and window procedure as required.  The */
/*     memory for the structure	is allocated and controlled through	*/
/*     Universal Resource Editor.   The	control	can reserve more memory	*/
/*     for its use by adding the memory	required to that of the		*/
/*     USER_CWINDOWWORDS constant.					*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HAB hAB;	= Application Anchor Block Handle			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     BannerRegister =	 TRUE :	Class Registration Successful		*/
/*		      =	FALSE :	Class Registration Failed		*/
/*									*/
/* --------------------------------------------------------------------	*/

BOOL EXPENTRY BannerRegister(HAB hAB)

{
		       /* Register the control class with OS/2		*/
		       /* Presentation Manager and return registration	*/
		       /* result					*/

return(WinRegisterClass(hAB, "Banner", BannerWndProc,
			CS_SYNCPAINT | CS_SIZEREDRAW, USER_CWINDOWWORDS));

}
#pragma	subtitle("   PMCX Control DLL - Query Control Information Function")
#pragma	page ( )

/* --- BannerQuery ------------------------------------- [ Public ] ---	*/
/*									*/
/*     This function is	used to	return to the caller information	*/
/*     regarding the installable control and its capabilities.	The	*/
/*     function	should return a	true value otherwise Universal		*/
/*     Resource	Editor will not	register the control as	being usable.	*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     PUSERINFO pUserInfo; = User Information Pointer			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     BannerQuery =  TRUE : User Information Being Returned		*/
/*		   = FALSE : No	User Information Available		*/
/*									*/
/* --------------------------------------------------------------------	*/

BOOL EXPENTRY BannerQuery(PUSERINFO pUserInfo)

{
		       /* Complete the User Information	structure	*/
		       /* passed to the	function by Universal Resource	*/
		       /* Editor					*/

		       /* Complete the version and number of control	*/
		       /* types.  In Version 1 of PMCX,	only one	*/
		       /* control type is used.				*/
pUserInfo->ulMajor = 1;
pUserInfo->ulMinor = 0;
		       /* Complete the author and control classname	*/

strcpy(pUserInfo->szAuthor,    "Prominare Inc.");
strcpy(pUserInfo->szClassname, "Banner");
strcpy(pUserInfo->szName,      "Banner");

		       /* Complete the default size and	style of the	*/
		       /* first	user control type			*/

pUserInfo->utDefined[0].cx	     = 50;
pUserInfo->utDefined[0].cy	     = 14;
pUserInfo->utDefined[0].flStyle	     = WS_VISIBLE;

		       /* Set the maximum amount of text control can	*/
		       /* accept including NULL	termination byte	*/

pUserInfo->utDefined[0].cMaxText     = CCHTEXTMAX;

		       /* Save the style's dialogue ID, type, control   */
		       /* data size and	count of style masks		*/

pUserInfo->utDefined[0].idDlg	     = DLG_BANNERSTYLES;
pUserInfo->utDefined[0].flOptions    = PMCXOPT_REFRESH | PMCXOPT_STYLECHG;
pUserInfo->utDefined[0].ulType	     = UTYPE_PRIVATE;
pUserInfo->utDefined[0].cCtlData     = 0UL;
pUserInfo->utDefined[0].flStyleType  = STYLETYPE_BITFLAGS;
pUserInfo->utDefined[0].cMasks	     = 3UL;
pUserInfo->utDefined[0].stMasks[0].flStyleMask = BNS_HORIZONTAL;
pUserInfo->utDefined[0].stMasks[0].idStyle     = IDS_BNS_HORIZONTAL;
pUserInfo->utDefined[0].stMasks[1].flStyleMask = BNS_VERTICAL;
pUserInfo->utDefined[0].stMasks[1].idStyle     = IDS_BNS_VERTICAL;
pUserInfo->utDefined[0].stMasks[2].flStyleMask = BNS_SHADOWTEXT;
pUserInfo->utDefined[0].stMasks[2].idStyle     = IDS_BNS_SHADOWTEXT;

		       /* Save the description of the control		*/

strcpy(pUserInfo->utDefined[0].szDescription, "PMCX Example");

		       /* Return the success flag back to Universal	*/
		       /* Resource Editor				*/
return(TRUE);
}
#pragma	subtitle("   PMCX Control DLL - Control Styles Dialogue Procedure")
#pragma	page ( )

/* --- BannerStyles ------------------------------------ [ Public ] ---	*/
/*									*/
/*     This function is	used for the custom control's styles dialogue   */
/*     box procedure.							*/
/*									*/
/*     When the	dialogue is invoked from the Universal Resource	Editor,	*/
/*     the address of the user style information is contained in	*/
/*     message parameter 2.  The dialogue is responsible for saving the	*/
/*     address.	 The best method to do this is to save the pointer	*/
/*     in the dialogue's reserved memory where it can be retrieved as   */
/*     needed.								*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND   hWnd; = Dialog Window Handle				*/
/*     ULONG  msg;  = PM Message					*/
/*     MPARAM mp1;  = Message Parameter	1				*/
/*     MPARAM mp2;  = Message Parameter	2				*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     BannerStyles = Message Handling Result				*/
/*									*/
/* --------------------------------------------------------------------	*/

MRESULT	EXPENTRY BannerStyles(HWND hWnd, ULONG msg, MPARAM mp1,	MPARAM mp2)

{
PUSERSTYLE   pust;		   /* User Style Pointer		*/
SWP	     swp;		   /* Screen Window Position Holder	*/

switch ( msg )
   {
		       /* Perform dialogue initialization		*/
   case	WM_INITDLG :
		       /* Save the pointer to user style information	*/
		       /* within the dialog's reserved memory           */

       WinSetWindowPtr(hWnd, QWL_USER, (PVOID)mp2);

		       /* Get the pointer to the user style information	*/

       if ( (pust = (PUSERSTYLE)mp2) !=	NULL )
	   {
		       /* Set the text,	ID symbol and value for	the	*/
		       /* control					*/

	   WinSetDlgItemText(hWnd, MLE_TEXT, pust->pszText);

	   pust->pfnSetSymbolID(hWnd, IDBX_SYMBOLID, pust);

	   if (	pust->flStyle &	BNS_VERTICAL )
	       WinSendDlgItemMsg(hWnd, RB_VERTICAL, BM_SETCHECK,
				 MPFROMSHORT(TRUE), 0L);
	   else
	       WinSendDlgItemMsg(hWnd, RB_HORIZONTAL, BM_SETCHECK,
				 MPFROMSHORT(TRUE), 0L);

	   if (	pust->flStyle &	BNS_SHADOWTEXT )
	       WinSendDlgItemMsg(hWnd, CB_SHADOWTEXT, BM_SETCHECK,
				 MPFROMSHORT(TRUE), 0L);

	   if (	pust->flStyle &	WS_VISIBLE )
	       WinSendDlgItemMsg(hWnd, CB_WS_VISIBLE, BM_SETCHECK,
				 MPFROMSHORT(TRUE), 0L);

	   if (	pust->flStyle &	WS_GROUP )
	       WinSendDlgItemMsg(hWnd, CB_WS_GROUP, BM_SETCHECK,
				 MPFROMSHORT(TRUE), 0L);

	   if (	pust->flStyle &	WS_DISABLED )
	       WinSendDlgItemMsg(hWnd, CB_WS_DISABLED, BM_SETCHECK,
				 MPFROMSHORT(TRUE), 0L);

	   if (	pust->flStyle &	WS_TABSTOP )
	       WinSendDlgItemMsg(hWnd, CB_WS_TABSTOP, BM_SETCHECK,
				 MPFROMSHORT(TRUE), 0L);
	   }
		       /* Centre dialog	on the screen			*/

       WinQueryWindowPos(hWnd, (PSWP)&swp);
       WinSetWindowPos(hWnd, HWND_TOP,
		       (WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN) - swp.cx) /	2L,
		       (WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN) - swp.cy) /	2L,
		       0L, 0L, SWP_MOVE);
       break;
		       /* Process push button selections		*/
   case	WM_COMMAND :
       switch (	SHORT1FROMMP(mp1) )
	   {
		       /* Presentation push button selected		*/

	   case	DID_PRESPARAM :

		       /* Get the pointer to the user style information	*/

	       if ( (pust = PDATAFROMDLG(hWnd))	!= NULL	)

		       /* Get the address of the look up function from	*/
		       /* user style information structure and display	*/
		       /* the dialog.  The value selected within the	*/
		       /* dialog will be automatically placed within	*/
		       /* the required entry fields			*/

		   pust->pfnGetFontClr(hWnd);
	       break;
		       /* Enter	pushbutton selected get	the definitions	*/
		       /* for the control				*/
	   case	DID_OK :

		       /* Get the pointer to the user style information	*/

	       if ( (pust = PDATAFROMDLG(hWnd))	!= NULL	)
		   {
		       /* Get the address of the CUA compliance		*/
		       /* function from	the user style information	*/
		       /* structure.  The function will	check the text	*/
		       /* for CUA compliance according to index	value	*/
		       /* selected.  A return value of TRUE from the	*/
		       /* compliance function indicates	that the text	*/
		       /* entered is acceptable.  Conversely, a	FALSE	*/
		       /* return value indicates that text is non-	*/
		       /* compliant.  In this case, the	dialog should	*/
		       /* not be exited	from and the values within the	*/
		       /* the entry fields should not be saved.		*/

       /*****************************************************************/
       /*      When your control supports text,	use the	following:	*/
       /*								*/
       /*	   if (	!pust->pfnCUACheck(hWnd, MLE_TEXT,		*/
       /*				   SAACHK_CAPS)	)		*/
       /*	       break;						*/
       /*								*/
       /*****************************************************************/

		       /* Get the address of the symbol	validation	*/
		       /* function from	the user style information	*/
		       /* structure.  The function will	validate the	*/
		       /* symbol and will check	for duplications of	*/
		       /* values.  A return value of TRUE from the	*/
		       /* validation function indicates	that the symbol	*/
		       /* and value are	acceptable.  Conversely, a	*/
		       /* FALSE	return value indicates that symbol or	*/
		       /* value	was not	acceptable.  In	this case,	*/
		       /* the dialog should not	be exited from and the	*/
		       /* values within	the entry fields should	not be	*/
		       /* saved.  The function will automatically	*/
		       /* retrieve the selected	symbol and ID from the	*/
		       /* ID Box control.				*/

		   if (	!pust->pfnGetSymbolID(hWnd, IDBX_SYMBOLID, pust) )
		       break;
		   else
		       {
		       /* Symbol and value validated, get the text of	*/
		       /* the control and save within the user style	*/
		       /* information structure	for use	by the		*/
		       /* Universal Resource Editor			*/

		       pust->cText = WinQueryDlgItemText(hWnd, MLE_TEXT,
							 CCHTEXTMAX,
							 pust->pszText);

		       /* Mask out current edit	field styles clearing	*/
		       /* selectable styles and	save new style		*/

		       pust->flStyle = 0UL;
		       if ( WinSendDlgItemMsg(hWnd, RB_VERTICAL,
					      BM_QUERYCHECK, 0L, 0L) )
			   pust->flStyle |= BNS_VERTICAL;

		       if ( WinSendDlgItemMsg(hWnd, CB_SHADOWTEXT,
					      BM_QUERYCHECK, 0L, 0L) )
			   pust->flStyle |= BNS_SHADOWTEXT;

		       /* Get the base styles				*/

		       if ( WinSendDlgItemMsg(hWnd, CB_WS_VISIBLE,
					      BM_QUERYCHECK, 0L, 0L) )
			   pust->flStyle |= WS_VISIBLE;

		       if ( WinSendDlgItemMsg(hWnd, CB_WS_GROUP,
					      BM_QUERYCHECK, 0L, 0L) )
			   pust->flStyle |= WS_GROUP;

		       if ( WinSendDlgItemMsg(hWnd, CB_WS_DISABLED,
					      BM_QUERYCHECK, 0L, 0L) )
			   pust->flStyle |= WS_DISABLED;

		       if ( WinSendDlgItemMsg(hWnd, CB_WS_TABSTOP,
					      BM_QUERYCHECK, 0L, 0L) )
			   pust->flStyle |= WS_TABSTOP;
		       }
		   }
		       /* Exit the dialogue indicating changes made	*/

	       WinDismissDlg(hWnd, TRUE);
	       break;
		       /* Cancel selected, exit	the dialogue without	*/
		       /* changing anything				*/

	   case	DID_CANCEL :
	       WinDismissDlg(hWnd, FALSE);
	       break;
	   }
       break;
		       /* Close	received, exit dialog			*/
   case	WM_CLOSE :
       WinDismissDlg(hWnd, FALSE);
       break;
		       /* Pass through unhandled messages		*/
   default :
       return(WinDefDlgProc(hWnd, msg, mp1, mp2));
   }
return(0L);
}
#pragma	subtitle("   Custom Control DLL - Presentation Parameters Lookup Procedure")
#pragma	page( )

/* --- lGetPresParam ----------------------------------	[ Private ] ---	*/
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
#pragma	subtitle("   Custom Control DLL - Default Colours Procedure")
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
/*     HWND	hWnd;  = Window	Handle					*/
/*     PCTLINFO	pctli; = Image Button Structure	Pointer			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Nothing								*/
/*									*/
/* --------------------------------------------------------------------	*/

VOID SetDefaultColours(HWND hWnd, PCTLINFO pctli)

{
		       /* Set up the colours that will be used within	*/
		       /* the painting of the control.	The colour	*/
		       /* indices are:					*/
		       /*						*/
		       /* 0 : Foreground (PP_FOREGROUND*)		*/
		       /* 1 : Background (PP_BACKGROUND*)		*/

pctli->lClrText	= lGetPresParam(hWnd, PP_FOREGROUNDCOLOR, PP_FOREGROUNDCOLORINDEX,
				CLR_WHITE);
pctli->lClrTextShadow =	lGetPresParam(hWnd, PP_SHADOW, PP_SHADOW,
				      CLR_PALEGRAY);
pctli->lClrBanner = lGetPresParam(hWnd,	PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLORINDEX,
				  CLR_BLACK);
}
#pragma	subtitle("   Custom Control DLL - Display Point Calculation Procedure")
#pragma	page ( )

/* --- CalcDisplayPoints ------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	calculate the display points for the	*/
/*     installed control.						*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND	hWnd;  = Control Window	Handle				*/
/*     PCTLINFO	pctli; = Control Data Structure	Pointer			*/
/*     LONG	cx;    = Control Width					*/
/*     LONG	cy;    = Control Height					*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Nothing								*/
/*									*/
/* --------------------------------------------------------------------	*/

VOID CalcDisplayPoints(HWND hWnd, PCTLINFO pctli, LONG cx, LONG	cy)

{
FONTMETRICS fm;			   /* Font Metrics			*/
HPS	    hPS;		   /* Presentation Space Handle		*/
POINTL	    rgptl[TXTBOX_COUNT];   /* Text Box Point Array		*/

		       /* Form the display rectangle			*/

pctli->rcl.xRight = (pctli->cx = cx);
pctli->rcl.yTop	  = (pctli->cy = cy);

		       /* Set the arc center point and arc parameters	*/

pctli->arcp.lP = (pctli->ptlArc.x = cx / 2L);
pctli->arcp.lQ = (pctli->ptlArc.y = cy / 2L);

		       /* Check	to see if the control is in vertical	*/
		       /* display mode in which	case need to		*/
		       /* calculate the	display	points for that		*/
		       /* condition instead of horizontal		*/

if ( pctli->flStyle & BNS_VERTICAL )
   {
		       /* Need to set up the display mode for the	*/
		       /* vertical text	such that the text box limits	*/
		       /* cna be properly calculated.			*/

   GpiSetCharBox(hPS = WinGetPS(hWnd), &pctli->sizfx);
   GpiCreateLogFont(hPS, (PSTR8)NULL, 4L, &pctli->fat);
   GpiSetCharSet(hPS, 4L);
   GpiSetCharAngle(hPS,	&gradl);

   GpiQueryFontMetrics(hPS, sizeof(FONTMETRICS), &fm);
   GpiQueryTextBox(hPS,	(LONG)pctli->cText, pctli->szText, 5L, rgptl);

   GpiSetCharSet(hPS, 0L);
   GpiDeleteSetId(hPS, 4L);

		       /* Calculate the	starting point for the text	*/

   pctli->ptlTextShadow.y = (pctli->ptlText.y =	cy / 2L	- rgptl[TXTBOX_CONCAT].y / 2L) + 1L;
   pctli->ptlTextShadow.x = (pctli->ptlText.x =	cx / 2L	-
			     (rgptl[TXTBOX_TOPRIGHT].x - rgptl[TXTBOX_BOTTOMRIGHT].x) /	2L -
			     fm.lMaxDescender) + 1L;
   }
else
   {
		       /* Text is horizontal, simply get the current	*/
		       /* font metrics and text	box since no changes	*/
		       /* need to be made to the character mode		*/

   GpiQueryFontMetrics(hPS = WinGetPS(hWnd), sizeof(FONTMETRICS), &fm);
   GpiQueryTextBox(hPS,	(LONG)pctli->cText, pctli->szText, 5L, rgptl);

   pctli->ptlTextShadow.x = (pctli->ptlText.x =	cx / 2L	- rgptl[TXTBOX_CONCAT].x / 2L) + 1L;
   pctli->ptlTextShadow.y = (pctli->ptlText.y =	cy / 2L	-
			     (rgptl[TXTBOX_TOPRIGHT].y - rgptl[TXTBOX_BOTTOMRIGHT].y) /	2L +
			     fm.lMaxDescender) - 1L;

   }
WinReleasePS(hPS);

}
#pragma	subtitle("   Font Functions - Image Font Selection Function")
#pragma	page( )

/* --- lSelectFont ------------------------------------- [ Public ] ---	*/
/*									*/
/*     This function is	used to	determine the lMatch component for	*/
/*     a requested image font.						*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     CHAR  *pszFacename; = Font Face Name				*/
/*     LONG  lPoints;	   = Points Size Requested			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     lSelectFont = lMatch Number for Requested Font			*/
/*									*/
/* --------------------------------------------------------------------	*/

LONG lSelectFont(CHAR *pszFacename, LONG lPoints)

{
HDC	     hDC;		   /* Display Device Context Handle	*/
HPS	     hPS;		   /* Presentation Space Handle		*/
LONG	     cFonts;		   /* Fonts Count			*/
LONG	     lFontsTotal = 0L;	   /* Fonts Total Count			*/
LONG	     lMatch = 1L;	   /* Font Match Value			*/
LONG	     lXDeviceRes;	   /* x	Device Resolution		*/
LONG	     lYDeviceRes;	   /* y	Device Resolution		*/
PFONTMETRICS pfm;		   /* Font Metrics Pointer		*/
register INT i;			   /* Loop Counter			*/

		       /* Get the height of the	screen in pels		*/

DevQueryCaps(hDC = GpiQueryDevice(hPS =	WinGetScreenPS(HWND_DESKTOP)),
	     CAPS_HORIZONTAL_FONT_RES, 1L, &lXDeviceRes);
DevQueryCaps(hDC, CAPS_VERTICAL_FONT_RES,   1L,	&lYDeviceRes);

		       /* Get the number of fonts for the face name	*/
		       /* provided					*/

cFonts = GpiQueryFonts(hPS, QF_PUBLIC, pszFacename, &lFontsTotal,
		       sizeof(FONTMETRICS), (PFONTMETRICS)NULL);

		       /* Allocate space for the font metrics for the	*/
		       /* different font sizes and devices of the font	*/

DosAllocMem((PPVOID)(PVOID)&pfm, (ULONG)(sizeof(FONTMETRICS) * cFonts),	PAG_READ | PAG_WRITE | PAG_COMMIT);

		       /* Make a pointer for the memory	allocated for	*/
		       /* the font metrics and get the font metrics for	*/
		       /* the number of	fonts for the face name		*/
		       /* provided					*/

GpiQueryFonts(hPS, QF_PUBLIC, pszFacename, &cFonts, sizeof(FONTMETRICS), pfm);

		       /* Release the presentation space acquired to	*/
		       /* determine the	screen height and to get the	*/
		       /* font metrics					*/
WinReleasePS(hPS);
		       /* Adjust the point size	to correspond to the	*/
		       /* the nominal point size that is contained	*/
		       /* within the font metrics structure		*/
lPoints	*= 10;
		       /* Loop through the font	metrics	returned to	*/
		       /* locate the desired font by matching the x and	*/
		       /* y device resolution of the font and the point	*/
		       /* size						*/

for ( i	= 0; i < (INT)cFonts; i++ )
   if (	(pfm[i].sXDeviceRes == (SHORT)lXDeviceRes) &&
	(pfm[i].sYDeviceRes == (SHORT)lYDeviceRes) &&
	((LONG)pfm[i].sNominalPointSize	== lPoints) )
       {
		       /* Font found, get the match value to allow the	*/
		       /* exact	font to	be selected by the calling	*/
		       /* application					*/

       lMatch =	pfm[i].lMatch;
       break;
       }
		       /* Release the memory allocated for the font	*/
		       /* metrics array					*/
DosFreeMem(pfm);
		       /* Return the match value to the	calling		*/
		       /* application					*/
return(lMatch);
}
#pragma	subtitle("   PDS Font Functions - Image Font Selection Function")
#pragma	page( )

/* --- pctliGetDataPtr --------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	get the	pointer	to the private control	*/
/*     data and	to check to see	if the style of	the control has		*/
/*     changed and if the case,	refresh	the internal data for the new	*/
/*     style.								*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND hWnd; = Window Handle					*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     pctliGetDataPtr = Pointer to Control Data			*/
/*									*/
/* --------------------------------------------------------------------	*/

PCTLINFO pctliGetDataPtr(HWND hWnd)

{
PCTLINFO pctli;			   /* Control Info Structure Pointer	*/
ULONG	 flStyle;		   /* Style Flag			*/

		       /* Get the pointer of the private data from the	*/
		       /* the controls window words			*/

pctli =	(PCTLINFO)WinQueryWindowPtr(hWnd, QWW_CDATA);

		       /* Check	to see if the current style of the	*/
		       /* control differs from that recorded internally	*/

if ( ((flStyle = WinQueryWindowULong(hWnd, QWL_STYLE)) & BNS_MASK) != (pctli->flStyle &	BNS_MASK) )
   {
		       /* Style	of the control has been	changed		*/
		       /* dynamically, re-calculate the	internal	*/
		       /* display points				*/
   pctli->flStyle = flStyle;
   CalcDisplayPoints(hWnd, pctli, pctli->cx, pctli->cy);
   }
		       /* Return the private data pointer		*/
return(pctli);
}
#pragma	subtitle("   Custom Control DLL - Custom Control Window Procedure")
#pragma	page ( )

/* --- BannerWndProc ----------------------------------	[ Private ] ---	*/
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
/*     BannerWndProc = Message Handling	Result				*/
/*									*/
/* --------------------------------------------------------------------	*/

MRESULT	EXPENTRY BannerWndProc(HWND hWnd, ULONG	msg, MPARAM mp1, MPARAM	mp2)

{
FONTMETRICS   fm;		   /* Font Metrics			*/
HPS	      hPS;		   /* Presentation Space Handle		*/
PCREATESTRUCT pcrst;		   /* Create Structure Pointer		*/
PCTLINFO      pctli;		   /* Control Info Structure Pointer	*/
PWNDPARAMS    pwprm;		   /* Window Parameters	Pointer		*/

switch ( msg )
   {
		       /* Perform control initialization when the	*/
		       /* control is created				*/
   case	WM_CREATE :
		       /* Allocate memory for the private control data	*/

       DosAllocMem((PPVOID)(PVOID)&pctli, 4096UL, PAG_READ | PAG_WRITE | PAG_COMMIT);

		       /* Save the address of the private control data	*/
		       /* in the control's reserved memory to allow it  */
		       /* to be	referenced as required by the control	*/

       WinSetWindowPtr(hWnd, QWW_CDATA,	(PVOID)pctli);

		       /* Get the control's creation structure address  */
		       /* to copy the default text of the control to	*/
		       /* the memory in	the heap			*/

       pcrst = (PCREATESTRUCT)PVOIDFROMMP(mp2);

       pctli->hWnd	 = hWnd;
       pctli->hwndOwner	 = pcrst->hwndOwner;
       pctli->hwndParent = pcrst->hwndParent;
       pctli->flStyle	 = pcrst->flStyle;

       pctli->fat.usRecordLength  = (USHORT)sizeof(FATTRS);
       pctli->fat.lMatch	  = lSelectFont(memcpy(pctli->fat.szFacename, "Helv", 5), 8);
       pctli->fat.usCodePage	  = (USHORT)850;
       GpiCreateLogFont(hPS = WinGetPS(hWnd), (PSTR8)NULL, 4L, &pctli->fat);
       GpiSetCharSet(hPS, 4L);

       GpiQueryFontMetrics(hPS,	sizeof(FONTMETRICS), &fm);

       pctli->sizfx.cx = MAKEFIXED(fm.lEmHeight, 0);
       pctli->sizfx.cy = MAKEFIXED(fm.lEmInc, 0);

       GpiSetCharSet(hPS, 0L);
       GpiDeleteSetId(hPS, 4L);
       WinReleasePS(hPS);

       pctli->fat.usRecordLength  = (USHORT)sizeof(FATTRS);
       pctli->fat.usCodePage	  = (USHORT)850;
       pctli->fat.fsFontUse	  = (USHORT)(FATTR_FONTUSE_OUTLINE | FATTR_FONTUSE_TRANSFORMABLE);
       memcpy(pctli->fat.szFacename, "Helvetica", 10);

		       /* Set up the colours that will be used within	*/
		       /* the painting of the control.	The colour	*/
		       /* indices are:					*/
		       /*						*/
		       /* 0 : Foreground (PP_FOREGROUND*)		*/
		       /* 1 : Background (PP_BACKGROUND*)		*/

       SetDefaultColours(hWnd, pctli);

       if ( pcrst->pszText )
	   pctli->cText	= (LONG)strlen(strcpy(pctli->szText, pcrst->pszText));

       CalcDisplayPoints(hWnd, pctli, pcrst->cx, pcrst->cy);
       break;

   case	WM_SIZE	:
       CalcDisplayPoints(hWnd, pctliGetDataPtr(hWnd),
			 (LONG)(SHORT)SHORT1FROMMP(mp2),
			 (LONG)(SHORT)SHORT2FROMMP(mp2));
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

	   pctli = pctliGetDataPtr(hWnd);

		       /* Check	to see if any text is being set		*/

	   if (	pwprm->cchText )

		       /* Check	to make	sure that the text that	is to	*/
		       /* be set is not	greater	than the memory		*/
		       /* allocated					*/

	       if ( pwprm->cchText > 511 )
		   {
		   memcpy(pctli->szText, pwprm->pszText, (UINT)(pctli->cText = 512));
		   pctli->szText[511] =	(CHAR)0;
		   }
	       else
		   memcpy(pctli->szText, pwprm->pszText, (UINT)(pctli->cText = (LONG)pwprm->cchText));
	   else
		       /* No text is being set,	clear any existing text	*/

	       pctli->szText[pctli->cText = 0] = (CHAR)0;

	   CalcDisplayPoints(hWnd, pctli, pctli->cx, pctli->cy);
	   }
       break;
		       /* Process window parameters query		*/

   case	WM_QUERYWINDOWPARAMS :

		       /* Get the address for the windows parameters	*/
		       /* structure					*/

       pwprm = (PWNDPARAMS)PVOIDFROMMP(mp1);

		       /* Query	type:  get text				*/

       if ( pwprm->fsStatus & WPM_TEXT )
	   {
		       /* Text being asked for,	get the	address	of the	*/
		       /* text string stored in	the heap		*/

	   pctli = pctliGetDataPtr(hWnd);

		       /* Copy the text	from the string	to the		*/
		       /* structure					*/

	   memcpy(pwprm->pszText, pctli->szText, (UINT)pctli->cText);
	   }
		       /* Query	type:  get text	length			*/

       if ( pwprm->fsStatus & WPM_CCHTEXT )
	   {
		       /* Text length being asked for, get the address	*/
		       /* of the text string stored in the heap		*/

	   pctli = pctliGetDataPtr(hWnd);

		       /* Place	the length the string within the	*/
		       /* structure					*/

	   pwprm->cchText = (ULONG)pctli->cText;
	   }
		       /* Query	type:  get control data	length		*/

       if ( pwprm->fsStatus & WPM_CBCTLDATA )

		       /* Set the control data length to zero		*/

	   pwprm->cbCtlData = 0;
       break;
		       /* Presentation parameters changed, record the	*/
		       /* change internally				*/

   case	WM_PRESPARAMCHANGED :

		       /* Check	to see if an individual	presentation	*/
		       /* parameter has	changed	if so, get the new	*/
		       /* colour value for use by the painting routines	*/

       if ( LONGFROMMP(mp1) )
	   {
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
	       case PP_FONTNAMESIZE :

		       /* Get the address of the control info from the	*/
		       /* control's reserved memory                     */

		   pctli = pctliGetDataPtr(hWnd);
		   CalcDisplayPoints(hWnd, pctli, pctli->cx, pctli->cy);
		   WinInvalidateRect(hWnd, (PRECTL)NULL, TRUE);
		   break;

	       case PP_FOREGROUNDCOLOR :
	       case PP_FOREGROUNDCOLORINDEX :

		       /* Get the address of the control info from the	*/
		       /* control's reserved memory                     */

		   pctli = pctliGetDataPtr(hWnd);
		   pctli->lClrText = lGetPresParam(hWnd, PP_FOREGROUNDCOLOR,
						   PP_FOREGROUNDCOLORINDEX,
						   CLR_WHITE);
		   WinInvalidateRect(hWnd, (PRECTL)NULL, TRUE);
		   break;

	       case PP_BACKGROUNDCOLOR :
	       case PP_BACKGROUNDCOLORINDEX :

		       /* Get the address of the control info from the	*/
		       /* control's reserved memory                     */

		   pctli = pctliGetDataPtr(hWnd);
		   pctli->lClrBanner = lGetPresParam(hWnd, PP_BACKGROUNDCOLOR,
						     PP_BACKGROUNDCOLORINDEX,
						     CLR_BLACK);
		   WinInvalidateRect(hWnd, (PRECTL)NULL, TRUE);
		   break;

	       case PP_SHADOW :

		       /* Get the address of the control info from the	*/
		       /* control's reserved memory                     */

		   pctli = pctliGetDataPtr(hWnd);
		   pctli->lClrTextShadow = lGetPresParam(hWnd, PP_SHADOW,
							 PP_SHADOW,
							 CLR_PALEGRAY);
		   WinInvalidateRect(hWnd, (PRECTL)NULL, TRUE);
		   break;
	       }
	   }
       else
		       /* Determine if the Scheme Palette has forced a	*/
		       /* global scheme	update in which	case, check all	*/
		       /* of the presentation parameters to see	if they	*/
		       /* have been added or removed			*/

	   if (	LONGFROMMP(mp1)	== 0L )
	       {
		       /* Set up the colours that will be used within	*/
		       /* the painting of the control.			*/

	       SetDefaultColours(hWnd, pctli = pctliGetDataPtr(hWnd));
	       CalcDisplayPoints(hWnd, pctli, pctli->cx, pctli->cy);
	       WinInvalidateRect(hWnd, (PRECTL)NULL, TRUE);
	       }
       break;
		       /* Mouse	being passed over the control, imply	*/
		       /* that the control is transparent to the	*/
		       /* system					*/
   case	WM_HITTEST :
       return(MRFROMLONG(HT_TRANSPARENT));

		       /* Erase	control	background			*/

   case	WM_ERASEBACKGROUND :
       return(MRFROMLONG(TRUE));

/************************************************************************/
/*									*/
/*	       Control message handling	something like:			*/
/*									*/
/* case	XM_MESSAGE :							*/
/*     pctli = (PCTLINFO)WinQueryWindowPtr(hWnd, QWW_CDATA);		*/
/*     WinInvalidateRect(hWnd, (PRECTL)NULL, TRUE);			*/
/*     break;								*/
/*									*/
/************************************************************************/

		       /* Paint	the Control				*/
   case	WM_PAINT :
		       /* Get the address of the text from the		*/
		       /* control's reserved memory                     */

       pctli = pctliGetDataPtr(hWnd);

		       /* Get the presentation space for the control	*/
		       /* and set the colour table to RGB mode		*/

       GpiCreateLogColorTable(hPS = WinBeginPaint(hWnd,	(HPS)NULL, (PRECTL)NULL),
			      0L, LCOLF_RGB, 0L, 0L, (PLONG)NULL);

		       /* Set the colour of the	fill for the banner	*/

       GpiSetColor(hPS,	pctli->lClrBanner);

		       /* Move to the center point of the ellipse and	*/
		       /* draw the ellipse				*/

       GpiMove(hPS, &pctli->ptlArc);
       GpiSetArcParams(hPS, &pctli->arcp);
       GpiFullArc(hPS, DRO_FILL, MAKEFIXED(1, 0));

		       /* Check	to see if there	is any text and	if not	*/
		       /* just skip the	text drawing part		*/

       if ( pctli->cText )
	   {
		       /* Set the colour of the	text			*/

	   GpiSetBackColor(hPS,	pctli->lClrBanner);

		       /* Check	to see if the text is being drawn	*/
		       /* vertically   */

	   if (	pctli->flStyle & BNS_VERTICAL )
	       {
		       /* Set the font up so it	can be drawn vertically	*/

	       GpiSetCharBox(hPS, &pctli->sizfx);
	       GpiCreateLogFont(hPS, (PSTR8)NULL, 4L, &pctli->fat);
	       GpiSetCharSet(hPS, 4L);
	       GpiSetCharAngle(hPS, &gradl);

		       /* Draw the text	vertically			*/

	       if ( pctli->flStyle & BNS_SHADOWTEXT )
		   {
		   GpiSetColor(hPS, pctli->lClrTextShadow);
		   GpiCharStringPosAt(hPS, &pctli->ptlTextShadow, &pctli->rcl, CHS_CLIP,
				      pctli->cText, pctli->szText, NULL);
		   }
	       GpiSetColor(hPS,	pctli->lClrText);
	       GpiCharStringPosAt(hPS, &pctli->ptlText,	&pctli->rcl, CHS_CLIP,
				  pctli->cText,	pctli->szText, NULL);
	       GpiSetCharSet(hPS, 0L);
	       GpiDeleteSetId(hPS, 4L);
	       }
	   else
	       {
		       /* Draw the text	horizontally			*/

	       if ( pctli->flStyle & BNS_SHADOWTEXT )
		   {
		   GpiSetColor(hPS, pctli->lClrTextShadow);
		   GpiCharStringPosAt(hPS, &pctli->ptlTextShadow, &pctli->rcl, CHS_CLIP,
				      pctli->cText, pctli->szText, NULL);
		   }
	       GpiSetColor(hPS,	pctli->lClrText);
	       GpiCharStringPosAt(hPS, &pctli->ptlText,	&pctli->rcl, CHS_CLIP,
				  pctli->cText,	pctli->szText, NULL);
	       }
	   }
       WinEndPaint(hPS);
       break;
		       /* Control being	destroyed			*/
   case	WM_DESTROY :
		       /* Get the address of the text from the		*/
		       /* control's reserved memory                     */

       pctli = (PCTLINFO)WinQueryWindowPtr(hWnd, QWW_CDATA);

		       /* Release the heap allocated for use by	the	*/
		       /* control					*/

       DosFreeMem((PVOID)pctli);
       break;
		       /* Default message processing			*/
   default :
       return(WinDefWindowProc(hWnd, msg, mp1, mp2));
   }

return(0L);
}
