/**********************************************************************/
/*                                                                    */
/*  QDISK                                                             */
/*                                                                    */
/* Sample source code to query the disk partitionning                 */
/**********************************************************************/
/* Version: 2.0             |   Marc Fiammante (FIAMMANT at LGEVMA)   */
/*                          |   La Gaude FRANCE                       */
/*                          |   Internet: fiammante@vnet.ibm.com      */
/**********************************************************************/
/* History:                                                           */
/* --------                                                           */
/*                                                                    */
/* created: Marc Fiammante November 1995                              */
/* enhanced: Sam Detweiler September 1996 Large disks and             */
/*           File system detected by FSattach                         */
/**********************************************************************/

#define INCL_DOSPROCESS   /* Device values */
#define INCL_DOSDEVICES   /* Device values */
#define INCL_DOSDEVIOCTL
#define INCL_DOSFILEMGR
#define INCL_DOSMISC
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#pragma pack(1)


/*                                                      */
/*   typedefs                                           */

typedef unsigned short int WORD;

typedef struct {                           /* gives sector position    */
                BYTE Head;                 /* Read/Write head          */
                WORD SectorAndCylinder;    /* Sector & cylinder number */
               } SECTORCYLINDER;

typedef struct {                               /* partition table entry    */
              BYTE            Status;          /* partition status         */
              SECTORCYLINDER  FirstSector;     /* First sector             */
              BYTE            PartitionType;   /* Partition type           */
              SECTORCYLINDER  LastSector;      /* Last sector              */
              unsigned long   BootSectorOffset;/* Boot sector Offset       */
              unsigned long   NumberOfSectors; /* Number of sectors        */
               } PARTITIONENTRY;

typedef struct {                   /* Give the partition sector mapping*/
                BYTE           BootCode[ 0x1BE ];
                PARTITIONENTRY PartitionTable[ 4 ];
                WORD           Magic;          /* 0xAA55 */
               } PARTITIONBOOTSECTOR;

typedef PARTITIONBOOTSECTOR  *PARSPTR; /* Pointer to a partition sector */

typedef struct _fs {
               UCHAR Disk;
               UCHAR FS;
               struct _fs *pNext;
               } LOGICALDISK, *PLOGICALDISK;

PLOGICALDISK pPrimary=0,pEnd=0,pLogical=0;
#define Primary 0
#define Logical 1

char * PhysFormat[] ={
/*0*/ "ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป\n",
/*1*/ "บ Drive    %2d: %3d heads with each %4d  cylinders of %3d sectors           บ\n",
/*2*/ "บ Partition table for sector in partition                                   บ\n",
/*3*/ "ฬออหอออออหอออออออออออออออออออหออออออออออออออหอออออออออออออออหออออออออหออออออน\n",
/*4*/ "บ  บ     บ                   บ    Start     บ      End      บDistanceบ Size บ\n",
/*5*/ "บ# บStartบType               บHead Cyl. Sec.บHead  Cyl. Sec.บBootSectบ Megs บ\n",
/*6*/ "ฬออฮอออออฮอออออออออออออออออออฮออออออออออออออฮอออออออออออออออฮออออออออฮออออออน\n",
/*7*/ "ศออสอออออสอออออออออออออออออออสออออออออออออออสอออออออออออออออสออออออออสออออออผ\n"
};
char * LogFormat[] ={
/*0*/ "ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป\n",
/*1*/ "บ Current Logical disk mapping                                                บ\n",
/*2*/ "ฬออออออออหออออออออออออออหออออออออออออออหออออออออออออหอออออออออออหออออออหออออออน\n",
/*3*/ "บ Drive  บ Sectors/Unit บ Bytes/Sector.บ Allocated  บ Free      บ FS   บ FS   บ\n",
/*4*/ "บLog Physบ              บ              บ            บ           บ Det  บ Rep  บ\n",
/*5*/ "ฬออออออออฮออออออออออออออฮออออออออออออออฮออออออออออออฮอออออออออออฮออออออฮออออออน\n",
/*6*/ "ศออออออออสออออออออออออออสออออออออออออออสออออออออออออสอออออออออออสออออออสออออออผ\n"
};

#define SectorFromSecCyl(sc)   ((sc)&0x3F)
#define CylinderFromSecCyl(sc) ( HIBYTE(sc)+(((WORD)(LOBYTE(sc)&0xC0))<<2) )
#define Partition(e)           (PartitionBootSector.PartitionTable[(e)])

#define  MAIN_TABLE      0x04
#define  EXTENDED_TABLE  0x01
#define  FS_Type_FAT 0
#define  FS_Type_HPFS 1

void AddDrive(UCHAR Disk, UCHAR FS, BOOL Type)
{
  PLOGICALDISK pD,pHead,pLink;

  if(pD=malloc(sizeof(*pD)))
    {
    memset(pD,0,sizeof(*pD));
    pD->Disk=Disk;
    pD->FS=FS;
    switch(Type)
      {
      case Primary:
        if(!(pHead=pPrimary))
          {
            pPrimary=pD;
            pEnd=pD;
            pD=NULL;
          } /* endif */
        break;
      case Logical:
        if(!(pHead=pLogical))
          {
            pLogical=pD;
            pD=NULL;
          } /* endif */
        break;
      default:
       break;
      } /* endswitch */
    if(pD)
      {
      while(pHead->pNext)
        {
        pHead=pHead->pNext;
        } /* endwhile */
      pHead->pNext=pD;
      if(Type==Primary)
        pEnd=pD;
      } /* endif */
    } /* endif */
}

void ReadPartitionTable(UCHAR PhysicalDisk,HFILE hDisk,USHORT Type,USHORT PCylinder,USHORT PSector) {
   /********************************************************/
   APIRET  rc;           /* Return code */
   BYTE Entry;
   unsigned long int     ParmLengthInOut;
   unsigned long int     DataLengthInOut;
   int SSector,SCylinder;
   int ESector,ECylinder;
   ULONG   Size;
   TRACKLAYOUT TrackLayout;
   PARTITIONBOOTSECTOR     PartitionBootSector;
   TrackLayout.bCommand     =0x00; /* read does not start with sector 1 */
   TrackLayout.usHead       =0;    /* read head 0 */
   TrackLayout.usCylinder   =PCylinder;    /* read Cylinder 0 */
   TrackLayout.usFirstSector=0;    /* Boot sector */
   TrackLayout.cSectors     =1;    /* Only one sector */
   TrackLayout.TrackTable[0].usSectorNumber=PSector; /* sector 1 */
   TrackLayout.TrackTable[0].usSectorSize  =sizeof(PARTITIONBOOTSECTOR);

   ParmLengthInOut=sizeof(TrackLayout);
   DataLengthInOut=sizeof(PARTITIONBOOTSECTOR);
   rc = DosDevIOCtl(hDisk, IOCTL_PHYSICALDISK,PDSK_READPHYSTRACK,
       &TrackLayout,sizeof(TrackLayout), &ParmLengthInOut,
       &PartitionBootSector,sizeof(PartitionBootSector),&DataLengthInOut);
   if (rc != 0)
   {
      printf("Read boot sector IOCTL failure: return code = %ld\n", rc);
   } else {
       /*-- read partition table       ----------------------------*/
      for ( Entry=0; Entry < Type; Entry++ ) {
         if (Type==MAIN_TABLE) {
            printf( "บ %hdบ ",(short int)Entry );
            if ( Partition(Entry).Status == 0x80 ) printf("Yes "); /* Active partition  */
            else printf ("No  ");
         } else {
            printf( "บ->บ     ");
         } /* endif */
         printf("บ");
         /* Partition type evaluation */
         switch( Partition(Entry).PartitionType ) {
            case 0x00 : printf( "empty              " );
                break;
              case 0x01 : printf( "DOS, FAT 12 bits   " );
                AddDrive(PhysicalDisk, FS_Type_FAT,Primary);
                break;
            case 0x02 :
            case 0x03 : printf( "XENIX              " );
                break;
            case 0x04 :
                if (Type==MAIN_TABLE) {
                  printf( "DOS, FAT 16 bits   " );
                  AddDrive(PhysicalDisk, FS_Type_FAT,Primary);
                } else {
                   printf( "  Logical          " );
                  AddDrive(PhysicalDisk, FS_Type_FAT,Logical);
                }

                break;
            case 0x05 : printf( "DOS or OS/2 Extend." );
                break;
            case 0x06 :
                if (Type==MAIN_TABLE) {
                   printf( "DOS or OS/2 > 32Mb " );
                  AddDrive(PhysicalDisk, FS_Type_FAT,Primary);
                } else {
                   printf( "  Logical > 32Mb   " );
                  AddDrive(PhysicalDisk, FS_Type_FAT,Logical);
                }
                break;
            case 0x07 :
                if (Type==MAIN_TABLE) {
                   printf( "HPFS               " );
                  AddDrive(PhysicalDisk, FS_Type_HPFS,Primary);
                } else {
                   printf( "  Logical HPFS     " );
                  AddDrive(PhysicalDisk, FS_Type_HPFS,Logical);
                }
                break;
            case 0x0A : printf( "Boot Manager       " );
                break;
            case 0x17 : printf( "HPFS    (inactive) " );
                break;
            case 0x14 :
            case 0x11 :
            case 0x16 : printf( "DOS 6.1 (inactive) " );
                break;
            case 0xDB : printf( "Concurrent DOS     " );
                break;
            default   : printf( "unknown   (%3d)    ",
                    PartitionBootSector.PartitionTable[ Entry ].PartitionType );
         } /* end switch */
         /*-- physical and logical  ---------------*/
         SCylinder=CylinderFromSecCyl(Partition(Entry).FirstSector.SectorAndCylinder);
         SSector  =SectorFromSecCyl(  Partition(Entry).FirstSector.SectorAndCylinder);
         printf( "บ%2hd %5hd  %3hd ", Partition(Entry).FirstSector.Head, SCylinder, SSector  );
         ECylinder=CylinderFromSecCyl(Partition(Entry).LastSector.SectorAndCylinder);
         ESector  =SectorFromSecCyl(  Partition(Entry).LastSector.SectorAndCylinder);
         printf( "บ%3hd %5hd  %3hd ", Partition(Entry).LastSector.Head, ECylinder, ESector  );
         Size=Partition(Entry).NumberOfSectors/2048;
         printf( "บ%7lu บ %4lu บ\n", Partition(Entry).BootSectorOffset,Size);
         if (( Partition(Entry).PartitionType ==0x05)&&(Type==MAIN_TABLE)) {
            ReadPartitionTable(PhysicalDisk,hDisk,EXTENDED_TABLE,SCylinder,SSector);
         }
      } /* end for  */
      /* Chain through extended partitions */
      Entry=1;
      if ((Type==EXTENDED_TABLE)&&(Partition(Entry).PartitionType!=0x00)) {
         SCylinder=CylinderFromSecCyl(Partition(Entry).FirstSector.SectorAndCylinder);
         SSector  =SectorFromSecCyl(  Partition(Entry).FirstSector.SectorAndCylinder);
         ReadPartitionTable(PhysicalDisk,hDisk,EXTENDED_TABLE,SCylinder,SSector);
      }
   }

}

int main(void);
int main() {
   ULONG   Function;     /* Type of information */
   USHORT  PhysDisks;   /* Data return buffer */
   ULONG   DataLen;      /* Data return buffer length */
   PVOID   ParmPtr;      /* Pointer to user-supplied information */
   ULONG   ParmLen;      /* Length of user-supplied information */
   ULONG   Size,SizeFree;
   FSALLOCATE FSAllocate;
   APIRET  rc;           /* Return code */
   char Buffer[10];
   ULONG disknum,logical;
   PSZ name;
   int i;

   Function = INFO_COUNT_PARTITIONABLE_DISKS;
                         /* Indicate that a count of the number of */
                         /*   partitionable disks within the       */
                         /*   system is requested                  */

   ParmPtr = 0;       /* No input parameters are relevant for */
   ParmLen = 0;       /*   the requested DosPhysicalDisk      */
                         /*   function                           */

   DataLen = 2;       /* Number of bytes in data return buffer */
   PhysDisks=0;
   rc = DosPhysicalDisk(Function, &PhysDisks, DataLen,
                                   ParmPtr, ParmLen);
                         /* On successful return, the DataBuf   */
                         /*   variable contains the number of   */
                         /*   partitionable disks in the system */

   if (rc != 0)
      {
         printf("DosPhysicalDisk error: return code = %ld", rc);
         return rc;
   } else {
         printf("Physical disks = %hd\n", PhysDisks);
   }
   for (i=1;i<=PhysDisks ;i++) {
      HFILE   hDisk;
      sprintf(Buffer,"%d:",i);
      ParmPtr=Buffer;
      ParmLen=strlen(Buffer)+1;
      hDisk=0;
      DataLen = 2;       /* Number of bytes in data return buffer */
      Function = INFO_GETIOCTLHANDLE;
      rc = DosPhysicalDisk(Function, &hDisk, DataLen,
                                     ParmPtr, ParmLen);
      if (rc != 0)
      {
         printf("DosPhysicalDisk error: return code = %ld", rc);
         return rc;
      } else {
         DEVICEPARAMETERBLOCK DeviceParameterBlock ;
         char    Parameter=0x00;
         unsigned long int     ParmLengthInOut;
         unsigned long int     DataLengthInOut;
         ParmLengthInOut=sizeof(Parameter);
         DataLengthInOut=sizeof(DeviceParameterBlock);
         rc = DosDevIOCtl(hDisk, IOCTL_PHYSICALDISK,PDSK_GETPHYSDEVICEPARAMS,
                &Parameter,sizeof(Parameter), &ParmLengthInOut,
                &DeviceParameterBlock,sizeof(DeviceParameterBlock),&DataLengthInOut);
         if (rc != 0)
         {
            printf("IOCTL GetPhys failure: return code = %ld\n", rc);
         } else {
            printf( PhysFormat[0]);
            printf( PhysFormat[1],i,
                    DeviceParameterBlock.cHeads,
                    DeviceParameterBlock.cCylinders,
                    DeviceParameterBlock.cSectorsPerTrack );
            printf( PhysFormat[2]);
            printf( PhysFormat[3]);
            printf( PhysFormat[4]);
            printf( PhysFormat[5]);
            printf( PhysFormat[6]);
            ReadPartitionTable((UCHAR)i,hDisk,MAIN_TABLE,0,1);

            printf( PhysFormat[7]);
            printf( "Press any key to continue ...\n");
            getch();
         }



         Function = INFO_FREEIOCTLHANDLE;
         ParmPtr=&hDisk;
         ParmLen=2;
         rc = DosPhysicalDisk(Function, 0,0,ParmPtr, ParmLen);
         if (rc != 0) {
            printf("DosPhysicalDisk error: return code = %ld", rc);
            return rc;
         }
      }
   } /* endfor */
   pEnd->pNext=pLogical;
   rc=DosQueryCurrentDisk(&disknum,&logical);
   if (rc != 0)
   {
      printf("DosQueryCurrentDisk error: return code = %ld", rc);
      return rc;
   }
   Buffer[1]=':';
   Buffer[2]=0x00;
   printf( LogFormat[0]);
   printf( LogFormat[1]);
   printf( LogFormat[2]);
   printf( LogFormat[3]);
   printf( LogFormat[4]);
   printf( LogFormat[5]);
   DosError(FERR_DISABLEHARDERR);
   for (i=2;i<26;i++) { /* skip diskettes 0 and 1 */
      ULONG Mask;
      static union {
         FSQBUFFER2 FSQBuffer2 ;
         char  Dummy[400];
      } QBuffer;
      Mask=1<<i;
      if (Mask&logical) {
         ParmLen=sizeof(QBuffer );
         rc=DosQueryFSAttach(0,i+1,FSAIL_DRVNUMBER,
                                   (PFSQBUFFER2)&QBuffer ,&ParmLen );
         if (rc == 0)
         {
            if (QBuffer.FSQBuffer2.iType==FSAT_LOCALDRV) {

               rc=DosQueryFSInfo(i+1,FSIL_ALLOC,&FSAllocate,sizeof(FSAllocate));
               if (rc != 0)
               {
                  printf("DosQueryFSInfo error: return code = %ld", rc);
                  return rc;
               }
               Buffer[0]='A'+i;
               SizeFree=Size=(FSAllocate.cSectorUnit*FSAllocate.cbSector);
               Size    *=FSAllocate.cUnit;
               Size    /=1024*1024;
               SizeFree*=FSAllocate.cUnitAvail;
               SizeFree/=1024*1024;
               name=QBuffer.FSQBuffer2.szName;
               name+=strlen(name)+1;
               printf( "บ %s  %d  บ % 7ld      บ % 7ld      บ  %4ld Megs บ % 4ld Megs บ %-4.4s บ %-4.4s บ\n",
                  Buffer,
                  pPrimary?pPrimary->Disk:0,
                  FSAllocate.cSectorUnit,
                  FSAllocate.cbSector,
                  Size,SizeFree,
                  pPrimary?(pPrimary->FS?"HPFS":"FAT"):((FSAllocate.cbSector==2048)?"CDFS":"????"),
                  name
               );

            } /* endif */
         }
      if(pPrimary)
       pPrimary=pPrimary->pNext;
      } /* endif */
   } /* endfor */
   printf( LogFormat[6]);



   printf( "Press any key to exit ...\n");
   getch();

   return rc;
}
