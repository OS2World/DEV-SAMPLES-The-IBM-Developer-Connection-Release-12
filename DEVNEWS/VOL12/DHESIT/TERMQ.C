// termq.c

/*

This is a sample program that shows the use of the TermQ option
in the STARTDATA structure for DosStartSession(). The child session
must be related.

When reading queue elements, be sure to free the memory pointer
or else risk a memory leak for the process.

DOS programs started this way report their errorlevel in the
result buffer provided you "call" the DOS program from command.com
using its /c option.

*/


#define INCL_DOS
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>


#define assert(exp) { if(!exp) printf( "assertion %s failed on line %d\n", #exp, __LINE__ ); }

struct _resultbuf
{
  SHORT   sessionid;
  SHORT   errorlevel;
};
typedef struct _resultbuf RESULTBUF, *PRESULTBUF;


//-------------------------------------------------------------------------------------------
int main()
{
  APIRET            rc;
  BYTE              bPriority;
  CHAR              szQueueName[ 64 ];
  HQUEUE            hQueue;
  LONG              lResult;
  PID               pid;
  PPIB              ppib;
  PRESULTBUF        presultbuf;
  PTIB              ptib;
  PVOID             pvData;
  REQUESTDATA       requestdata;
  STARTDATA         startdata;
  ULONG             sid;
  ULONG             ulLength;


  rc = DosGetInfoBlocks( &ptib, &ppib );
  assert( 0 == rc );

  // construct a queue name that includes PID
  sprintf( szQueueName, "\\QUEUES\\PID%X\\TERMQ", ppib->pib_ulpid );
  rc = DosCreateQueue( &hQueue, QUE_FIFO | QUE_CONVERT_ADDRESS, szQueueName );
  assert( 0 == rc );

  // start a related session in the foreground; command processor is default program
  memset( &startdata, 0, sizeof( startdata ));
  startdata.Length   = sizeof( startdata );  // or 32 for tshell
  startdata.TermQ    = szQueueName;
  startdata.Related  = 1;
  // startdata.SessionType = SSF_TYPE_VDM;   // or DOS session

  // 457 means started in background
  rc = DosStartSession( &startdata, &sid, &pid );
  assert( 0 == rc | rc == 457 );

  // block in read until child session terminates
  memset( &requestdata, 0, sizeof( requestdata ));
  pvData     = NULL;
  ulLength   = 0;
  rc = DosReadQueue( hQueue, &requestdata, &ulLength, &pvData, 0, FALSE, &bPriority, (HEV)0 );
  assert( 0 == rc );
  assert( pvData );
  assert( sizeof( RESULTBUF ) == ulLength );

  // data is a RESULTBUF; see the struct defined above
  presultbuf = (PRESULTBUF) pvData;

  // check result (errorlevel)
  lResult = (LONG) presultbuf->errorlevel;

  // check the session id
  assert( presultbuf->sessionid == sid );

  // free the memory allocated by the session manager
  rc = DosFreeMem( pvData );
  assert( 0 == rc );

  rc = DosCloseQueue( hQueue );
  assert( 0 == rc );

  return lResult;
}

