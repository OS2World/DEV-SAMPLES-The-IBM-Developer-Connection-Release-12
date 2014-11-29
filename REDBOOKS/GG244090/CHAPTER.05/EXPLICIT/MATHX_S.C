/******************************************************************************/
/* Module  : mathx_s.c                                                        */
/* Purpose : Perform the necessary setup for the server manager code          */
/*           module mathx_m.c. This module registers manager EPV,             */
/*           selects all protocol sequences available, advertises the server  */
/*           and listens for RPC requests.                                    */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dce/rpc.h>
#ifdef _WINDOWS
#include <dce/dcewin.h>
#endif
#include "errchk.h"
#include "mathx.h"

#define MAX_CONC_CALLS_PROTSEQ  4       /* Max concurrent calls per protocol. */
#define MAX_CONC_CALLS_TOTAL    8       /* Max concurrent calls total.        */
#define ENTRY_NAME "/.:/Servers/MathX"  /* Server entry name.                 */

int main ( int argc, char *argv[] )
{
   rpc_binding_vector_t *bv_p;
   unsigned32 status;

   /* Register interface/epv associations with RPC runtime.                   */
   printf("Registering server interface with RPC runtime...\n");
   rpc_server_register_if ( mathx_v1_0_s_ifspec, NULL, NULL, &status );
   ERRCHK ( status );

   /* Inform RPC runtime to use a supported protocol sequences.               */
   rpc_server_use_protseq ( "ncadg_ip_udp", MAX_CONC_CALLS_PROTSEQ, &status );
   ERRCHK ( status );

   /* Get the binding handle vector from RPC runtime.                         */
   rpc_server_inq_bindings ( &bv_p, &status );
   ERRCHK ( status );

   /* Register binding information with endpoint map.                         */
   printf("Registering server endpoints with endpoint mapper (RPCD)...\n");
   rpc_ep_register ( mathx_v1_0_s_ifspec, bv_p, NULL,
                     ( unsigned_char_t * )"Explicit math server, version 1.0",
                     &status );
   ERRCHK ( status );

   /* Export binding information to the namespace.                            */
   printf("Exporting server bindings into CDS namespace...\n");
   rpc_ns_binding_export ( rpc_c_ns_syntax_dce, ENTRY_NAME,
                           mathx_v1_0_s_ifspec, bv_p, NULL, &status );
   ERRCHK ( status );

   /* Listen for service requests.                                            */
   printf ( "Server %s listening...\n", ENTRY_NAME );
   rpc_server_listen ( MAX_CONC_CALLS_TOTAL, &status );
   ERRCHK ( status );
}
