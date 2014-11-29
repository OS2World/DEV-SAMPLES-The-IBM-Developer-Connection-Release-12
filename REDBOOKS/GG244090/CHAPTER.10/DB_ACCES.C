/*****************************************************************************/
/* Module: database_access.c                                                 */
/*                                                                           */
/* Description:                                                              */
/*    This module provides an online interface to retrieve the data in       */
/*    the remote database managed by database_server.exe.                    */
/*                                                                           */
/*****************************************************************************/
#ifdef _WINDOWS
#include <dce/dcewin.h>
#endif
#include "db.h"
#include "errorchk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  MAX_CLINE_LEN  256
enum  { _END, _READ, _MOVE, _HELP };

void database_access(db_context_t context_h)
{
   char cline[ MAX_CLINE_LEN ];
   int  cnt, len;
   long pos, new_pos;
   char cmd [ 8 ], arg1[ 8 ], last[ 8 ];
   db_data_t db_data;
   unsigned32 st;

   db_getpos( context_h, &pos, &st );
   do {
      printf( "(%ld)# ", pos ); fflush( stdout );
      if ( gets( cline ) == NULL ) break;
      cnt = sscanf( cline, "%s %s %s", cmd, arg1, last );

      switch( cmd[ 0 ] ) {
      case 'e': case 'E':
         cmd[ 0 ] = _END;
         break;
      case 'r': case 'R':
         switch( cnt ) {
         case 1:
            db_read( context_h, &db_data, &new_pos, &st );
            printf( "%4ld: %s\n", pos, db_data.data );
            pos = new_pos;
            break;
         case 2:
            len = atoi( arg1 );
            for ( cnt = 0; cnt < len; cnt++) {
               db_read( context_h, &db_data, &new_pos, &st );
               printf( "%4ld: %s\n", pos, db_data.data );
               if ( pos == new_pos ) break;
               else                  pos = new_pos;
            }
            break;
         default:
            printf("   Usage: r(ead) [len]\n" );
            break;
         }
         cmd[ 0 ] = _HELP;
         break;
      case 'm': case 'M':
         switch( cnt ) {
         case 1:
            printf( "   Current position: %ld\n", pos );
            break;
         case 2:
            new_pos = atol( arg1 );
            db_setpos( context_h, &new_pos, &st );
            db_getpos( context_h, &pos, &st );
            break;
         default:
            printf( "   Usage: m(ove) [pos]\n" );
            break;
         }
         cmd[ 0 ] = _HELP;
         break;
      case '?': case 'h': case 'H':
      default:
         cmd[ 0 ] = _HELP;
         printf( "Usage:\n" );
         printf( "   m(ove) [pos]\n" );
         printf( "   r(ead) [len]\n" );
         printf( "   e(nd)\n" );
         printf( "   h(elp) | ?\n" );
         break;
      }
   } while( cmd[ 0 ] != _END );
}

