#ifndef _CLOCKDEF_
#define _CLOCKDEF_

// Kind
#define kXMPKindTestClock "Apple:Kind:TestClock"

// Class ID
#define kClockPartID "appl:clockpart$class,1.0.0"

//-------------------------------------------------------------------------
// Constants
//-------------------------------------------------------------------------

#define MATLF_SIZE ( sizeof ( MATRIXLF ) / sizeof ( LONG ) )

#define CLK_MAJORTICKS  0x0001
#define CLK_MINORTICKS  0x0002

#define XDATE 52L
#define YDATE 14L
#define ZDATE 1L

#define HT_HOUR           1
#define HT_MINUTE         2
#define HT_HOUR_SHADE     3
#define HT_MINUTE_SHADE   4
#define HT_SECOND   5

#define DM_TIME         ( (USHORT)0x0001)
#define DM_DATE         ( (USHORT)0x0002)
#define DM_ANALOG       ( (USHORT)0x0010)
#define DM_DIGITAL      ( (USHORT)0x0020)
#define DM_SECONDHAND   ( (USHORT)0x0040)

#define MAXTITLELENGTH 256
#ifndef  MESSAGELEN
#define MESSAGELEN     80
#endif
#define SHADE               0
#define LIGHT               1
#define SURFACE             2
#define BACKGROUND          3

#define DP_INSIDE           0
#define DP_UNDER            1
#define DP_LEFTDOWN         2
#define DP_LEFTMIDDLE       3

#define DM_REDRAW           0
#define DM_UPDATE           1

#define AM_ACTIVE           ( (USHORT) 0x0001)
#define AM_AUDIO            ( (USHORT) 0x0002)
#define AM_MSGBOX           ( (USHORT) 0x0004)
#define TIMER_ASYNC         1
#define TIMER_CONT          2
#define SEM_TIMER_NAME      "\\sem32\\applnam.sem"
#ifndef NO_ERROR
#define NO_ERROR            (APIRET ) 0L
#endif

#define SEM_TIMER_VALUE     ( (ULONG) (1000L * 60 * 60) )

#define SZ_APPNAME  "ClockProgram 2.1"
#define SZ_KEYNAME  "Preferences"

/*--------------------------------------------------------------*\
 *  Messagetable ids
\*--------------------------------------------------------------*/
#ifndef IDMSG_INITFAILED

#define IDMSG_INITFAILED              101
#define IDMSG_MAINWINCREATEFAILED     102
#define IDMSG_CANNOTOPENINPUTFILE     103
#define IDMSG_CANNOTOPENOUTPUTFILE    104
#define IDMSG_CANNOTRUNCOLOR          108
#define IDMSG_CANNOTGETHPS            109
#define IDMSG_HELPLOADERROR           110
#define IDMSG_CANNOTLOADSTRING        111
#define IDMSG_CANNOTOPENPRINTER       112
#define IDMSG_HELPDISPLAYERROR        113
#define IDMSG_CANNOTLOADEXITLIST      114
#define IDMSG_PRINTINITFAILED         115
#define IDMSG_OVERWRITEFILE           116
#define IDMSG_PRINTERROR              117
#define IDMSG_UNDOFAILED              118
#define IDMSG_CANNOTGETFILEINFO       119
#define IDMSG_CANNOTALLOCATEMEMORY    120
#define IDMSG_CANNOTREADFILE          121
#define IDMSG_CANNOTWRITETOFILE       122
#define IDMSG_CANNOTLOADFONTS         123
#define IDMSG_CANNOTGETPAGEINFO       124
#endif
/**************************************************************************\
*                                                                          *
*       MODULE:     dialogs.h                                              *
*                                                                          *
*       COMMENT:    Constants for the Clock dialogs                        *
*                                                                          *
\**************************************************************************/


#define CLKTM_MAJOR 0x0100
#define CLKTM_MINOR 0x0200

#define CLKTM_ALWAYS    0x0001
#define CLKTM_NOTICONIC 0x0002
#define CLKTM_NEVER     0x0004

#define CLKCLR_BUTTONSHIFT 0x08

#define CLKCLR_OBJECTS      0x0100
#define CLKCLR_BACKGROUND   0x0101
#define CLKCLR_FACE         0x0102
#define CLKCLR_HOURHAND     0x0103
#define CLKCLR_MINUTEHAND   0x0104

#define CCHMAXSTRING                    256
#define TIME_SEP_LEN        1
#define DATE_SEP_LEN        1
#define STRING_AM_LEN       2
#define STRING_PM_LEN       2
#define AM_PM  2                    /* index into rTimeSep */
#define PATHMAX 259           /* path cb     max                     */
#define COUNTRY_USA     1              /* Country code               */
#define COUNTRY_UK      44             /* Country code               */

/*--------------------------------------------------------------*\
 *  Standard Dialog box ids
\*--------------------------------------------------------------*/
#define FILEOPEN        2000
#define FILESAVE        2001

#define MSGBOXID        1001
#define OBJECTID        1002

//-------------------------------------------------------------------------
// Structures
//-------------------------------------------------------------------------

typedef struct _ALARMTIME
{
   UCHAR uchHour;
   UCHAR uchMinutes;
   USHORT usMode;
} ALARMTIME;


typedef struct _CLOCKPREF
{
    USHORT usMajorTickPref;    /*0,1*/
    USHORT usMinorTickPref;    /*2,3*/
    LONG clrBackground;        /*4,5,6,7*/
    LONG clrFace;              /*8,9,a,b*/
    LONG clrHourHand;          /*c,d,e,f*/
    LONG clrMinuteHand;        /*10,11,12,13*/
    LONG clrDateOnAnalog;      /*14,15,16,17*/
    BOOL fControlsHidden;      /*18,19*/
    USHORT usDispMode;         /*1a,1b*/
    ALARMTIME alarm ;          /*1c,1d,1e,1f*/
    SWP swp;                   /*20....*/
    SHORT sTimerType;

} CLOCKPREF;
typedef CLOCKPREF *PCLOCKPREF;

typedef struct _INISAVERESTORE
{
    SWP    swp;
    USHORT yRestore;
    USHORT xRestore;
    USHORT yMinRestore;
    USHORT xMinRestore;
} INISAVERESTORE;
typedef struct _DATETIME FAR *LPDT;    /* For DosGet/SetDateTime     */

struct CLKDATA {
   HDC  hdcBuffer;
   HPS  hpsBuffer;
   BOOL fBufferDirty;
   BOOL fEndThread;
   HRGN hClipRegion;
   HRGN hrgnFace;
   CLOCKPREF cp;
   HWND hwndHelpInstance;
   HSWITCH hsw ;
   BOOL fStartAsIcon ;
   BOOL fHelpEnabled;
   SHORT      Selected ;
   SHORT      PreviousSelected;
   BOOL     bLeadingZero;
   BOOL     bIsIconic;
   WRECT rDateTime[6];
   WRECT rAlarmTime[3];      /* placement of time and date matters */
   WRECT rRepaintTime;
   WRECT rDateAssign[3];
   SHORT sFirstTime;
   SHORT sFirstDate;
   SHORT asLeftOf[6];
   SHORT asRightOf[6];
   SHORT wDateTime[6];
   SHORT wPrevDateTime[6];
   HWND  hDate,hTime;
   HWND  hDateScroll;
   HWND  hTimeScroll;
   HWND  hCurrentFocus;
   HWND  hDateTime;
   HWND  hAlarmTime;
   BOOL bTwelveHourFormat;
   WRECT rTimeSep[3];
   CHAR  szTimeSep[TIME_SEP_LEN+1];
   CHAR  szAnteMeridian[STRING_AM_LEN+1];
   CHAR  szPostMeridian[STRING_PM_LEN+1];
   CHAR  szTitle   [81];
   CHAR  szErrClocks[PATHMAX+1];
   WRECT rDateSep[2];
   CHAR  szDateSep[DATE_SEP_LEN+1];
   BOOL fTimeFreez ;
   BOOL fDateFreez ;
   USHORT vusDateFormat;
   MATRIXLF vmatlfDateTrans;
   MATRIXLF vmatlfDateScale;
   DATETIME dt ;
   BOOL fIconic , fShowSecondHand ;
   char achOldTime[9];
   char achAmPm[3];
   char achOldAmPm[3];
   char achOldDate[9];
   HPS hpsWnd;
   LONG cxScreen, cyScreen;  /* screen dimensions */
   LONG vclrBG[3];
   LONG vclrFace[3];
   LONG vclrRing[3];
   LONG vclrHands[3];
   LONG vclrMajorTicks[3];
   LONG vclrMinorTicks[3];
   LONG vclrDate[4];
   USHORT vusDatePos;
   BOOL vbInAlarm;
   HWND hwndEntryField;
};

typedef CLKDATA *PCLKDATA;

#endif
