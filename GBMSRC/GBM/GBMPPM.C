/*

gbmppm.c - Poskanzers PPM format

Reads and writes 24 bit RGB.

*/

/*...sincludes:0:*/
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>
#ifdef AIX
#include <unistd.h>
#else
#include <io.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gbm.h"
#include "gbmhelp.h"

/*...vgbm\46\h:0:*/
/*...vgbmhelp\46\h:0:*/
/*...e*/

/*...suseful:0:*/
#define	low_byte(w)	((byte)  ((w)&0x00ff)    )
#define	high_byte(w)	((byte) (((w)&0xff00)>>8))
#define	make_word(a,b)	(((word)a) + (((word)b) << 8))
/*...e*/
/*...sposk stuff:0:*/
/*...sread_byte:0:*/
static byte read_byte(int fd)
	{
	byte b = 0;

	read(fd, (char *) &b, 1);
	return  b;
	}
/*...e*/
/*...sread_char:0:*/
static char read_char(int fd)
	{
	char c;

	while ( (c = read_byte(fd)) == '#' )
		/* Discard to end of line */
		while ( (c = read_byte(fd)) != '\n' )
			;
	return c;
	}
/*...e*/
/*...sread_num:0:*/
static int read_num(int fd)
	{
	char c;
	int num;

	while ( isspace(c = read_char(fd)) )
		;
	num = c - '0';
	while ( isdigit(c = read_char(fd)) )
		num = num * 10 + (c - '0');
	return num;
	}
/*...e*/
/*...sread_posk_header:0:*/
static void read_posk_header(int fd, int *h1, int *h2, int *w, int *h, int *m)
	{
	lseek(fd, 0L, SEEK_SET);
	*h1 = read_byte(fd);
	*h2 = read_byte(fd);
	*w  = read_num(fd);
	*h  = read_num(fd);
	*m  = read_num(fd);
	}
/*...e*/
/*...e*/

static GBMFT ppm_gbmft =
	{
	"Pixmap",
	"Portable Pixel-map (binary P6 type)",
	"PPM",
	GBM_FT_R24|
	GBM_FT_W24,
	};

#define	GBM_ERR_PPM_BAD_M	((GBM_ERR) 200)

/*...srgb_bgr:0:*/
static void rgb_bgr(const byte *p, byte *q, int n)
	{
	while ( n-- )
		{
		byte	r = *p++;
		byte	g = *p++;
		byte	b = *p++;

		*q++ = b;
		*q++ = g;
		*q++ = r;
		}
	}
/*...e*/

/*...sppm_qft:0:*/
GBM_ERR ppm_qft(GBMFT *gbmft)
	{
	*gbmft = ppm_gbmft;
	return GBM_ERR_OK;
	}
/*...e*/
/*...sppm_rhdr:0:*/
GBM_ERR ppm_rhdr(const char *fn, int fd, GBM *gbm, const char *opt)
	{
	int	h1, h2, w, h, m;

	fn=fn; opt=opt; /* Suppress 'unref arg' compiler warnings */

	read_posk_header(fd, &h1, &h2, &w, &h, &m);
	if ( h1 != 'P' || h2 != '6' )
		return GBM_ERR_BAD_MAGIC;

	if ( w <= 0 || h <= 0 )
		return GBM_ERR_BAD_SIZE;

	if ( m <= 1 || m >= 0x100 )
		return GBM_ERR_PPM_BAD_M;

	gbm->w   = w;
	gbm->h   = h;
	gbm->bpp = 24;

	return GBM_ERR_OK;
	}
/*...e*/
/*...sppm_rpal:0:*/
GBM_ERR ppm_rpal(int fd, GBM *gbm, GBMRGB *gbmrgb)
	{
	fd=fd; gbm=gbm; gbmrgb=gbmrgb; /* Suppress 'unref arg' compiler warnings */

	return GBM_ERR_OK;
	}
/*...e*/
/*...sppm_rdata:0:*/
GBM_ERR ppm_rdata(int fd, GBM *gbm, byte *data)
	{
	int	h1, h2, w, h, m, i, stride;
	byte	*p;

	read_posk_header(fd, &h1, &h2, &w, &h, &m);

	stride = ((gbm->w * 3 + 3) & ~3);
	p = data + ((gbm->h - 1) * stride);
	for ( i = gbm->h - 1; i >= 0; i-- )
		{
		read(fd, p, gbm->w * 3);
		rgb_bgr(p, p, gbm->w);
		p -= stride;
		}
	return GBM_ERR_OK;
	}
/*...e*/
/*...sppm_w:0:*/
GBM_ERR ppm_w(const char *fn, int fd, const GBM *gbm, const GBMRGB *gbmrgb, const byte *data, const char *opt)
	{
	char s[100+1];
	int i, stride;
	const byte *p;
	byte *linebuf;

	fn=fn; gbmrgb=gbmrgb; opt=opt; /* Suppress 'unref arg' compiler warnings */

	if ( gbm->bpp != 24 )
		return GBM_ERR_NOT_SUPP;

	if ( (linebuf = malloc(gbm->w * 3)) == NULL )
		return GBM_ERR_MEM;

	sprintf(s, "P6\n%d %d 255\n", gbm->w, gbm->h);
	write(fd, s, strlen(s));

	stride = ((gbm->w * 3 + 3) & ~3);
	p = data + ((gbm->h - 1) * stride);
	for ( i = gbm->h - 1; i >= 0; i-- )
		{
		rgb_bgr(p, linebuf, gbm->w);
		write(fd, linebuf, gbm->w * 3);
		p -= stride;
		}

	free(linebuf);

	return GBM_ERR_OK;
	}
/*...e*/
/*...sppm_err:0:*/
char *ppm_err(GBM_ERR rc)
	{
	switch ( (int) rc )
		{
		case GBM_ERR_PPM_BAD_M:
			return "bad maximum pixel intensity";
		}
	return NULL;
	}
/*...e*/
