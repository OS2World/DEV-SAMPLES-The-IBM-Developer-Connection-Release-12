/****************************************************************************
* OSA test application
*
* name: osadlg.h
*
* description: #defines for dialog items
*
****************************************************************************/

#define ID_OSATEST                 1
#define ID_COMMANDBAR             701
#define ID_SCRIPTTOOLBAR          801
#define ID_SCRIPTCMDBAR           901

/* id for menu options */
#define IDM_OSA                    45
#define IDM_OSASENDEVENTS          50
#define IDM_OSARECORDEVENTS        51
#define IDM_OSASCRIPTEDITOR        52

/* constants for string table and WinLoadString */

#define PROGRAM_TITLE               3000

/* dialog id's */

#define IDD_ODSENDEVENTS            2570
#define IDD_EVENTCOMBO              2571
#define IDD_ODDOCUMENT              2573
#define IDD_PARTDESC                2574
#define IDD_RESULT                  2572
#define IDD_SPECIFY                 2575
#define IDD_PROPCOMBO               2576
#define IDD_NAMEENTRY               2577
#define IDD_NAME                    2578
#define IDD_ID                      2579
#define IDD_IDENTRY                 2580
#define IDD_ABS                     2581
#define IDD_ABSORD                  2582
#define IDD_ABSORDCOMBO             2583
#define IDD_ABSIDX                  2584
#define IDD_ABSIDXENTRY             2585
#define IDD_DATA                    2586
#define IDD_EVENTCANCEL             2587
#define IDD_SENDEVENT               2588
#define IDD_CANCELEVENT             2589
#define WM_SETDATA_INIT             WM_USER + 1
#define WM_GETDATA_INIT             WM_USER + 2
#define WM_PRINTEVENT_INIT          WM_USER + 3
#define WM_QUITEVENT_INIT           WM_USER + 4
#define IDD_APPCOMBO                2590


#define WM_ADD_RECORDEDEVENT        WM_USER + 1
#define IDD_ODRECORDEVENTS          2700
#define IDD_ODDISPLAYEVENTS         2701
#define IDD_ODRECSTART              2702
#define IDD_ODRECSTOP               2703
#define IDD_ODRECCANCEL             2704
#define IDD_ODRECSTATUS             2705


#define WM_ADD_RECORDEDTEXT         WM_USER + 1
#define WM_PROCESS_SCRIPT_CMD       WM_USER + 2
#define WM_ACK_THREADCOMPLETE       WM_USER + 3
#define WM_DISABLE_DIALOG           WM_USER + 4
#define WM_ENABLE_DIALOG            WM_USER + 5
#define ID_RECORDTIMER              WM_USER + 6
#define ID_PLAYTIMER                WM_USER + 7
#define ID_COMPILETIMER             WM_USER + 8
#define IDD_SCRED                   2800
#define IDD_SCRED_PLAY              2801
#define IDD_SCRED_RECORD            2802
#define IDD_SCRED_STOP              2803
#define IDD_SCRED_OPEN              2804
#define IDD_SCRED_SAVE              2805
#define IDD_SCRED_SAVEAS            2806
#define IDD_SCRED_STATUS            2807
#define IDD_SCRED_SCRIPT            2808
#define IDD_SCRED_COMPILE           2809
#define IDD_SCRED_CLOSE             2810
#define IDD_SCRED_SCRIPTNAME        2811
#define IDD_SCRED_CUT               2813
#define IDD_SCRED_COPY              2814
#define IDD_SCRED_PASTE             2815
#define IDD_SCRED_FONTS             2816
#define IDD_SCRED_STYLES            2817
#define IDD_SCRED_BOLD              2818
#define IDD_SCRED_ITALIC            2819
#define IDD_SCRED_UNDERLINE         2820
#define IDD_SCRED_NEW               2822
#define IDD_SCRED_UNDO              2823
#define IDD_SCRED_STATUSTEXT        2824
