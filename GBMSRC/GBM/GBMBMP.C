/*

gbmbmp.c - OS/2 1.1, 1.2, 2.0 and Windows 3.0 support

Reads and writes any OS/2 1.x bitmap.
Will also read uncompressed, RLE4 and RLE8 Windows 3.x bitmaps too.
There are horrific file structure alignment considerations hence each
word,dword is read individually.
Input options: index=# (default: 0)

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

#ifndef min
#define	min(a,b)	(((a)<(b))?(a):(b))
#endif
/*...e*/

/*...suseful:0:*/
#define	low_byte(w)	((byte)  ((w)&0x00ff)    )
#define	high_byte(w)	((byte) (((w)&0xff00)>>8))
#define	make_word(a,b)	(((word)a) + (((word)b) << 8))

/*...sread_word:0:*/
static BOOLEAN read_word(int fd, word *w)
	{
	byte low = 0, high = 0;

	read(fd, (char *) &low, 1);
	read(fd, (char *) &high, 1);
	*w = (word) (low + ((word) high << 8));
	return TRUE;
	}
/*...e*/
/*...sread_dword:0:*/
static BOOLEAN read_dword(int fd, dword *d)
	{
	word low, high;

	read_word(fd, &low);
	read_word(fd, &high);
	*d = low + ((dword) high << 16);
	return TRUE;
	}
/*...e*/
/*...swrite_word:0:*/
static BOOLEAN write_word(int fd, word w)
	{
	byte	low  = (byte) w;
	byte	high = (byte) (w >> 8);

	write(fd, &low, 1);
	write(fd, &high, 1);
	return TRUE;
	}
/*...e*/
/*...swrite_dword:0:*/
static BOOLEAN write_dword(int fd, dword d)
	{
	write_word(fd, (word) d);
	write_word(fd, (word) (d >> 16));
	return TRUE;
	}
/*...e*/
/*...e*/

static GBMFT bmp_gbmft =
	{
	"Bitmap",
	"OS/2 1.1, 1.2, 2.0 / Windows 3.0 bitmap",
	"BMP VGA BGA RLE DIB RL4 RL8",
	GBM_FT_R1|GBM_FT_R4|GBM_FT_R8|GBM_FT_R24|
	GBM_FT_W1|GBM_FT_W4|GBM_FT_W8|GBM_FT_W24,
	};

#define	GBM_ERR_BMP_PLANES	((GBM_ERR) 300)
#define	GBM_ERR_BMP_BITCOUNT	((GBM_ERR) 301)
#define	GBM_ERR_BMP_CBFIX	((GBM_ERR) 302)
#define	GBM_ERR_BMP_COMP	((GBM_ERR) 303)
#define	GBM_ERR_BMP_OFFSET	((GBM_ERR) 304)

typedef struct
	{
	dword base;
	BOOLEAN windows;
	dword cbFix;
	dword ulCompression;
	dword cclrUsed;
	dword offBits;
	BOOLEAN inv, invb;
	} BMP_PRIV;

#define	BFT_BMAP	0x4d42
#define	BFT_BITMAPARRAY	0x4142
#define	BCA_UNCOMP	0x00000000L
#define	BCA_RLE8	0x00000001L
#define	BCA_RLE4	0x00000002L
#define	BCA_HUFFFMAN1D	0x00000003L
#define	BCA_RLE24	0x00000004L
#define	MSWCC_EOL	0
#define	MSWCC_EOB	1
#define	MSWCC_DELTA	2

/*...sinvert:0:*/
static void invert(byte *buffer, unsigned count)
	{
	while ( count-- )
		*buffer++ ^= (byte) 0xff;
	}
/*...e*/
/*...sswap_pal:0:*/
static void swap_pal(GBMRGB *gbmrgb)
	{
	GBMRGB tmp = gbmrgb[0];
	gbmrgb[0] = gbmrgb[1];
	gbmrgb[1] = tmp;
	}
/*...e*/

/*...sbmp_qft:0:*/
GBM_ERR bmp_qft(GBMFT *gbmft)
	{
	*gbmft = bmp_gbmft;
	return GBM_ERR_OK;
	}
/*...e*/
/*...sbmp_rhdr:0:*/
GBM_ERR bmp_rhdr(const char *fn, int fd, GBM *gbm, const char *opt)
	{
	word usType, xHotspot, yHotspot;
	dword cbSize, offBits, cbFix;
	BMP_PRIV *bmp_priv = (BMP_PRIV *) gbm->priv;
	bmp_priv->inv  = ( gbm_find_word(opt, "inv" ) != NULL );
	bmp_priv->invb = ( gbm_find_word(opt, "invb") != NULL );

	fn=fn; /* Suppress 'unref arg' compiler warnings */

	lseek(fd, 0L, SEEK_SET);

	read_word(fd, &usType);
	if ( usType == BFT_BITMAPARRAY )
/*...shandle bitmap arrays:16:*/
{
const char *index;
int	i;

if ( (index = gbm_find_word_prefix(opt, "index=")) != NULL )
	sscanf(index + 6, "%d", &i);
else
	i = 0;

while ( i-- > 0 )
	{
	dword	cbSize2, offNext;

	read_dword(fd, &cbSize2);
	read_dword(fd, &offNext);
	if ( offNext == 0L )
		return GBM_ERR_BMP_OFFSET;
	lseek(fd, (long) offNext, SEEK_SET);
	read_word(fd, &usType);
	if ( usType != BFT_BITMAPARRAY )
		return GBM_ERR_BAD_MAGIC;
	}
lseek(fd, 4L + 4L + 2L + 2L, SEEK_CUR);
read_word(fd, &usType);
}
/*...e*/

	if ( usType != BFT_BMAP )
		return GBM_ERR_BAD_MAGIC;

	bmp_priv->base = lseek(fd, 0L, SEEK_CUR) - 2L;

	read_dword(fd, &cbSize);
	read_word(fd, &xHotspot);
	read_word(fd, &yHotspot);
	read_dword(fd, &offBits);
	read_dword(fd, &cbFix);

	bmp_priv->offBits = offBits;

	if ( cbFix == 12 )
/*...sOS\47\2 1\46\1\44\ 1\46\2:16:*/
/* OS/2 1.x uncompressed bitmap */
{
word cx, cy, cPlanes, cBitCount;

read_word(fd, &cx);
read_word(fd, &cy);
read_word(fd, &cPlanes);
read_word(fd, &cBitCount);

if ( cx == 0 || cy == 0 )
	return GBM_ERR_BAD_SIZE;
if ( cPlanes != 1 )
	return GBM_ERR_BMP_PLANES;
if ( cBitCount != 1 && cBitCount != 4 && cBitCount != 8 && cBitCount != 24 )
	return GBM_ERR_BMP_BITCOUNT;

gbm->w   = (int) cx;
gbm->h   = (int) cy;
gbm->bpp = (int) cBitCount;

bmp_priv->windows = FALSE;
}
/*...e*/
	else if ( cbFix >= 16 && cbFix <= 64 &&
	          ((cbFix & 3) == 0 || cbFix == 42 || cbFix == 46) )
/*...sOS\47\2 2\46\0 and Windows 3\46\0:16:*/
{
word cPlanes, cBitCount, usUnits, usReserved, usRecording, usRendering;
dword ulWidth, ulHeight, ulCompression;
dword ulSizeImage, ulXPelsPerMeter, ulYPelsPerMeter;
dword cclrUsed, cclrImportant, cSize1, cSize2, ulColorEncoding, ulIdentifier;

read_dword(fd, &ulWidth);
read_dword(fd, &ulHeight);
read_word(fd, &cPlanes);
read_word(fd, &cBitCount);
if ( cbFix > 16 )
	read_dword(fd, &ulCompression);
else
	ulCompression = BCA_UNCOMP;
if ( cbFix > 20 )
	read_dword(fd, &ulSizeImage);
if ( cbFix > 24 )
	read_dword(fd, &ulXPelsPerMeter);
if ( cbFix > 28 )
	read_dword(fd, &ulYPelsPerMeter);
if ( cbFix > 32 )
	read_dword(fd, &cclrUsed);
else
	cclrUsed = ( 1 << cBitCount );
if ( cBitCount != 24 && cclrUsed == 0 )
	cclrUsed = ( 1 << cBitCount );
if ( cbFix > 36 )
	read_dword(fd, &cclrImportant);
if ( cbFix > 40 )
	read_word(fd, &usUnits);
if ( cbFix > 42 )
	read_word(fd, &usReserved);
if ( cbFix > 44 )
	read_word(fd, &usRecording);
if ( cbFix > 46 )
	read_word(fd, &usRendering);
if ( cbFix > 48 )
	read_dword(fd, &cSize1);
if ( cbFix > 52 )
	read_dword(fd, &cSize2);
if ( cbFix > 56 )
	read_dword(fd, &ulColorEncoding);
if ( cbFix > 60 )
	read_dword(fd, &ulIdentifier);

if ( ulWidth == 0L || ulHeight == 0L )
	return GBM_ERR_BAD_SIZE;
if ( cPlanes != 1 )
	return GBM_ERR_BMP_PLANES;
if ( cBitCount != 1 && cBitCount != 4 && cBitCount != 8 && cBitCount != 24 )
	return GBM_ERR_BMP_BITCOUNT;

gbm->w   = (int) ulWidth;
gbm->h   = (int) ulHeight;
gbm->bpp = (int) cBitCount;

bmp_priv->windows       = TRUE;
bmp_priv->cbFix         = cbFix;
bmp_priv->ulCompression = ulCompression;
bmp_priv->cclrUsed      = cclrUsed;
}
/*...e*/
	else
		return GBM_ERR_BMP_CBFIX;

	return GBM_ERR_OK;
	}
/*...e*/
/*...sbmp_rpal:0:*/
GBM_ERR bmp_rpal(int fd, GBM *gbm, GBMRGB *gbmrgb)
	{
	BMP_PRIV *bmp_priv = (BMP_PRIV *) gbm->priv;

	if ( gbm->bpp != 24 )
		{
		int i;
		byte b[4];

		if ( bmp_priv->windows )
/*...sOS\47\2 2\46\0 and Windows 3\46\0:24:*/
{
lseek(fd, bmp_priv->base + 14L + bmp_priv->cbFix, SEEK_SET);
for ( i = 0; i < (int) bmp_priv->cclrUsed; i++ )
	{
	read(fd, b, 4);
	gbmrgb[i].b = b[0];
	gbmrgb[i].g = b[1];
	gbmrgb[i].r = b[2];
	}
}
/*...e*/
		else
/*...sOS\47\2 1\46\1\44\ 1\46\2:24:*/
{
lseek(fd, bmp_priv->base + 26L, SEEK_SET);
for ( i = 0; i < (1 << gbm->bpp); i++ )
	{
	read(fd, b, 3);
	gbmrgb[i].b = b[0];
	gbmrgb[i].g = b[1];
	gbmrgb[i].r = b[2];
	}
}
/*...e*/
		}

	if ( gbm->bpp == 1 && !bmp_priv->inv )
		swap_pal(gbmrgb);

	return GBM_ERR_OK;
	}
/*...e*/
/*...sbmp_rdata:0:*/
GBM_ERR bmp_rdata(int fd, GBM *gbm, byte *data)
	{
	BMP_PRIV *bmp_priv = (BMP_PRIV *) gbm->priv;
	int cLinesWorth = ((gbm->bpp * gbm->w + 31) / 32) * 4;

	if ( bmp_priv->windows )
/*...sOS\47\2 2\46\0 and Windows 3\46\0:16:*/
{
lseek(fd, bmp_priv->offBits, SEEK_SET);

switch ( (int) bmp_priv->ulCompression )
	{
/*...sBCA_UNCOMP:24:*/
case BCA_UNCOMP:
	read(fd, data, gbm->h * cLinesWorth);
	break;
/*...e*/
/*...sBCA_RLE8:24:*/
case BCA_RLE8:
	{
	AHEAD *ahead;
	int stride = ((gbm->w + 3) & ~3);
	int x = 0, y = 0;
	BOOLEAN eof8 = FALSE;

	if ( (ahead = gbm_create_ahead(fd)) == NULL )
		return GBM_ERR_MEM;

	while ( !eof8 )
		{
		byte c = (byte) gbm_read_ahead(ahead);
		byte d = (byte) gbm_read_ahead(ahead);

		if ( c )
			{
			memset(data, d, c);
			x += c;
			data += c;
			}
		else
			switch ( d )
				{
/*...sMSWCC_EOL:56:*/
case MSWCC_EOL:
	{
	int	to_eol = stride - x;

	memset(data, 0, to_eol);
	x = 0; y++;
	data += to_eol;
	}
	break;
/*...e*/
/*...sMSWCC_EOB:56:*/
case MSWCC_EOB:
	if ( y < gbm->h )
		{
		int	to_eol = stride - x;

		memset(data, 0, to_eol);
		x = 0; y++;
		data += to_eol;
		while ( y < gbm->h )
			{
			memset(data, 0, stride);
			data += stride;
			y++;
			}
		}
	eof8 = TRUE;
	break;
/*...e*/
/*...sMSWCC_DELTA:56:*/
case MSWCC_DELTA:
	{
	byte dx = (byte) gbm_read_ahead(ahead);
	byte dy = (byte) gbm_read_ahead(ahead);
	int fill = dx + dy * stride;

	x += dx; y += dy;

	memset(data, 0, fill);
	data += fill;
	}
	break;
/*...e*/
/*...sdefault:56:*/
default:
	{
	int	n = (int) d;

	while ( n-- > 0 )
		*data++ = (byte) gbm_read_ahead(ahead);
	x += d;
	if ( d & 1 )
		gbm_read_ahead(ahead); /* Align */
	}
	break;
/*...e*/
				}
		}

	gbm_destroy_ahead(ahead);
	}
	break;
/*...e*/
/*...sBCA_RLE4:24:*/
case BCA_RLE4:
	{
	AHEAD *ahead;
	int x = 0, y = 0;
	BOOLEAN eof4 = FALSE;
	int inx = 0;

	if ( (ahead = gbm_create_ahead(fd)) == NULL )
		return GBM_ERR_MEM;

	memset(data, 0, gbm->h * cLinesWorth);

	while ( !eof4 )
		{
		byte c = (byte) gbm_read_ahead(ahead);
		byte d = (byte) gbm_read_ahead(ahead);

		if ( c )
			{
			byte h, l;
			int i;

			if ( x & 1 )
				{
				h = (byte) (d >> 4); l = (byte) (d << 4);
				}
			else
				{
				h = (byte) (d & 0xf0); l = (byte) (d & 0x0f);
				}
			for ( i = 0; i < (int) c; i++, x++ )
				if ( x & 1 )
					data[inx++] |= l;
				else
					data[inx]   |= h;					
			}
		else
			switch ( d )
				{
/*...sMSWCC_EOL:56:*/
case MSWCC_EOL:
	for ( ; x < gbm->w; x++ )
		if ( x & 1 )
			inx++;
	x = 0; y++;
	inx = ((inx + 3) & ~3); /* Align output */
	break;
/*...e*/
/*...sMSWCC_EOB:56:*/
case MSWCC_EOB:
	eof4 = TRUE;
	break;
/*...e*/
/*...sMSWCC_DELTA:56:*/
case MSWCC_DELTA:
	{
	byte dx = (byte) gbm_read_ahead(ahead);
	byte dy = (byte) gbm_read_ahead(ahead);

	y   += dy;
	inx += dy * cLinesWorth;

	if ( dx > 0 )
		{
		if ( x & 1 )
			{
			inx++;
			x++;
			dx--;
			}

		inx += (dx / 2);
		x += dx;
		}
		
	}
	break;
/*...e*/
/*...sdefault:56:*/
default:
	{
	int	i, nr = 0;

	if ( x & 1 )
		{
		for ( i = 0; i < (int) d; i += 2 )
			{
			byte b = (byte) gbm_read_ahead(ahead);

			data[inx++] |= (b >> 4);
			data[inx  ] |= (b << 4);
			nr++;
			}
		if ( i < (int) d )
			{
			data[inx++] |= ((byte) gbm_read_ahead(ahead) >> 4);
			nr++;
			}
		}

	else
		{
		for ( i = 0; i < (int) d; i += 2 )
			{
			data[inx++] = (byte) gbm_read_ahead(ahead);
			nr++;
			}
		if ( i < (int) d )
			{
			data[inx] = (byte) gbm_read_ahead(ahead);
			nr++;
			}
		}
	x += d;

	if ( nr & 1 )
		gbm_read_ahead(ahead); /* Align input stream to next word */
	}
	break;
/*...e*/
				}
		}

	gbm_destroy_ahead(ahead);
	}
	break;
/*...e*/
/*...sdefault:24:*/
default:
	return GBM_ERR_BMP_COMP;
/*...e*/
	}
}
/*...e*/
	else
/*...sOS\47\2 1\46\1\44\ 1\46\2:16:*/
{
lseek(fd, bmp_priv->offBits, SEEK_SET);
read(fd, data, cLinesWorth * gbm->h);
}
/*...e*/

	if ( bmp_priv->invb )
		invert(data, cLinesWorth * gbm->h);

	return GBM_ERR_OK;
	}
/*...e*/
/*...sbmp_w:0:*/
/*...swrite_inv:0:*/
static unsigned write_inv(int fd, const char *buffer, unsigned count)
	{
	char small_buf[1024];
	unsigned so_far = 0, this_go, written;

	while ( so_far < count )
		{
		this_go = min(count - so_far, 1024);
		memcpy(small_buf, buffer + so_far, this_go);
		invert(small_buf, this_go);
		if ( (written = write(fd, small_buf, this_go)) != this_go )
			return so_far + written;
		so_far += written;
		}

	return so_far;
	}
/*...e*/

GBM_ERR bmp_w(const char *fn, int fd, const GBM *gbm, const GBMRGB *gbmrgb, const byte *data, const char *opt)
	{
	BOOLEAN	pm11 = ( gbm_find_word(opt, "1.1" ) != NULL );
	BOOLEAN	win  = ( gbm_find_word(opt, "win" ) != NULL ||
	                 gbm_find_word(opt, "2.0" ) != NULL );
	BOOLEAN inv  = ( gbm_find_word(opt, "inv" ) != NULL );
	BOOLEAN invb = ( gbm_find_word(opt, "invb") != NULL );
	int cRGB;
	GBMRGB gbmrgb_1bpp[2];

	if ( pm11 && win )
		return GBM_ERR_BAD_OPTION;

	fn=fn; /* Suppress 'unref arg' compiler warning */

	cRGB = ( (1 << gbm->bpp) & 0x1ff );
		/* 1->2, 4->16, 8->256, 24->0 */

	if ( cRGB == 2 )
/*...shandle messy 1bpp case:16:*/
{
/*
The palette entries inside a 1bpp PM bitmap are not honored, or handled
correctly by most programs. Current thinking is that they have no actual
meaning. Under OS/2 PM, bitmap 1's re fg and 0's are bg, and it is the job of
the displayer to pick fg and bg. We will pick fg=black, bg=white in the bitmap
file we save. If we do not write black and white, we find that most programs
will incorrectly honor these entries giving unpredicatable (and often black on
a black background!) results.
*/

gbmrgb_1bpp[0].r = gbmrgb_1bpp[0].g = gbmrgb_1bpp[0].b = 0xff;
gbmrgb_1bpp[1].r = gbmrgb_1bpp[1].g = gbmrgb_1bpp[1].b = 0x00;

/*
We observe these values must be the wrong way around to keep most PM
programs happy, such as WorkPlace Shell WPFolder backgrounds.
*/

if ( !inv )
	swap_pal(gbmrgb_1bpp);

gbmrgb = gbmrgb_1bpp;
}
/*...e*/

	if ( pm11 )
/*...sOS\47\2 1\46\1:16:*/
{
word usType, xHotspot, yHotspot, cx, cy, cPlanes, cBitCount;
dword cbSize, offBits, cbFix;
int cLinesWorth, i, total, actual;

usType      = BFT_BMAP;
xHotspot    = 0;
yHotspot    = 0;
cbFix       = 12L;
cx          = gbm->w;
cy          = gbm->h;
cPlanes     = 1;
cBitCount   = gbm->bpp;
cLinesWorth = (((cBitCount * cx + 31) / 32) * cPlanes) * 4;
offBits     = 26L + cRGB * 3L;
cbSize      = offBits + (dword) cy * (dword) cLinesWorth;

write_word(fd, usType);
write_dword(fd, cbSize);
write_word(fd, xHotspot);
write_word(fd, yHotspot);
write_dword(fd, offBits);
write_dword(fd, cbFix);
write_word(fd, cx);
write_word(fd, cy);
write_word(fd, cPlanes);
write_word(fd, cBitCount);

for ( i = 0; i < cRGB; i++ )
	{
	byte b[3];

	b[0] = gbmrgb[i].b;
	b[1] = gbmrgb[i].g;
	b[2] = gbmrgb[i].r;
	if ( write(fd, b, 3) != 3 )
		return GBM_ERR_WRITE;
	}

total = gbm->h * cLinesWorth;
if ( invb )
	actual = write_inv(fd, data, total);
else
	actual = write(fd, data, total);
if ( actual != total )
	return GBM_ERR_WRITE;
}
/*...e*/
	else
/*...sOS\47\2 2\46\0 and Windows 3\46\0:16:*/
{
word usType, xHotspot, yHotspot, cPlanes, cBitCount;
dword cx, cy, cbSize, offBits, cbFix, ulCompression, cbImage;
dword cxResolution, cyResolution, cclrUsed, cclrImportant;
int cLinesWorth, i, total, actual;

usType      = BFT_BMAP;
xHotspot    = 0;
yHotspot    = 0;
cbFix       = 40L;
cx          = gbm->w;
cy          = gbm->h;
cPlanes     = 1;
cBitCount   = gbm->bpp;
cLinesWorth = (((cBitCount * (int) cx + 31) / 32) * cPlanes) * 4;
offBits     = 54L + cRGB * 4L;
cbSize      = offBits + (dword) cy * (dword) cLinesWorth;

ulCompression = BCA_UNCOMP;
cbImage       = cLinesWorth * gbm->h;
cxResolution  = 0;
cyResolution  = 0;
cclrUsed      = 0;
cclrImportant = 0;

write_word(fd, usType);
write_dword(fd, cbSize);
write_word(fd, xHotspot);
write_word(fd, yHotspot);
write_dword(fd, offBits);

write_dword(fd, cbFix);
write_dword(fd, cx);
write_dword(fd, cy);
write_word(fd, cPlanes);
write_word(fd, cBitCount);
write_dword(fd, ulCompression);
write_dword(fd, cbImage);
write_dword(fd, cxResolution);
write_dword(fd, cyResolution);
write_dword(fd, cclrUsed);
write_dword(fd, cclrImportant);

for ( i = 0; i < cRGB; i++ )
	{
	byte b[4];

	b[0] = gbmrgb[i].b;
	b[1] = gbmrgb[i].g;
	b[2] = gbmrgb[i].r;
	b[3] = 0;
	if ( write(fd, b, 4) != 4 )
		return GBM_ERR_WRITE;
	}

total = gbm->h * cLinesWorth;
if ( invb )
	actual = write_inv(fd, data, total);
else
	actual = write(fd, data, total);
if ( actual != total )
	return GBM_ERR_WRITE;
}
/*...e*/

	return GBM_ERR_OK;
	}
/*...e*/
/*...sbmp_err:0:*/
char *bmp_err(GBM_ERR rc)
	{
	switch ( (int) rc )
		{
		case GBM_ERR_BMP_PLANES:
			return "number of bitmap planes is not 1";
		case GBM_ERR_BMP_BITCOUNT:
			return "bit count not 1, 4, 8 or 24";
		case GBM_ERR_BMP_CBFIX:
			return "cbFix bad";
		case GBM_ERR_BMP_COMP:
			return "compression type not uncompressed, RLE4 or RLE8";
		case GBM_ERR_BMP_OFFSET:
			return "less bitmaps in file than index requested";
		}
	return NULL;
	}
/*...e*/
