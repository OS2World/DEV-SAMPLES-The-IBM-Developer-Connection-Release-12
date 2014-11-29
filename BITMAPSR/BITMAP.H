#define STRINGLENGTH 20                 /* Length of string             */
#define FORWARD   0
#define REVERSE   1
#define RESTART   2

#define MAXPATH     130   /* Used for path specs. when loading .BMP files.   */
#define THREAD_STACKSIZE 4096*8

#define BITMAP_FILE   (SWP_FOCUSACTIVATE)
#define SLIDE_FILE    (SWP_FOCUSDEACTIVATE)

#define FILE_TEMPLATE     "*.BMP"  /* Used when creating the FILES dlgbox.*/
#define SLIDE_TEMPLATE    "*.SLD"  /* Used when creating the SLIDE dlgbox.*/


/* This is for a simple circular linked list to hold the "slide tray". */
typedef struct LINKNODE
{
  struct LINKNODE *pPrev;
  struct LINKNODE *pNext;
  PSZ      pString;
} LINKNODE, FAR *PLINKNODE;


int main(int argc, char *argv[]);
MRESULT EXPENTRY MyWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY FilterProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
void RemoveBitmapFromMem(PHDC phdc, PHPS phps, PHBITMAP phbm);
void ResetWindowText(HWND hWnd, UCHAR *pTextString, ULONG usNumberOfColors, ULONG x, ULONG y);
ULONG LoadBitmapFile(PSZ lpFileName, PHAB phAB, PHDC phdcMem,
                      PHPS phpsMem, PHBITMAP phbmMem,
                      BOOL bShowWaitPointer);
VOID RemoveWhiteSpace(PSZ lpString);
PSZ ChangeIconOrPtr(PBITMAPINFO pBmapHeader, PSZ lpFileBuf);
HSWITCH AddToSwitchList(HWND hSomeWnd, HWND hIcon, UCHAR *pEntryText);
void ShowErrors(ULONG ErrorCode, PSZ lpString);
BOOL EXPENTRY StaticBox(HWND hDlg, ULONG Message, MPARAM mp1, MPARAM mp2);
VOID SlideThread(PVOID pFileName);
PLINKNODE PutFileInLList(FILE *hFile);
VOID FreeLList(PLINKNODE pHeadNode);
HWND FixupMenus(HWND hMenuOwner, BOOL bStartShow);
ULONG ParseArgs(int argc, char *argv[], UCHAR **pFname, PRECTL pInitialSize);
ULONG LoadGifFile(PSZ data, PHAB phAB, PHDC phdcMem, PHPS phpsMem, PHBITMAP phbmMem);
ULONG LoadBitmap(CHAR *szFn, CHAR *szOpt, GBM *pgbm, GBMRGB *pgbmrgb, BYTE **ppbData);
