MSHELL: Mini PM Shell for OS/2                    MONTE COPELAND

(c) Copyright International Business Machines Corporation 1992.
All rights Reserved.


REVISION HISTORY:
--------------------------
20OCT94: IBM Employee Written Software release
16OCT93: memorize icon place
09OCT93: fix bug that started everything in the background
21SEP93: revised readme; rebundle startdos
27AUG93: startdos its own pkg; mshell.exe with new default INI
06MAY93: new startdos.exe; mshell.exe the same
20JAN93: included separate startdos.exe utility which lets users
         specify dos settings prior to dosstartsession
14JAN93: debug code out; illustrate use of & and CMD.EXE;
         creates a default INI if none present.
14SEP92: 32-bit; WinShutdownSystem; visual interface to the spooler;
         task list; debug code left in.
26JUN92: auto-starting of programs
14MAY92: same function; clean up source; setfocus to listbox; remove
         debug assertion code.
27MAR92: initial placement on os2tools



INTRODUCTION:
-------------

MShell is an alternative, simple, mini shell for OS/2 2.X that
uses the replaceable shell architecture of the Workplace Shell.
With this architecture and their own EXE, programmers can easily
make OS/2 a turnkey platform.

MShell is a program launcher. MShell provides one list of
programs to start; you configure these programs by editing a
plain text file called MSHELL.INI. In addition to starting
programs, MShell can save the desktop, interact with the spooler,
and do a system shutdown.

Because MShell has limited function, you have to install OS/2 PM
printer drivers when Workplace Shell is booted. Once installed,
MShell can manipulate print queues. To boot one shell or the
other, change the RUNWORKPLACE line in CONFIG.SYS and reboot.

MShell is IBM Employee Written Software. Please read the "as-is"
license agreement from IBM. Essentially, you use this program "as
is," and IBM makes no warranty about the correctness of this
program or its suitability to any purpose.

MShell is a sample program. MShell is written in IBM C Set/2, and
source code is available on Developer Connection for OS/2 CD
Volume 3.

Monte Copeland, IBM Boca Raton.






MSHELL INSTALLATION:
--------------------------

1) MShell requires OS/2 2.0 or later.

2) Copy MSHELL.EXE into the root directory of the boot drive.

3) Edit CONFIG.SYS and modify the SET for RUNWORKPLACE to name
   MSHELL.EXE. Example:

   SET RUNWORKPLACE=C:\MSHELL.EXE

   If you boot OS/2 from a drive other than C:, modify RUNWORKPLACE
   accordingly. When MSHELL.EXE runs, it looks for MSHELL.INI in the
   root directory of the boot drive. If there is no MSHELL.INI,
   MShell will create one for you.

4) Reboot.






CUSTOMIZING MSHELL:
--------------------------

MSHELL.INI contains program-start information.  MShell reads the INI
file, displays the program information in a listbox, and starts those
programs at the user's request.  Unlike most INI files, MSHELL.INI is
plain text.  Use a text editor when changing MSHELL.INI.

There are two parts to an MSHELL.INI line:

  1) the program title to display

  2) a START command acceptable to OS/2's command processor CMD.EXE.

Separate these two parts with a single semi-colon (;). Here is an
example MSHELL.INI line to start the Klondike Solitaire program:

      Solitaire ;  start /f "Solitaire" klondike.exe

Lines in the INI that begin with exclamation mark (!) will be
automatically started by MShell when the system boots. For example:

      !Clock ;  start pmclock

To start programs from other than the root directory, use the
command concatenator of CMD.EXE, the ampersand (&). Example INI
line:

   Save Changed Files;  d: & cd \wp & xcopy *.* a: /m

Comment lines in MSHELL.INI begin with an asterisk (*).

MShell can start up to 50 programs or batch files.  This is an MShell
limit, not OS/2.




MSHELL OPTIONS:
----------------------

There are five options that appear in the Options pull-down:

Print spooler:  presents a dialog which allows interaction with the
OS/2 spooler.  Pause, resume, and delete jobs.  Pause and resume print
queues.

Save desktop:  will broadcast a posted WM_SAVEAPPLICATION to all
children of HWND_DESKTOP, the desktop window.  Most PM applications
will then save their current settings to the INI file.

Refresh programs in INI:  allows you to re-load MSHELL.INI at any
time.  This is good for testing your modifications to MSHELL.INI
without having to reboot.

Command prompt:  starts a windowed OS/2 command prompt (CMD.EXE)
session.

Shutdown system: prompts the user to confirm the shutdown.
Respond Yes, wait until the "safe" message, then turn off or
reboot the computer.




STARTDOS AND DOS SETTINGS:
------------------------------

Certain DOS settings can only be specified prior to DOS session
start. Since the Workplace Shell DOS settings dialog is not
available under MShell, one needs a way to start these
special DOS sessions.

StartDos is an OS/2 program that programmatically starts DOS
sessions with settings. StartDos accomplishes this using Rexx.
StartDos has a built-in readme. Enter one of these commands:

      STARTDOS >README
      STARTDOS | MORE

Obtain the STARTDOS package from OS2TOOLS or STARTD.ZIP from
OS2USER Library 17.




KNOWN BUGS:
----------------------

On OS/2 2.1, MShell's shutdown does not write the cached INI file
to disk before shutdown is complete. On reboot, recently saved
settings are gone. Workaround: do a Save Desktop, wait 60 seconds
(until the disk light flashes), then shutdown.




SPOOLER NOTES:
---------------------

MShell, or any RUNWORKPLACE executable, must start the spooler.
MShell will initialize the spooler and provide a rudimentary
visual interface to the spooler.

To initialize the spooler, 32-bit MShell calls the OEM API
Spl32QmInitialize. The function prototype is

  VOID APIENTRY Spl32QmInitialize( PULONG );

An example of calling is

  Spl32QmInitialize( &ulrc );

On success, expect ulrc to be zero.  MShell imports this API with
this IMPORTS statement in its DEF file.

  IMPORTS
    SPL32QMINITIALIZE  =  PMSPL.SPL32QMINITIALIZE





LICENSE INFORMATION:
----------------------
Please read the file LICENSE.TXT for IBM licensing information.
