/************   OS/2 Application Primer Sample   **********************/
/*                                                                    */
/* Module Name : DBCS.H                                               */
/* Version : 1.0 (No change for OS/2 V2.x)                            */
/*                                                                    */
/* A header file for DBCS subroutines.                                */
/*                                                                    */
/**********************************************************************/

#define NON_DBCS_CP     0x0000
#define DBCS_CP         0x0001

#define SBCS            0x0000
#define DBCS_1ST        0x0001
#define DBCS_2ND        0x0002

extern USHORT InitDBCSTable( void );
extern USHORT isDBCS1st( UCHAR );
extern USHORT getCharType( UCHAR *, USHORT );
extern USHORT dbcsStrValidate( UCHAR *, USHORT *, USHORT );
