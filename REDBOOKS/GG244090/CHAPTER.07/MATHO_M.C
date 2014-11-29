/******************************************************************************/
/* Module  : matho_m.c                                                        */
/* Purpose : Server manager code. Implements the add procedures that will be  */
/*           called from the client matho_c.                                  */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dce/rpc.h>
#include "errchk.h"
#include "matho.h"

#define OBJ_UUID_STR_LEN 36

extern char **argv_global;

/******************************************************************************/
/* Procedure  : add_int                                                       */
/* Purpose    : Add numv integer values passed in number string array value_a */
/*              placing the sum in total. Uses object UUID and explicit       */
/*              binding.                                                      */
/******************************************************************************/

void add_int (
   handle_t bh,
   value_s_t value_a[],
   long numv,
   value_s_t total )
{
   char **argv;
   unsigned32 status;
   char *str_bind_p, obj_uuid_str[ OBJ_UUID_STR_LEN + 1 ];
   int i, aux;

   /* Get argv from a global variable to be used in the errchk macro.         */
   argv = argv_global;

   /* Print the object UUID coming from the client.                           */
   rpc_binding_to_string_binding ( bh, &str_bind_p, &status );
   ERRCHK ( status );
   strncpy ( obj_uuid_str, str_bind_p, OBJ_UUID_STR_LEN );
   obj_uuid_str[ OBJ_UUID_STR_LEN ] = '\0';
   printf( "Incoming object UUID : %s\n", obj_uuid_str );
   rpc_string_free ( &str_bind_p, &status );
   ERRCHK ( status );

   /* Display the parameters that have been passed.                           */
   printf ( "Add_int has been called with numv = %d\n", numv );
   for ( i = 0; i < numv; i++ )
      printf ( "Value_a[ %d ] = %s\n", i, value_a[ i ] );

   /* Zero the accumulator.                                                   */
   aux = 0;

   /* Add the values in the array of size numv accumulating the results.      */
   for ( i = 0; i < MAX_VALUES && i < numv; i++ )
      aux += atoi ( value_a[ i ] );

   /* Convert the result into a string placing it in an output variable.      */
   sprintf ( total, "%d", aux );
}

/* Declare and initialize the integer manager EPV table.                      */
matho_v1_0_epv_t epv_int = { add_int };

/******************************************************************************/
/* Procedure  : add_float                                                     */
/* Purpose    : Add numv float values passed in number string array value_a   */
/*              placing the sum in total. Uses object UUID and explicit       */
/*              binding.                                                      */
/******************************************************************************/

void add_float (
   handle_t bh,
   value_s_t value_a[],
   long numv,
   value_s_t total )
{
   char **argv;
   char *pstring;
   unsigned32 status;
   char *str_bind_p, obj_uuid_str[ 37 ];
   int i;
   float aux;

   /* Get argv from a global variable to be used in the errchk macro.         */
   argv = argv_global;

   /* Print the object UUID coming from the client.                           */
   rpc_binding_to_string_binding ( bh, &str_bind_p, &status );
   ERRCHK ( status );
   strncpy ( obj_uuid_str, str_bind_p, OBJ_UUID_STR_LEN );
   obj_uuid_str[ OBJ_UUID_STR_LEN ] = '\0';
   printf( "Incoming object UUID : %s\n", obj_uuid_str );
   rpc_string_free ( &str_bind_p, &status );
   ERRCHK ( status );

   /* Display the parameters that have been passed.                           */
   printf ( "Add_float has been called with numv = %d\n", numv );
   for ( i = 0; i < numv; i++ )
      printf ( "Value_a[ %d ] = %s\n", i, value_a[ i ] );

   /* Zero the accumulator.                                                   */
   aux = 0.0;

   /* Add the values in the array of size numv accumulating the results.      */
   for ( i = 0; i < MAX_VALUES && i < numv; i++ )
      aux += atof ( value_a[ i ] );

   /* Determine the precision desired for the total based on the object UUID. */
   pstring = obj_uuid_str;
   while ( (*pstring = toupper(*pstring) ) )
       pstring++;
   if ( strcmp ( obj_uuid_str, OBJ_UUID_FLOAT ) == 0 )
      sprintf ( total, "%8.3f", aux );
   else
      sprintf ( total, "%11.6f", aux );
}

/* Declare and initialize the float manager EPV table.                        */
matho_v1_0_epv_t epv_float = { add_float };
