#define  INCL_BASE
#include <os2.h>
#define  INCL_GRE_CLIP
#define  INCL_GRE_DCS
#define  INCL_GRE_STRINGS               /* include GRE strings        */
#include <pmddim.h>                     /* in Graphic engine include  */

extern PFN MyOld32CharStringPos;

APIRET APIENTRY My32CharStringPos(ULONG hdc, POINTL pptlStart, PRECTL pRectl, ULONG flOptions, LONG cChars, 
                                  PBYTE pchString, PLONG pAdx, PVOID pAttrs, ULONG hddc, ULONG ulFunN)
{

    /* . . . . . . . . . Freedom to do what you want here . . . . . . . . */
    /*                                  .                                 */
    /*                                  .                                 */
    /*                                  .                                 */
    /*                                  .                                 */
    /*                                  .                                 */
    /* just make sure to let the display driver finnish what was intended */
    /* for it.                                                            */


    return(MyOld32CharStringPos( hdc, pptlStart, pRectl, flOptions, cChars,
                                 pchString, pAdx, pAttrs, hddc, ulFunN));
}
