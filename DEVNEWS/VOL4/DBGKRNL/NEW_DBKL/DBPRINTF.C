// dbprintf.c

// Worker routine behind the DBPRINTF macro. See DBPRINTF.H.

#include <os2.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "dbprintf.h"

void _Optlink dbprintf( PSZ pszFormat, ... )
{
  CHAR     szTemp[160], szOutput[160];
  PCHAR    pchSrc, pchDest;
  va_list  list;

  // use vsprintf to format the arguments
  va_start( list, pszFormat );
  vsprintf( szTemp, pszFormat, list );
  va_end( list );

  // insert a carriage return in front of every line feed
  pchSrc  = szTemp;
  pchDest = szOutput;
  while( *pchSrc ) {
    switch( *pchSrc ) {
    case '\n':
      *pchDest++ = '\r';
      *pchDest++ = '\n';
      break;
    case '\r':
      break;
    default:
      *pchDest++ = *pchSrc;
      break;
    }
    pchSrc++;
  }
  // null-end the string; send to debug terminal
  *pchDest = 0;
  DebugOutput( szOutput );
}
