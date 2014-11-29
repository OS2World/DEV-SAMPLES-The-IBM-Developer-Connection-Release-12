#pragma	title("3D Text Control  --  Version 1.0 -- (3DText.C)")
#pragma	subtitle("  3D Text Control Control DLL - Interface Definitions")

#pragma	info(noext)

#define	INCL_DOS		   /* Include OS/2 DOS Kernal		*/
#define	INCL_GPI		   /* Include OS/2 PM GPI Interface	*/
#define	INCL_WIN		   /* Include OS/2 PM Windows Interface	*/

static char *MODID = "@(#)3dtext.c:1.00";

#include <malloc.h>
#include <os2.h>
#include <string.h>

#include <pmcx.h>

#include "3dtext.h"

/* This	module contains	an example installable control that can	be used	*/
/* within applications where additional	facilities are provided	that	*/
/* are not found within	the default controls of	OS/2 PM.		*/
/*									*/
/* This	example	differs	from the other PMCX examples in	that the	*/
/* actual control procedure is contained within	a separate DLL.	 This	*/
/* is done such	that the PMCX interface	is contained in	separate DLLs	*/
/* which are not shipped as part of the	final application.  Other the	*/
/* DLL which contains the control code is shipped (see 3D.C).		*/
/*									*/
/* For complete	details	regarding the PM Control Extensions (PMCX)	*/
/* consult the User's Guide.                                            */
/*									*/
/* The DLL is created using the	following command line invocation:	*/
/*									*/
/*     Icc -G3e- -O+ -Rn -W3 -C	Text3D.C				*/

/* Filename:   Text3D.C							*/

/*  Version:   1.0							*/
/*  Created:   1995-12-11						*/
/*  Revised:   1995-12-11						*/

/* Routines:   BOOL EXPENTRY Text3DRegister(HAB	hAB);			*/
/*	       BOOL EXPENTRY Text3DQuery(PUSERINFO pControlInfo);	*/
/*	       MRESULT EXPENTRY	Text3DStyles(HWND hWnd,	ULONG msg,	*/
/*					     MPARAM mp1, MPARAM	mp2);	*/


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

/* --- Module Prototype	Definitions -----------------------------------	*/

BOOL	EXPENTRY RegisterText3D(HAB hAB);

BOOL	EXPENTRY Text3DRegister(HAB hAB);
BOOL	EXPENTRY Text3DQuery(PUSERINFO pControlInfo);
MRESULT	EXPENTRY Text3DStyles(HWND hWnd, ULONG msg, MPARAM mp1,	MPARAM mp2);

#pragma	subtitle("   3D Text Control DLL - Control Initialization Function")
#pragma	page ( )

/* --- Text3DRegister ---------------------------------- [ Public ] ---	*/
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
/*     Text3DRegister =	 TRUE :	Class Registration Successful		*/
/*		      =	FALSE :	Class Registration Failed		*/
/*									*/
/* --------------------------------------------------------------------	*/

BOOL EXPENTRY Text3DRegister(HAB hAB)

{
		       /* Register the control class with OS/2		*/
		       /* Presentation Manager and return registration	*/
		       /* result					*/

return(RegisterText3D(hAB));

}
#pragma	subtitle("   3D Text Control DLL - Query Control Information Function")
#pragma	page ( )

/* --- Text3DQuery ------------------------------------- [ Public ] ---	*/
/*									*/
/*     This function is	used to	return to the caller information	*/
/*     regarding the installable control and its capabilities.	The	*/
/*     function	should return a	true value otherwise Resource		*/
/*     Editor will not register	the control as being usable.		*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     PUSERINFO pUserInfo; = User Information Pointer			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Text3DQuery =  TRUE : User Information Being Returned		*/
/*		   = FALSE : No	User Information Available		*/
/*									*/
/* --------------------------------------------------------------------	*/

BOOL EXPENTRY Text3DQuery(PUSERINFO pUserInfo)

{
		       /* Complete the User Information	structure	*/
		       /* passed to the	function by Resource Editor  */

		       /* Complete the version and number of control	*/
		       /* types.  In Version 1.00 of CCSL, only	one	*/
		       /* control type is used.				*/
pUserInfo->ulMajor = 3UL;
pUserInfo->ulMinor = 0UL;
		       /* Complete the author and control classname	*/

memcpy(pUserInfo->szAuthor,    "Prominare Inc.", 15);
memcpy(pUserInfo->szClassname, "3D.Text", 8);
memcpy(pUserInfo->szName,      "3DText", 7);

		       /* Complete the default size and	style of the	*/
		       /* first	user control type			*/

pUserInfo->utDefined[0].cx	     = 25L;
pUserInfo->utDefined[0].cy	     = 10L;
pUserInfo->utDefined[0].flStyle	     = WS_VISIBLE | SS_TEXT | DT_LEFT |	DT_VCENTER;

		       /* Set the maximum amount of text control can	*/
		       /* accept including NULL	termination byte	*/

pUserInfo->utDefined[0].cMaxText     = CCHTEXTMAX;

		       /* Save the style's dialogue ID, type, control   */
		       /* data size and	count of style masks		*/

pUserInfo->utDefined[0].idDlg	     = DLG_CTRLUSER;
pUserInfo->utDefined[0].flOptions    = PMCXOPT_REFRESH;
pUserInfo->utDefined[0].ulType	     = UTYPE_PRIVATE;
pUserInfo->utDefined[0].cCtlData     = 0UL;
pUserInfo->utDefined[0].cMasks	     = 12UL;
pUserInfo->utDefined[0].flStyleType  = STYLETYPE_BITFLAGS;
pUserInfo->utDefined[0].stMasks[0].flStyleMask = SS_TEXT;
pUserInfo->utDefined[0].stMasks[0].idStyle     = IDS_SS_TEXT;
pUserInfo->utDefined[0].stMasks[1].flStyleMask = DT_LEFT;
pUserInfo->utDefined[0].stMasks[1].idStyle     = IDS_DT_LEFT;
pUserInfo->utDefined[0].stMasks[2].flStyleMask = DT_CENTER;
pUserInfo->utDefined[0].stMasks[2].idStyle     = IDS_DT_CENTER;
pUserInfo->utDefined[0].stMasks[3].flStyleMask = DT_RIGHT;
pUserInfo->utDefined[0].stMasks[3].idStyle     = IDS_DT_RIGHT;
pUserInfo->utDefined[0].stMasks[4].flStyleMask = DT_TOP;
pUserInfo->utDefined[0].stMasks[4].idStyle     = IDS_DT_TOP;
pUserInfo->utDefined[0].stMasks[5].flStyleMask = DT_VCENTER;
pUserInfo->utDefined[0].stMasks[5].idStyle     = IDS_DT_VCENTER;
pUserInfo->utDefined[0].stMasks[6].flStyleMask = DT_BOTTOM;
pUserInfo->utDefined[0].stMasks[6].idStyle     = IDS_DT_BOTTOM;
pUserInfo->utDefined[0].stMasks[7].flStyleMask = DT_HALFTONE;
pUserInfo->utDefined[0].stMasks[7].idStyle     = IDS_DT_HALFTONE;
pUserInfo->utDefined[0].stMasks[8].flStyleMask = DT_MNEMONIC;
pUserInfo->utDefined[0].stMasks[8].idStyle     = IDS_DT_MNEMONIC;
pUserInfo->utDefined[0].stMasks[9].flStyleMask = DT_WORDBREAK;
pUserInfo->utDefined[0].stMasks[9].idStyle     = IDS_DT_WORDBREAK;
pUserInfo->utDefined[0].stMasks[10].flStyleMask	= DS_DEPRESSED;
pUserInfo->utDefined[0].stMasks[10].idStyle	= IDS_DS_DEPRESSED;
pUserInfo->utDefined[0].stMasks[11].flStyleMask	= DS_RAISED;
pUserInfo->utDefined[0].stMasks[11].idStyle	= IDS_DS_RAISED;

		       /* Save the description of the control		*/

memcpy(pUserInfo->utDefined[0].szDescription, "3D Text", 8);

		       /* Return the success flag back to Resource	*/
		       /* Editor					*/
return(TRUE);
}
#pragma	subtitle("   3D Text Control DLL - Control Styles Dialogue Procedure")
#pragma	page ( )

/* --- Text3DStyles ------------------------------------ [ Public ] ---	*/
/*									*/
/*     This function is	used for the custom control's styles dialogue   */
/*     box procedure.							*/
/*									*/
/*     When the	dialogue is invoked from Resource Editor, the		*/
/*     address of the user style information is	contained in message	*/
/*     parameter 2.  The dialogue is responsible for saving the		*/
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
/*     Text3DStyles = Message Handling Result				*/
/*									*/
/* --------------------------------------------------------------------	*/

MRESULT	EXPENTRY Text3DStyles(HWND hWnd, ULONG msg, MPARAM mp1,	MPARAM mp2)

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

	   WinSetDlgItemText(hWnd, EF_TEXT, pust->pszText);
	   pust->pfnSetSymbolID(hWnd, IDBX_SYMBOLVALUE,	pust);

	   if (	pust->flStyle &	DS_RAISED )
	       WinSendDlgItemMsg(hWnd, RB_RAISED, BM_SETCHECK, MPFROMSHORT(TRUE), 0L);
	   else
	       WinSendDlgItemMsg(hWnd, RB_DEPRESSED, BM_SETCHECK, MPFROMSHORT(TRUE), 0L);

		       /* Set Auto Scroll check	box if selected		*/

	   if (	pust->flStyle &	DT_RIGHT )
	       WinSendDlgItemMsg(hWnd, RB_RIGHTALIGNED,	BM_SETCHECK, MPFROMSHORT(TRUE),	0L);
	   else
	       if ( pust->flStyle & DT_CENTER )
		   WinSendDlgItemMsg(hWnd, RB_HORZALIGNED, BM_SETCHECK,	MPFROMSHORT(TRUE), 0L);
	       else
		   WinSendDlgItemMsg(hWnd, RB_LEFTALIGNED, BM_SETCHECK,	MPFROMSHORT(TRUE), 0L);

		       /* Set vertical alignment style radio button	*/

	   if (	pust->flStyle &	DT_BOTTOM )
	       WinSendDlgItemMsg(hWnd, RB_BOTALIGNED, BM_SETCHECK, MPFROMSHORT(TRUE), 0L);
	   else
	       if ( pust->flStyle & DT_VCENTER )
		   WinSendDlgItemMsg(hWnd, RB_VERTCENTERED, BM_SETCHECK, MPFROMSHORT(TRUE), 0L);
	       else
		   WinSendDlgItemMsg(hWnd, RB_TOPALIGNED, BM_SETCHECK, MPFROMSHORT(TRUE), 0L);

		       /* Set Word Break check box if style selected	*/

	   if (	pust->flStyle &	DT_WORDBREAK )
	       WinSendDlgItemMsg(hWnd, CB_WORDWRAP, BM_SETCHECK, MPFROMSHORT(TRUE), 0L);

		       /* Set Half Tone	check box if style selected	*/

	   if (	pust->flStyle &	DT_HALFTONE )
	       WinSendDlgItemMsg(hWnd, CB_HALFTONE, BM_SETCHECK, MPFROMSHORT(TRUE), 0L);

		       /* Set Mnemonic check box if style selected	*/

	   if (	pust->flStyle &	DT_MNEMONIC )
	       WinSendDlgItemMsg(hWnd, CB_MNEMONIC, BM_SETCHECK, MPFROMSHORT(TRUE), 0L);

	   if (	pust->flStyle &	WS_VISIBLE )
	       WinSendDlgItemMsg(hWnd, CB_VISIBLE, BM_SETCHECK,	MPFROMSHORT(TRUE), 0L);

	   if (	pust->flStyle &	WS_GROUP )
	       WinSendDlgItemMsg(hWnd, CB_GROUP, BM_SETCHECK, MPFROMSHORT(TRUE), 0L);

	   if (	pust->flStyle &	WS_DISABLED )
	       WinSendDlgItemMsg(hWnd, CB_DISABLED, BM_SETCHECK, MPFROMSHORT(TRUE), 0L);

	   if (	pust->flStyle &	WS_TABSTOP )
	       WinSendDlgItemMsg(hWnd, CB_TABSTOP, BM_SETCHECK,	MPFROMSHORT(TRUE), 0L);
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

	   case	DID_FONTCLR :

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

		   if (	!pust->pfnCUACheck(hWnd, EF_TEXT, CUACHK_CAPS) )
		       break;

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
		       /* saved.					*/

		   if (	!pust->pfnGetSymbolID(hWnd, IDBX_SYMBOLVALUE, pust) )
		       break;
		   else
		       {
		       /* Symbol and value validated, get the text of	*/
		       /* the control and save within the user style	*/
		       /* information structure	for use	by Resource	*/
		       /* Editor					*/

		       pust->cText = WinQueryDlgItemText(hWnd, EF_TEXT,	CCHTEXTMAX, pust->pszText);

		       /* Mask out current edit	field styles clearing	*/
		       /* selectable styles and	save new style		*/

		       pust->flStyle = SS_TEXT;

		       /* Get raised/depressed style			*/

		       if ( SHORT1FROMMR(WinSendDlgItemMsg(hWnd, RB_RAISED, BM_QUERYCHECK, 0L, 0L)) )
			   pust->flStyle |= DS_RAISED;
		       else
			   pust->flStyle |= DS_DEPRESSED;

		       /* Get horizontal alignment type			*/

		       switch (	(ULONG)SHORT1FROMMR(WinSendDlgItemMsg(hWnd, RB_LEFTALIGNED,
								      BM_QUERYCHECKINDEX, 0L, 0L)) + RB_LEFTALIGNED )
			   {
			   case	RB_LEFTALIGNED :
			       pust->flStyle |=	DT_LEFT;
			       break;

			   case	RB_HORZALIGNED :
			       pust->flStyle |=	DT_CENTER;
			       break;

			   case	RB_RIGHTALIGNED	:
			       pust->flStyle |=	DT_RIGHT;
			       break;
			   }

		       /* Get vertical alignment style			*/

		       switch (	(ULONG)SHORT1FROMMR(WinSendDlgItemMsg(hWnd, RB_TOPALIGNED,
								      BM_QUERYCHECKINDEX, 0L, 0L)) + RB_TOPALIGNED )
			   {
			   case	RB_TOPALIGNED :
			       pust->flStyle |=	DT_TOP;
			       break;

			   case	RB_VERTCENTERED	:
			       pust->flStyle |=	DT_VCENTER;
			       break;

			   case	RB_BOTALIGNED :
			       pust->flStyle |=	DT_BOTTOM;
			       break;
			   }

		       /* Set Half Tone	style if selected		*/

		       if ( WinSendDlgItemMsg(hWnd, CB_HALFTONE, BM_QUERYCHECK,	0L, 0L)	)
			   pust->flStyle |= DT_HALFTONE;

		       /* Set Word Break style if selected		*/

		       if ( WinSendDlgItemMsg(hWnd, CB_WORDWRAP, BM_QUERYCHECK,	0L, 0L)	)
			   pust->flStyle |= DT_WORDBREAK;

		       /* Set Mnemonic style if	selected		*/

		       if ( WinSendDlgItemMsg(hWnd, CB_MNEMONIC, BM_QUERYCHECK,	0L, 0L)	)
			   pust->flStyle |= DT_MNEMONIC;

		       /* Save completed edit field style in internal	*/
		       /* window information				*/

		       if ( WinSendDlgItemMsg(hWnd, CB_VISIBLE,	BM_QUERYCHECK, 0L, 0L) )
			   pust->flStyle |= WS_VISIBLE;

		       if ( WinSendDlgItemMsg(hWnd, CB_GROUP, BM_QUERYCHECK, 0L, 0L) )
			   pust->flStyle |= WS_GROUP;

		       if ( WinSendDlgItemMsg(hWnd, CB_DISABLED, BM_QUERYCHECK,	0L, 0L)	)
			   pust->flStyle |= WS_DISABLED;

		       if ( WinSendDlgItemMsg(hWnd, CB_TABSTOP,	BM_QUERYCHECK, 0L, 0L) )
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
