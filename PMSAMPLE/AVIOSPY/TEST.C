#define INCL_DOS
#define INCL_KBD
#include <stdio.h>
#include <os2.h>
KBDKEYINFO Info;
HKBD  Kbd;
void cdecl main() {
   int ch,rc;
   printf("Wait\n");
//   ch=getch();
   rc=KbdOpen(&Kbd);
   printf("Open rc=%d\n",rc);
   rc=KbdGetFocus(0,Kbd);
   printf("Focus rc=%d\n",rc);
   rc=KbdCharIn(&Info,0,Kbd);
   printf("Char in rc=%d\n",rc);
   printf("Status  = %2.2X\n",Info.fbStatus);
   printf("Shift   = %4.4X\n",Info.fsState);
   printf("Scancode= %2.2X\n",Info.chScan);
   printf("Char    = %2.2X\n",Info.chChar);
   rc=KbdClose(Kbd);
   printf("Close  rc=%d\n",rc);

}
