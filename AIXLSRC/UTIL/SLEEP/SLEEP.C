/* sleep(n) - suspends calling thread for n seconds */

#define INCL_DOSPROCESS
#include <os2.h>

/* Temporary mapping */

int sleep(unsigned int seconds)
{
   DosSleep((unsigned long)seconds * 1000);
   return(0);
}
