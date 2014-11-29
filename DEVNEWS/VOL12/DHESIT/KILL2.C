// kill2.c

/*

This test program shows how to handle DosKillThread() and
DosKillProcess() and Ctrl-C and terminate cleanly.

Killing a thread is a random act, so a thread that expects to be
killed should be prepared by setting exception handlers where
needed.

Set an exception handler in any function where you allocate
memory that must be freed before the function returns. Also, set
a handler for any function that uses semaphores. If killed, be
sure to release or post semaphores in your clean up section.

VERY IMPORTANT: Remove the exception handler before the function
returns. Watch out for return statements scattered in the middle
of a function. It is an error to set a handler and not remove it.

*/




#define INCL_DOS
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#define assert(exp) { if(!exp) printf( "assertion %s failed on line %d\n", #exp, __LINE__ ); }


//-----------------------------------------------------------------------
// typedef a "super" registration record

struct _regrec
{
  PVOID                 pnext;       // same as OS/2's
  PFN                   pfn;         // same as OS/2's
  EXCEPTIONREPORTRECORD report;      // a copy of the exception report
  jmp_buf               jumpBuffer;  // for C setjmp()/longjmp()
};
typedef struct _regrec REGREC, *PREGREC;



// ----------------------------------------------------------------------
// exception handler
// it doesn't really handle anything,
// it just jumps to the function that had the problem

ULONG APIENTRY Handler( PEXCEPTIONREPORTRECORD p1, PREGREC p2, PCONTEXTRECORD p3, PVOID pv )
{
  switch( p1->ExceptionNum ) {
  case XCPT_PROCESS_TERMINATE:
  case XCPT_ASYNC_PROCESS_TERMINATE:
    // make a copy of the exception report record
    p2->report = *p1;
    // jump to setjmp(); TRUE means an exception happened
    longjmp( p2->jumpBuffer, TRUE );
  }

  return XCPT_CONTINUE_SEARCH;
}





//----------------------------------------------------------------------
VOID APIENTRY Subroutine( int c )
{
  APIRET           rc;
  BOOL             fException;
  REGREC           regrec;
  ULONG            ul;


  // defer asynchronous exceptions while setting handler and jump buffer
  DosEnterMustComplete( &ul );

  regrec.pfn = (PFN)Handler;
  rc = DosSetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD) &regrec );
  assert( 0 == rc );

  fException = setjmp( regrec.jumpBuffer );

  DosExitMustComplete( &ul );

  if( fException )
  {
    printf( "Subroutine %d received exception %X.\n", c, regrec.report.ExceptionNum );
    goto depart;
  }



  // body of subroutine here
  if( c < 5 )
  {
    // recurse deeper
    Subroutine( c+1 );
  }
  else
  {
    // block until exception
    DosSleep( 30000 );
  }


depart:
  rc = DosUnsetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD)&regrec );
  assert( 0 == rc );

  // clean here up as needed

  if( fException )
  {
    // raise the same exception so caller sees it too
    DosRaiseException( &regrec.report );
  }
  return;
}



//----------------------------------------------------------------------
// thread 2 entry point

void _Optlink threadmain( void * pv  )
{
  APIRET           rc;
  BOOL             fException;
  REGREC           regrec;
  ULONG            ul;


  // defer asynchronous exceptions while setting handler and jump buffer
  DosEnterMustComplete( &ul );

  regrec.pfn = (PFN)Handler;
  rc = DosSetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD) &regrec );
  assert( rc == 0 );

  // record known point of execution
  fException = setjmp( regrec.jumpBuffer );

  DosExitMustComplete( &ul );

  if( fException )
  {
    printf( "threadmain received exception %X.\n", regrec.report.ExceptionNum );
    goto depart;
  }


  // body of function; do processing here
  Subroutine( 0 );


depart:
  rc = DosUnsetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD)&regrec );
  assert( 0 == rc );

  // clean up here as needed

  // return to the C runtime to exit the thread
  return;
}








//-------------------------------------------------------------------------------------------
int main()
{
  APIRET     rc;
  BOOL       fException;
  CHAR       ch;
  REGREC     regrec;
  TID        tid;
  ULONG      ul;

  tid = (TID) _beginthread( threadmain, NULL, 32000, NULL );
  assert( 2 == tid );

  // wait for enter key or a kill signal
  printf( "Thread 2 started. About to kill thread 2. Press enter or Ctrl-C.\n");

  DosRead( 0, &ch, 1, &ul );


  rc = DosKillThread( tid );
  assert( 0 == rc );

  // tidy thing: wait for threads before returning to C runtime
  rc = DosWaitThread( &tid, DCWW_WAIT );

  return 0;
}

