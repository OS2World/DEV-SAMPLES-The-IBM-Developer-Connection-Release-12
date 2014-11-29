/***************************************************************************/
/*                                                                         */
/* Name:        header file common.h                                       */
/*                                                                         */
/* Description: Common header file for the Message Box example.            */
/*              Defines the error check pseudo function which is called    */
/*              to check the return status of call to the RPC runtime.     */
/*              Defines also the principal used by the server to           */
/*              authenticate, the name for the entry in the CDS name       */
/*              space and the return values of the remote procedures.      */
/*                                                                         */
/***************************************************************************/

# ifndef _MBOX_COMMON
# define _MBOX_COMMON

# ifndef _H_STDIO
# include <stdio.h>
# endif

# ifdef _WINDOWS
# include <dce\nbase.h>
# include <dce\dceerror.h>
# endif

# define ERRCHK(x)                                                     \
        if ( (x) != error_status_ok ) {                                \
           fprintf(stderr, "FAULT: %s: %d\n", __FILE__, __LINE__ - 1); \
           dce_error_inq_text(x, dce_error_string, &error_inq_st);     \
           fprintf(stderr, "cause: %s\n", dce_error_string);           \
           fflush(stderr);                                             \
           exit((int)x);                                               \
        }

int                                     error_inq_st;
char                                    dce_error_string[256];

# define NUM_ELEMS(a)                   (sizeof(a)/sizeof(a[0]))

# define ENTRY_NAME                     "/.:/Servers/MessageBox"
# define PRINCIPAL_NAME                 "MessageBox"

/* Return values of remote procedures */
# define MBOX_OK                         0
# define MBOX_NO_MSG                     1
# define MBOX_READ                       2
# define MBOX_APPEND                     3
# define MBOX_CREATE                     4
# define MBOX_NOT_AUTHORIZED            -1
# define MBOX_PRINCIPAL_EXIST           -2
# define MBOX_PRINCIPAL_NOT_EXIST       -3
# define MBOX_ALL_USED                  -4

# endif
