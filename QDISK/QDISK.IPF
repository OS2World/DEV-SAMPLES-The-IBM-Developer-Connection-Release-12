.*--------------------------------------------------------------------
.* This file contains the tagged source for QPROC Package
.*--------------------------------------------------------------------

:userdoc.

.*--------------------------------------------------------------------
.* Provide a title for the title line of the main window
.*--------------------------------------------------------------------

:title.Qdisk Package

.*--------------------------------------------------------------------
.* Allow only heading level 1 to appear in the contents window and
.* specify no control area for pushbuttons
.*--------------------------------------------------------------------

:docprof toc=1 ctrlarea=none.

.*--------------------------------------------------------------------
.* Identify the heading level entries to be displayed in the contents
.* window
.*--------------------------------------------------------------------

:h1 id=content1 x=left y=top width=100% height=100% scroll=both
    clear.DosPhysicalDisk usage to gather partition mapping
:p.The Qdisk package is a source sample demonstrating the use of the
DosPhysicalDisk API to get the current partition mapping. This is a standard
OS/2 2.x and higher API.
:p.QDisk also returns the current partition logical mapping. Source code is in
Qdisk.c file
:h1 id=phys1 x=left y=top width=100% height=100% scroll=both
    clear.Physical disk structure
:p.Physical disk map
A physical disk is arranged in a pile of plates accessible via several heads
each offset from the disk center being called a cylinder, each cylinder for a
given plate being subdivised in sectors (512 bytes each).
If disk has q Heads,p Cylinders and n Sectors. Sectors could be represented
in a circular way, with some interleaving which speeds up disk access.
Interleaving means that physically sector 2 may not be just next to sector 1
but another sector may be found instead.
:xmp.
Head 0
<------------------n sector/cylinder ----------->
ÚÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄ ÄÄÄ  ÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄ¿    ¿
³Sector 1³Sector 2³           ³   n-1  ³Sector n³    ³ Cylinder 0
ÃÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄ ÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄ´    ³
³                                                    ³ Cylinder 1
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Ä Ä  Ä Ä Ä  ÄÄ      ÄÄÄÄÄÄÄÄÄÄÄ´    ³
                                                     ³
Ã                                               ´    Ã> p cylinders
                                                     ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´    ³
³                                               ³    ³ Cylinder p
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ    Ù
Head 1
<------------------n sector/cylinder ----------->
ÚÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄ ÄÄÄ  ÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄ¿    ¿
³Sector 1³Sector 2³           ³   n-1  ³Sector n³    ³ Cylinder 0
ÃÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄ ÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄ´    ³
³                                                    ³ Cylinder 1
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Ä Ä  Ä Ä Ä  ÄÄ      ÄÄÄÄÄÄÄÄÄÄÄ´    ³
                                                     ³
Ã                                               ´    Ã> p cylinders
                                                     ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´    ³
³                                               ³    ³ Cylinder p
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ    Ù
             ......

             ......

             ......

Head q
<------------------n sector/cylinder ----------->
ÚÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄ ÄÄÄ  ÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄ¿    ¿
³Sector 1³Sector 2³           ³   n-1  ³Sector n³    ³ Cylinder 0
ÃÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄ ÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄ´    ³
³                                                    ³ Cylinder 1
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Ä Ä  Ä Ä Ä  ÄÄ      ÄÄÄÄÄÄÄÄÄÄÄ´    ³
                                                     ³
Ã                                               ´    Ã> p cylinders
                                                     ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´    ³
³                                               ³    ³ Cylinder p
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ    Ù
:exmp.
:h1 id=boot1 x=left y=top width=100% height=100% scroll=both
    clear.Boot sector structure
:p.A PC will boot by loading code at Head 0 Cylinder 0 Sector 1
of the first physical disk.
:p.Physical Boot sector map
:xmp.
Bytes
Offset                512 (0x200) bytes boot Sector
(hex)
       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 0x000 ³  Boot code : Will read the partition map to find     ³
       ³                                                      ³
 0x080 ³  which partition to start eg: boot manager if present³
       ³                                                      ³
 0x100 ³                                                      ³
       ³                           ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄ´
 0x180 ³                           ³Partition map      ³0xAA55³ <- magic number
       ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÙ    at the end
                       offset 0x1BE
:exmp.
:h1 id=part1  x=left y=top width=100% height=100% scroll=both
    clear.Partition mapping
:h2 id=partmap1 x=left y=top width=100% height=100% scroll=both
    clear.Partition Map and Partition entry
:xmp.
        Partition Map
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³  Partition 1st Entry (16 bytes)   ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
³  Partition 2nd Entry (16 bytes)   ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
³  Partition 3rd Entry (16 bytes)   ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
³  Partition 4th Entry (16 bytes)   ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

       Partition Entry

     ÚÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  0  ³ Status   ³First head³First Sector Cylinder³
     ÃÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
  4  ³ Type     ³Last head ³Last Sector Cylinder ³
     ÃÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
  8  ³ partition boot sector offset              ³
     ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
  C  ³ number of sectors in partition            ³
     ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
:exmp.
:p.Warning in C code entries index in map start at 0 not  1
:p.Sector and Cylinders 2 bytes encoding
:xmp.
             First byte                   Second byte
     ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
     ³ 7 ³ 6 ³ 5 ³ 4 ³ 3 ³ 2 ³ 1 ³ 0 ³ 7 ³ 6 ³ 5 ³ 4 ³ 3 ³ 2 ³ 1 ³ 0 ³
     ³                               ³                               ³
     ³ 2 bits³   6 bits for sector   ³  8 bits for cylinder low part ³
     ³ high  ³     number            ³                               ³
     ³ cylin-³                       ³                               ³
     ³  der  ³                       ³                               ³
     ³ part  ³                       ³                               ³
     ÀÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
     ÀÄÄÄÂÄÄÄÙ                       ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
         ³                                            ³
         ÀÄÄÄÄÄÄÄÄÄÄ¿                 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
                ÚÄÄÄÁÄÄÄ¿ÚÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
                ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
                ³ 9 ³ 8 ³ 7 ³ 6 ³ 5 ³ 4 ³ 3 ³ 2 ³ 1 ³ 0 ³
                ³   10 bit cylinder number value        ³
                ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
:exmp.
:h2 id=types1  x=left y=top width=100% height=100% scroll=both
    clear.Partition types
:dl.
:dt.0x00 :dd.empty
:dt.0x01 :dd.DOS, FAT 12 bits
:dt.0x02 :dd.XENIX
:dt.0x03 :dd.XENIX
:dt.0x04 :dd.DOS, FAT 16 bits
:dt.0x05 :dd.DOS or OS/2 Extend. Partition contains logical partitions
:dt.0x06 :dd.DOS or OS/2 > 32Mb
:dt.0x07 :dd.HPFS
:dt.0x0A :dd.Boot Manager
:dt.0x16 :dd.DOS 6.1
:dt.0xDB :dd.Concurrent DOS
:edl.
:h1 id=part2  x=left y=top width=100% height=100% scroll=both
    clear.Logical partition mapping
:p.If the physical partition type is 0x05 then it contains logical partitions
:xmp.
³  Logical partition Entry (type 0x05)   ÃÄÄÄ¿
                                             ³
       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
       ³
       ³
       ³
       ÀÄÄÄ> Start Head Sector and Cylinder
             1st logical partition boot sector
             ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
             ³                                                     ³
             ³                     ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄ´
             ³                     ³Logical partition table ³0xAA55³
             ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÂÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÙ
                                     ³    ³
       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ    ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
       ³                                                       ³
       ÀÄÄÄ> 1st entry points to currently defined logical     ³
             partition                                         ³
                                                               ³
       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
       ³
       ÀÄÄÄ> 2nd entry points to next logical partition boot record
             with an identical structure. If type is 0x00 then there
             are no more logical partitions

:exmp.
:p.Logical partition table structure :
:xmp.
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³  1st Entry Current Logical partition (16 bytes) ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
³  2nd Entry Next Logical partition  (16 bytes)   ³
³  Points to the next logical partition boot      ³
³  record with the same structure as this one     ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
³  3rd Entry unused                  (16 bytes)   ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
³  4th Entry unused                  (16 bytes)   ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
:p.Warning in C code entries index in map start at 0 not  1
:exmp.




:euserdoc.


