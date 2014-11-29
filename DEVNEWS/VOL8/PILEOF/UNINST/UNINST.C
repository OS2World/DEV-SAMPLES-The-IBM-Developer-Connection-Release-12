
// UNINST.C -- program to un-install

#define          INCL_WIN
#include <os2.h>
#include <stdio.h>

int main ( int argc, char *argv[] )
{
   HAB       hab;                 // used as an anchor block
   BOOL    fSuccess;             // return TRUE/FALSE from API

   hab = WinInitialize ( 0 );

   fSuccess = WinDeregisterObjectClass ( "PileOf" );

   if ( fSuccess == FALSE )
   {
     printf ( "Unable to de-register object class PileOf, error: " );
     printf ( "%d\n", ERRORIDERROR ( WinGetLastError ( hab ) ) );
   }

   WinTerminate ( hab );

   return(0);
}

