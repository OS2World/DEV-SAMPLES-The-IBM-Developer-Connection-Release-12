// t1term.c

/*

This test program shows that thread 1 in OS/2 program has special
behavior: when thread 1 exits, the other threads in the process
will exit, too.

When thread 1 exits, thread 1 gets XCPT_PROCESS_TERMINATE
(0xC0010001) and thread 2 gets XCPT_ASYNC_PROCESS_TERMINATE
(0xC0010002). See \toolkit\h\bsexcpt.h to look up these and other
exception numbers.

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

ULONG APIENTRY Handler( PEXCEPTIONREPORTRECORD p1, PREGREC p2, PCONTEXTRECORD p3, PVOID pv )
{
  switch( p1->ExceptionNum ) {
  case XCPT_PROCESS_TERMINATE:
  case XCPT_ASYNC_PROCESS_TERMINATE:
    // make a copy of the exception report
    p2->report = *p1;
    // jump to setjmp(); TRUE means an exception happened
    longjmp( p2->jumpBuffer, TRUE );
  }

  return XCPT_CONTINUE_SEARCH;
}


//----------------------------------------------------------------------
// thread 2 entry point

void _Optlink threadmain( void * pv )
{
  APIRET     rc;
  BOOL       fException;
  REGREC     regrec;
  ULONG      ul;


  // defer asynchronous exceptions while setting the handler and jump buffer
  DosEnterMustComplete( &ul );

  regrec.pfn = (PFN)Handler;
  DosSetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD) &regrec );

  // store a known point of execution
  fException = setjmp( regrec.jumpBuffer );

  DosExitMustComplete( &ul );

  if( fException )
  {
    printf( "Thread 2 received exception %X.\n", regrec.report.ExceptionNum );
    goto depart;
  }


  // block here until the exception
  DosSleep( 0xffff0000 );



depart:

  rc = DosUnsetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD)&regrec );
  assert( 0 == rc );

  // C runtime will exit this thread
  return;
}



//-------------------------------------------------------------------------------------------
int main()
{
  APIRET           rc;
  BOOL             fException;
  CHAR             ch;
  REGREC           regrec;
  TID              tid;
  ULONG            ul;


  // defer asynchronous exceptions until handler and jump buffer set
  DosEnterMustComplete( &ul );

  regrec.pfn = (PFN)Handler;
  DosSetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD) &regrec );

  // record known point of execution
  fException = setjmp( regrec.jumpBuffer );

  DosExitMustComplete( &ul );

  if( fException )
  {
    printf( "Thread 1 received exception %X.\n", regrec.report.ExceptionNum );
    goto depart;
  }


  // start thread 2
  tid = (TID) _beginthread( threadmain, NULL, 32000, NULL );
  assert( tid );


  // prompt user to hit enter
  printf( "Thread 2 started. About to exit thread 1...\nPress enter.\n" );


  // wait for enter key
  DosRead( 0, &ch, 1, &ul );


  // when thread 1 exits, other threads in the process terminate, too
  DosExit( EXIT_THREAD, 0 );


  // execution never gets here


depart:
  rc = DosUnsetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD)&regrec );
  assert( 0 == rc );


  // tidy thing: wait for threads before returning to C runtime
  rc = DosWaitThread( &tid, DCWW_WAIT );

  return 0;
}

