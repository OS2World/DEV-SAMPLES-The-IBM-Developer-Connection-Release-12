#include "err6.h"
#include <stdio.h>
#include <stdlib.h>

void divide( long left, long right, long *result, error_status_t *st )
{
   if ( right == 0 ) {
      printf( "\n Oops, somebody tried to divide by zero.\n" );
      *st = rpc_s_fault_int_div_by_zero;
      return;
   }
   *result = left / right;
   *st = rpc_s_ok;
   printf( "\n %ld / %ld = %ld\n", left, right, *result );

   return;
}
