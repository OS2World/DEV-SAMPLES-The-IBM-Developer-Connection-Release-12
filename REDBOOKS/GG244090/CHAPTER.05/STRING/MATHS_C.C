/******************************************************************************/
/* Module  : maths_c.c                                                        */
/* Purpose : Client module for server maths_s. Gets the values to be          */
/*           added from the command line and sends them to the remote add     */
/*           procedure together with the number of values passed and a        */
/*           pointer to the variable that will contain the result.            */
/*           Uses implicit binding.                                           */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dce/rpc.h>
#ifdef _WINDOWS
#include <dce/dcewin.h>
#endif
#include "errchk.h"
#include "maths.h"

int main ( int argc, char *argv[] )
{
   long int i, numv, sum;
   value_array_t va;
   rpc_binding_handle_t bh;
   unsigned32 status;

   /* Check if the user passed the minimum number of parameters.              */
   if ( argc < 3 ) {
      printf ( "Usage : %s <string binding>  <values to be added>\n",
               argv[ 0 ] );
      exit ( 1 );
   }

   /* Generate a binding handle from the string binding in the command line.  */
   rpc_binding_from_string_binding ( argv[ 1 ], &bh, &status );
   ERRCHK ( status );

   /* Store the binding handle bh in the global variable defined in .acf.     */
   global_handle = bh;

   /* Get values from the command line and load them in array va.             */
   for ( i = 0; i < MAX_VALUES && i < argc - 2; i++ )
      va[ i ] = atoi ( argv[ i + 2 ] );
   numv = i;

   /* Call add passing values, number of values and pointer to result.        */
   add ( va, ( long )numv, &sum );
   printf ( "The sum is %d\n", sum );
}
