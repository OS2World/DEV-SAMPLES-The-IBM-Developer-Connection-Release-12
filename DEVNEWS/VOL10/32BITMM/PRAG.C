/* prag.c

Shows that data_seg() pragmas work in EXE's, too. Prove it by
running two copies each in their own session. OS/2 deallocates
the shared memory when all copies terminate.

Enhancement: needs a mutex sem to manage access to shared data.

*/

#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <string.h>

// place a string into a shared segment; See prag.def.
#pragma data_seg( PIECE_1 )
char szShared[ 256 ];

//---------------------------------------------------------------
int main()
{
  APIRET     rc;
  CHAR       szJunk[ 16 ];

  if( strlen( szShared ))
  {
    printf( "Current string in shared memory is:\n%s\n\n", szShared );
  }
  else
  {
    printf( "There is a nul string in shared memory.\n\n" );
  }

  printf( "Enter a new string: " );
  fflush( NULL );
  gets( szShared );

  printf( "Press enter to exit this program.\n" );
  gets( szJunk );
  return 0;
}
