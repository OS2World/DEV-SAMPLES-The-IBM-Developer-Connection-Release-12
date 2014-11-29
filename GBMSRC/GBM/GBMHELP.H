/*

gbmhelp.h - Internal helpers for GBM file I/O stuff

*/

extern BOOLEAN gbm_same(const char *s1, const char *s2, int n);
extern const char *gbm_find_word(const char *str, const char *substr);
extern const char *gbm_find_word_prefix(const char *str, const char *substr);

typedef void AHEAD;
extern AHEAD *gbm_create_ahead(int fd);
extern void gbm_destroy_ahead(AHEAD *ahead);
extern int gbm_read_ahead(AHEAD *ahead);
