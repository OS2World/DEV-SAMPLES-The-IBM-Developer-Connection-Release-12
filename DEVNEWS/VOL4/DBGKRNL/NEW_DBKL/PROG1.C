//  prog1.c
#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include "int3386.h"

int main( void );
int _System Add3Numbers( int, int, int );

//--------------------------------------------
int main()
{
   int i;

   INT3;
   i = Add3Numbers( 1, 2, 3 );
   i = Add3Numbers( 9, 10, 11 );
   return 0;
}
//--------------------------------------------
int _System Add3Numbers( int a, int b, int c )
{
   return a+b+c;
}
