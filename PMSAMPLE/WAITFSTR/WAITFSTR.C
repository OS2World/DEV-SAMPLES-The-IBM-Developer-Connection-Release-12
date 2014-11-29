#define  INCL_DOS
#define  INCL_WIN
#include <os2.h>
#define  INCL_RXSHV                     /* include shared variable    */
#define  INCL_RXMACRO                   /* include macrospace info    */
#include <rexxsaa.h>                   /* REXXSAA header information */
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
APIRET APIENTRY WaitForString(PID WinPid,PUCHAR String);
int             AddVar(PCHAR Data,int numvar);
HQUEUE QueueHandle=0;
FILE * Debug;
/* RxWaitForString */
LONG APIENTRY RxWaitForString(
   PUCHAR    func,
   ULONG     ac,
   PRXSTRING av,
   PSZ       que,
   PRXSTRING ret)
  {
     APIRET rc;
     PID    WinPid;
     if (ac==0) {
       strcpy(ret->strptr,"Error no Parms");   /*                            */
       ret->strlength=strlen(ret->strptr);     /*                            */
       return 1;
     } /* endif */
     /*--------------------------------------------------------------------*/
     if (ac>1) {
        WinPid=(PID)atoi(RXSTRPTR(av[1]));
     } else {
        WinPid=0;
     } /* endif */
     rc=WaitForString(WinPid,RXSTRPTR(av[0]));
     if (rc) {
         strcpy(ret->strptr,"Error Wait For string");  /*                  */
         ret->strlength=strlen(ret->strptr);     /*                        */
         return 1;
     } /* endif */
     /*--------------------------------------------------------------------*/
     strcpy(ret->strptr,"Found");            /*                            */
     ret->strlength=strlen(ret->strptr);     /*                            */
     return 0;
}
