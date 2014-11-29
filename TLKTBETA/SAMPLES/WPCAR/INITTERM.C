/*static char *SCCSID = "@(#)initterm.c	6.1 91/11/03";*/
/*
 * DLL init and term routine for the Toronto Compiler
 *
 */

 #define  INCL_DOSPROCESS

 #include <stdlib.h>
 #include <stdio.h>
 #include <os2.h>

/*
 * _edcinit is the C-runtime environment init. function.
 * Returns 0 to indicate success and -1 to indicate failure
 */
  int _CRT_init(void);

/*
 * _edcterm is the C-runtime termination function.
 */
  int _CRT_term(unsigned long);

extern DLLInit( void );
extern DLLUninit( void );

/*
 * _DLL_InitTerm is the function that tets called by the operation system
 * loader when it lads and frees the dll for each process that accesses
 * the dll.
 */
#pragma linkage( _DLL_InitTerm, system )

unsigned long _DLL_InitTerm (unsigned long modhandle, unsigned long flag)
{
  /*
   * If flag is 0 the the DLL is being loaded so initialization should be
   * preformed. If flag is 1 then the DLL is being freed , so termination
   * should be preformed.
   */
   switch (flag)
     {
     case 0:
        /*
         * Call the C run-time init function before any thing else.
         */
         if (_CRT_init() == -1)
           return 0UL;
         DLLInit();

         break;

     case 1:
        /*
         * Call the C run-time termination function.
         */
         DLLUninit();
         _CRT_term(0UL);
         break;
     } /* endswitch */

  return 1UL;

}

APIRET DosGetThreadInfo(PTIB *pptib,PPIB *pppib)
{
     return(DosGetInfoBlocks(pptib,pppib) );
}
