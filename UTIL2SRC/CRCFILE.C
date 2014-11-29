/*============================================================================*
 * module: crcfile.c - Calculate the 16-bit CRC of a file.
 *
 * (C)Copyright IBM Corporation, 1990, 1991, 1992.        Brian E. Yoder
 *
 * This module contains the externally-available crcfile() subroutine.
 * This subroutine opens a file, gets its length, calculates its
 * CRC, and closes the file.
 *
 * The code to calculate the CRC and much of the its description was taken
 * from the crc_chk.c module written by Jim Czenkusch in IBM Austin, TX.
 * He did the hard part - I only took the code and wrapped another interface
 * around it.
 *
 * 10/30/90 - Created from the crc_chk.c module written by Jim Czenkusch.
 * 10/30/90 - Initial version, ported to the interface defined by crcfile().
 * 05/02/91 - Ported from PS/2 AIX to DOS.  The file is explicitly opened
 *            in binary mode.
 * 06/07/92 - Added crcfile32() subroutine to calculate 32-bit CRCs.
 *============================================================================*/

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#include "util.h"

#define NO       0
#define YES      1

#define BUFFLEN  4096
#define DEBUG    1

/*============================================================================*
 * This table is directly from Jim's crc_chk.c, with no modifications other
 * than making it 'static'.  It is used for calculating 16-bit CRCs.
 *============================================================================*/

static unsigned short transition_tbl[256] =

{0x0000,0xA097,0xE1B9,0x412E,0x63E5,0xC372,0x825C,0x22CB,0xC7CA,0x675D,0x2673,
 0x86E4,0xA42F,0x04B8,0x4596,0xE501,0x2F03,0x8F94,0xCEBA,0x6E2D,0x4CE6,0xEC71,
 0xAD5F,0x0DC8,0xE8C9,0x485E,0x0970,0xA9E7,0x8B2C,0x2BBB,0x6A95,0xCA02,0x5E06,
 0xFE91,0xBFBF,0x1F28,0x3DE3,0x9D74,0xDC5A,0x7CCD,0x99CC,0x395B,0x7875,0xD8E2,
 0xFA29,0x5ABE,0x1B90,0xBB07,0x7105,0xD192,0x90BC,0x302B,0x12E0,0xB277,0xF359,
 0x53CE,0xB6CF,0x1658,0x5776,0xF7E1,0xD52A,0x75BD,0x3493,0x9404,0xBC0C,0x1C9B,
 0x5DB5,0xFD22,0xDFE9,0x7F7E,0x3E50,0x9EC7,0x7BC6,0xDB51,0x9A7F,0x3AE8,0x1823,
 0xB8B4,0xF99A,0x590D,0x930F,0x3398,0x72B6,0xD221,0xF0EA,0x507D,0x1153,0xB1C4,
 0x54C5,0xF452,0xB57C,0x15EB,0x3720,0x97B7,0xD699,0x760E,0xE20A,0x429D,0x03B3,
 0xA324,0x81EF,0x2178,0x6056,0xC0C1,0x25C0,0x8557,0xC479,0x64EE,0x4625,0xE6B2,
 0xA79C,0x070B,0xCD09,0x6D9E,0x2CB0,0x8C27,0xAEEC,0x0E7B,0x4F55,0xEFC2,0x0AC3,
 0xAA54,0xEB7A,0x4BED,0x6926,0xC9B1,0x889F,0x2808,0xD88F,0x7818,0x3936,0x99A1,
 0xBB6A,0x1BFD,0x5AD3,0xFA44,0x1F45,0xBFD2,0xFEFC,0x5E6B,0x7CA0,0xDC37,0x9D19,
 0x3D8E,0xF78C,0x571B,0x1635,0xB6A2,0x9469,0x34FE,0x75D0,0xD547,0x3046,0x90D1,
 0xD1FF,0x7168,0x53A3,0xF334,0xB21A,0x128D,0x8689,0x261E,0x6730,0xC7A7,0xE56C,
 0x45FB,0x04D5,0xA442,0x4143,0xE1D4,0xA0FA,0x006D,0x22A6,0x8231,0xC31F,0x6388,
 0xA98A,0x091D,0x4833,0xE8A4,0xCA6F,0x6AF8,0x2BD6,0x8B41,0x6E40,0xCED7,0x8FF9,
 0x2F6E,0x0DA5,0xAD32,0xEC1C,0x4C8B,0x6483,0xC414,0x853A,0x25AD,0x0766,0xA7F1,
 0xE6DF,0x4648,0xA349,0x03DE,0x42F0,0xE267,0xC0AC,0x603B,0x2115,0x8182,0x4B80,
 0xEB17,0xAA39,0x0AAE,0x2865,0x88F2,0xC9DC,0x694B,0x8C4A,0x2CDD,0x6DF3,0xCD64,
 0xEFAF,0x4F38,0x0E16,0xAE81,0x3A85,0x9A12,0xDB3C,0x7BAB,0x5960,0xF9F7,0xB8D9,
 0x184E,0xFD4F,0x5DD8,0x1CF6,0xBC61,0x9EAA,0x3E3D,0x7F13,0xDF84,0x1586,0xB511,
 0xF43F,0x54A8,0x7663,0xD6F4,0x97DA,0x374D,0xD24C,0x72DB,0x33F5,0x9362,0xB1A9,
 0x113E,0x5010,0xF087};

/*===========================================================================*
 * Lookup table for calculating 32-bit CRCs
 *===========================================================================*/

static ulong Crc32Table[256] = {

          0x00000000 , 0x77073096 , 0xEE0E612C , 0x990951BA ,
          0x076DC419 , 0x706AF48F , 0xE963A535 , 0x9E6495A3 ,
          0x0EDB8832 , 0x79DCB8A4 , 0xE0D5E91E , 0x97D2D988 ,
          0x09B64C2B , 0x7EB17CBD , 0xE7B82D07 , 0x90BF1D91 ,

          0x1DB71064 , 0x6AB020F2 , 0xF3B97148 , 0x84BE41DE ,
          0x1ADAD47D , 0x6DDDE4EB , 0xF4D4B551 , 0x83D385C7 ,
          0x136C9856 , 0x646BA8C0 , 0xFD62F97A , 0x8A65C9EC ,
          0x14015C4F , 0x63066CD9 , 0xFA0F3D63 , 0x8D080DF5 ,

          0x3B6E20C8 , 0x4C69105E , 0xD56041E4 , 0xA2677172 ,
          0x3C03E4D1 , 0x4B04D447 , 0xD20D85FD , 0xA50AB56B ,
          0x35B5A8FA , 0x42B2986C , 0xDBBBC9D6 , 0xACBCF940 ,
          0x32D86CE3 , 0x45DF5C75 , 0xDCD60DCF , 0xABD13D59 ,

          0x26D930AC , 0x51DE003A , 0xC8D75180 , 0xBFD06116 ,
          0x21B4F4B5 , 0x56B3C423 , 0xCFBA9599 , 0xB8BDA50F ,
          0x2802B89E , 0x5F058808 , 0xC60CD9B2 , 0xB10BE924 ,
          0x2F6F7C87 , 0x58684C11 , 0xC1611DAB , 0xB6662D3D ,

          0x76DC4190 , 0x01DB7106 , 0x98D220BC , 0xEFD5102A ,
          0x71B18589 , 0x06B6B51F , 0x9FBFE4A5 , 0xE8B8D433 ,
          0x7807C9A2 , 0x0F00F934 , 0x9609A88E , 0xE10E9818 ,
          0x7F6A0DBB , 0x086D3D2D , 0x91646C97 , 0xE6635C01 ,

          0x6B6B51F4 , 0x1C6C6162 , 0x856530D8 , 0xF262004E ,
          0x6C0695ED , 0x1B01A57B , 0x8208F4C1 , 0xF50FC457 ,
          0x65B0D9C6 , 0x12B7E950 , 0x8BBEB8EA , 0xFCB9887C ,
          0x62DD1DDF , 0x15DA2D49 , 0x8CD37CF3 , 0xFBD44C65 ,

          0x4DB26158 , 0x3AB551CE , 0xA3BC0074 , 0xD4BB30E2 ,
          0x4ADFA541 , 0x3DD895D7 , 0xA4D1C46D , 0xD3D6F4FB ,
          0x4369E96A , 0x346ED9FC , 0xAD678846 , 0xDA60B8D0 ,
          0x44042D73 , 0x33031DE5 , 0xAA0A4C5F , 0xDD0D7CC9 ,

          0x5005713C , 0x270241AA , 0xBE0B1010 , 0xC90C2086 ,
          0x5768B525 , 0x206F85B3 , 0xB966D409 , 0xCE61E49F ,
          0x5EDEF90E , 0x29D9C998 , 0xB0D09822 , 0xC7D7A8B4 ,
          0x59B33D17 , 0x2EB40D81 , 0xB7BD5C3B , 0xC0BA6CAD ,

          0xEDB88320 , 0x9ABFB3B6 , 0x03B6E20C , 0x74B1D29A ,
          0xEAD54739 , 0x9DD277AF , 0x04DB2615 , 0x73DC1683 ,
          0xE3630B12 , 0x94643B84 , 0x0D6D6A3E , 0x7A6A5AA8 ,
          0xE40ECF0B , 0x9309FF9D , 0x0A00AE27 , 0x7D079EB1 ,

          0xF00F9344 , 0x8708A3D2 , 0x1E01F268 , 0x6906C2FE ,
          0xF762575D , 0x806567CB , 0x196C3671 , 0x6E6B06E7 ,
          0xFED41B76 , 0x89D32BE0 , 0x10DA7A5A , 0x67DD4ACC ,
          0xF9B9DF6F , 0x8EBEEFF9 , 0x17B7BE43 , 0x60B08ED5 ,

          0xD6D6A3E8 , 0xA1D1937E , 0x38D8C2C4 , 0x4FDFF252 ,
          0xD1BB67F1 , 0xA6BC5767 , 0x3FB506DD , 0x48B2364B ,
          0xD80D2BDA , 0xAF0A1B4C , 0x36034AF6 , 0x41047A60 ,
          0xDF60EFC3 , 0xA867DF55 , 0x316E8EEF , 0x4669BE79 ,

          0xCB61B38C , 0xBC66831A , 0x256FD2A0 , 0x5268E236 ,
          0xCC0C7795 , 0xBB0B4703 , 0x220216B9 , 0x5505262F ,
          0xC5BA3BBE , 0xB2BD0B28 , 0x2BB45A92 , 0x5CB36A04 ,
          0xC2D7FFA7 , 0xB5D0CF31 , 0x2CD99E8B , 0x5BDEAE1D ,

          0x9B64C2B0 , 0xEC63F226 , 0x756AA39C , 0x026D930A ,
          0x9C0906A9 , 0xEB0E363F , 0x72076785 , 0x05005713 ,
          0x95BF4A82 , 0xE2B87A14 , 0x7BB12BAE , 0x0CB61B38 ,
          0x92D28E9B , 0xE5D5BE0D , 0x7CDCEFB7 , 0x0BDBDF21 ,

          0x86D3D2D4 , 0xF1D4E242 , 0x68DDB3F8 , 0x1FDA836E ,
          0x81BE16CD , 0xF6B9265B , 0x6FB077E1 , 0x18B74777 ,
          0x88085AE6 , 0xFF0F6A70 , 0x66063BCA , 0x11010B5C ,
          0x8F659EFF , 0xF862AE69 , 0x616BFFD3 , 0x166CCF45 ,

          0xA00AE278 , 0xD70DD2EE , 0x4E048354 , 0x3903B3C2 ,
          0xA7672661 , 0xD06016F7 , 0x4969474D , 0x3E6E77DB ,
          0xAED16A4A , 0xD9D65ADC , 0x40DF0B66 , 0x37D83BF0 ,
          0xA9BCAE53 , 0xDEBB9EC5 , 0x47B2CF7F , 0x30B5FFE9 ,

          0xBDBDF21C , 0xCABAC28A , 0x53B39330 , 0x24B4A3A6 ,
          0xBAD03605 , 0xCDD70693 , 0x54DE5729 , 0x23D967BF ,
          0xB3667A2E , 0xC4614AB8 , 0x5D681B02 , 0x2A6F2B94 ,
          0xB40BBE37 , 0xC30C8EA1 , 0x5A05DF1B , 0x2D02EF8D

}; /* end of Crc32Table[] */

/*============================================================================*
 * crcfile() - Gets a file's 16-bit CRC and length.
 *
 * PURPOSE:
 *   This subroutine gets the length and CRC (cyclic redundancy check) value
 *   associated with the file named 'fname'.
 *
 * RETURNS:
 *   0, if successful.  The length is copied to *flen and the CRC is copied
 *   to *crc.
 *
 *   If an error occurred (can't open or read the file), then 1 is returned.
 *
 * NOTE:
 *   The code to calculate the CRC and much of the its description was taken
 *   from the crc_chk.c module written by Jim Czenkusch in IBM Austin, TX.
 *   He did the hard part - I only took the code and wrapped another interface
 *   around it.
 *
 * REMARKS:
 *   This program generates and validates the CRC signatures for a file.
 *   The CRC signature is calculated using the following polynomial...
 *
 *     X**16 + X**15 + X**13 + X**7 + X**4 + X**2 + X + 1
 *
 *   where the register is initialized to 0x0000.  In addition to
 *   calculating the file's CRC signature, the program also counts
 *   the number of bytes that were processed to provided extra
 *   insurance and allow some small measure of debug capacity.
 *============================================================================*/
int crcfile(fname, crc, flen)

char     *fname;                    /* Pointer to name of a message file */
ushort   *crc;                      /* Pointer to location to store CRC in */
ulong    *flen;                     /* Pointer to location to store length in */

{
  int     rc;                       /* Return code storage */
  int     cnt;                      /* Count */

  int     cfile;                    /* File handle */
  ushort  crc_reg;                  /* CRC register value from the file */
  ulong   actual_len;               /* Actual file length */

  unsigned char *data;              /* Pointer to data */
  unsigned char in_buffer[BUFFLEN]; /* Buffer in which to read file */

  *crc = 0;                         /* Init. CRC value */
  crc_reg = 0;

  *flen = 0L;                       /* Init. file length */
  actual_len = 0L;

 /*---------------------------------------------------------------------------*
  * Open the file for reading
  *---------------------------------------------------------------------------*/

  cfile = open(fname, O_RDONLY | O_BINARY);
  if (cfile == -1)
     return(1);

 /*---------------------------------------------------------------------------*
  * Calculate CRC for the file: Main Loop
  *---------------------------------------------------------------------------*/

  for (;;)                          /* For each BUFFLEN block of the file: */
  {
    /*----------------------------------------------------------------------*
     * Read a block of the file
     *----------------------------------------------------------------------*/

     cnt = read(cfile, &in_buffer[0], BUFFLEN);
     if (cnt == -1)                      /* If we got an error: */
     {
        close(cfile);                         /* Close the file */
        return(1);                            /* Return with error */
     }

     if (cnt == 0)                       /* If we read no data: */
        break;                           /* We're done! */

    /*----------------------------------------------------------------------*
     * Update the CRC value in 'crc_reg'
     *----------------------------------------------------------------------*/

     data = in_buffer;
     while (cnt > 0 )
     {
       crc_reg =
          (crc_reg << 8) ^
          transition_tbl[ ((crc_reg >> 8)^(unsigned short) (*(data++))) ];
       actual_len++;
       cnt--;
     }

  } /* end of main for loop */

 /*---------------------------------------------------------------------------*
  * Close the file, copy information back to caller, and return
  *---------------------------------------------------------------------------*/

  close(cfile);

  *crc = crc_reg;
  *flen = actual_len;

  return(0);
}

/*============================================================================*
 * crcfile32() - Gets a file's 32-bit CRC and length.
 *
 * PURPOSE:
 *   This subroutine gets the length and CRC (cyclic redundancy check) value
 *   associated with the file named 'fname'.
 *
 * RETURNS:
 *   0, if successful.  The length is copied to *flen and the CRC is copied
 *   to *crc.
 *
 *   If an error occurred (can't open or read the file), then 1 is returned.
 *============================================================================*/
int crcfile32(

char     *fname,                    /* Pointer to name of a message file */
ulong    *crc,                      /* Pointer to location to store CRC in */
ulong    *flen )                    /* Pointer to location to store length in */

{
  int     rc;                       /* Return code storage */
  int     cnt;                      /* Count */

  int     cfile;                    /* File handle */
  ulong   crc_reg;                  /* CRC register value from the file */
  ulong   actual_len;               /* Actual file length */

  ulong   a, b;                     /* Temp. ulong values */

  uchar  *data;                     /* Pointer to data */
  uchar   buffer[BUFFLEN];          /* Buffer in which to read file */

  actual_len = 0L;                  /* Init. file length */
  *flen = 0L;

 /*---------------------------------------------------------------------------*
  * The CRC value (in crc_reg) is preconditioned with all 1s.  After the file
  * has been processed, the CRC value is inverted (one's complement) to give
  * us a value that is compatible with the value generated by PKZIP.
  *---------------------------------------------------------------------------*/

  crc_reg = 0xFFFFFFFFL;            /* Init. CRC value */
  *crc = crc_reg;

 /*---------------------------------------------------------------------------*
  * Open the file for reading
  *---------------------------------------------------------------------------*/

  cfile = open(fname, O_RDONLY | O_BINARY);
  if (cfile == -1)
     return(1);

 /*---------------------------------------------------------------------------*
  * Calculate CRC for the file: Main Loop
  *---------------------------------------------------------------------------*/

  for (;;)                          /* For each BUFFLEN block of the file: */
  {
    /*----------------------------------------------------------------------*
     * Read a block of the file
     *----------------------------------------------------------------------*/

     cnt = read(cfile, &buffer[0], BUFFLEN);
     if (cnt == -1)                      /* If we got an error: */
     {
        close(cfile);                         /* Close the file */
        return(1);                            /* Return with error */
     }

     if (cnt == 0)                       /* If we read no data: */
        break;                           /* We're done! */

    /*----------------------------------------------------------------------*
     * Update the CRC value in 'crc_reg'
     *----------------------------------------------------------------------*/

     data = buffer;
     while (cnt > 0 )
     {
        a = (crc_reg >> 8) & 0x00FFFFFFL;
        b = Crc32Table[ ( (int) crc_reg ^ *data++ ) & 0xFF ];
        crc_reg = a ^ b;

        actual_len++;
        cnt--;
     }

  } /* end of main for loop */

 /*---------------------------------------------------------------------------*
  * Close the file, copy information back to caller, and return
  *---------------------------------------------------------------------------*/

  close(cfile);

  *crc = crc_reg ^ 0xFFFFFFFFL;
  *flen = actual_len;

  return(0);
}
