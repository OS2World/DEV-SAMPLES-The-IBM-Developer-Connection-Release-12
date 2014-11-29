// INSTALL.C -- installation program for PileOf class

#define	INCL_WIN
#include <os2.h>
#include <stdio.h>

int main ( int argc, char *argv[] )
{
   HAB          hab;                // anchor block
   BOOL         fSuccess;           // return from API
   HOBJECT      hobjPileOf;         // PileOf object handle

   if (argc < 2)
   {
      printf ( "Specify Path and DLL ex. D:\\PILEOF\\PILEOF.DLL" );
      return (1);
   }
   hab = WinInitialize ( 0 );

   // register the PileOf class
   fSuccess = WinRegisterObjectClass ( "PileOf", argv[1] );
 //fSuccess = WinRegisterObjectClass ( "PileOf", "C:\\OS2\\DLL\\PILEOF.DLL" );

   if ( fSuccess == FALSE )
   {
        printf ( "Unable to register object class PileOf, error: " );
        printf ( "%x\n", ERRORIDERROR ( WinGetLastError ( hab ) ) );
   }
   else
   {

     // create an instance of the PileOf class
     hobjPileOf = WinCreateObject ( "PileOf"
                                    , "PileOf..."
                                    , "OBJECTID=<OBJECT_PILEOF>"
                                    , "<WP_DESKTOP>"
                                    , CO_REPLACEIFEXISTS );
     if ( hobjPileOf == 0 )
     {
        printf ( "Unable to create PileOf... object, error: " );
        printf ( "%x\n", ERRORIDERROR ( WinGetLastError ( hab ) ) );
     }
   }

   WinTerminate ( hab );

   return(0);
}

