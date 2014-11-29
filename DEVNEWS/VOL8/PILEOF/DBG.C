
#define INCL_DOS
#define INCL_PM
#define INCL_WINWORKPLACE
#include <os2.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <string.h>
#include <som.h>



#pragma linkage ( myDebugFunction, system )
int SOMLINK  myDebugFunction ( char c );
int SOM_TraceLevel = 2;     // get maximum debug info
int SOM_WarnLevel  = 2;     // get maximum debug info

static FILE   *errorfile;

void initdebug ( char *p );

int  SOMLINK  myDebugFunction ( char c )
{

    fputc ( (int) c, errorfile );

    return 1;   // successful return code
}


void initdebug ( char  *p )
{

        SOMOutCharRoutine = myDebugFunction;
        DosBeep ( 1000, 100 );

        errorfile = fopen ( "err.txt", "a" );
		fprintf ( errorfile, "\nDebug initialized from %s\n", p );
		fprintf ( errorfile, "*************************\n" );
        setbuf ( errorfile, NULL );
}


