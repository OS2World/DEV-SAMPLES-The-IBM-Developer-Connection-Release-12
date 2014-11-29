// kill.c

/*

This is a utility to kill a process. The program expects the
process ID, in hex, on the command line.

The PSTAT.EXE program provides a list of process IDs in hex.
Use PSTAT.EXE or PIDS.CMD to list processes and their IDs.

Enter kill with no parameters for a little help screen.

*/


#define INCL_DOS
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//----------------------------------------------------------------------
int main( int argc, char *argv[] )
{
  APIRET     rc;
  BOOL       fConfirm = TRUE;
  LONG       digit;
  LONG       i;
  LONG       n;
  LONG       pid;
  LONG       place;
  PCHAR      p;
  PCHAR      pch;
  PCHAR      psz;
  ULONG      cb;
  static PSZ pszLegals = "0123456789ABCDEF";
  CHAR       ach[ 4 ];


  if( argc <  2 )
  {
    printf( "Usage:  kill <process ID in hex> [y|Y|/y|/Y|-y|-Y|yes|Yes]\n" );
    return 2;
  }


  if( argc > 2 )
  {
    strupr( argv[ 2 ] );
    if( strchr( argv[ 2 ], 'Y' ))
    {
      // don't prompt
      fConfirm = FALSE;
    }
  }



  // convert hex to decimal
  psz = strdup( argv[ 1 ] );
  strupr( psz );
  pch = psz + strlen( psz ) - 1;

  pid   = 0;
  place = 0;

  while( pch >= psz )
  {
    p = strchr( pszLegals, *pch );
    if( !p )
    {
      printf( "Invalid parameter %s\n", argv[ 1 ] );
      return 2;
    }
    i = p - pszLegals;

    n = place;
    while( n-- )
    {
      i *= 16;
    }

    pid += i;
    pch--;
    place++;
  }


  if( fConfirm )
  {
    printf( "Press enter to kill process %s (%d decimal). Press Ctrl-C to cancel.\n", argv[ 1 ], pid );
    DosRead( 0, ach, 1, &cb );

    if( ach[ 0 ] != '\r' )
    {
      return 2;
    }
  }


  rc = DosKillProcess( 1, pid );

  if( rc )
  {
    printf( "unexpected kill rc %d (dec)\n", rc );
    rc = 1;
  }

  return rc;
}

