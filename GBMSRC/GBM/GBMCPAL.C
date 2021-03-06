/*

gbmcpal.c - Map to Common Palette

*/

/*...sincludes:0:*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
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
#ifndef O_BINARY
#define	O_BINARY	0
#endif
#include "gbm.h"
#include "gbmhist.h"
#include "gbmmcut.h"

/*...vgbm\46\h:0:*/
/*...vgbmhist\46\h:0:*/
/*...vgbmmcut\46\h:0:*/
/*...e*/

static char progname[] = "gbmcpal";
static char sccs_id[] = "@(#)Map to common palette  1/1/95";

/*...sfatal:0:*/
static void fatal(const char *fmt, ...)
	{
	va_list	vars;
	char s[256+1];

	va_start(vars, fmt);
	vsprintf(s, fmt, vars);
	va_end(vars);
	fprintf(stderr, "%s: %s\n", progname, s);
	exit(1);
	}
/*...e*/
/*...susage:0:*/
static void usage(void)
	{
	int ft, n_ft;

	fprintf(stderr, "usage: %s [-m map] [-v] n1 n2 n3 ifspec{,opt} ofspec{,opt}\n", progname);
	fprintf(stderr, "flags: -m map         mapping to perform (default freq6:6:6:256)\n");
	fprintf(stderr, "                      freqR:G:B:N       map all bitmaps to same palette, worked\n");
	fprintf(stderr, "                                        out using frequency of use histogram\n");
	fprintf(stderr, "                      mcutN             map all bitmaps to same palette, worked\n");
	fprintf(stderr, "                                        out using median cut algorithm\n");
	fprintf(stderr, "                      rofreqR:G:B:N:N2  map each bitmap to frequency palette,\n");
	fprintf(stderr, "                                        reordered to minimise differences\n");
	fprintf(stderr, "                                        between successive bitmaps\n");
	fprintf(stderr, "                      romcutN:N2        map each bitmap to median cut palette,\n");
	fprintf(stderr, "                                        reordered to minimise differences\n");
	fprintf(stderr, "                                        between successive bitmaps\n");
	fprintf(stderr, "                                        R,G,B are bits of red, green and blue\n");
	fprintf(stderr, "                                        to keep, N is number of unique colours,\n");
	fprintf(stderr, "                                        N2 is extra palette entries\n");
	fprintf(stderr, "       -v             verbose mode\n");
	fprintf(stderr, "       n1 n2 n3       for ( f=n1; f<n2; f+=n3 )\n");
	fprintf(stderr, "       ifspec           printf(ifspec, f);\n");
	fprintf(stderr, "       ofspec           printf(ofspec, f);\n");
	fprintf(stderr, "                      filespecs are of the form fn.ext\n");
	fprintf(stderr, "                      ext's are used to deduce desired bitmap file formats\n");

	gbm_init();
	gbm_query_n_filetypes(&n_ft);
	for ( ft = 0; ft < n_ft; ft++ )
		{
		GBMFT gbmft;

		gbm_query_filetype(ft, &gbmft);
		fprintf(stderr, "                      %s when ext in [%s]\n",
			gbmft.short_name, gbmft.extensions);
		}
	gbm_deinit();

	fprintf(stderr, "       opt's          bitmap format specific options\n");
	fprintf(stderr, "   eg: %s -m mcut256 0 100 1 24bit%%03d.bmp 8bit%%03d.bmp\n", progname);

	exit(1);
	}
/*...e*/
/*...ssame:0:*/
static BOOLEAN same(const char *s1, const char *s2, int n)
	{
	for ( ; n--; s1++, s2++ )
		if ( tolower(*s1) != tolower(*s2) )
			return FALSE;
	return TRUE;
	}
/*...e*/
/*...smain:0:*/
#define	CVT_FREQ   0
#define	CVT_MCUT   1
#define	CVT_ROFREQ 2
#define	CVT_ROMCUT 3

static BOOLEAN verbose = FALSE;

/*...sget_masks:0:*/
/*
Returns TRUE if a set of masks given at map.
Also sets *rm, *gm, *bm from these.
Else returns FALSE.
*/

static byte mask[] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };

static BOOLEAN get_masks(char *map, byte *rm, byte *gm, byte *bm)
	{
	if ( map[0] <  '0' || map[0] > '8' ||
	     map[1] != ':' ||
	     map[2] <  '0' || map[2] > '8' ||
	     map[3] != ':' ||
	     map[4] <  '0' || map[4] > '8' )
		return FALSE;

	*rm = mask[map[0] - '0'];
	*gm = mask[map[2] - '0'];
	*bm = mask[map[4] - '0'];
	return TRUE;
	}
/*...e*/
/*...salloc_mem:0:*/
static byte *alloc_mem(const GBM *gbm)
	{
	int stride, bytes;
	byte *p;

	stride = ( ((gbm->w * gbm->bpp + 31)/32) * 4 );
	bytes = stride * gbm->h;
	if ( (p = malloc(bytes)) == NULL )
		fatal("out of memory allocating %d bytes", bytes);

	return p;
	}
/*...e*/
/*...sread_bitmap:0:*/
static void read_bitmap(
	const char *fn, const char *opt,
	GBM *gbm, GBMRGB gbmrgb[], byte **data
	)
	{
	int ft, fd;
	GBM_ERR rc;

	if ( verbose )
		{
		if ( *opt != '\0' )
			printf("Reading %s,%s\n", fn, opt);
		else
			printf("Reading %s\n", fn);
		}

	if ( gbm_guess_filetype(fn, &ft) != GBM_ERR_OK )
		fatal("can't guess bitmap file format for %s", fn);

	if ( (fd = open(fn, O_RDONLY | O_BINARY)) == -1 )
		fatal("can't open %s", fn);

	if ( (rc = gbm_read_header(fn, fd, ft, gbm, opt)) != GBM_ERR_OK )
		{
		close(fd);
		fatal("can't read header of %s: %s", fn, gbm_err(rc));
		}

	if ( (rc = gbm_read_palette(fd, ft, gbm, gbmrgb)) != GBM_ERR_OK )
		{
		close(fd);
		fatal("can't read palette of %s: %s", fn, gbm_err(rc));
		}

	(*data) = alloc_mem(gbm);

	if ( (rc = gbm_read_data(fd, ft, gbm, (*data))) != GBM_ERR_OK )
		{
		free(*data);
		close(fd);
		fatal("can't read bitmap data of %s: %s", fn, gbm_err(rc));
		}

	close(fd);
	}
/*...e*/
/*...sread_bitmap_24:0:*/
/*...sexpand_to_24bit:0:*/
static void expand_to_24bit(GBM *gbm, GBMRGB *gbmrgb, byte **data)
	{
	int	stride = ((gbm->w * gbm->bpp + 31)/32) * 4;
	int	new_stride = ((gbm->w * 3 + 3) & ~3);
	int	bytes, y;
	byte	*new_data;

	if ( gbm->bpp == 24 )
		return;

	bytes = new_stride * gbm->h;
	if ( (new_data = malloc(bytes)) == NULL )
		fatal("out of memory allocating %d bytes", bytes);

	for ( y = 0; y < gbm->h; y++ )
		{
		byte	*src = *data + y * stride;
		byte	*dest = new_data + y * new_stride;
		int	x;

		switch ( gbm->bpp )
			{
/*...s1:24:*/
case 1:
	{
	byte	c;

	for ( x = 0; x < gbm->w; x++ )
		{
		if ( (x & 7) == 0 )
			c = *src++;
		else
			c <<= 1;

		*dest++ = gbmrgb[c >> 7].b;
		*dest++ = gbmrgb[c >> 7].g;
		*dest++ = gbmrgb[c >> 7].r;
		}
	}
	break;
/*...e*/
/*...s4:24:*/
case 4:
	for ( x = 0; x + 1 < gbm->w; x += 2 )
		{
		byte	c = *src++;

		*dest++ = gbmrgb[c >> 4].b;
		*dest++ = gbmrgb[c >> 4].g;
		*dest++ = gbmrgb[c >> 4].r;
		*dest++ = gbmrgb[c & 15].b;
		*dest++ = gbmrgb[c & 15].g;
		*dest++ = gbmrgb[c & 15].r;
		}

	if ( x < gbm->w )
		{
		byte	c = *src;

		*dest++ = gbmrgb[c >> 4].b;
		*dest++ = gbmrgb[c >> 4].g;
		*dest++ = gbmrgb[c >> 4].r;
		}
	break;
/*...e*/
/*...s8:24:*/
case 8:
	for ( x = 0; x < gbm->w; x++ )
		{
		byte	c = *src++;

		*dest++ = gbmrgb[c].b;
		*dest++ = gbmrgb[c].g;
		*dest++ = gbmrgb[c].r;
		}
	break;
/*...e*/
			}
		}
	free(*data);
	*data = new_data;
	gbm->bpp = 24;
	}
/*...e*/

static void read_bitmap_24(
	const char *fn, const char *opt,
	GBM *gbm, byte **data
	)
	{
	GBMRGB gbmrgb[0x100];
	read_bitmap(fn, opt, gbm, gbmrgb, data);
	if ( gbm->bpp != 24 )
		{
		if ( verbose )
			printf("Expanding to 24 bpp\n");
		expand_to_24bit(gbm, gbmrgb, data);
		}
	}
/*...e*/
/*...sread_bitmap_24_f:0:*/
static void read_bitmap_24_f(
	const char *fn, int f, const char *opt,
	GBM *gbm, byte **data
	)
	{
	char fn_f[500+1];
	sprintf(fn_f, fn, f);
	read_bitmap_24(fn_f, opt, gbm, data);
	}
/*...e*/
/*...swrite_bitmap:0:*/
static void write_bitmap(
	const char *fn, const char *opt,
	const GBM *gbm, const GBMRGB gbmrgb[], const byte *data
	)
	{
	int ft, fd, flag;
	GBM_ERR rc;
	GBMFT gbmft;

	if ( verbose )
		{
		if ( *opt != '\0' )
			printf("Writing %s,%s\n", fn, opt);
		else
			printf("Writing %s\n", fn);
		}

	if ( gbm_guess_filetype(fn, &ft) != GBM_ERR_OK )
		fatal("can't guess bitmap file format for %s", fn);

	gbm_query_filetype(ft, &gbmft);
	switch ( gbm->bpp )
		{
		case 24:	flag = GBM_FT_W24;	break;
		case 8:		flag = GBM_FT_W8;	break;
		case 4:		flag = GBM_FT_W4;	break;
		case 1:		flag = GBM_FT_W1;	break;
		}

	if ( (gbmft.flags & flag) == 0 )
		fatal("output bitmap format %s does not support writing %d bpp data",
			gbmft.short_name, gbm->bpp);

	if ( (fd = open(fn, O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, S_IREAD | S_IWRITE)) == -1 )
		fatal("can't create %s", fn);

	if ( (rc = gbm_write(fn, fd, ft, gbm, gbmrgb, data, opt)) != GBM_ERR_OK )
		{
		close(fd);
		remove(fn);
		fatal("can't write %s: %s", fn, gbm_err(rc));
		}

	close(fd);
	}
/*...e*/
/*...swrite_bitmap_f:0:*/
static void write_bitmap_f(
	const char *fn, int f, const char *opt,
	const GBM *gbm, const GBMRGB gbmrgb[], const byte *data
	)
	{
	char fn_f[500+1];
	sprintf(fn_f, fn, f);
	write_bitmap(fn_f, opt, gbm, gbmrgb, data);
	}
/*...e*/
/*...sfreq_map:0:*/
static void freq_map(
	int first, int last, int step,
	const char *fn_src, const char *opt_src,
	const char *fn_dst, const char *opt_dst,
	int ncols, byte rm, byte gm, byte bm
	)
	{
	int f;
	GBMHIST *hist;
	GBMRGB gbmrgb[0x100];

	for ( ;; )
		{
		if ( verbose )
			printf("Attempting to build histogram data with masks 0x%02x 0x%02x 0x%02x\n",
				rm, gm, bm);
		if ( (hist = gbm_create_hist(rm, gm, bm)) == NULL )
			fatal("can't create histogram data");
		for ( f = first; f < last; f += step )
			{
			GBM gbm; byte *data;
			BOOLEAN ok;
			read_bitmap_24_f(fn_src, f, opt_src, &gbm, &data);
			ok = gbm_add_to_hist(hist, &gbm, data);
			free(data);
			if ( !ok )
				{
				if ( verbose )
					printf("Too many colours\n");
				break;
				}
			}
		if ( f == last )
			break;

		gbm_delete_hist(hist);

		if ( gm > rm )
			gm <<= 1;
		else if ( rm > bm )
			rm <<= 1;
		else
			bm <<= 1;
		}

	if ( verbose )
		printf("Working out %d colour palette, based on histogram data\n", ncols);

	gbm_pal_hist(hist, gbmrgb, ncols);

	if ( verbose )
		printf("Converting files to new optimal palette\n");

	for ( f = first; f < last; f += step )
		{
		GBM gbm; byte *data, *data8;
		read_bitmap_24_f(fn_src, f, opt_src, &gbm, &data);
		gbm.bpp = 8;
		data8 = alloc_mem(&gbm);
		if ( verbose )
			printf("Mapping to optimal palette\n");
		gbm_map_hist(hist, &gbm, data, data8);
		free(data);
		write_bitmap_f(fn_dst, f, opt_dst, &gbm, gbmrgb, data8);
		free(data8);
		}			

	gbm_delete_hist(hist);
	}
/*...e*/
/*...smcut_map:0:*/
static void mcut_map(
	int first, int last, int step,
	const char *fn_src, const char *opt_src,
	const char *fn_dst, const char *opt_dst,
	int ncols
	)
	{
	int f;
	GBMMCUT *mcut;
	GBMRGB gbmrgb[0x100];

	if ( verbose )
		printf("Attempting to build median cut statistics\n");

	if ( (mcut = gbm_create_mcut()) == NULL )
		fatal("can't create median cut data");
	for ( f = first; f < last; f += step )
		{
		GBM gbm; byte *data;
		read_bitmap_24_f(fn_src, f, opt_src, &gbm, &data);
		gbm_add_to_mcut(mcut, &gbm, data);
		free(data);
		}

	if ( verbose )
		printf("Working out %d colour palette, based on median cut statistics\n", ncols);

	gbm_pal_mcut(mcut, gbmrgb, ncols);

	if ( verbose )
		printf("Converting files to new optimal palette\n");

	for ( f = first; f < last; f += step )
		{
		GBM gbm; byte *data, *data8;
		read_bitmap_24_f(fn_src, f, opt_src, &gbm, &data);
		gbm.bpp = 8;
		data8 = alloc_mem(&gbm);
		if ( verbose )
			printf("Mapping to optimal palette\n");
		gbm_map_mcut(mcut, &gbm, data, data8);
		free(data);
		write_bitmap_f(fn_dst, f, opt_dst, &gbm, gbmrgb, data8);
		free(data8);
		}			

	gbm_delete_mcut(mcut);
	}
/*...e*/
/*...srofreq_map\44\ romcut_map:0:*/
/*

This code has been written primarily to support crude animation schemes.
Imagine an animation which is a series of (palette, bitmap-bits) pairs.

If the displayer is unable to change both the palette and bitmap-bits in
the vertical retrace interval, or if ping-pong double buffering is not
available, there will be a short time where the new palette has been set, but
the old bits are still on display. This causes a very disturbing flicker.
(Similarly, this is true if the displaying program sets the bitmap-bits, and
then the palette).

This code reorders the new palette, so that its entries are close to the
previous palette. Old palette entries used by the most pixels are considered
for this matching process first. Hence only small areas of the image flicker.

eg: old palette           = { red   , green      , light green, black       }
    new palette           = { green , orange     , dark blue  , light green }
    reordered new palette = { orange, light green, green      , dark blue   }

    Clearly red->orange is less offensive than red->green etc..

*/

/*...sro_map:0:*/
/*

The palettes returned using gbm_hist/mcut are sorted with most used first.
(Index through MAP to get palette entries in PAL in order of frequency).

Map first image to palette PAL and bits BITS.
For i = 0 to ncols-1
  MAP[i] = i
Write out image PAL and BITS
For each subsequent image
  Map image to PAL' and BITS'
  For i = 0 to ncols-1
    MAP'[i] = -1
  For i = 0 to ncols-1
    j = index of closest entry to PAL[MAP[i]] in PAL',
      with MAP'[j] = -1
    MAP'[j] = MAP[i];
  For i = 0 to ncols-1
    PAL[MAP'[i]] = PAL'[i]
  For each pixel p
    BITS'[p] = MAP'[BITS'[p]]
  Write out PAL and BITS'
  BITS = BITS'
  MAP = MAP'

*/

/*...scalc_mapP:0:*/
/*
For each entry in the old palette, starting with the most used, find the
closest 'unclaimed' entry in the new palette, and 'claim' it.
Thus, if you iterate though mapP[0..ncols-1] you get palette indexes
of close entries in the old palette.
*/

static void calc_mapP(
	const GBMRGB gbmrgb [], const word map [],
	      GBMRGB gbmrgbP[],       word mapP[],
	int dists[],
	int ncols
	)
	{
	int i;

	if ( verbose )
		printf("Reordering palette to cause least flicker\n");

	for ( i = 0; i < ncols; i++ )
		mapP[i] = (word) 0xffff;

	/* Go through old palette entries, in descending freq order */
	for ( i = 0; i < ncols; i++ )
		{
		const GBMRGB *p = &(gbmrgb[map[i]]);
		int mindist = 255*255*3 + 1;
		int j, minj;
		/* Find closest entry in new palette */
		for ( j = 0; j < ncols; j++ )
			{
			int dr = (int) ( (unsigned int) p->r - (unsigned int) gbmrgbP[j].r );
			int dg = (int) ( (unsigned int) p->g - (unsigned int) gbmrgbP[j].g );
			int db = (int) ( (unsigned int) p->b - (unsigned int) gbmrgbP[j].b );
			int dist = dr*dr + dg*dg + db*db;
			if ( dist < mindist && mapP[j] == (word) 0xffff )
				{
				minj = j;
				mindist = dist;
				}
			}
		dists[minj] = mindist;
		mapP[minj] = map[i];
		}
	}
/*...e*/

static void ro_map(
	int first, int last, int step,
	const char *fn_src, const char *opt_src,
	const char *fn_dst, const char *opt_dst,
	int ncols, int ncolsextra, byte rm, byte gm, byte bm,
	void (*get)(
		const char *fn, int f, const char *opt,
		int ncols, byte rm, byte gm, byte bm,
		GBM *gbm, GBMRGB gbmrgb[], byte **data8
		)
	)
	{
	GBM gbm; GBMRGB gbmrgb[0x100]; byte *data8;
	word map[0x100], *extra = &(map[ncols]);
	int i, f;

	if ( first >= last )
		return;

	(*get)(fn_src, first, opt_src, ncols, rm, gm, bm, &gbm, gbmrgb, &data8);
	for ( i = 0; i < ncols+ncolsextra; i++ )
		map[i] = (word) i;

	write_bitmap_f(fn_dst, first, opt_dst, &gbm, gbmrgb, data8);

	for ( f = first + step; f < last; f += step )
		{
		GBM gbmP; GBMRGB gbmrgbP[0x100]; byte *data8P, *p;
		word mapP[0x100]; int dists[0x100];
		int x, y, stride;

		(*get)(fn_src, f, opt_src, ncols, rm, gm, bm, &gbmP, gbmrgbP, &data8P);
		calc_mapP(gbmrgb, map, gbmrgbP, mapP, dists, ncols);

/*...shandle ncolsextra worst matches specially:16:*/
{
int j;

/* Find the ncolsextra worst palette changes */

for ( i = 0; i < ncolsextra; i++ )
	{
	int jmax, maxdist = -1;
	for ( j = 0; j < ncols; j++ )
		if ( dists[j] != -1 && dists[j] > maxdist )
			{
			jmax = j;
			maxdist = dists[j];
			}
	dists[jmax] = -1;
	}

/* Use extra palette entries for these instead */

for ( i = 0, j = 0; i < ncolsextra; i++, j++ )
	{
	word t;
	while ( dists[j] != -1 )
		j++;
	t = mapP[j];		/* This is a bad palette entry */
	mapP[j] = extra[i];	/* Use extra entry instead */
	extra[i] = t;		/* So bad one is fair game next loop */
	}
}
/*...e*/

		for ( i = 0; i < ncols; i++ )
			gbmrgb[mapP[i]] = gbmrgbP[i];

		stride = ((gbmP.w+3)&~3);
		for ( y = 0, p = data8P; y < gbmP.h; y++, p += stride )
			for ( x = 0; x < gbmP.w; x++ )
				p[x] = (byte) mapP[p[x]];

		write_bitmap_f(fn_dst, f, opt_dst, &gbmP, gbmrgb, data8P);

		gbm = gbmP;
		free(data8);
		data8 = data8P;
		memcpy(map, mapP, ncols * sizeof(word));
		}

	free(data8);
	}
/*...e*/
/*...srofreq_map:0:*/
/*...sget_and_hist:0:*/
static void get_and_hist(
	const char *fn, int f, const char *opt,
	int ncols, byte rm, byte gm, byte bm,
	GBM *gbm, GBMRGB gbmrgb[], byte **data8
	)
	{
	byte *data24;
	read_bitmap_24_f(fn, f, opt, gbm, &data24);
	gbm->bpp = 8;
	(*data8) = alloc_mem(gbm);
	if ( !gbm_hist(gbm, data24, gbmrgb, *data8, ncols, rm, gm, bm) )
		fatal("can't compute histogram");
	free(data24);
	}
/*...e*/

static void rofreq_map(
	int first, int last, int step,
	const char *fn_src, const char *opt_src,
	const char *fn_dst, const char *opt_dst,
	int ncols, int ncolsextra, byte rm, byte gm, byte bm
	)
	{
	ro_map(
		first, last, step,
		fn_src, opt_src, fn_dst, opt_dst,
		ncols, ncolsextra, rm, gm, bm,
		get_and_hist
		);
	}
/*...e*/
/*...sromcut_map:0:*/
/*...sget_and_mcut:0:*/
static void get_and_mcut(
	const char *fn, int f, const char *opt,
	int ncols, byte rm, byte gm, byte bm,
	GBM *gbm, GBMRGB gbmrgb[], byte **data8
	)
	{
	byte *data24;
	rm=rm; gm=gm; bm=bm; /* Suppress 'unused arg warning' */
	read_bitmap_24_f(fn, f, opt, gbm, &data24);
	gbm->bpp = 8;
	(*data8) = alloc_mem(gbm);
	if ( !gbm_mcut(gbm, data24, gbmrgb, *data8, ncols) )
		fatal("can't perform median-cut");
	free(data24);
	}
/*...e*/

static void romcut_map(
	int first, int last, int step,
	const char *fn_src, const char *opt_src,
	const char *fn_dst, const char *opt_dst,
	int ncols, int ncolsextra
	)
	{
	ro_map(
		first, last, step,
		fn_src, opt_src, fn_dst, opt_dst,
		ncols, ncolsextra, 0, 0, 0,
		get_and_mcut
		);
	}
/*...e*/
/*...e*/

int main(int argc, char *argv[])
	{
	char *map = "freq6:6:6:256";
	char fn_src[500+1], fn_dst[500+1], *opt_src, *opt_dst;
	int i, m, ncols, ncolsextra, first, last, step;
	byte rm, gm, bm;

/*...sprocess command line options:8:*/
for ( i = 1; i < argc; i++ )
	{
	if ( argv[i][0] != '-' )
		break;
	switch ( argv[i][1] )
		{
		case 'm':	if ( ++i == argc )
					fatal("expected map argument");
				map = argv[i];
				break;
		case 'v':	verbose = TRUE;
				break;
		default:	usage();
				break;
		}
	}
/*...e*/
/*...sframes and filenames etc\46\:8:*/
if ( i == argc )
	usage();
sscanf(argv[i++], "%d", &first);

if ( i == argc )
	usage();
sscanf(argv[i++], "%d", &last);

if ( i == argc )
	usage();
sscanf(argv[i++], "%d", &step);

if ( i == argc )
	usage();
strcpy(fn_src, argv[i++]);
strcpy(fn_dst, ( i == argc ) ? fn_src : argv[i++]);
if ( i < argc )
	usage();

if ( (opt_src = strchr(fn_src, ',')) != NULL )
	*opt_src++ = '\0';
else
	opt_src = "";

if ( (opt_dst = strchr(fn_dst, ',')) != NULL )
	*opt_dst++ = '\0';
else
	opt_dst = "";
/*...e*/
/*...sdeduce mapping and bits per pixel etc\46\:8:*/
if ( same(map, "freq", 4) )
	{
	m = CVT_FREQ;
	if ( !get_masks(map + 4, &rm, &gm, &bm) )
		fatal("freqR:G:B:N has bad/missing R:G:B");
	if ( map[9] != ':' )
		fatal("freqR:G:B:N has bad/missing :N");
	sscanf(map + 10, "%i", &ncols);
	if ( ncols < 1 || ncols > 256 )
		fatal("freqR:G:B:N N number between 1 and 256 required");
	}
else if ( same(map, "mcut", 4) )
	{
	m = CVT_MCUT;
	sscanf(map+4, "%i", &ncols);
	if ( ncols < 1 || ncols > 256 )
		fatal("mcutN N number between 1 and 256 required");
	}
else if ( same(map, "rofreq", 6) )
	{
	m = CVT_ROFREQ;
	if ( !get_masks(map+6, &rm, &gm, &bm) )
		fatal("rofreqR:G:B:N has bad/missing R:G:B");
	if ( map[11] != ':' )
		fatal("rofreqR:G:B:N has bad/missing :N:N2");
	sscanf(map + 12, "%i:%i", &ncols, &ncolsextra);
	if ( ncols+ncolsextra < 1 || ncols+ncolsextra > 256 )
		fatal("rofreqR:G:B:N:N2 N+N2 must be between 1 and 256");
	}
else if ( same(map, "romcut", 6) )
	{
	m = CVT_ROMCUT;
	sscanf(map+6, "%i:%i", &ncols, &ncolsextra);
	if ( ncols+ncolsextra < 1 || ncols+ncolsextra > 256 )
		fatal("mcutN:N2 N+N2 must be between 1 and 256");
	}
else
	fatal("unrecognised mapping %s", map);
/*...e*/

	gbm_init();

	switch ( m )
		{
		case CVT_FREQ:
			freq_map(
				first, last, step,
				fn_src, opt_src, fn_dst, opt_dst,
				ncols, rm, gm, bm);
			break;
		case CVT_MCUT:
			mcut_map(
				first, last, step,
				fn_src, opt_src, fn_dst, opt_dst,
				ncols);
			break;
		case CVT_ROFREQ:
			rofreq_map(
				first, last, step,
				fn_src, opt_src, fn_dst, opt_dst,
				ncols, ncolsextra, rm, gm, bm);
			break;
		case CVT_ROMCUT:
			romcut_map(
				first, last, step,
				fn_src, opt_src, fn_dst, opt_dst,
				ncols, ncolsextra);
			break;
		}

	gbm_deinit();

	return 0;
	}
/*...e*/
