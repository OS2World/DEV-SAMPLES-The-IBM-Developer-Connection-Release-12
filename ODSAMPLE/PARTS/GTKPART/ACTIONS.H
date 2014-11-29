
SHORT msg2action( ULONG msg);

// attribute type selections
#define LS_COLOR         1
#define LS_STYLE         2
#define LS_WIDTH         3

#define FS_COLOR        11
#define FS_STYLE        12

#define TRANSLUCENCE    16
#define ARC_CLOSING     17
#define SHADOW_STYLE    18
#define BOX_ROUNDING    19

#define CLOSING_NONE    0
#define CLOSING_PIE     1
#define CLOSING_CHORD   2

// THESE MUST BE IN THE SAME ORDER AS THE IDM_TOOL_* COMMANDS!!!!!!!!!!!!!
#define ACTION_SELECT        0
#define ACTION_SKETCH        1
#define ACTION_LINE          2
#define ACTION_RECT          3
#define ACTION_ELLIPSE       4
#define ACTION_TEXT          5
#define ACTION_POLYLINE      6
#define ACTION_BOX           7
#define ACTION_ARC           8
#define ACTION_PIE           9

#define ACTION_MOVECOPY     20
#define ACTION_POINTEDIT    21
#define ACTION_RESIZE       22
#define ACTION_ROTATE       23

#define SELECT_ONLYONE       0
#define SELECT_THISONE       1
#define SELECT_TOGGLEONE     2
#define SELECT_NEXTONE       3

#define NEAREST_PT           0
#define NEAREST_ENDPT        1
#define NEAREST_CENTER       2

// Correlate options
#define HIT_ANY              0
#define HIT_EMBED            1

// Z-order op's
#define Z_ORDER_TOP          0
#define Z_ORDER_BOTTOM       1
#define Z_ORDER_UP           2
#define Z_ORDER_DOWN         3

// --- FINITE STATE MACHINE STUFF -----------------------------------------
#define  FSA_MOVE                 1
#define  FSA_PAINT                2
#define  FSA_INIT                 4

//#define  FSA_SINGLESELECT        10
//#define  FSA_POPUP               20

#if 0
#define  FSA_MB1DOWN             10
#define  FSA_MB1UP               11
#define  FSA_MB2UP               13
#define  FSA_MB2DBLCLK           17
#endif

#define  FSA_MB2DOWN             12
#define  FSA_MB1DBLCLK           16

#define  FSA_MB1CLICK            20
#define  FSA_BEGINSELECT         21
#define  FSA_ENDSELECT           22
#define  FSA_SINGLESELECT        23
#define  FSA_BEGINDRAG           24
#define  FSA_ENDDRAG             25

#define  FSA_ENTER               30
#define  FSA_ESC                 33
#define  FSA_RESET              999

#define FSAF_SHIFT       0x0001
#define FSAF_CONTROL     0x0002
#define FSAF_ALT         0x0004

