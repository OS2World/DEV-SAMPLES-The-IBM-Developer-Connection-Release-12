#define INCL_BASE
#define INCL_WIN
#include <stdio.h>
#include <os2.h>
main(argc, argv, envp)
   int argc;
   char *argv[];
   char *envp[];
{
  FILE *Input;
  FILE *Output;
  QMSG Qmsg;
  char Scanb[20];
  char buffer[80];
  char outbuffer[80];
  printf("TRANS Takes the journalled records in RECORD.JRN\n");
  printf("and translates them to readable data in RECORD.TRN\n");
  printf("Copy your journal file to RECORD.JRN if it was not its name \n");
  Input =fopen("RECORD.JRN","rb");
  Output=fopen("RECORD.TRN","w");
  fprintf(Output,"flag KC_CHAR        :%4.4x \n",KC_CHAR);
  fprintf(Output,"flag KC_SCANCODE    :%4.4x \n",KC_SCANCODE);
  fprintf(Output,"flag KC_VIRTUALKEY  :%4.4x \n",KC_VIRTUALKEY);
  fprintf(Output,"flag KC_KEYUP       :%4.4x \n",KC_KEYUP     );
  fprintf(Output,"flag KC_PREVDOWN    :%4.4x \n",KC_PREVDOWN  );
  fprintf(Output,"flag KC_DEADKEY     :%4.4x \n",KC_DEADKEY   );
  fprintf(Output,"flag KC_COMPOSITE   :%4.4x \n",KC_COMPOSITE );
  fprintf(Output,"flag KC_INVALIDCOMP :%4.4x \n",KC_INVALIDCOMP );
  fprintf(Output,"flag KC_LONEKEY     :%4.4x \n",KC_LONEKEY     );
  fprintf(Output,"flag KC_SHIFT       :%4.4x \n",KC_SHIFT       );
  fprintf(Output,"flag KC_ALT         :%4.4x \n",KC_ALT         );
  fprintf(Output,"flag KC_CTRL        :%4.4x \n",KC_CTRL        );
  while (fread((CHAR *)&Qmsg,sizeof(QMSG),1,Input)!=0) {
     switch (Qmsg.msg) {
#include "mess.h"
        break;
     default:
        sprintf(buffer,"??? %8X",Qmsg.msg);
     } /* endswitch */
     if (Qmsg.msg==WM_CHAR) {
        fprintf(Output,"Flags = %4.4x  , Repeat= %2.2x , Scancode=%2.2x , Ch = %4.4x , Vk = %4.4x \n",
                       SHORT1FROMMP(Qmsg.mp1),CHAR3FROMMP(Qmsg.mp1),CHAR4FROMMP(Qmsg.mp1),
                       SHORT1FROMMP(Qmsg.mp2),SHORT2FROMMP(Qmsg.mp2));
     } else {
     } /* endif */
     fprintf(Output,"%20.20s %8.8lX %8.8lX \n",buffer,Qmsg.mp1,Qmsg.mp2);

  } /* endwhile */


  fclose(Input);
  fclose(Output);
}
