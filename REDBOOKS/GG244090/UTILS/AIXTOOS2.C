/* usage =  AIXTOOS2 <filein >fileout  */

#include "stdio.h"


main(int argc, char *argv[], char *envp[])
{
  int c;

#ifdef IBMOS2
  freopen("","rb",stdin);
  freopen("","wb",stdout);
#endif
 
  while( (c = fgetc(stdin)) != EOF)  {
    if ( c == '\n' ) {
       fputc('\r',stdout);
    } /* endif */
    fputc(c,stdout);
  } /* endwhile */
  fputc(0x1A,stdout);

}

