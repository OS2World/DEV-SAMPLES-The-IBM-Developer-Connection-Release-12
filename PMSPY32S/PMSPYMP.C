/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                                          ³*/
/*³ PROGRAM NAME: PMSPY                                                      ³*/
/*³ -------------                                                            ³*/
/*³  A PM program that is used to look at or 'spy' on the message queue of   ³*/
/*³  other PM applications windows.                                          ³*/
/*³                                                                          ³*/
/*³ COPYRIGHT:                                                               ³*/
/*³ ----------                                                               ³*/
/*³  Copyright (C) International Business Machines Corp., 1992               ³*/
/*³                                                                          ³*/
/*³ DISCLAIMER OF WARRANTIES:                                                ³*/
/*³ -------------------------                                                ³*/
/*³  The following [enclosed] code is sample code created by IBM Corporation.³*/
/*³  This sample code is not part of any standard IBM product and is provided³*/
/*³  to you solely for the purpose of assisting you in the development of    ³*/
/*³  your applications.  The code is provided "AS IS", without warranty of   ³*/
/*³  any kind.  IBM shall not be liable for any damages arising out of your  ³*/
/*³  use of the sample code, even if they have been advised of the           ³*/
/*³  possibility of such damages.                                            ³*/
/*³                                                                          ³*/
/*³ For details on what this program does etc., please see the PMSPY.C file. ³*/
/*³                                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ PMSPYMP.C                                                                ³*/
/*³                                                                          ³*/
/*³ MP formatting code                                                       ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#include "pmspy.h"                      /* Resource symbolic identifiers      */
#include "pmspyMP.h"                    /* MP externals                       */

#include <ctype.h>

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ MP Formats                                                               ³*/
/*³                                                                          ³*/
/*³ - definitions to support all of the MP formats                           ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
typedef enum
{
  As_32,

  As_16_16,
  As_16_8_8,

  As_8_8_16,
  As_8_8_8_8,

  As_16_C_C,
  As_C_C_16,
  As_C_C_C_C

}  MP_METHODS;

typedef struct
{
  MP_FORMAT   mpThis;
  PSZ         pszThis;
  MP_METHODS  mpMethod;
  PSZ         pszFormat;

}  MPF_CONTROL,
 *PMPF_CONTROL;

static MPF_CONTROL asMP[] =
{
/*                                             Format
**  MP_FORMAT   Display Text                   Method      Format String
** ----------- ----------------------------   ----------  ------------------- */
 { MP_32H        ,"32 bit Hex"                  ,As_32      ,"0x%8.8lX"       },
 { MP_32P        ,"PVOID"                       ,As_32      ,"%p"             },
 { MP_32U        ,"ULONG"                       ,As_32      ,"%lu"            },
 { MP_32S        ,"LONG"                        ,As_32      ,"%ld"            },

 { MP_16H_16H    ,"HI(Hex) - LO(Hex)"           ,As_16_16   ,"%4.4X - %4.4X"  },
 { MP_16H_16S    ,"HI(Hex) - LO(SHORT)"         ,As_16_16   ,"%4.4X - %d"     },
 { MP_16H_16U    ,"HI(Hex) - LO(USHORT)"        ,As_16_16   ,"%4.4X - %u"     },

 { MP_16S_16H    ,"HI(SHORT) - LO(Hex)"         ,As_16_16   ,"%d - %4.4X"     },
 { MP_16S_16S    ,"HI(SHORT) - LO(SHORT)"       ,As_16_16   ,"%d - %d"        },
 { MP_16S_16U    ,"HI(SHORT) - LO(USHORT)"      ,As_16_16   ,"%d - %u"        },

 { MP_16U_16H    ,"HI(USHORT) - LO(Hex)"        ,As_16_16   ,"%u - %4.4X"     },
 { MP_16U_16S    ,"HI(USHORT) - LO(SHORT)"      ,As_16_16   ,"%u - %d"        },
 { MP_16U_16U    ,"HI(USHORT) - LO(USHORT)"     ,As_16_16   ,"%u - %u"        },

 { MP_16H_8H_8H  ,"HI(HEX) - LO(HEX/HEX)"       ,As_16_8_8  ,"%4.4X - %2.2X/%2.2X" },
 { MP_16H_8U_8U  ,"HI(HEX) - LO(UNS/UNS)"       ,As_16_8_8  ,"%4.4X - %u/%u"  },
 { MP_16H_8C_8C  ,"HI(HEX) - LO(CHAR/CHAR)"     ,As_16_C_C  ,"%4.4X - %s/%s"  },

 { MP_8H_8H_16H  ,"HI(HEX/HEX) - LO(HEX)"       ,As_8_8_16  ,"%2.2X/%2.2X - %4.4X" },
 { MP_8U_8U_16H  ,"HI(UNS/UNS) - LO(HEX)"       ,As_8_8_16  ,"%u/%u - %4.4X"  },
 { MP_8C_8C_16H  ,"HI(CHAR/CHAR) - LO(HEX)"     ,As_C_C_16  ,"%s/%s - %4.4X"  },

 { MP_8H_8H_8H_8H,"HI(HEX/HEX) - LO(HEX/HEX)"   ,As_8_8_8_8 ,"%2.2X/%2.2X - %2.2X/%2.2X" },
 { MP_8U_8U_8U_8U,"HI(UNS/UNS) - LO(UNS/UNS)"   ,As_8_8_8_8 ,"%u/%u - %u/%u"  },
 { MP_8C_8C_8C_8C,"HI(CHAR/CHAR) - LO(CHAR/CHAR)"
                                                ,As_C_C_C_C ,"%s/%s - %s/%s"  },

                 /* End-of-Table */
 { MP_EOT     ,"** Unknown MP value **"      ,As_32      ,"0x%8.8lX"          }
};

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ MpFind                                                                   ³*/
/*³                                                                          ³*/
/*³ - internal subroutine to 'find' specified MP format item                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static
PMPF_CONTROL
MpFind(MP_FORMAT mpThis)
{
  PMPF_CONTROL    pMPF;

  /**************************************************************************
  * Scan all MP format items for the specified drop-down list
  **************************************************************************/
  for( pMPF = &asMP[0];              /* Initialize: start @ first             */

       (pMPF->mpThis != MP_EOT) &&   /* While:      (1) not at EOT            */
       (pMPF->mpThis != mpThis);     /*             (2) MP item not found     */

       pMPF++                        /* Iterate:    try the next              */
     ) /*** Note: NULL "for" loop body ****/ ;

  /**************************************************************************
  * Return the result of our search...
  **************************************************************************/
  return(pMPF);
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ MpAsCharacter                                                            ³*/
/*³                                                                          ³*/
/*³ - format passed value as "Character" data                                ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static
PSZ
MpAsCharacter(CHAR chThis,
              PSZ  pszWhere)
{
  /**************************************************************************
  * Format character being sure to handle if not "printable"
  **************************************************************************/
  sprintf(pszWhere, "'%c'", isprint(chThis) ? chThis : ' ');

  /**************************************************************************
  * Return where formatted text went
  **************************************************************************/
  return(pszWhere);
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ MpFormat                                                                 ³*/
/*³                                                                          ³*/
/*³ - format specified MP value according to specified MP format             ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
PSZ
MpFormat(MPARAM    mpValue,
         MP_FORMAT mpFormat,
         PSZ       pszWhere)
{
  /**************************************************************************
  * Local data
  **************************************************************************/
  CHAR    szC1[32],
          szC2[32],
          szC3[32],
          szC4[32];

  /**************************************************************************
  * Locate control data for this MP format specification
  **************************************************************************/
  PMPF_CONTROL pMPF = MpFind(mpFormat);

  /**************************************************************************
  * Process based on MP format specified
  **************************************************************************/
  switch(pMPF->mpMethod)
  {
    /**************************************************************************
    * Format as one 32 bit value
    **************************************************************************/
    case As_32:

         sprintf(pszWhere,
                 pMPF->pszFormat,

                 mpValue);
    break;

    /**************************************************************************
    * Format as two 16 bit values
    **************************************************************************/
    case As_16_16:

         MpFormat(mpValue, MP_32H, pszWhere);

         strcat(pszWhere, ": ");

         sprintf( pszWhere + strlen(pszWhere),
                  pMPF->pszFormat,

                  SHORT2FROMMP(mpValue),

                  SHORT1FROMMP(mpValue)
                );
    break;

    /**************************************************************************
    * Format as 16 bit and two 8 bit values
    **************************************************************************/
    case As_16_8_8:

         MpFormat(mpValue, MP_32H, pszWhere);

         strcat(pszWhere, ": ");

         sprintf( pszWhere + strlen(pszWhere),
                  pMPF->pszFormat,

                  SHORT2FROMMP(mpValue),

                  CHAR2FROMMP(mpValue),
                  CHAR1FROMMP(mpValue)
                );
    break;

    /**************************************************************************
    * Format as 16 bit and two character values
    **************************************************************************/
    case As_16_C_C:

         MpFormat(mpValue, MP_32H, pszWhere);

         strcat(pszWhere, ": ");

         sprintf( pszWhere + strlen(pszWhere),
                  pMPF->pszFormat,

                  SHORT2FROMMP(mpValue),

                  MpAsCharacter( CHAR2FROMMP(mpValue), szC1 ),
                  MpAsCharacter( CHAR1FROMMP(mpValue), szC2 )
                );
    break;

    /**************************************************************************
    * Format as two 8 bit and 16 bit values
    **************************************************************************/
    case As_8_8_16:

         MpFormat(mpValue, MP_32H, pszWhere);

         strcat(pszWhere, ": ");

         sprintf( pszWhere + strlen(pszWhere),
                  pMPF->pszFormat,

                  CHAR4FROMMP(mpValue),
                  CHAR3FROMMP(mpValue),

                  SHORT1FROMMP(mpValue)
                );
    break;

    /**************************************************************************
    * Format as two character and 16 bit values
    **************************************************************************/
    case As_C_C_16:

         MpFormat(mpValue, MP_32H, pszWhere);

         strcat(pszWhere, ": ");

         sprintf( pszWhere + strlen(pszWhere),
                  pMPF->pszFormat,

                  MpAsCharacter( CHAR4FROMMP(mpValue), szC1 ),
                  MpAsCharacter( CHAR3FROMMP(mpValue), szC2 ),

                  SHORT1FROMMP(mpValue)
                );
    break;

    /**************************************************************************
    * Format as four 8 bit values
    **************************************************************************/
    case As_8_8_8_8:

         MpFormat(mpValue, MP_32H, pszWhere);

         strcat(pszWhere, ": ");

         sprintf( pszWhere + strlen(pszWhere),
                  pMPF->pszFormat,

                  CHAR4FROMMP(mpValue),
                  CHAR3FROMMP(mpValue),

                  CHAR2FROMMP(mpValue),
                  CHAR1FROMMP(mpValue)
                );
    break;

    /**************************************************************************
    * Format as four character values
    **************************************************************************/
    case As_C_C_C_C:

         MpFormat(mpValue, MP_32H, pszWhere);

         strcat(pszWhere, ": ");

         sprintf( pszWhere + strlen(pszWhere),
                  pMPF->pszFormat,

                  MpAsCharacter( CHAR4FROMMP(mpValue), szC1 ),
                  MpAsCharacter( CHAR3FROMMP(mpValue), szC2 ),

                  MpAsCharacter( CHAR2FROMMP(mpValue), szC3 ),
                  MpAsCharacter( CHAR1FROMMP(mpValue), szC4 )
                );
    break;
  }

  /**************************************************************************
  * Return where formatted MP text went
  **************************************************************************/
  return(pszWhere);
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ MpDisplayText                                                            ³*/
/*³                                                                          ³*/
/*³ - return display text for specific MP format                             ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
PSZ
MpDisplayText(MP_FORMAT mpFormat)
{
  /**************************************************************************
  * Locate control data for this MP format specification
  **************************************************************************/
  PMPF_CONTROL pMPF = MpFind(mpFormat);

  /**************************************************************************
  * Return PTR to displayable text for this MP format
  **************************************************************************/
  return(pMPF->pszThis);
}
