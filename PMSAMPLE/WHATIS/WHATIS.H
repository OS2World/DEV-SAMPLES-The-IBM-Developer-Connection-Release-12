/*********** whatis C Sample Program Include file (.H) ************************/
/*                                                                            */
/*  The whatis include file defines symbolic constants used                   */
/*  in the whatis.C and .RC files.                                            */
/*  whatis local procedure declarations may appear in this file to            */
/*  ensure have been declared before being used.                              */
/*                                                                            */
/*  NOTE: If using the Dialog Editor "DLGBOX.EXE" ALL comments                */
/*          (including this) MUST be removed from this file.                  */
/******************************************************************************/


#define ID_NULL     0
#define ID_WINDOW   256
#define DLG_LISTBOX 257
#define LISTBOX     258

#define ID_EXITPROG 261

#define ID_HEAP     361

#define INT_WC_FRAME             (0x0001L)
#define INT_WC_COMBOBOX          (0x0002L)
#define INT_WC_BUTTON            (0x0003L)
#define INT_WC_MENU              (0x0004L)
#define INT_WC_STATIC            (0x0005L)
#define INT_WC_ENTRYFIELD        (0x0006L)
#define INT_WC_LISTBOX           (0x0007L)
#define INT_WC_SCROLLBAR         (0x0008L)
#define INT_WC_TITLEBAR          (0x0009L)
#define INT_WC_MLE               (0x000AL)
/* 000B to 000F reserved */
#define INT_WC_APPSTAT           (0x0010L)
#define INT_WC_KBDSTAT           (0x0011L)
#define INT_WC_PECIC             (0x0012L)
#define INT_WC_DBE_KKPOPUP       (0x0013L)
/* 0014 to 001F reserved */
#define INT_WC_SPINBUTTON        (0x0020L)
/* 0021 to 0024 reserved */
#define INT_WC_CONTAINER         (0x0025L)
#define INT_WC_SLIDER            (0x0026L)
#define INT_WC_VALUESET          (0x0027L)
#define INT_WC_NOTEBOOK          (0x0028L)
