/**********************************************************************/
/*                                                                    */
/*  SYM                                                               */
/*                                                                    */
/* Sample C-Set** source code to parse .SYM files                     */
/**********************************************************************/
/* Version: 1.0             |   Marc Fiammante (FIAMMANT at LGEVMA)   */
/*                          |   La Gaude FRANCE                       */
/*                          |   Internet: fiammante@vnet.ibm.com      */
/**********************************************************************/
/* History:                                                           */
/* --------                                                           */
/*                                                                    */
/* created: Marc Fiammante November 1995                              */
/**********************************************************************/
#define INCL_BASE
#define INCL_DOS
#include <os2.h>
#include "sym.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
int main(int argc,char *argv[])
{
   FILE * SymFile;
   MAPDEF MapDef;
   SEGDEF   SegDef;
   SEGDEF *pSegDef;
   SYMDEF32 SymDef32;
   SYMDEF16 SymDef16;
   static char    Buffer[2560];
   int     SegNum,SymNum;
   unsigned long int SegOffset,SymOffset,SymPtrOffset;
   if (argc==1) {
      printf("No file name entered\n");
      printf("Syntax is SYM xxxx.sym\n");
      printf("where xxxx.sym is the symbol file name\n");
   } else {
      if (argv[1][0]=='?') {
            printf("Syntax is SYM xxxx.sym\n");
            printf("where xxxx.sym is the symbol file name\n");
      } else {
         SymFile=fopen(argv[1],"rb");
         if (SymFile==0) {
            perror("Error opening file");
            printf("Syntax is SYM xxxx.sym\n");
            printf("where xxxx.sym is the symbol file name\n");
            exit(99);
         } /* endif */
      } /* endif */
   } /* endif */
   fread(&MapDef,sizeof(MAPDEF),1,SymFile);
   SegOffset= SEGDEFOFFSET(MapDef);
   /* loop through executable objects (segments) */
   for (SegNum=0;SegNum<MapDef.cSegs;SegNum++) {
        /* printf("Scanning segment #%d Offset %4.4hX\n",SegNum+1,SegOffset); */
        if (fseek(SymFile,SegOffset,SEEK_SET)) {
           perror("Seek error ");
        }
        fread(&SegDef,sizeof(SEGDEF),1,SymFile);
        /* loop through symbols in that object */
        for (SymNum=0;SymNum<SegDef.cSymbols;SymNum++) {
           SymPtrOffset=SYMDEFOFFSET(SegOffset,SegDef,SymNum);
           fseek(SymFile,SymPtrOffset,SEEK_SET);
           fread(&SymOffset,sizeof(unsigned short int),1,SymFile);
           fseek(SymFile,SymOffset+SegOffset,SEEK_SET);
           if (SegDef.bFlags&0x01) {
              fread(&SymDef32,sizeof(SYMDEF32),1,SymFile);
              Buffer[0]= SymDef32.achSymName[0];
              fread(&Buffer[1],1,SymDef32.cbSymName,SymFile);
              Buffer[SymDef32.cbSymName]=0x00;
              printf("32 Bit Symbol <%s> Address %p\n",Buffer,SymDef32.wSymVal);
           } else {
              fread(&SymDef16,sizeof(SYMDEF16),1,SymFile);
              Buffer[0]=SymDef16.achSymName[0];
              fread(&Buffer[1],1,SymDef16.cbSymName,SymFile);
              Buffer[SymDef16.cbSymName]=0x00;
              printf("16 Bit Symbol <%s> Address %p\n",Buffer,SymDef16.wSymVal);
           } /* endif */
        }
        SegOffset=NEXTSEGDEFOFFSET(SegDef);
   } /* endwhile */
   fclose(SymFile);
}
