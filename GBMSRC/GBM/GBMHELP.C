/*

gbmhelp.c - Helpers for GBM file I/O stuff

*/

/*...sincludes:0:*/
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
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
#include "gbm.h"

/*...vgbm\46\h:0:*/
/*...e*/

/*...sgbm_same:0:*/
BOOLEAN gbm_same(const char *s1, const char *s2, int n)
	{
	for ( ; n--; s1++, s2++ )
		if ( tolower(*s1) != tolower(*s2) )
			return FALSE;
	return TRUE;
	}
/*...e*/
/*...sgbm_find_word:0:*/
const char *gbm_find_word(const char *str, const char *substr)
	{
	char buf[100+1], *s;
	int  len = strlen(substr);

	for ( s  = strtok(strcpy(buf, str), " \t,");
	      s != NULL;
	      s  = strtok(NULL, " \t,") )
		if ( gbm_same(s, substr, len) && s[len] == '\0' )
			return str + (s - buf);
	return NULL;
	}
/*...e*/
/*...sgbm_find_word_prefix:0:*/
const char *gbm_find_word_prefix(const char *str, const char *substr)
	{
	char buf[100+1], *s;
	int  len = strlen(substr);

	for ( s  = strtok(strcpy(buf, str), " \t,");
	      s != NULL;
	      s  = strtok(NULL, " \t,") )
		if ( gbm_same(s, substr, len) )
			return str + (s - buf);
	return NULL;
	}
/*...e*/
/*...sreading ahead:0:*/
#define	AHEAD_BUF 0x4000

typedef struct
	{
	byte buf[AHEAD_BUF];
	int inx, cnt;
	int fd;
	} AHEAD;

AHEAD *gbm_create_ahead(int fd)
	{
	AHEAD *ahead;

	if ( (ahead = malloc(sizeof(AHEAD))) == NULL )
		return NULL;

	ahead->inx = 0;
	ahead->cnt = 0;
	ahead->fd  = fd;

	return ahead;
	}

void gbm_destroy_ahead(AHEAD *ahead)
	{
	free(ahead);
	}	

int gbm_read_ahead(AHEAD *ahead)
	{
	if ( ahead->inx >= ahead->cnt )
		{
		ahead->cnt = read(ahead->fd, (char *) ahead->buf, AHEAD_BUF);
		if ( ahead->cnt <= 0 )
			return -1;
		ahead->inx = 0;
		}
	return (int) (unsigned int) ahead->buf[ahead->inx++];
	}
/*...e*/
