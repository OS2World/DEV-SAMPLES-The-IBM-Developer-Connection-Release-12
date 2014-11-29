#include "err5.h"
#include <stdio.h>
#include <stdlib.h>

error_status_t divide( long left, long right, long *result )
{
   if ( right == 0 ) {
      printf( "\n Oops, somebody tried to divide by zero.\n" );
      return rpc_s_fault_int_div_by_zero;
   }
   *result = left / right;
   printf( "\n %ld / %ld = %ld\n", left, right, *result );

   return rpc_s_ok;
}
