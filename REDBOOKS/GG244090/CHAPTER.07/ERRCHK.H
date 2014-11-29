# ifndef _ERRCHK_H
# define _ERRCHK_H

# ifndef _H_STDIO
# include <stdio.h>
# endif

#ifdef _WINDOWS
# include <dce\nbase.h>
# include <dce\dceerror.h>
#endif

int  error_inq_st;
char dce_error_string[256];

# define ERRCHK(x)                                                 \
        if ((x) != error_status_ok ) {                             \
           printf( "%s: error in %s[%d]:", argv[0] ,               \
                            __FILE__ , __LINE__ - 1 );             \
           dce_error_inq_text(x, dce_error_string, &error_inq_st); \
           printf( " %s.\n", dce_error_string);                    \
           exit ( (int)x );                                        \
        }
# endif
