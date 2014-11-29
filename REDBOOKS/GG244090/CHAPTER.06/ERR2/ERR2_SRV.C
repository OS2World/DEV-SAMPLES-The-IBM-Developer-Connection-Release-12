#include "err2.h"
#include "errorchk.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WINDOWS
#include <dce/pthreadx.h>
#include <dce/dcewin.h>
#else
#include <pthread_exc.h>
#endif

#define MAX_CALL_REQUESTS 3

int main( int argc, char *argv[] )
{
   unsigned32 st;
   rpc_binding_vector_t  *bvec;

#ifndef _WINDOWS
   sigset_t              sigset;
   pthread_t             this_thread = pthread_self();
#endif

#ifdef IBMOS2
   pthread_inst_exception_handler();
#endif

   /* Register interface/epv associations with rpc runtime.                   */
   printf("Registering server interface with RPC runtime...\n");
   rpc_server_register_if( MyError_v1_0_s_ifspec, NULL, NULL, &st );
   ERRORCK( "rpc_server_register_if", st );

   rpc_server_use_protseq ( "ncadg_ip_udp", MAX_CALL_REQUESTS, &st );
   ERRORCK( "rpc_server_use_protseq", st );

   rpc_server_inq_bindings( &bvec, &st );
   ERRORCK( "rpc_server_inq_binding", st );

   /* Register binding information with endpoint map                          */
   printf("Registering server endpoints with endpoint mapper (RPCD)...\n");
   rpc_ep_register( MyError_v1_0_s_ifspec, bvec, NULL,
       (unsigned_char_t *)"MyError2, version 1.0", &st );
   ERRORCK( "rpc_ep_register", st );

   /* Export binding info to the namespace.                                   */
   printf("Exporting server bindings into CDS namespace...\n");
   rpc_ns_binding_export( rpc_c_ns_syntax_dce, ENTRY_NAME,
         MyError_v1_0_s_ifspec, bvec, NULL, &st );
   ERRORCK( "rpc_ns_binding_export", st );

#ifndef _WINDOWS
   sigemptyset ( &sigset );
   sigaddset ( &sigset, SIGINT );
   sigaddset ( &sigset, SIGTERM);
   pthread_signal_to_cancel_np ( &sigset, &this_thread );
#endif

   TRY
      /* Listen for service requests.                                         */
      printf( "Server %s listening...\n", ENTRY_NAME );
      rpc_server_listen( MAX_CALL_REQUESTS, &st );
   FINALLY
      printf("Unexporting server bindings from CDS namespace...\n");
      rpc_ns_binding_unexport( rpc_c_ns_syntax_dce, ENTRY_NAME,
            MyError_v1_0_s_ifspec, NULL, &st );
      ERRORCK( "rpc_ns_binding_unexport", st );

      printf("Unregistering server interface with RPC runtime...\n");
      rpc_server_unregister_if( MyError_v1_0_s_ifspec, NULL, &st );
      ERRORCK( "rpc_server_unregister_if", st );

      printf("Unregistering server endpoints with endpoint mapper (RPCD)...\n");
      rpc_ep_unregister( MyError_v1_0_s_ifspec, bvec, NULL, &st );
      ERRORCK( "rpc_ep_unregister", st );

#ifdef IBMOS2
      pthread_dinst_exception_handler();
#endif
      exit( 0 );
   ENDTRY;
}
