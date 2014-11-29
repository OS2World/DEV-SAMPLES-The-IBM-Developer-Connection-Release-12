/*

gbmscale.h - Interface to scaling code

*/

extern GBM_ERR gbm_simple_scale(
	const byte *s, int sw, int sh,
	      byte *d, int dw, int dh,
	int bpp
	);
