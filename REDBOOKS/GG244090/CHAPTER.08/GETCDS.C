/***************************************************************************/
/*                                                                         */
/* Module:      getcds.c                                                   */
/*                                                                         */
/* Description: This module implements the client part of the sample       */
/*              application.                                               */
/*              getcds receives from the commandline a CDS name entry      */
/*              and looks up the CDS with this name as starting point      */
/*              for matching server entries.                               */
/*              From the resulting vector of binding information the       */
/*              client selects one randomly and calls the whoareyou()      */
/*              remote procedure exported from that server.                */
/*                                                                         */
/***************************************************************************/

# include <string.h>

# include <dce/rpc.h>

# ifdef _WINDOWS
# include <dce/dcewin.h>
# endif

# include "usecds.h"
# include "errchk.h"

int main ( int argc, char *argv[] )
{
        handle_t                bh;     /* binding handle               */
        rpc_binding_vector_p_t *bhv;    /* pointer to binding vector    */
        rpc_ns_handle_t         nh;     /* name space handle            */
        error_status_t          status;

        /* exit if not the rigth number of arguments */
        if ( argc != 2 ) {
                printf ( "usage %s: <entry name>\n",argv[0] );
                exit ( 1 );
        }

        /* print out the given entry point */
        printf ( "%s: entry point = %s\n",argv[0], argv[1] );

        /* Set up for lookup binding information */
        rpc_ns_binding_lookup_begin (
                rpc_c_ns_syntax_default,
                argv[1],        /* name space entry to look for */
                UseCDS_v1_1_c_ifspec,
                NULL,
                2,              /* max length of binding vector */
                &nh,
                &status
        );
        ERRCHK ( status );

        /* Lookup a list of binding information from name space */
        rpc_ns_binding_lookup_next ( nh, &bhv, &status );
        ERRCHK ( status );

        /* Randomly select one server */
        rpc_ns_binding_select ( bhv, &bh, &status );
        ERRCHK ( status );

        /* Stop lookup binding indormation */
        rpc_ns_binding_lookup_done ( &nh, &status );
        ERRCHK ( status );

        /* Free binding vector */
        rpc_binding_vector_free ( &bhv, &status );
        ERRCHK ( status );

        /* Call remote procedure and print out string */
        printf ( "%s: server returned:\n\t%s\n", argv[0], whoareyou ( bh ) );

        exit ( 0 );
}
