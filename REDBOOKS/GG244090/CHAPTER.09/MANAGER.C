/***************************************************************************/
/*                                                                         */
/* Name:        manager.c                                                  */
/*                                                                         */
/* Description: Implements the handling procedures for the mbox data       */
/*              structure in the MessageBox sample application             */
/*                                                                         */
/***************************************************************************/

# include <string.h>
# include <malloc.h>
# include <stdio.h>

# include "mbox.h"
# include "common.h"

# define MAX_USERS                      100
# define MBOX_IMPORT_FAILED             -14
# define MBOX_EXPORT_FAILED             -15
# define EOF_READ                       0
# define STR_READ                       1

# ifdef _WINDOWS
# define strdup _fstrdup
# define strcmp _fstrcmp
# endif

/* prototypes for routines imported from the security module */
char *get_principal ( handle_t );
int is_authorized ( handle_t );

/* entry is the structure for a single message in the message queue */
struct entry {
        char            *msg;
        char            *sdr;
        struct entry    *nxt;
};

typedef struct entry    entry_t;

/* mbox is the structure for a message box for a principal */
struct mbox {
        char            *principal;
        struct entry    *first;
};

typedef struct mbox     mbox_t;

/* allocate empty message boxes for MAX_USERS */
mbox_t          mbox[MAX_USERS] = { NULL, NULL };

/***************************************************************************/
/*                                                                         */
/* Name:        function get_mbox()                                        */
/*                                                                         */
/* Description: get_mbox returns a pointer to the message box of principal */
/*              or a NULL if there is no message box for this principal.   */
/*                                                                         */
/***************************************************************************/
mbox_t *get_mbox( char *principal )
{
        mbox_t          *mb = mbox;

        while ( mb -> principal ) {
                if ( strcmp (principal, mb -> principal) == 0 )
                        return mb;
                mb++;
        }
        return NULL;
}

/***************************************************************************/
/*                                                                         */
/* Name:        function new_message()                                     */
/*                                                                         */
/* Description: new_msg inserts a new entry structure a the end of the     */
/*              message queue and returns a pointer on it.                 */
/*                                                                         */
/***************************************************************************/
entry_t *new_msg( mbox_t *mbox )
{
        entry_t      *last;
        entry_t      *new = (entry_t *) malloc( sizeof(entry_t) );

        new -> msg = NULL;
        new -> sdr = NULL;
        new -> nxt = NULL;

        if ( mbox -> first ) {
                last = mbox -> first;
                while ( last -> nxt )
                        last = last -> nxt;
                last -> nxt = new;
        } else
                mbox -> first = new;

        return new;
}

/****************************************************************************/
/*                                                                          */
/* Name:        function mbox_new()                                         */
/*                                                                          */
/* Description: mbox_new is advertised by the server to RPC clients.        */
/*              It creates a new message box entry for the given principal. */
/*              The ACLs on /.:/Servers/MessageBox must grant you control   */
/*              rights to use this command from a client application.       */
/*              mbox_new checks against existing principals for free        */
/*              message box entries.                                        */
/*                                                                          */
/****************************************************************************/
idl_long_int mbox_new ( handle_t bh, unsigned char *principal )
{
        mbox_t          *mb = mbox;

        /* check authorization */
        if ( ! is_authorized(bh) )
                return ( MBOX_NOT_AUTHORIZED );

        /* look up the principal name among the existing enries */
        while ( mb -> principal ) {
                if ( strcmp (principal, mb -> principal) == 0 )
                        return( MBOX_PRINCIPAL_EXIST );
                mb++;
        }

        /* check if there is a next free entry */
        if ( mb - mbox > MAX_USERS - 2 )
                return ( MBOX_ALL_USED );

        /* assign principal name */
        mb -> principal = strdup( principal );
        printf("Creating new Message Box for Principal %s.\n",principal);

        /* initialize next entry */
        mb++;
        mb -> principal = NULL;
        mb -> first     = NULL;

        return ( MBOX_OK );
}

/***************************************************************************/
/*                                                                         */
/* Name:        function mbox_append()                                     */
/*                                                                         */
/* Description: mbox_append is advertised by the server to RPC clients.    */
/*              It appends a new message to the existing message box of    */
/*              the given principal.                                       */
/*              Every authenticated user can call mbox_append. It tries    */
/*              to get the principal name from the RPC runtime.            */
/*                                                                         */
/***************************************************************************/
idl_long_int    mbox_append (
        handle_t       bh,
        unsigned char *principal,
        unsigned char *message
)
{
        mbox_t          *mb;
        entry_t         *new;

        /* look up the mbox entry of the principal */
        if ( ! (mb = get_mbox( principal )) )
                return ( MBOX_PRINCIPAL_NOT_EXIST );

        /* append the new message entry to the message list */
        new = new_msg( mb );

        /* assign the message and the sender principal to the */
        /* message enntry                                     */
        new -> msg = strdup( message );
        new -> sdr = get_principal( bh );
        printf("Message from %s for %s.\n",new->sdr,principal);

        return ( MBOX_OK );
}

/***************************************************************************/
/*                                                                         */
/* Name:        function mbox_next()                                       */
/*                                                                         */
/* Description: mbox_next is advertised by the server to RPC clients.      */
/*              It returns in the message parameter the oldest unread      */
/*              message from caller's mbox structure.                      */
/*              mbox_next uses the get_principal call from the security    */
/*              module to select users message box.                        */
/*              The returned message is dicarded from the message queue.   */
/*                                                                         */
/***************************************************************************/
idl_long_int mbox_next ( handle_t bh, unsigned char *message )
{
        mbox_t *mb;
        char   *principal;

        /* get caller's principal name */
        if ( (principal = get_principal(bh)) == NULL )
                return ( MBOX_NOT_AUTHORIZED );

        /* get corresponding mbox */
        if ( ! (mb = get_mbox( principal )) )
                return ( MBOX_PRINCIPAL_NOT_EXIST );

        /* prepare message out of entry */
        if ( mb -> first ) {
                /* copy the message into message */
                strcpy( message, mb -> first -> sdr );
                strcat( message,": ");
                strcat( message, mb -> first -> msg );
                printf("%s reads message from %s.\n",principal,mb->first->sdr);
                /* discard the entry */
                mb -> first = mb -> first -> nxt;
                return ( MBOX_OK );
        }

        return ( MBOX_NO_MSG );
}

/***************************************************************************/
/*                                                                         */
/* Name:        function mbox_export()                                     */
/*                                                                         */
/* Description: mbox_export exports the mbox structure from memory to a    */
/*              local file to save the message queue entries for the next  */
/*              restart of the server.                                     */
/*                                                                         */
/* File format: <message box owner principal>:                             */
/*              <sender principal>:<message>                               */
/*              empty line                                                 */
/*              ........                                                   */
/*                                                                         */
/***************************************************************************/
int mbox_export( const char *fname )
{
        entry_t         *me;
        FILE            *export;
        mbox_t          *mb = mbox;

        /* open the file for writing */
        if ( (export = fopen(fname,"w")) == NULL ) {
                fprintf(stderr,"Cannot export messages to file %s\n", fname);
                return ( MBOX_EXPORT_FAILED );
        }

        /* loop through the mbox array and print out the name and messages */
        printf("Exporting unread messages to file %s.\n",fname);
        while ( mb -> principal ) {
                fprintf(export,"%s:\n", mb -> principal);

                me = mb -> first;
                while ( me ) {
                        fprintf(export,"%s:%s\n",me -> sdr, me -> msg);
                        me = me -> nxt;
                }
                fputc('\n',export);
                mb++;
        }

        fclose(export);
        return ( MBOX_OK );

}

/***************************************************************************/
/*                                                                         */
/* Name:        function readln()                                          */
/*                                                                         */
/* Description: readln reads a line into buffer from file 'stream' until   */
/*              it reads a newline character '\n' or the end-of-file       */
/*              mark EOF. The newline character is replaced by '\0'.       */
/*              readln discards carriage returns '\r' == 0xd.              */
/*                                                                         */
/* Return Values:                                                          */
/*              STR_READ = 1 if a line is read.                            */
/*              EOF_READ = 0 if EOF is encountered.                        */
/*                                                                         */
/* NOTE:        The last line is returned together with EOF_READ.          */
/*                                                                         */
/***************************************************************************/
int readln (FILE *stream, char *buffer)
{
        int     i;
        char    *c = buffer;

        while ( 1 ) {
                switch ( i = fgetc(stream) ) {
                case EOF:
                        *c = '\0';
                        return EOF_READ;
                case '\n':
                        *c = '\0';
                        return STR_READ;
                case '\r':
                        continue;
                default:
                        *c++ = i;
                        if ( c - buffer > MAX_CHAR )
                                return STR_READ;

                        continue;
                }
        }
}

/***************************************************************************/
/*                                                                         */
/* Name:        function mbox_import()                                     */
/*                                                                         */
/* Description: mbox_import imports the mbox structure from the file fname */
/*              written by mbox_export.                                    */
/*                                                                         */
/***************************************************************************/
int mbox_import ( const char *fname )
{
        FILE            *import;
        char            buffer[MAX_CHAR];
        char            *m;
        entry_t         *me;
        mbox_t          *mb = mbox;

        /* open local file fname for reading */
        if ( (import = fopen(fname,"r")) == NULL ) {
                fprintf(stderr, "Cannot import messages from file %s\n",fname);
                return( MBOX_IMPORT_FAILED );
        }

        printf("Importing old messages from file %s.\n",fname);

        /* while file is not empty read lines */
        /* NOTE: The last line is ignored because it is always */
        /* empty by definition of the file format.             */
        while ( readln(import, buffer) ) {
                if ( *buffer == '\0' ) {
                        /* line read is empty -> new principal */

                        if ( ++mb - mbox > MAX_USERS - 2 )
                                return ( MBOX_ALL_USED );

                        mb -> principal = NULL;
                        mb -> first     = NULL;
                        continue;
                }

                /* split line after ':' */
                if ( m = strchr(buffer,':') )
                        *m++ = '\0';

                /* if principal is not set a new message box is read */
                if ( mb -> principal ) {
                        /* append message to message list */
                        me = new_msg( mb );
                        me -> sdr = strdup(buffer);
                        me -> msg = strdup(m);
                } else
                        /* set principal to assign new message box */
                        mb -> principal = strdup(buffer);
        }

        fclose( import );
        return( MBOX_OK );
}
