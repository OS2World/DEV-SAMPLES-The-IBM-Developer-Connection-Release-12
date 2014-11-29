// test.c: program to load tester.dll

#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <assert.h>

//---------------------------------------------------------------
int main()
{
  APIRET     rc;
  CHAR       szJunk[ 16 ];
  HMODULE    hmod = 0;

  // load TESTER.DLL
  rc = DosLoadModule( szJunk, sizeof( szJunk ), "TESTER", &hmod );
  assert( 0 == rc );
  assert( hmod );

  printf( "TESTER.DLL loaded OK\nPress enter to unload it.\n" );
  gets( szJunk );

  rc = DosFreeModule( hmod );
  assert( 0 == rc );
  return 0;
}

