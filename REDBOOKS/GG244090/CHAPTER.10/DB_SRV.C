/*****************************************************************************/
/* Module: database_server.c                                                 */
/*                                                                           */
/* Description:                                                              */
/*     This module does everything for explicit binding and then executes    */
/*     the manager code.  This module makes use of TRY/ENDTRY macro to       */
/*     catch the SIGTERM signal raised by Ctrl-C.                            */
/*                                                                           */
/*****************************************************************************/
#ifdef _WINDOWS
#include <dce/dcewin.h>
#include <dce/pthreadx.h>
#else
#include <pthread_exc.h>
#endif

#include "db.h"
#include "errorchk.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_CALL_REQUESTS 3
#define MAX_CONC_CALLS_PROTSEQ  4       /* Max concurrent call per protocol.  */

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

   printf("Registering server interface with RPC runtime...\n");
   rpc_server_register_if( Database_v1_0_s_ifspec, NULL, NULL, &st );
   ERRORCK( "rpc_server_register_if", st );

   switch ( (( argc > 1 ) ? *argv[1] | ' ' : 'a') ) {
   case 't':
      rpc_server_use_protseq ("ncacn_ip_tcp", MAX_CONC_CALLS_PROTSEQ, &st );
      break;
   case 'u':
      rpc_server_use_protseq ("ncadg_ip_udp", MAX_CONC_CALLS_PROTSEQ, &st );
      break;
   case 'a':
   default:
      rpc_server_use_all_protseqs ( MAX_CONC_CALLS_PROTSEQ, &st );
   }
   ERRORCK( "rpc_server_use_all_protseqs", st );

   rpc_server_inq_bindings( &bvec, &st );
   ERRORCK( "rpc_server_inq_binding", st );

   printf("Registering server endpoints with endpoint mapper (RPCD)...\n");
   rpc_ep_register( Database_v1_0_s_ifspec, bvec, NULL,
                    ( unsigned_char_t * )"Database server, version 1.0" , &st );
   ERRORCK( "rpc_ep_register", st );

   rpc_ns_binding_export( rpc_c_ns_syntax_dce, ENTRY_NAME,
         Database_v1_0_s_ifspec, bvec, NULL, &st );
   ERRORCK( "rpc_ns_binding_export", st );

#ifndef _WINDOWS
   sigemptyset ( &sigset );
   sigaddset ( &sigset, SIGINT );
   sigaddset ( &sigset, SIGTERM );
   pthread_signal_to_cancel_np ( &sigset, &this_thread );
#endif

   TRY {
      printf ( "Server %s listening...\n", ENTRY_NAME );
      rpc_server_listen( MAX_CALL_REQUESTS, &st );
   }
   FINALLY {
      printf("Unexporting server bindings from CDS namespace...\n");
      rpc_ns_binding_unexport( rpc_c_ns_syntax_dce, ENTRY_NAME,
            Database_v1_0_s_ifspec, NULL, &st );
      ERRORCK( "rpc_ns_binding_unexport", st );

      printf("Unregistering server interface with RPC runtime...\n");
      rpc_server_unregister_if( Database_v1_0_s_ifspec, NULL, &st );
      ERRORCK( "rpc_server_unregister_if", st );

      printf("Unregistering server endpoints with endpoint mapper (RPCD)...\n");
      rpc_ep_unregister( Database_v1_0_s_ifspec, bvec, NULL, &st );
      ERRORCK( "rpc_ep_unregister", st );

#ifdef IBMOS2
      pthread_dinst_exception_handler();
#endif
      exit( 0 );
   }
   ENDTRY
}

