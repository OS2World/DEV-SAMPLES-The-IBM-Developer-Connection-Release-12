/******************************************************************************/
/* Module  : lookup.c                                                         */
/* Purpose : Client module for servers phon_s and addr_s. Gets a person's     */
/*           name from standard input and displays on standard output         */
/*           the person's phone number and address. Program is terminated     */
/*           by providing a null string. Uses threads, object UUIDs           */
/*           and explicit binding.                                            */
/******************************************************************************/

#ifdef IBMOS2
#define INCL_DOSPROCESS
#include <os2.h>
#endif

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dce/rpc.h>
#ifdef _WINDOWS
#include <dce/dcewin.h>
#endif
#include "errchk.h"
#include "look.h"

#define PHON_ENTRY_NAME "/.:/Servers/Phon"  /* Phone server entry name.       */
#define ADDR_ENTRY_NAME "/.:/Servers/Addr"  /* Address server entry name.     */

#define APP_EXIT   3
#define APP_LOOKUP 2


#define THRCHK( x )                                                            \
   if ( ( x ) == -1 ) {                                                        \
      printf ( "THREAD FAULT: %s:%d\n", __FILE__, __LINE__ );                  \
      exit ( 1 );                                                              \
   }

/*static void get_bind ( char **, char *, char *, rpc_binding_handle_t * );*/

pthread_t phon_thread_h,                /* Thread handle for phone thread.    */
          addr_thread_h,                /* Thread handle for address thread.  */
          addr_bind_thread_h,
          phon_bind_thread_h;

rpc_binding_handle_t phon_bh,           /* Binding handle of phone server.    */
                     addr_bh;           /* Binding handle of address server.  */

/* Mutex for condition variable name_ready.                                   */
pthread_mutex_t name_mutex;

/* Predicate for condition variable name_ready.                               */
int name_ready_p;

/* Condition variable for acessing variable name.                             */
pthread_cond_t name_ready;

char **argv;
name_t name;                            /* Person's name.                     */
dir_entry_t phone,                      /* Person's phone number.             */
            address;                    /* Person's address.                  */

/******************************************************************************/
/* Procedure  : main                                                          */
/* Purpose    : Client module main program. Gets binding                      */
/*              information for the phone directory server and the address    */
/*              directory server then spawns a thread to do a phone directory */
/*              lookup and spawns another thread to do address directory      */
/*              lookup on their servers.                                      */
/******************************************************************************/

int main ( int argc, char *argvv[] )
{
   int status;
   void look_thread (), get_person (), get_bind();

   argv = argvv;


   /* Get binding information of the phone server.                            */
   status = pthread_create ( &phon_bind_thread_h, pthread_attr_default,
                             ( pthread_startroutine_t )get_bind,
                             ( pthread_addr_t ) 1 );
   THRCHK ( status );

   /* Get binding information of the address server.                          */
   get_bind ( 2 );

   /* Wait for all binding   threads to terminate.                            */
   status = pthread_join ( phon_bind_thread_h, NULL );
   THRCHK ( status );

   /* Free the storage being used by the binding   threads.                   */
   status = pthread_detach ( &phon_bind_thread_h );
   THRCHK ( status );


   /* Initialize condition variable mutex.                                    */
   status = pthread_mutex_init ( &name_mutex, pthread_mutexattr_default );
   THRCHK ( status );

   /* Initialize condition variable.                                          */
   status = pthread_cond_init ( &name_ready, pthread_condattr_default );
   THRCHK ( status );

   /* Initialize condition variable predicate.                                */
   name_ready_p = APP_LOOKUP;


   /* Create a thread to do a phone number lookup in a server.                */
   status = pthread_create ( &phon_thread_h, pthread_attr_default,
                             ( pthread_startroutine_t )look_thread,
                             ( pthread_addr_t ) 1 );
   THRCHK ( status );

   /* Create a thread to do an address lookup in a server.                    */
   status = pthread_create ( &addr_thread_h, pthread_attr_default,
                             ( pthread_startroutine_t )look_thread,
                             ( pthread_addr_t ) 2 );
   THRCHK ( status );

   /* Get input of a person's name.                                          */
   get_person ( );

   /* Wait for all remaining threads to terminate.                            */
   status = pthread_join ( phon_thread_h, NULL );
   THRCHK ( status );
   status = pthread_join ( addr_thread_h, NULL );
   THRCHK ( status );

   /* Free the storage being used by the remaining threads.                   */
   status = pthread_detach ( &phon_thread_h );
   THRCHK ( status );
   status = pthread_detach ( &addr_thread_h );
   THRCHK ( status );

   printf ( "Thanks for using %s !\n", argv[ 0 ] );
}

/******************************************************************************/
/* Procedure  : get_person                                                    */
/* Purpose    : Routine for getting a person's name from stdin.               */
/*              Communicates with threads look_phon and look_addr using the   */
/*              global variable "name" controlled by the condition variable   */
/*              name_ready and its predicate name_ready_p. Routine is         */
/*              terminated by entering a null string.                         */
/******************************************************************************/

void get_person ()
{
   int status;

   /* Lock the condition variable mutex.                                      */
   status = pthread_mutex_lock ( &name_mutex );
   THRCHK ( status );


   /* Wait on condition variable until look_threads are ready.                */
   while ( name_ready_p ) {
      status = pthread_cond_wait ( &name_ready, &name_mutex );
      THRCHK ( status );
   };

   status = pthread_mutex_unlock ( &name_mutex );
   THRCHK ( status );

   /* Loop until a null string is entered.                                    */
   while ( 1 ) {

      /* Display a prompt.                                                    */
      printf ( "\nEnter a name or null string to end :\n" );

      /* Get another name, leaving loop if null string is entered.            */
      memset ( name, '\0', MAX_NAME_LENGTH + 1 );
      gets ( name );

      /* Lock the condition variable mutex.                                      */
      status = pthread_mutex_lock ( &name_mutex );
      THRCHK ( status );

      /* Update the predicate and broadcast a signal advertising an update.   */
      name_ready_p = ( *name == '\0' ) ? APP_EXIT : APP_LOOKUP;

      status = pthread_cond_broadcast ( &name_ready );
      THRCHK ( status );

      /* If a null was entered, exit                                          */
      if ( name_ready_p == APP_EXIT ) {
         status = pthread_mutex_unlock ( &name_mutex );
         THRCHK ( status );
         break;
      }

      /* Wait on condition variable until both threads finish the lookup.     */
      do {
         status = pthread_cond_wait ( &name_ready, &name_mutex );
         THRCHK ( status );
      } while ( name_ready_p );

      status = pthread_mutex_unlock ( &name_mutex );
      THRCHK ( status );

      /* Print findings.                                                      */
      if ( *phone != '\0' )
         printf ( "Phone number: %s\n", phone );
      else
         printf ( "Phone number of %s has not been found\n", name );

      if ( *address != '\0' )
         printf ( "Address: %s\n", address );
      else
         printf ( "Address of %s has not been found\n", name );
   }
}

/******************************************************************************/
/* Procedure  : look_thread                                                   */
/* Purpose    : Thread routine for doing a lookup in a phone number or        */
/*              address directory server using a person's name.               */
/*              Person's name is obtained from                                */
/*              global variable "name" controlled by the condition variable   */
/*              name_ready and its predicate name_ready_p. Result is          */
/*              displayed on stdout.                                          */
/******************************************************************************/

void look_thread (int type)
{
   int status;

   /* Execute until cancelled.                                                */
   while ( 1 ) {

      /* Lock the condition variable mutex.                                   */
      status = pthread_mutex_lock ( &name_mutex );
      THRCHK ( status );

      /* Flag that one access has been completed.                             */
      name_ready_p--;

      /* If all accesses have been completed, broadcast a signal.             */
      if ( name_ready_p == 0 ) {
         status = pthread_cond_broadcast ( &name_ready );
         THRCHK ( status );
      }

      /* Wait on condition variable until there is a new name.                */
      do {
         status = pthread_cond_wait ( &name_ready, &name_mutex );
      } while ( name_ready_p <= 1 );

      /* Release the condition variable mutex.                                */
      status = pthread_mutex_unlock ( &name_mutex );
      THRCHK ( status );

      if ( name_ready_p == APP_EXIT )
         break;


      /* Perform a lookup selecting the correct server through its handle.    */
      if (type == 1) {
         lookup_dir ( phon_bh, name, phone );
      } else {
         lookup_dir ( addr_bh, name, address );
      }

   }
}


/******************************************************************************/
/* Procedure  : get_bind                                                      */
/* Purpose    : Thread routine for getting the binding information of eiher a */
/*              phone or address directory server.                            */
/******************************************************************************/

void get_bind (int server_type)
{
   uuid_t obj_uuid;
   unsigned32 status;
   rpc_ns_handle_t imp_ctxt;
   char *obj_uuid_string;
   char *entry_name;
   rpc_binding_handle_t *binding_handle;

   if (server_type == 1) {
      printf ( "Obtaining phone server's binding handle\n" );
      obj_uuid_string = PHON_OBJ_UUID;
      entry_name =      PHON_ENTRY_NAME;
      binding_handle =  &phon_bh;
   } else {
      printf ( "Obtaining address server's binding handle\n" );
      obj_uuid_string = ADDR_OBJ_UUID;
      entry_name =      ADDR_ENTRY_NAME;
      binding_handle =  &addr_bh;
   } /* endif */


   /* Create object UUID for phone directory from string.                     */
   uuid_from_string ( obj_uuid_string, &obj_uuid, &status );
   ERRCHK ( status );

   /* Set up context to import the bindings of the phone server.              */
   rpc_ns_binding_import_begin ( rpc_c_ns_syntax_dce, entry_name,
                                 look_v1_0_c_ifspec, &obj_uuid,
                                 &imp_ctxt, &status );
   ERRCHK ( status );

   /* Get the first binding handle of the phone server.                       */
   rpc_ns_binding_import_next ( imp_ctxt, binding_handle, &status );
   ERRCHK ( status );

   /* Release the context.                                                    */
   rpc_ns_binding_import_done ( &imp_ctxt, &status );
   ERRCHK ( status );

   if (server_type == 1) {
      printf ( "Phone server's binding done\n" );
   } else {
      printf ( "Address server's binding done\n" );
   } /* endif */
}
