// dbprintf.h  --  defines the DBPRINTF(()) macro

/*

The printf() function alone has debugged many small programs, so its
usefulness in large programs should not be overlooked.  PMWIN.DLL and
PMDD.SYS provide an interface for threads operating at ring 3 to print text
to the debug terminal.

The PMWIN API is called DebugOutput.  It takes a single parameter, a
pointer to the null-terminated string to print.  DebugOutput() does no
formatting; therefore, call vsprintf first to format, then call
DebugOutput.

The DBPRINTF macro uses conditional compilation so that printfs work in the
debug build and disappear in the retail build. The code is conditional on
the symbol NDEBUG in manner of the ANSI C assert macro. Parameters to
DBPRINTF(()) must be surrounded by two pairs of parenthesis because C
macros do not accept a variable number of parameters.

DBPRINTF.C contains the worker routine behind the DBPRINTF(()) macro. The
dbprintf() subroutine first calls vsprintf() to do formatting, then it
calls the PMWIN API DebugOutput.



             RESOLVING DEBUGOUTPUT AT LINK TIME


You must import the API DebugOutput in your DEF file. The 32-bit entry
resides at ordinal 955 in both retail and debug versions of PMWIN.DLL. (A
16-bit FAR PASCAL DebugOutput exists at ordinal 511.)

    IMPORTS
       DebugOutput = PMWIN.955

Finally on the debuggee, add the /C2 switch to the DEVICE=PMDD.SYS line in
CONFIG.SYS. Use /C2 when the debugger is using COM2 or /C1 for COM1.

    DEVICE=C:\OS2\PMDD.SYS /C2


*/



#ifndef NDEBUG
  // symbolic NDEBUG is not defined; build debug version
  void APIENTRY DebugOutput( PSZ );
  void _Optlink dbprintf( PSZ pszFormat, ... );
  #define DBPRINTF(str) dbprintf str
#else
  // building retail version; DBPRINTF(()) is empty
  #define DBPRINTF(str)
#endif
