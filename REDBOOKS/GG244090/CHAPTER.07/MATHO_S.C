/******************************************************************************/
/* Module  : matho_s.c                                                        */
/* Purpose : Perform the necessary setup for the server manager code          */
/*           module matho_m.c. This module registers manager EPV,             */
/*           selects all protocol sequences available, advertises the server  */
/*           and listens for RPC requests. Uses object UUIDs.                 */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <dce/rpc.h>
#ifdef _WINDOWS
#include <dce/dcewin.h>
#endif
#include "errchk.h"
#include "matho.h"

#define MAX_CONC_CALLS_PROTSEQ  4       /* Max concurrent call per protocol.  */
#define MAX_CONC_CALLS_TOTAL    8       /* Max concurrent calls total.        */
#define ENTRY_NAME "/.:/Servers/MathO"  /* Server entry name.                 */

extern matho_v1_0_epv_t epv_int;        /* Integer manager EPV table.         */
extern matho_v1_0_epv_t epv_float;      /* Float manager EPV table.           */

char **argv_global;

int main ( int argc, char *argv[] )
{
   /* Define the object UUID vector.                                          */
   struct obj_uuid_vec_t {
      unsigned32 count;
#ifdef _WINDOWS
      uuid_t far *vec[ 3 ];
#else
      uuid_t     *vec[ 3 ];
#endif
   } obj_uuid_vec;

   uuid_t obj_uuid_int, obj_uuid_float, obj_uuid_double,
          type_uuid_int, type_uuid_float;
   rpc_binding_vector_t *bv_p;
   unsigned32 status;

   /* Statements for Ctrl-C handling.                                         */
#ifndef _WINDOWS
   sigset_t sigs;
   pthread_t setup_thread = pthread_self ();
#endif

#ifdef IBMOS2
   pthread_inst_exception_handler ();
#endif

   /* Create object UUID for integer and float math managers from string.     */
   uuid_from_string ( OBJ_UUID_INT, &obj_uuid_int, &status );
   ERRCHK ( status );
   uuid_from_string ( OBJ_UUID_FLOAT, &obj_uuid_float, &status );
   ERRCHK ( status );
   uuid_from_string ( OBJ_UUID_DOUBLE, &obj_uuid_double, &status );
   ERRCHK ( status );

   /* Initialize type UUIDs.                                                  */
   uuid_create ( &type_uuid_int, &status );
   ERRCHK ( status );
   uuid_create ( &type_uuid_float, &status );
   ERRCHK ( status );

   /* Associate object UUIDs with type UUIDs.                                 */
   rpc_object_set_type ( &obj_uuid_int, &type_uuid_int, &status );
   ERRCHK ( status );
   rpc_object_set_type ( &obj_uuid_float, &type_uuid_float, &status );
   ERRCHK ( status );
   rpc_object_set_type ( &obj_uuid_double, &type_uuid_float, &status );
   ERRCHK ( status );

   /* Register interface/epv associations with RPC runtime.                   */
   printf("Registering server interface with RPC runtime...\n");
   rpc_server_register_if ( matho_v1_0_s_ifspec, &type_uuid_int,
                            ( rpc_mgr_epv_t )&epv_int, &status );
   ERRCHK ( status );
   rpc_server_register_if ( matho_v1_0_s_ifspec, &type_uuid_float,
                            ( rpc_mgr_epv_t )&epv_float, &status );
   ERRCHK ( status );

   /* Inform RPC runtime to use selected supported protocol sequences.        */
   switch ( (( argc > 1 ) ? *argv[ 1 ] | ' ' : 'a')  ) {
   case 't' :
       rpc_server_use_protseq ("ncacn_ip_tcp", MAX_CONC_CALLS_PROTSEQ, &status );
       break;
   case 'u' :
       rpc_server_use_protseq ("ncadg_ip_udp", MAX_CONC_CALLS_PROTSEQ, &status );
       break;
   case 'a' :
   default:
      rpc_server_use_all_protseqs ( MAX_CONC_CALLS_PROTSEQ, &status );
   }
   ERRCHK ( status );

   /* Get the binding handle vector from RPC runtime.                         */
   rpc_server_inq_bindings ( &bv_p, &status );
   ERRCHK ( status );

   /* Initialize the object UUID vector.                                      */
   obj_uuid_vec.count = 3;
   obj_uuid_vec.vec[ 0 ] = &obj_uuid_int;
   obj_uuid_vec.vec[ 1 ] = &obj_uuid_float;
   obj_uuid_vec.vec[ 2 ] = &obj_uuid_double;

   /* Register binding information with endpoint map, including object UUIDs. */
   printf("Registering server endpoints with endpoint mapper (RPCD)...\n");
   rpc_ep_register ( matho_v1_0_s_ifspec, bv_p,
                     ( uuid_vector_t * )&obj_uuid_vec,
                     ( unsigned_char_t * )"Object math server, version 1.0",
                     &status );
   ERRCHK ( status );

   /* Export binding information to the namespace, including object UUIDs.    */
   printf("Exporting server bindings into CDS namespace...\n");
   rpc_ns_binding_export ( rpc_c_ns_syntax_dce, ENTRY_NAME,
                           matho_v1_0_s_ifspec, bv_p,
                           ( uuid_vector_t * )&obj_uuid_vec, &status );
   ERRCHK ( status );

   /* Make argv available to the manager routines.                            */
   argv_global = argv;

   /* Ctrl-C handling to remove stale entries.                                */

#ifndef _WINDOWS
   /* Make sure the setup thread receives the Ctrl-C in OS2 and AIX machines  */
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
   printf ( "Server %s listening...\n", ENTRY_NAME );
   rpc_server_listen ( MAX_CONC_CALLS_TOTAL, &status );
   ERRCHK ( status );
   }
   CATCH_ALL {
   /* Upon receiving a Ctrl-C ...
   /* Unregister the interface for all object UUIDs.                          */
      printf( "Interface being unregistered\n" );

   /* Remove the binding information from CDS.                                */
      printf("Unexporting server bindings from CDS namespace...\n");
      rpc_ns_binding_unexport ( rpc_c_ns_syntax_dce, ENTRY_NAME,
                                matho_v1_0_s_ifspec,
                                ( uuid_vector_t * )&obj_uuid_vec, &status );
      ERRCHK ( status );

      printf( "Unregistering server interface with RPC runtime...\n");
      rpc_server_unregister_if ( matho_v1_0_s_ifspec,
                                 (uuid_t *)&type_uuid_int, &status );
      ERRCHK ( status );
      rpc_server_unregister_if ( matho_v1_0_s_ifspec,
                                 (uuid_t *)&type_uuid_float, &status );
      ERRCHK ( status );

   /* Remove the entries from the endpoint map for all object UUIDs.          */
      printf("Unregistering server endpoints with endpoint mapper (RPCD)...\n");
      rpc_ep_unregister ( matho_v1_0_s_ifspec, bv_p,
                          ( uuid_vector_t * )&obj_uuid_vec, &status );
      ERRCHK ( status );

# ifdef IBMOS2
      pthread_dinst_exception_handler();
# endif
      exit ( 0 );
      }
   ENDTRY;
}
