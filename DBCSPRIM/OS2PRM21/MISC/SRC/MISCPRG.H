/*** OS/2 Application Primer "MISC Sample Secondary Window Procedures"*/
/*                                                                    */
/* Program name : MISCPRG.H                                           */
/* Version : 2.1 (12/24/93)                                           */
/*                                                                    */
/*    << This sample program is only for DBCS OS/2 V2.1. >>           */
/*                                                                    */
/*    Structure/constant definitions of Sample Miscellenaeus program. */
/*                                                                    */
/**********************************************************************/
#define SRC_LENGTH 30
#define MAX_KEY_IN 64
#define MAIN_WIN_WIDTH 40

#define SUCCESS     1
#define FAIL        0
#define NUM_EXMPL   5
#define NUM_CHAR    43
#define NUM_LINE    27
#define BORD_WIDTH  4
#define TITL_MENU   3
#define DLGXPOS     50
#define DLGYPOS     5
#define STXXPOS     4
#define STXYPOS     4

typedef struct LineData
{
  struct  LineData* pre;
  struct  LineData* next;
  wchar_t wcChars[MAX_KEY_IN];   /* characters in the line           */
  LONG    lWidth[MAX_KEY_IN];    /* width of each characters in pel  */
  USHORT  usNumChars;            /* number of characters in the line */
} LINEDATA;

typedef struct CurPos
{
  USHORT usCharNum;     /* on which character...  */
  USHORT usLineNum;     /* of which line ?        */
  POINTL ptlCursor;     /* cursor position in pel */
  LONG   cx;            /* cursor size            */
  LONG   cy;
} CURPOS;


typedef enum flagtype{ dbcs, sbcs, combined } CPTYPE;

typedef struct HCode{
  CHAR usCP[5];
  CHAR descr[30];
  CPTYPE CPflag;
  struct HCode *next;
} HCODE;

#pragma pack(2)                         /* parameter for TrnsDt will be  */
                                        /* packed along 2-byte boundary  */

struct TrnsDtParm                       /* Parameters for TrnsDt            */
{
  USHORT   Length;                      /* Length of parameters             */
  USHORT   exit;                        /* Exit code                        */
  USHORT   SourceLen;                   /* Length of a source string        */
  char * _Seg16 pSource;                /* (far) Address of a source string */
  USHORT   TargetLen;                   /* Length of a target string        */
  char * _Seg16 pTarget;                /* (far) Address of a target string */
  USHORT   id;                          /* Translation id                   */
  USHORT   SourceCP;                    /* Source code page                 */
  USHORT   TargetCP;                    /* Target code page                 */
  USHORT   Options;                     /* Translation options              */
};
#pragma pack()

