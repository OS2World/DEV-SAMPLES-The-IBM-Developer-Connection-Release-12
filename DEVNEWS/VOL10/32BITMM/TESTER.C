/* tester.c:  See tester.def and test.c.


This is source code for TESTER.DLL that prepares a named, shared,
mutex semaphore used to protect access to shared memory in
TESTER.DLL. First, the thread tries to open the named semaphore.
If that fails (it will fail for the first process to load) then
the thread tries to create the semaphore. If the create fails
due to a race condition with another process, the thread loops
and opens the semaphore created by the other process. This
ensures a single, shared mutex semaphore exists to protect access
to shared memory.

*/


#define INCL_DOSERRORS
#define INCL_DOS
#include <os2.h>
#include <assert.h>

typedef struct _SHAREDGLOBALS
{
  HMODULE  hmod;
  HMTX     hmtx;
  LONG     cInstances;
} SHAREDGLOBALS;

#pragma data_seg( GLOBALS )
SHAREDGLOBALS SharedGlobals;



LONG APIENTRY _DLL_InitTerm( HMODULE hmod, ULONG action )
{
  APIRET         rc;
  LONG           i = 0;
  static PSZ     pszSemName = "\\SEM32\\TESTER\\GLOBAL\\SEM";

  switch( action ) {
  case 0:   // init
    do
    {
      rc = DosOpenMutexSem( pszSemName, &SharedGlobals.hmtx );
      assert( 0 == rc || ERROR_SEM_NOT_FOUND == rc );
      if( rc )
      {
        rc = DosCreateMutexSem( pszSemName, &SharedGlobals.hmtx, 0, FALSE );
      }
      assert( i++ < 2 );
    } while( rc );

    rc = DosRequestMutexSem( SharedGlobals.hmtx, SEM_INDEFINITE_WAIT );
    assert( 0 == rc );
    if( 0 == SharedGlobals.hmod )
    {
      // first-load init here
      SharedGlobals.hmod = hmod;
    }

    // This function gets called for every new process instance
    // because of INITINSTANCE keyword in TESTER.DEF.
    SharedGlobals.cInstances++;

    // more per-process init here
    rc = DosReleaseMutexSem( SharedGlobals.hmtx );
    assert( 0 == rc );
    break;

  case 1:  // termination
    rc = DosRequestMutexSem( SharedGlobals.hmtx, SEM_INDEFINITE_WAIT );
    assert( 0 == rc );

    // This function gets called for every process termination
    // because of TERMINSTANCE keyword in TESTER.DEF.
    assert( SharedGlobals.cInstances > 0 );
    SharedGlobals.cInstances--;

    rc = DosReleaseMutexSem( SharedGlobals.hmtx );
    assert( 0 == rc );

    rc = DosCloseMutexSem( SharedGlobals.hmtx );
    assert( 0 == rc );
    break;
  }
  return 1;   // success
}
