/******************************************************************************/
/* Module  : matho_c.c                                                        */
/* Purpose : Client module for server matho_s. This program determines if the */
/*           user wants to use integer, floating point math with 3 decimal    */
/*           places of precision or floating point math with 6 decimal places */
/*           of precision. Based on the choice, selects the appropriate       */
/*           server using explicit binding and object UUIDs. The type of math */
/*           and the values to be added come from the command line. The       */
/*           number string values are sent to the remote add procedure        */
/*           together with the number of values passed and a pointer to the   */
/*           variable that will contain the result.                           */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dce/rpc.h>
#ifdef _WINDOWS
#include <dce/dcewin.h>
#endif

#include "errchk.h"
#include "matho.h"

#define ENTRY_NAME "/.:/Servers/MathO"  /* Server entry name.                 */

int main ( int argc, char *argv[] )
{
   uuid_t obj_uuid;
   long int i, numv;
   value_s_t va[ MAX_VALUES ], sum;
   rpc_binding_handle_t bh;
   unsigned32 status;
   rpc_ns_handle_t imp_ctxt;
   char *str_bind_p, obj_uuid_str[ 37 ];

   /* Check if the user passed the minimum number of parameters.              */
   if ( argc < 4 ) {
      printf ( "Usage : %s <math_type : i, f or d> <values to be added>\n",
               argv[ 0 ] );
      exit ( 1 );
   }

   /* Check if the user passed the math flag.                                 */
   switch ( argv[ 1 ][ 0 ] ) {
      case 'i' :
         /* Create object UUID for integer math from string.                  */
         uuid_from_string ( OBJ_UUID_INT, &obj_uuid, &status );
         ERRCHK ( status );

         /* Print the object UUID that will be used.                          */
         printf ( "Object UUID that will be used : %s\n", OBJ_UUID_INT );
         break;

      case 'f' :
         /* Create object UUID for float math from string.                    */
         uuid_from_string ( OBJ_UUID_FLOAT, &obj_uuid, &status );
         ERRCHK ( status );

         /* Print the object UUID that will be used.                          */
         printf ( "Object UUID that will be used : %s\n", OBJ_UUID_FLOAT );
         break;

      case 'd' :
         /* Create object UUID for double float math from string.             */
         uuid_from_string ( OBJ_UUID_DOUBLE, &obj_uuid, &status );
         ERRCHK ( status );

         /* Print the object UUID that will be used.                          */
         printf ( "Object UUID that will be used : %s\n", OBJ_UUID_DOUBLE );
         break;

      default :
         printf ( "Usage : %s <math_type : i, f or d> <values to be added>\n",
                  argv[ 0 ] );
         exit ( 1 );
         break;
   }

   /* Set up the context to import the bindings.                              */
   rpc_ns_binding_import_begin ( rpc_c_ns_syntax_dce, ENTRY_NAME,
                                 matho_v1_0_c_ifspec, &obj_uuid, &imp_ctxt,
                                 &status );
   ERRCHK ( status );

   /* Get the first binding handle.                                           */
   rpc_ns_binding_import_next ( imp_ctxt, &bh, &status );
   ERRCHK ( status );

   /* Get values from the command line and load them in array va.             */
   for ( i = 0; i < MAX_VALUES && i < argc - 2; i++ ) {
      strncpy ( va[ i ], argv[ i + 2 ], MAX_PRECISION );
      va[ i ][ MAX_PRECISION ] = '\0';
   }
   numv = i;

   /* Call add passing values, number of values and pointer to result.        */
   add ( bh, va, ( long )numv, sum );
   printf ( "The sum is %s\n", sum );

   /* Release the context.                                                    */
   rpc_ns_binding_import_done ( &imp_ctxt, &status );
   ERRCHK ( status );
}
