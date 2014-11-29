#include "err2.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WINDOWS
#include <dce/pthreadx.h>
#include <dce/dcewin.h>
#else
#include <pthread_exc.h>
#endif

int main( int argc, char *argv[] )
{
   long result;
   long left  ;
   long right ;

#ifndef _WINDOWS
   sigset_t              sigset;
   pthread_t             this_thread = pthread_self();
#endif

#ifdef IBMOS2
   pthread_inst_exception_handler();
#endif

   if ( argc < 3 ) {
      printf ( "Usage : %s <dividend> <divisor>\n", argv[ 0 ] );
      exit ( 1 );
   }

   left  = atol( argv[1] );
   right = atol( argv[2] );


#ifndef _WINDOWS
   sigemptyset ( &sigset );
   sigaddset ( &sigset, SIGINT );
   sigaddset ( &sigset, SIGTERM );
   pthread_signal_to_cancel_np ( &sigset, &this_thread );
#endif

   TRY
      divide( left, right, &result );
      printf( "   Result = %ld\n", result );
   CATCH( exc_intdiv_e )
      printf( "   Oops, I got a Divide By Zero exception.\n" );
   ENDTRY

#ifdef IBMOS2
   pthread_dinst_exception_handler();
#endif

   exit( 0 );
}
