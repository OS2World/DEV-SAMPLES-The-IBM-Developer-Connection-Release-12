/* SPARSE.C.  This program allocates a one MB memory object but commits no
pages.  The program then writes to that memory which is invalid, and this
causes a trap.  The handler commits the invalid page and resumes execution.
Compile and link this program with:  icc /Ss sparse.c */

// os2 includes
#define INCL_DOS
#define INCL_ERRORS
#include <os2.h>

// c includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// exception handler registration record
typedef struct _regrec {
  PVOID pNext;
  PFN   pfnHandler;
} REGREC;
typedef REGREC *PREGREC;

// ----------------------------------------------------------------------
ULONG _System Handler( PEXCEPTIONREPORTRECORD p1,
                       PREGREC p2,
                       PCONTEXTRECORD p3,
                       PVOID pv )
{
  // interested in access violation
  if( p1->ExceptionNum == XCPT_ACCESS_VIOLATION  ) {
    assert( p1->ExceptionInfo[0] == XCPT_WRITE_ACCESS );
    // try to commit the referenced page
    if( 0 == DosSetMem( (PVOID)p1->ExceptionInfo[1], 1, PAG_COMMIT|PAG_WRITE )) {
      // successful commit; resume execution
      return XCPT_CONTINUE_EXECUTION;
    }
  }
  // not handled, let other handlers in the chain have the exception
  return XCPT_CONTINUE_SEARCH;
}

// ----------------------------------------------------------------------
int main ( void )
{
  APIRET      rc;
  PCHAR       pchar;
  PSZ         psz;
  PVOID       pvBase;
  REGREC      regrec;

  // insert exception handler into the chain of handlers for this thread
  regrec.pfnHandler = (PFN)Handler;
  rc = DosSetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD) &regrec );
  assert( rc == 0 );

  // allocate a memory object without committing any of it;
  // note lack of PAG_COMMIT flag
  rc = DosAllocMem(  &pvBase, 1048576, PAG_WRITE );
  assert( rc == 0 );

  // this causes an exception since the page is not committed
  pchar = (PCHAR)pvBase;
  *pchar = 'a';

  // this string copy causes two more exceptions
  psz = (PSZ)pvBase + (4096 + 4092);
  strcpy( psz, "This string crosses a 4K page boundary." );

  // reference the memory
  printf( "%c\n", *pchar );
  printf( "%s\n", psz );

  // free memory object
  rc = DosFreeMem( pvBase );
  assert( rc == 0 );

  // unlink handler before returning
  rc = DosUnsetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD) &regrec );
  assert( rc == 0 );

  return 0;
}
