/******************************************************************************/
/* Module  : phon_m.c                                                         */
/* Purpose : Server manager code. Implements the phone number lookup          */
/*           procedure that will be called from the client lookup.            */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "look.h"

#define BUFFER_SIZE MAX_NAME_LENGTH + MAX_DIR_ENTRY_LENGTH + 1
#define PHONE_FIL "PHONE.FIL"

/******************************************************************************/
/* Procedure  : lookup_phon                                                   */
/* Purpose    : Allows a client to do an phone directory lookup using a       */
/*              person's name. Returns the phone number of the person if name */
/*              is found. If not, a zero length string is returned.           */
/******************************************************************************/

void lookup_phon (
   rpc_binding_handle_t bh,
   name_t name,
   dir_entry_t phone )
{
   FILE *file_p;
   char buffer[ BUFFER_SIZE ];

   /* Open directory file.                                                    */
   file_p = fopen ( PHONE_FIL, "r" );
   if ( file_p == NULL ) {
         printf ( "Cannot open file %s\n", PHONE_FIL );
         return;
   }

   /* Search for directory entry containing the person's name.                */
   printf ( "Doing a phone number lookup for %s\n", name );
   while ( 1 ) {
      /* Return a 0 length string if lookup fails.                            */
      if ( fgets ( buffer, BUFFER_SIZE, file_p ) == NULL ) {
         *phone = '\0';
         break;
      }

      /* Return a person's phone number if lookup is successful.              */
      if ( strstr ( buffer, name ) != NULL ) {
         buffer[ strlen ( buffer ) - 1 ] = '\0';
         strcpy ( phone, &buffer[ strlen ( name ) + 1 ] );
         break;
      }
   }

   /* Clean up and finish.                                                    */
   fclose ( file_p );
   return;
}

/* Declare and initialize the phone lookup manager EPV table.                 */
look_v1_0_epv_t epv_phon = { lookup_phon };
