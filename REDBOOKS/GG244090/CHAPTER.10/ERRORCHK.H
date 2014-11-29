/*****************************************************************************/
/* Module: errorck.h                                                         */
/*                                                                           */
/* Description:                                                              */
/*    This header file defines ERRORCK macro.                                */
/*                                                                           */
/*****************************************************************************/
#ifdef _WINDOWS
#include <dce\rpc.h>
#include <dce\dceerror.h>
#else
#include <rpc.h>
#include <dce_error.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#define ERRORCK( proc, st ) {                                      \
   if ( st != error_status_ok ) {                                  \
      dce_error_inq_text( st, error_string, &errst );              \
      printf( "%s (%d) : %s\n", proc, st, error_string );          \
      exit( 1);                                                    \
   }                                                               \
}

static int   errst;
static char  error_string[ dce_c_error_string_len ];
