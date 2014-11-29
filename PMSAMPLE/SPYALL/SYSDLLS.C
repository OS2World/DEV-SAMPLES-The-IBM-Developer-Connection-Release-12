#define INCL_WINSHELLDATA
#include <os2.h>
#include <stdio.h>
#include <string.h>
ULONG Length;
USHORT rc;
void cdecl main() {
    CHAR *pBuffer;
    CHAR *pBuffer1;
    CHAR *Token;
    BOOL Found=FALSE;
    SEL  Sel,Sel1;
   PrfQueryProfileSize(HINI_PROFILE,"SYS_DLLS","Load",&Length);
   rc=DosAllocSeg(((USHORT)Length)+strlen(" SPYALL")+1,&Sel,SEG_NONSHARED);
   rc=DosAllocSeg(((USHORT)Length)+strlen(" SPYALL")+1,&Sel1,SEG_NONSHARED);
   pBuffer=MAKEP(Sel,0);
   pBuffer1=MAKEP(Sel1,0);
   pBuffer[0]='\0';
   pBuffer1[0]='\0';
   Length=PrfQueryProfileString(HINI_PROFILE,"SYS_DLLS","Load",
                            "", pBuffer,(ULONG)Length);
   Token=strtok(pBuffer," ");
   while (Token!=NULL) {
      printf("Dll Name=%s\n",Token);
      if (strcmp("SPYALL",Token)==0) {
         printf("Removing SPYALL\n");
         Found=TRUE;
      } else {
         strcat(pBuffer1,Token);
      } /* endif */
      Token=strtok(NULL," ");
   } /* endwhile */
   if (Found==FALSE) {
      strcat(pBuffer1," SPYALL");
      printf("Adding SPYALL\n");
   } /* endif */
   Length=PrfWriteProfileString(HINI_PROFILE,"SYS_DLLS","Load",pBuffer1);
   DosFreeSeg(Sel1);
   DosFreeSeg(Sel);
}
