/******************************************************************************/
/* Module  : maths_s.c                                                        */
/* Purpose : Perform the necessary setup for the server manager code          */
/*           module maths_m.c. This module registers manager EPV,             */
/*           selects all protocol sequences available, advertises the server  */
/*           and listens for RPC requests. Uses string binding.               */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dce/rpc.h>
#ifdef _WINDOWS
#include <dce/dcewin.h>
#endif
#include "errchk.h"
#include "maths.h"

#define MAX_CONC_CALLS_PROTSEQ  4       /* Max concurrent calls per protocol. */
#define MAX_CONC_CALLS_TOTAL    8       /* Max concurrent calls total.        */

int main ( int argc, char *argv[] )
{
   int i;
   rpc_binding_vector_t *bv_p;
   unsigned32 status;
   char *str_bind_p;

   /* Register interface/epv associations with RPC runtime.                   */
   printf("Registering server interface with RPC runtime...\n\n");
   rpc_server_register_if ( maths_v1_0_s_ifspec, NULL, NULL, &status );
   ERRCHK ( status );

   /* Inform RPC runtime to use all supported protocol sequences.             */
   rpc_server_use_all_protseqs ( MAX_CONC_CALLS_PROTSEQ, &status );
   ERRCHK ( status );

   /* Get the binding handle vector from RPC runtime.                         */
   rpc_server_inq_bindings ( &bv_p, &status );
   ERRCHK ( status );

   /* Display string binding.                                                 */
   for ( i = 0 ; i < bv_p->count ; i++ ) {
       rpc_binding_to_string_binding ( ( bv_p->binding_h )[ i ], &str_bind_p,
                                   &status );
       ERRCHK ( status );

       printf ( "String binding is : %s\n", str_bind_p );

       rpc_string_free( &str_bind_p, &status);
       ERRCHK ( status );
   }

   /* Register binding information with endpoint map.                         */
   printf("\nRegistering server endpoints with endpoint mapper (RPCD)...\n");
   rpc_ep_register ( maths_v1_0_s_ifspec, bv_p, NULL,
                     ( unsigned_char_t * )"String math server, version 1.0",
                     &status );
   ERRCHK ( status );

   /* Listen for service requests.                                            */
   printf ( "Server String listening...\n"  );
   rpc_server_listen ( MAX_CONC_CALLS_TOTAL, &status );
   ERRCHK ( status );
}
