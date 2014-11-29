/******************************************************************************/
/* Module  : ft_m.c                                                           */
/* Purpose : Server manager code. Implements the send and receive file        */
/*           procedures that will be called from the client ft_c.             */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "ft.h"

idl_byte buffer[ BUFFER_SIZE ];

/******************************************************************************/
/* Procedure  : send_file                                                     */
/* Purpose    : Allow a client to send a file to the machine where the server */
/*              runs. Uses DCE pipes and implicit binding.                    */
/******************************************************************************/

error_status_t send_file (
   file_name_t file_name,
   b_pipe_t data )
{
   FILE *file_p;
   idl_ulong_int actual_data;

   /* Open local file.                                                        */
   file_p = fopen ( file_name, "wb" );
   if ( file_p == NULL ) {
         printf ( "Cannot open file %s\n", file_name );
         return -1;
   }

   /* Write data into file until pipe is empty.                               */
   printf ( "Receiving file %s\n", file_name );
   while ( 1 ) {
      ( data.pull ) (                   /* Server stub pull routine.          */
         data.state,                    /* Pipe state on the server side.     */
         buffer,                        /* Buffer to place data.              */
         BUFFER_SIZE,                   /* Maximum amount of data allowed.    */
         &actual_data );                /* Amount of data actually placed.    */
      if ( actual_data == 0 )
         break;
      fwrite ( buffer, 1, (int)actual_data, file_p );
   }

   /* Clean up and finish.                                                    */
   fclose ( file_p );
   return rpc_s_ok;
}

/******************************************************************************/
/* Procedure  : receive_file                                                  */
/* Purpose    : Allow a client to receive a file from the machine where the   */
/*              server runs. Uses DCE pipes and implicit binding.             */
/******************************************************************************/

error_status_t receive_file (
   file_name_t file_name,
   b_pipe_t *data )
{
   FILE *file_p;
   idl_ulong_int num_bytes;

   /* Open local file.                                                        */
   file_p = fopen ( file_name, "rb" );
   if ( file_p == NULL ) {
         printf ( "Cannot open file %s\n", file_name );
         return -1;
   }

   /* Read data from file until EOF is reached. Inform EOF with a 0 count.    */
   printf ( "Sending file %s\n", file_name );
   while ( 1 ) {
      num_bytes = fread ( buffer, 1, BUFFER_SIZE, file_p );
      ( data->push ) (                  /* Server stub push routine.          */
         data->state,                   /* Pipe state on the server side.     */
         buffer,                        /* Buffer to get data from.           */
         num_bytes );                   /* Amount of data in buffer.          */
      if ( num_bytes == 0 )
         break;
   }

   /* Clean up and finish.                                                    */
   fclose ( file_p );
   return rpc_s_ok;
}
