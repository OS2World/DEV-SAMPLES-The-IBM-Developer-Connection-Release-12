/*

gbmdlg.c - File Open / File Save as dialogs for GBM

*/

/*...sinclude:0:*/
#define	INCL_BASE
#define	INCL_DOS
#define	INCL_WIN
#define	INCL_GPI
#define	INCL_DEV
#define	INCL_BITMAPFILEFORMAT
#include <os2.h>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <memory.h>

#include "gbm.h"
#define	_GBMDLG_
#include "gbmdlg.h"
#include "gbmdlgrc.h"

/*...vgbmdlg\46\h:0:*/
/*...vgbmdlgrc\46\h:0:*/
/*...e*/

static CHAR szAllSupportedFiles [] = "<All GBM supported files>";

/*...sGbmDefFileDlgProc:0:*/
MRESULT _System GbmDefFileDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
	{
	switch ( (int) msg )
		{
/*...sWM_INITDLG \45\ set up controls:16:*/
case WM_INITDLG:
	{
	MRESULT mr = WinDefFileDlgProc(hwnd, msg, mp1, mp2);
	GBMFILEDLG *pgbmfild = (GBMFILEDLG *) WinQueryWindowULong(hwnd, QWL_USER);
	FILEDLG *pfild = &(pgbmfild -> fild);
	SHORT sInx = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, DID_FILTER_CB, LM_SEARCHSTRING, MPFROM2SHORT(LSS_CASESENSITIVE, 0), MPFROMP(szAllSupportedFiles)));
	WinSendDlgItemMsg(hwnd, DID_FILTER_CB, LM_SELECTITEM, MPFROMSHORT(sInx), MPFROMSHORT(TRUE));

	WinSendDlgItemMsg(hwnd, DID_GBM_OPTIONS_ED, EM_SETTEXTLIMIT, MPFROMSHORT(L_GBM_OPTIONS), NULL);
	WinSetDlgItemText(hwnd, DID_GBM_OPTIONS_ED, pgbmfild -> szOptions);

	if ( pfild -> pszTitle != NULL )
		WinSetWindowText(hwnd, pfild -> pszTitle);
	else if ( pfild -> fl & FDS_OPEN_DIALOG )
		WinSetWindowText(hwnd, "Open");
	else if ( pfild -> fl & FDS_SAVEAS_DIALOG )
		WinSetWindowText(hwnd, "Save as");

	return ( mr );
	}
/*...e*/
/*...sWM_COMMAND \45\ respond to button presses:16:*/
case WM_COMMAND:
	switch ( COMMANDMSG(&msg) -> cmd )
		{
/*...sDID_OK \45\ accept changes:32:*/
case DID_OK:
	{
	MRESULT mr = WinDefFileDlgProc(hwnd, msg, mp1, mp2);
	GBMFILEDLG *pgbmfild = (GBMFILEDLG *) WinQueryWindowULong(hwnd, QWL_USER);

	WinQueryDlgItemText(hwnd, DID_GBM_OPTIONS_ED, L_GBM_OPTIONS, pgbmfild -> szOptions);
	return ( mr );
	}
/*...e*/
		}
	break;
/*...e*/
/*...sWM_HELP    \45\ redirect help to our panel:16:*/
case WM_HELP:
	{
	/* Parent is HWND_DESKTOP */
	/* WinDefDlgProc() will pass this up to the parent */
	/* So redirect to the owner */
	/* PM Bug: (USHORT) SHORT1FROMMP(mp1) is not usCmd as it should be */
	/* So fix it up to always be the same for this dialog */

	return ( WinSendMsg(WinQueryWindow(hwnd, QW_OWNER), msg, MPFROMSHORT(DID_GBM_FILEDLG), mp2) );
	}
/*...e*/
/*...sFDM_FILTER \45\ filter which files to list:16:*/
/*
To be listed, the file must match the user specified filename (if present)
and the filetype specification.
*/

#define	L_FN	500

case FDM_FILTER:
	{
	CHAR szFn [L_FN+1], szFnOut [L_FN+1];
	WinQueryDlgItemText(hwnd, DID_FILENAME_ED, sizeof(szFn), szFn);

	if ( strlen(szFn) != 0 )
/*...suser has specified a filter himself:32:*/
if ( DosEditName(1, (PCH) mp1, szFn, szFnOut, sizeof(szFnOut)) == 0 )
	if ( stricmp(szFn, szFnOut) && stricmp((char *) mp1, szFnOut) )
		return ( (MRESULT) FALSE );
/*...e*/

/*...sfilter based on file type:24:*/
{
HWND hwndFt = WinWindowFromID(hwnd, DID_FILTER_CB);
CHAR szFt [100+1];
int ft, n_ft, guess_ft;
SHORT sInx;

if ( (sInx = SHORT1FROMMR(WinSendMsg(hwndFt, LM_QUERYSELECTION, NULL, NULL))) != -1 )
	WinSendMsg(hwndFt, LM_QUERYITEMTEXT, MPFROM2SHORT(sInx, sizeof(szFt)), MPFROMP(szFt));
else
	WinQueryWindowText(hwndFt, sizeof(szFt), szFt);

/* Look up type name in GBM supported file types */

gbm_query_n_filetypes(&n_ft);
for ( ft = 0; ft < n_ft; ft++ )
	{
	GBMFT gbmft;

	gbm_query_filetype(ft, &gbmft);
	if ( !strcmp(szFt, gbmft.long_name) )
		break;
	}

if ( ft < n_ft )
	/* Must not be <All Files> or <All GBM supported files> */
	{
	if ( gbm_guess_filetype((char *) mp1, &guess_ft) != GBM_ERR_OK ||
	     guess_ft != ft )
		return ( (MRESULT) FALSE );
	}
else if ( !strcmp(szFt, szAllSupportedFiles) )
	{
	if ( gbm_guess_filetype((char *) mp1, &guess_ft) != GBM_ERR_OK ||
	     guess_ft == -1 )
		return ( (MRESULT) FALSE );
	}
}
/*...e*/

	return ( (MRESULT) TRUE );
	}
/*...e*/
		}
	return ( WinDefFileDlgProc(hwnd, msg, mp1, mp2) );
	}
/*...e*/

HWND _System GbmFileDlg(HWND hwndP, HWND hwndO, GBMFILEDLG *pgbmfild)
	{
	FILEDLG *pfild = &(pgbmfild -> fild);
	HMODULE hmod;
	int ft, n_ft;
	CHAR **apsz;
	HWND hwndRet;

        DosQueryModuleHandle("GBMDLG", &hmod);

	pfild -> fl |= FDS_CUSTOM;

	if ( pfild -> pfnDlgProc == (PFNWP) NULL )
		pfild -> pfnDlgProc = GbmDefFileDlgProc;

	if ( pfild -> hMod == (HMODULE) NULL )
		{
		pfild -> hMod    = hmod;
		pfild -> usDlgId = RID_GBM_FILEDLG;
		}

	gbm_query_n_filetypes(&n_ft);

	if ( (apsz = malloc((n_ft + 2) * sizeof(CHAR *))) == NULL )
		return ( (HWND) NULL );

	for ( ft = 0; ft < n_ft; ft++ )
		{
		GBMFT gbmft;

		gbm_query_filetype(ft, &gbmft);
		apsz [ft] = gbmft.long_name;
		}
	apsz [n_ft++] = szAllSupportedFiles;
	apsz [n_ft  ] = NULL;

	pfild -> papszITypeList = (PAPSZ) apsz;

	hwndRet = WinFileDlg(hwndP, hwndO, (FILEDLG *) pgbmfild);

	free(apsz);

	return ( hwndRet );
	}
