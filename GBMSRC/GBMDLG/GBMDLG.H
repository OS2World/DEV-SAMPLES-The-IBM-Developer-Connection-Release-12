/*

gbmdlg.h - Interface to GBM File Dialog

*/

#define	L_GBM_OPTIONS 500

typedef struct _GBMFILEDLG
	{
	FILEDLG fild;
	CHAR szOptions [L_GBM_OPTIONS+1];
	} GBMFILEDLG;

#ifndef _GBMDLG

HWND _System GbmFileDlg(HWND hwndP, HWND hwndO, GBMFILEDLG *pgbmfild);
MRESULT _System GbmDefFileDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

#endif
