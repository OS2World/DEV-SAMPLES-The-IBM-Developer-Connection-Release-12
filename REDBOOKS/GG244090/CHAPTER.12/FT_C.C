/******************************************************************************/
/* Module  : ft_c.c                                                           */
/* Purpose : Client module for server ft_s. Transfers files to and from a     */
/*           host. The server name, direction of the transfer, local          */
/*           and remote file name are all obtained from the command line.     */
/*           Uses DCE pipes and implicit binding.                             */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dce/rpc.h>
#ifdef _WINDOWS
#include <dce/dcewin.h>
#endif
#include "errchk.h"
#include "ft.h"

/******************************************************************************/
/* Procedure  : main                                                          */
/* Purpose    : Client module main program. Parses the command line for the   */
/*              server name, direction of the transfer, local and remote file */
/*              name, initializes the state and pipe structures and invokes   */
/*              the necessary remote procedure to transfer the data.          */
/******************************************************************************/

/* Define the pipe application specific client state structure.               */
typedef struct ft_state {
   FILE *file_p;                        /* Local file pointer.                */
   char *file_name;                     /* Local file name.                   */
} ft_state;

int main ( int argc, char *argv[] )
{
   char *server_name;
   file_name_t local_file , remote_file;
   rpc_binding_handle_t bh;
   unsigned32 status;
   rpc_ns_handle_t imp_ctxt;

   /* Allocate the pipe application specific client state structure.          */
   ft_state state;

   /* Allocate the pipe structure.                                            */
   b_pipe_t ft_pipe;

   /* Declare the pull, push and alloc routines.                              */
   void data_alloc (), read_f (), write_f ();

   /* Check if the user passed the correct number of parameters.              */
   if ( argc != 5 ) {
      printf (
"Usage : %s <server> <direction : s or r> <local file> <remote file>\n",
         argv[ 0 ] );
      exit ( 1 );
   }

   /* Get the server name from the command line.                              */
   server_name = argv[ 1 ];

   /* Set up the context to import the bindings.                              */
   rpc_ns_binding_import_begin ( rpc_c_ns_syntax_dce, server_name,
                                 ft_v1_0_c_ifspec, NULL, &imp_ctxt,
                                 &status );
   ERRCHK ( status );

   /* Get the first binding handle.                                           */
   rpc_ns_binding_import_next ( imp_ctxt, &bh, &status );
   ERRCHK ( status );

   /* Store the binding handle bh in the global variable defined in .acf.     */
   global_handle = bh;

   /* Initialize the pipe state structure.                                    */
   state.file_p = NULL;
   state.file_name = local_file;

   /* Initialize the pipe structure.                                          */
   ft_pipe.state = ( rpc_ss_pipe_state_t )&state;
   ft_pipe.alloc = data_alloc;
   ft_pipe.pull = read_f;
   ft_pipe.push = write_f;

   /* Load the filenames, truncating if necessary.                            */
   strncpy ( local_file, argv[ 3 ], FILE_NAME_LENGTH );
   local_file[ FILE_NAME_LENGTH ] = '\0';
   strncpy ( remote_file, argv[ 4 ], FILE_NAME_LENGTH );
   remote_file[ FILE_NAME_LENGTH ] = '\0';

   /* Determine the direction of the transfer.                                */
   switch ( argv[ 2 ][ 0 ] ) {
      case 's' :
         /* Send the file testing the success of the transfer.                */
         if ( send_file ( remote_file, ft_pipe ) != rpc_s_ok )
            printf ( "Error sending file %s\n", local_file );
         else
            printf ( "File %s sent to %s as %s\n", local_file,
                     server_name, remote_file );
         break;
      case 'r' :
         /* Receive the file testing the success of the transfer.             */
         if ( receive_file ( remote_file, &ft_pipe ) != rpc_s_ok )
            printf ( "Error receiving file %s\n", remote_file );
         else
            printf ( "File %s received from %s as %s\n", remote_file,
                     server_name, local_file );
         break;
      default :
         printf (
"Usage : %s <server> <direction : s or r> <local file> <remote file>\n",
            argv[ 0 ] );
         break;
   }

   /* Release the context.                                                    */
   rpc_ns_binding_import_done ( &imp_ctxt, &status );
   ERRCHK ( status );
}

/******************************************************************************/
/* Procedure  : data_alloc                                                    */
/* Purpose    : Allocate memory on the client side for the pipe of the file   */
/*              transfer application.                                         */
/******************************************************************************/

idl_byte buffer[ BUFFER_SIZE ];

void data_alloc (
   ft_state *state,                     /* Pipe state on the client side.     */
   idl_ulong_int desired_size,          /* Size of buffer desired.            */
   idl_byte **buff,                     /* Pointer to buffer pointer.         */
   idl_ulong_int *alloc_size )          /* Size actually allocated.           */
{
   *buff = buffer;
   *alloc_size = BUFFER_SIZE;
   return;
}

/******************************************************************************/
/* Procedure  : read_f                                                        */
/* Purpose    : Read bytes from local file placing them in a buffer           */
/*              designated by a parameter. Performs the pull for the pipe of  */
/*              the file transfer application.                                */
/******************************************************************************/

void read_f (
   ft_state *state,                     /* Pipe state on the client side.     */
   idl_byte *buff,                      /* Buffer to place data.              */
   idl_ulong_int max_data,              /* Maximum amount of data allowed.    */
   idl_ulong_int *actual_data )         /* Amount of data actually placed.    */
{
   /* Open local file if not open yet.                                        */
   if ( state->file_p == NULL ) {
      state->file_p = fopen ( state->file_name, "rb" );
      if ( state->file_p == NULL ) {
         printf ( "Cannot open file %s\n", state->file_name );
         exit ( 1 );
      }
   }

   /* Load data into buffer, returning a 0 count on end of data.              */
   *actual_data = fread ( buff, 1, (int)max_data, state->file_p );

   /* Close file when end of data has been reached.                           */
   if ( *actual_data == 0 )
      fclose ( state->file_p );
}

/******************************************************************************/
/* Procedure  : write_f                                                       */
/* Purpose    : Write bytes into local file getting them from a buffer        */
/*              designated by a parameter. Performs the push for the pipe of  */
/*              the file transfer application.                                */
/******************************************************************************/

void write_f (
   ft_state *state,                     /* Pipe state on the client side.     */
   idl_byte *buff,                      /* Buffer to get data from.           */
   idl_ulong_int num_bytes )            /* Amount of data in buffer.          */
{
   /* Open local file if not open yet.                                        */
   if ( state->file_p == NULL ) {
      state->file_p = fopen ( state->file_name, "wb" );
      if ( state->file_p == NULL ) {
         printf ( "Cannot open file %s\n", state->file_name );
         exit ( 1 );
      }
   }

   /* If buffer empty, close file, else, get data from buffer.                */
   if ( num_bytes == 0 )
      fclose ( state->file_p );
   else
      fwrite ( buff, 1, (int)num_bytes, state->file_p );
   return;
}
