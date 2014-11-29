/******************************************************************************/
/* Module  : mathb_c.c                                                        */
/* Purpose : Client module for server mathb_s. Gets the values to be          */
/*           added from the command line and sends them to the remote add     */
/*           procedure together with the number of values passed and a        */
/*           pointer to the variable that will contain the result.            */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#ifdef _WINDOWS
#include <dce/dcewin.h>
#endif

#include "mathb.h"

int main ( int argc, char *argv[] )
{
   long int i, numv, sum;
   value_array_t va;

   /* Check if the user passed the minimum number of parameters.              */
   if ( argc < 3 ) {
      printf ( "Usage : %s <values to be added>\n", argv[ 0 ] );
      exit ( 1 );
   }

   /* Get values from the command line and load them in array va.             */
   for ( i = 0; i < MAX_VALUES && i < argc - 1; i++ )
      va[ i ] = atoi ( argv[ i + 1 ] );
   numv = i;

   /* Call add passing values, number of values and pointer to result.        */
   add ( va, ( long )numv, &sum );
   printf( "The sum is %d\n", sum );
}
