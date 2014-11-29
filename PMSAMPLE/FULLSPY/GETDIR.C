#include <stdio.h>
#include <stdlib.h>
#define INCL_BASE
#define INCL_DOS
#include <os2.h>
typedef struct {
                 PID   pid;
                 CHAR  Directory[256];
               } DIRINFO;
void cdecl main(int argc,char *argv[] ) {
    USHORT rc;
    PID    Proc;
    HSYSSEM   Sem1,Sem2;
    DIRINFO * DirInfo;
    SEL       DirSel;
    if ( argc > 1 ) {
       Proc=atoi(argv[1]);
       printf("Querying Process id %d\n",Proc);
       rc=DosOpenSem(&Sem1,"\\SEM\\FULLSPY.SEM1");
       rc=DosOpenSem(&Sem2,"\\SEM\\FULLSPY.SEM2");
       rc=DosGetShrSeg("\\SHAREMEM\\FULLSPY.DAT",&DirSel);
       DirInfo=MAKEP(DirSel,0);
       DirInfo->pid=Proc;
       DosSemSet((HSEM)Sem1);
       DosSemClear((HSEM)Sem2);
       DosSleep(500L);
       DosSemSet((HSEM)Sem2);
       DosSemClear((HSEM)Sem1);
       DosSemWait((HSEM)Sem2,-1L);
       printf("Directory is %s\n",DirInfo->Directory);

    } else {
       printf("No Process id entered\n");
    }
}
