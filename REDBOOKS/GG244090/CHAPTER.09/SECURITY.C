/***************************************************************************/
/*                                                                         */
/* Module:      security.c                                                 */
/*                                                                         */
/* Description: Implements two access checking routines for the            */
/*              MessageBox example.                                        */
/*              get_principal() is name based and returs the principal     */
/*              name of the RPC caller                                     */
/*              is_authorized() uses DCE ACL on server name space entry    */
/*              for authorization checking.                                */
/*                                                                         */
/***************************************************************************/

# include <string.h>

# include <dce/rpc.h>

# ifdef _WINDOWS
# include <dce/daclf.h>
# undef READ
# undef WRITE
# else
# include <dce/daclif.h>
# endif

# include "common.h"

/* ACL access checking values */
# define READ           0x00000001
# define WRITE          0x00000002
# define EXECUTE        0x00000004
# define CONTROL        0x00000008
# define INSERT         0x00000010
# define DELETE         0x00000020
# define TEST           0x00000040

# ifdef _WINDOWS
# define strdup  _fstrdup
# endif

/***************************************************************************/
/*                                                                         */
/* Name:        function get_principal()                                   */
/*                                                                         */
/* Description: Returns a pointer to a string with principal name          */
/*              associated with clients binding handle.                    */
/*              Client must set up name based authorization to send his    */
/*              principal name.                                            */
/*                                                                         */
/***************************************************************************/
char *get_principal ( handle_t bh )
{
        rpc_authz_handle_t      Credentials;
        unsigned32              authz_svc,
                                status;
        char                    *pname;

        /* get clients auth info, client should send principal name */
        rpc_binding_inq_auth_client(
                bh,                     /* binding handle               */
                &Credentials,           /* returned privileges          */
                NULL,                   /* we provide no principal name */
                NULL,                   /* no protection level returned */
                NULL,                   /* no authn_svc returned        */
                &authz_svc,             /* Credential contens indicator */
                &status
        );
        ERRCHK( status );

        /* check the contens of credentials */
        if ( authz_svc != rpc_c_authz_name )
                return NULL;

        /* cast type to string */
        pname = strdup((char *)Credentials);

        /* strip off leading cell name */
        return (strrchr(pname,'/') + 1);
}

/***************************************************************************/
/*                                                                         */
/* Name:        function is_authorized()                                   */
/*                                                                         */
/* Description: Returns true ( != 0 ) if the principal associated with     */
/*              clients binding handle has control rights granted by the   */
/*              ACL on server name space entry.                            */
/*              Client must set up DCE authorization to send his PAC.      */
/*                                                                         */
/***************************************************************************/
int is_authorized ( handle_t bh )
{
        rpc_authz_handle_t      Credentials;
        unsigned32              authz_svc,
                                status;
        sec_acl_handle_t        acl;
        boolean32               accessOK;
        unsigned32              num_rtnd, num_avail;
        uuid_t                  mgrs[10];

        /* get clients auth info, client should send PAC */
        rpc_binding_inq_auth_client(
                bh,                     /* binding handle               */
                &Credentials,           /* returned privileges          */
                NULL,                   /* we provide no principal name */
                NULL,                   /* no protection level returned */
                NULL,                   /* no authn_svc returned        */
                &authz_svc,             /* Credential contens indicator */
                &status
        );
        ERRCHK( status )

        /* check the contens of credentials */
        if ( authz_svc != rpc_c_authz_dce )
                return 0;

        /* get ACL handle */
        sec_acl_bind(
                ENTRY_NAME,             /* entry name for acl checking  */
                1,                      /* get handle to entry in namespace */
                &acl,                   /* handle to acl                */
                &status
        );
        ERRCHK( status );

        /* get ACL manager */
        sec_acl_get_manager_types(
                acl,                    /* handle to the acl            */
                sec_acl_type_object,    /* acl pts to a CDS object      */
                NUM_ELEMS( mgrs ),      /* number of items in array     */
                &num_rtnd,              /* number of items returned     */
                &num_avail,             /* number of items existing     */
                mgrs,                   /* array base                   */
                &status
        );
        ERRCHK( status );

        /* deny access if no ACL manager available */
        if ( num_rtnd == 0 )
                return 0;

        /* proof ACL granting control right */
        accessOK = sec_acl_test_access_on_behalf(
                acl,                    /* handle to acl                */
                &mgrs[0],               /* ACL mgr for object           */
                Credentials,            /* this is PAC from client      */
                CONTROL,                /* permissions to check for     */
                &status
        );
        ERRCHK( status );

        return accessOK;
}
