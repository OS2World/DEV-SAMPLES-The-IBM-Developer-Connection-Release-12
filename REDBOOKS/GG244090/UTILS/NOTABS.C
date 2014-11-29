/* usage =  NOTABS <filein >fileout  */

#include "stdio.h"

#define TS 8
int col = 1;     

main(int argc, char *argv[], char *envp[])
{
  int c;
 
#ifdef IBMOS2
  freopen("","rb",stdin);
  freopen("","wb",stdout);
#endif

  while( (c = fgetc(stdin)) != EOF)  {
    tabs(c);
  } /* endwhile */
}



int tabs(int ch)        
{
   switch (ch) {
   case '\t' :      
      do {
        fputc(' ',stdout);
        ++col;
      } while( (col % TS) != 1);
      break;

   case '\r' :          
   case '\n' :          
      fputc(ch,stdout);
      col = 1;
      break;

   default :
      fputc(ch,stdout);
      ++col;
      break;
   } /* endswitch */

}
