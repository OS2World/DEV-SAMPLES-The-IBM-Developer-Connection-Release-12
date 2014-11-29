/*

gbmscale.c - Scale bitmap to new size

*/

/*...sincludes:0:*/
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "gbm.h"

/*...vgbm\46\h:0:*/
/*...e*/

/*...sgbm_simple_scale \45\ point sampled:0:*/
/*...ssimple_scale_1:0:*/
static void simple_scale_1(
	const byte *s,
	byte *d, int dw,
	int xs[]
	)
	{
	int sx = 0;
	byte bit, value;

	for ( ; dw >= 8; dw -= 8 )
		{
		for ( value = 0, bit = 0x80; bit > 0; bit >>= 1 )
			{
			if ( s[sx>>3]&(0x80>>(sx&7)) ) value |= bit;
			sx += *xs++;
			}
		*d++ = value;
		}

	if ( dw > 0 )
		{
		for ( value = 0, bit = 0x80; dw-- > 0; bit >>= 1 )
			{
			if ( s[sx>>3]&(0x80>>(sx&7)) ) value |= bit;
			sx += *xs++;
			}
		*d = value;
		}
	}
/*...e*/
/*...ssimple_scale_4:0:*/
static void simple_scale_4(
	const byte *s,
	byte *d, int dw,
	int xs[]
	)
	{
	int sx = 0;
	for ( ;; )
		{
		if ( dw-- == 0 ) return;
		if ( sx&1 ) *d = (s[sx>>1] << 4);
		else        *d = (s[sx>>1]&0xf0);
		sx += *xs++;

		if ( dw-- == 0 ) return;
		if ( sx&1 ) *d++ |= (s[sx>>1]&0x0f);
		else        *d++ |= (s[sx>>1] >> 4);
		sx += *xs++;
		}
	}
/*...e*/
/*...ssimple_scale_8:0:*/
static void simple_scale_8(
	const byte *s,
	byte *d, int dw,
	int xs[]
	)
	{
	while ( dw-- > 0 )
		{
		*d++ = *s;
		s += *xs++;
		}
	}
/*...e*/
/*...ssimple_scale_24:0:*/
static void simple_scale_24(
	const byte *s,
	byte *d, int dw,
	int xs[]
	)
	{
	while ( dw-- > 0 )
		{
		*d++ = s[0];
		*d++ = s[1];
		*d++ = s[2];
		s += ( 3 * *xs++ );
		}
	}
/*...e*/
/*...sfast_simple_scale_1:0:*/
static void fast_simple_scale_1(
	const byte *s,
	byte *d, int dw,
	int xs[]
	)
	{
	xs=xs; /* Suppress warnings */
	memcpy(d, s, (dw+7) >> 3);
	}
/*...e*/
/*...sfast_simple_scale_4:0:*/
static void fast_simple_scale_4(
	const byte *s,
	byte *d, int dw,
	int xs[]
	)
	{
	xs=xs; /* Suppress warnings */
	memcpy(d, s, (dw+1)>>1);
	}
/*...e*/
/*...sfast_simple_scale_8:0:*/
static void fast_simple_scale_8(
	const byte *s,
	byte *d, int dw,
	int xs[]
	)
	{
	xs=xs; /* Suppress warnings */
	memcpy(d, s, dw);
	}
/*...e*/
/*...sfast_simple_scale_24:0:*/
static void fast_simple_scale_24(
	const byte *s,
	byte *d, int dw,
	int xs[]
	)
	{
	xs=xs; /* Suppress warnings */
	memcpy(d, s, dw*3);
	}
/*...e*/

GBM_ERR gbm_simple_scale(
	const byte *s, int sw, int sh,
	      byte *d, int dw, int dh,
	int bpp
	)
	{
	int sst = ( (sw * bpp + 31) / 32 ) * 4;
	int dst = ( (dw * bpp + 31) / 32 ) * 4;
	int *xs, *ys, i;
	void (*scaler)(const byte *s, byte *d, int dw, int xs[]);

	/* Allocate memory for step arrays */

	if ( (xs = malloc((dw+1+dh+1)*sizeof(int))) == NULL )
		return GBM_ERR_MEM;
	ys = xs + (dw+1);

	/* Make mapping to 0..dx from 0..sx (and same for y) */

	for ( i = 0; i <= dw; i++ )
		xs[i] = (i * sw) / dw;

	for ( i = 0; i <= dh; i++ )
		ys[i] = (i * sh) / dh;

	/* Compute step coefficients */

	for ( i = 0; i < dw; i++ )
		xs[i] = xs[i+1] - xs[i];
	
	for ( i = 0; i < dh; i++ )
		ys[i] = ys[i+1] - ys[i];

	/* Pick a scaling routine. Special optimisation to prevent
	   excessive work scaling horizontally if widths are the same.
	   Effectively reduces this code to a memcpy. */

	if ( dw == sw )
		switch ( bpp )
			{
			case 1 : scaler = fast_simple_scale_1 ; break;
			case 4 : scaler = fast_simple_scale_4 ; break;
			case 8 : scaler = fast_simple_scale_8 ; break;
			case 24: scaler = fast_simple_scale_24; break;
			}
	else
		switch ( bpp )
			{
			case 1 : scaler = simple_scale_1 ; break;
			case 4 : scaler = simple_scale_4 ; break;
			case 8 : scaler = simple_scale_8 ; break;
			case 24: scaler = simple_scale_24; break;
			}

	/* Now do guts of scaling */

	while ( dh-- > 0 )
		{
		(*scaler)(s, d, dw, xs);
		d += dst;
		s += (sst * *ys++);
		}

	free(xs);
	return GBM_ERR_OK;
	}
/*...e*/
