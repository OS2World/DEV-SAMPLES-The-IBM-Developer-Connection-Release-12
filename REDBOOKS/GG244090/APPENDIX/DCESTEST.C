/****************************************************************************

    PROGRAM: DCETest.c

    PURPOSE: Windows DCE shell test

****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "dcewin.h"

main( int argc, char *argv[])
{
     int i;
     char buffer[80];

     printf( "\n    %d Parameter(s):\n", argc);

     for ( i = 0 ; i < argc ; ++i )
        printf( "\tP%d = %s\n", i, argv[i]);

     printf( "\nInput: ");
     gets( buffer);
     printf( "Echo:%s\n", buffer);
     fprintf( stderr, "frintf rc=%d msg=%s\n", 123, "One two ...");
}
