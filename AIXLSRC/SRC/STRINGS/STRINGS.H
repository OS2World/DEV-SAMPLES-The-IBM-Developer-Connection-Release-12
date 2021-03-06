/* include file for strings.c */


#define MAXLINE 1024
#define YES 1
#define NO  0
#define BAILOUT -1
#define CR  0x0d
#define LF  0x0a
#define DEFAULT_MINLEN 4

int init(int argc, char **argv);
void search_the_file(char *filename);
void search_stdin(void);
void do_search(FILE *f);
void printit(char *line, unsigned long offset);
void tell_usage(void);
