/*

gbmlogo.c - Make VRAM files for sending to MAKELOGO

*/

/*...sincludes:0:*/
#define INCL_BASE
#include <os2.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gbm.h"
#include "gbmtrunc.h"
#include "gbmerr.h"
#include "gbmht.h"
/*...e*/

static char progname[] = "gbmlogo";
static char sccs_id[] = "@(#)Make VRAM files for sending to MAKELOGO  16/3/95";

/*...ssizes:0:*/
#define	SCN_W 640
#define	SCN_H 480
#define	LOGO_W SCN_W
#define	LOGO_H ((SCN_H*5)/6)
/*...e*/
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

	fprintf(stderr, "usage: %s [-e] [-hN] filename.ext{,opt}\n", progname);
	fprintf(stderr, "flags: -e             error-diffuse to RGBI\n");
	fprintf(stderr, "       -hN            halftone to RGBI\n");
	fprintf(stderr, "                      N is a halftoning algorithm number (default 0)\n");
	fprintf(stderr, "                      -e and -h can't be used together\n");
	fprintf(stderr, "       filename.ext   %dx%d bitmap\n", LOGO_W, LOGO_H);
	fprintf(stderr, "                      ext's are used to deduce desired bitmap file format\n");

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

	exit(1);
	}
/*...e*/
/*...smain:0:*/
static char errbuf[100+1];

/*...sread_bitmap:0:*/
static char *read_bitmap(
	const char *fn, const char *opt,
	GBM *gbm, GBMRGB gbmrgb[], byte **data
	)
	{
	int ft, fd, stride;
	GBM_ERR rc;

	if ( gbm_guess_filetype(fn, &ft) != GBM_ERR_OK )
		{
		sprintf(errbuf, "can't guess filetype of %s", fn);
		return errbuf;
		}

	if ( (fd = open(fn, O_RDONLY | O_BINARY)) == -1 )
		{
		sprintf(errbuf, "can't open %s for reading", fn);
		return "can't open bitmap file";
		}

	if ( (rc = gbm_read_header(fn, fd, ft, gbm, opt)) != GBM_ERR_OK )
		{
		close(fd);
		sprintf(errbuf, "can't read header of %s: %s", fn, gbm_err(rc));
		return errbuf;
		}

	if ( (rc = gbm_read_palette(fd, ft, gbm, gbmrgb)) != GBM_ERR_OK )
		{
		close(fd);
		sprintf(errbuf, "can't read palette of %s: %s", fn, gbm_err(rc));
		return errbuf;
		}

	stride = (gbm->w*gbm->bpp+31)/32*4;
	if ( ((*data) = malloc(stride*gbm->h)) == NULL )
		{
		close(fd);
		sprintf(errbuf, "can't allocate memory to hold data from %s", fn);
		return errbuf;
		}

	if ( (rc = gbm_read_data(fd, ft, gbm, (*data))) != GBM_ERR_OK )
		{
		close(fd);
		sprintf(errbuf, "can't read data from %s", fn);
		return errbuf;
		}

	close(fd);
	return NULL;
	}
/*...e*/
/*...sto_24_bit:0:*/
static BOOLEAN to_24_bit(GBM *gbm, GBMRGB *gbmrgb, BYTE **ppbData)
	{
	int stride = ((gbm -> w * gbm -> bpp + 31)/32) * 4;
	int new_stride = ((gbm -> w * 3 + 3) & ~3);
	int bytes, y;
	byte *pbDataNew;

	if ( gbm->bpp == 24 )
		return TRUE;

	bytes = new_stride * gbm -> h;
	if ( (pbDataNew = malloc(bytes)) == NULL )
		return FALSE;

	for ( y = 0; y < gbm -> h; y++ )
		{
		byte *src = *ppbData + y * stride;
		byte *dest = pbDataNew + y * new_stride;
		int x;

		switch ( gbm -> bpp )
			{
/*...s1:24:*/
case 1:
	{
	byte	c;

	for ( x = 0; x < gbm -> w; x++ )
		{
		if ( (x & 7) == 0 )
			c = *src++;
		else
			c <<= 1;

		*dest++ = gbmrgb [(c & 0x80) != 0].b;
		*dest++ = gbmrgb [(c & 0x80) != 0].g;
		*dest++ = gbmrgb [(c & 0x80) != 0].r;
		}
	}
	break;
/*...e*/
/*...s4:24:*/
case 4:
	for ( x = 0; x + 1 < gbm -> w; x += 2 )
		{
		byte	c = *src++;

		*dest++ = gbmrgb [c >> 4].b;
		*dest++ = gbmrgb [c >> 4].g;
		*dest++ = gbmrgb [c >> 4].r;
		*dest++ = gbmrgb [c & 15].b;
		*dest++ = gbmrgb [c & 15].g;
		*dest++ = gbmrgb [c & 15].r;
		}

	if ( x < gbm -> w )
		{
		byte	c = *src;

		*dest++ = gbmrgb [c >> 4].b;
		*dest++ = gbmrgb [c >> 4].g;
		*dest++ = gbmrgb [c >> 4].r;
		}
	break;
/*...e*/
/*...s8:24:*/
case 8:
	for ( x = 0; x < gbm -> w; x++ )
		{
		byte	c = *src++;

		*dest++ = gbmrgb [c].b;
		*dest++ = gbmrgb [c].g;
		*dest++ = gbmrgb [c].r;
		}
	break;
/*...e*/
			}
		}
	free(*ppbData);
	*ppbData = pbDataNew;
	gbm->bpp = 24;
	return TRUE;
	}
/*...e*/
/*...sgenerate_file:0:*/
/* data points to 480*5/6 lines, each of 640 4 bit pixels.
   We are interested in the plane'th bits of each nibble. */

static char *generate_file(const char *fn, int plane, const byte *data)
	{
	FILE *fp;
	int x, y;
	byte planemask = (0x11<<plane), sidemask, *p, *q;

	if ( (fp = fopen(fn, "w")) == NULL )
		{
		sprintf(errbuf, "can't create %s", fn);
		return errbuf;
		}

	if ( (p = q = malloc(LOGO_W*SCN_H/8)) == NULL )
		{
		fclose(fp); remove(fn);
		return "out of memory";
		}

	memset(p, 0, LOGO_W*SCN_H/8);

	data += (LOGO_H-1) * (LOGO_W/2);
	for ( y = 0; y < LOGO_H; y++, data -= LOGO_W/2, q += LOGO_W/8 )
		{
		for ( x = 0, sidemask = 0xf0; x < LOGO_W; x++, sidemask ^= 0xff )
			if ( data[x>>1] & sidemask & planemask )
				q[x>>3] |= (0x80>>(x&7));
		}

	for ( y = 0; y < 0x9600; y += 0x10 )
		{
		fprintf(fp, "%%%%%08x ", 0xa0000+y);
		for ( x = 0; x < 0x10; x += 2 )
			fprintf(fp, " %02x%02x", p[y+x+1], p[y+x]);
		fprintf(fp, "\n");
		}
	fclose(fp);
	}
/*...e*/

int main(int argc, char *argv[])
	{
	char fn[500+1], *opt, *err;
	BOOLEAN	errdiff = FALSE, halftone = FALSE;
	int htmode = 0, plane, i;
	GBM gbm; GBMRGB gbmrgb[0x100]; byte *data;

/*...sparse any options:8:*/
for ( i = 1; i < argc; i++ )
	{
	if ( argv[i][0] != '-' )
		break;
	switch ( argv[i][1] )
		{
		case 'e':	errdiff = TRUE;
				break;
		case 'h':	halftone = TRUE;
				if ( argv[i][2] != '\0' && isdigit(argv[i][2]) )
					htmode = argv[i][2] - '0';
				break;
		default:	usage();	break;
		}
	}

if ( errdiff && halftone )
	fatal("error-diffusion and halftoning can't both be done at once");

if ( i == argc )
	usage();

strcpy(fn, argv[i++]);
if ( (opt = strchr(fn, ',')) != NULL )
	*opt++ = '\0';
else
	opt = "";
/*...e*/

	gbm_init();

	if ( (err = read_bitmap(fn, opt, &gbm, gbmrgb, &data)) != NULL )
		fatal(err);

	if ( gbm.w != LOGO_W || gbm.h != LOGO_H )
		fatal("%s is %dx%d not %dx%d, as required", fn, gbm.w, gbm.h, LOGO_W, LOGO_H);

	if ( !to_24_bit(&gbm, gbmrgb, &data) )
		fatal("can't expand %s to 24 bpp, prior to mapping to RGBI", fn);

/*...smap down to RGBI \40\same as VGA palette\41\:8:*/
if ( errdiff )
	{
	gbm_errdiff_pal_VGA(gbmrgb);
	if ( !gbm_errdiff_VGA(&gbm, data, data) )
		fatal("can't error-diffuse to RGBI, out of memory");
	}
else if ( halftone )
	{
	gbm_ht_pal_VGA(gbmrgb);
	switch ( htmode )
		{
		default:
		case 0: gbm_ht_VGA_3x3(&gbm, data, data); break;
		case 1: gbm_ht_VGA_2x2(&gbm, data, data); break;
		}
	}
else
	{
	gbm_trunc_pal_VGA(gbmrgb);
	gbm_trunc_VGA(&gbm, data, data);
	}
gbm.bpp = 4;
/*...e*/
/*...swrite out the IBGR planes:8:*/
/* Palette is ordered IBGR, vram files are B G R I */

for ( plane = 0; plane < 4; plane++ )
	{
	char fn_o[20+1];
	int oplane;
	sprintf(fn_o, "vram%d.dat", plane);
	switch ( plane )
		{
		case 0:	oplane = 2; break;
		case 1:	oplane = 1; break;
		case 2:	oplane = 0; break;
		case 3:	oplane = 3; break;
		}
	if ( (err = generate_file(fn_o, oplane, data)) != NULL )
		{
		while ( --plane >= 0 )
			{
			sprintf(fn_o, "vram%d.dat", plane);
			remove(fn_o);
			}
		fatal(err);
		}
	}
/*...e*/

	gbm_deinit();

	return 0;
	}
/*...e*/
