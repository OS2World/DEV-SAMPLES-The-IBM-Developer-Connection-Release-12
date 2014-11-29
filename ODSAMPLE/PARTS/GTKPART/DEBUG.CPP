// for the sake of a strcpy, include string.h
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define INCL_WIN
#define INCL_BASE
//#define INCL_DOSERRORS

//#ifndef _ODTYPES_
//#include "ODTypes.h"
//#endif
#define INCL_ODAPI
#include <os2.h>

#include "debug.hpp"

extern "C" {
   int staticlinkdebughelper;

   //--------------------------------------------------------
   VOID EXPENTRY fappend(char* FileName, char* Data)
   {
#if 0   // [133493] ced  - use somPrintf so output can be redirected
      HFILE NewFileHandle;
      ULONG ActionTaken=1;
      ULONG BytesWritten;
   
      ULONG  NewLocation;
      if(DosOpen((PSZ)FileName,&NewFileHandle,&ActionTaken,0L,0,0x11,0x00A1,0L)) return;
      DosChgFilePtr(NewFileHandle,0L,2,&NewLocation);
   
        {
        ULONG size =strlen((char*)&Data[0]);
        DosWrite(NewFileHandle,
                 (char*)&Data[0],
                 size,
                 &BytesWritten);
        }
   
      DosClose(NewFileHandle);
#endif
      somPrintf("%s", Data);
   }
   #ifdef SYNCH_CLIB
      CHAR CLibrary;
   #endif
   
   //--------------------------------------------------------
   VOID EXPENTRY vfappendf(char* FileName, char* Format, va_list args)
   {
      char string[270];
   
      string[0]='\0';
      #ifdef SYNCH_CLIB
         EnterMonitor(&CLibrary, 1);
         vsprintf(string,Format,args);
         ExitMonitor(&CLibrary);
      #else
         vsprintf( (char*)string,Format,args);
      #endif
      fappend( FileName, string);
   }
   
   //--------------------------------------------------------
   VOID fprintf2(char* FileName, char* Format, ...)
   {
      va_list args;
   
      va_start(args, Format);
         vfappendf(FileName, Format, args);
      va_end(args);
   }
   
   //--------------------------------------------------------
   VOID EXPENTRY fappenddate( char* FileName)
   {
      DATETIME DateTime;
      USHORT rc;
   
      rc = DosGetDateTime(&DateTime);
      fprintf2(FileName, "[%d-%02d-%02d %2d:%02d:%02d] ",
                      DateTime.year,
                      DateTime.month,
                      DateTime.day,
                      DateTime.hours,
                      DateTime.minutes,
                      DateTime.seconds);
   }
   
   
   
   //--------------------------------------------------------
   VOID rrprintf(char* Format, ...)    /*debug*/
   {
      va_list args;
      char xxx[300];
      strcpy( xxx, Format);
   
      {
         strcat( xxx, "\r\n");
         fappenddate("opendoc.log");
      }
      fappenddate("opendoc.log");
      va_start(args, Format);
         vfappendf("opendoc.log", xxx, args);
      va_end(args);
   }
   
   //--------------------------------------------------------
   VOID beepprintf(char* Format, ...)    /*debug*/
   {
      va_list args;
      char xxx[300];
      strcpy( xxx, Format);
   
      {
         strcat( xxx, "\r\n");
         fappenddate("opendoc.log");
      }
      va_start(args, Format);
         vfappendf("opendoc.log", xxx, args);
      va_end(args);
      DosBeep(200,30);
   }
   void EXPENTRY WinMBox( char* msg )
   {
      WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                 (PSZ)msg, /*caption:*/(PSZ)"", /*id?*/5,
                 MB_OK | MB_ERROR
                 );
   }
};

