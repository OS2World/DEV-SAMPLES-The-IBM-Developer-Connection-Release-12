/* heap.c

This code shows wrappers for DosSubAllocMem() and
DosSubFreeMem(). For convenience, it stores the base heap pointer
and size in the suballocation unit.

Heap size is fixed at allocation time; there is no way to grow
it. This is not a problem when memory requirements are known and
steady. If memory requirements are dynamic and unknown, then
implement a chain of heaps.

*/

#define INCL_DOS
#include <os2.h>
#include <assert.h>


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>


// -----------------------------------------------------------------------
// Hide the size and base heap pointer just under
// the returned pointer where myfree() can find it.

PVOID APIENTRY myalloc( PVOID pvHeap, ULONG ulSize )
{
  APIRET       rc;
  PULONG       pul = NULL;

  assert( ulSize );

  // add space for pvHeap and ulSize; make multiple of 8
  ulSize = ( ulSize + 8 + 7 ) & 0xFFFFFFF8;

  rc = DosSubAllocMem( pvHeap, (PVOID*)&pul, ulSize );
  assert( 0 == rc );

  // fill the allocated memory with zeroes
  memset( pul, 0, ulSize );

  // store size at first ULONG allocated
  *pul++ = ulSize;

  // store pvHeap at second ULONG allocated
  *pul++ = (ULONG)pvHeap;

  // return pointer just above squirrelled variables
  return (PVOID)pul;
}


// -----------------------------------------------------------------------
// myfree() takes one parameter, the pointer returned by myalloc().
// The base heap pointer and size are just below this pointer. These
// parameters are needed for DosSubFreeMem().

BOOL APIENTRY myfree( PVOID pv )
{
  APIRET    rc;
  PULONG    pul;
  PVOID     pvHeap;
  ULONG     ulSize;

  assert( pv );
  pul = (PULONG)pv;

  // get stored heap base, a multiple of 4096
  pvHeap = (PVOID)*--pul;
  assert( 0 == ((ULONG)pvHeap & 0x0FFF));

  // get stored size, a multiple of 8
  ulSize = *--pul;
  assert( 0 == (ulSize & 7));

  // free it
  rc = DosSubFreeMem( pvHeap, (PVOID)pul, ulSize );
  assert( 0 == rc );

  return TRUE;
}


// -----------------------------------------------------------------------
// client code

int main()
{
  APIRET     rc;
  PSZ        psz;
  PVOID      pvHeap;


  #define LEN_HEAP   0x10000


  // allocate the heap without committing any of the pages
  rc = DosAllocMem( &pvHeap, LEN_HEAP, PAG_WRITE );
  assert( rc == 0 );

  // prepare memory for suballocation; DosSubAllocMem() will
  // commit pages as needed;
  // recommended to suballocate the whole allocation and not grow it;
  rc = DosSubSetMem( pvHeap, DOSSUB_INIT | DOSSUB_SPARSE_OBJ, LEN_HEAP );
  assert( rc == 0 );

  // sample allocation
  psz = myalloc( pvHeap, 256 );
  assert( psz );

  // sample free
  myfree( psz );

  rc = DosSubUnsetMem( pvHeap );
  assert( rc == 0 );

  rc = DosFreeMem( pvHeap );
  assert( rc == 0 );

  return 0;
}
