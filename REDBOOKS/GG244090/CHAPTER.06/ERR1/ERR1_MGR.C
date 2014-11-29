#include "err1.h"
#include <stdio.h>
#include <stdlib.h>

void divide( long left, long right, long *result )
{
   *result = left / right;
   printf( "\n %ld / %ld = %ld\n", left, right, *result );
}
