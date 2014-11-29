/*

gbmtrunc.h - Interface to error diffusion module

*/

extern void gbm_trunc_line_24(const byte *src, byte *dest, int cx, byte rm, byte gm, byte bm);
extern void gbm_trunc_24(const GBM *gbm, const byte *data24, byte *data24a, byte rm, byte gm, byte bm);

extern void gbm_trunc_pal_6R6G6B(GBMRGB *gbmrgb);
extern void gbm_trunc_line_6R6G6B(const byte *src, byte *dest, int cx);
extern void gbm_trunc_6R6G6B(const GBM *gbm, const byte *data24, byte *data8);

extern void gbm_trunc_pal_7R8G4B(GBMRGB *gbmrgb);
extern void gbm_trunc_line_7R8G4B(const byte *src, byte *dest, int cx);
extern void gbm_trunc_7R8G4B(const GBM *gbm, const byte *data24, byte *data8);

extern void gbm_trunc_pal_VGA(GBMRGB *gbmrgb);
extern void gbm_trunc_line_VGA(const byte *src, byte *dest, int cx);
extern void gbm_trunc_VGA(const GBM *gbm, const byte *data24, byte *data4);

extern void gbm_trunc_pal_8(GBMRGB *gbmrgb);
extern void gbm_trunc_line_8(const byte *src, byte *dest, int cx);
extern void gbm_trunc_8(const GBM *gbm, const byte *data24, byte *data4);

extern void gbm_trunc_pal_4G(GBMRGB *gbmrgb);
extern void gbm_trunc_line_4G(const byte *src, byte *dest, int cx);
extern void gbm_trunc_4G(const GBM *gbm, const byte *data24, byte *data4);

extern void gbm_trunc_pal_BW(GBMRGB *gbmrgb);
extern void gbm_trunc_line_BW(const byte *src, byte *dest, int cx);
extern void gbm_trunc_BW(const GBM *gbm, const byte *data24, byte *data1);
