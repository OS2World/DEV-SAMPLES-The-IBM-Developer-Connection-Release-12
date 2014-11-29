/******************************************************************************/
/* Module  : ft_s.c                                                           */
/* Purpose : Perform the necessary setup for the server manager code          */
/*           module ft_m.c. This module registers manager EPV,                */
/*           selects all protocol sequences available, advertises the server  */
/*           and listens for RPC requests. Server name can be provided in     */
/*           the command line.                                                */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>
#include <dce/rpc.h>
#ifdef _WINDOWS
#include <dce/dcewin.h>
#endif
#include "errchk.h"
#include "ft.h"

#define MAX_CONC_CALLS_PROTSEQ  4       /* Max concurrent calls per protocol. */
#define MAX_CONC_CALLS_TOTAL    8       /* Max concurrent calls total.        */
#define DEFAULT_PATH "/.:/Servers/"     /* Server default namespace path.     */
#define SERVER_NAME_LENGTH    256       /* Server entry name length.          */
#define SERVER_B_NAME_LENGTH    8       /* Server base entry name length.     */

#ifdef _WINDOWS
extern void _far _pascal dce_cf_get_host_name ( void far *, void far * );
#endif

int main ( int argc, char *argv[] )
{
   rpc_binding_vector_t *bv_p;
   unsigned32 status;
   char *aux, *host_name, server_name[ SERVER_NAME_LENGTH + 1 ];
   int path_len;

#ifndef _WINDOWS
   /* Statements for Ctrl-C handling.                                         */
   sigset_t sigs;
   pthread_t setup_thread = pthread_self ();
#endif

#ifdef IBMOS2
   pthread_inst_exception_handler ();
#endif

   /* Register interface/epv associations with RPC runtime.                   */
   printf("Registering server interface with RPC runtime...\n");
   rpc_server_register_if ( ft_v1_0_s_ifspec, NULL, NULL, &status );
   ERRCHK ( status );

   /* Inform RPC runtime to use UDP protocol sequence.                        */
   rpc_server_use_protseq ("ncadg_ip_udp", MAX_CONC_CALLS_PROTSEQ, &status );
   ERRCHK ( status );

   /* Get the binding handle vector from RPC runtime.                         */
   rpc_server_inq_bindings ( &bv_p, &status );
   ERRCHK ( status );

   /* Register binding information with endpoint map.                         */
   printf("Registering server endpoints with endpoint mapper (RPCD)...\n");
   rpc_ep_register ( ft_v1_0_s_ifspec, bv_p, NULL,
                     ( unsigned_char_t * )"File transfer server, version 1.0",
                     &status );
   ERRCHK ( status );

   /* If server name has been supplied, use it.                               */
   if ( argc == 2 ) {
      strncpy ( server_name, argv[ 1 ], SERVER_NAME_LENGTH );
      server_name[ SERVER_NAME_LENGTH ] = '\0';
   }

   /* Else compose a default name using the host name.                        */
   else {
      path_len = strlen ( DEFAULT_PATH );
      strcpy ( server_name, DEFAULT_PATH );
      dce_cf_get_host_name ( &aux, &status );
      ERRCHK ( status );
      host_name = strrchr ( aux, '/' );
      host_name++;
      strncpy ( &server_name[ path_len ], host_name,
                SERVER_B_NAME_LENGTH );
      server_name[ path_len ] = toupper ( server_name[ path_len ] );
      server_name[ path_len + SERVER_B_NAME_LENGTH ] = '\0';
      free ( aux );
   }

   /* Export binding information to the namespace.                            */
   printf("Exporting server bindings into CDS namespace...\n");
   rpc_ns_binding_export ( rpc_c_ns_syntax_dce, server_name,
                           ft_v1_0_s_ifspec, bv_p, NULL, &status );
   ERRCHK ( status );

#ifndef _WINDOWS
   /* Ctrl-C handling to remove stale entries.                                */
   /* Make sure the setup thread receives the Ctrl-C in non-OS2 machines.     */
   sigemptyset ( &sigs );
   sigaddset ( &sigs, SIGINT );
   sigaddset ( &sigs, SIGTERM );
   if ( pthread_signal_to_cancel_np ( &sigs, &setup_thread ) != 0 ) {
      printf ( "Error in pthread_signal_to_cancel_np\n" );
      exit ( 1 );
   }
#endif

   TRY {
   /* Listen for service requests.                                            */
   printf ( "Server %s listening...\n", server_name );
   rpc_server_listen ( MAX_CONC_CALLS_TOTAL, &status );
   ERRCHK ( status );
   }
   CATCH_ALL {
   /* Upon receiving a Ctrl-C ...
   /* Remove the entry from the CDS.                                          */
      printf("Unexporting server bindings from CDS namespace...\n");
      rpc_ns_binding_unexport ( rpc_c_ns_syntax_dce, server_name,
                                ft_v1_0_s_ifspec,
                                ( uuid_vector_t * )NULL, &status );
      ERRCHK ( status );

   /* Unregister the interface.                                               */
      printf("Unregistering server interface with RPC runtime...\n");
      rpc_server_unregister_if ( ft_v1_0_s_ifspec, (uuid_t *)NULL,
                                 &status );
      ERRCHK ( status );

   /* Remove the entry from the enpoint map.                                  */
      printf("Unregistering server endpoints with endpoint mapper (RPCD)...\n");
      rpc_ep_unregister ( ft_v1_0_s_ifspec, bv_p,
                          ( uuid_vector_t * )NULL, &status );
      ERRCHK ( status );

#ifdef IBMOS2
      pthread_dinst_exception_handler();
#endif
      exit ( 0 );
      }
   ENDTRY;
}
