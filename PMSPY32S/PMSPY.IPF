.*************************************************************************
.* PMSPY Help text
.*************************************************************************
.*--- C preprocessor macros to make IPFC code easier
.*
#define _RES(id) res=##id.
.*
#define _BLUE(text) :hp4.text:ehp4.
#define _RED(text) :hp8.text:ehp8.
.*
#define _ITALIC(text) :hp3.text:ehp3.
#define _BOLD(text) :hp2.text:ehp2.
#define _UNDER(text) :hp7.text:ehp7.
.*
#define _PMSPY _BLUE(PMSPY)
.*
#define _HYPER(id,text) :link reftype=hd res=##id.text:elink.
.*
#define SYSTEM_Q_WARNING \
:p._RED(W A R N I N G) \
:p.Since spying on the system message queue can _BOLD(greatly affect) \
PM and/or application operation, it is recommended that all unnecessary \
PM messages be _ITALIC(excluded) from spying before using this selection. \
_BOLD(Only) the minimum required subset of messages should be _ITALIC(included).
.*
.*--- Load values for symbolic substitutions as needed ---
#include "pmspyipf.h"
#include "pmspyrc.h"
.*--------------------------------------------------------------------------
#define _AGENTS 8
#define _VERSION 1.1
.*--------------------------------------------------------------------------
:userdoc.
.*
.*************************************************************************
:h1 _RES(PID_EXTHELP)PMSPY - Version _VERSION
.*************************************************************************
.*
.* Load all :i1 items one time
.*
.*************************************************************************
.* Top Level INDEX items
.*
.* - defined only ONCE here, but referenced many times in other panels
.*************************************************************************
:i1 id=iDLG.   Dialogs
:i1 id=iHOWTO. How do I?
:i1 id=iLIST.  Message list
:i1 id=iLOG.   Log file
:i1 id=iMsg.   Messages
:i1 id=iMSGFIL.Message filtering
:i1 id=iPRO.   Profile file
:i1 id=iPRODEF.Default profile file
:i1 id=iSPY.   PMSPY
.*
_PMSPY allows you to _ITALIC(spy)&colon.
:ol compact.
:li.messages that are _ITALIC(posted) to windows through a message queue
:li.messages that are _ITALIC(sent) directly to windows
:eol.
This includes PM as well as any internal application messages that may be used.
The window may be visible or invisible (OBJECT).
.*
:p.It is an excellent learning tool for PM programming because it visibly
demonstrates PM&apos.s message based operation.
.*
:p.It also excels as a PM application debugging tool because&colon.
:ul compact.
:li.No application coding is necessary for _ITALIC(passive) debugging
:li._ITALIC(Active) debugging is possible if the application
is coded with _PMSPY debugging in mind.  For example, current values of
flags, counters, etc can be "stuffed" into unused MP1 and/or MP2 fields
and then examined with _PMSPY.  This can provide an _BOLD(x-ray view) into
the internals of an application as it executes.
:li.It may be started before or while the application is running
:li.Up to _AGENTS different _ITALIC(agents) may be active
at once allowing for the debugging of
complex inter-window, intra-window, multi-threaded, or OBJECT window
applications
:eul.
.*
:p.Basic operations and features include:
:ol compact.
:li.Simple _ITALIC(point and shoot) selection of desktop windows
:li.Simple selection of invisible and/or OBJECT windows from a list
.*
:li.A
_HYPER(ID_AB_FILTER,filtering)
mechanism allows
you to _ITALIC(include) or _ITALIC(exclude) individual or
groups of messages.
:li.Messages can be defined to _ITALIC(trigger) the
message spying actions.
.*
:li.The filtering mechanism is totally user definable via a
_HYPER(PID_R_PROFILE,profile).
.*
:li.A
_HYPER(PID_DEFAULT_PROFILE,default profile)
is automatically provided for _BOLD(out-of-the-box) operation.
.*
:li.Messages may optionally be logged to a
_HYPER(PID_R_LOG_FILE,log file).
.*
:eol.
.*
:p.Other thing you may be interested in:
:ol compact.
.*
:li._HYPER(PID_HOWTO,How do I do it?)
:li._HYPER(PID_SPYLIST,The message list)
:li._HYPER(PID_R_CMDLINE,Command line parameters)
:li._HYPER(PID_LIMITATIONS,known limitations)
:eol.
.*
.*************************************************************************
:h1 _RES(PID_HOWTO)How do I do it?
.*************************************************************************
:i2 refid=iSPY.How do I do it?
.*
Here&apos.s some actions you may want to know _ITALIC(how-to-do).
Double-click on a highlighted phrase to move to its detailed explanation.
:ul compact.
.*
:li.To debug a _HYPER(PID_HOWTO_DIALOGS,dialog procedure)
.*
:li.To see all _HYPER(ID_F_INC_WM_USER,WM_USER) messages
.*
:li.To see all _HYPER(ID_F_INC_UNDEFINED,unknown) messages
.*
:li.Use _HYPER(PID_TRIGGER,trigger messages)
.*
:li.To
_HYPER(DLG_NOTES,add a text note)
to the spy list and (maybe) log file
.*
:li.To
_HYPER(PID_SAVE,save agent characteristics)
(like the _BOLD(font) you selected for the spy
list, window position and size)
for the next time you run _PMSPY
.*
:eul.
.*
.*************************************************************************
:h1 _RES(PID_HOW_TO_INSTALL)How to Install and Use PMSPY
.*************************************************************************
:i2 refid=iSPY.Installing
:i2 refid=iSPY.Using
:ol compact.
:li.The following files are required to run _PMSPY&colon.
.*
:dl tsize=16 compact.
:dthd.File Name
:ddhd.Description
:dt.PMSPY.EXE
:dd.Executable code
:dt.PMSPY.HLP
:dd.Online help
:dt.PMSPY.DLL
:dd.PM resources (strings, messages, dialogs, etc)
:dt.PMSPYHK.DLL
:dd.Dynamic link library containing "hook" code
:edl.
.*
:li.Copy PMSPY.DLL and PMSPYHK.DLL into a subdirectory which is
accessible using the _BOLD(LIBPATH) statement in the CONFIG.SYS
file (e.g. C&colon.&bsl.OS2&bsl.DLL) or add the path to the LIBPATH
statement and reboot (only after having made all CONFIG.SYS changes).
.*
:li.Copy PMSPY.EXE into a subdirectory which is in your _BOLD(PATH)
statement in the CONFIG.SYS file (e.g. C&colon.&bsl.OS2) or add the path
to the PATH statement.
.*
:li.Copy PMSPY.HLP into a subdirectory which is in your _BOLD(HELP)
statement in the CONFIG.SYS file (e.g. C&colon.&bsl.OS2&bsl.HELP) or add
the path to the HELP statement.
.*
:li.Create a _PMSPY program object in one of your OS/2 folders.
Be sure to specify that PMSPY.EXE to be executed from the drive and path
where you placed the code above.
Specify any desired _HYPER(PID_R_CMDLINE,command line parameters).
.*
:li.When you&apos.re ready to begin _ITALIC(spying), just
double-click on the _PMSPY program object.
You may start up to _AGENTS different instances of _PMSPY
to help you analyze/debug complex PM applications.
.*
:eol.
.*
.*************************************************************************
:h1 _RES(PID_HOW_IT_WORKS)How Does PMSPY Work?
.*************************************************************************
:i2 refid=iSPY.How does it work?
.*
:p.After you
_HYPER(ID_SELECT,select a target window)
_PMSPY installs an INPUT and SENDMSG _ITALIC(hook) into the
window&apos.s message
queue.  This allows the _ITALIC(spying)
of all the messages destined for the window.
:note.The PM system queue may also be selected...
.*
:p.If you have requested to spy on the _BOLD(queue),
you will see all messages going
through this queue, which means you get the messages from all the windows
associated with this queue.  This is particularly important when you want
to _HYPER(PID_HOWTO_DIALOGS,debug dialogs).
.*
.*************************************************************************
:h1 _RES(PID_LIMITATIONS)Known Limitations
.*************************************************************************
:i2 refid=iSPY.Limitations
.*
:p.The following limitations are known:
:ol compact.
.*
:li.There is no current method for defining the MP formatting styles on
a per message basis in a profile.
:note.This will be provided in the next release...
.*
:li.There is NO profile error reporting (for example, you may be able to
generate multiple occurrences of the same group and/or message)
.*
:li.If PMSPYHK.DLL is not found along _BOLD(LIBPATH),
_PMSPY will not start.
The message &apos.SYS1804 file not found&apos. is issued
with the explanation that this is an invalid command.
.*
:eol.
.*
.*************************************************************************
:h1 _RES(PID_HOWTO_DIALOGS)How do I debug dialog procs?
.*************************************************************************
:i2 refid=iHOWTO.Debug dialog procs
.*
To debug _BOLD(dialog procedures)&colon.
:ol compact.
.*
:li.Use _BOLD(Select desktop window) and select the
_BOLD(parent/frame window) for spying.  This is because
the dialog you want
to debug will share the same message queue once its created.
:li.Use _BOLD(Spy on queue) to be sure that you are spying
on all messages that all moving through the message queue.
This will include the frame and all subsequent dialogs.
.*
:li.Start the dialog.  You should be able to see _BOLD(WM_CREATE)
messages for each dialog control as well as the _BOLD(WM_INITDLG)
message that comes to your dialog procedure.
.*
:eol.
.*
.*************************************************************************
:h1 _RES(PID_SAVE)Saving agent characteristics
.*************************************************************************
:i2 refid=iSPY.Save agent characteristics
:i2 refid=iHOWTO.Save agent characteristics
.*
To save this agent&apos.s characteristics for subsequent _PMSPY use,
use the _HYPER(ID_O_SAVEOPTS,Save options) pull-down
under _BOLD(Options).
.*
:p.The following data is saved
on a _ITALIC(per agent basis) in PMSPY.INI.
This file is always placed in the current directory.
:ul compact.
:li.the current size and position of the _PMSPY window
:li.the name of the last _HYPER(DLG_FONT,font) you selected for the spy list
:li.the name of the last _HYPER(PID_R_PROFILE,profile) you opened
:li.the name of the last _HYPER(PID_R_LOG_FILE,log file) you opened
:eul.
.*
:p.Each time _PMSPY starts,
it tries to use any data previously saved for the _ITALIC(agent)
currently being started (1, 2, and so on)  If this data is not available,
default values are automatically used.
.*
.*************************************************************************
:h1 _RES(PID_SPYLIST)Message List
.*************************************************************************
:i2 refid=iSPY.Message list
:i2 refid=iLIST.Definition
:i2 refid=iLIST.What&apos.s in it
:p._PMSPY places the following items into the message list:
.*
:ol compact.
.*
:li.Eligible messages (as defined by the current profile and
operational settings).
Each list item represents a single PM message&colon.
:xmp.
WM_BUTTON1UP 005C002A 00000000 P 03300CC8
                             
Mnemonic     MP1      MP2      ³ target
 message                       ³ HWND
  name                S=sent  Ä´
                      P=postedÄÙ
:exmp.
:note.To view the _HYPER(DLG_MD,message parameters in different formats),
double-click mouse button #1 on the list item
.*
:li.Programmer entered _HYPER(DLG_NOTES,text notes)
.*
:li._PMSPY status messages.
.*
:eol.
.*
:p.The horizontal scroll bar can be used to scroll the message data left or
right to position just the data you care about in view.
:p.Here is a sample message list.  It contains notes, status messages, and
PM messages.
:xmp.
PM Spy - Window 03300CC8
WM_BUTTON2DOWN         00580085 00000000 P 03300CC8
WM_BUTTON2UP           0039008C 00000000 P 03300CC8
WM_PAINT               00000000 00000000 S 03300CC8
WM_SETFOCUS            0C902FB0 00000000 S 03300CC8
WM_SETSELECTION        00000000 00000000 S 03300CC8
WM_ACTIVATE            00000000 02900BA8 S 03300CC8
This is a NOTE: ready to look at UNDEFINED messages
WM_BUTTON2DOWN         004C0093 00000000 P 03300CC8
WM_FOCUSCHANGE         0C902FB0 00000001 S 03300CC8
WM_ACTIVATE            00000001 02900BA8 S 03300CC8
WM_SETFOCUS            0C902FB0 00000001 S 03300CC8
WM_???? (0303)         02470306 00010001 S 03300CC8
WM_???? (0303)         02370306 00010001 S 03300CC8
WM_???? (0303)         02300304 00010001 S 03300CC8
WM_???? (0303)         022D0303 00010001 S 03300CC8
WM_BUTTON2UP           0017002B 00000000 P 03300CC8
all done, closing the LOG file
:exmp.
.*
.*************************************************************************
:h1 _RES(PID_DEFAULT_PROFILE)Default Profile
.*************************************************************************
:i2 refid=iPRO.Default profile
:i2 refid=iSPY.Default profile
The default PROFILE defines how _PMSPY operates "out of the box".
:xmp.
.im pmspydef.pro
:exmp.
.*
.*************************************************************************
:h1 _RES(PID_TRIGGER)Using Trigger Messages
.*************************************************************************
:i2 refid=iPRO.Using trigger messages
:i2 refid=iHOWTO.Use trigger messages
It is possible to define messages to _ITALIC(trigger) the automatic
thawing or freezing of the _HYPER(PID_SPYLIST,message list)
using:
:ol compact.
:li.the _HYPER(IDD_FILTER,Message Filtering) dialog
:li.a _HYPER(PID_R_PROFILE,profile file) that you have created.
:eol.
.*
:p.Trigger messages are valuable when you only
want to see the messages that occur&colon.
:ul compact.
:li._BOLD(up to) a event
:li._BOLD(after) a event
:li._BOLD(between) two events (see example below)
:eul.
.*
:p.For example, to determine _BOLD(only) the messages that are processed
by the OS/2 Desktop when mouse button #1 is pressed and released:
:ol compact.
:li.Use the _HYPER(ID_O_FREEZE,Freeze) pull-down to stop messages from
being added to the message list
:li.Open the _HYPER(IDD_FILTER,Message Filtering) dialog
:li.Ensure that both the _BOLD(WM_BUTTON1DOWN) and _BOLD(WM_BUTTON1UP)
messages are currently _BOLD(included).
:li.Select only the _BOLD(WM_BUTTON1DOWN) message then press the
_BOLD(Thaw) pushbutton.
Now when this message is _ITALIC(seen), messages
will start being added to the message list.
:li.Select only the _BOLD(WM_BUTTON1UP) message then press the
_BOLD(Freeze) pushbutton.
When this message is _ITALIC(seen), messages
will stop being added to the message list.
:li.Select _BOLD(Done) to close the dialog.
:li.Use the _HYPER(ID_SELECT,Select desktop window) pull-down to select
the OS/2 Desktop for _ITALIC(spying).
:li.Move the mouse cursor over a
blank area of the desktop.  Press and hold mouse button #1.  You should
see messages added to the message list.
:li.Release the mouse button and the message list should _ITALIC(freeze).
:eol.
:p.This is a sample of what the previous steps generate&colon.
:xmp.
ÚÄÄÄ Step above
³ ÚÄ List items
 
1 <<< list frozen >>>
8 <<< list thawed by trigger >>>
8 WM_BUTTON1DOWN     01EA0191 00000000 P 04801E64
8 WM_FOCUSCHANGE     8FE0CB9C 00000001 S 04801E64
8 WM_ACTIVATE        00000001 04601D64 S 04801E64
8 WM_SETSELECTION    00000001 00000000 S 04801E64
8 WM_SETFOCUS        8FE0CB9C 00000001 S 04801E64
8 CM_PAINTBACKGROUND 0003ED70 00000000 S 04801E64
8 CM_QUERYCNRINFO    0003E910 0000005C S 04801E64
8 WM_CONTROL         00708008 04801E64 S 04801E64
9 WM_BUTTON1UP       01EA0192 00000000 P 04801E64
9 <<< list frozen by trigger >>>
:exmp.
.*
.*************************************************************************
.* PMSPY - Dialog Help
.*************************************************************************
.*
.*************************************************************************
:h1 _RES(IDD_FILTER)Message Filtering Dialog
.*************************************************************************
:i2 refid=iDLG.Message filtering dialog
.*
:p.This dialog allows you to assign _ITALIC(spying) attributes
to individual messages.
:p.All of the messages are displayed with their current attributes.
Select the item(s) you want to adjust, then
apply an attribute by using an _ITALIC(attribute) button.
After a button is used, each item is updated in the list.
Press the _BOLD(Done) button when you have completed all assignments.
.*
:dl tsize=10 compact.
.*
:dt._UNDER(Include)
:dd.when the message is detected, it will be added to the spy list.
the message will also be examined to see if it is a _ITALIC(triggering) event
:dt._UNDER(Exclude)
:dd.when the message is detected, it will not be added to the spy list.
the message&apos.s _ITALIC(triggering) attributes will not be examined
.*
:dt._UNDER(Freeze)
:dd.when the message is detected and is currently included,
it will automatically
_ITALIC(trigger the freezing) of the spy list
.*
:dt._UNDER(Thaw)
:dd.when the message is detected and is currently included,
it will automatically
_ITALIC(trigger the thawing) of the spy list
.*
:dt._UNDER(No trigger)
:dd.removes automatic _ITALIC(freeze/thaw) triggering
.*
:dt._UNDER(Reset)
:dd.resets message specific dynamic data
(like how many times the message has been seen, etc)
.*
:dt._UNDER(Delete)
:dd.deletes each message (it can NO longer be _ITALIC(spyed)..)
:edl.
.*
:p.Notes:
:ul compact.
:li.usually you will open this dialog after setting the filter list to
a known state (using one or more the following pull-downs:
_BOLD(Include All),
_BOLD(Exclude All),
_BOLD(Freeze), or
_BOLD(Thaw))
:li._UNDER(Delete) is normally used when creating custom profiles
:li.the _BOLD(Color list) allows you to additionally highlight messages
in the spy list with a special color.
Pick a color _BOLD(before) selecting an _ITALIC(attribute) pushbutton.
:eul.
.*
.*************************************************************************
:h1 _RES(ID_GRP_DLG)Group Filtering Dialog
.*************************************************************************
:i2 refid=iDLG.Group filtering dialog
.*
:p.This dialog allows you to assign _ITALIC(spying) attributes
to messages based on their logical grouping.
:p.All of the groups are displayed with their current attributes.
Select the item(s) you want to adjust, then
apply an attribute by using an _ITALIC(attribute) button.
After a button is used, each item is updated in the list.
Press the _BOLD(Done) button when you have completed all assignments.
.*
:dl tsize=10 compact.
.*
:dt._UNDER(Include)
:dd.when a message in this group is detected, it will be added to the spy list
:dt._UNDER(Exclude)
:dd.when a message in this group is detected, it will never added to the spy list
.*
:dt._UNDER(Delete)
:dd.deletes all messages defined in this group
(they can NO longer be _ITALIC(spyed)..)
:edl.
.*
:p.Notes:
:ul compact.
:li.usually you will open this dialog after setting the filter list to
a known state (using one or more the following pull-downs:
_BOLD(Include All), or
_BOLD(Exclude All))
:li._UNDER(Delete) is normally used when creating custom profiles
:li.the _BOLD(Color list) allows you to additionally highlight messages
in the spy list with a special color.
Pick a color _BOLD(before) selecting an _ITALIC(attribute) pushbutton.
:eul.
.*
.*************************************************************************
:h1 _RES(ID_OW_DLG)Window Selection dialog
.*************************************************************************
:i2 refid=iDLG.window selection
.*
:p.This dialog allows you to select a window for _ITALIC(spying) from a
list of windows who are descendents of HWND_DESKTOP and/or HWND_OBJECT.
The list includes the following data:
:xmp.
 Hmq: 09112F78 Hwnd: 0000968F Linage: DSK.491.1 Class: Clock
                                            
 Windows       Windows                ³   P   T  Windows
  queue         handle       DeSKtop Ä´   I   I   class name
                             OBJect  ÄÙ   D   D
:exmp.
.*
:p.Select the window from the list that
you would like to _ITALIC(spy)
and then press a pushbutton:
:dl tsize=18 compact.
:dt._UNDER(Spy HWND)
:dd.begins _ITALIC(spying) on the selected window&apos.s handle.
:dt._UNDER(Spy HMQ)
:dd.begins _ITALIC(spying) on the selected window&apos.s queue.
:dt._UNDER(Refresh)
:dd.refreshes the list using the current
_BOLD(DESKTOP windows) and _BOLD(OBJECT windows) values as criteria.
:dt._UNDER(Cancel)
:dd.ends the dialog and preserves the current window/queue being _ITALIC(spyed).
:edl.
.*
.*************************************************************************
:h1 _RES(DLG_MD)Message Detail dialog
.*************************************************************************
:i2 refid=iDLG.individual message detail
:i2 refid=iLIST.individual message detail
:i2 refid=iMsg.individual message detail
.*
:p.This dialog displays detailed information about a particular _ITALIC(spied)
message.  This includes displaying the message parameters in various formats
as well as message specific usage information.
.*
:p.To open this dialog, double-click mouse button #1 on a
message in the _HYPER(PID_SPYLIST,message list).
.*
.*************************************************************************
:h1 _RES(DLG_NOTES)Notes dialog
.*************************************************************************
:i2 refid=iDLG.Notes
.*
:p.This dialog allows you to enter a text note that is then placed
in the _PMSPY list and possible the LOG file.
:p.You can use these notes to document what is happening, the beginning or
end of a test case, etc.
:p.Enter the desired note text then press one of the pushbuttons.
.*
:dl tsize=12  compact.
.*
:dt._UNDER(OK)
:dd.This will accept the note text that you have entered.
If you have entered text,
it will be placed in the _PMSPY list and remembered as the _BOLD(default)
note text for the next dialog invocation.
If you have not entered text, you will be warned by a beep.
.*
:dt._UNDER(Cancel)
:dd.This will cancel the dialog.  All note text is discarded.
.*
:edl.
.*
.*************************************************************************
:h1 _RES(DLG_FONT)Font dialog
.*************************************************************************
:i2 refid=iDLG.Font selection
.*
:p.This dialog allows you to select the non-proportional font to be used
in the _PMSPY message list and dialogs.
:p.Select the desired font then press one of the pushbuttons.
.*
:dl tsize=9 compact.
:dt._UNDER(OK)
:dd.The _PMSPY list will change to use the selected font.
:dt._UNDER(Cancel)
:dd.This will cancel the dialog.  All note text is discarded.
.*
:edl.
.*************************************************************************
.* PMSPY - Action Bar and pull-down Help
.*************************************************************************
.*
.*------------------------------------------------------------------------
:h1 _RES(ID_AB_FILE)File
.*------------------------------------------------------------------------
This item allows you to work with&colon.
:ul compact.
:li.A _HYPER(PID_R_PROFILE,profile file) that you have created.
:li.A _HYPER(PID_R_LOG_FILE,log file) for saving the
_HYPER(PID_SPYLIST,message list) contents on disk.
:li.The _HYPER(PID_DEFAULT_PROFILE,default profile)
:eul.
.*
.*************************************************************************
:h1 _RES(ID_F_PROFILE)Message profile
.*************************************************************************
This pull-down allows you to work with
_HYPER(PID_R_PROFILE,profile files) that you have created.
.*
.*************************************************************************
:h1 _RES(ID_F_OPENPROFILE)Open, replace current
.*************************************************************************
:i2 refid=iPRO.Open, replace current
:i2 refid=iMSGFIL.Open, profile, replace current
.*
This pull-down allows you to open a
_HYPER(PID_R_PROFILE,profile file)
that you have created.  It totally replaces the current profile.
You would want to replace the current profile to ensure that only
the specific messages that you are interested in are defined.
:p.A _BOLD(file open dialog) is opened to let you select the
desired profile file.
.*
.*************************************************************************
:h1 _RES(ID_F_APPENDPROFILE)Open, append to current
.*************************************************************************
:i2 refid=iPRO.Open, append to current
:i2 refid=iMSGFIL.Open, profile, append to current
.*
This pull-down allows you to open a
_HYPER(PID_R_PROFILE,profile file) that
you have created.  It is appended to the current profile.
You would want to append to the current profile to only incrementally
add application specific messages to those that are currently defined.
For example, you may create a profile for your application&apos.s WM_USER and
above messages.  You would then append that to the _PMSPY
_HYPER(PID_DEFAULT_PROFILE,default profile)
to be able to process standard PM as well as application unique messages.
:p.A _BOLD(file open dialog) is opened to let you select the
desired profile file.
.*
.*************************************************************************
:h1 _RES(ID_F_SAVEPROFILE)Save current
.*************************************************************************
:i2 refid=iPRO.Save current
:i2 refid=iMSGFIL.Save to file
.*
This pull-down allows you to save the current profile in a disk file.
For example, you may want to create a custom profile.
After starting _PMSPY you would then select this pull-down to
save the _HYPER(PID_DEFAULT_PROFILE,default profile)
in a disk file.
:p.A _BOLD(file open dialog) is opened to let you select the
desired profile file.
.*
.*************************************************************************
:h1 _RES(ID_F_LOG)Log file
.*************************************************************************
This pull-down allows you to work with a _HYPER(PID_R_LOG_FILE,log file)
.*
.*************************************************************************
:h1 _RES(ID_F_OPENLOG)Open
.*************************************************************************
:i2 refid=iLOG.Open
.*
This pull-down allows you to open a
_HYPER(PID_R_LOG_FILE,log file)
for saving the message list contents on disk.
The log file provides an added debugging and/or learning opportunity
because it contains all of the messages and notes that have occurred
since the log file was opened.
:p.A _BOLD(file open dialog) is opened to let you select the
desired log file.
.*
.*************************************************************************
:h1 _RES(ID_F_OPENLOG_SNAPSHOT)Open with list snapshot
.*************************************************************************
:i2 refid=iLOG.Open with list snapshot
.*
This pull-down allows you to open a _HYPER(PID_R_LOG_FILE,log file)
for saving the message list contents on disk.
After the log file is opened, the current contents of the message list
is written to it.  This _ITALIC(after the fact) logging can be a very
useful debug aid.
:p.The log file provides an added debugging and/or learning opportunity
because it contains all of the messages and notes that have occurred
since the log file was opened.
:p.A _BOLD(file open dialog) is opened to let you select the
desired log file.
.*
.*************************************************************************
:h1 _RES(ID_F_CLOSELOG)Close
.*************************************************************************
:i2 refid=iLOG.Close
.*
This pull-down allows you to close the
_HYPER(PID_R_LOG_FILE,log file).
.*
.*************************************************************************
:h1 _RES(ID_F_DEFAULT)Default message profile
.*************************************************************************
This pull-down allows you to work with the
_HYPER(PID_DEFAULT_PROFILE,default profile file) shipped with _PMSPY.
.*
.*************************************************************************
:h1 _RES(ID_F_OPENDEFAULT)Open, replace current
.*************************************************************************
:i2 refid=iPRO.Open defaults, replace current
:i2 refid=iMSGFIL.Open defaults, replace
.*
This pull-down allows you to
totally replace the current profile with the
_HYPER(PID_DEFAULT_PROFILE,default profile file).
You would want to replace the current profile to restore
_PMSPY back to its startup state.
.*
.*************************************************************************
:h1 _RES(ID_F_APPENDDEFAULT)Open, append to current
.*************************************************************************
:i2 refid=iPRO.Open defaults, append to current
:i2 refid=iMSGFIL.Open defaults, append
.*
This pull-down allows you to append the current profile with the
_HYPER(PID_DEFAULT_PROFILE,default profile file).
This would add the standard set of PM messages.  Since _PMSPY
has already defined the standard messages, this is one less thing that
you as an application developer have to worry about.
.*
.*************************************************************************
:h1 _RES(ID_O_BROWSEDEFAULT)Browse
.*************************************************************************
:i2 refid=iPRO.Browse default
:i2 refid=iPRODEF.Browse
:i2 refid=iMSGFIL.Browse defaults
.*
This pull-down allows you to browse the
_HYPER(PID_DEFAULT_PROFILE,default profile file).
This provides a handy reference to all of the standard PM messages
(including their hex values).
.*
.*************************************************************************
:h1 _RES(ID_F_EXIT)Exit
.*************************************************************************
.*
This pull-down allows you to exit _PMSPY.
:note.If you select this pull-down,
the current _HYPER(PID_SAVE,agent characteristics) are saved.

.*
.*------------------------------------------------------------------------
:h1 _RES(ID_AB_SPYON)Spyon
.*------------------------------------------------------------------------
:p.This item allows you to&colon.
:ul compact.
.*
:li.Select the window to _ITALIC(spy) on
:li.Stop _ITALIC(spying) on a window
.*
:eul.
.*
.*************************************************************************
:h1 _RES(ID_SELECT)Select desktop window
.*************************************************************************
This pull-down allows you to select a visible window from the desktop
for _ITALIC(spying).
The pointer changes to a _ITALIC(cross-hair) to indicate that window selection
is in progress.
If mouse moves over the current _PMSPY window,
the pointer changes to an _ITALIC(illegal) pointer.
:p.Position the pointer over the window you wish to _ITALIC(spy)
and use one of the following mouse actions to complete the selection process:
.*
:table rules=both cols='7 20'.
.*
:row.
:c.Mouse action
:c.What happens then
.*
:row.
:c.single click button &numsign.1
:c.The window and queue handles will be retrieved for that window and
spying will begin as messages move through the queue or are sent to the window.
The window or queue handle will be displayed in the _PMSPY titlebar.
.*
:row.
:c.single click button &numsign.2
:c.Reveals the class name, window and queue handles for the window
beneath the cursor.
The selection process remains active.  This is a handy way to
explore the current set of windows.
.*
:row.
:c.double click button &numsign.2
:c.Cancels the selection of a window.
.*
:etable.
.*
.*************************************************************************
:h1 _RES(ID_SELECT_OBJECT)Select any window
.*************************************************************************
:p.This pull-down displays a
_HYPER(ID_OW_DLG,list of windows)
who are descendents of HWND_DESKTOP and/or HWND_OBJECT.
A window or queue can then be selected list for _ITALIC(spying).
.*
.*************************************************************************
:h1 _RES(ID_SELECT_SYSTEM_Q)System message queue
.*************************************************************************
:p.This pull-down allows you to
select the PM system message queue for _ITALIC(spying).
This allows you to _ITALIC(spy) all windows and application
message queues at once.
SYSTEM_Q_WARNING
.*
.*************************************************************************
:h1 _RES(ID_DESELECT)Deselect current
.*************************************************************************
This pull-down allows you to stop _ITALIC(spying)
on the selected window or queue.
.*
.*------------------------------------------------------------------------
:h1 _RES(ID_AB_SPYWHAT)SpyWhat
.*------------------------------------------------------------------------
:p.This item allows you to&colon.
:ul compact.
:li._ITALIC(Spy on) the selected window only
:li._ITALIC(Spy on) all windows supported by the current message queue
.*
:eul.
.*
.*************************************************************************
:h1 _RES(IDD_WND)Spy on window
.*************************************************************************
This pull-down limits the _ITALIC(spying)
of messages to only those destined for the current window or OBJECT.
Any message not destined for that specific window is ignored.
.*
.*************************************************************************
:h1 _RES(IDD_QUE)Spy on queue
.*************************************************************************
This pull-down expands the _ITALIC(spying) scope
to include messages for all windows supported by the
current message queue.
This capability to examine all windows is required if you desire to
debug _HYPER(PID_HOWTO_DIALOGS,dialogs).
.*
.*------------------------------------------------------------------------
:h1 _RES(ID_AB_OPTIONS)Options
.*------------------------------------------------------------------------
:p.This item allows you to&colon.
:ul compact.
:li.Control the _HYPER(PID_SPYLIST,message list)
:li.Control the _HYPER(PID_R_LOG_FILE,log file)
:li.Add a _HYPER(DLG_NOTES,text note)
:eul
.*
.*************************************************************************
:h1 _RES(ID_O_LIST)Spy list
.*************************************************************************
This set of pull-downs allow you to control
_HYPER(PID_SPYLIST,message list) operations.
.*
.*************************************************************************
:h1 _RES(ID_O_CLEAR)Clear
.*************************************************************************
:i2 refid=iLIST.Clear
.*
This pull-down clears all of the messages and/or notes from the
_HYPER(PID_SPYLIST,message list).
:p.You may want to clear the list as part of the setup for debugging
a special problem.
.*
.*************************************************************************
:h1 _RES(ID_O_FREEZE)Freeze
.*************************************************************************
:i2 refid=iLIST.Freeze
.*
This pull-down _ITALIC(freezes) the
_HYPER(PID_SPYLIST,message list).
This means that no new messages will be automatically added to the
list until either:
:ol compact.
:li.you manually _HYPER(ID_O_THAW,thaw) the message list
:li.a _HYPER(PID_TRIGGER,trigger message) automatically thaws the message list
:eol.
.*
.*************************************************************************
:h1 _RES(ID_O_THAW)Thaw
.*************************************************************************
:i2 refid=iLIST.Thaw
.*
This pull-down _ITALIC(thaws) a previously frozen
_HYPER(PID_SPYLIST,message list).
This means that new, eligible messages will again be automatically added to the
list until either:
:ol compact.
:li.you manually _HYPER(ID_O_FREEZE,freeze) the message list
:li.a _HYPER(PID_TRIGGER,trigger message) automatically freezes
the message list
:eol.
.*
.*************************************************************************
:h1 _RES(ID_O_SCROLL)Auto scroll?
.*************************************************************************
:i2 refid=iLIST.Auto scroll?
.*
This pull-down represents the _ITALIC(automatic scrolling)
state of
the _HYPER(PID_SPYLIST,message list).
.*
:p.When _BOLD(checked), the message list will automatically scroll to
keep the last added message visible.  This is the default setting.
.*
:p.When _BOLD(not checked), the list does not
automatically scroll to keep the last added message visible.
This is useful when you need to examine the messages that have previously
been added to the list and do not want to be bothered by the automatic scrolling
that occurs when a new message is subsequently added to the list.
.*
.*************************************************************************
:h1 _RES(ID_O_FONT)Font
.*************************************************************************
:i2 refid=iLIST.Font
.*
This pull-down presents the
_HYPER(DLG_FONT,Font dialog) so that you can select
the non-proportional font used by
the _HYPER(PID_SPYLIST,message list) and dialogs.
.*
.*************************************************************************
:h1 _RES(ID_O_LOG)Log file
.*************************************************************************
This set of pull-downs allow you to control
_HYPER(PID_R_LOG_FILE,log file) operations.
.*
.*************************************************************************
:h1 _RES(ID_O_STOPLOG)Freeze
.*************************************************************************
:i2 refid=iLOG.Freeze
.*
This pull-down _ITALIC(freezes) the
_HYPER(PID_R_LOG_FILE,log file).
This means that no new messages are automatically written to the
file (even though they are added to the
_HYPER(PID_SPYLIST,message list)).
.*
.*************************************************************************
:h1 _RES(ID_O_STARTLOG)Thaw
.*************************************************************************
:i2 refid=iLOG.Thaw
.*
This pull-down _ITALIC(thaws) a previously frozen
_HYPER(PID_R_LOG_FILE,log file).
This means that new messages are also automatically written to the
file (as well as being added to the
_HYPER(PID_SPYLIST,message list)).
.*
.*************************************************************************
:h1 _RES(ID_O_NOTES)Add note
.*************************************************************************
:i2 refid=iLIST.Add note
:i2 refid=iLOG.Add note
.*
This pull-down presents the
_HYPER(DLG_NOTES,Notes dialog) so that you can
enter a text note for addition to the
_HYPER(PID_SPYLIST,message list) and possibly the
_HYPER(PID_R_LOG_FILE,log file).
:p.You can use notes to document scenario setups, debugging notes, etc
for later reference or debugging assistance.
.*
.*************************************************************************
:h1 _RES(ID_O_SAVEOPTS)Save options
.*************************************************************************
:i2 refid=iSPY.Saving options
.*
This pull-down saves this _PMSPY agent&apos.s current
options in
_HYPER(PID_SAVE,PMSPY.INI).
.*
.*------------------------------------------------------------------------
:h1 _RES(ID_AB_FILTER)Filter
.*------------------------------------------------------------------------
:p.This item allows you to&colon.
:ul compact.
:li.Control the _ITALIC(spying) of profile defined messages
(on a group or individual message basis)
:li.Control the _ITALIC(spying) of undefined messages
:eul.
.*
.*************************************************************************
:h1 _RES(ID_F_GROUP)By group
.*************************************************************************
This pull-down presents the
_HYPER(ID_GRP_DLG,Group Filtering) dialog
so that you can select one or more _BOLD(groups) of messages.
.*
:p.Each group represents one or more individual messages that are logically
related to each other (for instance, all titlebar messages, etc).
Message groups are defined by _PMSPY in the
_HYPER(PID_DEFAULT_PROFILE,default profile)
or by you in a _HYPER(PID_R_PROFILE,profile file).
.*
.*************************************************************************
:h1 _RES(ID_F_SELECTIVE)By message
.*************************************************************************
This pull-down presents the
_HYPER(IDD_FILTER,Message Filtering) dialog
so that you can select one or more _BOLD(messages).
Messages are defined by _PMSPY in the
_HYPER(PID_DEFAULT_PROFILE,default profile)
or by you in a
_HYPER(PID_R_PROFILE,profile file).
.*
.*************************************************************************
:h1 _RES(ID_F_EXCLUDE)Exclude all messages
.*************************************************************************
This pull-down automatically excludes all defined messages (and groups)
from being included in the
_HYPER(PID_SPYLIST,message list).
By itself, this function is not very useful.  But when used before the
_HYPER(ID_GRP_DLG,Group Filtering) dialog or the
_HYPER(IDD_FILTER,Message Filtering) dialog
it provides a quick method for placing the message filtering controls
into a known state before setting up special filter combinations.
.*
.*************************************************************************
:h1 _RES(ID_F_INCLUDE)Include all messages
.*************************************************************************
This pull-down automatically enables the inclusion of all defined
messages (and groups) in the
_HYPER(PID_SPYLIST,message list).
By itself, this function is not very useful.  But when used before the
_HYPER(ID_GRP_DLG,Group Filtering) dialog or the
_HYPER(IDD_FILTER,Message Filtering) dialog
it provides a quick method for placing the message filtering controls
into a known state before setting up special filter combinations.
.*
.*************************************************************************
:h1 _RES(ID_F_INC_UNDEFINED)Include undefined messages?
.*************************************************************************
:i2 refid=iHOWTO.Spy on undefined messages
.*
This pull-down allows you to _BOLD(spy) on messages
with a value less than WM_USER that have not
been defined in the current
_HYPER(PID_R_PROFILE,profile file).
This allows you to spy on other applications and see the non-standard
messages that are used (for example, the messages used with a program
group dialog when items are selected and dragged from one line to another).
The _HYPER(ID_F_INC_WM_USER,Include undefined WM_USER)
pull-down allows you to just include undefined messages that begin at the
WM_USER value.
.*
:p.The following special format is used when displaying undefined messages&colon.
:xmp.
WM_???? (0303) 01B40372 00010001 S 03300CC8
:exmp.
.*
:p.To examine just the undefined messages, first use the
_HYPER(ID_F_EXCLUDE,Exclude all messages),
then select this pull-down item.
.*
.*************************************************************************
:h1 _RES(ID_F_INC_WM_USER)Include undefined WM_USER?
.*************************************************************************
:i2 refid=iHOWTO.Spy on undefined WM_USER messages
.*
This pull-down allows you to _BOLD(spy) on all messages defined
at or above the WM_USER value that have not been defined in the current
_HYPER(PID_R_PROFILE,profile file).
.*
This allows you to spy on your (or other) applications and see
the WM_USER messages being used without having to define a
special profile file.
.*
The _HYPER(ID_F_INC_UNDEFINED,Include undefined messages?)
pull-down allow you to include all undefined messages.
.*
:p.The following special format is used when displaying undefined
WM_USER messages&colon.
:xmp.
WM_USER (1010) 01B40372 00010001 S 03300CC8
:exmp.
.*
:p.To examine just the WM_USER messages, first use the
_HYPER(ID_F_EXCLUDE,Exclude all messages),
then select this pull-down item.
.*
.*------------------------------------------------------------------------
:h1 _RES(ID_AB_HELP)Help
.*------------------------------------------------------------------------
:p.This item allows you to&colon.
:ul compact.
:li.Obtain operational help on using _PMSPY
:li.Display _PMSPY product information
:eul.
.*
.*************************************************************************
:h1 _RES(ID_H_HELP)Help for help
.*************************************************************************
This pull-down provides help on using the online help system.
.*
.*************************************************************************
:h1 _RES(ID_H_KEYS)Keys help
.*************************************************************************
This pull-down provides information about the various keyboard keys
and their usage.
.*
.*************************************************************************
:h1 _RES(ID_H_EXTENDED)Extended help
.*************************************************************************
This pull-down provides extended help about _PMSPY.
.*
.*************************************************************************
:h1 _RES(ID_H_INDEX)Extended index
.*************************************************************************
This pull-down provides an index of all help topics.
.*
.*************************************************************************
:h1 _RES(ID_H_ABOUT)Product Information
.*************************************************************************
This pull-down provides information about this version of _PMSPY.

.*************************************************************************
.* PMSPY - Message Help
.*************************************************************************
.*
.*************************************************************************
:h1 _RES(IDS_MSG_IPF_INIT_1)WinCreateHelpInstance error
.*************************************************************************
:i2 refid=iMsg.WinCreateHelpInstance error
.*
:p.An error was detected during a WinCreateHelpInstance call.
.*
.*************************************************************************
:h1 _RES(IDS_MSG_IPF_INIT_2)WinAssociateHelpInstance error
.*************************************************************************
:i2 refid=iMsg.WinAssociateHelpInstance error
.*
:p.An error was detected during a WinAssociateHelpInstance call.
.*
.*************************************************************************
:h1 _RES(IDS_MSG_IPF_ERROR_1)IPF Processing error
.*************************************************************************
:i2 refid=iMsg.IPF processing error
.*
:p.An IPF error was detected.
.*
.*************************************************************************
:h1 _RES(IDS_MSG_TOO_MANY_SPIES)Too many PMSPY instances
.*************************************************************************
:i2 refid=iMsg.Too many PMSPY instances
.*
:p.There are already _AGENTS _PMSPY agents active.  No more can be started
at this time.  Re-use one of current _PMSPY instances to process a new
message queue or window.
.*
.*************************************************************************
:h1 _RES(IDS_FMT_LEVEL)Executible and "Hook" DLL Incompatible
.*************************************************************************
:i2 refid=iMsg.Executable and "Hook" DLL incompatible
.*
:p.The internal versions of PMSPY.EXE and PMSPYHK.DLL are not compatible.
Ensure that the _BOLD(LIBPATH) and
_BOLD(PATH) environment variables locate compatible
files.  If necessary, re-install _PMSPY.
.*
.*************************************************************************
:h1 _RES(IDS_MSG_LOG_EXISTS)Log file already exists
.*************************************************************************
:i2 refid=iMsg.Log file already exists
.*
:p.The _BOLD(log file) you selected already exists.  This message
requests verification before replacing the existing file.
.*
.*************************************************************************
:h1 _RES(IDS_MSG_PROFILE_EXISTS)Profile file already exists
.*************************************************************************
:i2 refid=iMsg.Profile file already exists
.*
:p.The _BOLD(profile file) you selected for saving already exists.
This message
requests verification before replacing the existing file.
.*
.*************************************************************************
:h1 _RES(IDS_MSG_SYSTEM_Q_VERIFY)Spying on the PM system queue
.*************************************************************************
.*
:p.This message requests verification before beginning to _ITALIC(spy)
on the PM system message queue.
SYSTEM_Q_WARNING
.*
.*************************************************************************
.* PMSPY - Reference Help
.*************************************************************************
.*
.*************************************************************************
:h1 _RES(PID_R_PROFILE)Profile file
.*************************************************************************
:i2 refid=iPRO.syntax
:i2 refid=iPRO.definition
.*
:p.An optional profile can be used to define colors
and message filtering options. The file is interpreted sequentially.
One advantage of creating a profile for your application is that you
define the message name to have the same mnemonic value as you used when
you coded it (for example, WM_STARTUP_PROCESSING)  It is even
possible to automate the generation of application specific profiles
to ensure that they remain consistent with your application code
as it changes during the development cycle.
:p.For an example, look at the
_HYPER(PID_DEFAULT_PROFILE,default profile).
.*
:xmp.
X       <n>                                 /* x position of window */
Y       <n>                                 /* y position of window */
CX      <n>                                 /* width of window */
CY      <n>                                 /* height of window */

SPYON   <WINDOW> | <QUEUE>                  /* spy on a window or a queue */

LIST    <THAW> | <FREEZE>                   /* the list is thawed or frozen */

FONT    <&cdq.10.System Monospaced&cdq.>            /* the font to use in the listbox */

COLOR   <color> <SYSCLR_xxx> | <CLR_xxx>    /* define own Color */

GROUP   <GroupID> [<color>]                 /* define Message Group */
DEFINE  <MsgID> <MsgNo> [<color>]           /* define Message in Group */

INCLUDE ALL | <GroupID> | <MsgID>           /* Include message(s) */
EXCLUDE ALL | <GroupID> | <MsgID>           /* Exclude message(s) */

TRIGGER <MsgID>     <THAW> | <FREEZE>       /* trigger message(s) */

/* | * | <Null>                             /* comment lines ignored */

* <color> is 1..8 (1=default color; omit to leave default color ASIS)

                   1=color used for undefined messages
                   2=color used for undefined WM_USER messages
                   7=color used for NOTES
                   8=color used for _PMSPY status messages
* <MsgID> is a blank delimited "word" (optionally it may be enclosed in
                                       double quotes)
* <MsgNo> is a decimal, hex, or octal (for those DEC fans) number.
  Simple addition is also supported: WM_USER+5
* <GroupID> is a blank delimited "word" (optionally it may be enclosed in
                                         double quotes)
:exmp.
.*
.*************************************************************************
:h1 _RES(PID_R_CMDLINE)Command Line
.*************************************************************************
:i2 refid=iSPY.command line parameters
:p.The following command line parameters are all optional and case insensitive.
They are processed as they are found (left-to-right).
:xmp.
_PMSPY [/PRO=file] [/APP=file]
:exmp.
:dl tsize=14 compact.
:dt./PRO=file
:dd.reads <file> as a profile and _BOLD(replaces) the default profile.
.*
:dt./APP=file
:dd.reads <file> as a profile and _BOLD(appends) it to the default profile.
:edl.
.*
:p.Combinations of _BOLD(/PRO=)
_BOLD(/APP=) may be used on one invocation to setup
special filtering situations.  For example, the following invocation
establishes an application specific filter set composed of two groups
of messages (without using the &apos.standard&apos. PM message set):
.*
:xmp.
_PMSPY /PRO=app_set1.pro /APP=app_set2.pro
:exmp.
:p.The first _BOLD(/PRO=) "clears" the default message filtering set
then loads the
message groups defined in APP_SET1.PRO.
The _BOLD(/APP=) then adds the additional
messages groups defined in APP_SET2.PRO.
.*
.*************************************************************************
:h1 _RES(PID_R_LOG_FILE)Log file
.*************************************************************************
:i2 refid=iLOG.definition
This file can be used to save the contents of the
_HYPER(PID_SPYLIST,message list) on disk.  All
messages and _HYPER(DLG_NOTES,text notes)
are written to the log file if one has been opened.
:p.One advantage of using a log file is that you can view _BOLD(all)
of the messages that have occurred.  Due to listbox limitations, the oldest
messages are deleted from the listbox to make room for the ones currently
occurring.
.*
.*************************************************************************
:h1 _RES(PID_KEYSHELP)Keys Help
.*************************************************************************
:p.All OS/2 keys are functional while you are using _PMSPY.
See the OS/2 keys help for a complete list of these keys.
:p.In addition, _ITALIC(shortcut keys) have been defined for selected
_PMSPY actions.  These are indicated when an action bar item is
selected and its pull-downs are displayed.
.*
.*************************************************************************
:h1 _RES(PID_HELPHELP)Help for Help
.*************************************************************************
.*
:p.Help is always available.
The type of help you get is determined by
the item selected when you requested help.
.*
:p.When you ask for
help, you will see a window containing help information.
You can size or move this window like any other.
You can also leave the help window visible while you do the task.
.*************************************************************************
:euserdoc.
