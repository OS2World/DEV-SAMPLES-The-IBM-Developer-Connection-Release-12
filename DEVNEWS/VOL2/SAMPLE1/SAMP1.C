
Listing 4/Keenleyside

This listing shows you how to write a DLL entry point function
using the features of C Set ++.


/* From DLLENTRY.C */
#pragma strings(readonly)

#define INCL_DOSFILEMGR
#define INCL_DOSMODULEMGR
#include <os2.h>
#include <string.h>

int _dllentry = 1;  /* just in case an object is compiled with /Ge-
*/
char name[CCHMAXPATH];

#pragma entry(entry)
unsigned long _System entry(unsigned long hModule, unsigned long
ulFlag)
{
   APIRET rc;
   unsigned long ulBytesWritten;

   rc = DosQueryModuleName(hModule, CCHMAXPATH, name);

   if (!rc)
   {
      if (ulFlag == 0)
      {
         rc = DosWrite(1, name, strlen(name), &ulBytesWritten);
         rc = DosWrite(1, " initialized.\r\n", 15,
&ulBytesWritten);
      }
      else
      {
         rc = DosWrite(1, name, strlen(name), &ulBytesWritten);
         rc = DosWrite(1, " terminated.\r\n", 14, &ulBytesWritten);
      }
   }

   return !rc;   /* non-zero means DLL init/term was successful */
}

void hello(void)
{
   unsigned long ulBytesWritten;

   DosWrite(1, "Hello there\r\n", 13, &ulBytesWritten);

   return;
}

/* From SIMPLE.DEF */
LIBRARY SIMPLE INITINSTANCE TERMINSTANCE

EXPORTS
   hello

/* From RUNTIME.C */
#pragma strings(readonly)

#define INCL_DOSMODULEMGR
#define INCL_DOSPROCESS
#include <os2.h>

char pszErrorBuf[CCHMAXPATH];

void hello(void);

int main(void)
{
   APIRET rc;
   HMODULE hDLL;
   PFN pHello;

   rc = DosLoadModule(pszErrorBuf, CCHMAXPATH, "SIMPLE", &hDLL);

   if (!rc)
   {
      rc = DosQueryProcAddr(hDLL, 0, "hello", &pHello);

      if (!rc)
         pHello();

      rc = DosFreeModule(hDLL);
   }

   return rc;
}

/* From BUILD.CMD */
/* Build a simple DLL that shows how the DLL entry point function
works. */
'ICC /C /Rn /O+ DLLENTRY.C'
'ICC /Rn /Ge- /FeSIMPLE.DLL DLLENTRY SIMPLE.DEF'
'ICC /Rn /O+ RUNTIME.C'

