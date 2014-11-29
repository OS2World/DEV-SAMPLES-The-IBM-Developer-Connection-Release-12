/******************************************************************************/
/* Module  : mathx_c.c                                                        */
/* Purpose : Client module for server mathx_s. Gets the values to be          */
/*           added from the command line and sends them to the remote add     */
/*           procedure together with the number of values passed and a        */
/*           pointer to the variable that will contain the result.            */
/*           Uses explicit binding.                                           */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dce/rpc.h>
#ifdef _WINDOWS
#include <dce/dcewin.h>
#endif
#include "errchk.h"
#include "mathx.h"

#define ENTRY_NAME "/.:/Servers/MathX"    /* Server entry name.               */

int main ( int argc, char *argv[] )
{
   long int i, numv, sum;
   value_array_t va;
   rpc_binding_handle_t bh;
   unsigned32 status;
   rpc_ns_handle_t imp_ctxt;

   /* Check if the user passed the minimum number of parameters.              */
   if ( argc < 3 ) {
      printf ( "Usage : %s <values to be added>\n", argv[ 0 ] );
      exit ( 1 );
   }

   /* Set up the context to import the bindings.                              */
   rpc_ns_binding_import_begin ( rpc_c_ns_syntax_dce, ENTRY_NAME,
                                 mathx_v1_0_c_ifspec, NULL, &imp_ctxt,
                                 &status );
   ERRCHK ( status );

   /* Get the first binding handle.                                           */
   rpc_ns_binding_import_next ( imp_ctxt, &bh, &status );
   ERRCHK ( status );

   /* Get values from the command line and load them in array va.             */
   for ( i = 0; i < MAX_VALUES && i < argc - 1; i++ )
      va[ i ] = atoi ( argv[ i + 1 ] );
   numv = i;

   /* Call add passing handle, values, number of values and pointer to result.*/
   add ( bh, va, ( long )numv, &sum );
   printf ( "The sum is %d\n", sum );

   /* Release the context.                                                    */
   rpc_ns_binding_import_done ( &imp_ctxt, &status );
   ERRCHK ( status );
}
