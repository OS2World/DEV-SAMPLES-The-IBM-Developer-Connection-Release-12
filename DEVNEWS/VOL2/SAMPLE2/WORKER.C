/* WORKER.C.  This program shows how a worker function can use an
exception handler like a safety net for calling threads.
Compile and link this program with:  icc /ss worker.c */

// os2 includes
#define INCL_DOS
#define INCL_ERRORS
#include <os2.h>

// c includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <assert.h>

// user-extended exception registration record
typedef struct _regrec {
  PVOID     pNext;
  PFN       pfnHandler;
  jmp_buf   jmpWorker;
} REGREC;
typedef REGREC *PREGREC;

// ----------------------------------------------------------------------
ULONG _System Handler( PEXCEPTIONREPORTRECORD p1,
                       PREGREC p2,
                       PCONTEXTRECORD p3,
                       PVOID pv )
{
  switch( p1->ExceptionNum ) {
  case XCPT_ACCESS_VIOLATION:
  case XCPT_INTEGER_DIVIDE_BY_ZERO:
  case XCPT_INTEGER_OVERFLOW:
  case XCPT_PROCESS_TERMINATE:        // killed thread case
  case XCPT_ASYNC_PROCESS_TERMINATE:  // killed thread case
    // interested in this one
    longjmp( p2->jmpWorker, p1->ExceptionNum );
  default:
    break;
  }
  // not handled
  return XCPT_CONTINUE_SEARCH;
}


// ----------------------------------------------------------------------
// returns TRUE for success, FALSE for failure
LONG _System WorkerFunction( PCHAR pch )
{
  LONG        rc;
  LONG        rcResult;
  ULONG       ulException;
  REGREC      regrec;

  // set a handler
  regrec.pfnHandler = (PFN)Handler;
  rc = DosSetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD) &regrec );
  assert( rc == 0 );

  // store a known thread state
  ulException = setjmp( regrec.jmpWorker );

  if( ulException ) {

    // cleanup here: free memory allocations, release mutex sems, etc.

    // get the handler off the chain
    rc = DosUnsetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD) &regrec );
    assert( rc == 0 );

    // check for the killed-thread case
    switch( ulException ) {
    case XCPT_PROCESS_TERMINATE:
    case XCPT_ASYNC_PROCESS_TERMINATE:
      // clean up done above and thread really wants to die
      DosExit( EXIT_THREAD, 0 );
      break;
    }
    // set a failing result code
    rcResult = FALSE;
    goto depart;
  }

  // dereference the supplied pointer
  *pch = 'a';

  rc = DosUnsetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD) &regrec );
  assert( rc == 0 );

  rcResult = TRUE;

depart:
  return rcResult;
}


// ----------------------------------------------------------------------
int main ( void )
{
  CHAR     szWork[ 16 ];
  LONG     rc;

  // try worker function with a good pointer
  rc = WorkerFunction( szWork );
  printf( "Good pointer returns %d\n", rc );

  // try worker function with a bad pointer
  rc = WorkerFunction( NULL );
  printf( "Bad pointer returns %d\n", rc );

  return 0;
}
