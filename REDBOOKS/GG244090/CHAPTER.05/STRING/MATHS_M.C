/******************************************************************************/
/* Module  : maths_m.c                                                        */
/* Purpose : Server manager code. Implements the add procedure that will be   */
/*           called from the client maths_c.                                  */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "maths.h"

/******************************************************************************/
/* Procedure  : add                                                           */
/* Purpose    : Add numv values passed in array value_a placing the sum in    */
/*              total. Uses implicit binding.                                 */
/******************************************************************************/

void add (
   value_array_t value_a,
   long numv,
   long *total )
{
   int i;

   /* Display the parameters that have been passed.                           */
   printf ( "Add has been called with numv = %d\n", numv );
   for ( i = 0; i < numv; i++ )
      printf ( "Value_a[ %d ] = %d\n", i, value_a[ i ] );

   /* Zero the accumulator.                                                   */
   *total = 0;

   /* Add the values in the array of size numv placing the result in total.   */
   for ( i = 0; i < MAX_VALUES && i < numv; i++ )
      *total += value_a[ i ];
}
