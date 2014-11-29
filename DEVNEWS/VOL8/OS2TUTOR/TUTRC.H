/* These are the constants for defining panels in the RC file */

#define SECTION         0
#define PANEL           0
#define TEXT         1000
#define DOITTB       2000
#define DOITTEXT     5000
#define PRACTICETB   6000

#define PRACTICESTEP 1000
#define PRACTICE1    51000
#define PRACTICE2    52000
#define PRACTICE3    53000
#define PRACTICE4    54000
#define PRACTICE5    55000
#define PRACTICE6    56000
#define PRACTICE7    57000
#define PRACTICE8    58000
#define PRACTICE9    59000
#define PRACTICE10   60000

#define LINE2        30000
#define COLOR16      1000
#define COLOR256     0

/* These are the constants for the error messages */

#define ERR_GENFAILURE        3000
#define ERR_STHR_NAME         3001
#define ERR_NO_HELP           3002
#define ERR_ALREADY_RUNNING   3005

/* These are the constants for the various controls */

#define MLE_SCROLL_BAR 50
#define IDLB_INDEX    269
#define IDD_INDEX    270
#define ID_PRIMWIN      255
#define IDMLE_MSGBOX 1050
#define ID_COVERPAGE    256
#define ID_BUTTONWIN    257
#define ID_MESSAGEWIN   258
#define ID_BITMAPWIN    259
#define ID_PAGEWIN      262
#define IDB_ENTER        294
#define IDB_TAB        292
#define IDB_SHIFTTAB        293

/* Section buttons */
#define IDB_SECTION1 10101
#define IDB_SECTION2 10201
#define IDB_SECTION3 10301
#define IDB_SECTION4 10401
#define IDB_SECTION5 10501
#define IDB_SECTION6 10601
#define IDB_SECTION7 10701
#define IDB_SECTION8 10801
#define IDB_SECTION9 10901

/* Action buttons */

#define IDB_INDEX    20101
#define IDB_PRACTICE 20201
#define IDB_DOIT     20301
#define IDB_QUIT     20401
#define IDB_RTT      20501
#define IDB_BACK     20601
#define IDB_FORWARD  20701
#define IDB_OS2      20801
#define IDB_PREVIOUS 21101

/* Bitmaps */
#define IDBMP_BACKUP          IDB_BACK
#define IDBMP_BACKDOWN        20602 /* IDB_BACK+1 */
#define IDBMP_BACKDUP         20603 /* IDB_BACK+2 */
#define IDBMP_FORWARDUP       IDB_FORWARD
#define IDBMP_FORWARDDOWN     20702 /* IDB_FORWARD+1 */
#define IDBMP_FORWARDDUP      20703 /* IDB_FORWARD+2 */
#define IDBMP_GENDOWN         1008
#define IDBMP_GENUP           1009
#define IDBMP_SECDOWN         1011
#define IDBMP_SECUP           1012
#define IDBMP_SECSELDOWN      1013
#define IDBMP_SECSELUP        1014
#define ID_SHAREDMEM          3003
