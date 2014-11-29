/***************************************************************************/
/*                                                                         */
/* Module:      usecds.c                                                   */
/*                                                                         */
/* Description: DCE RPC setup for the CDS sample application.              */
/*              Advertises himself under the name given one the            */
/*              commandline. Adds this name to specified groups.           */
/*                                                                         */
/***************************************************************************/

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include <dce/rpc.h>
# include <dce/dce_cf.h>
# include <pthread.h>

# ifdef _WINDOWS
# include <dce/dcewin.h>
# endif

# include "usecds.h"
# include "errchk.h"

string_t        hoststr         = "";

#define MAX_CONC_CALLS_PROTSEQ  10      /* Max concurrent call per protocol.  */

int main ( int argc, char *argv[] )
{
        rpc_binding_vector_t    *bind_vector_p;
        unsigned32              status;
        char                    *hostname;
        int                     i;
# ifndef _WINDOWS
        sigset_t                sigset;
        pthread_t               this_thread     = pthread_self();
# endif

# ifdef IBMOS2
        pthread_inst_exception_handler();
# endif

        if ( argc < 2 ) {
                printf("usage: %s <name> [group,...[group]]\n",argv[0]);
                exit(1);
        }

        /* Register interface/EPV associations with RPC runtime. */
        printf("Registering server interface with RPC runtime...\n");
        rpc_server_register_if ( UseCDS_v1_1_s_ifspec, NULL, NULL, &status );
        ERRCHK ( status );

        /* Inform RPC runtime to use selected supported protocol sequences.        */
        rpc_server_use_protseq ("ncadg_ip_udp", MAX_CONC_CALLS_PROTSEQ, &status );
        ERRCHK ( status );

        /* Ask the runtime which binding handle will be used. */
        rpc_server_inq_bindings ( &bind_vector_p, &status );
        ERRCHK ( status );

        /* Register binding information with endpoint map */
        printf("Registering server endpoints with endpoint mapper (RPCD)...\n");
        rpc_ep_register (
                UseCDS_v1_1_s_ifspec,
                bind_vector_p,
                NULL,
                "UseCDS Version 1.0",
                &status
        );
        ERRCHK ( status );

        /* Export binding info to the namespace. */
        printf("Exporting server bindings into CDS namespace...\n");
        rpc_ns_binding_export (
                rpc_c_ns_syntax_default,
                argv[1],
                UseCDS_v1_1_s_ifspec,
                bind_vector_p,
                NULL,
                &status
        );
        ERRCHK ( status );

        /* Get hostname relative to cell root */
        dce_cf_get_host_name( &hostname, &status );
        ERRCHK ( status );

        sprintf ( hoststr, "This is entry %s from host %s.",
                  argv[1], strrchr ( hostname, '/' ) + 1 );

        /* Add entry as a member in specified groups */
        for ( i=2 ; i < argc ; i++ ) {
                rpc_ns_group_mbr_add (
                        rpc_c_ns_syntax_default,
                        argv[i],                 /* CDS name of goup     */
                        rpc_c_ns_syntax_default,
                        argv[1],                 /* CDS name of entry    */
                        &status
                );
                ERRCHK ( status );
                printf ( "Added %s in group %s\n", argv[1], argv[i] );
        }

# ifndef _WINDOWS
        /* handle CTRL-C */
        sigemptyset ( &sigset );
        sigaddset ( &sigset, SIGINT );
        sigaddset ( &sigset, SIGTERM);
        if ( pthread_signal_to_cancel_np ( &sigset, &this_thread ) != 0 ) {
                printf ( "pthread_signal_to_cancel_np failed\n" );
                exit ( 1 );
        }
# endif

        TRY {
                /* Listen for service requests. */
                printf ( "Server %s listening...\n", argv[1] );
                rpc_server_listen ( 10, &status );
                ERRCHK ( status );
        }
        CATCH_ALL {
                /* Remove entry from group(s) */
                printf("Removing entries from group(s)...\n");
                for ( i=2 ; i < argc ; i++ ) {
                        rpc_ns_group_mbr_remove (
                                rpc_c_ns_syntax_default,
                                argv[i],
                                rpc_c_ns_syntax_default,
                                argv[1],
                                &status
                        );
                        ERRCHK ( status );
                }

                /* Unexport the binding information from the namespace. */
                printf("Unexporting server bindings from CDS namespace...\n");
                rpc_ns_binding_unexport (
                        rpc_c_ns_syntax_default,
                        argv[1],
                        UseCDS_v1_1_s_ifspec,
                        NULL,
                        &status
                );
                ERRCHK ( status );

                /* Unregister interface from RPC runtime */
                printf("Unregistering server interface with RPC runtime...\n");
                rpc_server_unregister_if (
                        UseCDS_v1_1_s_ifspec,
                        NULL,
                        &status
                );
                ERRCHK ( status );

                /* Unregister interface from EPV */
                printf("Unregistering server endpoints with endpoint mapper (RPCD)...\n");
                rpc_ep_unregister (
                        UseCDS_v1_1_s_ifspec,
                        bind_vector_p,
                        NULL,
                        &status
                );
                ERRCHK ( status );

# ifdef IBMOS2
                pthread_dinst_exception_handler();
# endif
                exit ( 0 );
        }
        ENDTRY;
}

/***************************************************************************/
/*                                                                         */
/* Function:    whoareyou()                                                */
/*                                                                         */
/* Description: Returns a pointer to a string containing the               */
/*              DCE hostname of the host running this RPC server.          */
/*                                                                         */
/*              This routine is exported by the server to RPC clients.     */
/*                                                                         */
/***************************************************************************/
string_t *whoareyou ( handle_t bh )
{
        return &hoststr;
}
