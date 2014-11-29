#include "err5.h"
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

   st = divide( left, right, &result );
   if ( st == rpc_s_ok )
      printf( "   Result = %ld\n", result );
   else if ( st == rpc_s_fault_int_div_by_zero )
      printf( "   Oops, I got a Divide By Zero fault.\n" );

   exit( 0 );
}
