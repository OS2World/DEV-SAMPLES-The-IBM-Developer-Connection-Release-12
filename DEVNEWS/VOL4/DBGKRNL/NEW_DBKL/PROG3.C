//  prog3.c
#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include "int3386.h"

int main( void );
VOID _System LoadBuffer( PSZ pBuf );
PSZ _System my_strcpy( PSZ pDest, PSZ pSrc );
//--------------------------------------------
int main()
{
   PSZ          psz;

   INT3;
   DosAllocMem( (PPVOID)&psz, 4096, PAG_COMMIT | PAG_READ | PAG_WRITE );
   LoadBuffer( psz + 4090 );
   return 0;
}
//--------------------------------------------
VOID _System loadBuffer( PSZ pDest )
{
   my_strcpy( pDest, "Hello World" );
}
//--------------------------------------------
PSZ _System my_strcpy( PSZ pDest, PSZ pSrc )
{
   PSZ pRet;

   pRet = pDest;
   while ( *pDest++ = *pSrc++ );
   return pRet;
}
