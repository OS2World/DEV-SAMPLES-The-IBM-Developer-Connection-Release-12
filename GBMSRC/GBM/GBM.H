/*

gbm.h - Generalised Bitmap Module

Data is stored as an array of lines.
Lines are stored with bottom line first, moving upwards.
Each line is an array of pixels, leftmost first.
Lines are padded to be a multiple of a dword long.
Palettised pixels are either a 1 bit, 4 bit, or 8 bit indexes.
Alternately a B, G, R triple in that order is stored.
This format exactly matches the format used by OS/2 and Windows bitmaps.

One notable point: a 1 in a 1bpp image denotes colour 1, as found by
looking at palette entry 1. Data is not inverse when passed to and from GBM.

*/

#ifndef GBM_H
#define	GBM_H

#ifndef BOOLEAN_DEFINED
#define	BOOLEAN_DEFINED
typedef	int BOOLEAN;
#define	TRUE  1
#define	FALSE 0
#endif

#ifndef BASICTYPES_DEFINED
#define	BASICTYPES_DEFINED
typedef unsigned  char  byte;
typedef unsigned short  word;
typedef unsigned  long dword;
#endif

typedef int GBM_ERR;
#define	GBM_ERR_OK		((GBM_ERR) 0)
#define	GBM_ERR_MEM		((GBM_ERR) 1)
#define	GBM_ERR_NOT_SUPP	((GBM_ERR) 2)
#define	GBM_ERR_BAD_OPTION	((GBM_ERR) 3)
#define	GBM_ERR_NOT_FOUND	((GBM_ERR) 4)
#define	GBM_ERR_BAD_MAGIC	((GBM_ERR) 5)
#define	GBM_ERR_BAD_SIZE	((GBM_ERR) 6)
#define	GBM_ERR_READ		((GBM_ERR) 7)
#define	GBM_ERR_WRITE		((GBM_ERR) 8)
#define	GBM_ERR_BAD_ARG		((GBM_ERR) 9)

#define	GBM_FT_R1		0x0001
#define	GBM_FT_R4		0x0002
#define	GBM_FT_R8		0x0004
#define	GBM_FT_R24		0x0008
#define	GBM_FT_W1		0x0010
#define	GBM_FT_W4		0x0020
#define	GBM_FT_W8		0x0040
#define	GBM_FT_W24		0x0080

typedef struct
	{
	char *short_name;		/* Eg: "Targa"                       */
	char *long_name;		/* Eg: "Truevision Targa / Vista"    */
	char *extensions;		/* Eg: "TGA VST"                     */
	int flags;			/* What functionality exists         */
	} GBMFT;

typedef struct { byte r, g, b; } GBMRGB;

#define	PRIV_SIZE 2000

typedef struct
	{
	int w, h, bpp;			/* Bitmap dimensions                 */
	byte priv[PRIV_SIZE];		/* Private internal buffer           */
	} GBM;

#ifndef _GBM_

#ifdef OS2
#define	GBMENTRY _Optlink
#else
#define	GBMENTRY
#endif

GBM_ERR GBMENTRY gbm_init(void);
GBM_ERR GBMENTRY gbm_deinit(void);
GBM_ERR GBMENTRY gbm_query_n_filetypes(int *n_ft);
GBM_ERR GBMENTRY gbm_query_filetype(int ft, GBMFT *gbmft);
GBM_ERR GBMENTRY gbm_guess_filetype(const char *fn, int *ft);
GBM_ERR GBMENTRY gbm_read_header(const char *fn, int fd, int ft, GBM *gbm, const char *opt);
GBM_ERR GBMENTRY gbm_read_palette(int fd, int ft, GBM *gbm, GBMRGB *gbmrgb);
GBM_ERR GBMENTRY gbm_read_data(int fd, int ft, GBM *gbm, byte *data);
GBM_ERR GBMENTRY gbm_write(const char *fn, int fd, int ft, const GBM *gbm, const GBMRGB *gbmrgb, const byte *data, const char *opt);
char *  GBMENTRY gbm_err(GBM_ERR rc);
int     GBMENTRY gbm_version(void);

#ifdef OS2
/*...s_System entrypoints:0:*/
/* For GBM.DLL to be callable from IBM Smalltalk under OS/2, the entrypoints
   must be of _System calling convention. These veneers help out here.
   I can't just change the usual entrypoints because people depend on them.
   For portability, avoid these entrypoints, use the gbm_ ones. */

GBM_ERR _System Gbm_init(void);
GBM_ERR _System Gbm_deinit(void);
GBM_ERR _System Gbm_query_n_filetypes(int *n_ft);
GBM_ERR	_System Gbm_guess_filetype(const char *fn, int *ft);
GBM_ERR _System Gbm_query_filetype(int ft, GBMFT *gbmft);
GBM_ERR _System Gbm_read_header(const char *fn, int fd, int ft, GBM *gbm, const char *opt);
GBM_ERR _System Gbm_read_palette(int fd, int ft, GBM *gbm, GBMRGB *gbmrgb);
GBM_ERR _System Gbm_read_data(int fd, int ft, GBM *gbm, byte *data);
GBM_ERR _System Gbm_write(const char *fn, int fd, int ft, const GBM *gbm, const GBMRGB *gbmrgb, const byte *data, const char *opt);
char *  _System Gbm_err(GBM_ERR rc);
int     _System Gbm_version(void);
/*...e*/
#endif

#endif

#endif
