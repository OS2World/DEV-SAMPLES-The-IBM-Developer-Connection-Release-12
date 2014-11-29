
/*

gbmtifh.c - Routines to handle TIFF file headers

*/

/*...sincludes:0:*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#ifdef AIX
#include <unistd.h>
#else
#include <io.h>
#endif
#include <fcntl.h>
#include "gbm.h"
#define	_GBMTIFH_
#include "gbmtifh.h"

/*...vgbmtifh\46\h:0:*/
/*...e*/

/*...susefull:0:*/
#define	ifd_malloc()	((IFD *) malloc(sizeof(IFD)))
#define	ifd_free(ifd)	free((char *) ifd)

#define	ifh_malloc()	((IFH *) malloc(sizeof(IFH)))
#define	ifh_free(ifh)	free((char *) ifh)

static int sizeof_data_type(short data_type)
	{
	switch ( data_type )
		{
		case D_BYTE:
		case D_SBYTE:
		case D_ASCII:
		case D_UNDEFINED:
			return 1;
		case D_SHORT:
		case D_SSHORT:
			return sizeof(short);
		case D_LONG:
		case D_SLONG:
			return sizeof(long);
		case D_RATIONAL:
		case D_SRATIONAL:
			return sizeof(rational);
		case D_FLOAT:
			return 4;
		case D_DOUBLE:
			return 8;
		}
	return 1;
	}

static void tag_free(TAG *tag)
	{
	free(tag->value);
	}

/*
This finds the slot for the new tag. It returns NULL if the limit of
MAX_TAGS tags is curruntly defined.
*/

static TAG *get_tag_slot(short type, IFD *ifd)
	{
	int	i;

	if ( ifd->n_tags == MAX_TAGS )
		return NULL;

	for ( i = ifd->n_tags;
	      i > 0 && ifd->tags[i - 1].type >= type;
	      i-- )
		memcpy(ifd->tags + i,
		       ifd->tags + i - 1,
		       sizeof(TAG));

	ifd->n_tags++;

	/* now i == slot, with greater than elements moved up */

	return &(ifd->tags[i]);
	}
/*...e*/

/*...sread_ifh_and_ifd:0:*/
/*...sread_long:0:*/
#define	make_long(b1,b2,b3,b4) ( ((long)(b1)<<24) | ((long)(b2)<<16) | ((long)(b3)<<8) | (long)(b4) )

static long read_long(int fd, BOOLEAN motorola)
	{
	byte b[4];

	read(fd, b, 4);
	return  ( motorola ) ?
		make_long(b[0], b[1], b[2], b[3]) :
		make_long(b[3], b[2], b[1], b[0]) ;
	}
/*...e*/
/*...sread_short:0:*/
#define	make_short(b1,b2) ( ((short)(b1)<<8) | (short)(b2) )

static short read_short(int fd, BOOLEAN motorola)
	{
	byte b[2];

	read(fd, b, 2);
	return ( motorola ) ?
		make_short(b[0], b[1]) :
		make_short(b[1], b[0]) ;
	}
/*...e*/
/*...sread_rational:0:*/
static void read_rational(int fd, BOOLEAN motorola, rational *r)
	{
	r->numerator   = read_long(fd, motorola);
	r->denominator = read_long(fd, motorola);
	}
/*...e*/
/*...sread_tag:0:*/
static int read_tag(int fd, BOOLEAN motorola, TAG *tag)
	{
	int	i, s, n;
	long	len;
	long	seek_to, old_pos;

	tag->type      = read_short(fd, motorola);
	tag->data_type = read_short(fd, motorola);
	tag->length    = read_long(fd, motorola);

	if ( tag->type & 0x8000 )
		/* proprietry tag */
		{
		lseek(fd, 4L, SEEK_CUR);	/* skip data */
		return TE_OK;			/* assumed ok */
		}

	n   = (int) tag->length;

/*...sbugfix for UBU\39\s writing of ColorMap tag:8:*/
/* UBU writes out a length feild of 256 when it should write 768 */

if ( tag->type == T_COLORMAP && (n / 3) * 3 != n )
	n *= 3;
/*...e*/

	s   = sizeof_data_type(tag->data_type);
	len = s * n;

	if ( len > 4 )
		/* will have to seek for data */
		{
		seek_to = read_long(fd, motorola);
		old_pos = lseek(fd, 0L, SEEK_CUR);
		lseek(fd, seek_to, SEEK_SET);
		}

	if ( (tag->value = malloc((int) len)) == NULL )
		return TE_MEM;

	switch ( tag->data_type )
		{
		case D_BYTE:
		case D_SBYTE:
			read(fd, tag->value, n);
			break;
		case D_ASCII:
			read(fd, tag->value, n);
			break;
		case D_SHORT:
		case D_SSHORT:
			{
			short	*short_ptr = (short *) tag->value;

			for ( i = 0; i < n; i++ )
				*short_ptr++ = read_short(fd, motorola);
			}
			break;
		case D_LONG:
		case D_SLONG:
			{
			long	*long_ptr = (long *) tag->value;

			for ( i = 0; i < n; i++ )
				*long_ptr++ = read_long(fd, motorola);
			}
			break;
		case D_RATIONAL:
		case D_SRATIONAL:
			{
			rational *rational_ptr = (rational *) tag->value;

			for ( i = 0; i < n; i++ )
				read_rational(fd, motorola, rational_ptr++);
			}
			break;
		case D_FLOAT:
			/* Skip 4 byte IEEE floating point */
			lseek(fd, 4 * len, SEEK_CUR);
			break;
		case D_DOUBLE:
			/* Skip 8 byte IEEE double precision floating point */
			lseek(fd, 8 * len, SEEK_CUR);
			break;
		default:
			read(fd, tag->value, (int) len);
			break;
		}

	if ( len > 4 )
		lseek(fd, old_pos, SEEK_SET);
	else if ( len < 4 )
		lseek(fd, 4L - len, SEEK_CUR);	/* advance past gap */

	return TE_OK;
	}
/*...e*/
/*...sread_ifd:0:*/
/*
For the time being we will assume there is only one IFD in
a given TIFF file. When this code was written, the author
knew of no software packages that support multiple IFDs.
*/

/*...sclean_up_ifd:0:*/
static void clean_up_ifd(IFD *ifd, int n)
	{
	int	i;
	TAG	*tag;

	for ( i = 0; i < n; i++ )
		{
		tag = &(ifd->tags[i]);
		if ( !(tag->type & 0x8000) )	/* its not read in */
			tag_free(tag);
		}
	ifd_free(ifd);
	}
/*...e*/

static int read_ifd(int fd, BOOLEAN motorola, IFD **ifd_return)
	{
	IFD	*ifd;
	int	i, ecode;

	if ( (ifd = ifd_malloc()) == NULL )
		return TE_MEM;

	/* ensure we can handle all the tags */

	if ( (ifd->n_tags = read_short(fd, motorola)) > MAX_TAGS )
		{
		ifd_free(ifd); return TE_N_TAGS;
		}

	/* get the tags */

	for ( i = 0; i < ifd->n_tags; i++ )
		if ( (ecode = read_tag(fd, motorola, &(ifd->tags[i]))) != TE_OK )
			{
			clean_up_ifd(ifd, i);
			return ecode;
			}

	*ifd_return = ifd;

	return TE_OK;
	}
/*...e*/
/*...sskip_ifd:0:*/
/* Returns TRUE if there is another IFD afterwards */

static BOOLEAN skip_ifd(int fd, BOOLEAN motorola)
	{
	short n_tags = read_short(fd, motorola);
	long offset_ifd;
	lseek(fd, 12L * n_tags, SEEK_CUR);
	offset_ifd = read_long(fd, motorola);
	if ( offset_ifd == 0L )
		return FALSE;
	lseek(fd, offset_ifd, SEEK_SET);
	return TRUE;
	}
/*...e*/

int read_ifh_and_ifd(int fd, int n_ifds_to_skip, IFH **ifh_return)
	{
	IFH	*ifh;
	long	offset_ifd;
	BOOLEAN	motorola;
	int	ecode;

	if ( (ifh = ifh_malloc()) == NULL )
		return TE_MEM;

	read(fd, (char *) &(ifh->byte_order), sizeof(short));
	motorola = ( ifh->byte_order == ('M' << 8) + 'M' );

	/* Apparently, the following number has great univeral significance! */
	/* See the TIFF 5.0 spec. for details! */

	if ( (ifh->version_no = read_short(fd, motorola)) != 42 )
		{
		ifh_free(ifh); return TE_VERSION;
		}

	offset_ifd = read_long(fd, motorola);
	lseek(fd, offset_ifd, SEEK_SET);
	while ( n_ifds_to_skip-- > 0 )
		if ( !skip_ifd(fd, motorola) )
			return TE_N_IFD;

	if ( (ecode = read_ifd(fd, motorola, &(ifh->ifd))) != TE_OK )
		{
		ifh_free(ifh); return ecode;
		}

	*ifh_return = ifh;

	return TE_OK;
	}
/*...e*/
/*...slocate_tag:0:*/
TAG *locate_tag(IFD *ifd, short type)
	{
	int i;

	for ( i = 0; i < ifd->n_tags; i++ )
		if ( ifd->tags[i].type == type )
			return &(ifd->tags[i]);
	return NULL;
	}
/*...e*/
/*...snumeric_tag:0:*/
BOOLEAN numeric_tag(TAG *tag)
	{
	short t = tag->data_type;
	return t == D_BYTE  ||
	       t == D_SHORT || t == D_SSHORT ||
	       t == D_LONG  || t == D_SLONG  ;
	}
/*...e*/
/*...svalue_of_tag_n:0:*/
/*
For a numeric tag, return the value of the nth item in it.
Upto the caller to know that tag is signed or unsigned.
*/

long value_of_tag_n(TAG *tag, int n)
	{
	switch ( tag->data_type )
		{
		case D_BYTE:
			{
			unsigned char *p = (unsigned char *) tag->value;
			return (long) (unsigned long) p[n];
			}
		case D_SBYTE:
			{
			signed char *p = (signed char *) tag->value;
			return (long) p[n];
			}
		case D_SHORT:
			{
			unsigned short *p = (unsigned short *) tag->value;
			return (long) (unsigned long) p[n];
			}
		case D_SSHORT:
			{
			signed short *p = (signed short *) tag->value;
			return (long) p[n];
			}
		case D_LONG:
			{
			unsigned long *p = (unsigned long *) tag->value;
			return (long) p[n];
			}
		case D_SLONG:
			{
			signed long *p = (signed long *) tag->value;
			return (long) p[n];
			}
		}
	return 0L;
	}
/*...e*/
/*...svalue_of_tag:0:*/
/*
For a numeric tag, return the value of the 1st value in it.
This is usefull for tags that typically only have 1 value anyway.
*/

long value_of_tag(TAG *tag)
	{
	return value_of_tag_n(tag, 0);
	}
/*...e*/
/*...sfree_ifh:0:*/
void	free_ifh(IFH *ifh)
	{
	IFD	*ifd;

	ifd = ifh->ifd;
	clean_up_ifd(ifd, ifd->n_tags);
	ifh_free(ifh);
	}
/*...e*/
/*...smake_ifh:0:*/
/*
Creates an empty IFH set up for the image.
Also creates an IFD as part of the IFH.
Use add_?_tag() routines to add tags to IFH's IFD.
*/

IFH	*make_ifh(void)
	{
	IFH	*ifh;
	IFD	*ifd;

	if ( (ifh = ifh_malloc()) == NULL )
		return NULL;

	if ( (ifh->ifd = ifd = ifd_malloc()) == NULL )
		{
		ifh_free(ifh);
		return NULL;
		}

	ifh->byte_order = ('I' << 8) + 'I';
	ifh->version_no = 42;

	ifd->n_tags = 0;

	return ifh;
	}
/*...e*/
/*...sadd_byte_tag:0:*/
BOOLEAN	add_byte_tag(IFD *ifd, short type, byte *value, int n)
	{
	byte	*byte_ptr;
	TAG	*tag;

	if ( (byte_ptr = (byte *) malloc(n * sizeof(byte))) == NULL )
		return FALSE;
	if ( (tag = get_tag_slot(type, ifd)) == NULL )
		return FALSE;
	tag->type      = type;
	tag->data_type = D_BYTE;
	tag->length    = (long) n;
	if ( value != NULL )
		memcpy(tag->value = (char *) byte_ptr,
		       value,
		       n * sizeof(byte));
	return TRUE;
	}
/*...e*/
/*...sadd_ascii_tag:0:*/
BOOLEAN	add_ascii_tag(IFD *ifd, short type, char *value)
	{
	char	*ascii_ptr;
	TAG	*tag;
	int	n;

	n = strlen(value) + 1;
	if ( (ascii_ptr = (char *) malloc(n)) == NULL )
		return FALSE;
	if ( (tag = get_tag_slot(type, ifd)) == NULL )
		return FALSE;
	tag->type      = type;
	tag->data_type = D_ASCII;
	tag->length    = (long) n;
	strcpy(tag->value = ascii_ptr, value);
	return TRUE;
	}
/*...e*/
/*...sadd_short_tag:0:*/
BOOLEAN	add_short_tag(IFD *ifd, short type, short *value, int n)
	{
	short	*short_ptr;
	TAG	*tag;

	if ( (short_ptr = (short *) malloc(n * sizeof(short))) == NULL )
		return FALSE;
	if ( (tag = get_tag_slot(type, ifd)) == NULL )
		return FALSE;
	tag->type      = type;
	tag->data_type = D_SHORT;
	tag->length    = (long) n;
	if ( value != NULL )
		memcpy(tag->value = (char *) short_ptr,
		       value,
		       n * sizeof(short));
	return TRUE;
	}
/*...e*/
/*...sadd_long_tag:0:*/
BOOLEAN	add_long_tag(IFD *ifd, short type, long *value, int n)
	{
	long	*long_ptr;
	TAG	*tag;

	if ( (long_ptr = (long *) malloc(n * sizeof(long))) == NULL )
		return FALSE;
	if ( (tag = get_tag_slot(type, ifd)) == NULL )
		return FALSE;
	tag->type      = type;
	tag->data_type = D_LONG;
	tag->length    = (long) n;
	if ( value != NULL )
		memcpy(tag->value = (char *) long_ptr,
		       value,
		       n * sizeof(long));
	return TRUE;
	}
/*...e*/
/*...sadd_rational_tag:0:*/
BOOLEAN	add_rational_tag(IFD *ifd, short type, rational *value, int n)
	{
	rational *rational_ptr;
	TAG	*tag;

	if ( (rational_ptr = (rational *) malloc(n * sizeof(rational))) == NULL )
		return FALSE;
	if ( (tag = get_tag_slot(type, ifd)) == NULL )
		return FALSE;
	tag->type      = type;
	tag->data_type = D_RATIONAL;
	tag->length    = (long) n;
	if ( value != NULL )
		memcpy(tag->value = (char *) rational_ptr,
		       value,
		       n * sizeof(rational));
	return TRUE;
	}
/*...e*/
/*...swrite_ifh_and_ifd:0:*/
/*...spad:0:*/
static BOOLEAN pad(int fd, int n)
	{
	static char padding[] = { 0, 0, 0, 0 };

	return write(fd, padding, n) == n;
	}
/*...e*/
/*...swrite_short:0:*/
static BOOLEAN write_short(int fd, short s)
	{
	byte b[2];

	b[0] = (byte)  (s & 0x00ff);
	b[1] = (byte) ((s & 0xff00) >> 8);

	return write(fd, b, 2) == 2;
	}
/*...e*/
/*...swrite_long:0:*/
static BOOLEAN write_long(int fd, long l)
	{
	byte b[4];

	b[0] = (byte)  (l & 0x000000ffL);
	b[1] = (byte) ((l & 0x0000ff00L) >>  8);
	b[2] = (byte) ((l & 0x00ff0000L) >> 16);
	b[3] = (byte) ((l & 0xff000000L) >> 24);

	return write(fd, b, 4) == 4;
	}
/*...e*/
/*...swrite_rational:0:*/
static BOOLEAN write_rational(int fd, rational *rational)
	{
	return write_long(fd, rational->numerator  ) &&
	       write_long(fd, rational->denominator) ;
	}
/*...e*/
/*...swrite_tag:0:*/
static BOOLEAN write_tag(int fd, TAG *tag, long *offset_upto)
	{
	BOOLEAN	ok;
	int	s, i, n, len;
	long	offset_return_to;

	ok = write_short(fd, tag->type) &&
	     write_short(fd, tag->data_type) &&
	     write_long(fd, tag->length);

	if ( !ok )
		return FALSE;

	/* if we can fit the tag into 4 bytes, do so */
	/* else we will have to allocate some disc space */

	s = sizeof_data_type(tag->data_type);
	n = (int) tag->length;
	len = s * n;

	if ( len > 4 )
		{
		if ( !write_long(fd, *offset_upto) )
			return FALSE;
		offset_return_to = lseek(fd, 0L, SEEK_CUR);
		lseek(fd, *offset_upto, SEEK_SET);
		}		

	/* actually write the tag */

	switch ( tag->data_type )
		{
		case D_BYTE:
		case D_ASCII:
			if  ( write(fd, tag->value, n) != n )
				return FALSE;
			break;
		case D_SHORT:
			{
			short	*short_ptr = (short *) tag->value;

			for ( i = 0; i < n; i++ )
				if ( !write_short(fd, *short_ptr++) )
					return FALSE;
			}
			break;
		case D_LONG:
			{
			long	*long_ptr = (long *) tag->value;

			for ( i = 0; i < n; i++ )
				if ( !write_long(fd, *long_ptr++) )
					return FALSE;
			}
			break;
		case D_RATIONAL:
			{
			rational *rational_ptr = (rational *) tag->value;

			for ( i = 0; i < n; i++ )
				if ( !write_rational(fd, rational_ptr++) )
					return FALSE;
			}
			break;
		}

	if ( len > 4 )
		{
		if ( (*offset_upto = lseek(fd, 0L, SEEK_CUR)) & 1L )
			/* pad to make next offset even */
			{
			if ( !pad(fd, 1) )
				return FALSE;
			(*offset_upto)++;
			}
		lseek(fd, offset_return_to, SEEK_SET);
		}
	else if ( len < 4 )
		if ( !pad(fd, 4 - len) )
			return FALSE;
	return TRUE;
	}
/*...e*/
/*...swrite_ifd:0:*/
/*
Given an IFD, write it out to disc.
Also patch the IFH (which we know will be at the start of the file).
In writing out a tag we may need some more disc space other than
that for the IFD table. This occurs when a field is larger than
4 bytes. What we do is to keep a pointer to the next free space
(after the table) and write_tag() will advance it if it uses any
extra space.
*/

BOOLEAN	write_ifd(int fd, IFD *ifd)
	{
	int	i, n;
	long	offset_upto;

	if ( !write_short(fd, n = ifd->n_tags) )
		return FALSE;

	/* write out tags */

	offset_upto = lseek(fd, 0L, SEEK_CUR) + n * 12L + 4L;
		/* leave space for each tag plus next IFD ptr */

	for ( i = 0; i < n; i++ )
		if ( !write_tag(fd, &(ifd->tags[i]), &offset_upto) )
			return FALSE;

	/* done writing out the IFD, now put null next IFD pointer */

	if ( !write_long(fd, 0L) )
		return FALSE;
	
	lseek(fd, offset_upto, SEEK_SET);

	return TRUE;
	}
/*...e*/

BOOLEAN	write_ifh_and_ifd(IFH *ifh, int fd)
	{
	return write_short(fd, ifh->byte_order) &&
	       write_short(fd, ifh->version_no) &&
	       write_long(fd, 8L) &&
	       write_ifd(fd, ifh->ifd);
	}
/*...e*/
/*...supdate_byte_tag:0:*/
void	update_byte_tag(IFD *ifd, short type, byte *value)
	{
	TAG	*tag;
	int	n;

	tag = locate_tag(ifd, type);
	n = (int) tag->length;
	memcpy(tag->value, value, n * sizeof(byte));
	}
/*...e*/
/*...supdate_ascii_tag:0:*/
void	update_ascii_tag(IFD *ifd, short type, char *value)
	{
	TAG	*tag;
	int	n;

	tag = locate_tag(ifd, type);
	n = (int) tag->length;
	memcpy(tag->value, value, n);
	}
/*...e*/
/*...supdate_short_tag:0:*/
void	update_short_tag(IFD *ifd, short type, short *value)
	{
	TAG	*tag;
	int	n;

	tag = locate_tag(ifd, type);
	n = (int) tag->length;
	memcpy(tag->value, value, n * sizeof(short));
	}
/*...e*/
/*...supdate_long_tag:0:*/
void	update_long_tag(IFD *ifd, short type, long *value)
	{
	TAG	*tag;
	int	n;

	tag = locate_tag(ifd, type);
	n = (int) tag->length;
	memcpy(tag->value, value, n * sizeof(long));
	}
/*...e*/
/*...supdate_rational_tag:0:*/
void	update_rational_tag(IFD *ifd, short type, rational *value)
	{
	TAG	*tag;
	int	n;

	tag = locate_tag(ifd, type);
	n = (int) tag->length;
	memcpy(tag->value, value, n * sizeof(rational));
	}
/*...e*/
/*...supdate_ifd:0:*/
/*
Go back to the IFD, and rewrite it.
*/

BOOLEAN	update_ifd(IFD *ifd, int fd)
	{
	lseek(fd, 8L, SEEK_SET);
	return write_ifd(fd, ifd);
	}
/*...e*/
