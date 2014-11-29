#include "err3.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WINDOWS
#include <dce/pthreadx.h>
#else
#include <pthread_exc.h>
#endif

void divide( long left, long right, long *result )
{
   if ( right == 0 ) {
      printf( "\n Oops, somebody tried to divide by zero.\n" );
      RAISE( exc_intdiv_e );
   }
   *result = left / right;
   printf( "\n %ld / %ld = %ld\n", left, right, *result );
}
