/*

gbmht.h - Interface to Halftoner

*/

extern void gbm_ht_24_2x2(const GBM *gbm, const byte *data24, byte *data24a, byte rm, byte gm, byte bm);

extern void gbm_ht_pal_6R6G6B(GBMRGB *gbmrgb);
extern void gbm_ht_6R6G6B_2x2(const GBM *gbm, const byte *data24, byte *data8);

extern void gbm_ht_pal_7R8G4B(GBMRGB *gbmrgb);
extern void gbm_ht_7R8G4B_2x2(const GBM *gbm, const byte *data24, byte *data8);

extern void gbm_ht_pal_VGA(GBMRGB *gbmrgb);
extern void gbm_ht_VGA_2x2(const GBM *gbm, const byte *src24, byte *dest4);
extern void gbm_ht_VGA_3x3(const GBM *gbm, const byte *src24, byte *dest4);

extern void gbm_ht_pal_8(GBMRGB *gbmrgb);
extern void gbm_ht_8_2x2(const GBM *gbm, const byte *src24, byte *dest4);
extern void gbm_ht_8_3x3(const GBM *gbm, const byte *src24, byte *dest4);
