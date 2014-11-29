#include "err7.h"
#include "errorchk.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WINDOWS
#include <dce/dcewin.h>
#endif

int main( int argc, char *argv[] )
{
   long result;
   long left  ;
   long right ;
   error_status_t  st;

   if ( argc < 3 ) {
      printf ( "Usage : %s <dividend> <divisor>\n", argv[ 0 ] );
      exit ( 1 );
   }

   left  = atol( argv[1] );
   right = atol( argv[2] );

   divide( left, right, &result, &st );
   ERRORCK( "divide()", st );
   printf( "   Result = %ld\n", result );

   exit( 0 );
}
