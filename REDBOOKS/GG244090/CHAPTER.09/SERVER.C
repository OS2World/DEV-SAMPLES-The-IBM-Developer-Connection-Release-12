/***************************************************************************/
/*                                                                         */
/* Module:      server.c                                                   */
/*                                                                         */
/* Description: DCE RPC setup for the MessageBox application.              */
/*              Catches ^C for cleaning up binding information and         */
/*              exporting the mbox structure to a local file.              */
/*                                                                         */
/***************************************************************************/

# include <stdlib.h>
# include <pthread.h>
# include <dce/rpc.h>
# ifdef _WINDOWS
# include <dce/dcewin.h>
# endif

# include "mbox.h"
# include "common.h"

# define MAX_CONC_CALLS_PROTSEQ         2
# define MAX_CONC_CALLS_TOTAL           4
# define IF_HANDLE                      MessageBox_v2_0_s_ifspec

# ifdef _AIX
# define FNAME                          "/tmp/message.box"
# else
# define FNAME                          "message.box"
# endif

int mbox_import( const char * );
int mbox_export( const char * );

int main ( int argc, char *argv[] )
{
        unsigned_char_t         *server_name;
        rpc_binding_vector_t    *bind_vector_p;
        unsigned32              status;
# ifndef _WINDOWS
        sigset_t                sigset;
        pthread_t               this_thread     = pthread_self();
# endif

# ifdef IBMOS2
        pthread_inst_exception_handler();
# endif

        /* Register the authentification level which will be used */
        printf("Registering authentication level with RPC runtime...\n");
        rpc_server_register_auth_info(
                PRINCIPAL_NAME,
                rpc_c_authn_default,
                NULL,
                NULL,
                &status
        );
        ERRCHK( status );

        /* Register interface/epv associations with rpc runtime. */
        printf("Registering server interface with RPC runtime...\n");
        rpc_server_register_if ( IF_HANDLE, NULL, NULL, &status );
        ERRCHK( status );

        /* Inform rpc runtime of a protocol sequence to use. */
        switch ( (( argc > 1 ) ? *argv[1] | ' ' : 'a') ) {
        case 't':
           rpc_server_use_protseq ("ncacn_ip_tcp", MAX_CONC_CALLS_PROTSEQ, &status );
           break;
        case 'u':
           rpc_server_use_protseq ("ncadg_ip_udp", MAX_CONC_CALLS_PROTSEQ, &status );
           break;
        case 'a':
        default:
           rpc_server_use_all_protseqs ( MAX_CONC_CALLS_PROTSEQ, &status );
        }
        ERRCHK( status );

        /* Ask the runtime which binding handle will be used. */
        rpc_server_inq_bindings( &bind_vector_p, &status );
        ERRCHK( status );

        /* Register binding information with endpoint map */
        printf("Registering server endpoints with endpoint mapper (RPCD)...\n");
        rpc_ep_register(
                IF_HANDLE,
                bind_vector_p,
                NULL,
                "Message Box server, version 2.0",
                &status
        );
        ERRCHK( status );

        /* Export binding info to the namespace. */
        printf("Exporting server bindings into CDS namespace...\n");
        rpc_ns_binding_export (
                rpc_c_ns_syntax_default,
                ENTRY_NAME,
                IF_HANDLE,
                bind_vector_p,
                NULL,
                &status
        );
        ERRCHK( status );

# ifndef _WINDOWS
        /* baggage to handle ctrl-C */
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGINT);
        sigaddset(&sigset, SIGTERM);
        if (pthread_signal_to_cancel_np(&sigset, &this_thread) != 0) {
                printf("pthread_signal_to_cancel_np failed\n");
                exit(1);
        }
# endif

        /* Import the mbox structure from file FNAME */
        mbox_import(FNAME);

        TRY {
                /* Listen for service requests. */
                printf( "Server %s listening...\n", ENTRY_NAME );
                rpc_server_listen ( MAX_CONC_CALLS_TOTAL, &status );
                ERRCHK( status );
        }
        FINALLY {
                /* Export the mbox structure to file FNAME */
                mbox_export(FNAME);

                /* Unexport the binding information from the namespace. */
                printf("Unexporting server bindings from CDS namespace...\n");
                rpc_ns_binding_unexport (
                        rpc_c_ns_syntax_default,
                        ENTRY_NAME,
                        IF_HANDLE,
                        NULL,
                        &status
                );
                ERRCHK( status );

                /* Unregister interface from RPC runtime */
                printf("Unregistering server interface with RPC runtime...\n");
                rpc_server_unregister_if ( IF_HANDLE, NULL, &status );
                ERRCHK( status );

                /* Unregister interface from EPV */
                printf("Unregistering server endpoints with endpoint mapper (RPCD)...\n");
                rpc_ep_unregister ( IF_HANDLE, bind_vector_p, NULL, &status );
                ERRCHK( status );

# ifdef IBMOS2
                pthread_dinst_exception_handler();
# endif
                exit ( 0 );
        }
        ENDTRY;
}
