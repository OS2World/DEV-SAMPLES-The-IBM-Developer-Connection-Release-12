/*

gbmmir.c - Produce Mirror Image of General Bitmap

*/

/*...sincludes:0:*/
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <malloc.h>
#include "gbm.h"

/*...vgbm\46\h:0:*/
/*...e*/

/*...sgbm_ref_vert:0:*/
BOOLEAN gbm_ref_vert(const GBM *gbm, byte *data)
	{
	int	stride = ( ((gbm->w * gbm->bpp + 31)/32) * 4 );
	byte	*p1 = data;
	byte	*p2 = data + (gbm->h - 1) * stride;
	byte	*p3;

	if ( (p3 = malloc(stride)) == NULL )
		return FALSE;

	for ( ; p1 < p2; p1 += stride, p2 -= stride )
		{
		memcpy(p3, p1, stride);
		memcpy(p1, p2, stride);
		memcpy(p2, p3, stride);
		}

	free(p3);

	return TRUE;
	}
/*...e*/
/*...sgbm_ref_horz:0:*/
/*...sref_horz_24:0:*/
static void ref_horz_24(byte *dst, byte *src, int n)
	{
	dst += n * 3;
	while ( n-- )
		{
		dst -= 3;
		dst[0] = *src++;
		dst[1] = *src++;
		dst[2] = *src++;
		}
	}
/*...e*/
/*...sref_horz_8:0:*/
static void ref_horz_8(byte *dst, byte *src, int n)
	{
	dst += n;
	while ( n-- )
		*(--dst) = *src++;
	}
/*...e*/
/*...sref_horz_4:0:*/
static byte rev4[0x100];

static void table_4(void)
	{
	int	i;

	for ( i = 0; i < 0x100; i++ )
		rev4[i] = (byte) ( ((i & 0x0f) << 4) | ((i & 0xf0) >> 4) );
	}

static void ref_horz_4(byte *dst, byte *src, int n)
	{
	if ( (n & 1) == 0 )
		{
		n /= 2;
		dst += n;
		while ( n-- )
			*(--dst) = rev4[*src++];
		}
	else
		{
		n /= 2;
		src += n;
		while ( n-- )
			{
			*dst    = (byte) (*(src--) & 0xf0);
			*dst++ |= (byte) (* src    & 0x0f);
			}
		*dst = (byte) (*src & 0xf0);
		}
	}
/*...e*/
/*...sref_horz_1:0:*/
static byte rev[0x100]; /* Reverses all bits in a byte */
static byte rev_top[7][0x100]; /* Reverses top N bits of a byte */
static byte rev_bot[7][0x100]; /* Reverses bottom N bits of a byte */
static byte lmask[8] = { 0, 0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe };

static void table_1(void)
	{
	int i;

	for ( i = 0; i < 0x100; i++ )
		{
		int j, p, q, b = 0;

		for ( p = 0x01, q = 0x80; p < 0x100; p <<= 1, q >>= 1 )
			if ( i & p )
				b |= q;
		rev[i] = b;

		for ( j = 1; j < 8; j++ )
			{
			byte l = 0, r = 0;
			byte lm = 0x80, lmr = (byte) (0x80 >> (j - 1));
			byte rm = 0x01, rmr = (byte) (0x01 << (j - 1));
			int k;

			for ( k = 0; k < j; k++ )
				{
				if ( i & (lm >> k) ) l |= (lmr << k);
				if ( i & (rm << k) ) r |= (rmr >> k);
				}
			rev_top[j-1][i] = l;
			rev_bot[j-1][i] = r;
			}
		}
	}

static void ref_horz_1(byte *dst, byte *src, int n)
	{
	int	last = ( n & 7 );

	n >>= 3;
	if ( last == 0 )
		{
		dst += n;
		while ( n-- )
			*(--dst) = rev[*src++];
		}
	else
		{
		byte	*lrev = rev_top[   last -1];
		byte	*rrev = rev_bot[(8-last)-1];
		byte	lm = lmask[last], rm = 0xff - lm;

		src += n;
		while ( n-- )
			{
			*dst    = lrev[*(src--) & lm];
			*dst++ |= rrev[* src    & rm];
			}
		*dst = lrev[*src & lm];
		}
	}
/*...e*/

BOOLEAN gbm_ref_horz(const GBM *gbm, byte *data)
	{
	int	stride = ( ((gbm->w * gbm->bpp + 31)/32) * 4 );
	int	y;
	byte	*p = data;
	byte	*tmp;

	if ( (tmp = malloc(stride)) == NULL )
		return FALSE;

	switch ( gbm->bpp )
		{
		case 24:
			for ( y = 0; y < gbm->h; y++, p += stride )
				{
				ref_horz_24(tmp, p, gbm->w);
				memcpy(p, tmp, stride);
				}
			break;
		case 8:
			for ( y = 0; y < gbm->h; y++, p += stride )
				{
				ref_horz_8(tmp, p, gbm->w);
				memcpy(p, tmp, stride);
				}
			break;
		case 4:
			table_4();
			for ( y = 0; y < gbm->h; y++, p += stride )
				{
				ref_horz_4(tmp, p, gbm->w);
				memcpy(p, tmp, stride);
				}
			break;
		case 1:
			table_1();
			for ( y = 0; y < gbm->h; y++, p += stride )
				{
				ref_horz_1(tmp, p, gbm->w);
				memcpy(p, tmp, stride);
				}
			break;
		}

	free(tmp);

	return TRUE;
	}
/*...e*/
/*...sgbm_transpose:0:*/
void gbm_transpose(const GBM *gbm, const byte *data, byte *data_t)
	{
	int stride   = ((gbm->w * gbm->bpp + 31) / 32) * 4;
	int stride_t = ((gbm->h * gbm->bpp + 31) / 32) * 4;

	switch ( gbm->bpp )
		{
/*...s24:16:*/
case 24:
	{
	int data_step = stride - gbm->w * 3;
	int p_step = stride_t - 2;
	int x, y;

	for ( y = 0; y < gbm->h; y++ )
		{
		byte *p = data_t; data_t += 3;

		for ( x = 0; x < gbm->w; x++ )
			{
			*p++ = *data++;
			*p++ = *data++;
			*p   = *data++;
			p += p_step;
			}
		data += data_step;
		}
	}
	break;
/*...e*/
/*...s8:16:*/
case 8:
	{
	int data_step = stride - gbm->w;
	int x, y;

	for ( y = 0; y < gbm->h; y++ )
		{
		byte *p = data_t++;

		for ( x = 0; x < gbm->w; x++ )
			{
			*p = *data++;
			p += stride_t;
			}
		data += data_step;
		}
	}
	break;
/*...e*/
/*...s4:16:*/
case 4:
	{
	int x, y;

	for ( y = 0; y < gbm->h; y += 2 )
		{
		for ( x = 0; x < gbm->w; x += 2 )
/*...s2x2 transpose to 2x2:40:*/
{
const byte *src = data + y * stride + (x >> 1);
byte ab        = src[0     ];
byte cd        = src[stride];
byte *dst      = data_t + x * stride_t + (y >> 1);
dst[0       ] = (byte) ((ab & 0xf0) | (cd >> 4));
dst[stride_t] = (byte) ((ab << 4) | (cd & 0x0f));
}
/*...e*/
		if ( x < gbm->w )
/*...s1x2 transpose to 2x1:40:*/
{
const byte *src = data + y * stride + (x >> 1);
byte a0      = src[0     ];
byte b0      = src[stride];
byte *dst    = data_t + x * stride_t + (y >> 1);
dst[0     ] = (byte) ((a0 & 0xf0) | (b0 >> 4));
}
/*...e*/
		}
	if ( y < gbm->h )
		{
		for ( x = 0; x < gbm->w; x += 2 )
/*...s2x1 transpose to 1x2:40:*/
{
const byte *src = data + y * stride + (x >> 1);
byte ab        = src[0     ];
byte *dst      = data_t + x * stride_t + (y >> 1);
dst[0       ] = (byte) (ab & 0xf0);
dst[stride_t] = (byte) (ab << 4);
}
/*...e*/
		if ( x < gbm->w )
/*...s1x1 transpose to 1x1:40:*/
{
const byte *src = data + y * stride + (x >> 1);
byte a0      = src[0     ];
byte *dst    = data_t + x * stride_t + (y >> 1);
dst[0     ] = (byte) (a0 & 0xf0);
}
/*...e*/
		}
	}
	break;
/*...e*/
/*...s1:16:*/
case 1:
	{
	int x, y;
	byte xbit, ybit;

	memset(data_t, 0, gbm->w * stride_t);

	ybit = 0x80;
	for ( y = 0; y < gbm->h; y++ )
		{
		xbit = 0x80;
		for ( x = 0; x < gbm->w; x++ )
			{
			const byte *src = data   + y * stride   + (x >> 3);
			      byte *dst = data_t + x * stride_t + (y >> 3); 

			if ( *src & xbit )
				*dst |= ybit;

			if ( (xbit >>= 1) == 0 )
				xbit = 0x80;
			}
		if ( (ybit >>= 1) == 0 )
			ybit = 0x80;
		}
	}
	break;
/*...e*/
		}
	}
/*...e*/
