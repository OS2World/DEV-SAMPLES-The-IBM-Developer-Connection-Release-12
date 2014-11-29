/*****************************************************************************/
/* Module: database_manager.c                                                */
/*                                                                           */
/* Description:                                                              */
/*    This module implements the following RPC operations:                   */
/*                                                                           */
/*             db_open         establishes the connection to a database      */
/*             db_close        detaches the connection                       */
/*             db_read         reads a record from the current position      */
/*             db_setpos       sets the current position                     */
/*             db_getpos       gets the current position                     */
/*                                                                           */
/*    This module also maintains the client's context that is defined by     */
/*    type db_context_t, and therefore implements the rundown procedure      */
/*    "db_context_t_rundown()"                                               */
/*                                                                           */
/*****************************************************************************/
#ifdef _WINDOWS
#include <dce/dcewin.h>
#endif
#include "db.h"
#include "errorchk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char database[][ MAX_DB_DATA_SIZE ] = {
   "Almeida, Marcio Cravo De",
   "Gottschalk, Klaus",
   "Miyamoto, Toshiro",
   "Villela, Agostinho De Arruda",
   "Silva, Nelson Alves da ",
   "ITSC, Austin ITSC "
};

#define  MAX_DB_RECORDS  sizeof( database ) / MAX_DB_DATA_SIZE
#define  LAST_DB_RECORD  MAX_DB_RECORDS - 1

typedef struct _db_client_context_t {
   long  pos;
} db_client_context_t;

/*****************************************************************************/
/* Operation: db_open                                                        */
/*****************************************************************************/
void db_open(
      handle_t         binding_h,
      db_context_t    *pcontext_h,
      error_status_t  *pst )
{
   db_client_context_t *pclient_context;

   pclient_context = (db_client_context_t *) malloc(
                             sizeof( db_client_context_t ) );
   if ( pclient_context == NULL ) {
      *pcontext_h = NULL;
      *pst = rpc_s_fault_remote_no_memory;
   } else {
      pclient_context->pos = 0;
      *pcontext_h = (db_context_t) pclient_context;
      *pst = rpc_s_ok;
      printf( "db_open:    Context handle = %08X\n", pclient_context );
   }
}

/*****************************************************************************/
/* Operation: db_close                                                       */
/*****************************************************************************/
void db_close(
      db_context_t    *pcontext_h,
      error_status_t  *pst )
{
   printf( "db_close:   Context handle = %08X\n", *pcontext_h );
   free( *pcontext_h );
   *pcontext_h = NULL;
   *pst = rpc_s_ok;
}

/*****************************************************************************/
/* Operation: db_read                                                        */
/*****************************************************************************/
void db_read(
      db_context_t     context_h,
      db_data_t       *pdata,
      long            *ppos,
      error_status_t  *pst )
{
   db_client_context_t *pclient_context = (db_client_context_t *) context_h;

   (*pdata).size = strlen( database[ pclient_context->pos ] ) + 1;
   strcpy( (*pdata).data, database[ pclient_context->pos ] );
   if ( pclient_context->pos < LAST_DB_RECORD )
      pclient_context->pos++;
   else
      pclient_context->pos = LAST_DB_RECORD;
   *ppos = pclient_context->pos;
   printf( "db_read:    New position for context %08X = %ld\n",
            pclient_context, pclient_context->pos );
   *pst = rpc_s_ok;
}

/*****************************************************************************/
/* Operation: db_setpos                                                      */
/*****************************************************************************/
void db_setpos(
      db_context_t     context_h,
      long            *ppos,
      error_status_t  *pst )
{
   db_client_context_t *pclient_context = (db_client_context_t *) context_h;

   if ( *ppos < MAX_DB_RECORDS )
      pclient_context->pos = *ppos;
   else
      pclient_context->pos = LAST_DB_RECORD ;

   printf( "db_setpos:  New position for context %08X = %ld\n",
            pclient_context, pclient_context->pos );
   *pst = rpc_s_ok;
}

/*****************************************************************************/
/* Operation: db_getpos                                                      */
/*****************************************************************************/
void db_getpos(
      db_context_t     context_h,
      long            *ppos,
      error_status_t  *pst )
{
   db_client_context_t *pclient_context = (db_client_context_t *) context_h;

   *ppos = pclient_context->pos;
   printf( "db_getpos   New position for context %08X = %ld\n",
            pclient_context, pclient_context->pos );
   *pst = rpc_s_ok;
}

/*****************************************************************************/
/* Operation: db_context_t_rundown                                            */
/*****************************************************************************/
void db_context_t_rundown(
      db_context_t     context_h )
{
   printf( "db_context_rundown: Context handle = %08X\n", context_h );
   free( context_h );
}

