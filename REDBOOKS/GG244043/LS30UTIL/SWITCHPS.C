/*
 * IBM INTERNAL USE ONLY
 *
 * Program to switch a partition between primary and secondary.
 *  switch [p|s] sector(in hex)
 */
 /**********************************************************************/
 #define   INCL_DOSPROCESS
 #define   INCL_DOSDEVICES
 #define   INCL_DOSFILEMGR
 #include  <os2.h>
 #include  <stdio.h>
 #include  <stdlib.h>
 #include  <memory.h>
 #include  <string.h>
 /**********************************************************************/
 /* DECLARATIONS */
 #define  STDOUT               1       // Screen's handle
 #define  SECTOR_SIZE        512
 #define  MAXSECTORS          64
 #define  MIRROR            0x87
 #define  HPFS              0x07
 #define  SIG1              0x55
 #define  SIG2              0xAA

 /* DosPhysicalDisk() stuff */
 #define  GET_DISK_HANDLE        2
 #define  REL_DISK_HANDLE        3

 /* Category 9 IOCTLs */
 #define  CATEGORY_9             0x09    // Category #
 #define  LOCK_PHYS_DISK         0x00
 #define  UNLOCK_PHYS_DISK       0x01
 #define  GET_DEV_PARMS          0x63    // Function #
 #define  READ_SECTOR            0x64    // Function #
 #define  WRITE_SECTOR           0x44    // Function #

 /* sector layout table */
 struct ttlayout {
     USHORT sector_nbr;
     USHORT sector_size;
 };
 typedef struct _rs {
     BYTE   Cmd_Info;
     USHORT Head;
     USHORT Cylinder;
     USHORT First_Sector;
     USHORT Number_Of_Sectors;
     struct ttlayout table[MAXSECTORS];
 } READPARMS;

 /**********************************************************************/
 /* GLOBAL VARIABLES */


//*******************************************************************
// 1. Validate input parameters
// 2. Initialize I/O structure
// 3. Get disk handle (open drive)
// 4. Read master boot record
// 5. Change partition type
// 6. Write master boot record
// 7. Close drive
//

main(argc, argv, envp)
   int argc;
   char *argv[];
   char *envp[];
{
    USHORT rc;
    ULONG  ndx;
    BYTE   Dname[4];
    USHORT save;
    READPARMS rp;                       // cmd packet for IOCtl
    BOOL     primary;
    BOOL     secondary;
    UCHAR    Buffer[512];
    USHORT   Disk;                // Disk number
    ULONG    Cylinder;            // Drive starting cylinder
    USHORT   Handle;              // open disk's handle

    printf("SWITCH Primary/Secondary Version 1.0 \n");
    printf("Program copyright International Business Machines Corp. 1992\n");
    printf("IBM Internal Use Only\n\n");


    /* validate input arguments */
    if ( argc < 3)
      {
      puts("Switch primary/secondary partition indicator.\n\
              Syntax: switchps [p|s] disk cylinder \n\
              p . . . . Primary to secondary \n\
              s . . . . Secondary to primary \n\
              disk. . . Disk # (1 ... 24 )\n\
              cylinder. Cylinder # (Decimal, zero-based)\n\
            Example:  switchps p 1 0\n");
      DosExit(EXIT_PROCESS, 0);
      }

    /* Validate command-line arguments */
    primary = FALSE;
    if ((argv[1][0] == 'P') || argv[1][0] == 'p')
      primary = TRUE;

    secondary = FALSE;
    if ((argv[1][0] == 'S') || argv[1][0] == 's')
      secondary = TRUE;

    if (primary == FALSE && secondary == FALSE)
      {
      printf("P/S parameter not specified\n");
      DosExit(EXIT_PROCESS, 0);
      }

    Disk = atoi(&argv[2][0]);
    Cylinder = atoi(&argv[3][0]);


    /* Init the track layout table w/in the cmd packet */
    for (ndx = 0; ndx < MAXSECTORS; ndx++)
      {
      rp.table[ndx].sector_nbr  = ndx+1;
      rp.table[ndx].sector_size = SECTOR_SIZE;
      }
    rp.Cmd_Info          = 1;
    rp.Cylinder          = Cylinder;
    rp.Head              = 0;
    rp.First_Sector      = 0;
    rp.Number_Of_Sectors = 1;


    /* Open the disk */
    Dname[0] = Disk + '0';      // Disk is 1 based - make it a character
    Dname[1] = ':';
    Dname[2] = '\0';
    if (rc = DosPhysicalDisk( (USHORT) GET_DISK_HANDLE, (PBYTE)  &Handle,
                (USHORT) sizeof(Handle), (PBYTE)  Dname, (USHORT) 3 ))
      {
      printf("Could not open physical disk #%hu\n",Disk);
      goto eext;
      }

    /* read the MBR */
    if (rc=DosDevIOCtl( (PVOID ) Buffer, (PCHAR ) &rp,
                      (USHORT) READ_SECTOR, (USHORT) CATEGORY_9,
                      (HFILE ) Handle ))
      {
      printf("ReadSector: Failure on disk# %hu, OS/2 error=%hu\n", Disk, rc);
      goto shutd;
      }

    /* try to change the data */
    if (Buffer[510] != SIG1)
      {
      printf("Disk location does not contain correct data\n");
      goto shutd;
      }
    if (Buffer[511] != SIG2)
      {
      printf("Disk location does not contain correct data\n");
      goto shutd;
      }

    if (primary)
      {
      if (Buffer[450] == HPFS)
        Buffer[450] = MIRROR;
      else if (Buffer[466] == HPFS)
        Buffer[466] = MIRROR;
      else
        {
        printf("Disk location does not contain a primary partition\n");
        goto shutd;
        }
      }
    else
      {
      if (Buffer[450] == MIRROR)
        Buffer[450] = HPFS;
      else if (Buffer[466] == MIRROR)
        Buffer[466] = HPFS;
      else
        {
        printf("Disk location does not contain a secondary partition\n");
        goto shutd;
        }
      }


    /* write the MBR */
    if (rc=DosDevIOCtl( (PVOID ) Buffer, (PCHAR ) &rp,
                      (USHORT) WRITE_SECTOR, (USHORT) CATEGORY_9,
                      (HFILE ) Handle ))
      printf("WriteSector: Failure on disk# %hu, OS/2 error=%hu\n", Disk, rc);
    else
      {
      if (primary)
        printf("Primary partition changed to secondary.\n");
      else
        printf("Secondary partition changed to primary.\n");
      }

shutd:
    if (DosPhysicalDisk( (USHORT) REL_DISK_HANDLE, (PBYTE)  0,
                          (USHORT) 0, (PBYTE) &Handle,
                          (USHORT) sizeof(USHORT) ))
      printf("ClosePhysDisk: Failure on disk# %hu, OS/2 error=%hu\n", Disk, rc);

eext:
    DosExit(EXIT_PROCESS, 0);
}
                                