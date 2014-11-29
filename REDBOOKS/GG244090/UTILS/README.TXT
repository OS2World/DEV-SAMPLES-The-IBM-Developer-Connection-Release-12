
OS/2 editors use 0x0D 0x0A as end of line markers. OS/2 editors also use
a end of file marker character 0x1A at the end of the file.

AIX editors use 0x0A as end of line markers. AIX editors don't use any end of 
file markers.

Some of the problems we have found due to diferences between OS/2 and AIX
editors are:

  - The AIX C and IDL compilers do not like to compile files created with OS/2
    editors.
  - Files created with AIX editors don't look good under OS/2 if you use the 
    OS/2 TYPE command to view the file.
  - If you use the OS/2 EPM editor to edit a file with TAB characters you will
    see on your screen a small circle for every TAB char in the file. AIX's VI
    editor saves the TAB char if you have used it in the file.

To help solve these problems we have included 3 utilities that work like
filters.

AIXTOOS2 will add a 0x0D char to every line and a 0x1A char at the end of the 
input file.

  usage: AIXTOOS2 <aixfile >os2file

OS2TOAIX will remove all 0x0D chars and the 0x1A char from the input file.

  usage: OS2TOAIX <os2file >aixfile

NOTABS will replace all TAB chars by as many 0x20 (blank) chars as required to
provide the same result of the TAB based on a 8 chars TAB stop.

  usage: NOTABS <filein >fileout


You can combine the use of these utilities with pipes.

  example: AIXTOOS2 <aixfile | NOTABS >os2file


To compile them under OS/2:
icc /DIBMOS2 os2toaix.c
icc /DIBMOS2 aixtoos2.c
icc /DIBMOS2 notabs.c

To compile them under AIX:
cc notabs.c -onotabs
cc os2toaix.c -oos2toaix
cc aixtoos2.c -oaixtoos2
