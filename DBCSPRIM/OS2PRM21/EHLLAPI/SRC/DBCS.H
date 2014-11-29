/**********************************************************************/
/*                                                                    */
/* Program Name: DBCS.H                                               */
/*                                                                    */
/* Header file of DBCS subroutines.                                   */
/*                                                                    */
/**********************************************************************/

#define NON_DBCS_CP     0x0000
#define DBCS_CP         0x0001

#define SBCS            0x0000
#define DBCS_1ST        0x0001
#define DBCS_2ND        0x0002

USHORT InitDBCSTable( void );
USHORT isDBCS1st( UCHAR );
USHORT getCharType( UCHAR *, USHORT );
USHORT dbcsStrValidate( UCHAR *, USHORT *, USHORT );
