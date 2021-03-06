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
��������������������� ���  ��������������������Ŀ    �
�Sector 1�Sector 2�           �   n-1  �Sector n�    � Cylinder 0
����������������������� �����������������������Ĵ    �
�                                                    � Cylinder 1
���������������� � �  � � �  ��      ����������Ĵ    �
                                                     �
�                                               �    �> p cylinders
                                                     �
�����������������������������������������������Ĵ    �
�                                               �    � Cylinder p
�������������������������������������������������    �
Head 1
<------------------n sector/cylinder ----------->
��������������������� ���  ��������������������Ŀ    �
�Sector 1�Sector 2�           �   n-1  �Sector n�    � Cylinder 0
����������������������� �����������������������Ĵ    �
�                                                    � Cylinder 1
���������������� � �  � � �  ��      ����������Ĵ    �
                                                     �
�                                               �    �> p cylinders
                                                     �
�����������������������������������������������Ĵ    �
�                                               �    � Cylinder p
�������������������������������������������������    �
             ......

             ......

             ......

Head q
<------------------n sector/cylinder ----------->
��������������������� ���  ��������������������Ŀ    �
�Sector 1�Sector 2�           �   n-1  �Sector n�    � Cylinder 0
����������������������� �����������������������Ĵ    �
�                                                    � Cylinder 1
���������������� � �  � � �  ��      ����������Ĵ    �
                                                     �
�                                               �    �> p cylinders
                                                     �
�����������������������������������������������Ĵ    �
�                                               �    � Cylinder p
�������������������������������������������������    �
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
       ������������������������������������������������������Ŀ
 0x000 �  Boot code : Will read the partition map to find     �
       �                                                      �
 0x080 �  which partition to start eg: boot manager if present�
       �                                                      �
 0x100 �                                                      �
       �                           ��������������������������Ĵ
 0x180 �                           �Partition map      �0xAA55� <- magic number
       ��������������������������������������������������������    at the end
                       offset 0x1BE
:exmp.
:h1 id=part1  x=left y=top width=100% height=100% scroll=both
    clear.Partition mapping
:h2 id=partmap1 x=left y=top width=100% height=100% scroll=both
    clear.Partition Map and Partition entry
:xmp.
        Partition Map
�����������������������������������Ŀ
�  Partition 1st Entry (16 bytes)   �
�����������������������������������Ĵ
�  Partition 2nd Entry (16 bytes)   �
�����������������������������������Ĵ
�  Partition 3rd Entry (16 bytes)   �
�����������������������������������Ĵ
�  Partition 4th Entry (16 bytes)   �
�������������������������������������

       Partition Entry

     �������������������������������������������Ŀ
  0  � Status   �First head�First Sector Cylinder�
     �������������������������������������������Ĵ
  4  � Type     �Last head �Last Sector Cylinder �
     �������������������������������������������Ĵ
  8  � partition boot sector offset              �
     �������������������������������������������Ĵ
  C  � number of sectors in partition            �
     ���������������������������������������������
:exmp.
:p.Warning in C code entries index in map start at 0 not  1
:p.Sector and Cylinders 2 bytes encoding
:xmp.
             First byte                   Second byte
     ���������������������������������������������������������������Ŀ
     � 7 � 6 � 5 � 4 � 3 � 2 � 1 � 0 � 7 � 6 � 5 � 4 � 3 � 2 � 1 � 0 �
     �                               �                               �
     � 2 bits�   6 bits for sector   �  8 bits for cylinder low part �
     � high  �     number            �                               �
     � cylin-�                       �                               �
     �  der  �                       �                               �
     � part  �                       �                               �
     �����������������������������������������������������������������
     ���������                       ���������������������������������
         �                                            �
         ����������Ŀ                 �����������������
                �������Ŀ������������������������������Ŀ
                ���������������������������������������Ŀ
                � 9 � 8 � 7 � 6 � 5 � 4 � 3 � 2 � 1 � 0 �
                �   10 bit cylinder number value        �
                �����������������������������������������
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
�  Logical partition Entry (type 0x05)   ���Ŀ
                                             �
       ���������������������������������������
       �
       �
       �
       ����> Start Head Sector and Cylinder
             1st logical partition boot sector
             �����������������������������������������������������Ŀ
             �                                                     �
             �                     �������������������������������Ĵ
             �                     �Logical partition table �0xAA55�
             �������������������������������������������������������
                                     �    �
       �������������������������������    ��������������������Ŀ
       �                                                       �
       ����> 1st entry points to currently defined logical     �
             partition                                         �
                                                               �
       ���������������������������������������������������������
       �
       ����> 2nd entry points to next logical partition boot record
             with an identical structure. If type is 0x00 then there
             are no more logical partitions

:exmp.
:p.Logical partition table structure :
:xmp.
�������������������������������������������������Ŀ
�  1st Entry Current Logical partition (16 bytes) �
�������������������������������������������������Ĵ
�  2nd Entry Next Logical partition  (16 bytes)   �
�  Points to the next logical partition boot      �
�  record with the same structure as this one     �
�������������������������������������������������Ĵ
�  3rd Entry unused                  (16 bytes)   �
�������������������������������������������������Ĵ
�  4th Entry unused                  (16 bytes)   �
���������������������������������������������������
:p.Warning in C code entries index in map start at 0 not  1
:exmp.




:euserdoc.


