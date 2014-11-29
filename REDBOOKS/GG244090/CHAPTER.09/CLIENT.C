/***************************************************************************/
/*                                                                         */
/* Module:      client.c                                                   */
/*                                                                         */
/* Description: Implements the client for the MessageBox application.      */
/*                                                                         */
/***************************************************************************/

# ifdef _WINDOWS
# include <dce/dcewin.h>
# endif

# include <string.h>

# include "mbox.h"
# include "common.h"

# define NO_ERROR        10
# define NO_OPTION      -11
# define NO_PRINCIPAL   -12

# ifdef _WINDOWS
# define strdup  _fstrdup
# endif

# ifdef IBMOS2
# define USAGE          "usage: %s [/c] [principal] [message ...]\n"
# define OPTCHAR        '/'
# else
# define USAGE          "usage: %s [-c] [principal] [message ...]\n"
# define OPTCHAR        '-'
# endif

# define IF_HANDLE      MessageBox_v2_0_c_ifspec

void error( int );

char *prgname   = NULL;
char *principal = NULL;

int main (int argc, char *argv[])
{
        char            msg[MAX_CHAR]   = "";
        int             action          = MBOX_READ;
        int             rc;
        handle_t        bh;
        rpc_ns_handle_t ns_handle;
        error_status_t  status;

        /* get application name */
        prgname = strdup(*argv++);

        /* parse the commandline for options */
        while ( --argc ) {
                if ( **argv != OPTCHAR )
                        break;

                while ( *++(*argv) ) {
                        switch ( **argv ) {
                        case 'c':
                                action = MBOX_CREATE;
                                break;
                        default:
                                error(NO_OPTION);
                        }
                }
                argv++;

        }

        /* if there are args, assume the first none option is the principal */
        if ( argc ) {
                principal = *argv++;
                if ( action == MBOX_READ )
                        action    = MBOX_APPEND;

                /* concat the rest of the command line to a message */
                while ( --argc ) {
                        strcat(msg,*argv++);
                        strcat(msg," ");
                }
        }

        /* check for the principal argument */
        if ( ( action == MBOX_CREATE ) && ( principal == "" ) )
                error( NO_PRINCIPAL );

        /* set up for reading bindin information */
        rpc_ns_binding_import_begin(
                rpc_c_ns_syntax_default,
                ENTRY_NAME,
                IF_HANDLE,
                NULL,
                &ns_handle,
                &status
        );
        ERRCHK( status );

        /* import first binding information from name space */
        rpc_ns_binding_import_next(
                ns_handle,
                &bh,
                &status
        );
        ERRCHK( status );

        /* stop reading binding indormation */
        rpc_ns_binding_import_done(
                &ns_handle,
                &status
        );
        ERRCHK( status );

        /* determine which authorization info to send based on action */
        if ( action == MBOX_CREATE )
                rpc_binding_set_auth_info(
                        bh,
                        PRINCIPAL_NAME,
                        rpc_c_protect_level_pkt_integ, /* send checksum       */
                        rpc_c_authn_default,
                        NULL,                          /* use login context   */
                        rpc_c_authz_dce,               /* send PAC            */
                        &status
                );
        else
                rpc_binding_set_auth_info(
                        bh,
                        PRINCIPAL_NAME,
                        rpc_c_protect_level_pkt_integ, /* send checksum       */
                        rpc_c_authn_default,
                        NULL,                          /* use login context   */
                        rpc_c_authz_name,              /* send name, not PAC  */
                        &status
                );
        ERRCHK( status );

        /* call the remote procedures for action */
        switch ( action ) {
        case MBOX_CREATE:
                if ( (rc = mbox_new(bh,principal)) != MBOX_OK )
                        error( rc );

                break;

        case MBOX_APPEND:
                if ( (rc = mbox_append(bh,principal,msg)) != MBOX_OK )
                        error( rc );
                break;

        case MBOX_READ:
                switch ( rc = mbox_next(bh,msg) ) {
                case MBOX_OK:
                        printf("%s\n",msg);
                        break;

                case MBOX_NO_MSG:
                        printf("%s: no more messages\n",prgname);
                        break;
                default:
                        error( rc );
                }
                break;

        }

        exit ( NO_ERROR );
}

/***************************************************************************/
/*                                                                         */
/* Name:        function error()                                           */
/*                                                                         */
/* Description: error() implements the error printing routine.             */
/*              All error output is send to stderr. After the error text   */
/*              is printed error() exist the application with a error      */
/*              return value.                                              */
/*                                                                         */
/***************************************************************************/
void error (int msg)
{
        switch ( msg ) {
        case MBOX_PRINCIPAL_EXIST:
                fprintf(stderr, "%s: mbox for principal %s exists already\n",
                        prgname,principal);
                break;

        case MBOX_PRINCIPAL_NOT_EXIST:
                fprintf(stderr, "%s: principal %s has no message box\n",
                        prgname,principal);
                break;

        case MBOX_NOT_AUTHORIZED:
                fprintf(stderr,"%s: you are not authorized\n",
                        prgname);
                break;

        case MBOX_ALL_USED:
                fprintf(stderr,"%s: no more message boxes available\n",
                        prgname);
                break;

        case NO_PRINCIPAL:
                fprintf(stderr,"%s: missing principal name\n",prgname);
                break;

        case NO_OPTION:
                fprintf(stderr,"%s: unknown option\n",prgname);
                break;

        default:
                fprintf(stderr,"%s: something is wrong\n",prgname);
        }
        fprintf(stderr, USAGE,prgname);

        exit( msg );
}

