/*

gbmmir.h - Interface to Mirror Image of General Bitmap

*/

extern BOOLEAN gbm_ref_vert(const GBM *gbm, byte *data);
extern BOOLEAN gbm_ref_horz(const GBM *gbm, byte *data);
extern void gbm_transpose(const GBM *gbm, const byte *data, byte *data_t);
