/************ NLS32 C Sample Program Include file (.H) ************************/
/*                                                                            */
/*  The NLS32 include file defines symbolic constants used                    */
/*  in the NLS32.C and .RC files.                                             */
/*  NLS32 local procedure declarations may appear in this file to             */
/*  ensure they have been declared before being used.                         */
/*                                                                            */
/*  NOTE: If using the Dialog Editor "DLGBOX.EXE" ALL comments                */
/*          (including this) MUST be removed from this file.                  */
/******************************************************************************/

/**********************************************************************/
/* Conatants                                                          */
/**********************************************************************/

#define CCHMAXSTRING 64

#define LINELENGTH   80

#define BUFFERLENGTH LINELENGTH*20+1

#define SBCSCHAR           1
#define DBCSCHAR           2

#define OUTLINE_FONT       0x0001
#define RADIX              10
#define ID_LCID            30L

#define INI_PORTD          HINI_SYSTEMPROFILE

/**********************************************************************/
/* Resource Ids                                                       */
/**********************************************************************/

#define ICONID             1

#define ID_NLSSAMP         1

#define IDM_IME            101
#define IDCMD_ACTIVATE     102
#define IDCMD_ENABLE       103
#define IDCMD_ONTHESPOT    104
#define IDCMD_INLINE       105
#define IDCMD_IMENAME      106
#define IDM_CHARACTER      107
#define IDCMD_ALPHA        108
#define IDCMD_KATAKANA     109
#define IDCMD_HIRAGANA     110
#define IDCMD_SBCSDBCS     111
#define IDCMD_ROMAN        112
#define IDCMD_APPLSTAT     126
#define IDCMD_PRIORITY     127

#define IDM_FONTS          113
#define IDCMD_SELECTFONT   114
#define IDCMD_LOADFD       115
#define IDCMD_UNLOAD       116
#define IDM_WORDWRAP       117
#define IDCMD_WORDWRAP     118
#define IDM_EXIT           119
#define IDCMD_EXIT         120
#define IDCMD_CONTINUE     121
#define IDM_HELP           122
#define IDCMD_HELPEXT      123
#define IDCMD_HELPIND      124
#define IDCMD_HELPKEY      125

#define IDD_FONTLIST         2
#define IDDLISTBOX         201
#define IDDLISTMSG         202


#define IDSNAME            301
#define IDSDLGHELP         302
#define IDSDEFAULTFD       303
#define IDSERRORFD         304
#define IDSHELP            305
#define IDSEND             306
#define IDSPORTD           307
#define IDSPORTDKEY        308
#define IDSNONEFDR         309


VOID main ( void );

VOID NlssampCreate( HWND );
VOID NlssampCommand( HWND, MPARAM );
VOID NlssampModeControl( MPARAM );
VOID PecicPaint( VOID );
BOOL NlssampCharInput( HWND, MPARAM, MPARAM );
VOID NlssampDisplayChar( HWND, ULONG );
BOOL NlssampInitListDlg( HWND );
VOID NlssampEndListDlg( HWND );

MRESULT EXPENTRY MyWndProc( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY MyPecicWndProc( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY NlssampDlgListFonts( HWND, ULONG, MPARAM, MPARAM );

