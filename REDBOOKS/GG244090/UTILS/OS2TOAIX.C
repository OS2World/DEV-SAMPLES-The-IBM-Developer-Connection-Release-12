/* usage =  OS2TOAIX <filein >fileout  */

#include "stdio.h"


main(int argc, char *argv[], char *envp[])
{
  int c;

#ifdef IBMOS2
  freopen("","rb",stdin);
  freopen("","wb",stdout);
#endif
 
  while( (c = fgetc(stdin)) != EOF)  {
    if ( (c != '\r') && (c != 0x1A) ) {
       fputc(c,stdout);
    } /* endif */
  } /* endwhile */
}

