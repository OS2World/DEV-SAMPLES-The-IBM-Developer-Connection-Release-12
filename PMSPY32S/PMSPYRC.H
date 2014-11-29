/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                                          ³*/
/*³ PROGRAM NAME: PMSPY                                                      ³*/
/*³ -------------                                                            ³*/
/*³  A PM program that is used to look at or 'spy' on the message queue of   ³*/
/*³  other PM applications windows.                                          ³*/
/*³                                                                          ³*/
/*³ COPYRIGHT:                                                               ³*/
/*³ ----------                                                               ³*/
/*³  Copyright (C) International Business Machines Corp., 1992               ³*/
/*³                                                                          ³*/
/*³ DISCLAIMER OF WARRANTIES:                                                ³*/
/*³ -------------------------                                                ³*/
/*³  The following [enclosed] code is sample code created by IBM Corporation.³*/
/*³  This sample code is not part of any standard IBM product and is provided³*/
/*³  to you solely for the purpose of assisting you in the development of    ³*/
/*³  your applications.  The code is provided "AS IS", without warranty of   ³*/
/*³  any kind.  IBM shall not be liable for any damages arising out of your  ³*/
/*³  use of the sample code, even if they have been advised of the           ³*/
/*³  possibility of such damages.                                            ³*/
/*³                                                                          ³*/
/*³ For details on what this program does etc., please see the PMSPY.C file. ³*/
/*³                                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ PMSPYRC.H                                                                ³*/
/*³                                                                          ³*/
/*³ include for .RC definitions                                              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ NLS Enabled STRINGTABLE items                                            ³*/
/*³                                                                          ³*/
/*³ - those strings reside in the NLS .DLL file!                             ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ This is who we are...                                                    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define IDS_TITLE                         0
#define IDS_HELP_TITLE_MAIN               1

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ printf/fprintf format strings                                            ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define IDS_FMT_AGENT                     5
#define IDS_FMT_FILTER_ITEM               6
#define IDS_FMT_GRP_EXCLUDED              7
#define IDS_FMT_GRP_INCLUDED              8
#define IDS_FMT_LEVEL                     9
#define IDS_FMT_PTR_QUEUE                10
#define IDS_FMT_SPY_OBJECT               11
#define IDS_FMT_SPY_QUEUE                12
#define IDS_FMT_SPY_QUEUE_SYSTEM         13
#define IDS_FMT_SPY_WINDOW               14
#define IDS_FMT_STANDARD                 15
#define IDS_FMT_TITLE                    16
#define IDS_FMT_UNKNOWN_MSG              17
#define IDS_FMT_USER_MSG                 18
#define IDS_FMT_EXISTING_COLOR           19

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ MSG attributes, etc                                                      ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define IDS_MSG_POSTED                   22
#define IDS_MSG_SENT                     23
#define IDS_MSG_INCLUDE                  24
#define IDS_MSG_EXCLUDE                  25
#define IDS_MSG_FREEZE                   26
#define IDS_MSG_THAW                     27

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Profile items                                                            ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define IDS_PRO_DELIM                    30
#define IDS_PRO_HEADER                   31
#define IDS_PRO_WINPOS_HEADER            32
#define IDS_PRO_SETTINGS_HEADER          33
#define IDS_PRO_COLOR_HEADER             34
#define IDS_PRO_DEFINE_HEADER            35
#define IDS_PRO_FIL_HEADER               36
#define IDS_PRO_TRIGGER_HEADER           37

#define IDS_PRO_END                      40
#define IDS_PRO_WINPOS_FMT               41
#define IDS_PRO_SETTINGS_FMT             42
#define IDS_PRO_FONT_FMT                 43
#define IDS_PRO_COLOR_FMT                44
#define IDS_PRO_DEFINE_FMT               45
#define IDS_PRO_DEFINE_FMT_COLOR         46
#define IDS_PRO_FILTER_FMT               47
#define IDS_PRO_GROUP_FMT                48
#define IDS_PRO_TRIGGER_FMT              49

#define IDS_PRO_KW_X                     50
#define IDS_PRO_KW_Y                     51
#define IDS_PRO_KW_CX                    52
#define IDS_PRO_KW_CY                    53
#define IDS_PRO_KW_SPYON                 54
#define IDS_PRO_KW_WINDOW                55
#define IDS_PRO_KW_QUEUE                 56
#define IDS_PRO_KW_LIST                  57
#define IDS_PRO_KW_FONT                  58
#define IDS_PRO_KW_COLOR                 59
#define IDS_PRO_KW_DEFINE                60
#define IDS_PRO_KW_GROUP                 61
#define IDS_PRO_KW_INCLUDE               62
#define IDS_PRO_KW_EXCLUDE               63
#define IDS_PRO_KW_COMMENT1              64
#define IDS_PRO_KW_COMMENT2              65

#define IDS_PRO_KW_TRIGGER               66
#define IDS_PRO_KW_THAW                  67
#define IDS_PRO_KW_FREEZE                68

#define IDS_PRO_VALUE_ALL                69

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Command line parameters                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define IDS_PARM_PROFILE                 70
#define IDS_PARM_APPEND                  71

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ File IO messages                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define IDS_PROFILE_OPEN                 75
#define IDS_PROFILE_SAVE                 76
#define IDS_LOGFILE_OPEN                 77

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Messages                                                                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define IDS_MSG_TOO_MANY_SPIES           80
#define IDS_MSG_ALLOCATE                 81
#define IDS_MSG_PROFILE_EXISTS           82
#define IDS_MSG_LOG_EXISTS               83
#define IDS_MSG_FREE                     84
#define IDS_MSG_IPF_INIT_1               85
#define IDS_MSG_IPF_INIT_2               86
#define IDS_MSG_IPF_ERROR_1              87
#define IDS_MSG_SYSTEM_Q_VERIFY          88

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Status                                                                   ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define IDS_STAT_CLOSE                   90
#define IDS_STAT_LIST_ON                 91
#define IDS_STAT_LIST_OFF                92
#define IDS_STAT_LOG_ON                  93
#define IDS_STAT_LOG_OFF                 94
#define IDS_STAT_DESELECTED              95
#define IDS_STAT_TRIGGERED_FREEZE        96
#define IDS_STAT_TRIGGERED_THAW          97
#define IDS_STAT_SNAPSHOT_ON             98
#define IDS_STAT_SNAPSHOT_OFF            99
                                     /* === */

#define IDS_TOTAL                       100  /* must be last IDS_ item + 1 */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Non-NLS Enabled STRINGTABLE items                                        ³*/
/*³                                                                          ³*/
/*³ - these strings reside in the .EXE file!                                 ³*/
/*³ - these strings define (externalized) control data for use by PMSPY code!³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define CTL_MAX_MESSAGES                  0     /* Max # messages in LISTBOX */
#define CTL_MAX_PM_Q_SIZE                 1     /* Max % elements for PM Queue */
#define CTL_MAX_DELETEITEM                2

                                     /* === */

#define CTL_TOTAL                         3  /* must be last CTL_ item + 1 */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Main Frame Definitions                                                   ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define ID_MAINWND               1       /* Frame window                      */

#define ID_SPYLIST            1001       /* SPY message list                  */

#define ID_AB_SPYON           1100
#define ID_SELECT             1101
#define ID_DESELECT           1102
#define ID_SELECT_OBJECT      1103
#define ID_SELECT_SYSTEM_Q    1104

#define ID_AB_SPYWHAT         1200
#define IDD_WND               1201
#define IDD_QUE               1202

#define ID_AB_OPTIONS         1300

#define ID_O_LIST             1310
#define ID_O_CLEAR            1311
#define ID_O_FREEZE           1312
#define ID_O_THAW             1313
#define ID_O_SCROLL           1314

#define ID_O_LOG              1320
#define ID_O_STARTLOG         1321
#define ID_O_STOPLOG          1322

#define ID_O_BROWSEDEFAULT    1330
#define ID_O_NOTES            1340

#define ID_O_FONT             1360

#define ID_O_SAVEOPTS         1370

#define ID_AB_FILTER          1400
#define ID_F_INCLUDE          1401
#define ID_F_EXCLUDE          1402
#define ID_F_SELECTIVE        1403
#define ID_F_GROUP            1404
#define ID_F_ONLY_UNDEFINED   1405
#define ID_F_INC_WM_USER      1406
#define ID_F_INC_UNDEFINED    1407

#define ID_AB_FILE            1500
#define ID_F_LOG              1510
#define ID_F_OPENLOG          1511
#define ID_F_OPENLOG_SNAPSHOT 1512
#define ID_F_CLOSELOG         1513

#define ID_F_EXIT             1515

#define ID_F_PROFILE          1520
#define ID_F_OPENPROFILE      1521
#define ID_F_APPENDPROFILE    1522
#define ID_F_SAVEPROFILE      1523

#define ID_F_DEFAULT          1530
#define ID_F_OPENDEFAULT      1531
#define ID_F_APPENDDEFAULT    1532

#define ID_AB_HELP            1600
#define ID_H_HELP             1610
#define ID_H_EXTENDED         1611
#define ID_H_KEYS             1612
#define ID_H_INDEX            1613
#define ID_H_ABOUT            1615
#define ID_H_NEWS             1616

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Selective Message Filtering Dialog                                       ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define IDD_FILTER            2000

#define ID_PB_F_INCLUDE       2100
#define ID_PB_F_EXCLUDE       2101

#define ID_PB_F_FREEZE        2105
#define ID_PB_F_THAW          2106
#define ID_PB_F_NO_TRIGGER    2107

#define ID_PB_F_RESET         2110

#define ID_PB_F_DELETE        2115

#define ID_PB_F_HELP          2120

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ GROUP Selection Dialog                                                   ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define ID_GRP_DLG            3000

#define ID_GRP_LB_GROUPS      3100

#define ID_GRP_PB_INCLUDE     3200
#define ID_GRP_PB_EXCLUDE     3201
#define ID_GRP_PB_HELP        3202
#define ID_GRP_PB_DELETE      3203

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Common LISTBOX ID                                                        ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define ID_LB_COLORS          4000

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ ABOUT Dialog                                                             ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define IDD_ABOUT             5000
#define IDD_ABOUTICON         5100

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ OBJECT Window Selection Dialog                                           ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define ID_OW_DLG             6000

#define ID_OW_LB_WINDOWS      6100

#define ID_OW_PB_SPY_HWND     6200
#define ID_OW_PB_SPY_HMQ      6201
#define ID_OW_PB_REFRESH      6202
#define ID_OW_PB_HELP         6203

#define ID_OW_ST_WINDOWS      6300

#define ID_OW_CB_DESKTOP      6400
#define ID_OW_CB_OBJECT       6401

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ NOTES dialog                                                             ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define DLG_NOTES             7000
#define DLG_NOTES_EF_TEXT     7100
#define DLG_NOTES_PB_HELP     7200

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ FONT dialog                                                              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define DLG_FONT              8000
#define DLG_FONT_LB           8100
#define DLG_FONT_PB_HELP      8200

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ MSG Decomposition dialog                                                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define DLG_MD                9000

#define DLG_MD_ST_MSG         9100

#define DLG_MD_ST_MP1         9200
#define DLG_MD_DD_MP1         9210

#define DLG_MD_ST_MP2         9300
#define DLG_MD_DD_MP2         9310

#define DLG_MD_ST_SEEN        9400

#define DLG_MD_PB_HELP        9500

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ *****************                                                        ³*/
/*³ **** WARNING ****   the non-RC IPF panel definitions begin at 20000      ³*/
/*³ *****************                                                        ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Programmer Define Resource definitions                                   ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define IDT_HELP        256         /* Help Resource Type */

#define IDT_DEFAULTS      1

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Other definitions                                                        ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define ID_SPY_POINTER  310
