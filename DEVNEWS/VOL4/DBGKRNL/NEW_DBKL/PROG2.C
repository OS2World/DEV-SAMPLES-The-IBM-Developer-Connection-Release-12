//  prog2.c
#define INCL_DOS
#include <os2.h>
#include <stdio.h>

int main( void );
void _System thread2( void *pv );

HMTX  hmtx1, hmtx2;

//--------------------------------------------
int main()
{
   // create two named, unowned mutex semaphores
   DosCreateMutexSem( "\\SEM32\\MUTEX1", &hmtx1, 0, 0);
   DosCreateMutexSem( "\\SEM32\\MUTEX2", &hmtx2, 0, 0);
   // start the second thread
   _beginthread( thread2, NULL, 32768, NULL );
   while ( 1 ) {
      DosRequestMutexSem( hmtx1, -1 );
      DosRequestMutexSem( hmtx2, -1 );
      printf( "Thread 1 owns the two semaphores\n" );
      DosReleaseMutexSem( hmtx2 );
      DosReleaseMutexSem( hmtx1 );
   }
   return 0;
}
//--------------------------------------------
void _System thread2( void *pv )
{
   while ( 1 ) {
      DosRequestMutexSem( hmtx2, -1 );
      DosRequestMutexSem( hmtx1, -1 );
      printf( "Thread 2 owns the two semaphores\n" );
      DosReleaseMutexSem( hmtx1 );
      DosReleaseMutexSem( hmtx2 );
   }
}
