#include "err4.h"
#include <stdio.h>
#include <stdlib.h>

error_status_t divide( long left, long right, long *result )
{
   *result = left / right;
   printf( "\n %ld / %ld = %ld\n", left, right, *result );

   return rpc_s_ok;
}
